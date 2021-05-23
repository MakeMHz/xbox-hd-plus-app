#pragma once
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <lwip/apps/http_client.h>

class Downloader {
    public:
        Downloader(const char *server_name, uint16_t port, const char *uri, const char *dest);
        virtual ~Downloader() { };

        uint8_t  getProgress();
        uint16_t getStatus();

        // Callbacks for lwip
        void result_fn(httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err);
        err_t headers_done_fn(httpc_state_t *connection, struct pbuf *hdr, u16_t hdr_len, u32_t content_len);
    private:
        char *outPath;
        uint8_t progress = 0;

        // Variables related to lwip
        httpc_state_t *connection;
        httpc_connection_t settings;
        err_t error;
};
