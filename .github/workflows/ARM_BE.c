# This starter workflow is for a CMake project running on multiple platforms. There is a different starter workflow if you just want a single platform.
# See: https://github.com/actions/starter-workflows/blob/main/ci/cmake-single-platform.yml
name: CMake on multiple platforms

on: [alias]
build-man = "run --package xtask-build-man --"
stale-label = "run --package xtask-stale-label --"
bump-check = "run --package xtask-bump-check --"
lint-docs = "run --package xtask-lint-docs --"

[env]
# HACK: Until this is stabilized, `snapbox`s polyfill could get confused
# inside of the rust-lang/rust repo because it looks for the furthest-away `Cargo.toml`
CARGO_RUSTC_CURRENT_DIR = { value = "", relative = true }
  push: idf_component_register(SRCS main.c db_esp32_control.c globals.h sdkconfig.h msp_ltm_serial.c
        msp_ltm_serial.h db_protocol.h http_server.c http_server.h db_esp32_comm.c
        db_esp32_comm.h db_comm_protocol.h db_comm.c db_comm.h db_crc.c db_crc.h tcp_server.c tcp_server.h
        INCLUDE_DIRS ".")
    branches: [ "main" ]
  pull_request: #
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

    branches: [ "main" ]

jobs: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
#include <cJSON.h>
#include <stdint.h>
#include <string.h>
#include "db_comm_protocol.h"
#include "db_protocol.h"
#include "db_comm.h"


uint32_t calc_crc32(uint32_t crc, unsigned char *buf, size_t len) {
    int k;
    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ 0xedb88320 : crc >> 1;
    }
    return ~crc;
}

/**
 * @brief Check CRC32 of a message
 * @param buf Buffer containing compete message (JSON+CRC)
 * @param msg_length Length of entire message (JSON+CRC)
 * @return 1 if CRC is good else 0
 */
int crc_ok(uint8_t *buf, int msg_length) {
    uint32_t c_crc = calc_crc32((uint32_t) 0, buf, (size_t) (msg_length - 4));
    uint8_t crc_bytes[4];
    crc_bytes[0] = c_crc;
    crc_bytes[1] = c_crc >> 8;
    crc_bytes[2] = c_crc >> 16;
    crc_bytes[3] = c_crc >> 24;
    if ((crc_bytes[0] == buf[msg_length - 4]) && (crc_bytes[1] == buf[msg_length - 3]) &&
        (crc_bytes[2] == buf[msg_length - 2]) && (crc_bytes[3] == buf[msg_length - 1])) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief Add crc32 at end and build final array
 * @return Length of response
 */
int finalize_message(uint8_t *msg_buf, char *req_json) {
    size_t json_length = strlen(req_json);
    uint32_t new_crc = calc_crc32((uint32_t) 0, (unsigned char *) req_json, strlen(req_json));
    msg_buf[json_length] = new_crc;
    msg_buf[json_length + 1] = new_crc >> 8;
    msg_buf[json_length + 2] = new_crc >> 16;
    msg_buf[json_length + 3] = new_crc >> 24;
    memcpy(msg_buf, req_json, json_length);
    return (int) json_length + 4;
}


/**
 * @brief Generate a system response message
 *
 * @param message_buffer Buffer where generated message will be placed
 * @param id Communication message ID to respond to
 * @param new_fw_id Firmware ID to respond with
 * @return Length of response
 */
int gen_db_comm_sys_ident_json(uint8_t *message_buffer, int id, int new_fw_id) {
    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, DB_COMM_KEY_DEST, DB_COMM_DST_GCS);
    cJSON_AddStringToObject(root, DB_COMM_KEY_TYPE, DB_COMM_TYPE_SYS_IDENT_RESPONSE);
    cJSON_AddStringToObject(root, DB_COMM_KEY_ORIGIN, DB_COMM_ORIGIN_GND);
    cJSON_AddNumberToObject(root, DB_COMM_KEY_HARDWID, DB_SYS_HID_ESP32);
    cJSON_AddNumberToObject(root, DB_COMM_KEY_FIRMWID, new_fw_id);
    cJSON_AddNumberToObject(root, DB_COMM_KEY_ID, id);
    return finalize_message(message_buffer, cJSON_Print(root));
}


int gen_db_comm_ping_resp(uint8_t *message_buffer, int id) {
    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, DB_COMM_KEY_DEST, DB_COMM_DST_GCS);
    cJSON_AddStringToObject(root, DB_COMM_KEY_TYPE, DB_COMM_TYPE_PING_RESPONSE);
    cJSON_AddStringToObject(root, DB_COMM_KEY_ORIGIN, DB_COMM_ORIGIN_GND);
    cJSON_AddNumberToObject(root, DB_COMM_KEY_ID, id);
    return finalize_message(message_buffer, cJSON_Print(root));
}


/**
 * @brief Generate error response
 *
 * @param message_buffer Buffer where generated message will be placed
 * @param id Communication message ID to respond to
 * @param error_message The error message
 * @return Length of response
 */
int gen_db_comm_err_resp(uint8_t *message_buffer, int id, char error_message[MAX_ERR_MSG_LENGTH]) {
    cJSON *root;
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, DB_COMM_KEY_DEST, DB_COMM_DST_GCS);
    cJSON_AddStringToObject(root, DB_COMM_KEY_TYPE, DB_COMM_TYPE_ERROR);
    cJSON_AddStringToObject(root, DB_COMM_KEY_ORIGIN, DB_COMM_ORIGIN_GND);
    cJSON_AddStringToObject(root, DB_COMM_KEY_MSG, error_message);
    cJSON_AddNumberToObject(root, DB_COMM_KEY_ID, id);
    return finalize_message(message_buffer, cJSON_Print(root));
}
  build: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#ifndef DB_ESP32_DB_COMM_H
#define DB_ESP32_DB_COMM_H

#define MAX_ERR_MSG_LENGTH 2048

int crc_ok(uint8_t *buf, int msg_length);

int gen_db_comm_sys_ident_json(uint8_t *message_buffer, int new_id, int new_fw_id);

int gen_db_comm_err_resp(uint8_t *message_buffer, int id, char error_message[MAX_ERR_MSG_LENGTH]);

int gen_db_comm_ping_resp(uint8_t *message_buffer, int id);

#endif //DB_ESP32_DB_COMM_H
    runs-on: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#ifndef DB_ESP32_DB_COMM_PROTOCOL_H
#define DB_ESP32_DB_COMM_PROTOCOL_H


#define DB_COMM_ORIGIN_GND "groundstation"
#define DB_COMM_ORIGIN_UAV "drone"
#define DB_COMM_TYPE_SETTINGS_SUCCESS "settingssuccess"
#define DB_COMM_TYPE_ERROR "error"
#define DB_COMM_TYPE_PING_REQUEST "pingrequest"
#define DB_COMM_TYPE_PING_RESPONSE "pingresponse"
#define DB_COMM_TYPE_SYS_IDENT_REQUEST "system_ident_req"
#define DB_COMM_TYPE_SYS_IDENT_RESPONSE "system_ident_rsp"
#define DB_COMM_TYPE_SETTINGS_CHANGE "settingschange"
#define DB_COMM_TYPE_CAMSELECT "camselect"
#define DB_COMM_TYPE_ADJUSTRC "adjustrc"
#define DB_COMM_TYPE_MSP "mspcommand"
#define DB_COMM_TYPE_ACK "ack"
#define DB_COMM_TYPE_SETTINGS_REQUEST "settingsrequest"
#define DB_COMM_TYPE_SETTINGS_RESPONSE "settingsresponse"
#define DB_COMM_REQUEST_TYPE_WBC "wbc"
#define DB_COMM_REQUEST_TYPE_DB "db"

#define DB_COMM_DST_GND 1
#define DB_COMM_DST_GND_UAV 2
#define DB_COMM_DST_PER 3
#define DB_COMM_DST_GCS 4
#define DB_COMM_DST_UAV 5


#define DB_COMM_KEY_TYPE "type"
#define DB_COMM_KEY_ORIGIN "origin"
#define DB_COMM_KEY_ID "id"
#define DB_COMM_KEY_DEST "destination"
#define DB_COMM_KEY_HARDWID "HID"   // Hardware ID
#define DB_COMM_KEY_FIRMWID "FID"   // Firmware version
#define DB_COMM_KEY_MSG "message"

#define DB_COMM_CHANGE_DB "db"
#define DB_COMM_CHANGE_DBESP32 "dbesp32"

#define DB_ESP32_FID 101

#endif //DB_ESP32_DB_COMM_PROTOCOL_H

    strategy: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2017 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include "db_crc.h"

/**
 * This function is part of Cleanflight & iNAV.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */
uint8_t crc8_dvb_s2(uint8_t crc, unsigned char a)
{
    crc ^= a;
    for (int ii = 0; ii < 8; ++ii) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0xD5;
        } else {
            crc = crc << 1;
        }
    }
    return crc;
}

/**
 * Does the same as crc8_dvb_s2 just way more efficient (CPU) but with a little bit more RAM usage
 * @param crc The current crc value
 * @param a The next byte
 * @return The new crc value
 */
uint8_t crc8_dvb_s2_table(uint8_t crc, unsigned char a)
{
    return (uint8_t) (crc_dvb_s2_table[(crc ^ a)] & 0xff);
}
      # Set fail-fast to false to ensure that feedback is delivered for all matrix combinations. Consider changing this to true when your workflow is stable.
      fail-fast: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2017 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

/**
 * \file
 * Functions and types for CRC checks.
 *
 * Generated on Sat Dec  9 16:03:14 2017
 * by pycrc v0.9.1, https://pycrc.org
 * using the configuration:
 *  - Width         = 8
 *  - Poly          = 0xa6
 *  - XorIn         = 0x00
 *  - ReflectIn     = False
 *  - XorOut        = 0x00
 *  - ReflectOut    = False
 *  - Algorithm     = table-driven
 *
 * This file defines the functions crc_init(), crc_update() and crc_finalize().
 *
 * The crc_init() function returns the inital \c crc value and must be called
 * before the first call to crc_update().
 * Similarly, the crc_finalize() function must be called after the last call
 * to crc_update(), before the \c crc is being used.
 * is being used.
 *
 * The crc_update() function can be called any number of times (including zero
 * times) in between the crc_init() and crc_finalize() calls.
 *
 * This pseudo-code shows an example usage of the API:
 * \code{.c}
 * crc_t crc;
 * unsigned char data[MAX_DATA_LEN];
 * size_t data_len;
 *
 * crc = crc_init();
 * while ((data_len = read_data(data, MAX_DATA_LEN)) > 0) {
 *     crc = crc_update(crc, data, data_len);
 * }
 * crc = crc_finalize(crc);
 * \endcode
 */
#ifndef CRC_H
#define CRC_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The definition of the used algorithm.
 *
 * This is not used anywhere in the generated code, but it may be used by the
 * application code to call algorithm-specific code, if desired.
 */
#define CRC_ALGO_TABLE_DRIVEN 1


/**
 * The type of the CRC values.
 *
 * This type must be big enough to contain at least 8 bits.
 */
typedef uint_fast8_t crc_t;

/**
 * Static table used for the table_driven implementation of crc_dvb_s2
 *  - Width         = 8
 *  - Poly          = 0xd5
 *  - XorIn         = 0x00
 *  - ReflectIn     = False
 *  - XorOut        = 0x00
 *  - ReflectOut    = False
 */
static const crc_t crc_dvb_s2_table[256] = {
        0x00, 0xd5, 0x7f, 0xaa, 0xfe, 0x2b, 0x81, 0x54, 0x29, 0xfc, 0x56, 0x83, 0xd7, 0x02, 0xa8, 0x7d,
        0x52, 0x87, 0x2d, 0xf8, 0xac, 0x79, 0xd3, 0x06, 0x7b, 0xae, 0x04, 0xd1, 0x85, 0x50, 0xfa, 0x2f,
        0xa4, 0x71, 0xdb, 0x0e, 0x5a, 0x8f, 0x25, 0xf0, 0x8d, 0x58, 0xf2, 0x27, 0x73, 0xa6, 0x0c, 0xd9,
        0xf6, 0x23, 0x89, 0x5c, 0x08, 0xdd, 0x77, 0xa2, 0xdf, 0x0a, 0xa0, 0x75, 0x21, 0xf4, 0x5e, 0x8b,
        0x9d, 0x48, 0xe2, 0x37, 0x63, 0xb6, 0x1c, 0xc9, 0xb4, 0x61, 0xcb, 0x1e, 0x4a, 0x9f, 0x35, 0xe0,
        0xcf, 0x1a, 0xb0, 0x65, 0x31, 0xe4, 0x4e, 0x9b, 0xe6, 0x33, 0x99, 0x4c, 0x18, 0xcd, 0x67, 0xb2,
        0x39, 0xec, 0x46, 0x93, 0xc7, 0x12, 0xb8, 0x6d, 0x10, 0xc5, 0x6f, 0xba, 0xee, 0x3b, 0x91, 0x44,
        0x6b, 0xbe, 0x14, 0xc1, 0x95, 0x40, 0xea, 0x3f, 0x42, 0x97, 0x3d, 0xe8, 0xbc, 0x69, 0xc3, 0x16,
        0xef, 0x3a, 0x90, 0x45, 0x11, 0xc4, 0x6e, 0xbb, 0xc6, 0x13, 0xb9, 0x6c, 0x38, 0xed, 0x47, 0x92,
        0xbd, 0x68, 0xc2, 0x17, 0x43, 0x96, 0x3c, 0xe9, 0x94, 0x41, 0xeb, 0x3e, 0x6a, 0xbf, 0x15, 0xc0,
        0x4b, 0x9e, 0x34, 0xe1, 0xb5, 0x60, 0xca, 0x1f, 0x62, 0xb7, 0x1d, 0xc8, 0x9c, 0x49, 0xe3, 0x36,
        0x19, 0xcc, 0x66, 0xb3, 0xe7, 0x32, 0x98, 0x4d, 0x30, 0xe5, 0x4f, 0x9a, 0xce, 0x1b, 0xb1, 0x64,
        0x72, 0xa7, 0x0d, 0xd8, 0x8c, 0x59, 0xf3, 0x26, 0x5b, 0x8e, 0x24, 0xf1, 0xa5, 0x70, 0xda, 0x0f,
        0x20, 0xf5, 0x5f, 0x8a, 0xde, 0x0b, 0xa1, 0x74, 0x09, 0xdc, 0x76, 0xa3, 0xf7, 0x22, 0x88, 0x5d,
        0xd6, 0x03, 0xa9, 0x7c, 0x28, 0xfd, 0x57, 0x82, 0xff, 0x2a, 0x80, 0x55, 0x01, 0xd4, 0x7e, 0xab,
        0x84, 0x51, 0xfb, 0x2e, 0x7a, 0xaf, 0x05, 0xd0, 0xad, 0x78, 0xd2, 0x07, 0x53, 0x86, 0x2c, 0xf9
};

/**
 * Calculate the initial crc value.
 *
 * \return     The initial crc value.
 */
static inline crc_t crc_init(void)
{
    return 0x00;
}


/**
 * Update the crc value with new data.
 *
 * \param[in] crc      The current crc value.
 * \param[in] data     Pointer to a buffer of \a data_len bytes.
 * \param[in] data_len Number of bytes in the \a data buffer.
 * \return             The updated crc value.
 */
crc_t crc_update(crc_t crc, const void *data, size_t data_len);


/**
 * Calculate the final crc value.
 *
 * \param[in] crc  The current crc value.
 * \return     The final crc value.
 */
static inline crc_t crc_finalize(crc_t crc)
{
    return crc;
}

uint8_t crc8_dvb_s2(uint8_t crc, unsigned char a);
uint8_t crc8_dvb_s2_table(uint8_t crc, unsigned char a);

#ifdef __cplusplus
}           /* closing brace for extern "C" */
#endif

#endif      /* CRC_H */

      # Set up a matrix to run the following 3 configurations:
      # 1. <Windows, Release, latest MSVC compiler toolchain on the default runner image, default generator>
      # 2. <Linux, Release, latest GCC compiler toolchain on the default runner image, default generator>
      # 3. <Linux, Release, latest Clang compiler toolchain on the default runner image, default generator>
      #
      # To add more build types (Release, Debug, RelWithDebInfo, etc.) customize the build_type list.
      matrix: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include <esp_log.h>
#include <string.h>
#include <cJSON.h>
#include "lwip/sockets.h"
#include "globals.h"
#include "db_protocol.h"
#include "db_comm_protocol.h"
#include "db_comm.h"
#include "tcp_server.h"


#define TCP_COMM_BUF_SIZE 4096
#define TAG "COMM"

uint8_t tcp_comm_buffer[TCP_COMM_BUF_SIZE];
uint8_t comm_resp_buf[TCP_COMM_BUF_SIZE];

void parse_comm_protocol(int client_socket, char *new_json_bytes) {
    cJSON *json_pointer = cJSON_Parse(new_json_bytes);
    int dest = cJSON_GetObjectItem(json_pointer, DB_COMM_KEY_DEST)->valueint;
    if (dest == DB_COMM_DST_GND) {
        int resp_length = 0;

        cJSON *j_type = cJSON_GetObjectItem(json_pointer, DB_COMM_KEY_TYPE);
        char type[strlen(j_type->valuestring)];
        strcpy(type, j_type->valuestring);
        int id = cJSON_GetObjectItem(json_pointer, DB_COMM_KEY_ID)->valueint;

        if (strcmp(type, DB_COMM_TYPE_SYS_IDENT_REQUEST) == 0) {
            ESP_LOGI(TAG, "Generating SYS_IDENT_RESPONSE");
            resp_length = gen_db_comm_sys_ident_json(comm_resp_buf, id, DB_ESP32_FID);
        } else if (strcmp(type, DB_COMM_TYPE_PING_REQUEST) == 0) {
            resp_length = gen_db_comm_ping_resp(comm_resp_buf, id);
        } else {
            resp_length = gen_db_comm_err_resp(comm_resp_buf, id, "Command not supported by DB for ESP32");
        }
        if (resp_length > 0)
            lwip_send(client_socket, comm_resp_buf, resp_length, 0);
    } else {
        ESP_LOGI(TAG, "Message not for us (%i)", DB_COMM_DST_GND);
        int id = cJSON_GetObjectItem(json_pointer, DB_COMM_KEY_ID)->valueint;
        int resp_length = gen_db_comm_err_resp(comm_resp_buf, id, "Command not supported by DB for ESP32");
        lwip_send(client_socket, comm_resp_buf, resp_length, 0);
    }
    cJSON_Delete(json_pointer);
}

void communication_module_server(void *parameters) {
    int tcp_master_socket = open_tcp_server(APP_PORT_COMM);
    ESP_LOGI(TAG, "Started communication module");
    while (1) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        memset(tcp_comm_buffer, 0, TCP_COMM_BUF_SIZE);
        struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
        uint addr_len = sizeof(source_addr);
        int new_tcp_client = accept(tcp_master_socket, (struct sockaddr *) &source_addr, &addr_len);
        if (new_tcp_client < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: %s", esp_err_to_name(errno));
            return;
        }
        ssize_t received_from_client;
        do {
            received_from_client = lwip_recv(new_tcp_client, tcp_comm_buffer, TCP_COMM_BUF_SIZE, 0);
            if (received_from_client > 0) {
                if (crc_ok(tcp_comm_buffer, received_from_client)) {
                    uint8_t json_byte_buf[(received_from_client - 4)];
                    memcpy(json_byte_buf, tcp_comm_buffer, (size_t) (received_from_client - 4));
                    parse_comm_protocol(new_tcp_client, (char *) tcp_comm_buffer);
                } else {
                    ESP_LOGE(TAG, "Bad CRC!");
                    uint8_t rsp_buffer[4096];
                    int resp_length = gen_db_comm_err_resp(rsp_buffer, 9999, "Bad CRC");
                    lwip_send(new_tcp_client, rsp_buffer, resp_length, 0);
                }
            }
        } while (received_from_client > 0);
        ESP_LOGI(TAG, "Client disconnected.");
        lwip_close(new_tcp_client);
    }
}


void communication_module() {
    xTaskCreate(&communication_module_server, "comm_server", 8192, NULL, 5, NULL);
}
        os: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#ifndef DB_ESP32_DB_ESP32_COMM_H
#define DB_ESP32_DB_ESP32_COMM_H

void communication_module();

#endif //DB_ESP32_DB_ESP32_COMM_H
        build_type: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include <sys/fcntl.h>
#include <sys/param.h>
#include <string.h>
#include <esp_task_wdt.h>
#include <esp_vfs_dev.h>
#include <esp_wifi.h>
#include <lwip/inet.h>
#include "esp_log.h"
#include "lwip/sockets.h"
#include "driver/uart.h"
#include "globals.h"
#include "msp_ltm_serial.h"
#include "db_protocol.h"
#include "tcp_server.h"

#define TAG "DB_CONTROL"
#define TRANS_RD_BYTES_NUM  8   // amount of bytes read form serial port at once when transparent is selected
#define UDP_BUF_SIZE    2048
#define UART_BUF_SIZE   (1024)
#define MAX_UDP_CLIENTS 8

struct db_udp_connection_t {
    int udp_socket;
    struct sockaddr_in udp_clients[MAX_UDP_CLIENTS];
    bool is_broadcast[MAX_UDP_CLIENTS];  // sockaddr_in at index is added because of station that connected
};

uint16_t app_port_proxy = APP_PORT_PROXY;
uint8_t ltm_frame_buffer[MAX_LTM_FRAMES_IN_BUFFER * LTM_MAX_FRAME_SIZE];
uint ltm_frames_in_buffer = 0;
uint ltm_frames_in_buffer_pnt = 0;

int open_serial_socket() {
    int serial_socket;
    uart_config_t uart_config = {
            .baud_rate = DB_UART_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, DB_UART_PIN_TX, DB_UART_PIN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, 1024, 0, 0, NULL, 0));
    if ((serial_socket = open("/dev/uart/2", O_RDWR)) == -1) {
        ESP_LOGE(TAG, "Cannot open UART2");
        close(serial_socket);
        uart_driver_delete(UART_NUM_2);
        return ESP_FAIL;
    }
    esp_vfs_dev_uart_use_driver(2);
    return serial_socket;
}

int open_udp_socket() {
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    struct sockaddr_in server_addr;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(APP_PORT_PROXY_UDP);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(server_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

    int udp_socket = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (udp_socket < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }
    int err = bind(udp_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    }
    ESP_LOGI(TAG, "Opened UDP socket on port %i", APP_PORT_PROXY_UDP);

    return udp_socket;
}

/**
 * Send to all connected TCP & UDP clients
 *
 * @param tcp_clients
 * @param udp_conn
 * @param data
 * @param data_length
 */
void send_to_all_clients(int tcp_clients[], struct db_udp_connection_t *udp_conn, uint8_t data[], uint data_length) {
    send_to_all_tcp_clients(tcp_clients, data, data_length);

    for (int i = 0; i < MAX_UDP_CLIENTS; i++) {  // send to all UDP clients
        if (udp_conn->udp_clients[i].sin_len > 0) {
            int sent = sendto(udp_conn->udp_socket, data, data_length, 0, (struct sockaddr *) &udp_conn->udp_clients[i],
                              sizeof(udp_conn->udp_clients[i]));
            if (sent != data_length) {
                ESP_LOGE(TAG, "UDP - Error sending (%i/%i) because of %d", sent, data_length, errno);
                udp_conn->udp_clients[i].sin_len = 0;
            }
        }
    }
}

void write_to_uart(const char tcp_client_buffer[], const size_t data_length) {
    int written = uart_write_bytes(UART_NUM_2, tcp_client_buffer, data_length);
    if (written > 0)
        ESP_LOGD(TAG, "Wrote %i bytes", written);
    else
        ESP_LOGE(TAG, "Error writing to UART %s", esp_err_to_name(errno));
}

/**
 * @brief Parses & sends complete MSP & LTM messages
 */
void parse_msp_ltm(int tcp_clients[], struct db_udp_connection_t *udp_conn, uint8_t msp_message_buffer[],
                   uint *serial_read_bytes,
                   msp_ltm_port_t *db_msp_ltm_port) {
    uint8_t serial_bytes[TRANS_RD_BYTES_NUM];
    uint read = 0;
    if ((read = uart_read_bytes(UART_NUM_2, serial_bytes, TRANS_RD_BYTES_NUM, 200 / portTICK_RATE_MS)) > 0) {
        for (uint j = 0; j < read; j++) {
            (*serial_read_bytes)++;
            uint8_t serial_byte = serial_bytes[j];
            if (parse_msp_ltm_byte(db_msp_ltm_port, serial_byte)) {
                msp_message_buffer[(*serial_read_bytes - 1)] = serial_byte;
                if (db_msp_ltm_port->parse_state == MSP_PACKET_RECEIVED) {
                    *serial_read_bytes = 0;
                    send_to_all_clients(tcp_clients, udp_conn, msp_message_buffer, *serial_read_bytes);
                } else if (db_msp_ltm_port->parse_state == LTM_PACKET_RECEIVED) {
                    memcpy(&ltm_frame_buffer[ltm_frames_in_buffer_pnt], db_msp_ltm_port->ltm_frame_buffer,
                           (db_msp_ltm_port->ltm_payload_cnt + 4));
                    ltm_frames_in_buffer_pnt += (db_msp_ltm_port->ltm_payload_cnt + 4);
                    ltm_frames_in_buffer++;
                    if (ltm_frames_in_buffer == LTM_FRAME_NUM_BUFFER &&
                        (LTM_FRAME_NUM_BUFFER <= MAX_LTM_FRAMES_IN_BUFFER)) {
                        send_to_all_clients(tcp_clients, udp_conn, ltm_frame_buffer, *serial_read_bytes);
                        ESP_LOGV(TAG, "Sent %i LTM message(s) to telemetry port!", LTM_FRAME_NUM_BUFFER);
                        ltm_frames_in_buffer = 0;
                        ltm_frames_in_buffer_pnt = 0;
                        *serial_read_bytes = 0;
                    }
                }
            }
        }
    }
}


/**
 * Reads one byte from UART and checks if we already got enough bytes to send them out
 *
 * @param tcp_clients Array of connected TCP clients
 * @param serial_read_bytes Number of bytes already read for the current packet
 */
void parse_transparent(int tcp_clients[], struct db_udp_connection_t *udp_conn, uint8_t serial_buffer[],
                       uint *serial_read_bytes) {
    uint8_t serial_bytes[TRANS_RD_BYTES_NUM];
    uint read = 0;
    if ((read = uart_read_bytes(UART_NUM_2, serial_bytes, TRANS_RD_BYTES_NUM, 200 / portTICK_RATE_MS)) > 0) {
        memcpy(&serial_buffer[*serial_read_bytes], serial_bytes, read);
        *serial_read_bytes += read;
        if (*serial_read_bytes >= TRANSPARENT_BUF_SIZE) {
            send_to_all_clients(tcp_clients, udp_conn, serial_buffer, *serial_read_bytes);
            *serial_read_bytes = 0;
        }
    }
}

/**
 * Check for incoming connections on TCP server
 *
 * @param tcp_master_socket
 * @param tcp_clients
 */
void handle_tcp_master(const int tcp_master_socket, int tcp_clients[]) {
    struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
    uint addr_len = sizeof(source_addr);
    int new_tcp_client = accept(tcp_master_socket, (struct sockaddr *) &source_addr, &addr_len);
    if (new_tcp_client > 0) {
        for (int i = 0; i < CONFIG_LWIP_MAX_ACTIVE_TCP; i++) {
            if (tcp_clients[i] <= 0) {
                tcp_clients[i] = new_tcp_client;
                fcntl(tcp_clients[i], F_SETFL, O_NONBLOCK);
                char addr_str[128];
                inet_ntoa_r(((struct sockaddr_in *) &source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
                ESP_LOGI(TAG, "TCP: New client connected: %s", addr_str);
                return;
            }
        }
        ESP_LOGI(TAG, "TCP: Could not accept connection. Too many clients connected.");
    }
}

/**
 * Add a new client to the list of known UDP clients. Check if client is already known
 *
 * @param connections Structure containing all UDP connection information
 * @param new_client_addr Address of new client
 */
void
add_udp_to_known_clients(struct db_udp_connection_t *connections, struct sockaddr_in new_client_addr, bool is_brdcst) {
    for (int i = 0; i < MAX_UDP_CLIENTS; i++) {
        if (connections->udp_clients[i].sin_port == new_client_addr.sin_port && new_client_addr.sin_family == PF_INET &&
            ((struct sockaddr_in *) &connections->udp_clients[i])->sin_addr.s_addr ==
            ((struct sockaddr_in *) &new_client_addr)->sin_addr.s_addr) {
            return;
        } else if (connections->udp_clients[i].sin_len == 0) {
            connections->udp_clients[i] = new_client_addr;
            char addr_str[128];
            if (new_client_addr.sin_family == PF_INET) {
                inet_ntoa_r(((struct sockaddr_in *) &new_client_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
            }
            connections->is_broadcast[i] = is_brdcst;
            if (!is_brdcst)
                ESP_LOGI(TAG, "UDP: New client connected: %s:%i", addr_str, new_client_addr.sin_port);
            return;
        }
    }
}

/**
 * Get all connected clients to local AP and add to UDP connection list. List is updated every second
 *
 * @param pInt
 */
void update_udp_broadcast(int64_t *last_update, struct db_udp_connection_t *connections, const wifi_mode_t *wifi_mode) {
    if (*wifi_mode == WIFI_MODE_AP && (esp_timer_get_time() - *last_update) >= 1000000) {
        *last_update = esp_timer_get_time();
        // clear all entries
        for (int i = 0; i < MAX_UDP_CLIENTS; i++) {
            if (connections->is_broadcast[i]) {
                memset(&connections->udp_clients[i], 0, sizeof(connections->udp_clients[i]));
            }
        }
        // add based on connected stations
        wifi_sta_list_t sta_list;
        tcpip_adapter_sta_list_t tcpip_sta_list;
        memset(&sta_list, 0, sizeof(sta_list));
        memset(&tcpip_sta_list, 0, sizeof(tcpip_sta_list));
        ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&sta_list));
        ESP_ERROR_CHECK(tcpip_adapter_get_sta_list(&sta_list, &tcpip_sta_list));
        for (int i = 0; i < tcpip_sta_list.num; i++) {
            tcpip_adapter_sta_info_t station = tcpip_sta_list.sta[i];

            struct sockaddr_in new_client_addr;
            new_client_addr.sin_family = PF_INET;
            new_client_addr.sin_port = htons(APP_PORT_PROXY_UDP);
            new_client_addr.sin_len = 16;
            char ip[100];
            sprintf(ip, IPSTR, IP2STR(&station.ip));
            inet_pton(AF_INET, ip, &new_client_addr.sin_addr);
            ESP_LOGD(TAG, "%i %s " IPSTR " " MACSTR "", tcpip_sta_list.num, ip4addr_ntoa(&(station.ip)),
                    IP2STR(&station.ip), MAC2STR(station.mac));
            if (ip[0] != '0')  // DHCP bug. Assigns 0.0.0.0 to station when directly connected on startup
                add_udp_to_known_clients(connections, new_client_addr, true);
        }
    }
}

void control_module_tcp() {
    int uart_socket = open_serial_socket();
    int tcp_master_socket = open_tcp_server(app_port_proxy);

    struct db_udp_connection_t udp_conn;
    udp_conn.udp_socket = open_udp_socket();
    fcntl(udp_conn.udp_socket, F_SETFL, O_NONBLOCK);
    char udp_buffer[UDP_BUF_SIZE];
    struct sockaddr_in udp_source_addr;
    socklen_t udp_socklen = sizeof(udp_source_addr);
    for (int i = 0; i < MAX_UDP_CLIENTS; i++)
        memset(&(udp_conn.udp_clients[i]), 0, sizeof(struct sockaddr_in));

    int tcp_clients[CONFIG_LWIP_MAX_ACTIVE_TCP];
    for (int i = 0; i < CONFIG_LWIP_MAX_ACTIVE_TCP; i++) {
        tcp_clients[i] = -1;
    }
    if (tcp_master_socket == ESP_FAIL || uart_socket == ESP_FAIL) {
        ESP_LOGE(TAG, "Can not start control module");
    }
    fcntl(tcp_master_socket, F_SETFL, O_NONBLOCK);
    uint read_transparent = 0;
    uint read_msp_ltm = 0;
    char tcp_client_buffer[TCP_BUFF_SIZ];
    memset(tcp_client_buffer, 0, TCP_BUFF_SIZ);
    uint8_t msp_message_buffer[UART_BUF_SIZE];
    uint8_t serial_buffer[TRANSPARENT_BUF_SIZE];
    msp_ltm_port_t db_msp_ltm_port;

    int64_t last_udp_brdc_update = esp_timer_get_time();  // time since boot for UDP broadcast update
    wifi_mode_t wifi_mode;
    esp_wifi_get_mode(&wifi_mode);

    ESP_LOGI(TAG, "Started control module");
    while (1) {
        handle_tcp_master(tcp_master_socket, tcp_clients);
        for (int i = 0; i < CONFIG_LWIP_MAX_ACTIVE_TCP; i++) {  // handle TCP clients
            if (tcp_clients[i] > 0) {
                ssize_t recv_length = recv(tcp_clients[i], tcp_client_buffer, TCP_BUFF_SIZ, 0);
                if (recv_length > 0) {
                    ESP_LOGD(TAG, "TCP: Received %i bytes", recv_length);
                    write_to_uart(tcp_client_buffer, recv_length);
                } else if (recv_length == 0) {
                    shutdown(tcp_clients[i], 0);
                    close(tcp_clients[i]);
                    tcp_clients[i] = -1;
                    ESP_LOGI(TAG, "TCP client disconnected");
                } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    ESP_LOGE(TAG, "Error receiving from TCP client %i (fd: %i): %d", i, tcp_clients[i], errno);
                    shutdown(tcp_clients[i], 0);
                    close(tcp_clients[i]);
                    tcp_clients[i] = -1;
                }
            }
        }
        // handle incoming UDP data
        ssize_t recv_length = recvfrom(udp_conn.udp_socket, udp_buffer, UDP_BUF_SIZE, 0,
                                       (struct sockaddr *) &udp_source_addr, &udp_socklen);
        if (recv_length > 0) {
            ESP_LOGD(TAG, "UDP: Received %i bytes", recv_length);
            write_to_uart(udp_buffer, recv_length);
            add_udp_to_known_clients(&udp_conn, udp_source_addr, false);
        }
        update_udp_broadcast(&last_udp_brdc_update, &udp_conn, &wifi_mode);
        switch (SERIAL_PROTOCOL) {
            case 1:
            case 2:
                parse_msp_ltm(tcp_clients, &udp_conn, msp_message_buffer, &read_msp_ltm, &db_msp_ltm_port);
                break;
            default:
            case 3:
            case 4:
            case 5:
                parse_transparent(tcp_clients, &udp_conn, serial_buffer, &read_transparent);
                break;
        }
    }
    vTaskDelete(NULL);
}

/**
 * @brief DroneBridge control module implementation for a ESP32 device. Bi-directional link between FC and ground. Can
 * handle MSPv1, MSPv2, LTM and MAVLink.
 * MSP & LTM is parsed and sent packet/frame by frame to ground
 * MAVLink is passed through (fully transparent). Can be used with any protocol.
 */
void control_module() {
    xEventGroupWaitBits(wifi_event_group, BIT2, false, true, portMAX_DELAY);
    xTaskCreate(&control_module_tcp, "control_tcp", 40960, NULL, 5, NULL);
}
        c_compiler: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#ifndef DB_ESP32_DB_ESP32_CONTROL_H
#define DB_ESP32_DB_ESP32_CONTROL_H

void control_module();

#endif //DB_ESP32_DB_ESP32_CONTROL_H
        include: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#ifndef DB_ESP32_DB_ESP32_SETTINGS_H
#define DB_ESP32_DB_ESP32_SETTINGS_H

#endif //DB_ESP32_DB_ESP32_SETTINGS_H
          - os: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2017 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include <stdint.h>
#include <time.h>
#include <net/if.h>

#ifndef DB_PROTOCOL_H_INCLUDED
#define DB_PROTOCOL_H_INCLUDED

#define RADIOTAP_LENGTH         13
#define DB_RAW_V2_HEADER_LENGTH 10
#define DB_MAX_ADAPTERS 4

#define MSP_DATA_LENTH          34      // size of MSP v1
#define MSP_V2_DATA_LENGTH      37      // size of MSP v2 frame
#define DB_RC_DATA_LENGTH                16                // size of DB_RC frame
#define DATA_UNI_LENGTH         2048        // max payload length for raw protocol
#define DB_RAW_OFFSET                        14      // when adhering the 802.11 header the payload is offset to not be overwritten by SQN
#define MAX_DB_DATA_LENGTH                (RADIOTAP_LENGTH + DB_RAW_V2_HEADER_LENGTH + DATA_UNI_LENGTH) // max length of a db raw packet
#define ETHER_TYPE              0x88ab

#define DEFAULT_DB_MODE         'm'
#define DEFAULT_DB_IF           "18a6f716a511"
#define DEFAULT_V2_COMMID                0xc8

#define DB_FRAMETYPE_RTS        1
#define DB_FRAMETYPE_DATA       2
#define DB_FRAMETYPE_BEACON     3
#define DB_FRAMETYPE_DEFAULT    DB_FRAMETYPE_DATA

#define NUM_CHANNELS            14      // max number of channels sent over DroneBridge control module (ground)
#define DB_RC_NUM_CHANNELS      12      // number of channels supported by DroneBridge RC protocol

#define DB_PORT_CONTROLLER  0x01
#define DB_PORT_TELEMETRY   0x02  // deprecated. Use proxy port for bidirectional telemetry
#define DB_PORT_VIDEO       0x03
#define DB_PORT_COMM                0x04
#define DB_PORT_STATUS                0x05
#define DB_PORT_PROXY                0x06
#define DB_PORT_RC                        0x07

#define DB_DIREC_DRONE      0x01 // packet to/for drone
#define DB_DIREC_GROUND           0x03 // packet to/for ground station

#define APP_PORT_STATUS     1602 // for all kinds of status protocol messages. Same port on ground station and app
#define APP_PORT_COMM       1603
#define APP_PORT_TELEMETRY  1604 // accepts MAVLink and LTM telemetry messages. Non MAVLink telemetry messages get rerouted internally to APP_PORT_PROXY
#define PORT_TCP_SYSLOG_SERVER 1605
#define APP_PORT_PROXY                 5760 // use this port for all MAVLink messages (TCP)
#define APP_PORT_PROXY_UDP        14550 // use this port for all MAVLink messages (UDP)
#define APP_PORT_VIDEO      5000 // app accepts raw H.264 streams
#define APP_PORT_VIDEO_FEC  5001 // app accepts raw DroneBridge video stream data, performs FEC on Android device

#define DB_MAVLINK_SYS_ID        69
#define        MAX_PENUMBRA_INTERFACES 8

#define DB_UNIX_DOMAIN_VIDEO_PATH   "/tmp/db_video_out"
#define DB_AP_CLIENT_IP             "192.168.2.1"   // default IP address of GCS connected via WiFi AP

#define DB_SYS_HID_ESP32 1

#endif // DB_PROTOCOL_H_INCLUDED
            c_compiler: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#ifndef DB_ESP32_GLOBALS_H
#define DB_ESP32_GLOBALS_H

#include <freertos/event_groups.h>

#define MAX_LTM_FRAMES_IN_BUFFER 5
#define BUILDVERSION 6    //v0.6

// can be set by user
extern uint8_t DEFAULT_SSID[32];
extern uint8_t DEFAULT_PWD[64];
extern uint8_t DEFAULT_CHANNEL;
extern uint8_t SERIAL_PROTOCOL;  // 1,2=MSP, 3,4,5=MAVLink/transparent
extern uint8_t DB_UART_PIN_TX;
extern uint8_t DB_UART_PIN_RX;
extern uint32_t DB_UART_BAUD_RATE;
extern uint16_t TRANSPARENT_BUF_SIZE;
extern uint8_t LTM_FRAME_NUM_BUFFER;    // Number of LTM frames per UDP packet (min = 1; max = 5)
extern EventGroupHandle_t wifi_event_group;

#endif //DB_ESP32_GLOBALS_H
            cpp_compiler: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include <sys/socket.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <string.h>
#include <nvs.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "globals.h"
#include <math.h>
#include <driver/gpio.h>

#define LISTENQ 2
#define REQUEST_BUF_SIZE 1024
#define WEBSITE_RESPONSE_BUFFER_SIZE 3072
#define TAG "TCP_SERVER"

const char *save_response = "HTTP/1.1 200 OK\r\n"
                            "Server: DroneBridgeESP32\r\n"
                            "Content-type: text/html, text, plain\r\n"
                            "\r\n"
                            "<!DOCTYPE html>\n"
                            "<html>\n"
                            "<head>\n"
                            "  <style>\n"
                            "  .mytext {\n"
                            "    font-family: Verdana, Geneva, sans-serif;\n"
                            "    color: #FFFFF;\n"
                            "  }\n"
                            "  h1 {\n"
                            "    font-family: Verdana, Geneva, sans-serif;\n"
                            "    color: #FF7900;\n"
                            "  }\n"
                            "  </style>\n"
                            "<title>DB for ESP32 Settings</title>\n"
                            "<meta name=\"viewport\" content=\"width=device-width, user-scalable=no\" />\n"
                            "</head>\n"
                            "<body>\n"
                            "  <h1>DroneBridge for ESP32</h1>\n"
                            "  <p class=\"mytext\">Saved settings!</p>\n"
                            "  <a class=\"mytext\" href=\"/\">Back to settings</a>\n"
                            "</body>\n"
                            "</html>\n"
                            "";

const char *bad_gateway = "HTTP/1.1 502 Bad Gateway \r\n"
                          "Server: DroneBridgeESP32 \r\n"
                          "Content-Type: text/html \r\n"
                          "\r\n";

/**
 * @brief Check if we got a simple request or new settings
 * @param request_buffer
 * @param length
 * @return 0 if GET-Request, 1 if new settings, -1 if none
 */
int http_request_type(uint8_t *request_buffer, uint length) {
    uint8_t http_get_header[] = {'G', 'E', 'T', ' ', '/', ' ', 'H', 'T', 'T', 'P'};
    uint8_t http_get_header_settings[] = {'G', 'E', 'T', ' ', '/', 's', 'e', 't', 't', 'i', 'n', 'g', 's'};
    uint8_t http_get_header_other_data[] = {'G', 'E', 'T', ' ', '/'};
    if (memcmp(request_buffer, http_get_header, sizeof(http_get_header)) == 0) return 0;
    if (memcmp(request_buffer, http_get_header_settings, sizeof(http_get_header_settings)) == 0) return 1;
    if (memcmp(request_buffer, http_get_header_other_data, sizeof(http_get_header_other_data)) == 0) return 2;
    return -1;
}


void write_settings_to_nvs() {
    ESP_LOGI(TAG, "Saving to NVS");
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("settings", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, "ssid", (char *) DEFAULT_SSID));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, "wifi_pass", (char *) DEFAULT_PWD));
    ESP_ERROR_CHECK(nvs_set_u8(my_handle, "wifi_chan", DEFAULT_CHANNEL));
    ESP_ERROR_CHECK(nvs_set_u32(my_handle, "baud", DB_UART_BAUD_RATE));
    ESP_ERROR_CHECK(nvs_set_u8(my_handle, "gpio_tx", DB_UART_PIN_TX));
    ESP_ERROR_CHECK(nvs_set_u8(my_handle, "gpio_rx", DB_UART_PIN_RX));
    ESP_ERROR_CHECK(nvs_set_u8(my_handle, "proto", SERIAL_PROTOCOL));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "trans_pack_size", TRANSPARENT_BUF_SIZE));
    ESP_ERROR_CHECK(nvs_set_u8(my_handle, "ltm_per_packet", LTM_FRAME_NUM_BUFFER));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}


void parse_save_get_parameters(char *request_buffer, uint length) {
    ESP_LOGI(TAG, "Parsing new settings:");
    char *ptr;
    char delimiter[] = "?=& ";
    ptr = strtok(request_buffer, delimiter);
    while (ptr != NULL) {
        if (strcmp(ptr, "ssid") == 0) {
            ptr = strtok(NULL, delimiter);
            if (strlen(ptr) >= 1) {
                strcpy((char *) DEFAULT_SSID, ptr);
                for(size_t i = 0; i <= strlen((char *) DEFAULT_SSID); i++)
                    if(DEFAULT_SSID[i] == '+'){ DEFAULT_SSID[i] = ' '; }  // replace + with space
                ESP_LOGI(TAG, "New ssid: %s", DEFAULT_SSID);
            }
        } else if (strcmp(ptr, "wifi_pass") == 0) {
            ptr = strtok(NULL, delimiter);
            if (strlen(ptr) >= 8) {
                strcpy((char *) DEFAULT_PWD, ptr);
                ESP_LOGI(TAG, "New password: %s", DEFAULT_PWD);
            }
        } else if (strcmp(ptr, "wifi_chan") == 0) {
            ptr = strtok(NULL, delimiter);
            if (atoi(ptr) <= 13) DEFAULT_CHANNEL = atoi(ptr);
            ESP_LOGI(TAG, "New wifi channel: %i", DEFAULT_CHANNEL);
        } else if (strcmp(ptr, "baud") == 0) {
            ptr = strtok(NULL, delimiter);
            if (atoi(ptr) > 2399)
                DB_UART_BAUD_RATE = atoi(ptr);
            ESP_LOGI(TAG, "New baud: %i", DB_UART_BAUD_RATE);
        } else if (strcmp(ptr, "gpio_tx") == 0) {
            ptr = strtok(NULL, delimiter);
            if (atoi(ptr) <= GPIO_NUM_MAX) DB_UART_PIN_TX = atoi(ptr);
            ESP_LOGI(TAG, "New gpio_tx: %i", DB_UART_PIN_TX);
        } else if (strcmp(ptr, "gpio_rx") == 0) {
            ptr = strtok(NULL, delimiter);
            if (atoi(ptr) <= GPIO_NUM_MAX) DB_UART_PIN_RX = atoi(ptr);
            ESP_LOGI(TAG, "New gpio_rx: %i", DB_UART_PIN_RX);
        } else if (strcmp(ptr, "proto") == 0) {
            ptr = strtok(NULL, delimiter);
            if (strcmp(ptr, "msp_ltm") == 0) {
                SERIAL_PROTOCOL = 2;
            } else {
                SERIAL_PROTOCOL = 4;
            }
            ESP_LOGI(TAG, "New proto: %i", SERIAL_PROTOCOL);
        } else if (strcmp(ptr, "trans_pack_size") == 0) {
            ptr = strtok(NULL, delimiter);
            TRANSPARENT_BUF_SIZE = atoi(ptr);
            ESP_LOGI(TAG, "New trans_pack_size: %i", TRANSPARENT_BUF_SIZE);
        } else if (strcmp(ptr, "ltm_per_packet") == 0) {
            ptr = strtok(NULL, delimiter);
            LTM_FRAME_NUM_BUFFER = atoi(ptr);
            ESP_LOGI(TAG, "New ltm_per_packet: %i", LTM_FRAME_NUM_BUFFER);
        } else {
            ptr = strtok(NULL, delimiter);
        }
    }
    write_settings_to_nvs();
}


char *create_response(char *website_response) {
    char baud_selection1[9] = "";
    char baud_selection2[9] = "";
    char baud_selection3[9] = "";
    char baud_selection4[9] = "";
    char baud_selection5[9] = "";
    char baud_selection6[9] = "";
    char baud_selection7[9] = "";
    char uart_serial_selection1[9] = "";
    char uart_serial_selection2[9] = "";
    char trans_pack_size_selection1[9] = "";
    char trans_pack_size_selection2[9] = "";
    char trans_pack_size_selection3[9] = "";
    char trans_pack_size_selection4[9] = "";
    char trans_pack_size_selection5[9] = "";
    char ltm_size_selection1[9] = "";
    char ltm_size_selection2[9] = "";
    char ltm_size_selection3[9] = "";
    char ltm_size_selection4[9] = "", ltm_size_selection5[9] = "";

    switch (SERIAL_PROTOCOL) {
        default:
        case 1:
        case 2:
            strcpy(uart_serial_selection1, "selected");
            break;
        case 3:
        case 4:
        case 5:
            strcpy(uart_serial_selection2, "selected");
            break;
    }
    switch (TRANSPARENT_BUF_SIZE) {
        case 16:
            strcpy(trans_pack_size_selection1, "selected");
            break;
        case 32:
            strcpy(trans_pack_size_selection2, "selected");
            break;
        default:
        case 64:
            strcpy(trans_pack_size_selection3, "selected");
            break;
        case 128:
            strcpy(trans_pack_size_selection4, "selected");
            break;
        case 256:
            strcpy(trans_pack_size_selection5, "selected");
            break;
    }
    switch (LTM_FRAME_NUM_BUFFER) {
        default:
        case 1:
            strcpy(ltm_size_selection1, "selected");
            break;
        case 2:
            strcpy(ltm_size_selection2, "selected");
            break;
        case 3:
            strcpy(ltm_size_selection3, "selected");
            break;
        case 4:
            strcpy(ltm_size_selection4, "selected");
            break;
        case 5:
            strcpy(ltm_size_selection5, "selected");
            break;
    }
    switch (DB_UART_BAUD_RATE) {
        default:
        case 115200:
            strcpy(baud_selection1, "selected");
            break;
        case 57600:
            strcpy(baud_selection2, "selected");
            break;
        case 38400:
            strcpy(baud_selection3, "selected");
            break;
        case 19200:
            strcpy(baud_selection4, "selected");
            break;
        case 9600:
            strcpy(baud_selection5, "selected");
            break;
        case 4800:
            strcpy(baud_selection6, "selected");
            break;
        case 2400:
            strcpy(baud_selection7, "selected");
            break;
    }
    char build_version[16];
    sprintf(build_version, "v%.2f", floorf(BUILDVERSION) / 100);
    sprintf(website_response, "HTTP/1.1 200 OK\r\n"
                              "Server: DroneBridgeESP32\r\n"
                              "Content-type: text/html, text, plain\r\n"
                              "\r\n"
                              "<!DOCTYPE html><html><head><title>DB for ESP32 Settings</title>"
                              "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">"
                              "<style> "
                              "h1 { font-family: Verdana, Geneva, sans-serif; color: #FF7900; } "
                              "table.DroneBridge { font-family: Verdana, Geneva, sans-serif; background-color: #145396; text-align: right; border-collapse: collapse; } "
                              "table.DroneBridge td, table.DroneBridge th { padding: 6px 0px; } "
                              "table.DroneBridge tbody td { font-size: 1rm; font-weight: bold; color: #FFFFFF; padding: 0.5em; } "
                              "table.DroneBridge td:nth-child(even) { background: #FF7900; }"
                              "@media only screen and (max-width:768px) { .DroneBridge {width:100%%; }}"
                              ".foot{color: #145396;font-family: Verdana, Geneva, sans-serif;font-size: 0.8em;}"
                              "</style>"
                              "</head>\n"
                              "<body><h1>DroneBridge for ESP32</h1>"
                              "<form action=\"/settings.html\" id=\"settings_form\" method=\"get\" target=\"_blank\">"
                              "<table class=\"DroneBridge\"><tbody>"
                              "<tr><td>Wifi SSID</td>"
                              "<td><input type=\"text\" name=\"ssid\" value=\"%s\"></td></tr>"
                              "<tr><td>Wifi password</td>"
                              "<td><input type=\"text\" name=\"wifi_pass\" value=\"%s\"></td></tr>"
                              "<tr><td>Wifi channel</td>"
                              "<td><input type=\"number\" name=\"wifi_chan\" min=\"0\" max=\"13\" value=\"%i\"></td></tr>"
                              "<tr><td>UART baud rate</td><td>"
                              "<select name=\"baud\" form=\"settings_form\">"
                              "<option %s value=\"115200\">115200</option>"
                              "<option %s value=\"57600\">57600</option>"
                              "<option %s value=\"38400\">38400</option>"
                              "<option %s value=\"19200\">19200</option>"
                              "<option %s value=\"9600\">9600</option>"
                              "<option %s value=\"4800\">4800</option>"
                              "<option %s value=\"2400\">2400</option>"
                              "</select>"
                              "</td></tr><tr><td>GPIO TX pin number</td>"
                              "<td><input type=\"text\" name=\"gpio_tx\" value=\"%i\"></td></tr>"
                              "<tr><td>GPIO RX pin number</td><td>"
                              "<input type=\"text\" name=\"gpio_rx\" value=\"%i\">"
                              "</td></tr><tr><td>UART serial protocol</td><td>"
                              "<select name=\"proto\" form=\"settings_form\">"
                              "<option %s value=\"msp_ltm\">MSP/LTM</option>"
                              "<option %s value=\"trans\">Transparent/MAVLink</option>"
                              "</select>"
                              "</td></tr><tr><td>Transparent packet size</td><td>"
                              "<select name=\"trans_pack_size\" form=\"settings_form\">"
                              "<option %s value=\"16\">16</option><option %s value=\"32\">32</option>"
                              "<option %s value=\"64\">64</option><option %s value=\"128\">128</option>"
                              "<option %s value=\"256\">256</option>"
                              "</select>"
                              "</td></tr><tr><td>LTM frames per packet</td><td>"
                              "<select name=\"ltm_per_packet\" form=\"settings_form\">"
                              "<option %s value=\"1\">1</option>"
                              "<option %s value=\"2\">2</option>"
                              "<option %s value=\"3\">3</option>"
                              "<option %s value=\"4\">4</option>"
                              "<option %s value=\"5\">5</option>"
                              "</select>"

                              "</td></tr><tr><td></td><td>"
                              "</td></tr></tbody></table><p></p>"

                              "<input target= \"_top\" type=\"submit\" value=\"Save\">"
                              "</form>"
                              "<p class=\"foot\">%s</p>\n"
                              "<p class=\"foot\">&copy; Wolfgang Christl 2018 - Apache 2.0 License</p>"
                              "</body></html>\n"
                              "", DEFAULT_SSID, DEFAULT_PWD, DEFAULT_CHANNEL, baud_selection1, baud_selection2, baud_selection3, baud_selection4,
            baud_selection5, baud_selection6, baud_selection7, DB_UART_PIN_TX, DB_UART_PIN_RX, uart_serial_selection1,
            uart_serial_selection2, trans_pack_size_selection1, trans_pack_size_selection2, trans_pack_size_selection3,
            trans_pack_size_selection4, trans_pack_size_selection5, ltm_size_selection1, ltm_size_selection2,
            ltm_size_selection3, ltm_size_selection4, ltm_size_selection5, build_version);
    return website_response;
}

void http_settings_server(void *parameter) {
    ESP_LOGI(TAG, "http_settings_server task started");
    struct sockaddr_in tcpServerAddr;
    tcpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons(80);
    int tcp_socket, r;
    char recv_buf[64];
    static struct sockaddr_in remote_addr;
    static unsigned int socklen;
    socklen = sizeof(remote_addr);
    xEventGroupWaitBits(wifi_event_group, BIT2, false, true, portMAX_DELAY);
    while (1) {
        tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (tcp_socket < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        if (bind(tcp_socket, (struct sockaddr *) &tcpServerAddr, sizeof(tcpServerAddr)) != 0) {
            ESP_LOGE(TAG, "... socket bind failed errno=%d", errno);
            close(tcp_socket);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        if (listen(tcp_socket, LISTENQ) != 0) {
            ESP_LOGE(TAG, "... socket listen failed errno=%d", errno);
            close(tcp_socket);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        uint8_t *request_buffer = malloc(REQUEST_BUF_SIZE * sizeof(uint8_t));
        // char *website_response = malloc(WEBSITE_RESPONSE_BUFFER_SIZE*sizeof(char));
        char website_response[WEBSITE_RESPONSE_BUFFER_SIZE];
        while (1) {
            int client_socket = accept(tcp_socket, (struct sockaddr *) &remote_addr, &socklen);
            fcntl(client_socket, F_SETFL, O_NONBLOCK);
            uint rec_length = 0;
            do {
                bzero(recv_buf, sizeof(recv_buf));
                r = recv(client_socket, recv_buf, sizeof(recv_buf) - 1, 0);
                if (r > 0) {
                    if (REQUEST_BUF_SIZE >= (rec_length + r)) {
                        memcpy(&request_buffer[rec_length], recv_buf, (size_t) r);
                        rec_length += r;
                    } else {
                        ESP_LOGE(TAG, "Request bigger than buffer");
                    }
                }
            } while (r > 0);
            // prints the requests for debugging
//            ESP_LOGI(TAG2,"New connection request,Request data:");
//            for(int i = 0; i < rec_length; i++) {
//                putchar(request_buffer[i]);
//            }
            int http_req = http_request_type(request_buffer, rec_length);
            if (http_req == 0) {
                char *response = create_response(website_response);
                if (write(client_socket, response, strlen(response)) < 0) {
                    ESP_LOGE(TAG, "... Send failed");
                    close(tcp_socket);
                    vTaskDelay(4000 / portTICK_PERIOD_MS);
                    continue;
                }
            } else if (http_req == 1) {
                parse_save_get_parameters((char *) request_buffer, rec_length);
                if (write(client_socket, save_response, strlen(save_response)) < 0) {
                    ESP_LOGE(TAG, "... Send failed");
                    close(tcp_socket);
                    vTaskDelay(4000 / portTICK_PERIOD_MS);
                    continue;
                }
            } else if (http_req == 2) {
                if (write(client_socket, bad_gateway, strlen(bad_gateway)) < 0) {
                    ESP_LOGE(TAG, "... Send failed");
                    close(tcp_socket);
                    vTaskDelay(4000 / portTICK_PERIOD_MS);
                    continue;
                }
            }
            close(client_socket);
        }
        //free(website_response);
        free(request_buffer);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "...tcp_client task closed\n");
    vTaskDelete(NULL);
}


/**
 * @brief Starts a TCP server that serves the page to change settings & handles the changes
 */
void start_tcp_server() {
    xTaskCreate(&http_settings_server, "http_settings_server", 10240, NULL, 5, NULL);
}
          - os: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#ifndef DB_ESP32_HTTP_SERVER_H
#define DB_ESP32_HTTP_SERVER_H

void start_tcp_server();
void write_settings_to_nvs();

#endif //DB_ESP32_HTTP_SERVER_H
            c_compiler: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include <stdio.h>
#include <nvs_flash.h>
#include <esp_wifi_types.h>
#include <mdns.h>
#include <string.h>
#include <driver/gpio.h>
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "db_esp32_control.h"
#include "http_server.h"
#include "db_esp32_comm.h"
#include "db_protocol.h"

EventGroupHandle_t wifi_event_group;
static const char *TAG = "DB_ESP32";

uint8_t DEFAULT_SSID[32] = "DroneBridge ESP32";
uint8_t DEFAULT_PWD[64] = "dronebridge";
uint8_t DEFAULT_CHANNEL = 6;
uint8_t SERIAL_PROTOCOL = 2;  // 1,2=MSP, 3,4,5=MAVLink/transparent
uint8_t DB_UART_PIN_TX = GPIO_NUM_17;
uint8_t DB_UART_PIN_RX = GPIO_NUM_16;
uint32_t DB_UART_BAUD_RATE = 115200;
uint16_t TRANSPARENT_BUF_SIZE = 64;
uint8_t LTM_FRAME_NUM_BUFFER = 1;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    wifi_event_ap_staconnected_t *event;
    wifi_event_ap_stadisconnected_t* evente;
    switch (event_id) {
        case SYSTEM_EVENT_AP_START:
            ESP_LOGI(TAG, "Wifi AP started!");
            xEventGroupSetBits(wifi_event_group, BIT2);
            break;
        case SYSTEM_EVENT_AP_STOP:
            ESP_LOGI(TAG, "Wifi AP stopped!");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            event = (wifi_event_ap_staconnected_t *) event_data;
            ESP_LOGI(TAG, "Client connected - station:"MACSTR", AID=%d", MAC2STR(event->mac), event->aid);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            evente = (wifi_event_ap_stadisconnected_t*) event_data;
            ESP_LOGI(TAG, "Client disconnected - station:"MACSTR", AID=%d",
                     MAC2STR(evente->mac), evente->aid);
            break;
        default:
            break;
    }
}

void start_mdns_service()
{
    xEventGroupWaitBits(wifi_event_group, BIT2, false, true, portMAX_DELAY);
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return;
    }
    ESP_ERROR_CHECK(mdns_hostname_set("dronebridge"));
    ESP_ERROR_CHECK(mdns_instance_name_set("DroneBridge for ESP32"));

    ESP_ERROR_CHECK(mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0));
    ESP_ERROR_CHECK(mdns_service_add(NULL, "_db_proxy", "_tcp", APP_PORT_PROXY, NULL, 0));
    ESP_ERROR_CHECK(mdns_service_add(NULL, "_db_comm", "_tcp", APP_PORT_COMM, NULL, 0));
    ESP_ERROR_CHECK(mdns_service_instance_name_set("_http", "_tcp", "DroneBridge for ESP32"));
}


void init_wifi(){
    wifi_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t ap_config = {
            .ap = {
                    .ssid = "Init DroneBridge ESP32",
                    .ssid_len = 0,
                    .authmode = WIFI_AUTH_WPA_PSK,
                    .channel = DEFAULT_CHANNEL,
                    .ssid_hidden = 0,
                    .beacon_interval = 100,
                    .max_connection = 10
            },
    };
    xthal_memcpy(ap_config.ap.ssid, DEFAULT_SSID, 32);
    xthal_memcpy(ap_config.ap.password, DEFAULT_PWD, 64);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_AP, WIFI_PROTOCOL_11B));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    wifi_country_t wifi_country = {.cc = "XX", .schan = 1, .nchan = 13, .policy = WIFI_COUNTRY_POLICY_MANUAL};
    ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, "DBESP32"));

    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    tcpip_adapter_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, 192,168,2,1);
    IP4_ADDR(&ip_info.gw, 192,168,2,1);
    IP4_ADDR(&ip_info.netmask, 255,255,255,0);
    ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info));
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
}


void read_settings_nvs(){
    nvs_handle my_handle;
    if (nvs_open("settings", NVS_READONLY, &my_handle) == ESP_ERR_NVS_NOT_FOUND){
        // First start
        nvs_close(my_handle);
        write_settings_to_nvs();
    } else {
        ESP_LOGI(TAG, "Reading settings from NVS");
        size_t required_size = 0;
        ESP_ERROR_CHECK(nvs_get_str(my_handle, "ssid", NULL, &required_size));
        char* ssid = malloc(required_size);
        ESP_ERROR_CHECK(nvs_get_str(my_handle, "ssid", ssid, &required_size));
        memcpy(DEFAULT_SSID, ssid, required_size);

        ESP_ERROR_CHECK(nvs_get_str(my_handle, "wifi_pass", NULL, &required_size));
        char* wifi_pass = malloc(required_size);
        ESP_ERROR_CHECK(nvs_get_str(my_handle, "wifi_pass", wifi_pass, &required_size));
        memcpy(DEFAULT_PWD, wifi_pass, required_size);

        ESP_ERROR_CHECK(nvs_get_u8(my_handle, "wifi_chan", &DEFAULT_CHANNEL));
        ESP_ERROR_CHECK(nvs_get_u32(my_handle, "baud", &DB_UART_BAUD_RATE));
        ESP_ERROR_CHECK(nvs_get_u8(my_handle, "gpio_tx", &DB_UART_PIN_TX));
        ESP_ERROR_CHECK(nvs_get_u8(my_handle, "gpio_rx", &DB_UART_PIN_RX));
        ESP_ERROR_CHECK(nvs_get_u8(my_handle, "proto", &SERIAL_PROTOCOL));
        ESP_ERROR_CHECK(nvs_get_u16(my_handle, "trans_pack_size", &TRANSPARENT_BUF_SIZE));
        ESP_ERROR_CHECK(nvs_get_u8(my_handle, "ltm_per_packet", &LTM_FRAME_NUM_BUFFER));
        nvs_close(my_handle);
        free(wifi_pass);
        free(ssid);
    }
}


void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    read_settings_nvs();
    esp_log_level_set("*", ESP_LOG_INFO);
    init_wifi();
    start_mdns_service();
    control_module();
    start_tcp_server();
    communication_module();
}
            cpp_compiler: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2018 Wolfgang Christl
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "msp_ltm_serial.h"
#include "db_crc.h"

/**
 * This function is part of Cleanflight/iNAV.
 *
 * Optimized for crc performance in the DroneBridge project and ">" & "<" adjusted
 * LTM telemetry parsing added by 
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */
bool parse_msp_ltm_byte(msp_ltm_port_t *msp_ltm_port, uint8_t new_byte) {
    switch (msp_ltm_port->parse_state) {
        default:
        case IDLE:
            if (new_byte == '$') {
                msp_ltm_port->mspVersion = MSP_V1;
                msp_ltm_port->parse_state = HEADER_START;
                msp_ltm_port->ltm_frame_buffer[0] = '$';
            } else { return false; }
            msp_ltm_port->ltm_payload_cnt = 0;
            msp_ltm_port->checksum1 = 0;
            break;

        case HEADER_START:
            switch (new_byte) {
                case 'M':
                    msp_ltm_port->parse_state = MSP_HEADER_M;
                    break;
                case 'X':
                    msp_ltm_port->parse_state = MSP_HEADER_X;
                    break;
                case 'T':
                    msp_ltm_port->parse_state = LTM_HEADER;
                    msp_ltm_port->ltm_frame_buffer[1] = 'T';
                    break;
                default:
                    msp_ltm_port->parse_state = IDLE;
                    break;
            }
            break;

        case LTM_HEADER:
            switch (new_byte) {
                case 'A':
                    msp_ltm_port->ltm_type = LTM_TYPE_A;
                    msp_ltm_port->parse_state = LTM_TYPE_IDENT;
                    break;
                case 'G':
                    msp_ltm_port->ltm_type = LTM_TYPE_G;
                    msp_ltm_port->parse_state = LTM_TYPE_IDENT;
                    break;
                case 'N':
                    msp_ltm_port->ltm_type = LTM_TYPE_N;
                    msp_ltm_port->parse_state = LTM_TYPE_IDENT;
                    break;
                case 'O':
                    msp_ltm_port->ltm_type = LTM_TYPE_O;
                    msp_ltm_port->parse_state = LTM_TYPE_IDENT;
                    break;
                case 'S':
                    msp_ltm_port->ltm_type = LTM_TYPE_S;
                    msp_ltm_port->parse_state = LTM_TYPE_IDENT;
                    break;
                case 'X':
                    msp_ltm_port->ltm_type = LTM_TYPE_X;
                    msp_ltm_port->parse_state = LTM_TYPE_IDENT;
                    break;
                default:
                    msp_ltm_port->parse_state = IDLE;
                    break;
            }
            msp_ltm_port->ltm_frame_buffer[2] = new_byte;
            msp_ltm_port->ltm_payload_cnt = 0;
            break;

        case LTM_TYPE_IDENT:
            msp_ltm_port->ltm_payload_cnt++;
            msp_ltm_port->ltm_frame_buffer[2 + msp_ltm_port->ltm_payload_cnt] = new_byte;
            msp_ltm_port->checksum1 ^= new_byte;
            switch (msp_ltm_port->ltm_type) {
                case LTM_TYPE_A:
                case LTM_TYPE_N:
                case LTM_TYPE_X:
                    if (msp_ltm_port->ltm_payload_cnt == LTM_TYPE_A_PAYLOAD_SIZE) msp_ltm_port->parse_state = LTM_CRC;
                    break;
                case LTM_TYPE_G:
                case LTM_TYPE_O:
                    if (msp_ltm_port->ltm_payload_cnt == LTM_TYPE_G_PAYLOAD_SIZE) msp_ltm_port->parse_state = LTM_CRC;
                    break;
                case LTM_TYPE_S:
                    if (msp_ltm_port->ltm_payload_cnt == LTM_TYPE_S_PAYLOAD_SIZE) msp_ltm_port->parse_state = LTM_CRC;
                    break;
                default:
                    msp_ltm_port->parse_state = IDLE;
                    break;
            }
            break;

        case LTM_CRC:
            msp_ltm_port->ltm_frame_buffer[3 + msp_ltm_port->ltm_payload_cnt] = new_byte;
            if (msp_ltm_port->checksum1 == new_byte) {
                msp_ltm_port->parse_state = LTM_PACKET_RECEIVED;
            } else {
                msp_ltm_port->parse_state = IDLE;
            }
            break;

        case MSP_HEADER_M:
            if (new_byte == '>') {
                msp_ltm_port->offset = 0;
                msp_ltm_port->checksum1 = 0;
                msp_ltm_port->checksum2 = 0;
                msp_ltm_port->parse_state = MSP_HEADER_V1;
            } else {
                msp_ltm_port->parse_state = IDLE;
            }
            break;

        case MSP_HEADER_X:
            if (new_byte == '>') {
                msp_ltm_port->offset = 0;
                msp_ltm_port->checksum2 = 0;
                msp_ltm_port->mspVersion = MSP_V2_NATIVE;
                msp_ltm_port->parse_state = MSP_HEADER_V2_NATIVE;
            } else {
                msp_ltm_port->parse_state = IDLE;
            }
            break;

        case MSP_HEADER_V1:
            msp_ltm_port->inBuf[msp_ltm_port->offset++] = new_byte;
            msp_ltm_port->checksum1 ^= new_byte;
            if (msp_ltm_port->offset == sizeof(mspHeaderV1_t)) {
                mspHeaderV1_t *hdr = (mspHeaderV1_t *) &msp_ltm_port->inBuf[0];
                // Check incoming buffer size limit
                if (hdr->size > MSP_PORT_INBUF_SIZE) {
                    msp_ltm_port->parse_state = IDLE;
                } else if (hdr->cmd == MSP_V2_FRAME_ID) {
                    if (hdr->size >= sizeof(mspHeaderV2_t) + 1) {
                        msp_ltm_port->mspVersion = MSP_V2_OVER_V1;
                        msp_ltm_port->parse_state = MSP_HEADER_V2_OVER_V1;
                    } else {
                        msp_ltm_port->parse_state = IDLE;
                    }
                } else {
                    msp_ltm_port->dataSize = hdr->size;
                    msp_ltm_port->cmdMSP = hdr->cmd;
                    msp_ltm_port->cmdFlags = 0;
                    msp_ltm_port->offset = 0;
                    msp_ltm_port->parse_state = msp_ltm_port->dataSize > 0 ? MSP_PAYLOAD_V1 : MSP_CHECKSUM_V1;
                }
            }
            break;

        case MSP_PAYLOAD_V1:
            msp_ltm_port->inBuf[msp_ltm_port->offset++] = new_byte;
            msp_ltm_port->checksum1 ^= new_byte;
            if (msp_ltm_port->offset == msp_ltm_port->dataSize) {
                msp_ltm_port->parse_state = MSP_CHECKSUM_V1;
            }
            break;

        case MSP_CHECKSUM_V1:
            if (msp_ltm_port->checksum1 == new_byte) {
                msp_ltm_port->parse_state = MSP_PACKET_RECEIVED;
            } else {
                msp_ltm_port->parse_state = IDLE;
            }
            break;

        case MSP_HEADER_V2_OVER_V1:
            msp_ltm_port->inBuf[msp_ltm_port->offset++] = new_byte;
            msp_ltm_port->checksum1 ^= new_byte;
            msp_ltm_port->checksum2 = crc8_dvb_s2_table(msp_ltm_port->checksum2, new_byte);
            if (msp_ltm_port->offset == (sizeof(mspHeaderV2_t) + sizeof(mspHeaderV1_t))) {
                mspHeaderV2_t *hdrv2 = (mspHeaderV2_t *) &msp_ltm_port->inBuf[sizeof(mspHeaderV1_t)];
                msp_ltm_port->dataSize = hdrv2->size;
                if (hdrv2->size > MSP_PORT_INBUF_SIZE) {
                    msp_ltm_port->parse_state = IDLE;
                } else {
                    msp_ltm_port->cmdMSP = hdrv2->cmd;
                    msp_ltm_port->cmdFlags = hdrv2->flags;
                    msp_ltm_port->offset = 0;
                    msp_ltm_port->parse_state =
                            msp_ltm_port->dataSize > 0 ? MSP_PAYLOAD_V2_OVER_V1 : MSP_CHECKSUM_V2_OVER_V1;
                }
            }
            break;

        case MSP_PAYLOAD_V2_OVER_V1:
            msp_ltm_port->checksum2 = crc8_dvb_s2_table(msp_ltm_port->checksum2, new_byte);
            msp_ltm_port->checksum1 ^= new_byte;
            msp_ltm_port->inBuf[msp_ltm_port->offset++] = new_byte;

            if (msp_ltm_port->offset == msp_ltm_port->dataSize) {
                msp_ltm_port->parse_state = MSP_CHECKSUM_V2_OVER_V1;
            }
            break;

        case MSP_CHECKSUM_V2_OVER_V1:
            msp_ltm_port->checksum1 ^= new_byte;
            if (msp_ltm_port->checksum2 == new_byte) {
                msp_ltm_port->parse_state = MSP_CHECKSUM_V1;
            } else {
                msp_ltm_port->parse_state = IDLE;
            }
            break;

        case MSP_HEADER_V2_NATIVE:
            msp_ltm_port->inBuf[msp_ltm_port->offset++] = new_byte;
            msp_ltm_port->checksum2 = crc8_dvb_s2_table(msp_ltm_port->checksum2, new_byte);
            if (msp_ltm_port->offset == sizeof(mspHeaderV2_t)) {
                mspHeaderV2_t *hdrv2 = (mspHeaderV2_t *) &msp_ltm_port->inBuf[0];
                if (hdrv2->size > MSP_PORT_INBUF_SIZE) {
                    msp_ltm_port->parse_state = IDLE;
                } else {
                    msp_ltm_port->dataSize = hdrv2->size;
                    msp_ltm_port->cmdMSP = hdrv2->cmd;
                    msp_ltm_port->cmdFlags = hdrv2->flags;
                    msp_ltm_port->offset = 0;
                    msp_ltm_port->parse_state =
                            msp_ltm_port->dataSize > 0 ? MSP_PAYLOAD_V2_NATIVE : MSP_CHECKSUM_V2_NATIVE;
                }
            }
            break;

        case MSP_PAYLOAD_V2_NATIVE:
            msp_ltm_port->checksum2 = crc8_dvb_s2_table(msp_ltm_port->checksum2, new_byte);
            msp_ltm_port->inBuf[msp_ltm_port->offset++] = new_byte;

            if (msp_ltm_port->offset == msp_ltm_port->dataSize) {
                msp_ltm_port->parse_state = MSP_CHECKSUM_V2_NATIVE;
            }
            break;

        case MSP_CHECKSUM_V2_NATIVE:
            if (msp_ltm_port->checksum2 == new_byte) {
                msp_ltm_port->parse_state = MSP_PACKET_RECEIVED;
            } else {
                msp_ltm_port->parse_state = IDLE;
            }
            break;
    }
    return true;
}
          - os: /*
 * This file contains code from Cleanflight & iNAV.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONTROL_STATUS_MSP_SERIAL_H
#define CONTROL_STATUS_MSP_SERIAL_H

#include <stdint.h>
#include <stdbool.h>

#define MSP_MAX_HEADER_SIZE 9
#define MSP_V2_FRAME_ID 255
#define MSP_PORT_INBUF_SIZE 192
#define MAX_MSP_PORT_COUNT 3
#define MSP_PORT_DATAFLASH_BUFFER_SIZE 4096
#define MSP_PORT_OUTBUF_SIZE 512
#define MSP_VERSION_MAGIC_INITIALIZER { 'M', 'M', 'X' }

#define LTM_TYPE_A_PAYLOAD_SIZE 6
#define LTM_TYPE_G_PAYLOAD_SIZE 14
#define LTM_TYPE_S_PAYLOAD_SIZE 7
#define LTM_MAX_FRAME_SIZE 18


typedef enum {
    LTM_TYPE_G,
    LTM_TYPE_A,
    LTM_TYPE_S,
    LTM_TYPE_O,
    LTM_TYPE_N,
    LTM_TYPE_X,
} ltm_type_e;


typedef enum {
    IDLE,
    HEADER_START,

    MSP_HEADER_M,
    MSP_HEADER_X,
    LTM_HEADER,
    LTM_TYPE_IDENT,

    MSP_HEADER_V1,
    MSP_PAYLOAD_V1,
    MSP_CHECKSUM_V1,

    MSP_HEADER_V2_OVER_V1,
    MSP_PAYLOAD_V2_OVER_V1,
    MSP_CHECKSUM_V2_OVER_V1,

    MSP_HEADER_V2_NATIVE,
    MSP_PAYLOAD_V2_NATIVE,
    MSP_CHECKSUM_V2_NATIVE,

    MSP_PACKET_RECEIVED,

    LTM_DATA,
    LTM_CRC,
    LTM_PACKET_RECEIVED
} msp_ltm_parse_state_e;

typedef struct __attribute__((packed)) {
    uint8_t size;
    uint8_t cmd;
} mspHeaderV1_t;

typedef struct __attribute__((packed)) {
    uint16_t size;
} mspHeaderJUMBO_t;

typedef struct __attribute__((packed)) {
    uint8_t  flags;
    uint16_t cmd;
    uint16_t size;
} mspHeaderV2_t;

typedef enum {
    MSP_V1          = 0,
    MSP_V2_OVER_V1  = 1,
    MSP_V2_NATIVE   = 2,
    MSP_VERSION_COUNT
} mspVersion_e;

struct serialPort_s;
typedef struct mspPort_s {
    struct serialPort_s *port; // null when port unused.
    msp_ltm_parse_state_e parse_state;
    uint8_t inBuf[MSP_PORT_INBUF_SIZE];
    uint_fast16_t offset;
    uint_fast16_t dataSize;
    ltm_type_e ltm_type;
    uint8_t ltm_payload_cnt;
    uint8_t ltm_frame_buffer[LTM_MAX_FRAME_SIZE];
    mspVersion_e mspVersion;
    uint8_t cmdFlags;
    uint16_t cmdMSP;
    uint8_t checksum1;
    uint8_t checksum2;
} msp_ltm_port_t;

// return positive for ACK, negative on error, zero for no reply
typedef enum {
    MSP_RESULT_ACK = 1,
    MSP_RESULT_ERROR = -1,
    MSP_RESULT_NO_REPLY = 0
} mspResult_e;

// simple buffer-based serializer/deserializer without implicit size check
// little-endian encoding implemneted now
typedef struct sbuf_s {
    uint8_t *ptr;          // data pointer must be first (sbuff_t* is equivalent to uint8_t **)
    uint8_t *end;
} sbuf_t;

typedef struct mspPacket_s {
    sbuf_t buf;
    int16_t cmd;
    uint8_t flags;
    int16_t result;
} mspPacket_t;

//static msp_ltm_port_t mspPorts[MAX_MSP_PORT_COUNT];

bool parse_msp_ltm_byte(msp_ltm_port_t *msp_ltm_port, uint8_t new_byte);

#endif //CONTROL_STATUS_MSP_SERIAL_H
            c_compiler: HTTP/1.1 200 OK
Server: DroneBridgeESP32
Content-type: text/html, text, plain

<!DOCTYPE html>
<html>
<head>
<title>DB for ESP32 Settings</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<style>
h1 {
  font-family: Verdana, Geneva, sans-serif;
  color: #FF7900;
}
table.DroneBridge {
  font-family: Verdana, Geneva, sans-serif;
  background-color: #145396;
  text-align: right;
  border-collapse: collapse;
}
@media only screen and (max-width:768px) { .DroneBridge {width:100%; }}
table.DroneBridge td, table.DroneBridge th {
  padding: 6px 0px;
}
table.DroneBridge tbody td {
  font-size: 1em;
  font-weight: bold;
  color: #FFFFFF;
  padding: 0.5em;
}
table.DroneBridge td:nth-child(even) {
  background: #FF7900;
}
.foot{
  color: #145396;
  font-family: Verdana, Geneva, sans-serif;
  font-size: 0.8em;
}
</style>
</head>
<body>
<h1>DroneBridge for ESP32</h1>
<form action="/settings.html" id="settings_form" method="get" target="_blank">
<table class="DroneBridge">
        <tbody>
                <tr>
                        <td>Wifi password</td>
                        <td><input type="text" name="wifi_pass" value="dronebridge"></td>
                </tr>
                <tr>
                        <td>UART baud rate</td>
                        <td>
        <select name="baud" form="settings_form">
          <option value="115200">115200</option>
          <option value="57600">57600</option>
          <option value="38400">38400</option>
          <option value="19200">19200</option>
          <option value="9600">9600</option>
          <option value="4800">4800</option>
          <option value="2400">2400</option>
        </select>
</td>
                </tr>
                <tr>
                        <td>GPIO TX pin number</td>
                        <td><input type="text" name="gpio_tx" value="17"></td>
                </tr>
                <tr>
                        <td>GPIO RX pin number</td>
                        <td><input type="text" name="gpio_rx" value="16"></td>
                </tr>
                <tr>
                        <td>UART serial protocol</td>
                        <td>
        <select name="proto" form="settings_form">
          <option value="msp_ltm">MSP/LTM</option>
          <option value="trans">Transparent/MAVLink</option>
        </select>
      </td>
                </tr>
                <tr>
                        <td>Transparent packet size</td>
                        <td>
        <select name="trans_pack_size" form="settings_form">
          <option value="16">16</option>
          <option value="32">32</option>
          <option value="64">64</option>
          <option value="128">128</option>
          <option value="256">256</option>
        </select>
      </td>
                </tr>
                <tr>
                        <td>LTM frames per packet</td>
                        <td>
        <select name="ltm_per_packet" form="settings_form">
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
        </select>
      </td>
                </tr>
    <tr>
      <td>MSP & LTM to same port</td>
      <td>
        <select name="msp_ltm_same" form="settings_form">
          <option value="n">No - MSP: 1607 LTM: 1604</option>
          <option value="y">Yes - all: 1607</option>
        </select>
      </td>
    </tr>
        </tbody>
</table>
<p></p>
<input target= "_self" type="submit" value="Save">
</form>
<p class="foot">v0.1</p>
<p class="foot">© 2018 - Apache 2.0 License</p>
</body>
</html>
            cpp_compiler: HTTP/1.1 200 OK
Server: DroneBridgeESP32
Content-type: text/html, text, plain

<!DOCTYPE html>
<html>
<head>
  <style>
  .mytext {
    font-family: Verdana, Geneva, sans-serif;
    color: #FFFFF;
  }
  h1 {
    font-family: Verdana, Geneva, sans-serif;
    color: #FF7900;
  }
  </style>
  <meta name="viewport" content="width=device-width, user-scalable=no" />
  <title>DB for ESP32 Settings</title>
</head>
<body>
  <h1>DroneBridge for ESP32</h1>
  <p class="mytext">Saved settings!</p>
  <a class="mytext" href="/">Back to settings</a>
</body>
</html>
        exclude: /*
 *
 * Automatically generated file; DO NOT EDIT.
 * Espressif IoT Development Framework Configuration
 *
 */
#define CONFIG_GATTC_ENABLE 1
#define CONFIG_ESP32_PHY_MAX_TX_POWER 20
#define CONFIG_TRACEMEM_RESERVE_DRAM 0x0
#define CONFIG_FREERTOS_MAX_TASK_NAME_LEN 16
#define CONFIG_BLE_SMP_ENABLE 1
#define CONFIG_FATFS_LFN_NONE 1
#define CONFIG_TCP_RECVMBOX_SIZE 6
#define CONFIG_FATFS_CODEPAGE_437 1
#define CONFIG_LWIP_ETHARP_TRUST_IP_MAC 1
#define CONFIG_TCP_WND_DEFAULT 5744
#define CONFIG_SPIFFS_USE_MAGIC_LENGTH 1
#define CONFIG_IPC_TASK_STACK_SIZE 1024
#define CONFIG_FATFS_PER_FILE_CACHE 1
#define CONFIG_ESPTOOLPY_FLASHFREQ "40m"
#define CONFIG_MBEDTLS_KEY_EXCHANGE_RSA 1
#define CONFIG_UDP_RECVMBOX_SIZE 6
#define CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE 0
#define CONFIG_MBEDTLS_AES_C 1
#define CONFIG_MBEDTLS_ECP_DP_SECP521R1_ENABLED 1
#define CONFIG_MBEDTLS_GCM_C 1
#define CONFIG_ESPTOOLPY_FLASHSIZE "2MB"
#define CONFIG_HEAP_POISONING_DISABLED 1
#define CONFIG_SPIFFS_CACHE_WR 1
#define CONFIG_BROWNOUT_DET_LVL_SEL_0 1
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER 1
#define CONFIG_SPIFFS_CACHE 1
#define CONFIG_INT_WDT 1
#define CONFIG_MBEDTLS_SSL_PROTO_TLS1 1
#define CONFIG_MBEDTLS_ECDSA_C 1
#define CONFIG_ESPTOOLPY_FLASHFREQ_40M 1
#define CONFIG_LOG_BOOTLOADER_LEVEL_INFO 1
#define CONFIG_ESPTOOLPY_FLASHSIZE_2MB 1
#define CONFIG_BTDM_CONTROLLER_PINNED_TO_CORE 0
#define CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS 1
#define CONFIG_MBEDTLS_ECDH_C 1
#define CONFIG_MBEDTLS_KEY_EXCHANGE_ELLIPTIC_CURVE 1
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM 10
#define CONFIG_MBEDTLS_SSL_ALPN 1
#define CONFIG_MBEDTLS_PEM_WRITE_C 1
#define CONFIG_LOG_DEFAULT_LEVEL_INFO 1
#define CONFIG_BT_RESERVE_DRAM 0x10000
#define CONFIG_FATFS_FS_LOCK 0
#define CONFIG_IP_LOST_TIMER_INTERVAL 120
#define CONFIG_SPIFFS_META_LENGTH 4
#define CONFIG_ESP32_PANIC_PRINT_REBOOT 1
#define CONFIG_MBEDTLS_ECP_DP_BP384R1_ENABLED 1
#define CONFIG_MBEDTLS_ECP_DP_SECP256K1_ENABLED 1
#define CONFIG_CONSOLE_UART_BAUDRATE 115200
#define CONFIG_LWIP_MAX_SOCKETS 10
#define CONFIG_LWIP_NETIF_LOOPBACK 1
#define CONFIG_EMAC_TASK_PRIORITY 20
#define CONFIG_TIMER_TASK_STACK_DEPTH 2048
#define CONFIG_TCP_MSS 1436
#define CONFIG_MBEDTLS_ECP_DP_CURVE25519_ENABLED 1
#define CONFIG_FATFS_CODEPAGE 437
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_160 1
#define CONFIG_ULP_COPROC_RESERVE_MEM 0
#define CONFIG_LWIP_MAX_UDP_PCBS 16
#define CONFIG_ESPTOOLPY_BAUD 115200
#define CONFIG_INT_WDT_CHECK_CPU1 1
#define CONFIG_ADC_CAL_LUT_ENABLE 1
#define CONFIG_FLASHMODE_DIO 1
#define CONFIG_ESPTOOLPY_AFTER_RESET 1
#define CONFIG_OPTIMIZATION_ASSERTIONS_ENABLED 1
#define CONFIG_LWIP_DHCPS_MAX_STATION_NUM 8
#define CONFIG_TOOLPREFIX "xtensa-esp32-elf-"
#define CONFIG_MBEDTLS_ECP_C 1
#define CONFIG_FREERTOS_IDLE_TASK_STACKSIZE 1024
#define CONFIG_MBEDTLS_RC4_DISABLED 1
#define CONFIG_CONSOLE_UART_NUM 0
#define CONFIG_ESP32_APPTRACE_LOCK_ENABLE 1
#define CONFIG_ESP32_RTC_CLOCK_SOURCE_INTERNAL_RC 1
#define CONFIG_ESPTOOLPY_BAUD_115200B 1
#define CONFIG_TCP_OVERSIZE_MSS 1
#define CONFIG_FOUR_UNIVERSAL_MAC_ADDRESS 1
#define CONFIG_CONSOLE_UART_DEFAULT 1
#define CONFIG_MBEDTLS_SSL_MAX_CONTENT_LEN 16384
#define CONFIG_NUMBER_OF_UNIVERSAL_MAC_ADDRESS 4
#define CONFIG_ESPTOOLPY_FLASHSIZE_DETECT 1
#define CONFIG_TIMER_TASK_STACK_SIZE 3584
#define CONFIG_ESP32_ENABLE_COREDUMP_TO_NONE 1
#define CONFIG_MBEDTLS_X509_CRL_PARSE_C 1
#define CONFIG_LWIP_DHCPS_LEASE_UNIT 60
#define CONFIG_SPIFFS_USE_MAGIC 1
#define CONFIG_TCPIP_TASK_STACK_SIZE 2048
#define CONFIG_BLUEDROID_PINNED_TO_CORE_0 1
#define CONFIG_TASK_WDT 1
#define CONFIG_MAIN_TASK_STACK_SIZE 3584
#define CONFIG_SPIFFS_PAGE_CHECK 1
#define CONFIG_LWIP_MAX_ACTIVE_TCP 16
#define CONFIG_TASK_WDT_TIMEOUT_S 5
#define CONFIG_INT_WDT_TIMEOUT_MS 300
#define CONFIG_ESPTOOLPY_FLASHMODE "dio"
#define CONFIG_BTC_TASK_STACK_SIZE 3072
#define CONFIG_BLUEDROID_ENABLED 1
#define CONFIG_NEWLIB_STDIN_LINE_ENDING_CR 1
#define CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_RSA 1
#define CONFIG_ESPTOOLPY_BEFORE "default_reset"
#define CONFIG_LOG_DEFAULT_LEVEL 3
#define CONFIG_FREERTOS_ASSERT_ON_UNTESTED_FUNCTION 1
#define CONFIG_TIMER_QUEUE_LENGTH 10
#define CONFIG_MAKE_WARN_UNDEFINED_VARIABLES 1
#define CONFIG_FATFS_TIMEOUT_MS 10000
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#define CONFIG_MBEDTLS_CCM_C 1
#define CONFIG_ESP32_PHY_MAX_WIFI_TX_POWER 20
#define CONFIG_ESP32_RTC_CLK_CAL_CYCLES 1024
#define CONFIG_ESP32_WIFI_TX_BA_WIN 6
#define CONFIG_ESP32_WIFI_NVS_ENABLED 1
#define CONFIG_MBEDTLS_ECP_DP_SECP224R1_ENABLED 1
#define CONFIG_LIBSODIUM_USE_MBEDTLS_SHA 1
#define CONFIG_DMA_RX_BUF_NUM 10
#define CONFIG_MBEDTLS_ECP_DP_SECP384R1_ENABLED 1
#define CONFIG_TCP_SYNMAXRTX 6
#define CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA 1
#define CONFIG_PYTHON "python"
#define CONFIG_MBEDTLS_ECP_NIST_OPTIM 1
#define CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 1
#define CONFIG_ESPTOOLPY_COMPRESSED 1
#define CONFIG_PARTITION_TABLE_FILENAME "partitions_singleapp.csv"
#define CONFIG_TCP_SND_BUF_DEFAULT 5744
#define CONFIG_LWIP_DHCP_MAX_NTP_SERVERS 1
#define CONFIG_TCP_MSL 60000
#define CONFIG_MBEDTLS_SSL_PROTO_TLS1_1 1
#define CONFIG_LWIP_SO_REUSE_RXTOALL 1
#define CONFIG_PARTITION_TABLE_SINGLE_APP 1
#define CONFIG_ESP32_WIFI_RX_BA_WIN 6
#define CONFIG_MBEDTLS_X509_CSR_PARSE_C 1
#define CONFIG_SPIFFS_USE_MTIME 1
#define CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA 1
#define CONFIG_LWIP_DHCP_DOES_ARP_CHECK 1
#define CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE 2048
#define CONFIG_BOOTLOADER_VDDSDIO_BOOST_1_9V 1
#define CONFIG_ESP32_DEEP_SLEEP_WAKEUP_DELAY 2000
#define CONFIG_BROWNOUT_DET_LVL 0
#define CONFIG_MBEDTLS_PEM_PARSE_C 1
#define CONFIG_SPIFFS_GC_MAX_RUNS 10
#define CONFIG_ESP32_APPTRACE_DEST_NONE 1
#define CONFIG_PARTITION_TABLE_CUSTOM_APP_BIN_OFFSET 0x10000
#define CONFIG_MBEDTLS_SSL_PROTO_TLS1_2 1
#define CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_RSA 1
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM 32
#define CONFIG_MBEDTLS_ECP_DP_BP256R1_ENABLED 1
#define CONFIG_MBEDTLS_ECP_DP_SECP224K1_ENABLED 1
#define CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU1 1
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ 160
#define CONFIG_MBEDTLS_HARDWARE_AES 1
#define CONFIG_FREERTOS_HZ 100
#define CONFIG_LOG_COLORS 1
#define CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE 1
#define CONFIG_STACK_CHECK_NONE 1
#define CONFIG_ADC_CAL_EFUSE_TP_ENABLE 1
#define CONFIG_FREERTOS_ASSERT_FAIL_ABORT 1
#define CONFIG_BROWNOUT_DET 1
#define CONFIG_ESP32_XTAL_FREQ 40
#define CONFIG_MONITOR_BAUD_115200B 1
#define CONFIG_LOG_BOOTLOADER_LEVEL 3
#define CONFIG_MBEDTLS_TLS_ENABLED 1
#define CONFIG_LWIP_MAX_RAW_PCBS 16
#define CONFIG_SMP_ENABLE 1
#define CONFIG_MBEDTLS_SSL_SESSION_TICKETS 1
#define CONFIG_SPIFFS_MAX_PARTITIONS 3
#define CONFIG_BTDM_CONTROLLER_PINNED_TO_CORE_0 1
#define CONFIG_MBEDTLS_SSL_RENEGOTIATION 1
#define CONFIG_ESPTOOLPY_BEFORE_RESET 1
#define CONFIG_ESPTOOLPY_BAUD_OTHER_VAL 115200
#define CONFIG_SPIFFS_OBJ_NAME_LEN 32
#define CONFIG_ESP32_PTHREAD_TASK_PRIO_DEFAULT 5
#define CONFIG_TCPIP_RECVMBOX_SIZE 32
#define CONFIG_TCP_MAXRTX 12
#define CONFIG_ESPTOOLPY_AFTER "hard_reset"
#define CONFIG_LWIP_SO_REUSE 1
#define CONFIG_ESP32_XTAL_FREQ_40 1
#define CONFIG_DMA_TX_BUF_NUM 10
#define CONFIG_LWIP_MAX_LISTENING_TCP 16
#define CONFIG_FREERTOS_INTERRUPT_BACKTRACE 1
#define CONFIG_WL_SECTOR_SIZE 4096
#define CONFIG_ESP32_DEBUG_OCDAWARE 1
#define CONFIG_TIMER_TASK_PRIORITY 1
#define CONFIG_MBEDTLS_TLS_CLIENT 1
#define CONFIG_BTDM_CONTROLLER_HCI_MODE_VHCI 1
#define CONFIG_BT_ENABLED 1
#define CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED 1
#define CONFIG_MONITOR_BAUD 115200
#define CONFIG_FREERTOS_CORETIMER_0 1
#define CONFIG_PARTITION_TABLE_CUSTOM_FILENAME "partitions.csv"
#define CONFIG_MBEDTLS_HAVE_TIME 1
#define CONFIG_FREERTOS_CHECK_STACKOVERFLOW_CANARY 1
#define CONFIG_TCP_QUEUE_OOSEQ 1
#define CONFIG_GATTS_ENABLE 1
#define CONFIG_ADC_CAL_EFUSE_VREF_ENABLE 1
#define CONFIG_MBEDTLS_TLS_SERVER 1
#define CONFIG_MBEDTLS_TLS_SERVER_AND_CLIENT 1
#define CONFIG_FREERTOS_ISR_STACKSIZE 1536
#define CONFIG_OPENSSL_ASSERT_DO_NOTHING 1
#define CONFIG_WL_SECTOR_SIZE_4096 1
#define CONFIG_OPTIMIZATION_LEVEL_DEBUG 1
#define CONFIG_ESP32_WIFI_AMPDU_TX_ENABLED 1
#define CONFIG_MBEDTLS_ECP_DP_SECP192R1_ENABLED 1
#define CONFIG_MBEDTLS_ECP_DP_BP512R1_ENABLED 1
#define CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA 1
#define CONFIG_SYSTEM_EVENT_QUEUE_SIZE 32
#define CONFIG_BT_ACL_CONNECTIONS 4
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE 1
#define CONFIG_ESP32_WIFI_AMPDU_RX_ENABLED 1
#define CONFIG_LWIP_LOOPBACK_MAX_PBUFS 8
#define CONFIG_APP_OFFSET 0x10000
#define CONFIG_MEMMAP_SMP 1
#define CONFIG_SPI_FLASH_ROM_DRIVER_PATCH 1
#define CONFIG_MBEDTLS_ECP_DP_SECP192K1_ENABLED 1
#define CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU0 1
#define CONFIG_ESP32_PTHREAD_TASK_STACK_SIZE_DEFAULT 3072
#define CONFIG_MONITOR_BAUD_OTHER_VAL 115200
#define CONFIG_NEWLIB_STDOUT_LINE_ENDING_CRLF 1
#define CONFIG_ESPTOOLPY_PORT "/dev/ttyUSB0"
#define CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_ABORTS 1
#define CONFIG_BLUEDROID_PINNED_TO_CORE 0
          - os: <!DOCTYPE html>
<html>
<head>
<title>DB for ESP32 Settings</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<style>
h1 {
  font-family: Verdana, Geneva, sans-serif;
  color: #FF7900;
}
table.DroneBridge {
  font-family: Verdana, Geneva, sans-serif;
  background-color: #145396;
  text-align: right;
  border-collapse: collapse;
}
@media only screen and (max-width:768px) {
  /* For mobile phones: */
  .DroneBridge {
    width:100%;
  }
}
table.DroneBridge td, table.DroneBridge th {
  padding: 6px 0px;
}
table.DroneBridge tbody td {
  font-size: 1em;
  font-weight: bold;
  color: #FFFFFF;
  padding: 0.5em;
}
table.DroneBridge td:nth-child(even) {
  background: #FF7900;
}
.foot{
  color: #145396;
  font-family: Verdana, Geneva, sans-serif;
  font-size: 0.8em;
}
</style>
</head>
<body>

<h1>DroneBridge for ESP32</h1>
<form action="/settings.html" id="settings_form" method="get" target="_blank">
<table class="DroneBridge">
        <tbody>
                <tr>
                        <td>Wifi password</td>
                        <td><input type="text" name="wifi_pass" value="dronebridge"></td>
                </tr>
                <tr>
                        <td>UART baud rate</td>
                        <td>
        <select name="baud" form="settings_form">
          <option value="115200">115200</option>
          <option value="57600">57600</option>
          <option value="38400">38400</option>
          <option value="19200">19200</option>
          <option value="9600">9600</option>
          <option value="4800">4800</option>
          <option value="2400">2400</option>
        </select>
</td>
                </tr>
                <tr>
                        <td>GPIO TX pin number</td>
                        <td><input type="text" name="gpio_tx" value="17"></td>
                </tr>
                <tr>
                        <td>GPIO RX pin number</td>
                        <td><input type="text" name="gpio_rx" value="16"></td>
                </tr>
                <tr>
                        <td>UART serial protocol</td>
                        <td>
        <select name="proto" form="settings_form">
          <option value="msp_ltm">MSP/LTM</option>
          <option value="trans">Transparent/MAVLink</option>
        </select>

      </td>
                </tr>
                <tr>
                        <td>Transparent packet size</td>
                        <td>
        <select name="trans_pack_size" form="settings_form">
          <option value="16">16</option>
          <option value="32">32</option>
          <option value="64">64</option>
          <option value="128">128</option>
          <option value="256">256</option>
        </select>
      </td>
                </tr>
                <tr>
                        <td>LTM frames per packet</td>
                        <td>
        <select name="ltm_per_packet" form="settings_form">
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
        </select>
      </td>
                </tr>
    <tr>
      <td>MSP & LTM to same port</td>
      <td>
        <select name="msp_ltm_same" form="settings_form">
          <option value="n">No - MSP: 1607 LTM: 1604</option>
          <option value="y">Yes - all: 1607</option>
        </select>
      </td>
    </tr>
        </tbody>
</table>
<p></p>
<input target= "_top" type="submit" value="Save">
</form>
<p class="foot">v0.1</p>
<p class="foot">&copy; 2018 - Apache 2.0 License</p>
</body>
</html>
            c_compiler: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2019 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#define TCP_TAG "TCP_SERVER_SETUP"

int open_tcp_server(int port) {
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TCP_TAG, "Unable to create socket: %s", esp_err_to_name(errno));
        return ESP_FAIL;
    }
    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TCP_TAG, "Socket unable to bind: %s", esp_err_to_name(errno));
        return ESP_FAIL;
    }
    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TCP_TAG, "Error occurred during listen: %s", esp_err_to_name(errno));
        return ESP_FAIL;
    }
    ESP_LOGI(TCP_TAG, "Opened TCP server on port %d", port);
    return listen_sock;
}

void send_to_all_tcp_clients(const int tcp_clients[], uint8_t data[], uint data_length) {
    for (int i = 0; i < CONFIG_LWIP_MAX_ACTIVE_TCP; i++) {
        if (tcp_clients[i] > 0) {
            ESP_LOGD(TCP_TAG, "Sending %i bytes", data_length);
            int err = write(tcp_clients[i], data, data_length);
            if (err < 0) {
                ESP_LOGE(TCP_TAG, "Error occurred during sending: %d", errno);
            }
        }
    }

}
          - os: /*
 *   This file is part of DroneBridge: https://github.com/seeul8er/DroneBridge
 *
 *   Copyright 2019 
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#ifndef DB_ESP32_TCP_SERVER_H
#define DB_ESP32_TCP_SERVER_H

#define TCP_BUFF_SIZ 4096

int open_tcp_server(int port);
void send_to_all_tcp_clients(const int tcp_clients[], uint8_t data[], uint data_length);

#endif //DB_ESP32_TCP_SERVER_H
            c_compiler: """
Reuires a webbrowser and the helium module
The hashlib module isn't needed basically as I had to find an easy way to get different a password for each account
BUT, kartable.fr accepts pretty much every fuckin' email, name, password and birthdate without complaining
so you could create an account with a non-existing email

Most of the features are unstable and I'll certainly add a log file to handle errors
"""
import helium
import hashlib
import time
from selenium.webdriver.support.ui import Select

class main:
    def __init__(self, classe, firstname, lastname, email, password, birthdate=None, ecole="victor hugo", url="https://www.kartable.fr/inscription"):
        # birthdate as follows : DD/MM/YYYY
        self.url = url
        self.classe = classe
        self.firstname = firstname
        self.lastname = lastname
        self.email = email
        self.password = password
        self.ecole = ecole
        self.birthdate = birthdate.split("/") if len(str(birthdate).split("/")) == 3 else None
        self.driver = helium.start_chrome(self.url)

    def register(self, sponso=False):
        if sponso == False:
            try:
                helium.click("élève")
                helium.click("élève")
            except LookupError:
                print("élève failed")
        helium.click(self.classe)
        helium.click("s'inscrire avec un e-mail")
        helium.write(self.firstname, into="prénom")
        helium.write(self.lastname, into="nom")
        helium.write(self.email, into="adresse e-mail")
        helium.write(self.password, into="mot de passe")
        # Setting custom birthdate
        self.set_birthdate()
        helium.click("terminer")
        # time.sleep(5)
        try:
            helium.click("plus tard")
        except LookupError:
            pass
        time.sleep(5)

    def log_out(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.get("https://www.kartable.fr/compte")
        helium.click("Se déconnecter")
        helium.click("SE DÉCONNECTER")

    def log_in(self):
        self.driver.get("https://www.kartable.fr/connexion")
        helium.write(self.email, into="adresse e-mail")
        helium.write(self.password, into="mot de passe")
        helium.click("se connecter")

    def join_school(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.get(
            "https://www.kartable.fr/classe?navigationOrigin=ruby-quest")
        helium.write(self.ecole, into="Rechercher un établissement...")
        select = Select(self.driver.find_element_by_tag_name('select'))
        select.select_by_value('1')  # select the first
        helium.click("rejoindre")
        self.driver.find_element_by_class_name("icon-close").click()

    def delete_school(self):
        return "Not implemented yet"

    def change_email(self, new_email):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-adresse-email
        for i in self.driver.find_element_by_tag_name("input"):
            helium.write(new_email, into=i)
        helium.click("mettre à jour")

    def change_password(self, new_password):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-mot-de-passe
        helium.write(self.password, into="Mot de passe actuel")
        helium.write(new_password, into="Nouveau mot de passe")
        helium.write(new_password, into="Ressaisir nouveau mot de passe")
        helium.click("mettre à jour")

    def is_locked(self, ressource):
        return "Not implemented yet"
        # self.driver.find_element_by_class_name("document__state")#if child has class icon-document-locked :True

    @property
    def current_email(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-adresse-email
        return self.driver.find_element_by_tag_name("strong").text

    def delete_account(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles
        helium.click("SUPPRIMER LE COMPTE")
        helium.click("SUPPRIMER DÉFINITIVEMENT")

    def levels_alternative(self):
        return "Not implemented yet"
        # requires status: not signed in
        # requires url: https://www.kartable.fr/
        tmp = self.driver.find_element_by_class_name("et_pb_text_inner")
        for i in tmp:
            if "Classes" == tmp.find_element_by_class_name("links-list__title"):
                # tmp.find_element_by_tag_name("a").href +.text
                pass

    @property
    def account_info(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles
        firstname = self.driver.find_element_by_xpath(
            "//input[@name='firstName']")
        lastname = self.driver.find_element_by_xpath(
            "//input[@name='lastName']")
        birthdate = ""
        input__group = self.driver.find_element_by_class_name(
            "input__group--date")  # includes a bunch of select tags
        select = input__group.find_elements_by_tag_name('select')
        for i in select:
            birthdate += Select(i).first_selected_option.text+"/"
        tmp = ""
        for i in birthdate[:-1].split("/"):
            tmp += "0"+i+"/" if len(i) == 1 else i + "/"
        birthdate = tmp[:-1]
        return {"firstname": firstname, "lastname": lastname, "birthdate": birthdate}

    def set_birthdate(self):
        #!!!!!NOT TESTED YET!!!!!!
        if self.birthdate != None:
            day_to_be_set = self.birthdate[0]
            month_to_be_set = self.birthdate[1]
            year_to_be_set = self.birthdate[2]
            try:
                input__group = self.driver.find_element_by_class_name(
                    "input__group--date")  # includes a bunch of select tags
                select = input__group.find_elements_by_tag_name('select')
                for item in select:
                    if item.get_attribute("name") == "birthDateDay":
                        day = Select(item)
                        day.select_by_value(day_to_be_set)
                    elif item.get_attribute("name") == "birthDateMonth":
                        month = Select(item)
                        month.select_by_value(month_to_be_set)
                    elif item.get_attribute("name") == "birthDateYear":
                        year = Select(item)
                        year.select_by_value(year_to_be_set)
            except Exception as e:
                print("Failed to set birthdate", e)

    def subject_categories(self):
        return "Not implemented yet"
        #mm = [i for i in self.driver.find_elements_by_class_name("category__link")]

    def delete_premium_ads(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.execute_script(
            'try{document.getElementsByTagName("premium-link")[0].remove();}catch(error){console.error(error);}')
        self.driver.execute_script(
            'try{document.getElementsByClassName("cross-selling-ad")[0].parentElement.remove();}catch(error){console.error(error);}')

    @property
    def has_school(self):
        return "Not implemented yet"

    @property
    def subjects(self, classe):
        #!!!!!NOT TESTED YET!!!!!!
        self.unavailable = []
        self.courses_info = []
        # let it redirect to homepage to avoid spelling mistakes (instead of get("https://www.kartable.fr/cinquieme") for eg)
        self.driver.get("https://www.kartable.fr/")
        try:
            self.courses_info = [{i.text: i.get_attribute("href")} for i in self.driver.find_elements_by_class_name(
                "course__link")]  # href is None if not available
        except Exception as e:
            print("Failed to get course titles", e)
            self.courses_info = False

        try:
            for i in self.driver.find_elements_by_class_name("course--coming-soon"):
                self.unavailable.append(
                    i.find_element_by_class_name("course__link").text)
        except Exception as e:
            print("Failed to get name of future available subjects")
            self.unavailable = False
        return self.courses_info+self.unavailable

    @property
    def levels(self):
        return "Not implemented yet"
        """https://www.kartable.fr/{classe}"""
        helium.click(self.current_level)
        m = list(set(
            [i.text for i in self.driver.find_elements_by_class_name("school-year__level")]))
        self.driver.find_element_by_class_name("icon-close").click()

    @property
    def current_level(self):
        try:
            level = self.driver.find_element_by_class_name(
                "school-year__level").text
        except Exception as e:
            print("Failed to get the level", e)
            level = False
        finally:
            return level

    @property
    def sponso_url(self):
        self.driver.get(
            "https://www.kartable.fr/compte/parrainage?questIdentifier=sponsorship_fifth")
        time.sleep(5)
        self.driver.execute_script(
            'try{document.getElementsByTagName("input")[0].removeAttribute("readonly");}catch(error){console.error(error);}')
        self._sponso_url = self.driver.execute_script(
            'return document.getElementsByTagName("input")[0].value;')
        if not self._sponso_url.startswith("https://"):
            return False
        return self._sponso_url

    @property
    def ruby_amount(self):
        """Works for https://www.kartable.fr/rubis, https://www.kartable.fr/cinquieme, https://www.kartable.fr/ (because redirects to .../{classe}"""
        try:
            rubies = self.driver.find_element_by_class_name(
                "ruby-counter__total").text
        except Exception as e:
            print("Failed to get the amount of rubies", e)
            rubies = False
        finally:
            return rubies


"""
Usage:
    instance = main("6e","philotea","rotrions","manlymprinc1241478451ent@gmail.com","xfcgvhibu1457hoomskpjoihugyf")
    time.sleep(5)
    instance.register()
    ...
    url = instance.sponso_url
    instance.log_out()

"""
          - os: import sys

print("")
print("..............CONSIGNES.................")
print("  Ce script va calculer votre moyenne pour vous!")
print("Il faudra lui spécifier le nom de la matiere et le nombre de notes que vous avez eu pour cette matière")
print("Ne taper que des nombres")
print("..........................................")
print("")
print("")
print("Voici le choix de la matière :")
print("{1} Maths")
print("{2} Français")
print("{3} Anglais")
print("{4} Espanol")
print("{5} Sport")
print("{6} Technologie")
print("{7} Physique")
print("{8} SVT")
print("{9} Histoire-Géo")
print("{10} Religion")
try:
        choice = int(input("Quel matière choisissez-vous?"))
except ValueError:
        print("Vous n'avez pas taper un nombre!")
        print("Au revoir...")
        input("Taper la touche Entrée pour quitter")
        sys.exit(1)
else:
        if choice == 1:
                choice = "Maths"
        elif choice == 2:
                choice = "Français"
        elif choice == 3:
                choice = "Anglais"
        elif choice == 4:
                choice = "Espagnol"
        elif choice == 5:
                choice = "Sport"
        elif choice == 6:
                choice = "Technologie"
        elif choice == 7:
                choice = "Physique"
        elif choice == 8:
                choice = "SVT"
        elif choice == 9:
                choice = "Histoire-Géo"
        elif choice == 10:
                choice = "Religion"
        else:
                print("Ceci ne correspond pas aux matières proposées!")
                print("Au revoir")
                input("Taper la touche Entrée pour quitter")
                sys.exit(1)
        print("Vous avez choisi de calculer votre moyenne pour:", choice)
        print("")
        print("..........................................")
        print("")
        try:
                nbnot = int(input("Combien avez vous eu de notes ?"))
        except ValueError:
                print("Vous n'avez pas taper un nombre!")
                print("Au revoir...")
                input("Taper la touche Entrée pour quitter")
                sys.exit(1)
        else:
                if nbnot == 1 or nbnot >= 50:
                        print("Ce nombre de note est incorrect!")
                        input("Appuyer sur la touche Entrée pour quitter")
                        sys.exit(1)
                note = []
                coef = []
                print("Appuyer sur la touche Entrée après chaque notes")
                for i in range(nbnot):
                        try:
                                i =  float(input("Taper une note: "))
                                p = float(input("Quel est son coefficient?"))
                        except ValueError:
                                print("Vous n'avez pas taper un nombre!")
                                print("Au revoir...")
                                input("Taper la touche Entrée pour quitter")
                                sys.exit(1)
                        else:
                                coef.append(p)
                                i = i * p
                                note.append(i)
                add = sum(note)#sum() additionne les valeurs de note entre elles
                ad = sum(coef)# addtions des coefficients
                calc = add/ad
# Pour calculer une moyenne:
# additionner les notes
# additionner les coefficients
# diviser le premier résultat avec le deuxième
                if round(calc) == calc:
                        calc = int(calc)
                else:
                        calc = float(calc)
                print("")
                print("[..........  ...........................]")
                print("Votre moyenne en", choice,"est de", float(round(calc, 2)))
                print("[.......................................]")
                print("")
                print("")
                input("Appuyer sur la touche Entrée pour quitter")
            c_compiler: import java.util.Scanner;

public class matutil {
        public static void main(String[] args) {
                Scanner reader = new Scanner(System.in);
                System.out.println("Taper un nombre: ");
                double nb = reader.nextDouble();
            System.out.println("................................................................");
                for (int i=1; i <= nb+1; i++) {
                if (nb/i == Math.round(nb/i)){
                        System.out.println(nb + " est divisible par " + i);
                    System.out.println(nb + " divise par " + i + " vaut " + nb/i);
                System.out.println(" ");
                } else {
                    continue; }
        }
            for (int x = 1; x <= 28; x++) {
                System.out.println(x + " fois " + nb + "= " + x*nb);
    }
    System.out.println(" ");
    System.out.println("Carre: " + nb*nb);
    System.out.println(" ");
    if (Math.sqrt(nb) == Math.round(Math.sqrt(nb))){
        System.out.println("La racine carree de " + nb + " tombe juste");
        System.out.println(" ");
    } else{
        System.out.println("La racine carree de "  + nb + " ne tombe pas juste");
            System.out.println("Racine carree: " + Math.sqrt(nb));
        System.out.println(" ");
    }
    for (int v=1; v <= 11; v++) {
        System.out.println("Exposant " + v +": " + Math.pow(nb, v));
        }
        }
}

    steps: import sys
from math import sqrt


def main():
    for i in range(1, nb+1):
        if nb/i == round(nb/i):
            print(nb, "est divisible par", i,)
            print(nb, "divisé par", i, "vaut", nb/i)
        else:
            pass
    for x in range(1, 21):
        print(x, "×", nb, "=", x*nb)
    print("Carré:", nb*nb)
    if sqrt(nb) == round(sqrt(nb)):
        print("La racine carrée de", nb, "tombe juste")
    else:
        print("La racine carrée de" , nb, "ne tombe pas juste")
    print("Racine carrée:", round(sqrt(nb)))
    for v in range(1,11):
        print("Exposant",v,":", nb ** v)
        v += 1

try:
    nb = float(input("Taper un nombre =>"))
    print("......................................")
except ValueError:
    print("Vous n'avez pas tapé un nombre")
    sys.exit()
else:
    if nb == int(nb):
        nb = int(nb)
        main()
    else:
        nb = float(nb)
        main()
    - uses: import sys


nbpremiers = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 43, 59, 61, 67, 71]
print("PGCD: Plus Grand Diviseur Commun")

def Euclide(a, b):
        while a%b != 0:
                a, b = b, a%b
                maxi = max(a, b)
                mini = min(a, b)
                reste = maxi%mini
                div = int(maxi/mini)
                print("{0} = {1}*{2} + {3}".format(maxi, mini, div, reste))
                maxi = mini
                mini = reste
        Back()

def Back():
        print("................................................................................")
        print()
        print("1- Revenir au menu principal")
        print("2- Quitter")
        print()
        try:
                re = int(input("Taper le nombre de votre choix => "))
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
        if re == 1:
                Menu()
        elif re == 2:
                print("Au revoir.")
                sys.exit()
        else:
                print("Ce nombre ne correspond pas aux choix disponibles")
                sys.exit()

def PGCD(A, B):
        if A <= 0 or B <= 0:
                print("Les nombres ne peuvent pas être égaux ou inférieur à 0")
                sys.exit()
        elif A >= 100000000 or B >= 100000000:
                print("Les nombres ne peuvent pas être au-dessus de 1000")
                sys.exit()
        divA = []
        divB = []
        divcom = []
        for x in range(1, A+1):
                calc = int(A)/x
                if calc == int(round(calc)):
                        if x in divA:
                                pass
                        else:
                                divA.append(int(x))
                else:
                        pass
        for i in range(1,B+1):
                ca = int(B)/i
                if ca == int(round(ca)):
                        if i in divB:
                                pass
                        else:
                                divB.append(i)
                else:
                        pass
        for nb in range(max(A, B)+1):
                if nb in divA and nb in divB:
                        if nb in divcom:
                                pass
                        else:
                                divcom.append(nb)
                else:
                        pass
        print("................................................................................")
        print("Liste des diviseurs de", A)
        print(divA)
        print("")
        print("Liste des diviseurs de", B)
        print(divB)
        print("")
        print("Liste des diviseurs communs")
        print(divcom)
        longueur = max(divcom)
        print("D'où le plus grand diviseur commun de {0} et de {1} est:".format(A, B), longueur)
        Back()

def View(a, b):
        if a != int(a) and b != int(b):
                print("Tape des entiers!")
                sys.exit()
        if a > b:
                grand = a
                g = b
        elif b > a:
                grand = b
                g = a
        print("PGCD({0}, {1})".format(a, b))
        while True:
                petit = max(a, b) - min(a, b)
                print("= PGCD({0}, {1})".format(min(a, b), petit))
                ok = min(a, b)
                encor = max(ok, petit) - min(petit, ok)
                print("= PGCD({0}, {1})".format(petit, encor))
                a = petit
                b = encor
                if petit == encor:
                        break
                if max(a, b) <= 0 or min(a, b) <= 0 or petit <= 0:
                        break
        Back()

def Menu():
        print()
        print()
        print("......................................MENU......................................")
        print()
        print("1- Methode d'Euclide")
        print("2- Methode par soustractions successives")
        print("3- Methode détaillée")
        print()
        try:
                choix = int(input("Taper le nombre correspondant à votre choix => "))
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
        print("..................................................................................")
        try:
                if choix == 1:
                        a = int(input("Taper A: "))
                        b = int(input("Taper B: "))
                        Euclide(a, b)
                elif choix == 2:
                        a = int(input("Taper A: "))
                        b = int(input("Taper B: "))
                        View(a, b)
                elif choix == 3:
                        A = int(input("Taper A: "))
                        B = int(input("Taper B: "))
                        PGCD(A, B)
                else:
                        print("Ce nombre ne correspond pas aux choix disponibles")
        except ValueError:
                print("Vous n'avez pas taper un nombre valable")
                sys.exit()
Menu()

    - name: u = {} # {0:1, 1:2, 2:5, 3:26,...}


def recurrence_sequence(first_n: int, first_u_n: int, n_seeked: int):
    """
    Parameters: first_n corresponds to the value of the initial term of the sequence whose image is known by Un.
                first_u_n is the image of this initial term by Un
                n_seeked is the value n searched for
    Example: U0 = 1
              0 would then be the value of the "first_n" parameter, and
              1 the value of "first_u_n".
    """
    for n in range(first_n, n_seeked+1):
        u[n] = u[n-1]*2 - 3 if n-1 in u.keys() else first_u_n
        yield f"U{n} = {u[n]}"


for i in recurrence_sequence(0, 1, int(input("n value seeked: "))):
    print(i)
      # Turn repeated input strings (such as the build output directory) into step outputs. These step outputs can be used throughout the workflow file.
      id: strings
      shell: bash
      run: |
        echo "build-output-dir=${{ github.workspace }}/build" >> "$GITHUB_OUTPUT"

    - name: Configure CMake
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: >
        cmake -B ${{ steps.strings.outputs.build-output-dir }}
        -DCMAKE_CXX_COMPILER=${{ matrix.cpp_compiler }}
        -DCMAKE_C_COMPILER=${{ matrix.c_compiler }}
        -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
        -S ${{ github.workspace }}

    - name: Build
      # Build your program with the given configuration. Note that --config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      run: cmake --build ${{ steps.strings.outputs.build-output-dir }} --config ${{ matrix.build_type }}

    - name: Test
      working-directory: ${{ steps.strings.outputs.build-output-dir }}
      # Execute tests defined by the CMake configuration. Note that --build-config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest --build-config ${{ matrix.build_type }}
