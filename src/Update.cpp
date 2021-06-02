#include <stdio.h>
#include <string.h>
#include <fstream>
#include <windows.h>
#include <zlib.h>
#include <semver.h>
#include <lvgl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "Downloader.h"
#include "Network.h"
#include "Update.h"

static void StateTaskHandler(lv_task_t * task) {
    Update* scene = static_cast<Update *>(task->user_data);
    scene->StateTask(task);
}

Update::Update()
{
    //
    if(!network_init)
        init_networking();
}

Update::~Update(void)
{
    // TODO: Shouldn't have to worry about this for now since the system is forced to restart at the end
}

void Update::check()
{
    status_text = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text(status_text, "Loading config file");

    task = lv_task_create(StateTaskHandler, 1000, LV_TASK_PRIO_MID, static_cast<lv_obj_user_data_t>(this));
}

void Update::StateTask(lv_task_t * task) {
    uint8_t progress = 0;
    std::string version;

    switch(current_state) {
    case UPDATE_LOAD_INIT:
        current_state = UPDATE_LOAD_CONFIG;
    case UPDATE_LOAD_CONFIG:
        lv_msgbox_set_text(status_text, "Loading config file");
        current_state = UPDATE_CREATE_TEMP_FOLDER;
        break;
    case UPDATE_LOAD_CONFIG_ERROR:
        lv_msgbox_set_text(status_text, "Invalid config file");
        current_state = UPDATE_CREATE_TEMP_FOLDER;
        break;
    case UPDATE_CREATE_TEMP_FOLDER:
        lv_msgbox_set_text(status_text, "Creating temp folder");

        // Create the xboxhd directory just incase we're running from the DVD drive.
        CreateDirectory("C:\\xboxhd", NULL);

        // Create a temp directory for the latest release
        CreateDirectory("C:\\xboxhd\\temp", NULL);

        current_state = UPDATE_CHECK_LATEST_VERSION;
        break;
    case UPDATE_CHECK_LATEST_VERSION:
        lv_msgbox_set_text(status_text, "Downloading latest release information");
        cDownloader = new Downloader("storage.googleapis.com", 80, "/fw.xboxhdplus.com/latest.json", "C:\\xboxhd\\temp\\latest.json");

        current_state = UPDATE_CHECK_LATEST_VERSION_DOWNLOADING;
        break;
    case UPDATE_CHECK_LATEST_VERSION_DOWNLOADING:
        progress = cDownloader->getProgress();

        if(progress == 100) {
            delete cDownloader;

            current_state = (cDownloader->getStatus() == 200) ?
                UPDATE_CHECK_LATEST_VERSION_COMPARE : UPDATE_DOWNLOAD_FAILED;
        }
        break;
    case UPDATE_CHECK_LATEST_VERSION_COMPARE:
        lv_msgbox_set_text(status_text, "Checking if latest releases is newer");

        std::ifstream("C:\\xboxhd\\temp\\latest.json") >> latest_release;

        //
        semver_parse(latest_release["version"].get<std::string>().c_str(), &latest_version);

        current_state = (semver_satisfies(latest_version, current_version, ">")) ?
            UPDATE_NEW_VERSION_FOUND : UPDATE_LATEST_INSTALLED;
        break;
    case UPDATE_CHECK_LATEST_VERSION_ERROR:
        lv_msgbox_set_text(status_text, "Update check failed!");
        break;
    case UPDATE_NEW_VERSION_FOUND:
        lv_msgbox_set_text_fmt(status_text, "New software update found! v%d.%d.%d",
            latest_version.major, latest_version.minor, latest_version.patch);

        current_files_index = 0;
        current_state = UPDATE_DOWNLOAD_FILES;
        break;
    case UPDATE_DOWNLOAD_FILES:
        current_file = latest_release["files"].at(current_files_index);

        //
        current_file_download   = current_file["download"].get<std::string>();
        current_file_crc32      = std::stoi(current_file["crc32"].get<std::string>(), 0, 16);
        current_file_path_short = current_file["path"].get<std::string>();
        current_file_path       = std::string("C:\\xboxhd\\temp\\") + current_file_path_short;

        lv_msgbox_set_text_fmt(status_text, "Downloading file... \"%s\"", current_file_path_short.c_str());

        cDownloader = new Downloader("storage.googleapis.com", 80,
            current_file_download.c_str(), current_file_path.c_str());

        current_state = UPDATE_DOWNLOAD_FILES_DOWNLOADING;
        break;
    case UPDATE_DOWNLOAD_FILES_DOWNLOADING:
        progress = cDownloader->getProgress();

        if(progress == 100) {
            current_state = (cDownloader->getStatus() == 200) ? UPDATE_DOWNLOAD_FILES_CRC : UPDATE_DOWNLOAD_FAILED;

            // Cleanup
            delete cDownloader;
        }
        break;
    case UPDATE_DOWNLOAD_FILES_CRC:
        lv_msgbox_set_text_fmt(status_text, "Checking CRC... \"%s\"", current_file_path_short.c_str());
        current_state = (current_file_crc32 == calcCRC32(current_file_path.c_str())) ?
            UPDATE_DOWNLOAD_FILES_NEXT : UPDATE_DOWNLOAD_FAILED;
        break;
    case UPDATE_DOWNLOAD_FILES_NEXT:
        if(current_files_index < latest_release["files"].size() - 1) {
            current_files_index++;
            current_state = UPDATE_DOWNLOAD_FILES;
        } else {
            current_state = UPDATE_DOWNLOAD_COMPLETE;
        }
        break;
    case UPDATE_LATEST_INSTALLED:
        lv_msgbox_set_text(status_text, "No new update found.");
        current_state = UPDATE_DONE;
        break;
    case UPDATE_DOWNLOAD_FAILED:
        lv_msgbox_set_text(status_text, "Download failed!");
        current_state = UPDATE_DONE;
        break;
    case UPDATE_DOWNLOAD_COMPLETE:
        lv_msgbox_set_text(status_text, "Download complete!");
        //
        for(auto &elem : latest_release["files"]) {
            DeleteFileA(
                (std::string("C:\\xboxhd\\") + elem["path"].get<std::string>()).c_str()
            );
            MoveFileA(
                (std::string("C:\\xboxhd\\temp\\") + elem["path"].get<std::string>()).c_str(),
                (std::string("C:\\xboxhd\\") + elem["path"].get<std::string>()).c_str()
            );
        }

        // Cleanup
        DeleteFileA("C:\\xboxhd\\temp\\latest.json");
        RemoveDirectoryA("C:\\xboxhd\\temp");

        // 'Clear' EEPROM
        DeleteFileA("C:\\xboxhd\\eeprom.bin");

        current_state = UPDATE_INSTALL_COMPLETE;
        break;
    case UPDATE_INSTALL_COMPLETE:
        lv_msgbox_set_text(status_text, "Update has successuly completed!\nPress B to restart app.");
        update_successful = true;
        current_state = UPDATE_DONE;
        break;
    case UPDATE_ERROR:
        lv_msgbox_set_text(status_text, "An unexpected error has occured!\nPress B to restart app.");
        current_state = UPDATE_DONE;
        break;
    case UPDATE_DONE:
        break;
    }
}

extern "C" { unsigned long crc32(uLong crc, const Bytef *buf, uInt len); }
uint32_t Update::calcCRC32(const char *file) {
    uint32_t crc = crc32(0L, Z_NULL, 0);
    char buffer;

    std::ifstream is(file, std::ifstream::binary);
    while(is.read(&buffer, 1)) {
        crc = crc32(crc, (const Bytef *)&buffer, 1);
    }

    return crc;
}
