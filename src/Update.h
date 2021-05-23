#pragma once
#include <semver.h>
#include <lvgl.h>
#include <json.hpp>

#include "Downloader.h"
#include "Update.h"
#include "Version.h"
#include "lvgl-extra/ButtonGroup.h"

class Update {
    public:
        Update();
        virtual ~Update();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);

        enum STATE {
            UPDATE_LOAD_INIT,
            UPDATE_LOAD_CONFIG,
            UPDATE_LOAD_CONFIG_ERROR,
            UPDATE_CREATE_TEMP_FOLDER,
            UPDATE_CHECK_LATEST_VERSION,
            UPDATE_CHECK_LATEST_VERSION_DOWNLOADING,
            UPDATE_CHECK_LATEST_VERSION_COMPARE,
            UPDATE_CHECK_LATEST_VERSION_ERROR,
            UPDATE_NEW_VERSION_FOUND,
            UPDATE_DOWNLOAD_FILES,
            UPDATE_DOWNLOAD_FILES_DOWNLOADING,
            UPDATE_DOWNLOAD_FILES_CRC,
            UPDATE_DOWNLOAD_FILES_NEXT,
            UPDATE_LATEST_INSTALLED,
            UPDATE_DOWNLOAD_FAILED,
            UPDATE_DOWNLOAD_COMPLETE,
            UPDATE_INSTALL_COMPLETE,
            UPDATE_ERROR,
            UPDATE_DONE
        };

        void check();
        void StateTask(lv_task_t * task);
        uint32_t calcCRC32(const char *file);

        STATE current_state = UPDATE_LOAD_INIT;
    private:
        nlohmann::json config;

        lv_obj_t *status_text;
        lv_obj_t *progress_main;

        //
        lv_task_t *task;

        //
        semver_t latest_version  = { 1, 0, 0 };

        //
        bool update_successful = false;

        //
        Downloader *cDownloader;

        //
        nlohmann::json latest_release;

        // Keep track of which file from the JSON we're downloading
        uint32_t current_files_index = 0;

        // Currrent file being downloaded
        nlohmann::json current_file;
        std::string current_file_download;
        std::string current_file_path;
        std::string current_file_path_short;
        uint32_t    current_file_crc32;
};
