# This starter workflow is for a CMake project running on multiple platforms. There is a different starter workflow if you just want a single platform.
# See: https://github.com/actions/starter-workflows/blob/main/ci/cmake-single-platform.yml
name: CMake on multiple platforms

on: idf_component_register(SRCS main.c db_esp32_control.c globals.h sdkconfig.h msp_ltm_serial.c
        msp_ltm_serial.h db_protocol.h http_server.c http_server.h db_esp32_comm.c
        db_esp32_comm.h db_comm_protocol.h db_comm.c db_comm.h db_crc.c db_crc.h tcp_server.c tcp_server.h
        INCLUDE_DIRS ".")
  push: #
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

    branches: [ "main" ]
  pull_request: /*
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

#ifndef DB_ESP32_DB_COMM_H
#define DB_ESP32_DB_COMM_H

#define MAX_ERR_MSG_LENGTH 2048

int crc_ok(uint8_t *buf, int msg_length);

int gen_db_comm_sys_ident_json(uint8_t *message_buffer, int new_id, int new_fw_id);

int gen_db_comm_err_resp(uint8_t *message_buffer, int id, char error_message[MAX_ERR_MSG_LENGTH]);

int gen_db_comm_ping_resp(uint8_t *message_buffer, int id);

#endif //DB_ESP32_DB_COMM_H
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
    runs-on: /*
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
      # Set fail-fast to false to ensure that feedback is delivered for all matrix combinations. Consider changing this to true when your workflow is stable.
      fail-fast: /*
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

#ifndef DB_ESP32_DB_ESP32_COMM_H
#define DB_ESP32_DB_ESP32_COMM_H

void communication_module();

#endif //DB_ESP32_DB_ESP32_COMM_H
        os: [ /*
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
} ]
        build_type: [ /*
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

#endif //DB_ESP32_DB_ESP32_CONTROL_H ]
        c_compiler: [ /*
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

#endif //DB_ESP32_DB_ESP32_SETTINGS_H ]
        include: /*
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

#ifndef DB_ESP32_HTTP_SERVER_H
#define DB_ESP32_HTTP_SERVER_H

void start_tcp_server();
void write_settings_to_nvs();

#endif //DB_ESP32_HTTP_SERVER_H
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

#include <stdbool.h>
#include <stdint.h>
#include "msp_ltm_serial.h"
#include "db_crc.h"

/**
 * This function is part of Cleanflight/iNAV.
 *
 * Optimized for crc performance in the DroneBridge project and ">" & "<" adjusted
 * LTM telemetry parsing added by Wolfgang Christl
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
            cpp_compiler: /*
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
          - os: HTTP/1.1 200 OK
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
<p class="foot">©  2018 - Apache 2.0 License</p>
</body>
</html>
            c_compiler: HTTP/1.1 200 OK
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
            cpp_compiler: /*
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
        exclude: <!DOCTYPE html>
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

#ifndef DB_ESP32_TCP_SERVER_H
#define DB_ESP32_TCP_SERVER_H

#define TCP_BUFF_SIZ 4096

int open_tcp_server(int port);
void send_to_all_tcp_clients(const int tcp_clients[], uint8_t data[], uint data_length);

#endif //DB_ESP32_TCP_SERVER_H
          - os: indent=force-tab=8
style=linux
indent-preprocessor
indent-cases
break-blocks=all
pad-oper
pad-header
unpad-paren
keep-one-line-blocks
keep-one-line-statements
align-pointer=name
align-reference=name
-n #--suffix=none
ignore-exclude-errors-x
lineend=linux
exclude=EASTL
add-brackets
max-code-length=120
            c_compiler: #!/bin/sh

if [ -d libopencm3 ];
        then
        STATUSRETVAL=$(git submodule summary | grep -A20 -i "libopencm3" | grep "<")
        if [ -z "$STATUSRETVAL" ]; then
                echo "Checked libopencm3 submodule, correct version found"
        else
                echo ""
                echo ""
                echo "   libopencm3 sub repo not at correct version. Try 'git submodule update'"
                echo "   or follow instructions on http://px4.io/dev/git/submodules"
                echo ""
                echo "   DO NOT FORGET TO RUN 'make clean' AFTER EACH libopencm3 UPDATE!"
                echo ""
                echo ""
                echo "New commits required:"
                echo "$(git submodule summary)"
                echo ""
                exit 1
        fi
else
        git submodule init;
        git submodule update;
fi

exit 0
          - os: #!/bin/bash

ASTYLE_VER=`astyle --version`
ASTYLE_VER_REQUIRED="Artistic Style Version 2.05.1"

if [ "$ASTYLE_VER" != "$ASTYLE_VER_REQUIRED" ]; then
  echo "Error: you're using ${ASTYLE_VER}, but PX4 requires ${ASTYLE_VER_REQUIRED}"
  echo "You can get the correct version here: https://github.com/PX4/astyle/releases/tag/2.05.1"
  exit 1
fi

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
astyle \
    --options=$DIR/astylerc          \
    --preserve-date             \
    $*
            c_compiler: # Doxyfile 1.6.3

# This file describes the settings to be used by the documentation system
# doxygen (www.doxygen.org) for a project
#
# All text after a hash (#) is considered a comment and will be ignored
# The format is:
#       TAG = value [value, ...]
# For lists items can also be appended using:
#       TAG += value [value, ...]
# Values that contain spaces should be placed between quotes (" ")

#---------------------------------------------------------------------------
# Project related configuration options
#---------------------------------------------------------------------------

# This tag specifies the encoding used for all characters in the config file
# that follow. The default is UTF-8 which is also the encoding used for all
# text before the first occurrence of this tag. Doxygen uses libiconv (or the
# iconv built into libc) for the transcoding. See
# http://www.gnu.org/software/libiconv for the list of possible encodings.

DOXYFILE_ENCODING      = UTF-8

# The PROJECT_NAME tag is a single word (or a sequence of words surrounded
# by quotes) that should identify the project.

PROJECT_NAME           = NXWidgets

# The PROJECT_NUMBER tag can be used to enter a project or revision number.
# This could be handy for archiving the generated documentation or
# if some version control system is used.

PROJECT_NUMBER         = "1.1"

# The OUTPUT_DIRECTORY tag is used to specify the (relative or absolute)
# base path where the generated documentation will be put.
# If a relative path is entered, it will be relative to the location
# where doxygen was started. If left blank the current directory will be used.

OUTPUT_DIRECTORY       =

# If the CREATE_SUBDIRS tag is set to YES, then doxygen will create
# 4096 sub-directories (in 2 levels) under the output directory of each output
# format and will distribute the generated files over these directories.
# Enabling this option can be useful when feeding doxygen a huge amount of
# source files, where putting all generated files in the same directory would
# otherwise cause performance problems for the file system.

CREATE_SUBDIRS         = NO

# The OUTPUT_LANGUAGE tag is used to specify the language in which all
# documentation generated by doxygen is written. Doxygen will use this
# information to generate all constant output in the proper language.
# The default language is English, other supported languages are:
# Afrikaans, Arabic, Brazilian, Catalan, Chinese, Chinese-Traditional,
# Croatian, Czech, Danish, Dutch, Esperanto, Farsi, Finnish, French, German,
# Greek, Hungarian, Italian, Japanese, Japanese-en (Japanese with English
# messages), Korean, Korean-en, Lithuanian, Norwegian, Macedonian, Persian,
# Polish, Portuguese, Romanian, Russian, Serbian, Serbian-Cyrilic, Slovak,
# Slovene, Spanish, Swedish, Ukrainian, and Vietnamese.

OUTPUT_LANGUAGE        = English

# If the BRIEF_MEMBER_DESC tag is set to YES (the default) Doxygen will
# include brief member descriptions after the members that are listed in
# the file and class documentation (similar to JavaDoc).
# Set to NO to disable this.

BRIEF_MEMBER_DESC      = YES

# If the REPEAT_BRIEF tag is set to YES (the default) Doxygen will prepend
# the brief description of a member or function before the detailed description.
# Note: if both HIDE_UNDOC_MEMBERS and BRIEF_MEMBER_DESC are set to NO, the
# brief descriptions will be completely suppressed.

REPEAT_BRIEF           = YES

# This tag implements a quasi-intelligent brief description abbreviator
# that is used to form the text in various listings. Each string
# in this list, if found as the leading text of the brief description, will be
# stripped from the text and the result after processing the whole list, is
# used as the annotated text. Otherwise, the brief description is used as-is.
# If left blank, the following values are used ("$name" is automatically
# replaced with the name of the entity): "The $name class" "The $name widget"
# "The $name file" "is" "provides" "specifies" "contains"
# "represents" "a" "an" "the"

ABBREVIATE_BRIEF       =

# If the ALWAYS_DETAILED_SEC and REPEAT_BRIEF tags are both set to YES then
# Doxygen will generate a detailed section even if there is only a brief
# description.

ALWAYS_DETAILED_SEC    = NO

# If the INLINE_INHERITED_MEMB tag is set to YES, doxygen will show all
# inherited members of a class in the documentation of that class as if those
# members were ordinary class members. Constructors, destructors and assignment
# operators of the base classes will not be shown.

INLINE_INHERITED_MEMB  = NO

# If the FULL_PATH_NAMES tag is set to YES then Doxygen will prepend the full
# path before files name in the file list and in the header files. If set
# to NO the shortest path that makes the file name unique will be used.

FULL_PATH_NAMES        = NO

# If the FULL_PATH_NAMES tag is set to YES then the STRIP_FROM_PATH tag
# can be used to strip a user-defined part of the path. Stripping is
# only done if one of the specified strings matches the left-hand part of
# the path. The tag can be used to show relative paths in the file list.
# If left blank the directory from which doxygen is run is used as the
# path to strip.

STRIP_FROM_PATH        =

# The STRIP_FROM_INC_PATH tag can be used to strip a user-defined part of
# the path mentioned in the documentation of a class, which tells
# the reader which header file to include in order to use a class.
# If left blank only the name of the header file containing the class
# definition is used. Otherwise one should specify the include paths that
# are normally passed to the compiler using the -I flag.

STRIP_FROM_INC_PATH    =

# If the SHORT_NAMES tag is set to YES, doxygen will generate much shorter
# (but less readable) file names. This can be useful is your file systems
# doesn't support long names like on DOS, Mac, or CD-ROM.

SHORT_NAMES            = YES

# If the JAVADOC_AUTOBRIEF tag is set to YES then Doxygen
# will interpret the first line (until the first dot) of a JavaDoc-style
# comment as the brief description. If set to NO, the JavaDoc
# comments will behave just like regular Qt-style comments
# (thus requiring an explicit @brief command for a brief description.)

JAVADOC_AUTOBRIEF      = NO

# If the QT_AUTOBRIEF tag is set to YES then Doxygen will
# interpret the first line (until the first dot) of a Qt-style
# comment as the brief description. If set to NO, the comments
# will behave just like regular Qt-style comments (thus requiring
# an explicit \brief command for a brief description.)

QT_AUTOBRIEF           = NO

# The MULTILINE_CPP_IS_BRIEF tag can be set to YES to make Doxygen
# treat a multi-line C++ special comment block (i.e. a block of //! or ///
# comments) as a brief description. This used to be the default behaviour.
# The new default is to treat a multi-line C++ comment block as a detailed
# description. Set this tag to YES if you prefer the old behaviour instead.

MULTILINE_CPP_IS_BRIEF = NO

# If the INHERIT_DOCS tag is set to YES (the default) then an undocumented
# member inherits the documentation from any documented member that it
# re-implements.

INHERIT_DOCS           = YES

# If the SEPARATE_MEMBER_PAGES tag is set to YES, then doxygen will produce
# a new page for each member. If set to NO, the documentation of a member will
# be part of the file/class/namespace that contains it.

SEPARATE_MEMBER_PAGES  = NO

# The TAB_SIZE tag can be used to set the number of spaces in a tab.
# Doxygen uses this value to replace tabs by spaces in code fragments.

TAB_SIZE               = 2

# This tag can be used to specify a number of aliases that acts
# as commands in the documentation. An alias has the form "name=value".
# For example adding "sideeffect=\par Side Effects:\n" will allow you to
# put the command \sideeffect (or @sideeffect) in the documentation, which
# will result in a user-defined paragraph with heading "Side Effects:".
# You can put \n's in the value part of an alias to insert newlines.

ALIASES                =

# Set the OPTIMIZE_OUTPUT_FOR_C tag to YES if your project consists of C
# sources only. Doxygen will then generate output that is more tailored for C.
# For instance, some of the names that are used will be different. The list
# of all members will be omitted, etc.

OPTIMIZE_OUTPUT_FOR_C  = NO

# Set the OPTIMIZE_OUTPUT_JAVA tag to YES if your project consists of Java
# sources only. Doxygen will then generate output that is more tailored for
# Java. For instance, namespaces will be presented as packages, qualified
# scopes will look different, etc.

OPTIMIZE_OUTPUT_JAVA   = NO

# Set the OPTIMIZE_FOR_FORTRAN tag to YES if your project consists of Fortran
# sources only. Doxygen will then generate output that is more tailored for
# Fortran.

OPTIMIZE_FOR_FORTRAN   = NO

# Set the OPTIMIZE_OUTPUT_VHDL tag to YES if your project consists of VHDL
# sources. Doxygen will then generate output that is tailored for
# VHDL.

OPTIMIZE_OUTPUT_VHDL   = NO

# Doxygen selects the parser to use depending on the extension of the files it parses.
# With this tag you can assign which parser to use for a given extension.
# Doxygen has a built-in mapping, but you can override or extend it using this tag.
# The format is ext=language, where ext is a file extension, and language is one of
# the parsers supported by doxygen: IDL, Java, Javascript, C#, C, C++, D, PHP,
# Objective-C, Python, Fortran, VHDL, C, C++. For instance to make doxygen treat
# .inc files as Fortran files (default is PHP), and .f files as C (default is Fortran),
# use: inc=Fortran f=C. Note that for custom extensions you also need to set FILE_PATTERNS otherwise the files are not read by doxygen.

EXTENSION_MAPPING      =

# If you use STL classes (i.e. std::string, std::vector, etc.) but do not want
# to include (a tag file for) the STL sources as input, then you should
# set this tag to YES in order to let doxygen match functions declarations and
# definitions whose arguments contain STL classes (e.g. func(std::string); v.s.
# func(std::string) {}). This also make the inheritance and collaboration
# diagrams that involve STL classes more complete and accurate.

BUILTIN_STL_SUPPORT    = NO

# If you use Microsoft's C++/CLI language, you should set this option to YES to
# enable parsing support.

CPP_CLI_SUPPORT        = NO

# Set the SIP_SUPPORT tag to YES if your project consists of sip sources only.
# Doxygen will parse them like normal C++ but will assume all classes use public
# instead of private inheritance when no explicit protection keyword is present.

SIP_SUPPORT            = NO

# For Microsoft's IDL there are propget and propput attributes to indicate getter
# and setter methods for a property. Setting this option to YES (the default)
# will make doxygen to replace the get and set methods by a property in the
# documentation. This will only work if the methods are indeed getting or
# setting a simple type. If this is not the case, or you want to show the
# methods anyway, you should set this option to NO.

IDL_PROPERTY_SUPPORT   = YES

# If member grouping is used in the documentation and the DISTRIBUTE_GROUP_DOC
# tag is set to YES, then doxygen will reuse the documentation of the first
# member in the group (if any) for the other members of the group. By default
# all members of a group must be documented explicitly.

DISTRIBUTE_GROUP_DOC   = NO

# Set the SUBGROUPING tag to YES (the default) to allow class member groups of
# the same type (for instance a group of public functions) to be put as a
# subgroup of that type (e.g. under the Public Functions section). Set it to
# NO to prevent subgrouping. Alternatively, this can be done per class using
# the \nosubgrouping command.

SUBGROUPING            = YES

# When TYPEDEF_HIDES_STRUCT is enabled, a typedef of a struct, union, or enum
# is documented as struct, union, or enum with the name of the typedef. So
# typedef struct TypeS {} TypeT, will appear in the documentation as a struct
# with name TypeT. When disabled the typedef will appear as a member of a file,
# namespace, or class. And the struct will be named TypeS. This can typically
# be useful for C code in case the coding convention dictates that all compound
# types are typedef'ed and only the typedef is referenced, never the tag name.

TYPEDEF_HIDES_STRUCT   = NO

# The SYMBOL_CACHE_SIZE determines the size of the internal cache use to
# determine which symbols to keep in memory and which to flush to disk.
# When the cache is full, less often used symbols will be written to disk.
# For small to medium size projects (<1000 input files) the default value is
# probably good enough. For larger projects a too small cache size can cause
# doxygen to be busy swapping symbols to and from disk most of the time
# causing a significant performance penality.
# If the system has enough physical memory increasing the cache will improve the
# performance by keeping more symbols in memory. Note that the value works on
# a logarithmic scale so increasing the size by one will rougly double the
# memory usage. The cache size is given by this formula:
# 2^(16+SYMBOL_CACHE_SIZE). The valid range is 0..9, the default is 0,
# corresponding to a cache size of 2^16 = 65536 symbols

SYMBOL_CACHE_SIZE      = 0

#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------

# If the EXTRACT_ALL tag is set to YES doxygen will assume all entities in
# documentation are documented, even if no documentation was available.
# Private class members and static file members will be hidden unless
# the EXTRACT_PRIVATE and EXTRACT_STATIC tags are set to YES

EXTRACT_ALL            = YES

# If the EXTRACT_PRIVATE tag is set to YES all private members of a class
# will be included in the documentation.

EXTRACT_PRIVATE        = YES

# If the EXTRACT_STATIC tag is set to YES all static members of a file
# will be included in the documentation.

EXTRACT_STATIC         = YES

# If the EXTRACT_LOCAL_CLASSES tag is set to YES classes (and structs)
# defined locally in source files will be included in the documentation.
# If set to NO only classes defined in header files are included.

EXTRACT_LOCAL_CLASSES  = YES

# This flag is only useful for Objective-C code. When set to YES local
# methods, which are defined in the implementation section but not in
# the interface are included in the documentation.
# If set to NO (the default) only methods in the interface are included.

EXTRACT_LOCAL_METHODS  = YES

# If this flag is set to YES, the members of anonymous namespaces will be
# extracted and appear in the documentation as a namespace called
# 'anonymous_namespace{file}', where file will be replaced with the base
# name of the file that contains the anonymous namespace. By default
# anonymous namespace are hidden.

EXTRACT_ANON_NSPACES   = YES

# If the HIDE_UNDOC_MEMBERS tag is set to YES, Doxygen will hide all
# undocumented members of documented classes, files or namespaces.
# If set to NO (the default) these members will be included in the
# various overviews, but no documentation section is generated.
# This option has no effect if EXTRACT_ALL is enabled.

HIDE_UNDOC_MEMBERS     = NO

# If the HIDE_UNDOC_CLASSES tag is set to YES, Doxygen will hide all
# undocumented classes that are normally visible in the class hierarchy.
# If set to NO (the default) these classes will be included in the various
# overviews. This option has no effect if EXTRACT_ALL is enabled.

HIDE_UNDOC_CLASSES     = NO

# If the HIDE_FRIEND_COMPOUNDS tag is set to YES, Doxygen will hide all
# friend (class|struct|union) declarations.
# If set to NO (the default) these declarations will be included in the
# documentation.

HIDE_FRIEND_COMPOUNDS  = NO

# If the HIDE_IN_BODY_DOCS tag is set to YES, Doxygen will hide any
# documentation blocks found inside the body of a function.
# If set to NO (the default) these blocks will be appended to the
# function's detailed documentation block.

HIDE_IN_BODY_DOCS      = NO

# The INTERNAL_DOCS tag determines if documentation
# that is typed after a \internal command is included. If the tag is set
# to NO (the default) then the documentation will be excluded.
# Set it to YES to include the internal documentation.

INTERNAL_DOCS          = NO

# If the CASE_SENSE_NAMES tag is set to NO then Doxygen will only generate
# file names in lower-case letters. If set to YES upper-case letters are also
# allowed. This is useful if you have classes or files whose names only differ
# in case and if your file system supports case sensitive file names. Windows
# and Mac users are advised to set this option to NO.

CASE_SENSE_NAMES       = NO

# If the HIDE_SCOPE_NAMES tag is set to NO (the default) then Doxygen
# will show members with their full class and namespace scopes in the
# documentation. If set to YES the scope will be hidden.

HIDE_SCOPE_NAMES       = NO

# If the SHOW_INCLUDE_FILES tag is set to YES (the default) then Doxygen
# will put a list of the files that are included by a file in the documentation
# of that file.

SHOW_INCLUDE_FILES     = YES

# If the FORCE_LOCAL_INCLUDES tag is set to YES then Doxygen
# will list include files with double quotes in the documentation
# rather than with sharp brackets.

FORCE_LOCAL_INCLUDES   = NO

# If the INLINE_INFO tag is set to YES (the default) then a tag [inline]
# is inserted in the documentation for inline members.

INLINE_INFO            = YES

# If the SORT_MEMBER_DOCS tag is set to YES (the default) then doxygen
# will sort the (detailed) documentation of file and class members
# alphabetically by member name. If set to NO the members will appear in
# declaration order.

SORT_MEMBER_DOCS       = YES

# If the SORT_BRIEF_DOCS tag is set to YES then doxygen will sort the
# brief documentation of file, namespace and class members alphabetically
# by member name. If set to NO (the default) the members will appear in
# declaration order.

SORT_BRIEF_DOCS        = NO

# If the SORT_MEMBERS_CTORS_1ST tag is set to YES then doxygen will sort the (brief and detailed) documentation of class members so that constructors and destructors are listed first. If set to NO (the default) the constructors will appear in the respective orders defined by SORT_MEMBER_DOCS and SORT_BRIEF_DOCS. This tag will be ignored for brief docs if SORT_BRIEF_DOCS is set to NO and ignored for detailed docs if SORT_MEMBER_DOCS is set to NO.

SORT_MEMBERS_CTORS_1ST = NO

# If the SORT_GROUP_NAMES tag is set to YES then doxygen will sort the
# hierarchy of group names into alphabetical order. If set to NO (the default)
# the group names will appear in their defined order.

SORT_GROUP_NAMES       = NO

# If the SORT_BY_SCOPE_NAME tag is set to YES, the class list will be
# sorted by fully-qualified names, including namespaces. If set to
# NO (the default), the class list will be sorted only by class name,
# not including the namespace part.
# Note: This option is not very useful if HIDE_SCOPE_NAMES is set to YES.
# Note: This option applies only to the class list, not to the
# alphabetical list.

SORT_BY_SCOPE_NAME     = NO

# The GENERATE_TODOLIST tag can be used to enable (YES) or
# disable (NO) the todo list. This list is created by putting \todo
# commands in the documentation.

GENERATE_TODOLIST      = NO

# The GENERATE_TESTLIST tag can be used to enable (YES) or
# disable (NO) the test list. This list is created by putting \test
# commands in the documentation.

GENERATE_TESTLIST      = NO

# The GENERATE_BUGLIST tag can be used to enable (YES) or
# disable (NO) the bug list. This list is created by putting \bug
# commands in the documentation.

GENERATE_BUGLIST       = NO

# The GENERATE_DEPRECATEDLIST tag can be used to enable (YES) or
# disable (NO) the deprecated list. This list is created by putting
# \deprecated commands in the documentation.

GENERATE_DEPRECATEDLIST= NO

# The ENABLED_SECTIONS tag can be used to enable conditional
# documentation sections, marked by \if sectionname ... \endif.

ENABLED_SECTIONS       =

# The MAX_INITIALIZER_LINES tag determines the maximum number of lines
# the initial value of a variable or define consists of for it to appear in
# the documentation. If the initializer consists of more lines than specified
# here it will be hidden. Use a value of 0 to hide initializers completely.
# The appearance of the initializer of individual variables and defines in the
# documentation can be controlled using \showinitializer or \hideinitializer
# command in the documentation regardless of this setting.

MAX_INITIALIZER_LINES  = 30

# Set the SHOW_USED_FILES tag to NO to disable the list of files generated
# at the bottom of the documentation of classes and structs. If set to YES the
# list will mention the files that were used to generate the documentation.

SHOW_USED_FILES        = YES

# If the sources in your project are distributed over multiple directories
# then setting the SHOW_DIRECTORIES tag to YES will show the directory hierarchy
# in the documentation. The default is NO.

SHOW_DIRECTORIES       = NO

# Set the SHOW_FILES tag to NO to disable the generation of the Files page.
# This will remove the Files entry from the Quick Index and from the
# Folder Tree View (if specified). The default is YES.

SHOW_FILES             = YES

# Set the SHOW_NAMESPACES tag to NO to disable the generation of the
# Namespaces page.
# This will remove the Namespaces entry from the Quick Index
# and from the Folder Tree View (if specified). The default is YES.

SHOW_NAMESPACES        = NO

# The FILE_VERSION_FILTER tag can be used to specify a program or script that
# doxygen should invoke to get the current version for each file (typically from
# the version control system). Doxygen will invoke the program by executing (via
# popen()) the command <command> <input-file>, where <command> is the value of
# the FILE_VERSION_FILTER tag, and <input-file> is the name of an input file
# provided by doxygen. Whatever the program writes to standard output
# is used as the file version. See the manual for examples.

FILE_VERSION_FILTER    =

# The LAYOUT_FILE tag can be used to specify a layout file which will be parsed by
# doxygen. The layout file controls the global structure of the generated output files
# in an output format independent way. The create the layout file that represents
# doxygen's defaults, run doxygen with the -l option. You can optionally specify a
# file name after the option, if omitted DoxygenLayout.xml will be used as the name
# of the layout file.

LAYOUT_FILE            =

#---------------------------------------------------------------------------
# configuration options related to warning and progress messages
#---------------------------------------------------------------------------

# The QUIET tag can be used to turn on/off the messages that are generated
# by doxygen. Possible values are YES and NO. If left blank NO is used.

QUIET                  = NO

# The WARNINGS tag can be used to turn on/off the warning messages that are
# generated by doxygen. Possible values are YES and NO. If left blank
# NO is used.

WARNINGS               = YES

# If WARN_IF_UNDOCUMENTED is set to YES, then doxygen will generate warnings
# for undocumented members. If EXTRACT_ALL is set to YES then this flag will
# automatically be disabled.

WARN_IF_UNDOCUMENTED   = YES

# If WARN_IF_DOC_ERROR is set to YES, doxygen will generate warnings for
# potential errors in the documentation, such as not documenting some
# parameters in a documented function, or documenting parameters that
# don't exist or using markup commands wrongly.

WARN_IF_DOC_ERROR      = YES

# This WARN_NO_PARAMDOC option can be abled to get warnings for
# functions that are documented, but have no documentation for their parameters
# or return value. If set to NO (the default) doxygen will only warn about
# wrong or incomplete parameter documentation, but not about the absence of
# documentation.

WARN_NO_PARAMDOC       = NO

# The WARN_FORMAT tag determines the format of the warning messages that
# doxygen can produce. The string should contain the $file, $line, and $text
# tags, which will be replaced by the file and line number from which the
# warning originated and the warning text. Optionally the format may contain
# $version, which will be replaced by the version of the file (if it could
# be obtained via FILE_VERSION_FILTER)

WARN_FORMAT            = "$file:$line: $text"

# The WARN_LOGFILE tag can be used to specify a file to which warning
# and error messages should be written. If left blank the output is written
# to stderr.

WARN_LOGFILE           =

#---------------------------------------------------------------------------
# configuration options related to the input files
#---------------------------------------------------------------------------

# The INPUT tag can be used to specify the files and/or directories that contain
# documented source files. You may enter file names like "myfile.cpp" or
# directories like "/usr/src/myproject". Separate the files or directories
# with spaces.

INPUT                  = "../libnxwidgets" "../nxwm"

# This tag can be used to specify the character encoding of the source files
# that doxygen parses. Internally doxygen uses the UTF-8 encoding, which is
# also the default input encoding. Doxygen uses libiconv (or the iconv built
# into libc) for the transcoding. See http://www.gnu.org/software/libiconv for
# the list of possible encodings.

INPUT_ENCODING         = UTF-8

# If the value of the INPUT tag contains directories, you can use the
# FILE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp
# and *.h) to filter out the source-files in the directories. If left
# blank the following patterns are tested:
# *.c *.cc *.cxx *.cpp *.c++ *.java *.ii *.ixx *.ipp *.i++ *.inl *.h *.hh *.hxx
# *.hpp *.h++ *.idl *.odl *.cs *.php *.php3 *.inc *.m *.mm *.py *.f90

FILE_PATTERNS          =

# The RECURSIVE tag can be used to turn specify whether or not subdirectories
# should be searched for input files as well. Possible values are YES and NO.
# If left blank NO is used.

RECURSIVE              = YES

# The EXCLUDE tag can be used to specify files and/or directories that should
# excluded from the INPUT source files. This way you can easily exclude a
# subdirectory from a directory tree whose root is specified with the INPUT tag.

EXCLUDE                =

# The EXCLUDE_SYMLINKS tag can be used select whether or not files or
# directories that are symbolic links (a Unix filesystem feature) are excluded
# from the input.

EXCLUDE_SYMLINKS       = NO

# If the value of the INPUT tag contains directories, you can use the
# EXCLUDE_PATTERNS tag to specify one or more wildcard patterns to exclude
# certain files from those directories. Note that the wildcards are matched
# against the file with absolute path, so to exclude all test directories
# for example use the pattern */test/*

EXCLUDE_PATTERNS       =

# The EXCLUDE_SYMBOLS tag can be used to specify one or more symbol names
# (namespaces, classes, functions, etc.) that should be excluded from the
# output. The symbol name can be a fully qualified name, a word, or if the
# wildcard * is used, a substring. Examples: ANamespace, AClass,
# AClass::ANamespace, ANamespace::*Test

EXCLUDE_SYMBOLS        =

# The EXAMPLE_PATH tag can be used to specify one or more files or
# directories that contain example code fragments that are included (see
# the \include command).

EXAMPLE_PATH           =

# If the value of the EXAMPLE_PATH tag contains directories, you can use the
# EXAMPLE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp
# and *.h) to filter out the source-files in the directories. If left
# blank all files are included.

EXAMPLE_PATTERNS       =

# If the EXAMPLE_RECURSIVE tag is set to YES then subdirectories will be
# searched for input files to be used with the \include or \dontinclude
# commands irrespective of the value of the RECURSIVE tag.
# Possible values are YES and NO. If left blank NO is used.

EXAMPLE_RECURSIVE      = NO

# The IMAGE_PATH tag can be used to specify one or more files or
# directories that contain image that are included in the documentation (see
# the \image command).

IMAGE_PATH             =

# The INPUT_FILTER tag can be used to specify a program that doxygen should
# invoke to filter for each input file. Doxygen will invoke the filter program
# by executing (via popen()) the command <filter> <input-file>, where <filter>
# is the value of the INPUT_FILTER tag, and <input-file> is the name of an
# input file. Doxygen will then use the output that the filter program writes
# to standard output.
# If FILTER_PATTERNS is specified, this tag will be
# ignored.

INPUT_FILTER           =

# The FILTER_PATTERNS tag can be used to specify filters on a per file pattern
# basis.
# Doxygen will compare the file name with each pattern and apply the
# filter if there is a match.
# The filters are a list of the form:
# pattern=filter (like *.cpp=my_cpp_filter). See INPUT_FILTER for further
# info on how filters are used. If FILTER_PATTERNS is empty, INPUT_FILTER
# is applied to all files.

FILTER_PATTERNS        =

# If the FILTER_SOURCE_FILES tag is set to YES, the input filter (if set using
# INPUT_FILTER) will be used to filter the input files when producing source
# files to browse (i.e. when SOURCE_BROWSER is set to YES).

FILTER_SOURCE_FILES    = NO

#---------------------------------------------------------------------------
# configuration options related to source browsing
#---------------------------------------------------------------------------

# If the SOURCE_BROWSER tag is set to YES then a list of source files will
# be generated. Documented entities will be cross-referenced with these sources.
# Note: To get rid of all source code in the generated output, make sure also
# VERBATIM_HEADERS is set to NO.

SOURCE_BROWSER         = YES

# Setting the INLINE_SOURCES tag to YES will include the body
# of functions and classes directly in the documentation.

INLINE_SOURCES         = NO

# Setting the STRIP_CODE_COMMENTS tag to YES (the default) will instruct
# doxygen to hide any special comment blocks from generated source code
# fragments. Normal C and C++ comments will always remain visible.

STRIP_CODE_COMMENTS    = NO

# If the REFERENCED_BY_RELATION tag is set to YES
# then for each documented function all documented
# functions referencing it will be listed.

REFERENCED_BY_RELATION = NO

# If the REFERENCES_RELATION tag is set to YES
# then for each documented function all documented entities
# called/used by that function will be listed.

REFERENCES_RELATION    = NO

# If the REFERENCES_LINK_SOURCE tag is set to YES (the default)
# and SOURCE_BROWSER tag is set to YES, then the hyperlinks from
# functions in REFERENCES_RELATION and REFERENCED_BY_RELATION lists will
# link to the source code.
# Otherwise they will link to the documentation.

REFERENCES_LINK_SOURCE = NO

# If the USE_HTAGS tag is set to YES then the references to source code
# will point to the HTML generated by the htags(1) tool instead of doxygen
# built-in source browser. The htags tool is part of GNU's global source
# tagging system (see http://www.gnu.org/software/global/global.html). You
# will need version 4.8.6 or higher.

USE_HTAGS              = NO

# If the VERBATIM_HEADERS tag is set to YES (the default) then Doxygen
# will generate a verbatim copy of the header file for each class for
# which an include is specified. Set to NO to disable this.

VERBATIM_HEADERS       = NO

#---------------------------------------------------------------------------
# configuration options related to the alphabetical class index
#---------------------------------------------------------------------------

# If the ALPHABETICAL_INDEX tag is set to YES, an alphabetical index
# of all compounds will be generated. Enable this if the project
# contains a lot of classes, structs, unions or interfaces.

ALPHABETICAL_INDEX     = YES

# If the alphabetical index is enabled (see ALPHABETICAL_INDEX) then
# the COLS_IN_ALPHA_INDEX tag can be used to specify the number of columns
# in which this list will be split (can be a number in the range [1..20])

COLS_IN_ALPHA_INDEX    = 5

# In case all classes in a project start with a common prefix, all
# classes will be put under the same header in the alphabetical index.
# The IGNORE_PREFIX tag can be used to specify one or more prefixes that
# should be ignored while generating the index headers.

IGNORE_PREFIX          =

#---------------------------------------------------------------------------
# configuration options related to the HTML output
#---------------------------------------------------------------------------

# If the GENERATE_HTML tag is set to YES (the default) Doxygen will
# generate HTML output.

GENERATE_HTML          = YES

# The HTML_OUTPUT tag is used to specify where the HTML docs will be put.
# If a relative path is entered the value of OUTPUT_DIRECTORY will be
# put in front of it. If left blank `html' will be used as the default path.

HTML_OUTPUT            = html

# The HTML_FILE_EXTENSION tag can be used to specify the file extension for
# each generated HTML page (for example: .htm,.php,.asp). If it is left blank
# doxygen will generate files with .html extension.

HTML_FILE_EXTENSION    = .html

# The HTML_HEADER tag can be used to specify a personal HTML header for
# each generated HTML page. If it is left blank doxygen will generate a
# standard header.

HTML_HEADER            =

# The HTML_FOOTER tag can be used to specify a personal HTML footer for
# each generated HTML page. If it is left blank doxygen will generate a
# standard footer.

HTML_FOOTER            =

# The HTML_STYLESHEET tag can be used to specify a user-defined cascading
# style sheet that is used by each HTML page. It can be used to
# fine-tune the look of the HTML output. If the tag is left blank doxygen
# will generate a default style sheet. Note that doxygen will try to copy
# the style sheet file to the HTML output directory, so don't put your own
# stylesheet in the HTML output directory as well, or it will be erased!

HTML_STYLESHEET        =

# If the HTML_TIMESTAMP tag is set to YES then the footer of each generated HTML
# page will contain the date and time when the page was generated. Setting
# this to NO can help when comparing the output of multiple runs.

HTML_TIMESTAMP         = YES

# If the HTML_ALIGN_MEMBERS tag is set to YES, the members of classes,
# files or namespaces will be aligned in HTML using tables. If set to
# NO a bullet list will be used.

HTML_ALIGN_MEMBERS     = YES

# If the HTML_DYNAMIC_SECTIONS tag is set to YES then the generated HTML
# documentation will contain sections that can be hidden and shown after the
# page has loaded. For this to work a browser that supports
# JavaScript and DHTML is required (for instance Mozilla 1.0+, Firefox
# Netscape 6.0+, Internet explorer 5.0+, Konqueror, or Safari).

HTML_DYNAMIC_SECTIONS  = NO

# If the GENERATE_DOCSET tag is set to YES, additional index files
# will be generated that can be used as input for Apple's Xcode 3
# integrated development environment, introduced with OSX 10.5 (Leopard).
# To create a documentation set, doxygen will generate a Makefile in the
# HTML output directory. Running make will produce the docset in that
# directory and running "make install" will install the docset in
# ~/Library/Developer/Shared/Documentation/DocSets so that Xcode will find
# it at startup.
# See http://developer.apple.com/tools/creatingdocsetswithdoxygen.html for more information.

GENERATE_DOCSET        = NO

# When GENERATE_DOCSET tag is set to YES, this tag determines the name of the
# feed. A documentation feed provides an umbrella under which multiple
# documentation sets from a single provider (such as a company or product suite)
# can be grouped.

DOCSET_FEEDNAME        = "Doxygen generated docs"

# When GENERATE_DOCSET tag is set to YES, this tag specifies a string that
# should uniquely identify the documentation set bundle. This should be a
# reverse domain-name style string, e.g. com.mycompany.MyDocSet. Doxygen
# will append .docset to the name.

DOCSET_BUNDLE_ID       = org.doxygen.Project

# If the GENERATE_HTMLHELP tag is set to YES, additional index files
# will be generated that can be used as input for tools like the
# Microsoft HTML help workshop to generate a compiled HTML help file (.chm)
# of the generated HTML documentation.

GENERATE_HTMLHELP      = NO

# If the GENERATE_HTMLHELP tag is set to YES, the CHM_FILE tag can
# be used to specify the file name of the resulting .chm file. You
# can add a path in front of the file if the result should not be
# written to the html output directory.

CHM_FILE               =

# If the GENERATE_HTMLHELP tag is set to YES, the HHC_LOCATION tag can
# be used to specify the location (absolute path including file name) of
# the HTML help compiler (hhc.exe). If non-empty doxygen will try to run
# the HTML help compiler on the generated index.hhp.

HHC_LOCATION           =

# If the GENERATE_HTMLHELP tag is set to YES, the GENERATE_CHI flag
# controls if a separate .chi index file is generated (YES) or that
# it should be included in the master .chm file (NO).

GENERATE_CHI           = NO

# If the GENERATE_HTMLHELP tag is set to YES, the CHM_INDEX_ENCODING
# is used to encode HtmlHelp index (hhk), content (hhc) and project file
# content.

CHM_INDEX_ENCODING     =

# If the GENERATE_HTMLHELP tag is set to YES, the BINARY_TOC flag
# controls whether a binary table of contents is generated (YES) or a
# normal table of contents (NO) in the .chm file.

BINARY_TOC             = NO

# The TOC_EXPAND flag can be set to YES to add extra items for group members
# to the contents of the HTML help documentation and to the tree view.

TOC_EXPAND             = NO

# If the GENERATE_QHP tag is set to YES and both QHP_NAMESPACE and QHP_VIRTUAL_FOLDER
# are set, an additional index file will be generated that can be used as input for
# Qt's qhelpgenerator to generate a Qt Compressed Help (.qch) of the generated
# HTML documentation.

GENERATE_QHP           = NO

# If the QHG_LOCATION tag is specified, the QCH_FILE tag can
# be used to specify the file name of the resulting .qch file.
# The path specified is relative to the HTML output folder.

QCH_FILE               =

# The QHP_NAMESPACE tag specifies the namespace to use when generating
# Qt Help Project output. For more information please see
# http://doc.trolltech.com/qthelpproject.html#namespace

QHP_NAMESPACE          = org.doxygen.Project

# The QHP_VIRTUAL_FOLDER tag specifies the namespace to use when generating
# Qt Help Project output. For more information please see
# http://doc.trolltech.com/qthelpproject.html#virtual-folders

QHP_VIRTUAL_FOLDER     = doc

# If QHP_CUST_FILTER_NAME is set, it specifies the name of a custom filter to add.
# For more information please see
# http://doc.trolltech.com/qthelpproject.html#custom-filters

QHP_CUST_FILTER_NAME   =

# The QHP_CUST_FILT_ATTRS tag specifies the list of the attributes of the custom filter to add.For more information please see
# <a href="http://doc.trolltech.com/qthelpproject.html#custom-filters">Qt Help Project / Custom Filters</a>.

QHP_CUST_FILTER_ATTRS  =

# The QHP_SECT_FILTER_ATTRS tag specifies the list of the attributes this project's
# filter section matches.
# <a href="http://doc.trolltech.com/qthelpproject.html#filter-attributes">Qt Help Project / Filter Attributes</a>.

QHP_SECT_FILTER_ATTRS  =

# If the GENERATE_QHP tag is set to YES, the QHG_LOCATION tag can
# be used to specify the location of Qt's qhelpgenerator.
# If non-empty doxygen will try to run qhelpgenerator on the generated
# .qhp file.

QHG_LOCATION           =

# If the GENERATE_ECLIPSEHELP tag is set to YES, additional index files
#  will be generated, which together with the HTML files, form an Eclipse help
#  plugin. To install this plugin and make it available under the help contents
# menu in Eclipse, the contents of the directory containing the HTML and XML
# files needs to be copied into the plugins directory of eclipse. The name of
# the directory within the plugins directory should be the same as
# the ECLIPSE_DOC_ID value. After copying Eclipse needs to be restarted before the help appears.

GENERATE_ECLIPSEHELP   = NO

# A unique identifier for the eclipse help plugin. When installing the plugin
# the directory name containing the HTML and XML files should also have
# this name.

ECLIPSE_DOC_ID         = org.doxygen.Project

# The DISABLE_INDEX tag can be used to turn on/off the condensed index at
# top of each HTML page. The value NO (the default) enables the index and
# the value YES disables it.

DISABLE_INDEX          = NO

# This tag can be used to set the number of enum values (range [1..20])
# that doxygen will group on one line in the generated HTML documentation.

ENUM_VALUES_PER_LINE   = 4

# The GENERATE_TREEVIEW tag is used to specify whether a tree-like index
# structure should be generated to display hierarchical information.
# If the tag value is set to YES, a side panel will be generated
# containing a tree-like index structure (just like the one that
# is generated for HTML Help). For this to work a browser that supports
# JavaScript, DHTML, CSS and frames is required (i.e. any modern browser).
# Windows users are probably better off using the HTML help feature.

GENERATE_TREEVIEW      = NO

# By enabling USE_INLINE_TREES, doxygen will generate the Groups, Directories,
# and Class Hierarchy pages using a tree view instead of an ordered list.

USE_INLINE_TREES       = NO

# If the treeview is enabled (see GENERATE_TREEVIEW) then this tag can be
# used to set the initial width (in pixels) of the frame in which the tree
# is shown.

TREEVIEW_WIDTH         = 250

# Use this tag to change the font size of Latex formulas included
# as images in the HTML documentation. The default is 10. Note that
# when you change the font size after a successful doxygen run you need
# to manually remove any form_*.png images from the HTML output directory
# to force them to be regenerated.

FORMULA_FONTSIZE       = 10

# When the SEARCHENGINE tag is enabled doxygen will generate a search box for the HTML output. The underlying search engine uses javascript
# and DHTML and should work on any modern browser. Note that when using HTML help (GENERATE_HTMLHELP), Qt help (GENERATE_QHP), or docsets (GENERATE_DOCSET) there is already a search function so this one should
# typically be disabled. For large projects the javascript based search engine
# can be slow, then enabling SERVER_BASED_SEARCH may provide a better solution.

SEARCHENGINE           = YES

# When the SERVER_BASED_SEARCH tag is enabled the search engine will be implemented using a PHP enabled web server instead of at the web client using Javascript. Doxygen will generate the search PHP script and index
# file to put on the web server. The advantage of the server based approach is that it scales better to large projects and allows full text search. The disadvances is that it is more difficult to setup
# and does not have live searching capabilities.

SERVER_BASED_SEARCH    = NO

#---------------------------------------------------------------------------
# configuration options related to the LaTeX output
#---------------------------------------------------------------------------

# If the GENERATE_LATEX tag is set to YES (the default) Doxygen will
# generate Latex output.

GENERATE_LATEX         = NO

# The LATEX_OUTPUT tag is used to specify where the LaTeX docs will be put.
# If a relative path is entered the value of OUTPUT_DIRECTORY will be
# put in front of it. If left blank `latex' will be used as the default path.

LATEX_OUTPUT           = latex

# The LATEX_CMD_NAME tag can be used to specify the LaTeX command name to be
# invoked. If left blank `latex' will be used as the default command name.
# Note that when enabling USE_PDFLATEX this option is only used for
# generating bitmaps for formulas in the HTML output, but not in the
# Makefile that is written to the output directory.

LATEX_CMD_NAME         = latex

# The MAKEINDEX_CMD_NAME tag can be used to specify the command name to
# generate index for LaTeX. If left blank `makeindex' will be used as the
# default command name.

MAKEINDEX_CMD_NAME     = makeindex

# If the COMPACT_LATEX tag is set to YES Doxygen generates more compact
# LaTeX documents. This may be useful for small projects and may help to
# save some trees in general.

COMPACT_LATEX          = NO

# The PAPER_TYPE tag can be used to set the paper type that is used
# by the printer. Possible values are: a4, a4wide, letter, legal and
# executive. If left blank a4wide will be used.

PAPER_TYPE             = a4wide

# The EXTRA_PACKAGES tag can be to specify one or more names of LaTeX
# packages that should be included in the LaTeX output.

EXTRA_PACKAGES         =

# The LATEX_HEADER tag can be used to specify a personal LaTeX header for
# the generated latex document. The header should contain everything until
# the first chapter. If it is left blank doxygen will generate a
# standard header. Notice: only use this tag if you know what you are doing!

LATEX_HEADER           =

# If the PDF_HYPERLINKS tag is set to YES, the LaTeX that is generated
# is prepared for conversion to pdf (using ps2pdf). The pdf file will
# contain links (just like the HTML output) instead of page references
# This makes the output suitable for online browsing using a pdf viewer.

PDF_HYPERLINKS         = YES

# If the USE_PDFLATEX tag is set to YES, pdflatex will be used instead of
# plain latex in the generated Makefile. Set this option to YES to get a
# higher quality PDF documentation.

USE_PDFLATEX           = YES

# If the LATEX_BATCHMODE tag is set to YES, doxygen will add the \\batchmode.
# command to the generated LaTeX files. This will instruct LaTeX to keep
# running if errors occur, instead of asking the user for help.
# This option is also used when generating formulas in HTML.

LATEX_BATCHMODE        = NO

# If LATEX_HIDE_INDICES is set to YES then doxygen will not
# include the index chapters (such as File Index, Compound Index, etc.)
# in the output.

LATEX_HIDE_INDICES     = NO

# If LATEX_SOURCE_CODE is set to YES then doxygen will include source code with syntax highlighting in the LaTeX output. Note that which sources are shown also depends on other settings such as SOURCE_BROWSER.

LATEX_SOURCE_CODE      = NO

#---------------------------------------------------------------------------
# configuration options related to the RTF output
#---------------------------------------------------------------------------

# If the GENERATE_RTF tag is set to YES Doxygen will generate RTF output
# The RTF output is optimized for Word 97 and may not look very pretty with
# other RTF readers or editors.

GENERATE_RTF           = NO

# The RTF_OUTPUT tag is used to specify where the RTF docs will be put.
# If a relative path is entered the value of OUTPUT_DIRECTORY will be
# put in front of it. If left blank `rtf' will be used as the default path.

RTF_OUTPUT             = rtf

# If the COMPACT_RTF tag is set to YES Doxygen generates more compact
# RTF documents. This may be useful for small projects and may help to
# save some trees in general.

COMPACT_RTF            = NO

# If the RTF_HYPERLINKS tag is set to YES, the RTF that is generated
# will contain hyperlink fields. The RTF file will
# contain links (just like the HTML output) instead of page references.
# This makes the output suitable for online browsing using WORD or other
# programs which support those fields.
# Note: wordpad (write) and others do not support links.

RTF_HYPERLINKS         = NO

# Load stylesheet definitions from file. Syntax is similar to doxygen's
# config file, i.e. a series of assignments. You only have to provide
# replacements, missing definitions are set to their default value.

RTF_STYLESHEET_FILE    =

# Set optional variables used in the generation of an rtf document.
# Syntax is similar to doxygen's config file.

RTF_EXTENSIONS_FILE    =

#---------------------------------------------------------------------------
# configuration options related to the man page output
#---------------------------------------------------------------------------

# If the GENERATE_MAN tag is set to YES (the default) Doxygen will
# generate man pages

GENERATE_MAN           = NO

# The MAN_OUTPUT tag is used to specify where the man pages will be put.
# If a relative path is entered the value of OUTPUT_DIRECTORY will be
# put in front of it. If left blank `man' will be used as the default path.

MAN_OUTPUT             = man

# The MAN_EXTENSION tag determines the extension that is added to
# the generated man pages (default is the subroutine's section .3)

MAN_EXTENSION          = .3

# If the MAN_LINKS tag is set to YES and Doxygen generates man output,
# then it will generate one additional man file for each entity
# documented in the real man page(s). These additional files
# only source the real man page, but without them the man command
# would be unable to find the correct page. The default is NO.

MAN_LINKS              = NO

#---------------------------------------------------------------------------
# configuration options related to the XML output
#---------------------------------------------------------------------------

# If the GENERATE_XML tag is set to YES Doxygen will
# generate an XML file that captures the structure of
# the code including all documentation.

GENERATE_XML           = NO

# The XML_OUTPUT tag is used to specify where the XML pages will be put.
# If a relative path is entered the value of OUTPUT_DIRECTORY will be
# put in front of it. If left blank `xml' will be used as the default path.

XML_OUTPUT             = xml

# The XML_SCHEMA tag can be used to specify an XML schema,
# which can be used by a validating XML parser to check the
# syntax of the XML files.

XML_SCHEMA             =

# The XML_DTD tag can be used to specify an XML DTD,
# which can be used by a validating XML parser to check the
# syntax of the XML files.

XML_DTD                =

# If the XML_PROGRAMLISTING tag is set to YES Doxygen will
# dump the program listings (including syntax highlighting
# and cross-referencing information) to the XML output. Note that
# enabling this will significantly increase the size of the XML output.

XML_PROGRAMLISTING     = YES

#---------------------------------------------------------------------------
# configuration options for the AutoGen Definitions output
#---------------------------------------------------------------------------

# If the GENERATE_AUTOGEN_DEF tag is set to YES Doxygen will
# generate an AutoGen Definitions (see autogen.sf.net) file
# that captures the structure of the code including all
# documentation. Note that this feature is still experimental
# and incomplete at the moment.

GENERATE_AUTOGEN_DEF   = NO

#---------------------------------------------------------------------------
# configuration options related to the Perl module output
#---------------------------------------------------------------------------

# If the GENERATE_PERLMOD tag is set to YES Doxygen will
# generate a Perl module file that captures the structure of
# the code including all documentation. Note that this
# feature is still experimental and incomplete at the
# moment.

GENERATE_PERLMOD       = NO

# If the PERLMOD_LATEX tag is set to YES Doxygen will generate
# the necessary Makefile rules, Perl scripts and LaTeX code to be able
# to generate PDF and DVI output from the Perl module output.

PERLMOD_LATEX          = NO

# If the PERLMOD_PRETTY tag is set to YES the Perl module output will be
# nicely formatted so it can be parsed by a human reader.
# This is useful
# if you want to understand what is going on.
# On the other hand, if this
# tag is set to NO the size of the Perl module output will be much smaller
# and Perl will parse it just the same.

PERLMOD_PRETTY         = YES

# The names of the make variables in the generated doxyrules.make file
# are prefixed with the string contained in PERLMOD_MAKEVAR_PREFIX.
# This is useful so different doxyrules.make files included by the same
# Makefile don't overwrite each other's variables.

PERLMOD_MAKEVAR_PREFIX =

#---------------------------------------------------------------------------
# Configuration options related to the preprocessor
#---------------------------------------------------------------------------

# If the ENABLE_PREPROCESSING tag is set to YES (the default) Doxygen will
# evaluate all C-preprocessor directives found in the sources and include
# files.

ENABLE_PREPROCESSING   = NO

# If the MACRO_EXPANSION tag is set to YES Doxygen will expand all macro
# names in the source code. If set to NO (the default) only conditional
# compilation will be performed. Macro expansion can be done in a controlled
# way by setting EXPAND_ONLY_PREDEF to YES.

MACRO_EXPANSION        = NO

# If the EXPAND_ONLY_PREDEF and MACRO_EXPANSION tags are both set to YES
# then the macro expansion is limited to the macros specified with the
# PREDEFINED and EXPAND_AS_DEFINED tags.

EXPAND_ONLY_PREDEF     = NO

# If the SEARCH_INCLUDES tag is set to YES (the default) the includes files
# in the INCLUDE_PATH (see below) will be search if a #include is found.

SEARCH_INCLUDES        = YES

# The INCLUDE_PATH tag can be used to specify one or more directories that
# contain include files that are not input files but should be processed by
# the preprocessor.

INCLUDE_PATH           =

# You can use the INCLUDE_FILE_PATTERNS tag to specify one or more wildcard
# patterns (like *.h and *.hpp) to filter out the header-files in the
# directories. If left blank, the patterns specified with FILE_PATTERNS will
# be used.

INCLUDE_FILE_PATTERNS  =

# The PREDEFINED tag can be used to specify one or more macro names that
# are defined before the preprocessor is started (similar to the -D option of
# gcc). The argument of the tag is a list of macros of the form: name
# or name=definition (no spaces). If the definition and the = are
# omitted =1 is assumed. To prevent a macro definition from being
# undefined via #undef or recursively expanded use the := operator
# instead of the = operator.

PREDEFINED             =

# If the MACRO_EXPANSION and EXPAND_ONLY_PREDEF tags are set to YES then
# this tag can be used to specify a list of macro names that should be expanded.
# The macro definition that is found in the sources will be used.
# Use the PREDEFINED tag if you want to use a different macro definition.

EXPAND_AS_DEFINED      =

# If the SKIP_FUNCTION_MACROS tag is set to YES (the default) then
# doxygen's preprocessor will remove all function-like macros that are alone
# on a line, have an all uppercase name, and do not end with a semicolon. Such
# function macros are typically used for boiler-plate code, and will confuse
# the parser if not removed.

SKIP_FUNCTION_MACROS   = YES

#---------------------------------------------------------------------------
# Configuration::additions related to external references
#---------------------------------------------------------------------------

# The TAGFILES option can be used to specify one or more tagfiles.
# Optionally an initial location of the external documentation
# can be added for each tagfile. The format of a tag file without
# this location is as follows:
#
# TAGFILES = file1 file2 ...
# Adding location for the tag files is done as follows:
#
# TAGFILES = file1=loc1 "file2 = loc2" ...
# where "loc1" and "loc2" can be relative or absolute paths or
# URLs. If a location is present for each tag, the installdox tool
# does not have to be run to correct the links.
# Note that each tag file must have a unique name
# (where the name does NOT include the path)
# If a tag file is not located in the directory in which doxygen
# is run, you must also specify the path to the tagfile here.

TAGFILES               =

# When a file name is specified after GENERATE_TAGFILE, doxygen will create
# a tag file that is based on the input files it reads.

GENERATE_TAGFILE       =

# If the ALLEXTERNALS tag is set to YES all external classes will be listed
# in the class index. If set to NO only the inherited external classes
# will be listed.

ALLEXTERNALS           = NO

# If the EXTERNAL_GROUPS tag is set to YES all external groups will be listed
# in the modules index. If set to NO, only the current project's groups will
# be listed.

EXTERNAL_GROUPS        = YES

# The PERL_PATH should be the absolute path and name of the perl script
# interpreter (i.e. the result of `which perl').

PERL_PATH              = /usr/bin/perl

#---------------------------------------------------------------------------
# Configuration options related to the dot tool
#---------------------------------------------------------------------------

# If the CLASS_DIAGRAMS tag is set to YES (the default) Doxygen will
# generate a inheritance diagram (in HTML, RTF and LaTeX) for classes with base
# or super classes. Setting the tag to NO turns the diagrams off. Note that
# this option is superseded by the HAVE_DOT option below. This is only a
# fallback. It is recommended to install and use dot, since it yields more
# powerful graphs.

CLASS_DIAGRAMS         = YES

# You can define message sequence charts within doxygen comments using the \msc
# command. Doxygen will then run the mscgen tool (see
# http://www.mcternan.me.uk/mscgen/) to produce the chart and insert it in the
# documentation. The MSCGEN_PATH tag allows you to specify the directory where
# the mscgen tool resides. If left empty the tool is assumed to be found in the
# default search path.

MSCGEN_PATH            =

# If set to YES, the inheritance and collaboration graphs will hide
# inheritance and usage relations if the target is undocumented
# or is not a class.

HIDE_UNDOC_RELATIONS   = YES

# If you set the HAVE_DOT tag to YES then doxygen will assume the dot tool is
# available from the path. This tool is part of Graphviz, a graph visualization
# toolkit from AT&T and Lucent Bell Labs. The other options in this section
# have no effect if this option is set to NO (the default)

HAVE_DOT               = YES

# By default doxygen will write a font called FreeSans.ttf to the output
# directory and reference it in all dot files that doxygen generates. This
# font does not include all possible unicode characters however, so when you need
# these (or just want a differently looking font) you can specify the font name
# using DOT_FONTNAME. You need need to make sure dot is able to find the font,
# which can be done by putting it in a standard location or by setting the
# DOTFONTPATH environment variable or by setting DOT_FONTPATH to the directory
# containing the font.

DOT_FONTNAME           = FreeSans

# The DOT_FONTSIZE tag can be used to set the size of the font of dot graphs.
# The default size is 10pt.

DOT_FONTSIZE           = 10

# By default doxygen will tell dot to use the output directory to look for the
# FreeSans.ttf font (which doxygen will put there itself). If you specify a
# different font using DOT_FONTNAME you can set the path where dot
# can find it using this tag.

DOT_FONTPATH           =

# If the CLASS_GRAPH and HAVE_DOT tags are set to YES then doxygen
# will generate a graph for each documented class showing the direct and
# indirect inheritance relations. Setting this tag to YES will force the
# the CLASS_DIAGRAMS tag to NO.

CLASS_GRAPH            = YES

# If the COLLABORATION_GRAPH and HAVE_DOT tags are set to YES then doxygen
# will generate a graph for each documented class showing the direct and
# indirect implementation dependencies (inheritance, containment, and
# class references variables) of the class with other documented classes.

COLLABORATION_GRAPH    = NO

# If the GROUP_GRAPHS and HAVE_DOT tags are set to YES then doxygen
# will generate a graph for groups, showing the direct groups dependencies

GROUP_GRAPHS           = NO

# If the UML_LOOK tag is set to YES doxygen will generate inheritance and
# collaboration diagrams in a style similar to the OMG's Unified Modeling
# Language.

UML_LOOK               = NO

# If set to YES, the inheritance and collaboration graphs will show the
# relations between templates and their instances.

TEMPLATE_RELATIONS     = NO

# If the ENABLE_PREPROCESSING, SEARCH_INCLUDES, INCLUDE_GRAPH, and HAVE_DOT
# tags are set to YES then doxygen will generate a graph for each documented
# file showing the direct and indirect include dependencies of the file with
# other documented files.

INCLUDE_GRAPH          = NO

# If the ENABLE_PREPROCESSING, SEARCH_INCLUDES, INCLUDED_BY_GRAPH, and
# HAVE_DOT tags are set to YES then doxygen will generate a graph for each
# documented header file showing the documented files that directly or
# indirectly include this file.

INCLUDED_BY_GRAPH      = NO

# If the CALL_GRAPH and HAVE_DOT options are set to YES then
# doxygen will generate a call dependency graph for every global function
# or class method. Note that enabling this option will significantly increase
# the time of a run. So in most cases it will be better to enable call graphs
# for selected functions only using the \callgraph command.

CALL_GRAPH             = NO

# If the CALLER_GRAPH and HAVE_DOT tags are set to YES then
# doxygen will generate a caller dependency graph for every global function
# or class method. Note that enabling this option will significantly increase
# the time of a run. So in most cases it will be better to enable caller
# graphs for selected functions only using the \callergraph command.

CALLER_GRAPH           = NO

# If the GRAPHICAL_HIERARCHY and HAVE_DOT tags are set to YES then doxygen
# will graphical hierarchy of all classes instead of a textual one.

GRAPHICAL_HIERARCHY    = NO

# If the DIRECTORY_GRAPH, SHOW_DIRECTORIES and HAVE_DOT tags are set to YES
# then doxygen will show the dependencies a directory has on other directories
# in a graphical way. The dependency relations are determined by the #include
# relations between the files in the directories.

DIRECTORY_GRAPH        = NO

# The DOT_IMAGE_FORMAT tag can be used to set the image format of the images
# generated by dot. Possible values are png, jpg, or gif
# If left blank png will be used.

DOT_IMAGE_FORMAT       = png

# The tag DOT_PATH can be used to specify the path where the dot tool can be
# found. If left blank, it is assumed the dot tool can be found in the path.

DOT_PATH               =

# The DOTFILE_DIRS tag can be used to specify one or more directories that
# contain dot files that are included in the documentation (see the
# \dotfile command).

DOTFILE_DIRS           =

# The DOT_GRAPH_MAX_NODES tag can be used to set the maximum number of
# nodes that will be shown in the graph. If the number of nodes in a graph
# becomes larger than this value, doxygen will truncate the graph, which is
# visualized by representing a node as a red box. Note that doxygen if the
# number of direct children of the root node in a graph is already larger than
# DOT_GRAPH_MAX_NODES then the graph will not be shown at all. Also note
# that the size of a graph can be further restricted by MAX_DOT_GRAPH_DEPTH.

DOT_GRAPH_MAX_NODES    = 50

# The MAX_DOT_GRAPH_DEPTH tag can be used to set the maximum depth of the
# graphs generated by dot. A depth value of 3 means that only nodes reachable
# from the root by following a path via at most 3 edges will be shown. Nodes
# that lay further from the root node will be omitted. Note that setting this
# option to 1 or 2 may greatly reduce the computation time needed for large
# code bases. Also note that the size of a graph can be further restricted by
# DOT_GRAPH_MAX_NODES. Using a depth of 0 means no depth restriction.

MAX_DOT_GRAPH_DEPTH    = 0

# Set the DOT_TRANSPARENT tag to YES to generate images with a transparent
# background. This is disabled by default, because dot on Windows does not
# seem to support this out of the box. Warning: Depending on the platform used,
# enabling this option may lead to badly anti-aliased labels on the edges of
# a graph (i.e. they become hard to read).

DOT_TRANSPARENT        = NO

# Set the DOT_MULTI_TARGETS tag to YES allow dot to generate multiple output
# files in one run (i.e. multiple -o and -T options on the command line). This
# makes dot run faster, but since only newer versions of dot (>1.8.10)
# support this, this feature is disabled by default.

DOT_MULTI_TARGETS      = YES

# If the GENERATE_LEGEND tag is set to YES (the default) Doxygen will
# generate a legend page explaining the meaning of the various boxes and
# arrows in the dot generated graphs.

GENERATE_LEGEND        = NO

# If the DOT_CLEANUP tag is set to YES (the default) Doxygen will
# remove the intermediate dot files that are used to generate
# the various graphs.

DOT_CLEANUP            = YES

    steps: README
======

This directory contains the documentation automatically generated by Doxygen.

Contents
========

  o Installing the necessary packages in Ubuntu
  o Generating documentation
  o References

Installing the necessary packages in Ubuntu
===========================================

1. Install the following packages.

  $ sudo aptitude install doxygen doxygen-doc doxygen-gui dot2tex graphviz

2. (Optional) Install Doxygen from the latest sourcode.

  The Ubuntu package is outdated. The newer the version of Doxygen, the better
  the documentation looks.

  Place yourself in some temporary folder where you can download the source,
  and run [1]:

  $ svn co https://doxygen.svn.sourceforge.net/svnroot/doxygen/trunk doxygen-svn
  $ cd doxygen-svn
  $ ./configure
  $ make
  $ make install

Generating documentation
========================

Two ways described here:

1. Use the provided gendoc.sh script.

    trunk/NXWidgets/Doxygen/gendoc.sh

  The script only needs the argument to the absolute path where to place the
  generated documentation. I.e.:

    $ cd /path/to/nuttx/trunk/NXWidgets/Doxygen/
    $ mkdir doc
    $ ./gendoc.sh $PWD/doc


2. Using the Doxyfile directly:

   The file "Doxyfile" contains the configuration of the Doxygen settings
   for the run, edit only if necessary.

   To generate the documentation type:

    $ cd /path/to/nuttx/trunk/NXWidgets/Doxygen/
    $ doxygen Doxyfile

References
==========

[1] http://www.stack.nl/~dimitri/doxygen/download.html
    - uses: #!/bin/bash
#################################################################################
# NxWidgets/Doxygen/gendoc.sh
#
#   Copyright (C) 2012. All rights reserved.
#   
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name NuttX, NxWidgets, nor the names of its contributors
#    me be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
#################################################################################
#
# set -x
# Functions

function ShowUsage()
{
    echo ""
    echo "USAGE: $0 <doxygen-output-directory-path>"
    echo ""
    echo "Where:"
    echo " <doxygen-output-directory-path> is the full, absolut path to place the doxygen output"
    echo ""
}

# Input parameters

DOXYGENOUTPUT_DIR=$1
if [ -z "${DOXYGENOUTPUT_DIR}" ]; then
    echo "Missing required arguments"
    ShowUsage
    exit 1
fi

# Check that the directory exist

if [ ! -d "${DOXYGENOUTPUT_DIR}" ]; then
    echo "Directory ${DOXYGENOUTPUT_DIR} does not exist"
    exit 1
fi

# Find the doxygen configuration file

DOXYFILE="Doxyfile"
if [ ! -e "${DOXYFILE}" ]; then
    echo "This script must be executed in the documentation/ directory"
    exit 1
fi

doxygen "${DOXYFILE}" || \
    {
        echo "Failed to run doxygen"; \
        exit 1;

    }

cp -rf html "${DOXYGENOUTPUT_DIR}" || \
    {
        echo "Failed to move html output"; \
        exit 1;
    }

rm -rf html || \
    {
        echo "Failed to remove the html/ directory"; \
        exit 1;
    }

echo "open ${DOXYGENOUTPUT_DIR}/html/index.html to start browsing"


    - name: # Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY} in
# about every subdir
# since cmake 2.4.0
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Put the include dirs which are in the source or build tree
# before all other include dirs, so the headers in the sources
# are prefered over the already installed ones
# since cmake 2.4.1
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# Use colored output
# since cmake 2.4.0
set(CMAKE_COLOR_MAKEFILE ON)

# Define the generic version of the libraries here
set(GENERIC_LIB_VERSION "0.1.0")
set(GENERIC_LIB_SOVERSION "0")

# Set the default build type to release with debug info
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo
    CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
  )
endif (NOT CMAKE_BUILD_TYPE)

# disallow in-source build
include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. 
Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")
      # Turn repeated input strings (such as the build output directory) into step outputs. These step outputs can be used throughout the workflow file.
      id: # define system dependent compiler flags

include(CheckCCompilerFlag)
include(MacroCheckCCompilerFlagSSP)

#
# Define GNUCC compiler flags
#
if (${CMAKE_C_COMPILER_ID} MATCHES GNU)

    # add -Wconversion ?
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pedantic -pedantic-errors")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Wmissing-prototypes -Wdeclaration-after-statement")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wformat-security")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute")

    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -pedantic-errors")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wformat-security")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-format-attribute")

    if (UNIX AND NOT WIN32)

        # with -fPIC
        check_c_compiler_flag("-fPIC" WITH_FPIC)
        if (WITH_FPIC)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
        endif (WITH_FPIC)

    endif(UNIX AND NOT WIN32)

    check_c_compiler_flag_ssp("-fstack-protector" WITH_STACK_PROTECTOR)
    if (WITH_STACK_PROTECTOR)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fstack-protector")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKDER_FLAGS} -fstack-protector")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKDER_FLAGS} -fstack-protector")
    endif (WITH_STACK_PROTECTOR)

    check_c_compiler_flag("-D_FORTIFY_SOURCE=2" WITH_FORTIFY_SOURCE)
    if (WITH_FORTIFY_SOURCE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2")
    endif (WITH_FORTIFY_SOURCE)
endif (${CMAKE_C_COMPILER_ID} MATCHES GNU)

if (UNIX AND NOT WIN32)
    #
    # Check for large filesystem support
    #
    if (CMAKE_SIZEOF_VOID_P MATCHES "8")
        # with large file support
        execute_process(
            COMMAND
                getconf LFS64_CFLAGS
            OUTPUT_VARIABLE
                _lfs_CFLAGS
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    else (CMAKE_SIZEOF_VOID_P MATCHES "8")
        # with large file support
        execute_process(
            COMMAND
                getconf LFS_CFLAGS
            OUTPUT_VARIABLE
                _lfs_CFLAGS
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
    if (_lfs_CFLAGS)
        string(REGEX REPLACE "[\r\n]" " " "${_lfs_CFLAGS}" "${${_lfs_CFLAGS}}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_lfs_CFLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_lfs_CFLAGS}")
    endif (_lfs_CFLAGS)

endif (UNIX AND NOT WIN32)

if (MSVC)
    # Use secure functions by defaualt and suppress warnings about
    #"deprecated" functions
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_NONSTDC_NO_WARNINGS=1 /D _CRT_SECURE_NO_WARNINGS=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")
endif (MSVC)
      shell: # - Create custom targets to build projects in external trees
# The 'ExternalProjectWithFilename_Add' function creates a custom target to drive
# download, update/patch, configure, build, install and test steps of an
# external project:
#  ExternalProjectWithFilename_Add(<name>    # Name for custom target
#    [DEPENDS projects...]       # Targets on which the project depends
#    [PREFIX dir]                # Root dir for entire project
#    [LIST_SEPARATOR sep]        # Sep to be replaced by ; in cmd lines
#    [TMP_DIR dir]               # Directory to store temporary files
#    [STAMP_DIR dir]             # Directory to store step timestamps
#   #--Download step--------------
#    [FILENAME filename]         # Set the download filename
#    [DOWNLOAD_DIR dir]          # Directory to store downloaded files
#    [DOWNLOAD_COMMAND cmd...]   # Command to download source tree
#    [CVS_REPOSITORY cvsroot]    # CVSROOT of CVS repository
#    [CVS_MODULE mod]            # Module to checkout from CVS repo
#    [CVS_TAG tag]               # Tag to checkout from CVS repo
#    [SVN_REPOSITORY url]        # URL of Subversion repo
#    [SVN_REVISION rev]          # Revision to checkout from Subversion repo
#    [SVN_USERNAME john ]        # Username for Subversion checkout and update
#    [SVN_PASSWORD doe ]         # Password for Subversion checkout and update
#    [SVN_TRUST_CERT 1 ]         # Trust the Subversion server site certificate
#    [GIT_REPOSITORY url]        # URL of git repo
#    [GIT_TAG tag]               # Git branch name, commit id or tag
#    [URL /.../src.tgz]          # Full path or URL of source
#    [URL_MD5 md5]               # MD5 checksum of file at URL
#    [TIMEOUT seconds]           # Time allowed for file download operations
#   #--Update/Patch step----------
#    [UPDATE_COMMAND cmd...]     # Source work-tree update command
#    [PATCH_COMMAND cmd...]      # Command to patch downloaded source
#   #--Configure step-------------
#    [SOURCE_DIR dir]            # Source dir to be used for build
#    [CONFIGURE_COMMAND cmd...]  # Build tree configuration command
#    [CMAKE_COMMAND /.../cmake]  # Specify alternative cmake executable
#    [CMAKE_GENERATOR gen]       # Specify generator for native build
#    [CMAKE_ARGS args...]        # Arguments to CMake command line
#    [CMAKE_CACHE_ARGS args...]  # Initial cache arguments, of the form -Dvar:string=on
#   #--Build step-----------------
#    [BINARY_DIR dir]            # Specify build dir location
#    [BUILD_COMMAND cmd...]      # Command to drive the native build
#    [BUILD_IN_SOURCE 1]         # Use source dir for build dir
#   #--Install step---------------
#    [INSTALL_DIR dir]           # Installation prefix
#    [INSTALL_COMMAND cmd...]    # Command to drive install after build
#   #--Test step------------------
#    [TEST_BEFORE_INSTALL 1]     # Add test step executed before install step
#    [TEST_AFTER_INSTALL 1]      # Add test step executed after install step
#    [TEST_COMMAND cmd...]       # Command to drive test
#   #--Output logging-------------
#    [LOG_DOWNLOAD 1]            # Wrap download in script to log output
#    [LOG_UPDATE 1]              # Wrap update in script to log output
#    [LOG_CONFIGURE 1]           # Wrap configure in script to log output
#    [LOG_BUILD 1]               # Wrap build in script to log output
#    [LOG_TEST 1]                # Wrap test in script to log output
#    [LOG_INSTALL 1]             # Wrap install in script to log output
#   #--Custom targets-------------
#    [STEP_TARGETS st1 st2 ...]  # Generate custom targets for these steps
#    )
# The *_DIR options specify directories for the project, with default
# directories computed as follows.
# If the PREFIX option is given to ExternalProjectWithFilename_Add() or the EP_PREFIX
# directory property is set, then an external project is built and installed
# under the specified prefix:
#   TMP_DIR      = <prefix>/tmp
#   STAMP_DIR    = <prefix>/src/<name>-stamp
#   DOWNLOAD_DIR = <prefix>/src
#   SOURCE_DIR   = <prefix>/src/<name>
#   BINARY_DIR   = <prefix>/src/<name>-build
#   INSTALL_DIR  = <prefix>
# Otherwise, if the EP_BASE directory property is set then components
# of an external project are stored under the specified base:
#   TMP_DIR      = <base>/tmp/<name>
#   STAMP_DIR    = <base>/Stamp/<name>
#   DOWNLOAD_DIR = <base>/Download/<name>
#   SOURCE_DIR   = <base>/Source/<name>
#   BINARY_DIR   = <base>/Build/<name>
#   INSTALL_DIR  = <base>/Install/<name>
# If no PREFIX, EP_PREFIX, or EP_BASE is specified then the default
# is to set PREFIX to "<name>-prefix".
# Relative paths are interpreted with respect to the build directory
# corresponding to the source directory in which ExternalProjectWithFilename_Add is
# invoked.
#
# If SOURCE_DIR is explicitly set to an existing directory the project
# will be built from it.
# Otherwise a download step must be specified using one of the
# DOWNLOAD_COMMAND, CVS_*, SVN_*, or URL options.
# The URL option may refer locally to a directory or source tarball,
# or refer to a remote tarball (e.g. http://.../src.tgz).
#
# The 'ExternalProjectWithFilename_Add_Step' function adds a custom step to an external
# project:
#  ExternalProjectWithFilename_Add_Step(<name> <step> # Names of project and custom step
#    [COMMAND cmd...]        # Command line invoked by this step
#    [COMMENT "text..."]     # Text printed when step executes
#    [DEPENDEES steps...]    # Steps on which this step depends
#    [DEPENDERS steps...]    # Steps that depend on this step
#    [DEPENDS files...]      # Files on which this step depends
#    [ALWAYS 1]              # No stamp file, step always runs
#    [WORKING_DIRECTORY dir] # Working directory for command
#    [LOG 1]                 # Wrap step in script to log output
#    )
# The command line, comment, and working directory of every standard
# and custom step is processed to replace tokens
# <SOURCE_DIR>,
# <BINARY_DIR>,
# <INSTALL_DIR>,
# and <TMP_DIR>
# with corresponding property values.
#
# The 'ExternalProjectWithFilename_Get_Property' function retrieves external project
# target properties:
#  ExternalProjectWithFilename_Get_Property(<name> [prop1 [prop2 [...]]])
# It stores property values in variables of the same name.
# Property names correspond to the keyword argument names of
# 'ExternalProjectWithFilename_Add'.
#
# The 'ExternalProjectWithFilename_Add_StepTargets' function generates custom targets for
# the steps listed:
#  ExternalProjectWithFilename_Add_StepTargets(<name> [step1 [step2 [...]]])
#
# If STEP_TARGETS is set then ExternalProjectWithFilename_Add_StepTargets is automatically
# called at the end of matching calls to ExternalProjectWithFilename_Add_Step. Pass
# STEP_TARGETS explicitly to individual ExternalProjectWithFilename_Add calls, or
# implicitly to all ExternalProjectWithFilename_Add calls by setting the directory property
# EP_STEP_TARGETS.
#
# If STEP_TARGETS is not set, clients may still manually call
# ExternalProjectWithFilename_Add_StepTargets after calling ExternalProjectWithFilename_Add or
# ExternalProjectWithFilename_Add_Step.
#
# This functionality is provided to make it easy to drive the steps
# independently of each other by specifying targets on build command lines.
# For example, you may be submitting to a sub-project based dashboard, where
# you want to drive the configure portion of the build, then submit to the
# dashboard, followed by the build portion, followed by tests. If you invoke
# a custom target that depends on a step halfway through the step dependency
# chain, then all the previous steps will also run to ensure everything is
# up to date.
#
# For example, to drive configure, build and test steps independently for each
# ExternalProjectWithFilename_Add call in your project, write the following line prior to
# any ExternalProjectWithFilename_Add calls in your CMakeLists file:
#
#   set_property(DIRECTORY PROPERTY EP_STEP_TARGETS configure build test)

#=============================================================================
# Copyright 2008-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# Pre-compute a regex to match documented keywords for each command.
math(EXPR _ep_documentation_line_count "${CMAKE_CURRENT_LIST_LINE} - 16")
file(STRINGS "${CMAKE_CURRENT_LIST_FILE}" lines
     LIMIT_COUNT ${_ep_documentation_line_count}
     REGEX "^#  (  \\[[A-Z0-9_]+ [^]]*\\] +#.*$|[A-Za-z0-9_]+\\()")
foreach(line IN LISTS lines)
  if("${line}" MATCHES "^#  [A-Za-z0-9_]+\\(")
    if(_ep_func)
      set(_ep_keywords_${_ep_func} "${_ep_keywords_${_ep_func}})$")
    endif()
    string(REGEX REPLACE "^#  ([A-Za-z0-9_]+)\\(.*" "\\1" _ep_func "${line}")
    #message("function [${_ep_func}]")
    set(_ep_keywords_${_ep_func} "^(")
    set(_ep_keyword_sep)
  else()
    string(REGEX REPLACE "^#    \\[([A-Z0-9_]+) .*" "\\1" _ep_key "${line}")
    #message("  keyword [${_ep_key}]")
    set(_ep_keywords_${_ep_func}
      "${_ep_keywords_${_ep_func}}${_ep_keyword_sep}${_ep_key}")
    set(_ep_keyword_sep "|")
  endif()
endforeach()
if(_ep_func)
  set(_ep_keywords_${_ep_func} "${_ep_keywords_${_ep_func}})$")
endif()


function(_ep_parse_arguments f name ns args)
  # Transfer the arguments to this function into target properties for the
  # new custom target we just added so that we can set up all the build steps
  # correctly based on target properties.
  #
  # We loop through ARGN and consider the namespace starting with an
  # upper-case letter followed by at least two more upper-case letters,
  # numbers or underscores to be keywords.
  set(key)

  foreach(arg IN LISTS args)
    set(is_value 1)

    if(arg MATCHES "^[A-Z][A-Z0-9_][A-Z0-9_]+$" AND
        NOT ((arg STREQUAL "${key}") AND (key STREQUAL "COMMAND")) AND
        NOT arg MATCHES "^(TRUE|FALSE)$")
      if(_ep_keywords_${f} AND arg MATCHES "${_ep_keywords_${f}}")
        set(is_value 0)
      endif()
    endif()

    if(is_value)
      if(key)
        # Value
        if(NOT arg STREQUAL "")
          set_property(TARGET ${name} APPEND PROPERTY ${ns}${key} "${arg}")
        else()
          get_property(have_key TARGET ${name} PROPERTY ${ns}${key} SET)
          if(have_key)
            get_property(value TARGET ${name} PROPERTY ${ns}${key})
            set_property(TARGET ${name} PROPERTY ${ns}${key} "${value};${arg}")
          else()
            set_property(TARGET ${name} PROPERTY ${ns}${key} "${arg}")
          endif()
        endif()
      else()
        # Missing Keyword
        message(AUTHOR_WARNING "value '${arg}' with no previous keyword in ${f}")
      endif()
    else()
      set(key "${arg}")
    endif()
  endforeach()
endfunction(_ep_parse_arguments)


define_property(DIRECTORY PROPERTY "EP_BASE" INHERITED
  BRIEF_DOCS "Base directory for External Project storage."
  FULL_DOCS
  "See documentation of the ExternalProjectWithFilename_Add() function in the "
  "ExternalProjectWithFilename module."
  )

define_property(DIRECTORY PROPERTY "EP_PREFIX" INHERITED
  BRIEF_DOCS "Top prefix for External Project storage."
  FULL_DOCS
  "See documentation of the ExternalProjectWithFilename_Add() function in the "
  "ExternalProjectWithFilename module."
  )

define_property(DIRECTORY PROPERTY "EP_STEP_TARGETS" INHERITED
  BRIEF_DOCS
  "List of ExternalProjectWithFilename steps that automatically get corresponding targets"
  FULL_DOCS
  "See documentation of the ExternalProjectWithFilename_Add_StepTargets() function in the "
  "ExternalProjectWithFilename module."
  )


function(_ep_write_gitclone_script script_filename source_dir git_EXECUTABLE git_repository git_tag src_name work_dir)
  file(WRITE ${script_filename}
"if(\"${git_tag}\" STREQUAL \"\")
  message(FATAL_ERROR \"Tag for git checkout should not be empty.\")
endif()

execute_process(
  COMMAND \${CMAKE_COMMAND} -E remove_directory \"${source_dir}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to remove directory: '${source_dir}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" clone \"${git_repository}\" \"${src_name}\"
  WORKING_DIRECTORY \"${work_dir}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to clone repository: '${git_repository}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" checkout ${git_tag}
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to checkout tag: '${git_tag}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" submodule init
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to init submodules in: '${work_dir}/${src_name}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" submodule update --recursive
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to update submodules in: '${work_dir}/${src_name}'\")
endif()

"
)

endfunction(_ep_write_gitclone_script)


function(_ep_write_downloadfile_script script_filename remote local timeout md5)
  if(timeout)
    set(timeout_args TIMEOUT ${timeout})
    set(timeout_msg "${timeout} seconds")
  else()
    set(timeout_args "# no TIMEOUT")
    set(timeout_msg "none")
  endif()

  if(md5)
    set(md5_args EXPECTED_MD5 ${md5})
  else()
    set(md5_args "# no EXPECTED_MD5")
  endif()

  file(WRITE ${script_filename}
"message(STATUS \"downloading...
     src='${remote}'
     dst='${local}'
     timeout='${timeout_msg}'\")

file(DOWNLOAD
  \"${remote}\"
  \"${local}\"
  SHOW_PROGRESS
  ${md5_args}
  ${timeout_args}
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR \"error: downloading '${remote}' failed
  status_code: \${status_code}
  status_string: \${status_string}
  log: \${log}
\")
endif()

message(STATUS \"downloading... done\")
"
)

endfunction(_ep_write_downloadfile_script)


function(_ep_write_verifyfile_script script_filename local md5)
  file(WRITE ${script_filename}
"message(STATUS \"verifying file...
     file='${local}'\")

set(verified 0)

# If an expected md5 checksum exists, compare against it:
#
if(NOT \"${md5}\" STREQUAL \"\")
  execute_process(COMMAND \${CMAKE_COMMAND} -E md5sum \"${local}\"
    OUTPUT_VARIABLE ov
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE rv)

  if(NOT rv EQUAL 0)
    message(FATAL_ERROR \"error: computing md5sum of '${local}' failed\")
  endif()

  string(REGEX MATCH \"^([0-9A-Fa-f]+)\" md5_actual \"\${ov}\")

  string(TOLOWER \"\${md5_actual}\" md5_actual)
  string(TOLOWER \"${md5}\" md5)

  if(NOT \"\${md5}\" STREQUAL \"\${md5_actual}\")
    message(FATAL_ERROR \"error: md5sum of '${local}' does not match expected value
  md5_expected: \${md5}
    md5_actual: \${md5_actual}
\")
  endif()

  set(verified 1)
endif()

if(verified)
  message(STATUS \"verifying file... done\")
else()
  message(STATUS \"verifying file... warning: did not verify file - no URL_MD5 checksum argument? corrupt file?\")
endif()
"
)

endfunction(_ep_write_verifyfile_script)


function(_ep_write_extractfile_script script_filename name filename directory)
  set(args "")

  if(filename MATCHES "(\\.|=)(bz2|tar\\.gz|tgz|zip)$")
    set(args xfz)
  endif()

  if(filename MATCHES "(\\.|=)tar$")
    set(args xf)
  endif()

  if(args STREQUAL "")
    message(SEND_ERROR "error: do not know how to extract '${filename}' -- known types are .bz2, .tar, .tar.gz, .tgz and .zip")
    return()
  endif()

  file(WRITE ${script_filename}
"# Make file names absolute:
#
get_filename_component(filename \"${filename}\" ABSOLUTE)
get_filename_component(directory \"${directory}\" ABSOLUTE)

message(STATUS \"extracting...
     src='\${filename}'
     dst='\${directory}'\")

if(NOT EXISTS \"\${filename}\")
  message(FATAL_ERROR \"error: file to extract does not exist: '\${filename}'\")
endif()

# Prepare a space for extracting:
#
set(i 1234)
while(EXISTS \"\${directory}/../ex-${name}\${i}\")
  math(EXPR i \"\${i} + 1\")
endwhile()
set(ut_dir \"\${directory}/../ex-${name}\${i}\")
file(MAKE_DIRECTORY \"\${ut_dir}\")

# Extract it:
#
message(STATUS \"extracting... [tar ${args}]\")
execute_process(COMMAND \${CMAKE_COMMAND} -E tar ${args} \${filename}
  WORKING_DIRECTORY \${ut_dir}
  RESULT_VARIABLE rv)

if(NOT rv EQUAL 0)
  message(STATUS \"extracting... [error clean up]\")
  file(REMOVE_RECURSE \"\${ut_dir}\")
  message(FATAL_ERROR \"error: extract of '\${filename}' failed\")
endif()

# Analyze what came out of the tar file:
#
message(STATUS \"extracting... [analysis]\")
file(GLOB contents \"\${ut_dir}/*\")
list(LENGTH contents n)
if(NOT n EQUAL 1 OR NOT IS_DIRECTORY \"\${contents}\")
  set(contents \"\${ut_dir}\")
endif()

# Move \"the one\" directory to the final directory:
#
message(STATUS \"extracting... [rename]\")
file(REMOVE_RECURSE \${directory})
get_filename_component(contents \${contents} ABSOLUTE)
file(RENAME \${contents} \${directory})

# Clean up:
#
message(STATUS \"extracting... [clean up]\")
file(REMOVE_RECURSE \"\${ut_dir}\")

message(STATUS \"extracting... done\")
"
)

endfunction(_ep_write_extractfile_script)


function(_ep_set_directories name)
  get_property(prefix TARGET ${name} PROPERTY _EP_PREFIX)
  if(NOT prefix)
    get_property(prefix DIRECTORY PROPERTY EP_PREFIX)
    if(NOT prefix)
      get_property(base DIRECTORY PROPERTY EP_BASE)
      if(NOT base)
        set(prefix "${name}-prefix")
      endif()
    endif()
  endif()
  if(prefix)
    set(tmp_default "${prefix}/tmp")
    set(download_default "${prefix}/src")
    set(source_default "${prefix}/src/${name}")
    set(binary_default "${prefix}/src/${name}-build")
    set(stamp_default "${prefix}/src/${name}-stamp")
    set(install_default "${prefix}")
  else() # assert(base)
    set(tmp_default "${base}/tmp/${name}")
    set(download_default "${base}/Download/${name}")
    set(source_default "${base}/Source/${name}")
    set(binary_default "${base}/Build/${name}")
    set(stamp_default "${base}/Stamp/${name}")
    set(install_default "${base}/Install/${name}")
  endif()
  get_property(build_in_source TARGET ${name} PROPERTY _EP_BUILD_IN_SOURCE)
  if(build_in_source)
    get_property(have_binary_dir TARGET ${name} PROPERTY _EP_BINARY_DIR SET)
    if(have_binary_dir)
      message(FATAL_ERROR
        "External project ${name} has both BINARY_DIR and BUILD_IN_SOURCE!")
    endif()
  endif()
  set(top "${CMAKE_CURRENT_BINARY_DIR}")
  set(places stamp download source binary install tmp)
  foreach(var ${places})
    string(TOUPPER "${var}" VAR)
    get_property(${var}_dir TARGET ${name} PROPERTY _EP_${VAR}_DIR)
    if(NOT ${var}_dir)
      set(${var}_dir "${${var}_default}")
    endif()
    if(NOT IS_ABSOLUTE "${${var}_dir}")
      get_filename_component(${var}_dir "${top}/${${var}_dir}" ABSOLUTE)
    endif()
    set_property(TARGET ${name} PROPERTY _EP_${VAR}_DIR "${${var}_dir}")
  endforeach()
  if(build_in_source)
    get_property(source_dir TARGET ${name} PROPERTY _EP_SOURCE_DIR)
    set_property(TARGET ${name} PROPERTY _EP_BINARY_DIR "${source_dir}")
  endif()

  # Make the directories at CMake configure time *and* add a custom command
  # to make them at build time. They need to exist at makefile generation
  # time for Borland make and wmake so that CMake may generate makefiles
  # with "cd C:\short\paths\with\no\spaces" commands in them.
  #
  # Additionally, the add_custom_command is still used in case somebody
  # removes one of the necessary directories and tries to rebuild without
  # re-running cmake.
  foreach(var ${places})
    string(TOUPPER "${var}" VAR)
    get_property(dir TARGET ${name} PROPERTY _EP_${VAR}_DIR)
    file(MAKE_DIRECTORY "${dir}")
    if(NOT EXISTS "${dir}")
      message(FATAL_ERROR "dir '${dir}' does not exist after file(MAKE_DIRECTORY)")
    endif()
  endforeach()
endfunction(_ep_set_directories)


# IMPORTANT: this MUST be a macro and not a function because of the
# in-place replacements that occur in each ${var}
#
macro(_ep_replace_location_tags target_name)
  set(vars ${ARGN})
  foreach(var ${vars})
    if(${var})
      foreach(dir SOURCE_DIR BINARY_DIR INSTALL_DIR TMP_DIR)
        get_property(val TARGET ${target_name} PROPERTY _EP_${dir})
        string(REPLACE "<${dir}>" "${val}" ${var} "${${var}}")
      endforeach()
    endif()
  endforeach()
endmacro()


function(_ep_write_initial_cache target_name script_filename args)
  # Write out values into an initial cache, that will be passed to CMake with -C
  set(script_initial_cache "")
  set(regex "^([^:]+):([^=]+)=(.*)$")
  set(setArg "")
  foreach(line ${args})
    if("${line}" MATCHES "^-D")
      if(setArg)
        # This is required to build up lists in variables, or complete an entry
        set(setArg "${setArg}${accumulator}\" CACHE ${type} \"Initial cache\" FORCE)")
        set(script_initial_cache "${script_initial_cache}\n${setArg}")
        set(accumulator "")
        set(setArg "")
      endif()
      string(REGEX REPLACE "^-D" "" line ${line})
      if("${line}" MATCHES "${regex}")
        string(REGEX MATCH "${regex}" match "${line}")
        set(name "${CMAKE_MATCH_1}")
        set(type "${CMAKE_MATCH_2}")
        set(value "${CMAKE_MATCH_3}")
        set(setArg "set(${name} \"${value}")
      else()
        message(WARNING "Line '${line}' does not match regex. Ignoring.")
      endif()
    else()
      # Assume this is a list to append to the last var
      set(accumulator "${accumulator};${line}")
    endif()
  endforeach()
  # Catch the final line of the args
  if(setArg)
    set(setArg "${setArg}${accumulator}\" CACHE ${type} \"Initial cache\" FORCE)")
    set(script_initial_cache "${script_initial_cache}\n${setArg}")
  endif()
  # Replace location tags.
  _ep_replace_location_tags(${target_name} script_initial_cache)
  # Write out the initial cache file to the location specified.
  if(NOT EXISTS "${script_filename}.in")
    file(WRITE "${script_filename}.in" "\@script_initial_cache\@\n")
  endif()
  configure_file("${script_filename}.in" "${script_filename}")
endfunction(_ep_write_initial_cache)


function(ExternalProjectWithFilename_Get_Property name)
  foreach(var ${ARGN})
    string(TOUPPER "${var}" VAR)
    get_property(${var} TARGET ${name} PROPERTY _EP_${VAR})
    if(NOT ${var})
      message(FATAL_ERROR "External project \"${name}\" has no ${var}")
    endif()
    set(${var} "${${var}}" PARENT_SCOPE)
  endforeach()
endfunction(ExternalProjectWithFilename_Get_Property)


function(_ep_get_configure_command_id name cfg_cmd_id_var)
  get_target_property(cmd ${name} _EP_CONFIGURE_COMMAND)

  if(cmd STREQUAL "")
    # Explicit empty string means no configure step for this project
    set(${cfg_cmd_id_var} "none" PARENT_SCOPE)
  else()
    if(NOT cmd)
      # Default is "use cmake":
      set(${cfg_cmd_id_var} "cmake" PARENT_SCOPE)
    else()
      # Otherwise we have to analyze the value:
      if(cmd MATCHES "^[^;]*/configure")
        set(${cfg_cmd_id_var} "configure" PARENT_SCOPE)
      elseif(cmd MATCHES "^[^;]*/cmake" AND NOT cmd MATCHES ";-[PE];")
        set(${cfg_cmd_id_var} "cmake" PARENT_SCOPE)
      elseif(cmd MATCHES "config")
        set(${cfg_cmd_id_var} "configure" PARENT_SCOPE)
      else()
        set(${cfg_cmd_id_var} "unknown:${cmd}" PARENT_SCOPE)
      endif()
    endif()
  endif()
endfunction(_ep_get_configure_command_id)


function(_ep_get_build_command name step cmd_var)
  set(cmd "${${cmd_var}}")
  if(NOT cmd)
    set(args)
    _ep_get_configure_command_id(${name} cfg_cmd_id)
    if(cfg_cmd_id STREQUAL "cmake")
      # CMake project.  Select build command based on generator.
      get_target_property(cmake_generator ${name} _EP_CMAKE_GENERATOR)
      if("${CMAKE_GENERATOR}" MATCHES "Make" AND
         ("${cmake_generator}" MATCHES "Make" OR NOT cmake_generator))
        # The project uses the same Makefile generator.  Use recursive make.
        set(cmd "$(MAKE)")
        if(step STREQUAL "INSTALL")
          set(args install)
        endif()
        if(step STREQUAL "TEST")
          set(args test)
        endif()
      else()
        # Drive the project with "cmake --build".
        get_target_property(cmake_command ${name} _EP_CMAKE_COMMAND)
        if(cmake_command)
          set(cmd "${cmake_command}")
        else()
          set(cmd "${CMAKE_COMMAND}")
        endif()
        set(args --build ${binary_dir} --config ${CMAKE_CFG_INTDIR})
        if(step STREQUAL "INSTALL")
          list(APPEND args --target install)
        endif()
        # But for "TEST" drive the project with corresponding "ctest".
        if(step STREQUAL "TEST")
          string(REGEX REPLACE "^(.*/)cmake([^/]*)$" "\\1ctest\\2" cmd "${cmd}")
          set(args "")
        endif()
      endif()
    else() # if(cfg_cmd_id STREQUAL "configure")
      # Non-CMake project.  Guess "make" and "make install" and "make test".
      # But use "$(MAKE)" to get recursive parallel make.
      set(cmd "$(MAKE)")
      if(step STREQUAL "INSTALL")
        set(args install)
      endif()
      if(step STREQUAL "TEST")
        set(args test)
      endif()
    endif()

    # Use user-specified arguments instead of default arguments, if any.
    get_property(have_args TARGET ${name} PROPERTY _EP_${step}_ARGS SET)
    if(have_args)
      get_target_property(args ${name} _EP_${step}_ARGS)
    endif()

    list(APPEND cmd ${args})
  endif()

  set(${cmd_var} "${cmd}" PARENT_SCOPE)
endfunction(_ep_get_build_command)

function(_ep_write_log_script name step cmd_var)
  ExternalProjectWithFilename_Get_Property(${name} stamp_dir)
  set(command "${${cmd_var}}")

  set(make "")
  set(code_cygpath_make "")
  if("${command}" MATCHES "^\\$\\(MAKE\\)")
    # GNU make recognizes the string "$(MAKE)" as recursive make, so
    # ensure that it appears directly in the makefile.
    string(REGEX REPLACE "^\\$\\(MAKE\\)" "\${make}" command "${command}")
    set(make "-Dmake=$(MAKE)")

    if(WIN32 AND NOT CYGWIN)
      set(code_cygpath_make "
if(\${make} MATCHES \"^/\")
  execute_process(
    COMMAND cygpath -w \${make}
    OUTPUT_VARIABLE cygpath_make
    ERROR_VARIABLE cygpath_make
    RESULT_VARIABLE cygpath_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NOT cygpath_error)
    set(make \${cygpath_make})
  endif()
endif()
")
    endif()
  endif()

  set(config "")
  if("${CMAKE_CFG_INTDIR}" MATCHES "^\\$")
    string(REPLACE "${CMAKE_CFG_INTDIR}" "\${config}" command "${command}")
    set(config "-Dconfig=${CMAKE_CFG_INTDIR}")
  endif()

  # Wrap multiple 'COMMAND' lines up into a second-level wrapper
  # script so all output can be sent to one log file.
  if("${command}" MATCHES ";COMMAND;")
    set(code_execute_process "
${code_cygpath_make}
execute_process(COMMAND \${command} RESULT_VARIABLE result)
if(result)
  set(msg \"Command failed (\${result}):\\n\")
  foreach(arg IN LISTS command)
    set(msg \"\${msg} '\${arg}'\")
  endforeach(arg)
  message(FATAL_ERROR \"\${msg}\")
endif()
")
    set(code "")
    set(cmd "")
    set(sep "")
    foreach(arg IN LISTS command)
      if("x${arg}" STREQUAL "xCOMMAND")
        set(code "${code}set(command \"${cmd}\")${code_execute_process}")
        set(cmd "")
        set(sep "")
      else()
        set(cmd "${cmd}${sep}${arg}")
        set(sep ";")
      endif()
    endforeach()
    set(code "set(ENV{VS_UNICODE_OUTPUT} \"\")\n${code}set(command \"${cmd}\")${code_execute_process}")
    file(WRITE ${stamp_dir}/${name}-${step}-impl.cmake "${code}")
    set(command ${CMAKE_COMMAND} "-Dmake=\${make}" "-Dconfig=\${config}" -P ${stamp_dir}/${name}-${step}-impl.cmake)
  endif()

  # Wrap the command in a script to log output to files.
  set(script ${stamp_dir}/${name}-${step}.cmake)
  set(logbase ${stamp_dir}/${name}-${step})
  file(WRITE ${script} "
${code_cygpath_make}
set(ENV{VS_UNICODE_OUTPUT} \"\")
set(command \"${command}\")
execute_process(
  COMMAND \${command}
  RESULT_VARIABLE result
  OUTPUT_FILE \"${logbase}-out.log\"
  ERROR_FILE \"${logbase}-err.log\"
  )
if(result)
  set(msg \"Command failed: \${result}\\n\")
  foreach(arg IN LISTS command)
    set(msg \"\${msg} '\${arg}'\")
  endforeach(arg)
  set(msg \"\${msg}\\nSee also\\n  ${logbase}-*.log\\n\")
  message(FATAL_ERROR \"\${msg}\")
else()
  set(msg \"${name} ${step} command succeeded.  See also ${logbase}-*.log\\n\")
  message(STATUS \"\${msg}\")
endif()
")
  set(command ${CMAKE_COMMAND} ${make} ${config} -P ${script})
  set(${cmd_var} "${command}" PARENT_SCOPE)
endfunction(_ep_write_log_script)

# This module used to use "/${CMAKE_CFG_INTDIR}" directly and produced
# makefiles with "/./" in paths for custom command dependencies. Which
# resulted in problems with parallel make -j invocations.
#
# This function was added so that the suffix (search below for ${cfgdir}) is
# only set to "/${CMAKE_CFG_INTDIR}" when ${CMAKE_CFG_INTDIR} is not going to
# be "." (multi-configuration build systems like Visual Studio and Xcode...)
#
function(_ep_get_configuration_subdir_suffix suffix_var)
  set(suffix "")
  if(CMAKE_CONFIGURATION_TYPES)
    set(suffix "/${CMAKE_CFG_INTDIR}")
  endif()
  set(${suffix_var} "${suffix}" PARENT_SCOPE)
endfunction(_ep_get_configuration_subdir_suffix)


function(ExternalProjectWithFilename_Add_StepTargets name)
  set(steps ${ARGN})

  _ep_get_configuration_subdir_suffix(cfgdir)
  ExternalProjectWithFilename_Get_Property(${name} stamp_dir)

  foreach(step ${steps})
    add_custom_target(${name}-${step}
      DEPENDS ${stamp_dir}${cfgdir}/${name}-${step})
  endforeach()
endfunction(ExternalProjectWithFilename_Add_StepTargets)


function(ExternalProjectWithFilename_Add_Step name step)
  set(cmf_dir ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles)
  ExternalProjectWithFilename_Get_Property(${name} stamp_dir)

  _ep_get_configuration_subdir_suffix(cfgdir)

  add_custom_command(APPEND
    OUTPUT ${cmf_dir}${cfgdir}/${name}-complete
    DEPENDS ${stamp_dir}${cfgdir}/${name}-${step}
    )
  _ep_parse_arguments(ExternalProjectWithFilename_Add_Step
                       ${name} _EP_${step}_ "${ARGN}")

  # Steps depending on this step.
  get_property(dependers TARGET ${name} PROPERTY _EP_${step}_DEPENDERS)
  foreach(depender IN LISTS dependers)
    add_custom_command(APPEND
      OUTPUT ${stamp_dir}${cfgdir}/${name}-${depender}
      DEPENDS ${stamp_dir}${cfgdir}/${name}-${step}
      )
  endforeach()

  # Dependencies on files.
  get_property(depends TARGET ${name} PROPERTY _EP_${step}_DEPENDS)

  # Dependencies on steps.
  get_property(dependees TARGET ${name} PROPERTY _EP_${step}_DEPENDEES)
  foreach(dependee IN LISTS dependees)
    list(APPEND depends ${stamp_dir}${cfgdir}/${name}-${dependee})
  endforeach()

  # The command to run.
  get_property(command TARGET ${name} PROPERTY _EP_${step}_COMMAND)
  if(command)
    set(comment "Performing ${step} step for '${name}'")
  else()
    set(comment "No ${step} step for '${name}'")
  endif()
  get_property(work_dir TARGET ${name} PROPERTY _EP_${step}_WORKING_DIRECTORY)

  # Replace list separators.
  get_property(sep TARGET ${name} PROPERTY _EP_LIST_SEPARATOR)
  if(sep AND command)
    string(REPLACE "${sep}" "\\;" command "${command}")
  endif()

  # Replace location tags.
  _ep_replace_location_tags(${name} comment command work_dir)

  # Custom comment?
  get_property(comment_set TARGET ${name} PROPERTY _EP_${step}_COMMENT SET)
  if(comment_set)
    get_property(comment TARGET ${name} PROPERTY _EP_${step}_COMMENT)
  endif()

  # Run every time?
  get_property(always TARGET ${name} PROPERTY _EP_${step}_ALWAYS)
  if(always)
    set_property(SOURCE ${stamp_dir}${cfgdir}/${name}-${step} PROPERTY SYMBOLIC 1)
    set(touch)
  else()
    set(touch ${CMAKE_COMMAND} -E touch ${stamp_dir}${cfgdir}/${name}-${step})
  endif()

  # Wrap with log script?
  get_property(log TARGET ${name} PROPERTY _EP_${step}_LOG)
  if(command AND log)
    _ep_write_log_script(${name} ${step} command)
  endif()

  add_custom_command(
    OUTPUT ${stamp_dir}${cfgdir}/${name}-${step}
    COMMENT ${comment}
    COMMAND ${command}
    COMMAND ${touch}
    DEPENDS ${depends}
    WORKING_DIRECTORY ${work_dir}
    VERBATIM
    )

  # Add custom "step target"?
  get_property(step_targets TARGET ${name} PROPERTY _EP_STEP_TARGETS)
  if(NOT step_targets)
    get_property(step_targets DIRECTORY PROPERTY EP_STEP_TARGETS)
  endif()
  foreach(st ${step_targets})
    if("${st}" STREQUAL "${step}")
      ExternalProjectWithFilename_Add_StepTargets(${name} ${step})
      break()
    endif()
  endforeach()
endfunction(ExternalProjectWithFilename_Add_Step)


function(_ep_add_mkdir_command name)
  ExternalProjectWithFilename_Get_Property(${name}
    source_dir binary_dir install_dir stamp_dir download_dir tmp_dir)

  _ep_get_configuration_subdir_suffix(cfgdir)

  ExternalProjectWithFilename_Add_Step(${name} mkdir
    COMMENT "Creating directories for '${name}'"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${source_dir}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${binary_dir}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${install_dir}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${tmp_dir}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${stamp_dir}${cfgdir}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${download_dir}
    )
endfunction(_ep_add_mkdir_command)


function(_ep_get_git_version git_EXECUTABLE git_version_var)
  if(git_EXECUTABLE)
    execute_process(
      COMMAND "${git_EXECUTABLE}" --version
      OUTPUT_VARIABLE ov
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    string(REGEX REPLACE "^git version (.+)$" "\\1" version "${ov}")
    set(${git_version_var} "${version}" PARENT_SCOPE)
  endif()
endfunction()


function(_ep_is_dir_empty dir empty_var)
  file(GLOB gr "${dir}/*")
  if("${gr}" STREQUAL "")
    set(${empty_var} 1 PARENT_SCOPE)
  else()
    set(${empty_var} 0 PARENT_SCOPE)
  endif()
endfunction()


function(_ep_add_download_command name)
  ExternalProjectWithFilename_Get_Property(${name} source_dir stamp_dir download_dir tmp_dir)

  get_property(cmd_set TARGET ${name} PROPERTY _EP_DOWNLOAD_COMMAND SET)
  get_property(cmd TARGET ${name} PROPERTY _EP_DOWNLOAD_COMMAND)
  get_property(cvs_repository TARGET ${name} PROPERTY _EP_CVS_REPOSITORY)
  get_property(svn_repository TARGET ${name} PROPERTY _EP_SVN_REPOSITORY)
  get_property(git_repository TARGET ${name} PROPERTY _EP_GIT_REPOSITORY)
  get_property(url TARGET ${name} PROPERTY _EP_URL)
  get_property(fname TARGET ${name} PROPERTY _EP_FILENAME)

  # TODO: Perhaps file:// should be copied to download dir before extraction.
  string(REGEX REPLACE "^file://" "" url "${url}")

  set(depends)
  set(comment)
  set(work_dir)

  if(cmd_set)
    set(work_dir ${download_dir})
  elseif(cvs_repository)
    find_package(CVS)
    if(NOT CVS_EXECUTABLE)
      message(FATAL_ERROR "error: could not find cvs for checkout of ${name}")
    endif()

    get_target_property(cvs_module ${name} _EP_CVS_MODULE)
    if(NOT cvs_module)
      message(FATAL_ERROR "error: no CVS_MODULE")
    endif()

    get_property(cvs_tag TARGET ${name} PROPERTY _EP_CVS_TAG)

    set(repository ${cvs_repository})
    set(module ${cvs_module})
    set(tag ${cvs_tag})
    configure_file(
      "${CMAKE_ROOT}/Modules/RepositoryInfo.txt.in"
      "${stamp_dir}/${name}-cvsinfo.txt"
      @ONLY
      )

    get_filename_component(src_name "${source_dir}" NAME)
    get_filename_component(work_dir "${source_dir}" PATH)
    set(comment "Performing download step (CVS checkout) for '${name}'")
    set(cmd ${CVS_EXECUTABLE} -d ${cvs_repository} -q co ${cvs_tag} -d ${src_name} ${cvs_module})
    list(APPEND depends ${stamp_dir}/${name}-cvsinfo.txt)
  elseif(svn_repository)
    find_package(Subversion)
    if(NOT Subversion_SVN_EXECUTABLE)
      message(FATAL_ERROR "error: could not find svn for checkout of ${name}")
    endif()

    get_property(svn_revision TARGET ${name} PROPERTY _EP_SVN_REVISION)
    get_property(svn_username TARGET ${name} PROPERTY _EP_SVN_USERNAME)
    get_property(svn_password TARGET ${name} PROPERTY _EP_SVN_PASSWORD)
    get_property(svn_trust_cert TARGET ${name} PROPERTY _EP_SVN_TRUST_CERT)

    set(repository "${svn_repository} user=${svn_username} password=${svn_password}")
    set(module)
    set(tag ${svn_revision})
    configure_file(
      "${CMAKE_ROOT}/Modules/RepositoryInfo.txt.in"
      "${stamp_dir}/${name}-svninfo.txt"
      @ONLY
      )

    get_filename_component(src_name "${source_dir}" NAME)
    get_filename_component(work_dir "${source_dir}" PATH)
    set(comment "Performing download step (SVN checkout) for '${name}'")
    set(svn_user_pw_args "")
    if(svn_username)
      set(svn_user_pw_args ${svn_user_pw_args} "--username=${svn_username}")
    endif()
    if(svn_password)
      set(svn_user_pw_args ${svn_user_pw_args} "--password=${svn_password}")
    endif()
    if(svn_trust_cert)
      set(svn_trust_cert_args --trust-server-cert)
    endif()
    set(cmd ${Subversion_SVN_EXECUTABLE} co ${svn_repository} ${svn_revision}
      --non-interactive ${svn_trust_cert_args} ${svn_user_pw_args} ${src_name})
    list(APPEND depends ${stamp_dir}/${name}-svninfo.txt)
  elseif(git_repository)
    find_package(Git)
    if(NOT GIT_EXECUTABLE)
      message(FATAL_ERROR "error: could not find git for clone of ${name}")
    endif()

    # The git submodule update '--recursive' flag requires git >= v1.6.5
    #
    _ep_get_git_version("${GIT_EXECUTABLE}" git_version)
    if(git_version VERSION_LESS 1.6.5)
      message(FATAL_ERROR "error: git version 1.6.5 or later required for 'git submodule update --recursive': git_version='${git_version}'")
    endif()

    get_property(git_tag TARGET ${name} PROPERTY _EP_GIT_TAG)
    if(NOT git_tag)
      set(git_tag "master")
    endif()

    set(repository ${git_repository})
    set(module)
    set(tag ${git_tag})
    configure_file(
      "${CMAKE_ROOT}/Modules/RepositoryInfo.txt.in"
      "${stamp_dir}/${name}-gitinfo.txt"
      @ONLY
      )

    get_filename_component(src_name "${source_dir}" NAME)
    get_filename_component(work_dir "${source_dir}" PATH)

    # Since git clone doesn't succeed if the non-empty source_dir exists,
    # create a cmake script to invoke as download command.
    # The script will delete the source directory and then call git clone.
    #
    _ep_write_gitclone_script(${tmp_dir}/${name}-gitclone.cmake ${source_dir}
      ${GIT_EXECUTABLE} ${git_repository} ${git_tag} ${src_name} ${work_dir}
      )
    set(comment "Performing download step (git clone) for '${name}'")
    set(cmd ${CMAKE_COMMAND} -P ${tmp_dir}/${name}-gitclone.cmake)
    list(APPEND depends ${stamp_dir}/${name}-gitinfo.txt)
  elseif(url)
    get_filename_component(work_dir "${source_dir}" PATH)
    get_property(md5 TARGET ${name} PROPERTY _EP_URL_MD5)
    set(repository "external project URL")
    set(module "${url}")
    set(tag "${md5}")
    configure_file(
      "${CMAKE_ROOT}/Modules/RepositoryInfo.txt.in"
      "${stamp_dir}/${name}-urlinfo.txt"
      @ONLY
      )
    list(APPEND depends ${stamp_dir}/${name}-urlinfo.txt)
    if(IS_DIRECTORY "${url}")
      get_filename_component(abs_dir "${url}" ABSOLUTE)
      set(comment "Performing download step (DIR copy) for '${name}'")
      set(cmd   ${CMAKE_COMMAND} -E remove_directory ${source_dir}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${abs_dir} ${source_dir})
    else()
      if("${url}" MATCHES "^[a-z]+://")
        # TODO: Should download and extraction be different steps?

        # MODIFICATION HERE: allows setting filename for urls
        # where filename is not embedded, such as github
        if ("${fname}" STREQUAL "")
            string(REGEX MATCH "[^/\\?]*$" fname "${url}")
        endif()
        # this is set by filename now
        if(NOT "${fname}" MATCHES "(\\.|=)(bz2|tar|tgz|tar\\.gz|zip)$")
          string(REGEX MATCH "([^/\\?]+(\\.|=)(bz2|tar|tgz|tar\\.gz|zip))/.*$" match_result "${url}")
          set(fname "${CMAKE_MATCH_1}")
        endif()
        if(NOT "${fname}" MATCHES "(\\.|=)(bz2|tar|tgz|tar\\.gz|zip)$")
          message(FATAL_ERROR "Could not extract tarball filename from url:\n  ${url}")
        endif()
        string(REPLACE ";" "-" fname "${fname}")
        set(file ${download_dir}/${fname})
        get_property(timeout TARGET ${name} PROPERTY _EP_TIMEOUT)
        _ep_write_downloadfile_script("${stamp_dir}/download-${name}.cmake" "${url}" "${file}" "${timeout}" "${md5}")
        set(cmd ${CMAKE_COMMAND} -P ${stamp_dir}/download-${name}.cmake
          COMMAND)
        set(comment "Performing download step (download, verify and extract) for '${name}'")
      else()
        set(file "${url}")
        set(comment "Performing download step (verify and extract) for '${name}'")
      endif()
      _ep_write_verifyfile_script("${stamp_dir}/verify-${name}.cmake" "${file}" "${md5}")
      list(APPEND cmd ${CMAKE_COMMAND} -P ${stamp_dir}/verify-${name}.cmake)
      _ep_write_extractfile_script("${stamp_dir}/extract-${name}.cmake" "${name}" "${file}" "${source_dir}")
      list(APPEND cmd ${CMAKE_COMMAND} -P ${stamp_dir}/extract-${name}.cmake)
    endif()
  else()
    _ep_is_dir_empty("${source_dir}" empty)
    if(${empty})
      message(SEND_ERROR "error: no download info for '${name}' -- please specify existing/non-empty SOURCE_DIR or one of URL, CVS_REPOSITORY and CVS_MODULE, SVN_REPOSITORY, GIT_REPOSITORY or DOWNLOAD_COMMAND")
    endif()
  endif()

  get_property(log TARGET ${name} PROPERTY _EP_LOG_DOWNLOAD)
  if(log)
    set(log LOG 1)
  else()
    set(log "")
  endif()

  ExternalProjectWithFilename_Add_Step(${name} download
    COMMENT ${comment}
    COMMAND ${cmd}
    WORKING_DIRECTORY ${work_dir}
    DEPENDS ${depends}
    DEPENDEES mkdir
    ${log}
    )
endfunction(_ep_add_download_command)


function(_ep_add_update_command name)
  ExternalProjectWithFilename_Get_Property(${name} source_dir)

  get_property(cmd_set TARGET ${name} PROPERTY _EP_UPDATE_COMMAND SET)
  get_property(cmd TARGET ${name} PROPERTY _EP_UPDATE_COMMAND)
  get_property(cvs_repository TARGET ${name} PROPERTY _EP_CVS_REPOSITORY)
  get_property(svn_repository TARGET ${name} PROPERTY _EP_SVN_REPOSITORY)
  get_property(git_repository TARGET ${name} PROPERTY _EP_GIT_REPOSITORY)

  set(work_dir)
  set(comment)
  set(always)

  if(cmd_set)
    set(work_dir ${source_dir})
  elseif(cvs_repository)
    if(NOT CVS_EXECUTABLE)
      message(FATAL_ERROR "error: could not find cvs for update of ${name}")
    endif()
    set(work_dir ${source_dir})
    set(comment "Performing update step (CVS update) for '${name}'")
    get_property(cvs_tag TARGET ${name} PROPERTY _EP_CVS_TAG)
    set(cmd ${CVS_EXECUTABLE} -d ${cvs_repository} -q up -dP ${cvs_tag})
    set(always 1)
  elseif(svn_repository)
    if(NOT Subversion_SVN_EXECUTABLE)
      message(FATAL_ERROR "error: could not find svn for update of ${name}")
    endif()
    set(work_dir ${source_dir})
    set(comment "Performing update step (SVN update) for '${name}'")
    get_property(svn_revision TARGET ${name} PROPERTY _EP_SVN_REVISION)
    get_property(svn_username TARGET ${name} PROPERTY _EP_SVN_USERNAME)
    get_property(svn_password TARGET ${name} PROPERTY _EP_SVN_PASSWORD)
    get_property(svn_trust_cert TARGET ${name} PROPERTY _EP_SVN_TRUST_CERT)
    set(svn_user_pw_args "")
    if(svn_username)
      set(svn_user_pw_args ${svn_user_pw_args} "--username=${svn_username}")
    endif()
    if(svn_password)
      set(svn_user_pw_args ${svn_user_pw_args} "--password=${svn_password}")
    endif()
    if(svn_trust_cert)
      set(svn_trust_cert_args --trust-server-cert)
    endif()
    set(cmd ${Subversion_SVN_EXECUTABLE} up ${svn_revision}
      --non-interactive ${svn_trust_cert_args} ${svn_user_pw_args})
    set(always 1)
  elseif(git_repository)
    if(NOT GIT_EXECUTABLE)
      message(FATAL_ERROR "error: could not find git for fetch of ${name}")
    endif()
    set(work_dir ${source_dir})
    set(comment "Performing update step (git fetch) for '${name}'")
    get_property(git_tag TARGET ${name} PROPERTY _EP_GIT_TAG)
    if(NOT git_tag)
      set(git_tag "master")
    endif()
    set(cmd ${GIT_EXECUTABLE} fetch
      COMMAND ${GIT_EXECUTABLE} checkout ${git_tag}
      COMMAND ${GIT_EXECUTABLE} submodule update --recursive
      )
    set(always 1)
  endif()

  get_property(log TARGET ${name} PROPERTY _EP_LOG_UPDATE)
  if(log)
    set(log LOG 1)
  else()
    set(log "")
  endif()

  ExternalProjectWithFilename_Add_Step(${name} update
    COMMENT ${comment}
    COMMAND ${cmd}
    ALWAYS ${always}
    WORKING_DIRECTORY ${work_dir}
    DEPENDEES download
    ${log}
    )
endfunction(_ep_add_update_command)


function(_ep_add_patch_command name)
  ExternalProjectWithFilename_Get_Property(${name} source_dir)

  get_property(cmd_set TARGET ${name} PROPERTY _EP_PATCH_COMMAND SET)
  get_property(cmd TARGET ${name} PROPERTY _EP_PATCH_COMMAND)

  set(work_dir)

  if(cmd_set)
    set(work_dir ${source_dir})
  endif()

  ExternalProjectWithFilename_Add_Step(${name} patch
    COMMAND ${cmd}
    WORKING_DIRECTORY ${work_dir}
    DEPENDEES download
    )
endfunction(_ep_add_patch_command)


# TODO: Make sure external projects use the proper compiler
function(_ep_add_configure_command name)
  ExternalProjectWithFilename_Get_Property(${name} source_dir binary_dir tmp_dir)

  _ep_get_configuration_subdir_suffix(cfgdir)

  # Depend on other external projects (file-level).
  set(file_deps)
  get_property(deps TARGET ${name} PROPERTY _EP_DEPENDS)
  foreach(dep IN LISTS deps)
    get_property(dep_stamp_dir TARGET ${dep} PROPERTY _EP_STAMP_DIR)
    list(APPEND file_deps ${dep_stamp_dir}${cfgdir}/${dep}-done)
  endforeach()

  get_property(cmd_set TARGET ${name} PROPERTY _EP_CONFIGURE_COMMAND SET)
  if(cmd_set)
    get_property(cmd TARGET ${name} PROPERTY _EP_CONFIGURE_COMMAND)
  else()
    get_target_property(cmake_command ${name} _EP_CMAKE_COMMAND)
    if(cmake_command)
      set(cmd "${cmake_command}")
    else()
      set(cmd "${CMAKE_COMMAND}")
    endif()

    get_property(cmake_args TARGET ${name} PROPERTY _EP_CMAKE_ARGS)
    list(APPEND cmd ${cmake_args})

    # If there are any CMAKE_CACHE_ARGS, write an initial cache and use it
    get_property(cmake_cache_args TARGET ${name} PROPERTY _EP_CMAKE_CACHE_ARGS)
    if(cmake_cache_args)
      set(_ep_cache_args_script "${tmp_dir}/${name}-cache.cmake")
      _ep_write_initial_cache(${name} "${_ep_cache_args_script}" "${cmake_cache_args}")
      list(APPEND cmd "-C${_ep_cache_args_script}")
    endif()

    get_target_property(cmake_generator ${name} _EP_CMAKE_GENERATOR)
    if(cmake_generator)
      list(APPEND cmd "-G${cmake_generator}" "${source_dir}")
    else()
      if(CMAKE_EXTRA_GENERATOR)
        list(APPEND cmd "-G${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}"
          "${source_dir}")
      else()
        list(APPEND cmd "-G${CMAKE_GENERATOR}" "${source_dir}")
      endif()
    endif()
  endif()

  # If anything about the configure command changes, (command itself, cmake
  # used, cmake args or cmake generator) then re-run the configure step.
  # Fixes issue http://public.kitware.com/Bug/view.php?id=10258
  #
  if(NOT EXISTS ${tmp_dir}/${name}-cfgcmd.txt.in)
    file(WRITE ${tmp_dir}/${name}-cfgcmd.txt.in "cmd='\@cmd\@'\n")
  endif()
  configure_file(${tmp_dir}/${name}-cfgcmd.txt.in ${tmp_dir}/${name}-cfgcmd.txt)
  list(APPEND file_deps ${tmp_dir}/${name}-cfgcmd.txt)
  list(APPEND file_deps ${_ep_cache_args_script})

  get_property(log TARGET ${name} PROPERTY _EP_LOG_CONFIGURE)
  if(log)
    set(log LOG 1)
  else()
    set(log "")
  endif()

  ExternalProjectWithFilename_Add_Step(${name} configure
    COMMAND ${cmd}
    WORKING_DIRECTORY ${binary_dir}
    DEPENDEES update patch
    DEPENDS ${file_deps}
    ${log}
    )
endfunction(_ep_add_configure_command)


function(_ep_add_build_command name)
  ExternalProjectWithFilename_Get_Property(${name} binary_dir)

  get_property(cmd_set TARGET ${name} PROPERTY _EP_BUILD_COMMAND SET)
  if(cmd_set)
    get_property(cmd TARGET ${name} PROPERTY _EP_BUILD_COMMAND)
  else()
    _ep_get_build_command(${name} BUILD cmd)
  endif()

  get_property(log TARGET ${name} PROPERTY _EP_LOG_BUILD)
  if(log)
    set(log LOG 1)
  else()
    set(log "")
  endif()

  ExternalProjectWithFilename_Add_Step(${name} build
    COMMAND ${cmd}
    WORKING_DIRECTORY ${binary_dir}
    DEPENDEES configure
    ${log}
    )
endfunction(_ep_add_build_command)


function(_ep_add_install_command name)
  ExternalProjectWithFilename_Get_Property(${name} binary_dir)

  get_property(cmd_set TARGET ${name} PROPERTY _EP_INSTALL_COMMAND SET)
  if(cmd_set)
    get_property(cmd TARGET ${name} PROPERTY _EP_INSTALL_COMMAND)
  else()
    _ep_get_build_command(${name} INSTALL cmd)
  endif()

  get_property(log TARGET ${name} PROPERTY _EP_LOG_INSTALL)
  if(log)
    set(log LOG 1)
  else()
    set(log "")
  endif()

  ExternalProjectWithFilename_Add_Step(${name} install
    COMMAND ${cmd}
    WORKING_DIRECTORY ${binary_dir}
    DEPENDEES build
    ${log}
    )
endfunction(_ep_add_install_command)


function(_ep_add_test_command name)
  ExternalProjectWithFilename_Get_Property(${name} binary_dir)

  get_property(before TARGET ${name} PROPERTY _EP_TEST_BEFORE_INSTALL)
  get_property(after TARGET ${name} PROPERTY _EP_TEST_AFTER_INSTALL)
  get_property(cmd_set TARGET ${name} PROPERTY _EP_TEST_COMMAND SET)

  # Only actually add the test step if one of the test related properties is
  # explicitly set. (i.e. the test step is omitted unless requested...)
  #
  if(cmd_set OR before OR after)
    if(cmd_set)
      get_property(cmd TARGET ${name} PROPERTY _EP_TEST_COMMAND)
    else()
      _ep_get_build_command(${name} TEST cmd)
    endif()

    if(before)
      set(dep_args DEPENDEES build DEPENDERS install)
    else()
      set(dep_args DEPENDEES install)
    endif()

    get_property(log TARGET ${name} PROPERTY _EP_LOG_TEST)
    if(log)
      set(log LOG 1)
    else()
      set(log "")
    endif()

    ExternalProjectWithFilename_Add_Step(${name} test
      COMMAND ${cmd}
      WORKING_DIRECTORY ${binary_dir}
      ${dep_args}
      ${log}
      )
  endif()
endfunction(_ep_add_test_command)


function(ExternalProjectWithFilename_Add name)
  _ep_get_configuration_subdir_suffix(cfgdir)

  # Add a custom target for the external project.
  set(cmf_dir ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles)
  add_custom_target(${name} ALL DEPENDS ${cmf_dir}${cfgdir}/${name}-complete)
  set_property(TARGET ${name} PROPERTY _EP_IS_EXTERNAL_PROJECT 1)
  _ep_parse_arguments(ExternalProjectWithFilename_Add ${name} _EP_ "${ARGN}")
  _ep_set_directories(${name})
  ExternalProjectWithFilename_Get_Property(${name} stamp_dir)

  # The 'complete' step depends on all other steps and creates a
  # 'done' mark.  A dependent external project's 'configure' step
  # depends on the 'done' mark so that it rebuilds when this project
  # rebuilds.  It is important that 'done' is not the output of any
  # custom command so that CMake does not propagate build rules to
  # other external project targets.
  add_custom_command(
    OUTPUT ${cmf_dir}${cfgdir}/${name}-complete
    COMMENT "Completed '${name}'"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${cmf_dir}${cfgdir}
    COMMAND ${CMAKE_COMMAND} -E touch ${cmf_dir}${cfgdir}/${name}-complete
    COMMAND ${CMAKE_COMMAND} -E touch ${stamp_dir}${cfgdir}/${name}-done
    DEPENDS ${stamp_dir}${cfgdir}/${name}-install
    VERBATIM
    )


  # Depend on other external projects (target-level).
  get_property(deps TARGET ${name} PROPERTY _EP_DEPENDS)
  foreach(arg IN LISTS deps)
    add_dependencies(${name} ${arg})
  endforeach()

  # Set up custom build steps based on the target properties.
  # Each step depends on the previous one.
  #
  # The target depends on the output of the final step.
  # (Already set up above in the DEPENDS of the add_custom_target command.)
  #
  _ep_add_mkdir_command(${name})
  _ep_add_download_command(${name})
  _ep_add_update_command(${name})
  _ep_add_patch_command(${name})
  _ep_add_configure_command(${name})
  _ep_add_build_command(${name})
  _ep_add_install_command(${name})

  # Test is special in that it might depend on build, or it might depend
  # on install.
  #
  _ep_add_test_command(${name})
endfunction(ExternalProjectWithFilename_Add)
      run: |
        echo "build-output-dir=${{ # - Check whether the C compiler supports a given flag in the
# context of a stack checking compiler option.
# CHECK_C_COMPILER_FLAG_SSP(FLAG VARIABLE)
#
#  FLAG - the compiler flag
#  VARIABLE - variable to store the result
# 
#  This actually calls the check_c_source_compiles macro.
#  See help for CheckCSourceCompiles for a listing of variables
#  that can modify the build.

# Copyright (c) 2006, 
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


INCLUDE(CheckCSourceCompiles)

MACRO (CHECK_C_COMPILER_FLAG_SSP _FLAG _RESULT)
   SET(SAFE_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
   SET(CMAKE_REQUIRED_DEFINITIONS "${_FLAG}")
   CHECK_C_SOURCE_COMPILES("int main(int argc, char **argv) { char buffer[256]; return buffer[argc]=0;}" ${_RESULT})
   SET (CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")
ENDMACRO (CHECK_C_COMPILER_FLAG_SSP)
 }}/build" >> "$GITHUB_OUTPUT"

    - name: # - MACRO_ENSURE_OUT_OF_SOURCE_BUILD(<errorMessage>)
# MACRO_ENSURE_OUT_OF_SOURCE_BUILD(<errorMessage>)

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro (MACRO_ENSURE_OUT_OF_SOURCE_BUILD _errorMessage)

    string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" _insource)
    if (_insource)
        file(REMOVE [CMakeCache.txt CMakeFiles]) 
        message(FATAL_ERROR "${_errorMessage}")
    endif (_insource)

endmacro (MACRO_ENSURE_OUT_OF_SOURCE_BUILD)

# vim:ts=4:sw=4:expandtab
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: #!/usr/bin/python
# 
# Based on bash script by (github.com/)
#
# script used to update cmake modules from git repo, can't make this
# a submodule otherwise it won't know how to interpret the CMakeLists.txt
# # # # # # subprocess# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

import os # for os.path
import subprocess # for check_call()

clone_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
print(clone_path)
os.chdir(clone_path)
subprocess.check_call(["git", "clone", "git://github.com/arktools/arkcmake.git","arkcmake_tmp"])
subprocess.check_call(["rm", "-rf", "arkcmake_tmp/.git"])
if os.path.isdir("arkcmake"):
        subprocess.check_call(["rm", "-rf", "arkcmake"])
subprocess.check_call(["mv", "arkcmake_tmp", "arkcmake"])
        cmake -B ${{ # Doxyfile 1.6.1 

# This file describes the settings to be used by the documentation system 
# doxygen (www.doxygen.org) for a project 
# 
# All text after a hash (#) is considered a comment and will be ignored 
# The format is: 
#       TAG = value [value, ...] 
# For lists items can also be appended using: 
#       TAG += value [value, ...] 
# Values that contain spaces should be placed between quotes (" ") 

#--------------------------------------------------------------------------- 
# Project related configuration options 
#--------------------------------------------------------------------------- 

# This tag specifies the encoding used for all characters in the config file 
# that follow. The default is UTF-8 which is also the encoding used for all 
# text before the first occurrence of this tag. Doxygen uses libiconv (or the 
# iconv built into libc) for the transcoding. See 
# http://www.gnu.org/software/libiconv for the list of possible encodings. 

DOXYFILE_ENCODING      = UTF-8 

# The PROJECT_NAME tag is a single word (or a sequence of words surrounded 
# by quotes) that should identify the project. 

PROJECT_NAME           = "PIXHAWK IMU / Autopilot" 

# The PROJECT_NUMBER tag can be used to enter a project or revision number. 
# This could be handy for archiving the generated documentation or 
# if some version control system is used. 

PROJECT_NUMBER         = 

# The OUTPUT_DIRECTORY tag is used to specify the (relative or absolute) 
# base path where the generated documentation will be put. 
# If a relative path is entered, it will be relative to the location 
# where doxygen was started. If left blank the current directory will be used. 

OUTPUT_DIRECTORY       = 

# If the CREATE_SUBDIRS tag is set to YES, then doxygen will create 
# 4096 sub-directories (in 2 levels) under the output directory of each output 
# format and will distribute the generated files over these directories. 
# Enabling this option can be useful when feeding doxygen a huge amount of 
# source files, where putting all generated files in the same directory would 
# otherwise cause performance problems for the file system. 

CREATE_SUBDIRS         = NO 

# The OUTPUT_LANGUAGE tag is used to specify the language in which all 
# documentation generated by doxygen is written. Doxygen will use this 
# information to generate all constant output in the proper language. 
# The default language is English, other supported languages are: 
# Afrikaans, Arabic, Brazilian, Catalan, Chinese, Chinese-Traditional, 
# Croatian, Czech, Danish, Dutch, Esperanto, Farsi, Finnish, French, German, 
# Greek, Hungarian, Italian, Japanese, Japanese-en (Japanese with English 
# messages), Korean, Korean-en, Lithuanian, Norwegian, Macedonian, Persian, 
# Polish, Portuguese, Romanian, Russian, Serbian, Serbian-Cyrilic, Slovak, 
# Slovene, Spanish, Swedish, Ukrainian, and Vietnamese. 

OUTPUT_LANGUAGE        = English 

# If the BRIEF_MEMBER_DESC tag is set to YES (the default) Doxygen will 
# include brief member descriptions after the members that are listed in 
# the file and class documentation (similar to JavaDoc). 
# Set to NO to disable this. 

BRIEF_MEMBER_DESC      = YES 

# If the REPEAT_BRIEF tag is set to YES (the default) Doxygen will prepend 
# the brief description of a member or function before the detailed description. 
# Note: if both HIDE_UNDOC_MEMBERS and BRIEF_MEMBER_DESC are set to NO, the 
# brief descriptions will be completely suppressed. 

REPEAT_BRIEF           = YES 

# This tag implements a quasi-intelligent brief description abbreviator 
# that is used to form the text in various listings. Each string 
# in this list, if found as the leading text of the brief description, will be 
# stripped from the text and the result after processing the whole list, is 
# used as the annotated text. Otherwise, the brief description is used as-is. 
# If left blank, the following values are used ("$name" is automatically 
# replaced with the name of the entity): "The $name class" "The $name widget" 
# "The $name file" "is" "provides" "specifies" "contains" 
# "represents" "a" "an" "the" 

ABBREVIATE_BRIEF       = 

# If the ALWAYS_DETAILED_SEC and REPEAT_BRIEF tags are both set to YES then 
# Doxygen will generate a detailed section even if there is only a brief 
# description. 

ALWAYS_DETAILED_SEC    = YES 

# If the INLINE_INHERITED_MEMB tag is set to YES, doxygen will show all 
# inherited members of a class in the documentation of that class as if those 
# members were ordinary class members. Constructors, destructors and assignment 
# operators of the base classes will not be shown. 

INLINE_INHERITED_MEMB  = YES 

# If the FULL_PATH_NAMES tag is set to YES then Doxygen will prepend the full 
# path before files name in the file list and in the header files. If set 
# to NO the shortest path that makes the file name unique will be used. 

FULL_PATH_NAMES        = NO 

# If the FULL_PATH_NAMES tag is set to YES then the STRIP_FROM_PATH tag 
# can be used to strip a user-defined part of the path. Stripping is 
# only done if one of the specified strings matches the left-hand part of 
# the path. The tag can be used to show relative paths in the file list. 
# If left blank the directory from which doxygen is run is used as the 
# path to strip. 

STRIP_FROM_PATH        = 

# The STRIP_FROM_INC_PATH tag can be used to strip a user-defined part of 
# the path mentioned in the documentation of a class, which tells 
# the reader which header file to include in order to use a class. 
# If left blank only the name of the header file containing the class 
# definition is used. Otherwise one should specify the include paths that 
# are normally passed to the compiler using the -I flag. 

STRIP_FROM_INC_PATH    = 

# If the SHORT_NAMES tag is set to YES, doxygen will generate much shorter 
# (but less readable) file names. This can be useful is your file systems 
# doesn't support long names like on DOS, Mac, or CD-ROM. 

SHORT_NAMES            = NO 

# If the JAVADOC_AUTOBRIEF tag is set to YES then Doxygen 
# will interpret the first line (until the first dot) of a JavaDoc-style 
# comment as the brief description. If set to NO, the JavaDoc 
# comments will behave just like regular Qt-style comments 
# (thus requiring an explicit @brief command for a brief description.) 

JAVADOC_AUTOBRIEF      = NO 

# If the QT_AUTOBRIEF tag is set to YES then Doxygen will 
# interpret the first line (until the first dot) of a Qt-style 
# comment as the brief description. If set to NO, the comments 
# will behave just like regular Qt-style comments (thus requiring 
# an explicit \brief command for a brief description.) 

QT_AUTOBRIEF           = NO 

# The MULTILINE_CPP_IS_BRIEF tag can be set to YES to make Doxygen 
# treat a multi-line C++ special comment block (i.e. a block of //! or /// 
# comments) as a brief description. This used to be the default behaviour. 
# The new default is to treat a multi-line C++ comment block as a detailed 
# description. Set this tag to YES if you prefer the old behaviour instead. 

MULTILINE_CPP_IS_BRIEF = NO 

# If the INHERIT_DOCS tag is set to YES (the default) then an undocumented 
# member inherits the documentation from any documented member that it 
# re-implements. 

INHERIT_DOCS           = YES 

# If the SEPARATE_MEMBER_PAGES tag is set to YES, then doxygen will produce 
# a new page for each member. If set to NO, the documentation of a member will 
# be part of the file/class/namespace that contains it. 

SEPARATE_MEMBER_PAGES  = NO 

# The TAB_SIZE tag can be used to set the number of spaces in a tab. 
# Doxygen uses this value to replace tabs by spaces in code fragments. 

TAB_SIZE               = 8 

# This tag can be used to specify a number of aliases that acts 
# as commands in the documentation. An alias has the form "name=value". 
# For example adding "sideeffect=\par Side Effects:\n" will allow you to 
# put the command \sideeffect (or @sideeffect) in the documentation, which 
# will result in a user-defined paragraph with heading "Side Effects:". 
# You can put \n's in the value part of an alias to insert newlines. 

ALIASES                = 

# Set the OPTIMIZE_OUTPUT_FOR_C tag to YES if your project consists of C 
# sources only. Doxygen will then generate output that is more tailored for C. 
# For instance, some of the names that are used will be different. The list 
# of all members will be omitted, etc. 

OPTIMIZE_OUTPUT_FOR_C  = YES 

# Set the OPTIMIZE_OUTPUT_JAVA tag to YES if your project consists of Java 
# sources only. Doxygen will then generate output that is more tailored for 
# Java. For instance, namespaces will be presented as packages, qualified 
# scopes will look different, etc. 

OPTIMIZE_OUTPUT_JAVA   = NO 

# Set the OPTIMIZE_FOR_FORTRAN tag to YES if your project consists of Fortran 
# sources only. Doxygen will then generate output that is more tailored for 
# Fortran. 

OPTIMIZE_FOR_FORTRAN   = NO 

# Set the OPTIMIZE_OUTPUT_VHDL tag to YES if your project consists of VHDL 
# sources. Doxygen will then generate output that is tailored for 
# VHDL. 

OPTIMIZE_OUTPUT_VHDL   = NO 

# Doxygen selects the parser to use depending on the extension of the files it parses. 
# With this tag you can assign which parser to use for a given extension. 
# Doxygen has a built-in mapping, but you can override or extend it using this tag. 
# The format is ext=language, where ext is a file extension, and language is one of 
# the parsers supported by doxygen: IDL, Java, Javascript, C#, C, C++, D, PHP, 
# Objective-C, Python, Fortran, VHDL, C, C++. For instance to make doxygen treat 
# .inc files as Fortran files (default is PHP), and .f files as C (default is Fortran), 
# use: inc=Fortran f=C. Note that for custom extensions you also need to set FILE_PATTERNS otherwise the files are not read by doxygen. 

EXTENSION_MAPPING      = 

# If you use STL classes (i.e. std::string, std::vector, etc.) but do not want 
# to include (a tag file for) the STL sources as input, then you should 
# set this tag to YES in order to let doxygen match functions declarations and 
# definitions whose arguments contain STL classes (e.g. func(std::string); v.s. 
# func(std::string) {}). This also make the inheritance and collaboration 
# diagrams that involve STL classes more complete and accurate. 

BUILTIN_STL_SUPPORT    = NO 

# If you use Microsoft's C++/CLI language, you should set this option to YES to 
# enable parsing support. 

CPP_CLI_SUPPORT        = NO 

# Set the SIP_SUPPORT tag to YES if your project consists of sip sources only. 
# Doxygen will parse them like normal C++ but will assume all classes use public 
# instead of private inheritance when no explicit protection keyword is present. 

SIP_SUPPORT            = NO 

# For Microsoft's IDL there are propget and propput attributes to indicate getter 
# and setter methods for a property. Setting this option to YES (the default) 
# will make doxygen to replace the get and set methods by a property in the 
# documentation. This will only work if the methods are indeed getting or 
# setting a simple type. If this is not the case, or you want to show the 
# methods anyway, you should set this option to NO. 

IDL_PROPERTY_SUPPORT   = YES 

# If member grouping is used in the documentation and the DISTRIBUTE_GROUP_DOC 
# tag is set to YES, then doxygen will reuse the documentation of the first 
# member in the group (if any) for the other members of the group. By default 
# all members of a group must be documented explicitly. 

DISTRIBUTE_GROUP_DOC   = NO 

# Set the SUBGROUPING tag to YES (the default) to allow class member groups of 
# the same type (for instance a group of public functions) to be put as a 
# subgroup of that type (e.g. under the Public Functions section). Set it to 
# NO to prevent subgrouping. Alternatively, this can be done per class using 
# the \nosubgrouping command. 

SUBGROUPING            = YES 

# When TYPEDEF_HIDES_STRUCT is enabled, a typedef of a struct, union, or enum 
# is documented as struct, union, or enum with the name of the typedef. So 
# typedef struct TypeS {} TypeT, will appear in the documentation as a struct 
# with name TypeT. When disabled the typedef will appear as a member of a file, 
# namespace, or class. And the struct will be named TypeS. This can typically 
# be useful for C code in case the coding convention dictates that all compound 
# types are typedef'ed and only the typedef is referenced, never the tag name. 

TYPEDEF_HIDES_STRUCT   = NO 

# The SYMBOL_CACHE_SIZE determines the size of the internal cache use to 
# determine which symbols to keep in memory and which to flush to disk. 
# When the cache is full, less often used symbols will be written to disk. 
# For small to medium size projects (<1000 input files) the default value is 
# probably good enough. For larger projects a too small cache size can cause 
# doxygen to be busy swapping symbols to and from disk most of the time 
# causing a significant performance penality. 
# If the system has enough physical memory increasing the cache will improve the 
# performance by keeping more symbols in memory. Note that the value works on 
# a logarithmic scale so increasing the size by one will rougly double the 
# memory usage. The cache size is given by this formula: 
# 2^(16+SYMBOL_CACHE_SIZE). The valid range is 0..9, the default is 0, 
# corresponding to a cache size of 2^16 = 65536 symbols 

SYMBOL_CACHE_SIZE      = 0 

#--------------------------------------------------------------------------- 
# Build related configuration options 
#--------------------------------------------------------------------------- 

# If the EXTRACT_ALL tag is set to YES doxygen will assume all entities in 
# documentation are documented, even if no documentation was available. 
# Private class members and static file members will be hidden unless 
# the EXTRACT_PRIVATE and EXTRACT_STATIC tags are set to YES 

EXTRACT_ALL            = YES 

# If the EXTRACT_PRIVATE tag is set to YES all private members of a class 
# will be included in the documentation. 

EXTRACT_PRIVATE        = NO 

# If the EXTRACT_STATIC tag is set to YES all static members of a file 
# will be included in the documentation. 

EXTRACT_STATIC         = YES 

# If the EXTRACT_LOCAL_CLASSES tag is set to YES classes (and structs) 
# defined locally in source files will be included in the documentation. 
# If set to NO only classes defined in header files are included. 

EXTRACT_LOCAL_CLASSES  = YES 

# This flag is only useful for Objective-C code. When set to YES local 
# methods, which are defined in the implementation section but not in 
# the interface are included in the documentation. 
# If set to NO (the default) only methods in the interface are included. 

EXTRACT_LOCAL_METHODS  = NO 

# If this flag is set to YES, the members of anonymous namespaces will be 
# extracted and appear in the documentation as a namespace called 
# 'anonymous_namespace{file}', where file will be replaced with the base 
# name of the file that contains the anonymous namespace. By default 
# anonymous namespace are hidden. 

EXTRACT_ANON_NSPACES   = NO 

# If the HIDE_UNDOC_MEMBERS tag is set to YES, Doxygen will hide all 
# undocumented members of documented classes, files or namespaces. 
# If set to NO (the default) these members will be included in the 
# various overviews, but no documentation section is generated. 
# This option has no effect if EXTRACT_ALL is enabled. 

HIDE_UNDOC_MEMBERS     = NO 

# If the HIDE_UNDOC_CLASSES tag is set to YES, Doxygen will hide all 
# undocumented classes that are normally visible in the class hierarchy. 
# If set to NO (the default) these classes will be included in the various 
# overviews. This option has no effect if EXTRACT_ALL is enabled. 

HIDE_UNDOC_CLASSES     = NO 

# If the HIDE_FRIEND_COMPOUNDS tag is set to YES, Doxygen will hide all 
# friend (class|struct|union) declarations. 
# If set to NO (the default) these declarations will be included in the 
# documentation. 

HIDE_FRIEND_COMPOUNDS  = NO 

# If the HIDE_IN_BODY_DOCS tag is set to YES, Doxygen will hide any 
# documentation blocks found inside the body of a function. 
# If set to NO (the default) these blocks will be appended to the 
# function's detailed documentation block. 

HIDE_IN_BODY_DOCS      = NO 

# The INTERNAL_DOCS tag determines if documentation 
# that is typed after a \internal command is included. If the tag is set 
# to NO (the default) then the documentation will be excluded. 
# Set it to YES to include the internal documentation. 

INTERNAL_DOCS          = NO 

# If the CASE_SENSE_NAMES tag is set to NO then Doxygen will only generate 
# file names in lower-case letters. If set to YES upper-case letters are also 
# allowed. This is useful if you have classes or files whose names only differ 
# in case and if your file system supports case sensitive file names. Windows 
# and Mac users are advised to set this option to NO. 

CASE_SENSE_NAMES       = YES 

# If the HIDE_SCOPE_NAMES tag is set to NO (the default) then Doxygen 
# will show members with their full class and namespace scopes in the 
# documentation. If set to YES the scope will be hidden. 

HIDE_SCOPE_NAMES       = NO 

# If the SHOW_INCLUDE_FILES tag is set to YES (the default) then Doxygen 
# will put a list of the files that are included by a file in the documentation 
# of that file. 

SHOW_INCLUDE_FILES     = YES 

# If the INLINE_INFO tag is set to YES (the default) then a tag [inline] 
# is inserted in the documentation for inline members. 

INLINE_INFO            = YES 

# If the SORT_MEMBER_DOCS tag is set to YES (the default) then doxygen 
# will sort the (detailed) documentation of file and class members 
# alphabetically by member name. If set to NO the members will appear in 
# declaration order. 

SORT_MEMBER_DOCS       = YES 

# If the SORT_BRIEF_DOCS tag is set to YES then doxygen will sort the 
# brief documentation of file, namespace and class members alphabetically 
# by member name. If set to NO (the default) the members will appear in 
# declaration order. 

SORT_BRIEF_DOCS        = NO 

# If the SORT_MEMBERS_CTORS_1ST tag is set to YES then doxygen will sort the (brief and detailed) documentation of class members so that constructors and destructors are listed first. If set to NO (the default) the constructors will appear in the respective orders defined by SORT_MEMBER_DOCS and SORT_BRIEF_DOCS. This tag will be ignored for brief docs if SORT_BRIEF_DOCS is set to NO and ignored for detailed docs if SORT_MEMBER_DOCS is set to NO. 

SORT_MEMBERS_CTORS_1ST = NO 

# If the SORT_GROUP_NAMES tag is set to YES then doxygen will sort the 
# hierarchy of group names into alphabetical order. If set to NO (the default) 
# the group names will appear in their defined order. 

SORT_GROUP_NAMES       = NO 

# If the SORT_BY_SCOPE_NAME tag is set to YES, the class list will be 
# sorted by fully-qualified names, including namespaces. If set to 
# NO (the default), the class list will be sorted only by class name, 
# not including the namespace part. 
# Note: This option is not very useful if HIDE_SCOPE_NAMES is set to YES. 
# Note: This option applies only to the class list, not to the 
# alphabetical list. 

SORT_BY_SCOPE_NAME     = NO 

# The GENERATE_TODOLIST tag can be used to enable (YES) or 
# disable (NO) the todo list. This list is created by putting \todo 
# commands in the documentation. 

GENERATE_TODOLIST      = YES 

# The GENERATE_TESTLIST tag can be used to enable (YES) or 
# disable (NO) the test list. This list is created by putting \test 
# commands in the documentation. 

GENERATE_TESTLIST      = YES 

# The GENERATE_BUGLIST tag can be used to enable (YES) or 
# disable (NO) the bug list. This list is created by putting \bug 
# commands in the documentation. 

GENERATE_BUGLIST       = YES 

# The GENERATE_DEPRECATEDLIST tag can be used to enable (YES) or 
# disable (NO) the deprecated list. This list is created by putting 
# \deprecated commands in the documentation. 

GENERATE_DEPRECATEDLIST= YES 

# The ENABLED_SECTIONS tag can be used to enable conditional 
# documentation sections, marked by \if sectionname ... \endif. 

ENABLED_SECTIONS       = 

# The MAX_INITIALIZER_LINES tag determines the maximum number of lines 
# the initial value of a variable or define consists of for it to appear in 
# the documentation. If the initializer consists of more lines than specified 
# here it will be hidden. Use a value of 0 to hide initializers completely. 
# The appearance of the initializer of individual variables and defines in the 
# documentation can be controlled using \showinitializer or \hideinitializer 
# command in the documentation regardless of this setting. 

MAX_INITIALIZER_LINES  = 30 

# Set the SHOW_USED_FILES tag to NO to disable the list of files generated 
# at the bottom of the documentation of classes and structs. If set to YES the 
# list will mention the files that were used to generate the documentation. 

SHOW_USED_FILES        = YES 

# If the sources in your project are distributed over multiple directories 
# then setting the SHOW_DIRECTORIES tag to YES will show the directory hierarchy 
# in the documentation. The default is NO. 

SHOW_DIRECTORIES       = YES 

# Set the SHOW_FILES tag to NO to disable the generation of the Files page. 
# This will remove the Files entry from the Quick Index and from the 
# Folder Tree View (if specified). The default is YES. 

SHOW_FILES             = YES 

# Set the SHOW_NAMESPACES tag to NO to disable the generation of the 
# Namespaces page. 
# This will remove the Namespaces entry from the Quick Index 
# and from the Folder Tree View (if specified). The default is YES. 

SHOW_NAMESPACES        = YES 

# The FILE_VERSION_FILTER tag can be used to specify a program or script that 
# doxygen should invoke to get the current version for each file (typically from 
# the version control system). Doxygen will invoke the program by executing (via 
# popen()) the command <command> <input-file>, where <command> is the value of 
# the FILE_VERSION_FILTER tag, and <input-file> is the name of an input file 
# provided by doxygen. Whatever the program writes to standard output 
# is used as the file version. See the manual for examples. 

FILE_VERSION_FILTER    = 

# The LAYOUT_FILE tag can be used to specify a layout file which will be parsed by 
# doxygen. The layout file controls the global structure of the generated output files 
# in an output format independent way. The create the layout file that represents 
# doxygen's defaults, run doxygen with the -l option. You can optionally specify a 
# file name after the option, if omitted DoxygenLayout.xml will be used as the name 
# of the layout file. 

LAYOUT_FILE            = 

#--------------------------------------------------------------------------- 
# configuration options related to warning and progress messages 
#--------------------------------------------------------------------------- 

# The QUIET tag can be used to turn on/off the messages that are generated 
# by doxygen. Possible values are YES and NO. If left blank NO is used. 

QUIET                  = YES 

# The WARNINGS tag can be used to turn on/off the warning messages that are 
# generated by doxygen. Possible values are YES and NO. If left blank 
# NO is used. 

WARNINGS               = YES 

# If WARN_IF_UNDOCUMENTED is set to YES, then doxygen will generate warnings 
# for undocumented members. If EXTRACT_ALL is set to YES then this flag will 
# automatically be disabled. 

WARN_IF_UNDOCUMENTED   = YES 

# If WARN_IF_DOC_ERROR is set to YES, doxygen will generate warnings for 
# potential errors in the documentation, such as not documenting some 
# parameters in a documented function, or documenting parameters that 
# don't exist or using markup commands wrongly. 

WARN_IF_DOC_ERROR      = YES 

# This WARN_NO_PARAMDOC option can be abled to get warnings for 
# functions that are documented, but have no documentation for their parameters 
# or return value. If set to NO (the default) doxygen will only warn about 
# wrong or incomplete parameter documentation, but not about the absence of 
# documentation. 

WARN_NO_PARAMDOC       = YES 

# The WARN_FORMAT tag determines the format of the warning messages that 
# doxygen can produce. The string should contain the $file, $line, and $text 
# tags, which will be replaced by the file and line number from which the 
# warning originated and the warning text. Optionally the format may contain 
# $version, which will be replaced by the version of the file (if it could 
# be obtained via FILE_VERSION_FILTER) 

WARN_FORMAT            = "$file:$line: $text" 

# The WARN_LOGFILE tag can be used to specify a file to which warning 
# and error messages should be written. If left blank the output is written 
# to stderr. 

WARN_LOGFILE           = doxy.log 

#--------------------------------------------------------------------------- 
# configuration options related to the input files 
#--------------------------------------------------------------------------- 

# The INPUT tag can be used to specify the files and/or directories that contain 
# documented source files. You may enter file names like "myfile.cpp" or 
# directories like "/usr/src/myproject". Separate the files or directories 
# with spaces. 

INPUT                  = .. 

# This tag can be used to specify the character encoding of the source files 
# that doxygen parses. Internally doxygen uses the UTF-8 encoding, which is 
# also the default input encoding. Doxygen uses libiconv (or the iconv built 
# into libc) for the transcoding. See http://www.gnu.org/software/libiconv for 
# the list of possible encodings. 

INPUT_ENCODING         = UTF-8 

# If the value of the INPUT tag contains directories, you can use the 
# FILE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp 
# and *.h) to filter out the source-files in the directories. If left 
# blank the following patterns are tested: 
# *.c *.cc *.cxx *.cpp *.c++ *.java *.ii *.ixx *.ipp *.i++ *.inl *.h *.hh *.hxx 
# *.hpp *.h++ *.idl *.odl *.cs *.php *.php3 *.inc *.m *.mm *.py *.f90 

FILE_PATTERNS          = *.c *.h *.hpp *.hxx *.cc *.cpp *.cxx *.dox 
#FILE_PATTERNS          =

# The RECURSIVE tag can be used to turn specify whether or not subdirectories 
# should be searched for input files as well. Possible values are YES and NO. 
# If left blank NO is used. 

RECURSIVE              = YES 

# The EXCLUDE tag can be used to specify files and/or directories that should 
# excluded from the INPUT source files. This way you can easily exclude a 
# subdirectory from a directory tree whose root is specified with the INPUT tag. 

EXCLUDE                = ../Debug \ 
                         ../Release \ 
                         ../external \ 
                         ../testing \ 
                         ../tools \ 
                         ../arm7/include \ 
                         ../measurements 

# The EXCLUDE_SYMLINKS tag can be used select whether or not files or 
# directories that are symbolic links (a Unix filesystem feature) are excluded 
# from the input. 

EXCLUDE_SYMLINKS       = NO 

# If the value of the INPUT tag contains directories, you can use the 
# EXCLUDE_PATTERNS tag to specify one or more wildcard patterns to exclude 
# certain files from those directories. Note that the wildcards are matched 
# against the file with absolute path, so to exclude all test directories 
# for example use the pattern */test/* 

EXCLUDE_PATTERNS       = 

# The EXCLUDE_SYMBOLS tag can be used to specify one or more symbol names 
# (namespaces, classes, functions, etc.) that should be excluded from the 
# output. The symbol name can be a fully qualified name, a word, or if the 
# wildcard * is used, a substring. Examples: ANamespace, AClass, 
# AClass::ANamespace, ANamespace::*Test 

EXCLUDE_SYMBOLS        = 

# The EXAMPLE_PATH tag can be used to specify one or more files or 
# directories that contain example code fragments that are included (see 
# the \include command). 

EXAMPLE_PATH           = 

# If the value of the EXAMPLE_PATH tag contains directories, you can use the 
# EXAMPLE_PATTERNS tag to specify one or more wildcard pattern (like *.cpp 
# and *.h) to filter out the source-files in the directories. If left 
# blank all files are included. 

EXAMPLE_PATTERNS       = 

# If the EXAMPLE_RECURSIVE tag is set to YES then subdirectories will be 
# searched for input files to be used with the \include or \dontinclude 
# commands irrespective of the value of the RECURSIVE tag. 
# Possible values are YES and NO. If left blank NO is used. 

EXAMPLE_RECURSIVE      = NO 

# The IMAGE_PATH tag can be used to specify one or more files or 
# directories that contain image that are included in the documentation (see 
# the \image command). 

IMAGE_PATH             = 

# The INPUT_FILTER tag can be used to specify a program that doxygen should 
# invoke to filter for each input file. Doxygen will invoke the filter program 
# by executing (via popen()) the command <filter> <input-file>, where <filter> 
# is the value of the INPUT_FILTER tag, and <input-file> is the name of an 
# input file. Doxygen will then use the output that the filter program writes 
# to standard output. 
# If FILTER_PATTERNS is specified, this tag will be 
# ignored. 

INPUT_FILTER           = 

# The FILTER_PATTERNS tag can be used to specify filters on a per file pattern 
# basis. 
# Doxygen will compare the file name with each pattern and apply the 
# filter if there is a match. 
# The filters are a list of the form: 
# pattern=filter (like *.cpp=my_cpp_filter). See INPUT_FILTER for further 
# info on how filters are used. If FILTER_PATTERNS is empty, INPUT_FILTER 
# is applied to all files. 

FILTER_PATTERNS        = 

# If the FILTER_SOURCE_FILES tag is set to YES, the input filter (if set using 
# INPUT_FILTER) will be used to filter the input files when producing source 
# files to browse (i.e. when SOURCE_BROWSER is set to YES). 

FILTER_SOURCE_FILES    = NO 

#--------------------------------------------------------------------------- 
# configuration options related to source browsing 
#--------------------------------------------------------------------------- 

# If the SOURCE_BROWSER tag is set to YES then a list of source files will 
# be generated. Documented entities will be cross-referenced with these sources. 
# Note: To get rid of all source code in the generated output, make sure also 
# VERBATIM_HEADERS is set to NO. 

SOURCE_BROWSER         = YES 

# Setting the INLINE_SOURCES tag to YES will include the body 
# of functions and classes directly in the documentation. 

INLINE_SOURCES         = NO 

# Setting the STRIP_CODE_COMMENTS tag to YES (the default) will instruct 
# doxygen to hide any special comment blocks from generated source code 
# fragments. Normal C and C++ comments will always remain visible. 

STRIP_CODE_COMMENTS    = YES 

# If the REFERENCED_BY_RELATION tag is set to YES 
# then for each documented function all documented 
# functions referencing it will be listed. 

REFERENCED_BY_RELATION = YES 

# If the REFERENCES_RELATION tag is set to YES 
# then for each documented function all documented entities 
# called/used by that function will be listed. 

REFERENCES_RELATION    = NO 

# If the REFERENCES_LINK_SOURCE tag is set to YES (the default) 
# and SOURCE_BROWSER tag is set to YES, then the hyperlinks from 
# functions in REFERENCES_RELATION and REFERENCED_BY_RELATION lists will 
# link to the source code. 
# Otherwise they will link to the documentation. 

REFERENCES_LINK_SOURCE = YES 

# If the USE_HTAGS tag is set to YES then the references to source code 
# will point to the HTML generated by the htags(1) tool instead of doxygen 
# built-in source browser. The htags tool is part of GNU's global source 
# tagging system (see http://www.gnu.org/software/global/global.html). You 
# will need version 4.8.6 or higher. 

USE_HTAGS              = NO 

# If the VERBATIM_HEADERS tag is set to YES (the default) then Doxygen 
# will generate a verbatim copy of the header file for each class for 
# which an include is specified. Set to NO to disable this. 

VERBATIM_HEADERS       = YES 

#--------------------------------------------------------------------------- 
# configuration options related to the alphabetical class index 
#--------------------------------------------------------------------------- 

# If the ALPHABETICAL_INDEX tag is set to YES, an alphabetical index 
# of all compounds will be generated. Enable this if the project 
# contains a lot of classes, structs, unions or interfaces. 

ALPHABETICAL_INDEX     = NO 

# If the alphabetical index is enabled (see ALPHABETICAL_INDEX) then 
# the COLS_IN_ALPHA_INDEX tag can be used to specify the number of columns 
# in which this list will be split (can be a number in the range [1..20]) 

COLS_IN_ALPHA_INDEX    = 5 

# In case all classes in a project start with a common prefix, all 
# classes will be put under the same header in the alphabetical index. 
# The IGNORE_PREFIX tag can be used to specify one or more prefixes that 
# should be ignored while generating the index headers. 

IGNORE_PREFIX          = 

#--------------------------------------------------------------------------- 
# configuration options related to the HTML output 
#--------------------------------------------------------------------------- 

# If the GENERATE_HTML tag is set to YES (the default) Doxygen will 
# generate HTML output. 

GENERATE_HTML          = YES 

# The HTML_OUTPUT tag is used to specify where the HTML docs will be put. 
# If a relative path is entered the value of OUTPUT_DIRECTORY will be 
# put in front of it. If left blank `html' will be used as the default path. 

HTML_OUTPUT            = html 

# The HTML_FILE_EXTENSION tag can be used to specify the file extension for 
# each generated HTML page (for example: .htm,.php,.asp). If it is left blank 
# doxygen will generate files with .html extension. 

HTML_FILE_EXTENSION    = .html 

# The HTML_HEADER tag can be used to specify a personal HTML header for 
# each generated HTML page. If it is left blank doxygen will generate a 
# standard header. 

HTML_HEADER            = 

# The HTML_FOOTER tag can be used to specify a personal HTML footer for 
# each generated HTML page. If it is left blank doxygen will generate a 
# standard footer. 

HTML_FOOTER            = 

# The HTML_STYLESHEET tag can be used to specify a user-defined cascading 
# style sheet that is used by each HTML page. It can be used to 
# fine-tune the look of the HTML output. If the tag is left blank doxygen 
# will generate a default style sheet. Note that doxygen will try to copy 
# the style sheet file to the HTML output directory, so don't put your own 
# stylesheet in the HTML output directory as well, or it will be erased! 

HTML_STYLESHEET        = 

# If the HTML_ALIGN_MEMBERS tag is set to YES, the members of classes, 
# files or namespaces will be aligned in HTML using tables. If set to 
# NO a bullet list will be used. 

HTML_ALIGN_MEMBERS     = YES 

# If the HTML_DYNAMIC_SECTIONS tag is set to YES then the generated HTML 
# documentation will contain sections that can be hidden and shown after the 
# page has loaded. For this to work a browser that supports 
# JavaScript and DHTML is required (for instance Mozilla 1.0+, Firefox 
# Netscape 6.0+, Internet explorer 5.0+, Konqueror, or Safari). 

HTML_DYNAMIC_SECTIONS  = NO 

# If the GENERATE_DOCSET tag is set to YES, additional index files 
# will be generated that can be used as input for Apple's Xcode 3 
# integrated development environment, introduced with OSX 10.5 (Leopard). 
# To create a documentation set, doxygen will generate a Makefile in the 
# HTML output directory. Running make will produce the docset in that 
# directory and running "make install" will install the docset in 
# ~/Library/Developer/Shared/Documentation/DocSets so that Xcode will find 
# it at startup. 
# See http://developer.apple.com/tools/creatingdocsetswithdoxygen.html for more information. 

GENERATE_DOCSET        = NO 

# When GENERATE_DOCSET tag is set to YES, this tag determines the name of the 
# feed. A documentation feed provides an umbrella under which multiple 
# documentation sets from a single provider (such as a company or product suite) 
# can be grouped. 

DOCSET_FEEDNAME        = "Doxygen generated docs" 

# When GENERATE_DOCSET tag is set to YES, this tag specifies a string that 
# should uniquely identify the documentation set bundle. This should be a 
# reverse domain-name style string, e.g. com.mycompany.MyDocSet. Doxygen 
# will append .docset to the name. 

DOCSET_BUNDLE_ID       = org.doxygen.Project 

# If the GENERATE_HTMLHELP tag is set to YES, additional index files 
# will be generated that can be used as input for tools like the 
# Microsoft HTML help workshop to generate a compiled HTML help file (.chm) 
# of the generated HTML documentation. 

GENERATE_HTMLHELP      = NO 

# If the GENERATE_HTMLHELP tag is set to YES, the CHM_FILE tag can 
# be used to specify the file name of the resulting .chm file. You 
# can add a path in front of the file if the result should not be 
# written to the html output directory. 

CHM_FILE               = 

# If the GENERATE_HTMLHELP tag is set to YES, the HHC_LOCATION tag can 
# be used to specify the location (absolute path including file name) of 
# the HTML help compiler (hhc.exe). If non-empty doxygen will try to run 
# the HTML help compiler on the generated index.hhp. 

HHC_LOCATION           = 

# If the GENERATE_HTMLHELP tag is set to YES, the GENERATE_CHI flag 
# controls if a separate .chi index file is generated (YES) or that 
# it should be included in the master .chm file (NO). 

GENERATE_CHI           = NO 

# If the GENERATE_HTMLHELP tag is set to YES, the CHM_INDEX_ENCODING 
# is used to encode HtmlHelp index (hhk), content (hhc) and project file 
# content. 

CHM_INDEX_ENCODING     = 

# If the GENERATE_HTMLHELP tag is set to YES, the BINARY_TOC flag 
# controls whether a binary table of contents is generated (YES) or a 
# normal table of contents (NO) in the .chm file. 

BINARY_TOC             = NO 

# The TOC_EXPAND flag can be set to YES to add extra items for group members 
# to the contents of the HTML help documentation and to the tree view. 

TOC_EXPAND             = NO 

# If the GENERATE_QHP tag is set to YES and both QHP_NAMESPACE and QHP_VIRTUAL_FOLDER 
# are set, an additional index file will be generated that can be used as input for 
# Qt's qhelpgenerator to generate a Qt Compressed Help (.qch) of the generated 
# HTML documentation. 

GENERATE_QHP           = NO 

# If the QHG_LOCATION tag is specified, the QCH_FILE tag can 
# be used to specify the file name of the resulting .qch file. 
# The path specified is relative to the HTML output folder. 

QCH_FILE               = 

# The QHP_NAMESPACE tag specifies the namespace to use when generating 
# Qt Help Project output. For more information please see 
# http://doc.trolltech.com/qthelpproject.html#namespace 

QHP_NAMESPACE          = 

# The QHP_VIRTUAL_FOLDER tag specifies the namespace to use when generating 
# Qt Help Project output. For more information please see 
# http://doc.trolltech.com/qthelpproject.html#virtual-folders 

QHP_VIRTUAL_FOLDER     = doc 

# If QHP_CUST_FILTER_NAME is set, it specifies the name of a custom filter to add. 
# For more information please see 
# http://doc.trolltech.com/qthelpproject.html#custom-filters 

QHP_CUST_FILTER_NAME   = 

# The QHP_CUST_FILT_ATTRS tag specifies the list of the attributes of the custom filter to add.For more information please see 
# <a href="http://doc.trolltech.com/qthelpproject.html#custom-filters">Qt Help Project / Custom Filters</a>. 

QHP_CUST_FILTER_ATTRS  = 

# The QHP_SECT_FILTER_ATTRS tag specifies the list of the attributes this project's 
# filter section matches. 
# <a href="http://doc.trolltech.com/qthelpproject.html#filter-attributes">Qt Help Project / Filter Attributes</a>. 

QHP_SECT_FILTER_ATTRS  = 

# If the GENERATE_QHP tag is set to YES, the QHG_LOCATION tag can 
# be used to specify the location of Qt's qhelpgenerator. 
# If non-empty doxygen will try to run qhelpgenerator on the generated 
# .qhp file. 

QHG_LOCATION           = 

# The DISABLE_INDEX tag can be used to turn on/off the condensed index at 
# top of each HTML page. The value NO (the default) enables the index and 
# the value YES disables it. 

DISABLE_INDEX          = NO 

# This tag can be used to set the number of enum values (range [1..20]) 
# that doxygen will group on one line in the generated HTML documentation. 

ENUM_VALUES_PER_LINE   = 4 

# The GENERATE_TREEVIEW tag is used to specify whether a tree-like index 
# structure should be generated to display hierarchical information. 
# If the tag value is set to YES, a side panel will be generated 
# containing a tree-like index structure (just like the one that 
# is generated for HTML Help). For this to work a browser that supports 
# JavaScript, DHTML, CSS and frames is required (i.e. any modern browser). 
# Windows users are probably better off using the HTML help feature. 

GENERATE_TREEVIEW      = NO 

# By enabling USE_INLINE_TREES, doxygen will generate the Groups, Directories, 
# and Class Hierarchy pages using a tree view instead of an ordered list. 

USE_INLINE_TREES       = NO 

# If the treeview is enabled (see GENERATE_TREEVIEW) then this tag can be 
# used to set the initial width (in pixels) of the frame in which the tree 
# is shown. 

TREEVIEW_WIDTH         = 250 

# Use this tag to change the font size of Latex formulas included 
# as images in the HTML documentation. The default is 10. Note that 
# when you change the font size after a successful doxygen run you need 
# to manually remove any form_*.png images from the HTML output directory 
# to force them to be regenerated. 

FORMULA_FONTSIZE       = 10 

# When the SEARCHENGINE tag is enable doxygen will generate a search box for the HTML output. The underlying search engine uses javascript 
# and DHTML and should work on any modern browser. Note that when using HTML help (GENERATE_HTMLHELP) or Qt help (GENERATE_QHP) 
# there is already a search function so this one should typically 
# be disabled. 

SEARCHENGINE           = YES 

#--------------------------------------------------------------------------- 
# configuration options related to the LaTeX output 
#--------------------------------------------------------------------------- 

# If the GENERATE_LATEX tag is set to YES (the default) Doxygen will 
# generate Latex output. 

GENERATE_LATEX         = NO 

# The LATEX_OUTPUT tag is used to specify where the LaTeX docs will be put. 
# If a relative path is entered the value of OUTPUT_DIRECTORY will be 
# put in front of it. If left blank `latex' will be used as the default path. 

LATEX_OUTPUT           = latex 

# The LATEX_CMD_NAME tag can be used to specify the LaTeX command name to be 
# invoked. If left blank `latex' will be used as the default command name. 

LATEX_CMD_NAME         = latex 

# The MAKEINDEX_CMD_NAME tag can be used to specify the command name to 
# generate index for LaTeX. If left blank `makeindex' will be used as the 
# default command name. 

MAKEINDEX_CMD_NAME     = makeindex 

# If the COMPACT_LATEX tag is set to YES Doxygen generates more compact 
# LaTeX documents. This may be useful for small projects and may help to 
# save some trees in general. 

COMPACT_LATEX          = NO 

# The PAPER_TYPE tag can be used to set the paper type that is used 
# by the printer. Possible values are: a4, a4wide, letter, legal and 
# executive. If left blank a4wide will be used. 

PAPER_TYPE             = a4wide 

# The EXTRA_PACKAGES tag can be to specify one or more names of LaTeX 
# packages that should be included in the LaTeX output. 

EXTRA_PACKAGES         = 

# The LATEX_HEADER tag can be used to specify a personal LaTeX header for 
# the generated latex document. The header should contain everything until 
# the first chapter. If it is left blank doxygen will generate a 
# standard header. Notice: only use this tag if you know what you are doing! 

LATEX_HEADER           = 

# If the PDF_HYPERLINKS tag is set to YES, the LaTeX that is generated 
# is prepared for conversion to pdf (using ps2pdf). The pdf file will 
# contain links (just like the HTML output) instead of page references 
# This makes the output suitable for online browsing using a pdf viewer. 

PDF_HYPERLINKS         = YES 

# If the USE_PDFLATEX tag is set to YES, pdflatex will be used instead of 
# plain latex in the generated Makefile. Set this option to YES to get a 
# higher quality PDF documentation. 

USE_PDFLATEX           = YES 

# If the LATEX_BATCHMODE tag is set to YES, doxygen will add the \\batchmode. 
# command to the generated LaTeX files. This will instruct LaTeX to keep 
# running if errors occur, instead of asking the user for help. 
# This option is also used when generating formulas in HTML. 

LATEX_BATCHMODE        = NO 

# If LATEX_HIDE_INDICES is set to YES then doxygen will not 
# include the index chapters (such as File Index, Compound Index, etc.) 
# in the output. 

LATEX_HIDE_INDICES     = NO 

# If LATEX_SOURCE_CODE is set to YES then doxygen will include source code with syntax highlighting in the LaTeX output. Note that which sources are shown also depends on other settings such as SOURCE_BROWSER. 

LATEX_SOURCE_CODE      = NO 

#--------------------------------------------------------------------------- 
# configuration options related to the RTF output 
#--------------------------------------------------------------------------- 

# If the GENERATE_RTF tag is set to YES Doxygen will generate RTF output 
# The RTF output is optimized for Word 97 and may not look very pretty with 
# other RTF readers or editors. 

GENERATE_RTF           = NO 

# The RTF_OUTPUT tag is used to specify where the RTF docs will be put. 
# If a relative path is entered the value of OUTPUT_DIRECTORY will be 
# put in front of it. If left blank `rtf' will be used as the default path. 

RTF_OUTPUT             = rtf 

# If the COMPACT_RTF tag is set to YES Doxygen generates more compact 
# RTF documents. This may be useful for small projects and may help to 
# save some trees in general. 

COMPACT_RTF            = NO 

# If the RTF_HYPERLINKS tag is set to YES, the RTF that is generated 
# will contain hyperlink fields. The RTF file will 
# contain links (just like the HTML output) instead of page references. 
# This makes the output suitable for online browsing using WORD or other 
# programs which support those fields. 
# Note: wordpad (write) and others do not support links. 

RTF_HYPERLINKS         = NO 

# Load stylesheet definitions from file. Syntax is similar to doxygen's 
# config file, i.e. a series of assignments. You only have to provide 
# replacements, missing definitions are set to their default value. 

RTF_STYLESHEET_FILE    = 

# Set optional variables used in the generation of an rtf document. 
# Syntax is similar to doxygen's config file. 

RTF_EXTENSIONS_FILE    = 

#--------------------------------------------------------------------------- 
# configuration options related to the man page output 
#--------------------------------------------------------------------------- 

# If the GENERATE_MAN tag is set to YES (the default) Doxygen will 
# generate man pages 

GENERATE_MAN           = NO 

# The MAN_OUTPUT tag is used to specify where the man pages will be put. 
# If a relative path is entered the value of OUTPUT_DIRECTORY will be 
# put in front of it. If left blank `man' will be used as the default path. 

MAN_OUTPUT             = man 

# The MAN_EXTENSION tag determines the extension that is added to 
# the generated man pages (default is the subroutine's section .3) 

MAN_EXTENSION          = .3 

# If the MAN_LINKS tag is set to YES and Doxygen generates man output, 
# then it will generate one additional man file for each entity 
# documented in the real man page(s). These additional files 
# only source the real man page, but without them the man command 
# would be unable to find the correct page. The default is NO. 

MAN_LINKS              = NO 

#--------------------------------------------------------------------------- 
# configuration options related to the XML output 
#--------------------------------------------------------------------------- 

# If the GENERATE_XML tag is set to YES Doxygen will 
# generate an XML file that captures the structure of 
# the code including all documentation. 

GENERATE_XML           = NO 

# The XML_OUTPUT tag is used to specify where the XML pages will be put. 
# If a relative path is entered the value of OUTPUT_DIRECTORY will be 
# put in front of it. If left blank `xml' will be used as the default path. 

XML_OUTPUT             = xml 

# The XML_SCHEMA tag can be used to specify an XML schema, 
# which can be used by a validating XML parser to check the 
# syntax of the XML files. 

XML_SCHEMA             = 

# The XML_DTD tag can be used to specify an XML DTD, 
# which can be used by a validating XML parser to check the 
# syntax of the XML files. 

XML_DTD                = 

# If the XML_PROGRAMLISTING tag is set to YES Doxygen will 
# dump the program listings (including syntax highlighting 
# and cross-referencing information) to the XML output. Note that 
# enabling this will significantly increase the size of the XML output. 

XML_PROGRAMLISTING     = YES 

#--------------------------------------------------------------------------- 
# configuration options for the AutoGen Definitions output 
#--------------------------------------------------------------------------- 

# If the GENERATE_AUTOGEN_DEF tag is set to YES Doxygen will 
# generate an AutoGen Definitions (see autogen.sf.net) file 
# that captures the structure of the code including all 
# documentation. Note that this feature is still experimental 
# and incomplete at the moment. 

GENERATE_AUTOGEN_DEF   = NO 

#--------------------------------------------------------------------------- 
# configuration options related to the Perl module output 
#--------------------------------------------------------------------------- 

# If the GENERATE_PERLMOD tag is set to YES Doxygen will 
# generate a Perl module file that captures the structure of 
# the code including all documentation. Note that this 
# feature is still experimental and incomplete at the 
# moment. 

GENERATE_PERLMOD       = NO 

# If the PERLMOD_LATEX tag is set to YES Doxygen will generate 
# the necessary Makefile rules, Perl scripts and LaTeX code to be able 
# to generate PDF and DVI output from the Perl module output. 

PERLMOD_LATEX          = NO 

# If the PERLMOD_PRETTY tag is set to YES the Perl module output will be 
# nicely formatted so it can be parsed by a human reader. 
# This is useful 
# if you want to understand what is going on. 
# On the other hand, if this 
# tag is set to NO the size of the Perl module output will be much smaller 
# and Perl will parse it just the same. 

PERLMOD_PRETTY         = YES 

# The names of the make variables in the generated doxyrules.make file 
# are prefixed with the string contained in PERLMOD_MAKEVAR_PREFIX. 
# This is useful so different doxyrules.make files included by the same 
# Makefile don't overwrite each other's variables. 

PERLMOD_MAKEVAR_PREFIX = 

#--------------------------------------------------------------------------- 
# Configuration options related to the preprocessor 
#--------------------------------------------------------------------------- 

# If the ENABLE_PREPROCESSING tag is set to YES (the default) Doxygen will 
# evaluate all C-preprocessor directives found in the sources and include 
# files. 

ENABLE_PREPROCESSING   = YES 

# If the MACRO_EXPANSION tag is set to YES Doxygen will expand all macro 
# names in the source code. If set to NO (the default) only conditional 
# compilation will be performed. Macro expansion can be done in a controlled 
# way by setting EXPAND_ONLY_PREDEF to YES. 

MACRO_EXPANSION        = NO 

# If the EXPAND_ONLY_PREDEF and MACRO_EXPANSION tags are both set to YES 
# then the macro expansion is limited to the macros specified with the 
# PREDEFINED and EXPAND_AS_DEFINED tags. 

EXPAND_ONLY_PREDEF     = NO 

# If the SEARCH_INCLUDES tag is set to YES (the default) the includes files 
# in the INCLUDE_PATH (see below) will be search if a #include is found. 

SEARCH_INCLUDES        = YES 

# The INCLUDE_PATH tag can be used to specify one or more directories that 
# contain include files that are not input files but should be processed by 
# the preprocessor. 

INCLUDE_PATH           = 

# You can use the INCLUDE_FILE_PATTERNS tag to specify one or more wildcard 
# patterns (like *.h and *.hpp) to filter out the header-files in the 
# directories. If left blank, the patterns specified with FILE_PATTERNS will 
# be used. 

INCLUDE_FILE_PATTERNS  = 

# The PREDEFINED tag can be used to specify one or more macro names that 
# are defined before the preprocessor is started (similar to the -D option of 
# gcc). The argument of the tag is a list of macros of the form: name 
# or name=definition (no spaces). If the definition and the = are 
# omitted =1 is assumed. To prevent a macro definition from being 
# undefined via #undef or recursively expanded use the := operator 
# instead of the = operator. 

PREDEFINED             = IMU_PIXHAWK_V200 IMU_PIXHAWK_V210 

# If the MACRO_EXPANSION and EXPAND_ONLY_PREDEF tags are set to YES then 
# this tag can be used to specify a list of macro names that should be expanded. 
# The macro definition that is found in the sources will be used. 
# Use the PREDEFINED tag if you want to use a different macro definition. 

EXPAND_AS_DEFINED      =  

# If the SKIP_FUNCTION_MACROS tag is set to YES (the default) then 
# doxygen's preprocessor will remove all function-like macros that are alone 
# on a line, have an all uppercase name, and do not end with a semicolon. Such 
# function macros are typically used for boiler-plate code, and will confuse 
# the parser if not removed. 

SKIP_FUNCTION_MACROS   = NO 

#--------------------------------------------------------------------------- 
# Configuration::additions related to external references 
#--------------------------------------------------------------------------- 

# The TAGFILES option can be used to specify one or more tagfiles. 
# Optionally an initial location of the external documentation 
# can be added for each tagfile. The format of a tag file without 
# this location is as follows: 
# 
# TAGFILES = file1 file2 ... 
# Adding location for the tag files is done as follows: 
# 
# TAGFILES = file1=loc1 "file2 = loc2" ... 
# where "loc1" and "loc2" can be relative or absolute paths or 
# URLs. If a location is present for each tag, the installdox tool 
# does not have to be run to correct the links. 
# Note that each tag file must have a unique name 
# (where the name does NOT include the path) 
# If a tag file is not located in the directory in which doxygen 
# is run, you must also specify the path to the tagfile here. 

TAGFILES               = 

# When a file name is specified after GENERATE_TAGFILE, doxygen will create 
# a tag file that is based on the input files it reads. 

GENERATE_TAGFILE       = 

# If the ALLEXTERNALS tag is set to YES all external classes will be listed 
# in the class index. If set to NO only the inherited external classes 
# will be listed. 

ALLEXTERNALS           = NO 

# If the EXTERNAL_GROUPS tag is set to YES all external groups will be listed 
# in the modules index. If set to NO, only the current project's groups will 
# be listed. 

EXTERNAL_GROUPS        = YES 

# The PERL_PATH should be the absolute path and name of the perl script 
# interpreter (i.e. the result of `which perl'). 

PERL_PATH              = /usr/bin/perl 

#--------------------------------------------------------------------------- 
# Configuration options related to the dot tool 
#--------------------------------------------------------------------------- 

# If the CLASS_DIAGRAMS tag is set to YES (the default) Doxygen will 
# generate a inheritance diagram (in HTML, RTF and LaTeX) for classes with base 
# or super classes. Setting the tag to NO turns the diagrams off. Note that 
# this option is superseded by the HAVE_DOT option below. This is only a 
# fallback. It is recommended to install and use dot, since it yields more 
# powerful graphs. 

CLASS_DIAGRAMS         = YES 

# You can define message sequence charts within doxygen comments using the \msc 
# command. Doxygen will then run the mscgen tool (see 
# http://www.mcternan.me.uk/mscgen/) to produce the chart and insert it in the 
# documentation. The MSCGEN_PATH tag allows you to specify the directory where 
# the mscgen tool resides. If left empty the tool is assumed to be found in the 
# default search path. 

MSCGEN_PATH            = 

# If set to YES, the inheritance and collaboration graphs will hide 
# inheritance and usage relations if the target is undocumented 
# or is not a class. 

HIDE_UNDOC_RELATIONS   = YES 

# If you set the HAVE_DOT tag to YES then doxygen will assume the dot tool is 
# available from the path. This tool is part of Graphviz, a graph visualization 
# toolkit from AT&T and Lucent Bell Labs. The other options in this section 
# have no effect if this option is set to NO (the default) 

HAVE_DOT               = YES 

# By default doxygen will write a font called FreeSans.ttf to the output 
# directory and reference it in all dot files that doxygen generates. This 
# font does not include all possible unicode characters however, so when you need 
# these (or just want a differently looking font) you can specify the font name 
# using DOT_FONTNAME. You need need to make sure dot is able to find the font, 
# which can be done by putting it in a standard location or by setting the 
# DOTFONTPATH environment variable or by setting DOT_FONTPATH to the directory 
# containing the font. 

DOT_FONTNAME           = FreeSans 

# The DOT_FONTSIZE tag can be used to set the size of the font of dot graphs. 
# The default size is 10pt. 

DOT_FONTSIZE           = 10 

# By default doxygen will tell dot to use the output directory to look for the 
# FreeSans.ttf font (which doxygen will put there itself). If you specify a 
# different font using DOT_FONTNAME you can set the path where dot 
# can find it using this tag. 

DOT_FONTPATH           = 

# If the CLASS_GRAPH and HAVE_DOT tags are set to YES then doxygen 
# will generate a graph for each documented class showing the direct and 
# indirect inheritance relations. Setting this tag to YES will force the 
# the CLASS_DIAGRAMS tag to NO. 

CLASS_GRAPH            = YES 

# If the COLLABORATION_GRAPH and HAVE_DOT tags are set to YES then doxygen 
# will generate a graph for each documented class showing the direct and 
# indirect implementation dependencies (inheritance, containment, and 
# class references variables) of the class with other documented classes. 

COLLABORATION_GRAPH    = YES 

# If the GROUP_GRAPHS and HAVE_DOT tags are set to YES then doxygen 
# will generate a graph for groups, showing the direct groups dependencies 

GROUP_GRAPHS           = YES 

# If the UML_LOOK tag is set to YES doxygen will generate inheritance and 
# collaboration diagrams in a style similar to the OMG's Unified Modeling 
# Language. 

UML_LOOK               = NO 

# If set to YES, the inheritance and collaboration graphs will show the 
# relations between templates and their instances. 

TEMPLATE_RELATIONS     = NO 

# If the ENABLE_PREPROCESSING, SEARCH_INCLUDES, INCLUDE_GRAPH, and HAVE_DOT 
# tags are set to YES then doxygen will generate a graph for each documented 
# file showing the direct and indirect include dependencies of the file with 
# other documented files. 

INCLUDE_GRAPH          = YES 

# If the ENABLE_PREPROCESSING, SEARCH_INCLUDES, INCLUDED_BY_GRAPH, and 
# HAVE_DOT tags are set to YES then doxygen will generate a graph for each 
# documented header file showing the documented files that directly or 
# indirectly include this file. 

INCLUDED_BY_GRAPH      = YES 

# If the CALL_GRAPH and HAVE_DOT options are set to YES then 
# doxygen will generate a call dependency graph for every global function 
# or class method. Note that enabling this option will significantly increase 
# the time of a run. So in most cases it will be better to enable call graphs 
# for selected functions only using the \callgraph command. 

CALL_GRAPH             = NO 

# If the CALLER_GRAPH and HAVE_DOT tags are set to YES then 
# doxygen will generate a caller dependency graph for every global function 
# or class method. Note that enabling this option will significantly increase 
# the time of a run. So in most cases it will be better to enable caller 
# graphs for selected functions only using the \callergraph command. 

CALLER_GRAPH           = NO 

# If the GRAPHICAL_HIERARCHY and HAVE_DOT tags are set to YES then doxygen 
# will graphical hierarchy of all classes instead of a textual one. 

GRAPHICAL_HIERARCHY    = YES 

# If the DIRECTORY_GRAPH, SHOW_DIRECTORIES and HAVE_DOT tags are set to YES 
# then doxygen will show the dependencies a directory has on other directories 
# in a graphical way. The dependency relations are determined by the #include 
# relations between the files in the directories. 

DIRECTORY_GRAPH        = YES 

# The DOT_IMAGE_FORMAT tag can be used to set the image format of the images 
# generated by dot. Possible values are png, jpg, or gif 
# If left blank png will be used. 

DOT_IMAGE_FORMAT       = png 

# The tag DOT_PATH can be used to specify the path where the dot tool can be 
# found. If left blank, it is assumed the dot tool can be found in the path. 

DOT_PATH               = 

# The DOTFILE_DIRS tag can be used to specify one or more directories that 
# contain dot files that are included in the documentation (see the 
# \dotfile command). 

DOTFILE_DIRS           = 

# The DOT_GRAPH_MAX_NODES tag can be used to set the maximum number of 
# nodes that will be shown in the graph. If the number of nodes in a graph 
# becomes larger than this value, doxygen will truncate the graph, which is 
# visualized by representing a node as a red box. Note that doxygen if the 
# number of direct children of the root node in a graph is already larger than 
# DOT_GRAPH_MAX_NODES then the graph will not be shown at all. Also note 
# that the size of a graph can be further restricted by MAX_DOT_GRAPH_DEPTH. 

DOT_GRAPH_MAX_NODES    = 50 

# The MAX_DOT_GRAPH_DEPTH tag can be used to set the maximum depth of the 
# graphs generated by dot. A depth value of 3 means that only nodes reachable 
# from the root by following a path via at most 3 edges will be shown. Nodes 
# that lay further from the root node will be omitted. Note that setting this 
# option to 1 or 2 may greatly reduce the computation time needed for large 
# code bases. Also note that the size of a graph can be further restricted by 
# DOT_GRAPH_MAX_NODES. Using a depth of 0 means no depth restriction. 

MAX_DOT_GRAPH_DEPTH    = 0 

# Set the DOT_TRANSPARENT tag to YES to generate images with a transparent 
# background. This is disabled by default, because dot on Windows does not 
# seem to support this out of the box. Warning: Depending on the platform used, 
# enabling this option may lead to badly anti-aliased labels on the edges of 
# a graph (i.e. they become hard to read). 

DOT_TRANSPARENT        = NO 

# Set the DOT_MULTI_TARGETS tag to YES allow dot to generate multiple output 
# files in one run (i.e. multiple -o and -T options on the command line). This 
# makes dot run faster, but since only newer versions of dot (>1.8.10) 
# support this, this feature is disabled by default. 

DOT_MULTI_TARGETS      = YES 

# If the GENERATE_LEGEND tag is set to YES (the default) Doxygen will 
# generate a legend page explaining the meaning of the various boxes and 
# arrows in the dot generated graphs. 

GENERATE_LEGEND        = YES 

# If the DOT_CLEANUP tag is set to YES (the default) Doxygen will 
# remove the intermediate dot files that are used to generate 
# the various graphs. 

DOT_CLEANUP            = YES  }}
        -DCMAKE_CXX_COMPILER=${{ MAVLink Micro Air Vehicle Message Marshalling Library

Files:

- **Doxyfile**: -
- **MAVLink2.md**: MAVLink2 Documentation
- **README**: This file
- **mavlink.php**: Generates online documentation from MAVLink XML - http://mavlink.org/messages/common
  - **mavlink_to_html_table.xsl**: XSL transform used by **mavlink.php**
  - **mavlink.css**: CSS used by online documentation
- **mavlink_gitbook.py**: Generates documentation from MAVLink XML that can be imported into gitbook
  - **mavlink_to_html_table_gitbook.xsl**: XSL transform used by **mavlink_gitbook.py**
----

For more information, please visit:

http://qgroundcontrol.org/mavlink/start

(c) 2009-2010 / PIXHAWK Team }}
        -DCMAKE_C_COMPILER=${{ body {
    font-family:'Helvetica',Arial;
    font-size:90%;
    margin: 80px;
}

h1 {
    margin-top: 2em;
}

h2 {
    margin-top: 1em;
}

h3 {
    margin-top: 0.8em;
    font-size:150%;
}

p.description {
    font-style:italic;
}

table {
    margin-bottom: 5em;
}

table.sortable {
    border: 1px solid #656575;
    width: 100%;
}

table.sortable th {
    margin: 5px;
}

#tr:nth-child(odd)   { background-color:#eee; }
#tr:nth-child(even)  { background-color:#fff; }

table.sortable thead {
    background-color:#eee;
    color:#666666;
    font-weight: bold;
    cursor: default;
}

table.sortable td {
    margin: 5px 5px 20px 5px;
    vertical-align: top;
}

table.sortable td.mavlink_name {
    color:#226633;
    font-weight: bold;
    width: 25%;
    vertical-align: top;
}

table.sortable td.mavlink_mission_param {
    color:#334455;
    font-weight: bold;
    width: 25%;
}

table.sortable td.mavlink_type {
    color:#323232;
    font-weight: normal;
    width: 12%;
}

table.sortable td.mavlink_comment {
    color:#555555;
    font-weight: normal;
    width: 60%;
}

p.description {
    color:#808080;
    font-weight: normal;
} }}
        -DCMAKE_BUILD_TYPE=${{ <?php

// Requires the installation of php5-xsl
// e.g. on Debian/Ubuntu: sudo apt-get install php5-xsl

// Load the file from the repository / server.
// Update this URL if the file location changes

$xml_file_name = "https://raw.github.com/mavlink/mavlink/master/message_definitions/v1.0/common.xml";

// Load the XSL transformation file from the repository / server.
// This file can be updated by any client to adjust the table

$xsl_file_name= "https://raw.github.com/mavlink/mavlink/master/doc/mavlink_to_html_table.xsl";

// Load data XML file
$xml = file_get_contents($xml_file_name);
$xml_doc = new DomDocument;
$xml_doc->loadXML($xml);

// Load stylesheet XSL file
$xsl = file_get_contents($xsl_file_name);
$xsl_doc = new DomDocument;
$xsl_doc->loadXML($xsl);

$xsltproc = new XsltProcessor();
$xsltproc->importStylesheet($xsl_doc);
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
        <title>MAVLINK Common Message set specifications</title>
        <link rel="stylesheet" type="text/css" href="mavlink.css">
</head>
<body>
<h1>MAVLINK Common Message Set</h1>

<p>
These messages define the common message set, which is the reference message set implemented by most ground control stations and autopilots.
</p>

<?php
// process the files and write the output to $out_file
if ($html = $xsltproc->transformToXML($xml_doc))
{
  echo $html;
}
else
{
  trigger_error("XSL transformation failed",E_USER_ERROR);
}

?>

<br />
<br />

<p>
Messages are defined by the <a href="https://raw.github.com/mavlink/mavlink/master/message_definitions/v1.0/common.xml">common.xml</a> file. The C packing/unpacking code is generated from this specification, as well as the HTML documentaiton in the section above.<br />
<br />
<i>The XML displayed here is updated on every commit and therefore up-to-date.</i>
</p>
</body>
</html>
<?php
//require_once("inc/geshi.php");
//$xml_file_name = "http://github.com/pixhawk/mavlink/raw/master/mavlink_standard_message.xml";
//
//// Load data XML file
//$xml = file_get_contents($xml_file_name);
//
//// Show the current code
//$geshi_xml = new GeSHi($xml, 'xml');
//$display_xml = $geshi_xml->parse_code();
//
//echo $display_xml;

?> }}
        -S ${{ #! /usr/bin/python

"""
This script generates markdown files for all the MAVLink message definition XML at: 
https://github.com/mavlink/mavlink/tree/master/message_definitions/v1.0
  
The files can be imported into a gitbook to display the messages as HTML

The script runs on both Python2 and Python 3. The following libraries must be imported: lxml, requests, bs4.

The file is run in mavlink/doc/ with no arguments. It writes the files to /messages/
"""

import lxml.etree as ET
import requests
from bs4 import BeautifulSoup as bs
import re
import os # for walk


xsl_file_name = "mavlink_to_html_table_gitbook.xsl"
xml_message_definitions_dir_name = "../message_definitions/v1.0/"

output_dir = "./messages/"
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

# File for index
index_file_name = "README.md"
index_file_name = output_dir + index_file_name

# Get XSLT
with open(xsl_file_name, 'r') as content_file:
    xsl_file = content_file.read()
xslt = ET.fromstring(xsl_file)

#initialise text for index file. 
index_text='<!-- THIS FILE IS AUTO-GENERATED (DO NOT UPDATE GITBOOK): https://github.com/mavlink/mavlink/blob/master/doc/mavlink_gitbook.py -->'
index_text+='\n# Message Definitions'
index_text+='\n\nMAVLink messages are defined in XML files in the [mavlink/message definitions](https://github.com/mavlink/mavlink/blob/master/message_definitions/) folder. The messages that are common to all systems are defined in [common.xml](https://github.com/mavlink/mavlink/blob/master/message_definitions/v1.0/common.xml) (only messages contained in this file are considered standard messages). MAVLink protocol-specific messages and vendor-specific messages are stored in separate XML files.'
index_text+='\n\nThe common messages are provided as human-readable tables in: [Common](../messages/common.md).'
index_text+='\n\n> **Note** Vendor forks of MAVLink may contain messages that are not get merged, and hence will not appear in this documentation.'
index_text+='\n\nThe human-readable forms of the vendor XML files are linked below:'


#Fix up the BeautifulSoup output so to fix build-link errors in the generated gitbook.
## BS puts each tag/content in its own line. Gitbook generates anchors using the spaces/newlines. 
## This puts displayed text content immediately within tags so that anchors/links generate properly
def fix_content_in_tags(input_html):
    #print("fix_content_in_tags was called")
    def remove_space_between_content_tags(matchobj):
        stripped_string=matchobj.group(1).strip()
        return '>%s<' % stripped_string

    input_html=re.sub(r'\>(\s+?\w+?.*?)\<', remove_space_between_content_tags, input_html,flags=re.DOTALL)
    return input_html

def fix_include_file_extension(input_html):
    ## Fixes up file extension .xml.md.unlikely (easier than fixing up the XSLT to strip file extensions!)
    input_html=input_html.replace('.xml.md.unlikely','.md')
    return input_html

def strip_text_before_string(original_text,strip_text):
    # Strip out all text before some string
    index=original_text.find(strip_text)
    stripped_string=original_text
    if index !=-1 :
        stripped_string = stripped_string[index:] 
    return stripped_string

def inject_top_level_docs(input_html,filename):
    #Inject top level heading and other details.
    print('FILENAME: %s' % filename)
    insert_text='<!-- THIS FILE IS AUTO-GENERATED: https://github.com/mavlink/mavlink/blob/master/doc/mavlink_gitbook.py -->'
    if filename == 'common.xml':
        insert_text+='\n# MAVLINK Common Message Set'
        insert_text+='\n\nThese messages define the common message set, which is the reference message set implemented by most ground control stations and autopilots.'
        insert_text+='\n\n*This is a human-readable form of the XML definition file: [common.xml](https://github.com/mavlink/mavlink/blob/master/message_definitions/v1.0/common.xml).*'
    elif filename == 'ardupilotmega.xml':
        insert_text+='\n# MAVLINK ArduPilotMega Message Set'
        insert_text+='\n\nThese messages define the APM specific message set, which is custom to [http://ardupilot.org](http://ardupilot.org).'
        insert_text+='\n\n*This is a human-readable form of the XML definition file: [ardupilotmega.xml](https://github.com/mavlink/mavlink/blob/master/message_definitions/v1.0/ardupilotmega.xml).*'
        insert_text+='\n\n> **Warning** The ArduPilot MAVLink fork of [ardupilotmega.xml](https://github.com/ArduPilot/mavlink/blob/master/message_definitions/v1.0/ardupilotmega.xml) may contain messages that have not yet been merged into this documentation.'
    else:
        insert_text+='\n# MAVLINK Message Set: %s' % filename
        insert_text+='\n\n*This is a human-readable form of the XML definition file: [%s](https://github.com/mavlink/mavlink/blob/master/message_definitions/v1.0/%s).*' % (filename, filename)


    insert_text+='\n\n<span></span>\n> **Note** MAVLink 2 messages have an ID > 255 and are marked up using **(MAVLink 2)** in their description.'
    insert_text+='\n\n<span id="mav2_extension_field"></span>\n> **Note** MAVLink 2 extension fields that have been added to MAVLink 1 messages are displayed in blue.'
    input_html=insert_text+'\n\n'+input_html
    #print(input_html)
    return input_html



for subdir, dirs, files in os.walk(xml_message_definitions_dir_name):
    for file in files:
        print(file)
        if not file.endswith('.xml'): #only process xml files.
           continue
        xml_file_name = xml_message_definitions_dir_name+file
        with open(xml_file_name, 'r') as content_file:
            xml_file = content_file.read()
            dom = ET.fromstring(xml_file)
            transform = ET.XSLT(xslt)
            newdom = transform(dom)

            #Prettify the HTML using BeautifulSoup
            soup=bs(str(newdom), "lxml")
            prettyHTML=soup.prettify()

            #Strip out text before <html> tag in XSLT output
            prettyHTML=strip_text_before_string(prettyHTML,'<html>')
            prettyHTML = fix_content_in_tags(prettyHTML)

            #Inject a heading and doc-type intro (markdown format)
            prettyHTML = inject_top_level_docs(prettyHTML,file)

            #Replace invalid file extensions (workaround for xslt)
            prettyHTML = fix_include_file_extension(prettyHTML)

            #Write output markdown file
            output_file_name = file.rsplit('.',1)[0]+".md"
            output_file_name_withdir = output_dir+output_file_name
            print("Output filename: %s" % output_file_name)

            with open(output_file_name_withdir, 'w') as out:
                out.write(prettyHTML )

            if not file=='common.xml':
                index_text+='\n* [%s](%s)' % (file,output_file_name)

#Write the index - Disabled for now.
#with open(index_file_name, 'w') as content_file:
#    content_file.write(index_text)


print("COMPLETED")

 }}

    - name: <?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="//include">
   <h1>MAVLink Include Files</h1>
   <p><strong><em>Including files: </em><xsl:value-of select="." /></strong></p>
</xsl:template>

<xsl:template match="//enums">
   <h1>Official MAVLink Documentation</h1>
   <p>
    Please refer to the <a href="https://mavlink.io/en/messages/common.html">official MAVLink message documentation</a> for more details.
   </p>
   <h1>MAVLink Type Enumerations</h1>
   <xsl:apply-templates />
</xsl:template>

<xsl:template match="//messages">
   <h1>MAVLink Messages</h1>
   <xsl:apply-templates />
</xsl:template>

<xsl:template match="//message">
  <a>
    <xsl:attribute name="name">
      <xsl:value-of select="@name"/>
    </xsl:attribute>
  </a>
  <h3 class="mavlink_message_name"><xsl:value-of select="@name" /> (
   <a>
    <xsl:attribute name="href">
      #<xsl:value-of select="@name"/>
    </xsl:attribute>
   #<xsl:value-of select="@id" />
   </a>
   )</h3>
   <p class="description"><xsl:value-of select="description" /></p>
   <table class="sortable">
   <thead>
   <tr>
     <th class="mavlink_field_header">Field Name</th>
     <th class="mavlink_field_header">Type</th>
     <th class="mavlink_field_header">Description</th>
   </tr>
   </thead>
   <tbody>
   <xsl:apply-templates select="field" />
  </tbody>
  </table>
</xsl:template>

<xsl:template match="//field">
   <tr class="mavlink_field">
   <td class="mavlink_name" valign="top"><xsl:value-of select="@name" /></td>
   <td class="mavlink_type" valign="top"><xsl:value-of select="@type" /></td>
   <td class="mavlink_comment"><xsl:value-of select="." /></td>
   </tr>
</xsl:template>

<xsl:template match="//version">
   <h2>MAVLink Protocol Version</h2>
   <p>The current MAVLink version is 2.<xsl:value-of select="." />. The minor version numbers (after the dot) range from 1-255. </p>
</xsl:template>

<xsl:template match="//enum">
   <a>
    <xsl:attribute name="name">
      ENUM_<xsl:value-of select="@name"/>
    </xsl:attribute>
  </a>
   <h3 class="mavlink_message_name"><xsl:value-of select="@name" /></h3>

   <p class="description"><xsl:value-of select="description" /></p>
   <table class="sortable">
   <thead>
   <tr>
     <th class="mavlink_field_header">CMD ID</th>
     <th class="mavlink_field_header">Field Name</th>
     <th class="mavlink_field_header">Description</th>
   </tr>
   </thead>
   <tbody>
   <xsl:apply-templates select="entry" />
  </tbody>
  </table>
</xsl:template>

<xsl:template match="//entry">
   <tr class="mavlink_field" id="{@name}">
   <td class="mavlink_type" valign="top"><xsl:value-of select="@value" /></td>
   <td class="mavlink_name" valign="top"><xsl:value-of select="@name" /></td>
   <td class="mavlink_comment"><xsl:value-of select="description" /></td>
   </tr>
   <tr>
     <td></td>
            <xsl:apply-templates select="param" />
   </tr>
   <tr>
    <td colspan="3"><br /></td>
   </tr>
</xsl:template>

<xsl:template match="//param">
   <tr>
   <td></td>
   <td class="mavlink_mission_param" valign="top">Mission Param #<xsl:value-of select="@index" /></td>
   <td class="mavlink_comment"><xsl:value-of select="." /></td>
   </tr>
</xsl:template>


</xsl:stylesheet>
      # Build your program with the given configuration. Note that --config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      run: cmake --build ${{ <?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="//include">
   <p><strong>MAVLink Include Files: </strong> <a><xsl:attribute name="href"><xsl:value-of select="."/>.md.unlikely</xsl:attribute><xsl:value-of select="." /></a> </p>
</xsl:template>

<xsl:template match="//enums">
   <h2>MAVLink Type Enumerations</h2>
   <xsl:apply-templates />
</xsl:template>

<xsl:template match="//messages">
   <h2>MAVLink Messages</h2>
   <xsl:apply-templates />
</xsl:template>

<xsl:template match="//message">
  <h3 class="mavlink_message_name">
   <xsl:attribute name="id"><xsl:value-of select="@name"/></xsl:attribute>
   <xsl:value-of select="@name" /> (
   <a>
    <xsl:attribute name="href">#<xsl:value-of select="@name"/></xsl:attribute>
    #<xsl:value-of select="@id" />
   </a>
  )</h3>
   <p class="description">
     <xsl:if test='@id > 255'><strong>(MAVLink 2) </strong></xsl:if>
     <xsl:value-of select="description" /></p>
   <table class="sortable">
   <thead>
   <tr>
     <th class="mavlink_field_header">Field Name</th>
     <th class="mavlink_field_header">Type</th>
     <th class="mavlink_field_header">Description</th>
   </tr>
   </thead>
   <tbody>
   <xsl:apply-templates select="field" /> 
  </tbody>
  </table>
</xsl:template>

<xsl:template match="//field">
   <tr class="mavlink_field">
   <xsl:choose>
     <xsl:when test="preceding-sibling::extensions">
       <td class="mavlink_name" valign="top" style="color:blue;"><xsl:value-of select="@name" />&#160;<a href="#mav2_extension_field" title="MAVLink2 extension field">**</a></td>
     </xsl:when>
     <xsl:otherwise>
       <td class="mavlink_name" valign="top"><xsl:value-of select="@name" /></td>
     </xsl:otherwise>
   </xsl:choose>

   <td class="mavlink_type" valign="top"><xsl:value-of select="@type" /></td>
   <td class="mavlink_comment"> <xsl:value-of select="." />
     <xsl:if test='@units'>
     (Units: <xsl:value-of select="@units" />)
     </xsl:if>
     <xsl:if test='@enum'>
     (Enum: <a><xsl:attribute name="href">#<xsl:value-of select="@enum" /></xsl:attribute><xsl:value-of select="@enum" /></a>)
     </xsl:if>
   </td>
   </tr>
</xsl:template>

<xsl:template match="//version">
   <h2>MAVLink Protocol Version</h2>
   <p>The current MAVLink version is 2.<xsl:value-of select="." />. The minor version numbers (after the dot) range from 1-255. </p>
</xsl:template>

<xsl:template match="//dialect">
   <p>This file has protocol dialect: <xsl:value-of select="." />.</p>
</xsl:template>

<xsl:template match="//enum">
   <h3 class="mavlink_message_name">    
     <xsl:attribute name="id"><xsl:value-of select="@name"/></xsl:attribute>
     <a><xsl:attribute name="href">#<xsl:value-of select="@name"/></xsl:attribute>
     <xsl:value-of select="@name" /></a></h3>

   <p class="description"><xsl:value-of select="description" /></p>
   <table class="sortable">
   <thead>
   <tr>
     <th class="mavlink_field_header">CMD ID</th>
     <th class="mavlink_field_header">Field Name</th>
     <th class="mavlink_field_header">Description</th>
   </tr>
   </thead>
   <tbody>
   <xsl:apply-templates select="entry" />
  </tbody>
  </table>
</xsl:template>

<xsl:template match="//entry">
   <tr class="mavlink_field" id="{@name}">
   <td class="mavlink_type" valign="top"><xsl:value-of select="@value" /></td>
   <td class="mavlink_name" valign="top"><a><xsl:attribute name="href">#<xsl:value-of select="@name"/></xsl:attribute>
   <xsl:value-of select="@name" /></a></td>
   <td class="mavlink_comment"><xsl:value-of select="description" /></td>
   </tr>
<xsl:if test='param'>
   <tr>
     <td></td>
     <xsl:apply-templates select="param" />
   </tr>
   <tr>
    <td colspan="3"><br /></td>
   </tr>
</xsl:if>


</xsl:template>

<xsl:template match="//param">
   <tr>
   <td></td>
   <td class="mavlink_mission_param" valign="top">Mission Param #<xsl:value-of select="@index" /></td>
   <td class="mavlink_comment"><xsl:value-of select="." /></td>
   </tr>
</xsl:template>


</xsl:stylesheet> }} --config ${{ 


/*
**        Description:        To decode the data, encoded using 3 character encoding technique.
*/





#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void ThreeCharacterEncoding_Decode(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint32_t *decodedOutput);

int main(void)
{
        uint32_t forDecodingDebug;
        uint8_t byte3, byte2, byte1;
        while(1)
        {
                printf("Input 3 bytes of data : \n");
                scanf("%u%u%u", &byte3, &byte2, &byte1);
                ThreeCharacterEncoding_Decode(byte3, byte2, byte1, &forDecodingDebug);
                printf("Decoded Value = %d\n", forDecodingDebug);
        }
}


/*
**        @brief                                                                                        Function to decode the 3 character encoding, to extract the encoded distance.
**        @byte3(PARAM_IN)                                                Higher encoded byte.
**        @byte2(PARAM_IN)                                                High encoded byte.
**        @byte1(PARAM_IN)                                                Low encoded byte.
**        @decodedOutput(PARAM_OUT)                Pointer to the variable which will contain the decoded output.
*/
void ThreeCharacterEncoding_Decode(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint32_t *decodedOutput)
{
        uint8_t temp_byte1;
        uint8_t temp_byte2;
        uint8_t temp_byte3;

        /*        Decryption starts with subtracting 0x30 from each of the encoded bytes        */
        temp_byte3 = byte3 - 0x30;
        temp_byte2 = byte2 - 0x30;
        temp_byte1 = byte1 - 0x30;

        /*        Reset all bits of the output variable        */
        *decodedOutput &= ~(*decodedOutput);

        /*        Populate the output variable with decoded data        */
        *decodedOutput |= (temp_byte3 << 12);
        *decodedOutput |= (temp_byte2 << 6);
        *decodedOutput |= (temp_byte1 << 0);

        return;
} }}

    - name:                     GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The GNU General Public License is a free, copyleft license for
software and other kinds of works.

  The licenses for most software and other practical works are designed
to take away your freedom to share and change the works.  By contrast,
the GNU General Public License is intended to guarantee your freedom to
share and change all versions of a program--to make sure it remains free
software for all its users.  We, the Free Software Foundation, use the
GNU General Public License for most of our software; it applies also to
any other work released this way by its authors.  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
them if you wish), that you receive source code or can get it if you
want it, that you can change the software or use pieces of it in new
free programs, and that you know you can do these things.

  To protect your rights, we need to prevent others from denying you
these rights or asking you to surrender the rights.  Therefore, you have
certain responsibilities if you distribute copies of the software, or if
you modify it: responsibilities to respect the freedom of others.

  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must pass on to the recipients the same
freedoms that you received.  You must make sure that they, too, receive
or can get the source code.  And you must show them these terms so they
know their rights.

  Developers that use the GNU GPL protect your rights with two steps:
(1) assert copyright on the software, and (2) offer you this License
giving you legal permission to copy, distribute and/or modify it.

  For the developers' and authors' protection, the GPL clearly explains
that there is no warranty for this free software.  For both users' and
authors' sake, the GPL requires that modified versions be marked as
changed, so that their problems will not be attributed erroneously to
authors of previous versions.

  Some devices are designed to deny users access to install or run
modified versions of the software inside them, although the manufacturer
can do so.  This is fundamentally incompatible with the aim of
protecting users' freedom to change the software.  The systematic
pattern of such abuse occurs in the area of products for individuals to
use, which is precisely where it is most unacceptable.  Therefore, we
have designed this version of the GPL to prohibit the practice for those
products.  If such problems arise substantially in other domains, we
stand ready to extend this provision to those domains in future versions
of the GPL, as needed to protect the freedom of users.

  Finally, every program is threatened constantly by software patents.
States should not allow patents to restrict development and use of
software on general-purpose computers, but in those that do, we wish to
avoid the special danger that patents applied to a free program could
make it effectively proprietary.  To prevent this, the GPL assures that
patents cannot be used to render the program non-free.

  The precise terms and conditions for copying, distribution and
modification follow.

                       TERMS AND CONDITIONS

  0. Definitions.

  "This License" refers to version 3 of the GNU General Public License.

  "Copyright" also means copyright-like laws that apply to other kinds of
works, such as semiconductor masks.

  "The Program" refers to any copyrightable work licensed under this
License.  Each licensee is addressed as "you".  "Licensees" and
"recipients" may be individuals or organizations.

  To "modify" a work means to copy from or adapt all or part of the work
in a fashion requiring copyright permission, other than the making of an
exact copy.  The resulting work is called a "modified version" of the
earlier work or a work "based on" the earlier work.

  A "covered work" means either the unmodified Program or a work based
on the Program.

  To "propagate" a work means to do anything with it that, without
permission, would make you directly or secondarily liable for
infringement under applicable copyright law, except executing it on a
computer or modifying a private copy.  Propagation includes copying,
distribution (with or without modification), making available to the
public, and in some countries other activities as well.

  To "convey" a work means any kind of propagation that enables other
parties to make or receive copies.  Mere interaction with a user through
a computer network, with no transfer of a copy, is not conveying.

  An interactive user interface displays "Appropriate Legal Notices"
to the extent that it includes a convenient and prominently visible
feature that (1) displays an appropriate copyright notice, and (2)
tells the user that there is no warranty for the work (except to the
extent that warranties are provided), that licensees may convey the
work under this License, and how to view a copy of this License.  If
the interface presents a list of user commands or options, such as a
menu, a prominent item in the list meets this criterion.

  1. Source Code.

  The "source code" for a work means the preferred form of the work
for making modifications to it.  "Object code" means any non-source
form of a work.

  A "Standard Interface" means an interface that either is an official
standard defined by a recognized standards body, or, in the case of
interfaces specified for a particular programming language, one that
is widely used among developers working in that language.

  The "System Libraries" of an executable work include anything, other
than the work as a whole, that (a) is included in the normal form of
packaging a Major Component, but which is not part of that Major
Component, and (b) serves only to enable use of the work with that
Major Component, or to implement a Standard Interface for which an
implementation is available to the public in source code form.  A
"Major Component", in this context, means a major essential component
(kernel, window system, and so on) of the specific operating system
(if any) on which the executable work runs, or a compiler used to
produce the work, or an object code interpreter used to run it.

  The "Corresponding Source" for a work in object code form means all
the source code needed to generate, install, and (for an executable
work) run the object code and to modify the work, including scripts to
control those activities.  However, it does not include the work's
System Libraries, or general-purpose tools or generally available free
programs which are used unmodified in performing those activities but
which are not part of the work.  For example, Corresponding Source
includes interface definition files associated with source files for
the work, and the source code for shared libraries and dynamically
linked subprograms that the work is specifically designed to require,
such as by intimate data communication or control flow between those
subprograms and other parts of the work.

  The Corresponding Source need not include anything that users
can regenerate automatically from other parts of the Corresponding
Source.

  The Corresponding Source for a work in source code form is that
same work.

  2. Basic Permissions.

  All rights granted under this License are granted for the term of
copyright on the Program, and are irrevocable provided the stated
conditions are met.  This License explicitly affirms your unlimited
permission to run the unmodified Program.  The output from running a
covered work is covered by this License only if the output, given its
content, constitutes a covered work.  This License acknowledges your
rights of fair use or other equivalent, as provided by copyright law.

  You may make, run and propagate covered works that you do not
convey, without conditions so long as your license otherwise remains
in force.  You may convey covered works to others for the sole purpose
of having them make modifications exclusively for you, or provide you
with facilities for running those works, provided that you comply with
the terms of this License in conveying all material for which you do
not control copyright.  Those thus making or running the covered works
for you must do so exclusively on your behalf, under your direction
and control, on terms that prohibit them from making any copies of
your copyrighted material outside their relationship with you.

  Conveying under any other circumstances is permitted solely under
the conditions stated below.  Sublicensing is not allowed; section 10
makes it unnecessary.

  3. Protecting Users' Legal Rights From Anti-Circumvention Law.

  No covered work shall be deemed part of an effective technological
measure under any applicable law fulfilling obligations under article
11 of the WIPO copyright treaty adopted on 20 December 1996, or
similar laws prohibiting or restricting circumvention of such
measures.

  When you convey a covered work, you waive any legal power to forbid
circumvention of technological measures to the extent such circumvention
is effected by exercising rights under this License with respect to
the covered work, and you disclaim any intention to limit operation or
modification of the work as a means of enforcing, against the work's
users, your or third parties' legal rights to forbid circumvention of
technological measures.

  4. Conveying Verbatim Copies.

  You may convey verbatim copies of the Program's source code as you
receive it, in any medium, provided that you conspicuously and
appropriately publish on each copy an appropriate copyright notice;
keep intact all notices stating that this License and any
non-permissive terms added in accord with section 7 apply to the code;
keep intact all notices of the absence of any warranty; and give all
recipients a copy of this License along with the Program.

  You may charge any price or no price for each copy that you convey,
and you may offer support or warranty protection for a fee.

  5. Conveying Modified Source Versions.

  You may convey a work based on the Program, or the modifications to
produce it from the Program, in the form of source code under the
terms of section 4, provided that you also meet all of these conditions:

    a) The work must carry prominent notices stating that you modified
    it, and giving a relevant date.

    b) The work must carry prominent notices stating that it is
    released under this License and any conditions added under section
    7.  This requirement modifies the requirement in section 4 to
    "keep intact all notices".

    c) You must license the entire work, as a whole, under this
    License to anyone who comes into possession of a copy.  This
    License will therefore apply, along with any applicable section 7
    additional terms, to the whole of the work, and all its parts,
    regardless of how they are packaged.  This License gives no
    permission to license the work in any other way, but it does not
    invalidate such permission if you have separately received it.

    d) If the work has interactive user interfaces, each must display
    Appropriate Legal Notices; however, if the Program has interactive
    interfaces that do not display Appropriate Legal Notices, your
    work need not make them do so.

  A compilation of a covered work with other separate and independent
works, which are not by their nature extensions of the covered work,
and which are not combined with it such as to form a larger program,
in or on a volume of a storage or distribution medium, is called an
"aggregate" if the compilation and its resulting copyright are not
used to limit the access or legal rights of the compilation's users
beyond what the individual works permit.  Inclusion of a covered work
in an aggregate does not cause this License to apply to the other
parts of the aggregate.

  6. Conveying Non-Source Forms.

  You may convey a covered work in object code form under the terms
of sections 4 and 5, provided that you also convey the
machine-readable Corresponding Source under the terms of this License,
in one of these ways:

    a) Convey the object code in, or embodied in, a physical product
    (including a physical distribution medium), accompanied by the
    Corresponding Source fixed on a durable physical medium
    customarily used for software interchange.

    b) Convey the object code in, or embodied in, a physical product
    (including a physical distribution medium), accompanied by a
    written offer, valid for at least three years and valid for as
    long as you offer spare parts or customer support for that product
    model, to give anyone who possesses the object code either (1) a
    copy of the Corresponding Source for all the software in the
    product that is covered by this License, on a durable physical
    medium customarily used for software interchange, for a price no
    more than your reasonable cost of physically performing this
    conveying of source, or (2) access to copy the
    Corresponding Source from a network server at no charge.

    c) Convey individual copies of the object code with a copy of the
    written offer to provide the Corresponding Source.  This
    alternative is allowed only occasionally and noncommercially, and
    only if you received the object code with such an offer, in accord
    with subsection 6b.

    d) Convey the object code by offering access from a designated
    place (gratis or for a charge), and offer equivalent access to the
    Corresponding Source in the same way through the same place at no
    further charge.  You need not require recipients to copy the
    Corresponding Source along with the object code.  If the place to
    copy the object code is a network server, the Corresponding Source
    may be on a different server (operated by you or a third party)
    that supports equivalent copying facilities, provided you maintain
    clear directions next to the object code saying where to find the
    Corresponding Source.  Regardless of what server hosts the
    Corresponding Source, you remain obligated to ensure that it is
    available for as long as needed to satisfy these requirements.

    e) Convey the object code using peer-to-peer transmission, provided
    you inform other peers where the object code and Corresponding
    Source of the work are being offered to the general public at no
    charge under subsection 6d.

  A separable portion of the object code, whose source code is excluded
from the Corresponding Source as a System Library, need not be
included in conveying the object code work.

  A "User Product" is either (1) a "consumer product", which means any
tangible personal property which is normally used for personal, family,
or household purposes, or (2) anything designed or sold for incorporation
into a dwelling.  In determining whether a product is a consumer product,
doubtful cases shall be resolved in favor of coverage.  For a particular
product received by a particular user, "normally used" refers to a
typical or common use of that class of product, regardless of the status
of the particular user or of the way in which the particular user
actually uses, or expects or is expected to use, the product.  A product
is a consumer product regardless of whether the product has substantial
commercial, industrial or non-consumer uses, unless such uses represent
the only significant mode of use of the product.

  "Installation Information" for a User Product means any methods,
procedures, authorization keys, or other information required to install
and execute modified versions of a covered work in that User Product from
a modified version of its Corresponding Source.  The information must
suffice to ensure that the continued functioning of the modified object
code is in no case prevented or interfered with solely because
modification has been made.

  If you convey an object code work under this section in, or with, or
specifically for use in, a User Product, and the conveying occurs as
part of a transaction in which the right of possession and use of the
User Product is transferred to the recipient in perpetuity or for a
fixed term (regardless of how the transaction is characterized), the
Corresponding Source conveyed under this section must be accompanied
by the Installation Information.  But this requirement does not apply
if neither you nor any third party retains the ability to install
modified object code on the User Product (for example, the work has
been installed in ROM).

  The requirement to provide Installation Information does not include a
requirement to continue to provide support service, warranty, or updates
for a work that has been modified or installed by the recipient, or for
the User Product in which it has been modified or installed.  Access to a
network may be denied when the modification itself materially and
adversely affects the operation of the network or violates the rules and
protocols for communication across the network.

  Corresponding Source conveyed, and Installation Information provided,
in accord with this section must be in a format that is publicly
documented (and with an implementation available to the public in
source code form), and must require no special password or key for
unpacking, reading or copying.

  7. Additional Terms.

  "Additional permissions" are terms that supplement the terms of this
License by making exceptions from one or more of its conditions.
Additional permissions that are applicable to the entire Program shall
be treated as though they were included in this License, to the extent
that they are valid under applicable law.  If additional permissions
apply only to part of the Program, that part may be used separately
under those permissions, but the entire Program remains governed by
this License without regard to the additional permissions.

  When you convey a copy of a covered work, you may at your option
remove any additional permissions from that copy, or from any part of
it.  (Additional permissions may be written to require their own
removal in certain cases when you modify the work.)  You may place
additional permissions on material, added by you to a covered work,
for which you have or can give appropriate copyright permission.

  Notwithstanding any other provision of this License, for material you
add to a covered work, you may (if authorized by the copyright holders of
that material) supplement the terms of this License with terms:

    a) Disclaiming warranty or limiting liability differently from the
    terms of sections 15 and 16 of this License; or

    b) Requiring preservation of specified reasonable legal notices or
    author attributions in that material or in the Appropriate Legal
    Notices displayed by works containing it; or

    c) Prohibiting misrepresentation of the origin of that material, or
    requiring that modified versions of such material be marked in
    reasonable ways as different from the original version; or

    d) Limiting the use for publicity purposes of names of licensors or
    authors of the material; or

    e) Declining to grant rights under trademark law for use of some
    trade names, trademarks, or service marks; or

    f) Requiring indemnification of licensors and authors of that
    material by anyone who conveys the material (or modified versions of
    it) with contractual assumptions of liability to the recipient, for
    any liability that these contractual assumptions directly impose on
    those licensors and authors.

  All other non-permissive additional terms are considered "further
restrictions" within the meaning of section 10.  If the Program as you
received it, or any part of it, contains a notice stating that it is
governed by this License along with a term that is a further
restriction, you may remove that term.  If a license document contains
a further restriction but permits relicensing or conveying under this
License, you may add to a covered work material governed by the terms
of that license document, provided that the further restriction does
not survive such relicensing or conveying.

  If you add terms to a covered work in accord with this section, you
must place, in the relevant source files, a statement of the
additional terms that apply to those files, or a notice indicating
where to find the applicable terms.

  Additional terms, permissive or non-permissive, may be stated in the
form of a separately written license, or stated as exceptions;
the above requirements apply either way.

  8. Termination.

  You may not propagate or modify a covered work except as expressly
provided under this License.  Any attempt otherwise to propagate or
modify it is void, and will automatically terminate your rights under
this License (including any patent licenses granted under the third
paragraph of section 11).

  However, if you cease all violation of this License, then your
license from a particular copyright holder is reinstated (a)
provisionally, unless and until the copyright holder explicitly and
finally terminates your license, and (b) permanently, if the copyright
holder fails to notify you of the violation by some reasonable means
prior to 60 days after the cessation.

  Moreover, your license from a particular copyright holder is
reinstated permanently if the copyright holder notifies you of the
violation by some reasonable means, this is the first time you have
received notice of violation of this License (for any work) from that
copyright holder, and you cure the violation prior to 30 days after
your receipt of the notice.

  Termination of your rights under this section does not terminate the
licenses of parties who have received copies or rights from you under
this License.  If your rights have been terminated and not permanently
reinstated, you do not qualify to receive new licenses for the same
material under section 10.

  9. Acceptance Not Required for Having Copies.

  You are not required to accept this License in order to receive or
run a copy of the Program.  Ancillary propagation of a covered work
occurring solely as a consequence of using peer-to-peer transmission
to receive a copy likewise does not require acceptance.  However,
nothing other than this License grants you permission to propagate or
modify any covered work.  These actions infringe copyright if you do
not accept this License.  Therefore, by modifying or propagating a
covered work, you indicate your acceptance of this License to do so.

  10. Automatic Licensing of Downstream Recipients.

  Each time you convey a covered work, the recipient automatically
receives a license from the original licensors, to run, modify and
propagate that work, subject to this License.  You are not responsible
for enforcing compliance by third parties with this License.

  An "entity transaction" is a transaction transferring control of an
organization, or substantially all assets of one, or subdividing an
organization, or merging organizations.  If propagation of a covered
work results from an entity transaction, each party to that
transaction who receives a copy of the work also receives whatever
licenses to the work the party's predecessor in interest had or could
give under the previous paragraph, plus a right to possession of the
Corresponding Source of the work from the predecessor in interest, if
the predecessor has it or can get it with reasonable efforts.

  You may not impose any further restrictions on the exercise of the
rights granted or affirmed under this License.  For example, you may
not impose a license fee, royalty, or other charge for exercise of
rights granted under this License, and you may not initiate litigation
(including a cross-claim or counterclaim in a lawsuit) alleging that
any patent claim is infringed by making, using, selling, offering for
sale, or importing the Program or any portion of it.

  11. Patents.

  A "contributor" is a copyright holder who authorizes use under this
License of the Program or a work on which the Program is based.  The
work thus licensed is called the contributor's "contributor version".

  A contributor's "essential patent claims" are all patent claims
owned or controlled by the contributor, whether already acquired or
hereafter acquired, that would be infringed by some manner, permitted
by this License, of making, using, or selling its contributor version,
but do not include claims that would be infringed only as a
consequence of further modification of the contributor version.  For
purposes of this definition, "control" includes the right to grant
patent sublicenses in a manner consistent with the requirements of
this License.

  Each contributor grants you a non-exclusive, worldwide, royalty-free
patent license under the contributor's essential patent claims, to
make, use, sell, offer for sale, import and otherwise run, modify and
propagate the contents of its contributor version.

  In the following three paragraphs, a "patent license" is any express
agreement or commitment, however denominated, not to enforce a patent
(such as an express permission to practice a patent or covenant not to
sue for patent infringement).  To "grant" such a patent license to a
party means to make such an agreement or commitment not to enforce a
patent against the party.

  If you convey a covered work, knowingly relying on a patent license,
and the Corresponding Source of the work is not available for anyone
to copy, free of charge and under the terms of this License, through a
publicly available network server or other readily accessible means,
then you must either (1) cause the Corresponding Source to be so
available, or (2) arrange to deprive yourself of the benefit of the
patent license for this particular work, or (3) arrange, in a manner
consistent with the requirements of this License, to extend the patent
license to downstream recipients.  "Knowingly relying" means you have
actual knowledge that, but for the patent license, your conveying the
covered work in a country, or your recipient's use of the covered work
in a country, would infringe one or more identifiable patents in that
country that you have reason to believe are valid.

  If, pursuant to or in connection with a single transaction or
arrangement, you convey, or propagate by procuring conveyance of, a
covered work, and grant a patent license to some of the parties
receiving the covered work authorizing them to use, propagate, modify
or convey a specific copy of the covered work, then the patent license
you grant is automatically extended to all recipients of the covered
work and works based on it.

  A patent license is "discriminatory" if it does not include within
the scope of its coverage, prohibits the exercise of, or is
conditioned on the non-exercise of one or more of the rights that are
specifically granted under this License.  You may not convey a covered
work if you are a party to an arrangement with a third party that is
in the business of distributing software, under which you make payment
to the third party based on the extent of your activity of conveying
the work, and under which the third party grants, to any of the
parties who would receive the covered work from you, a discriminatory
patent license (a) in connection with copies of the covered work
conveyed by you (or copies made from those copies), or (b) primarily
for and in connection with specific products or compilations that
contain the covered work, unless you entered into that arrangement,
or that patent license was granted, prior to 28 March 2007.

  Nothing in this License shall be construed as excluding or limiting
any implied license or other defenses to infringement that may
otherwise be available to you under applicable patent law.

  12. No Surrender of Others' Freedom.

  If conditions are imposed on you (whether by court order, agreement or
otherwise) that contradict the conditions of this License, they do not
excuse you from the conditions of this License.  If you cannot convey a
covered work so as to satisfy simultaneously your obligations under this
License and any other pertinent obligations, then as a consequence you may
not convey it at all.  For example, if you agree to terms that obligate you
to collect a royalty for further conveying from those to whom you convey
the Program, the only way you could satisfy both those terms and this
License would be to refrain entirely from conveying the Program.

  13. Use with the GNU Affero General Public License.

  Notwithstanding any other provision of this License, you have
permission to link or combine any covered work with a work licensed
under version 3 of the GNU Affero General Public License into a single
combined work, and to convey the resulting work.  The terms of this
License will continue to apply to the part which is the covered work,
but the special requirements of the GNU Affero General Public License,
section 13, concerning interaction through a network will apply to the
combination as such.

  14. Revised Versions of this License.

  The Free Software Foundation may publish revised and/or new versions of
the GNU General Public License from time to time.  Such new versions will
be similar in spirit to the present version, but may differ in detail to
address new problems or concerns.

  Each version is given a distinguishing version number.  If the
Program specifies that a certain numbered version of the GNU General
Public License "or any later version" applies to it, you have the
option of following the terms and conditions either of that numbered
version or of any later version published by the Free Software
Foundation.  If the Program does not specify a version number of the
GNU General Public License, you may choose any version ever published
by the Free Software Foundation.

  If the Program specifies that a proxy can decide which future
versions of the GNU General Public License can be used, that proxy's
public statement of acceptance of a version permanently authorizes you
to choose that version for the Program.

  Later license versions may give you additional or different
permissions.  However, no additional obligations are imposed on any
author or copyright holder as a result of your choosing to follow a
later version.

  15. Disclaimer of Warranty.

  THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY
OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

  16. Limitation of Liability.

  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS
THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY
GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE
USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF
DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD
PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),
EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGES.

  17. Interpretation of Sections 15 and 16.

  If the disclaimer of warranty and limitation of liability provided
above cannot be given local legal effect according to their terms,
reviewing courts shall apply local law that most closely approximates
an absolute waiver of all civil liability in connection with the
Program, unless a warranty or assumption of liability accompanies a
copy of the Program in return for a fee.

                     END OF TERMS AND CONDITIONS

            How to Apply These Terms to Your New Programs

  If you develop a new program, and you want it to be of the greatest
possible use to the public, the best way to achieve this is to make it
free software which everyone can redistribute and change under these terms.

  To do so, attach the following notices to the program.  It is safest
to attach them to the start of each source file to most effectively
state the exclusion of warranty; and each file should have at least
the "copyright" line and a pointer to where the full notice is found.

    {one line to give the program's name and a brief idea of what it does.}
    Copyright (C) {year}  {name of author}

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Also add information on how to contact you by electronic and paper mail.

  If the program does terminal interaction, make it output a short
notice like this when it starts in an interactive mode:

    {project}  Copyright (C) {year}  {fullname}
    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type `show c' for details.

The hypothetical commands `show w' and `show c' should show the appropriate
parts of the General Public License.  Of course, your program's commands
might be different; for a GUI interface, you would use an "about box".

  You should also get your employer (if you work as a programmer) or school,
if any, to sign a "copyright disclaimer" for the program, if necessary.
For more information on this, and how to apply and follow the GNU GPL, see
<http://www.gnu.org/licenses/>.

  The GNU General Public License does not permit incorporating your program
into proprietary programs.  If your program is a subroutine library, you
may consider it more useful to permit linking proprietary applications with
the library.  If this is what you want to do, use the GNU Lesser General
Public License instead of this License.  But first, please read
<http://www.gnu.org/philosophy/why-not-lgpl.html>.
      working-directory: ${{ 


/*
**        Description        :        Set of API to initialise and use FIFO buffers
*/



/*
**        @brief                                Function to read data from the buffer of a FIFO object.
**        @fifo(PARAM_IN)                        Pointer to the FIFO object.
**        @data_read(PARAM_IN)                The pointer to the array to which the read data will be copied.
**        @bytes_count(PARAM_IN)                The number of bytes to be read from the buffer of the FIFO object.
*/
uint16_t Fifo_Read(struct FIFO_struct *fifo, uint8_t *data_read, const uint16_t bytes_count)
{
        /*        For operations        */
        int16_t for_var;

        /*        For updation of buffer read pointer        */
        int16_t next;

        /*        To return the number of bytes not read from the Fifo buffer        */
        uint16_t bytesToRead = bytes_count;


        /*        Dont do anything and return, if the buffer points to NULL(i.e if the fifo object was not initialised)        */
        if (!fifo->buffer)
                return bytesToRead;

        /*        Perform the read operations read one byte each time        */
        for (for_var = 0; for_var < bytes_count; for_var ++)
        {
                if (Fifo_bufferIsEmpty(fifo))
                        return bytesToRead;

                /*        Read one byte from the Fifo buffer at the read position        */
                *(data_read + (bytes_count - bytesToRead)) = fifo->buffer[fifo->head];

                /*        Update the position where the next read has to be performed        */
                next  = (fifo->head+1)%fifo->size;

                /*        If the buffer has emptied after reading the byte        */
                if (fifo->head == fifo->tail)
                {
                        /*        Flush the Fifo buffer        */
                        Fifo_Flush(fifo);
                }
                /*        If the fifo buffer has not emptied after reading the byte        */
                else
                {
                        /*        Update the next read position in the Fifo buffer        */
                        fifo->head = next;
                }


                /*        Update the number of bytes remaining to be read        */
                --bytesToRead;

        }

        return bytesToRead;
}



/*
**        @brief                                                                                Function to write data into the buffer of a FIFO object.
**        @fifo(PARAM_IN)                                                Pointer to the FIFO object.
**        @data_to_write(PARAM_IN)        The pointer to the array containing the data to be written to the buffer
**        @bytes_count(PARAM_IN)                The number of bytes to be written the buffer of the FIFO object.
*/
uint16_t Fifo_Write(struct FIFO_struct *fifo, const uint8_t *data_to_write, const uint16_t bytes_count)
{
        /*        For operations        */
        int16_t for_var;

        /*        For updation of buffer write pointer        */
        int16_t next;

        /*        To return the number of bytes not written to the Fifo buffer        */
        uint16_t bytesToWrite = bytes_count;


        /*        Dont do anything and return, if the buffer points to NULL(i.e if the fifo object was not initialised)        */
        if (!fifo->buffer)
                return bytesToWrite;

        /*        Perform the write operations to write one byte each time        */
        for (for_var = 0; for_var < bytes_count; for_var ++)
        {
                /*        Position where the present write has to be performed        */
                next  = (fifo->tail+1)%fifo->size;

                if (Fifo_bufferIsFull(fifo))
                {
                        return bytesToWrite;
                }

                /*        Update the write position in the Fifo buffer        */
                fifo->tail = next;

                /*        Write one byte into the Fifo buffer at the updated write position        */
                fifo->buffer[next] = *(data_to_write + (bytes_count - bytesToWrite));

                /*        Update the number of bytes remaining to be written        */
                --bytesToWrite;

                /*        Indicate that the Fifo buffer is not empty        */
                if (fifo->head == -1)
                        fifo->head = 0;
        }

        return bytesToWrite;
}


/*
**        @brief                                                Function to check if the buffer of a FIFO object if empty.
**        @pFifo(PARAM_IN)                                Pointer to the FIFO object(structure).
*/
bool Fifo_bufferIsEmpty(const struct FIFO_struct *pFifo)
{
        if (pFifo->head < 0)
                return true;                        /*        Buffer is empty        */
        else
                return false;                        /*        Buffer is not empty        */
}


/*
**        @brief                                                Function to check if the buffer of a FIFO object if full.
**        @pFifo(PARAM_IN)                                Pointer to the FIFO object(structure).
*/
bool Fifo_bufferIsFull(const struct FIFO_struct *pFifo)
{
        int16_t next;

        next = (pFifo->tail+1)%pFifo->size;

        if (next == pFifo->head)
                return true;                        /*        Buffer is Full        */
        else
                return false;                        /*        Buffer is not Full        */
}


/*
**        @brief                                                Function to prepare a FIFO object for functioning.
**        @pFifo(PARAM_IN)                                Pointer to the FIFO object(structure).
**        @size(PARAM_IN)                                        The size of the buffer of the FIFO object.
*/
void Fifo_Init(struct FIFO_struct *pFifo, const uint8_t size)
{
        pFifo->head = -1;                                                                                                                                                        
        pFifo->tail = -1;
        pFifo->size = size;
        pFifo->buffer = malloc(size*(sizeof(uint8_t)));        /*        Dynamic allocation for buffer        */
}


/*
**        @brief                                                Function to discard a FIFO object.
**        @pFifo(PARAM_IN)                                Pointer to the FIFO object(structure).
**        @size(PARAM_IN)                                        The size of the buffer of the FIFO object.
*/
void Fifo_DeInit(struct FIFO_struct *pFifo)
{
        pFifo->head = -1;                                                                                                                                                        
        pFifo->tail = -1;
        pFifo->size = 0;                                /*        Indicate the size of the FIFO buffer to be zero        */
        free(pFifo->buffer);                                /*        Free the memory reserved by the FIFO buffer        */
        pFifo->buffer = NULL;                                /*        Null the buffer pointer        */
}


/*
**        @brief                                Function to put read and write pointers of Fifo buffer to their initial states.
**        @pFifo(PARAM_IN)                Pointer to the FIFO object(structure).
*/
void Fifo_Flush(struct FIFO_struct *pFifo)
{
        pFifo->head = -1;                                                                                                                                                        
        pFifo->tail = -1;
} }}
      # Execute tests defined by the CMake configuration. Note that --build-config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest --build-config ${{ import requests
from email.utils import parseaddr

def Check(account):
        headers = {
                'User-Agent':'Pwnage-Checker-For-Python',
                "Accept": "application/vnd.haveibeenpwned.v2+json"
        }
        r = requests.get("https://haveibeenpwned.com/api/v2/breachedaccount/{0}".format(account),headers=headers)
        names = {}
        if not r.status_code == 200:
                return False
        json = r.json()
        for i in range(len(json)):
                names[json[i]["Name"]] = json[i]["BreachDate"]
        return names

a = input("-> ")
liste = []
a = a.replace("\n"," ")
for i in a.split(","):
        r = parseaddr(i)
        if r[1] != "":
                liste.append(r)
for n in liste:
        print("Checking ", str(n[0]))
        che = Check(str(n[1]))
        if che != False:
                print(" > Results for ",str(n[1]),": \n",che,"\n")
 }}
