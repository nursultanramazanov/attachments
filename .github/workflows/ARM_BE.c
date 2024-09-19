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
          - os:
#include "MoveCommand.h"

static const std::string _name("move");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e4\n"
            "Description: Moves the selected piece to the specified square if legal.";

MoveCommand::MoveCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result MoveCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.move({row, column});
    return {error, false};
}
            c_compiler: #pragma once

#include "../Command.h"

// Moves the selected piece to a new square.
// Params:
//      1. string of format {char}{int} representing a
//          field of the chessboard.
class MoveCommand : public Command
{
public:
    MoveCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
          - os: #include "QuitCommand.h"

static const std::string _name = "quit";
static const std::string _description =
    "Arguments: [None]\n"
    "Description: Quits the game.";

QuitCommand::QuitCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result QuitCommand::apply(const std::vector<std::string> &params)
{
    bool error = !params.empty();
    bool quit = true;
    return {error, quit};
}
            c_compiler: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};

    steps: #include "SelectCommand.h"

static const std::string _name("select");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e2\n"
            "Description: Selects the specified square.";

SelectCommand::SelectCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result SelectCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.select({row, column});
    return {error, false};
}

    - uses: #pragma once

#include "../Command.h"

// Selects a square.
// Params:
//     1. row
//     2. column
class SelectCommand : public Command
{
public:
    SelectCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};

    - name: #include "Command.h"

Command::Command(Chess &chess, std::string name, std::string description)
    : chess(chess), name(name), description(description) {}

const std::string &Command::get_name() const
{
    return this->name;
}

const std::string &Command::get_description() const
{
    return this->description;
}

Chess &Command::get_chess()
{
    return this->chess;
}

      # Turn repeated input strings (such as the build output directory) into step outputs. These step outputs can be used throughout the workflow file.
      id: #pragma once

#include <string>
#include <vector>

#include "Result.h"

#include "../model/Chess.h"

class Command
{
public:
    Command(Chess &chess, std::string name, std::string description);
    virtual ~Command() = default;

    const std::string &get_name() const;
    const std::string &get_description() const;

    virtual Result apply(const std::vector<std::string> &params) = 0;

protected:
    Chess &get_chess();

private:
    Chess &chess;
    std::string name;
    std::string description;
};
      shell: #pragma once

struct Result
{
    bool error;
    bool quit;
};

      run: [alias]
build-man = "run --package xtask-build-man --"
stale-label = "run --package xtask-stale-label --"
bump-check = "run --package xtask-bump-check --"
lint-docs = "run --package xtask-lint-docs --"

[env]
# HACK: Until this is stabilized, `snapbox`s polyfill could get confused
# inside of the rust-lang/rust repo because it looks for the furthest-away `Cargo.toml`
CARGO_RUSTC_CURRENT_DIR = { value = "", relative = true }
        

    - name: - config:
    id: c_c++
    name: C / C++
    icons_html: >
      <i class="devicon-cplusplus-plain"></i>
      <i class="devicon-c-plain"></i>
  projects:
    - url: https://github.com/spotify/annoy
    - url: https://github.com/spotify/sparkey
    - url: https://github.com/spotify/spotify-json
    - url: https://github.com/spotify/pedalboard
    - url: https://github.com/spotify/voyager
- config:
    id: java
    name: Java
    icons_html: >
      <i class="devicon-java-plain-wordmark"></i>
  projects:
    - url: https://github.com/spotify/android-auth
    - url: https://github.com/spotify/android-sdk
    - url: https://github.com/spotify/completable-futures
    - url: https://github.com/spotify/dbeam
    - url: https://github.com/spotify/folsom
    - url: https://github.com/spotify/futures-extra
    - url: https://github.com/spotify/missinglink
    - url: https://github.com/spotify/mobius
    - url: https://github.com/spotify/zoltar
    - url: https://github.com/spotify/github-java-client
    - url: https://github.com/spotify/dataenum
    - url: https://github.com/spotify/futuristic-feline
    - url: https://github.com/spotify/logging-java
    - url: https://github.com/spotify/java-hamcrest
    - url: https://github.com/spotify/fmt-maven-plugin
    - url: https://github.com/spotify/dns-java
    - url: https://github.com/spotify/semantic-metrics
    - url: https://github.com/spotify/Vandelay
- config:
    id: javascript
    name: JavaScript
    icons_html: >
      <i class="devicon-javascript-plain"></i>
  projects:
    - url: https://github.com/backstage/backstage
    - url: https://github.com/spotify/reactochart
    - url: https://github.com/spotify/web-scripts
    - url: https://github.com/spotify/should-up
    - url: https://github.com/spotify/spotify-web-api-ts-sdk
    - url: https://github.com/spotify/confidence-openfeature-provider-js
    - url: https://github.com/spotify/lighthouse-audit-service
    - url: https://github.com/spotify/basic-pitch-ts
    - url: https://github.com/spotify/soa-reference-integration
    - url: https://github.com/spotify/web-api-examples
- config:
    id: kotlin
    name: Kotlin
    icons_html: >
      <i class="devicon-kotlin-plain"></i>
  projects:
    - url: https://github.com/spotify/ruler
    - url: https://github.com/spotify/confidence-openfeature-provider-kotlin
- config:
    id: objective-c
    name: Objective-C
    icons_html: >
      <i class="devicon-objectivec-plain"></i>
  projects:
    - url: https://github.com/spotify/ios-style
    - url: https://github.com/spotify/ios-sdk
    - url: https://github.com/spotify/ios-auth
    - url: https://github.com/spotify/SPTDataLoader
    - url: https://github.com/spotify/SPTPersistentCache
- config:
    id: python
    name: Python
    icons_html: >
      <i class="devicon-python-plain alt-colored"></i>
  projects:
    - url: https://github.com/spotify/annoy
    - url: https://github.com/spotify/basic-pitch
    - url: https://github.com/spotify/confidence
    - url: https://github.com/spotify/realbook
    - url: https://github.com/spotify/ulogger
    - url: https://github.com/spotify/cstar
    - url: https://github.com/spotify/dh-virtualenv
    - url: https://github.com/spotify/luigi
    - url: https://github.com/spotify/pythonflow
    - url: https://github.com/spotify/spotify-tensorflow
    - url: https://github.com/spotify/chartify
    - url: https://github.com/spotify/klio
    - url: https://github.com/spotify/pedalboard
    - url: https://github.com/spotify/postgresql-metrics
    - url: https://github.com/spotify/sparkey-python
- config:
    id: ruby
    name: Ruby
    icons_html: >
      <i class="devicon-ruby-plain"></i>
  projects:
    - url: https://github.com/spotify/homebrew-public
    - url: https://github.com/spotify/talos
- config:
    id: scala
    name: Scala
    icons_html: >
      <i class="devicon-scala-plain"></i>
  projects:
    - url: https://github.com/spotify/big-data-rosetta-code
    - url: https://github.com/spotify/featran
    - url: https://github.com/spotify/noether
    - url: https://github.com/spotify/ratatool
    - url: https://github.com/spotify/scio
    - url: https://github.com/spotify/magnolify
    - url: https://github.com/spotify/tfreader
    - url: https://github.com/spotify/scio-idea-plugin
    - url: https://github.com/spotify/gcs-tools
    - url: https://github.com/spotify/elitzur
    - url: https://github.com/spotify/scio.g8
- config:
    id: go
    name: Go
    icons_html: >
      <i class="devicon-go-original-wordmark"></i>
  projects:
    - url: https://github.com/spotify/flyte-flink-plugin
    - url: https://github.com/spotify/flink-on-k8s-operator
    - url: https://github.com/spotify/confidence-openfeature-provider-go
- config:
    id: swift
    name: Swift
    icons_html: >
      <i class="devicon-swift-plain"></i>
  projects:
    - url: https://github.com/spotify/Mobius.swift
    - url: https://github.com/spotify/XCRemoteCache
    - url: https://github.com/spotify/XCMetrics
    - url: https://github.com/spotify/confidence-openfeature-provider-swift
    - url: https://github.com/spotify/diffuser

- config:
    id: other
    name: Other
  projects:
    - url: https://github.com/spotify/git-test
    - url: https://github.com/spotify/cstar
    - url: https://github.com/spotify/dockerfile-mode
    - url: https://github.com/spotify/foss-root
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: - config:
    id: c_c++
    name: C / C++
    icons_html: |
      <i class="devicon-cplusplus-plain"></i> <i class="devicon-c-plain"></i>
  projects:
    - name: annoy
      description: Approximate Nearest Neighbors in C++/Python optimized for memory
        usage and loading/saving to disk
      stargazerCount: 13101
      forkCount: 1156
      url: https://github.com/spotify/annoy
      openIssues:
        totalCount: 55
      watchers:
        totalCount: 319
    - name: sparkey
      description: Simple constant key/value storage library, for read-heavy systems
        with infrequent large bulk inserts.
      stargazerCount: 1160
      forkCount: 80
      url: https://github.com/spotify/sparkey
      openIssues:
        totalCount: 5
      watchers:
        totalCount: 144
    - name: spotify-json
      description: Fast and nice to use C++ JSON library.
      stargazerCount: 194
      forkCount: 40
      url: https://github.com/spotify/spotify-json
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 20
    - name: pedalboard
      description: 🎛 🔊 A Python library for audio.
      stargazerCount: 5127
      forkCount: 259
      url: https://github.com/spotify/pedalboard
      openIssues:
        totalCount: 54
      watchers:
        totalCount: 55
    - name: voyager
      description: 🛰️ An approximate nearest-neighbor search library for Python and
        Java with a focus on ease of use, simplicity, and deployability.
      stargazerCount: 1271
      forkCount: 52
      url: https://github.com/spotify/voyager
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 13
- config:
    id: java
    name: Java
    icons_html: |
      <i class="devicon-java-plain-wordmark"></i>
  projects:
    - name: android-auth
      description: Spotify authentication and authorization for Android. Part of the
        Spotify Android SDK.
      stargazerCount: 143
      forkCount: 66
      url: https://github.com/spotify/android-auth
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 32
    - name: android-sdk
      description: Spotify SDK for Android
      stargazerCount: 459
      forkCount: 119
      url: https://github.com/spotify/android-sdk
      openIssues:
        totalCount: 153
      watchers:
        totalCount: 59
    - name: completable-futures
      description: Utilities for working with futures in Java 8
      stargazerCount: 389
      forkCount: 51
      url: https://github.com/spotify/completable-futures
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 101
    - name: dbeam
      description: DBeam exports SQL tables into Avro files using JDBC and Apache Beam
      stargazerCount: 189
      forkCount: 56
      url: https://github.com/spotify/dbeam
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 19
    - name: folsom
      description: An asynchronous memcache client for Java
      stargazerCount: 147
      forkCount: 67
      url: https://github.com/spotify/folsom
      openIssues:
        totalCount: 11
      watchers:
        totalCount: 94
    - name: futures-extra
      description: Java library for working with Guava futures
      stargazerCount: 137
      forkCount: 32
      url: https://github.com/spotify/futures-extra
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 25
    - name: missinglink
      description: Build time tool for detecting link problems in java projects
      stargazerCount: 144
      forkCount: 27
      url: https://github.com/spotify/missinglink
      openIssues:
        totalCount: 16
      watchers:
        totalCount: 96
    - name: mobius
      description: A functional reactive framework for managing state evolution and
        side-effects.
      stargazerCount: 1221
      forkCount: 97
      url: https://github.com/spotify/mobius
      openIssues:
        totalCount: 3
      watchers:
        totalCount: 49
    - name: zoltar
      description: Common library for serving TensorFlow, XGBoost and scikit-learn
        models in production.
      stargazerCount: 139
      forkCount: 33
      url: https://github.com/spotify/zoltar
      openIssues:
        totalCount: 17
      watchers:
        totalCount: 24
    - name: github-java-client
      description: A Java client to Github API
      stargazerCount: 129
      forkCount: 82
      url: https://github.com/spotify/github-java-client
      openIssues:
        totalCount: 16
      watchers:
        totalCount: 21
    - name: dataenum
      description: Algebraic data types in Java.
      stargazerCount: 164
      forkCount: 16
      url: https://github.com/spotify/dataenum
      openIssues:
        totalCount: 3
      watchers:
        totalCount: 16
    - name: futuristic-feline
      description: futuristic-feline is a library for detecting blocking Java futures
        at runtime
      stargazerCount: 11
      forkCount: 9
      url: https://github.com/spotify/futuristic-feline
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 15
    - name: logging-java
      description: Utility classes to set up logging in a Spotify compatible way
      stargazerCount: 22
      forkCount: 23
      url: https://github.com/spotify/logging-java
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 98
    - name: java-hamcrest
      description: Libraries that extend the Hamcrest matching library with useful matchers
      stargazerCount: 82
      forkCount: 16
      url: https://github.com/spotify/java-hamcrest
      openIssues:
        totalCount: 5
      watchers:
        totalCount: 20
    - name: fmt-maven-plugin
      description: Opinionated Maven Plugin that formats your Java code.
      stargazerCount: 326
      forkCount: 62
      url: https://github.com/spotify/fmt-maven-plugin
      openIssues:
        totalCount: 11
      watchers:
        totalCount: 12
    - name: dns-java
      description: DNS wrapper library that provides SRV lookup functionality
      stargazerCount: 205
      forkCount: 47
      url: https://github.com/spotify/dns-java
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 106
    - name: semantic-metrics
      description: Capturing meaningful metrics in your Java application
      stargazerCount: 66
      forkCount: 36
      url: https://github.com/spotify/semantic-metrics
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 20
    - name: Vandelay
      description: A small library for importing/exporting BigTable instance schemas
        and row data.
      stargazerCount: 10
      forkCount: 1
      url: https://github.com/spotify/Vandelay
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 9
- config:
    id: javascript
    name: JavaScript
    icons_html: |
      <i class="devicon-javascript-plain"></i>
  projects:
    - name: backstage
      description: Backstage is an open framework for building developer portals
      stargazerCount: 27552
      forkCount: 5840
      url: https://github.com/backstage/backstage
      openIssues:
        totalCount: 343
      watchers:
        totalCount: 223
    - name: reactochart
      description: ":chart_with_upwards_trend: React chart component library
        :chart_with_downwards_trend:"
      stargazerCount: 552
      forkCount: 54
      url: https://github.com/spotify/reactochart
      openIssues:
        totalCount: 8
      watchers:
        totalCount: 20
    - name: web-scripts
      description: "DEPRECATED: A collection of base configs and CLI wrappers used to
        speed up development @ Spotify."
      stargazerCount: 383
      forkCount: 62
      url: https://github.com/spotify/web-scripts
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 6
    - name: should-up
      description: Remove most of the "should" noise from your tests
      stargazerCount: 155
      forkCount: 23
      url: https://github.com/spotify/should-up
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 12
    - name: spotify-web-api-ts-sdk
      description: A Typescript SDK for the Spotify Web API with types for returned data.
      stargazerCount: 355
      forkCount: 69
      url: https://github.com/spotify/spotify-web-api-ts-sdk
      openIssues:
        totalCount: 29
      watchers:
        totalCount: 10
    - name: confidence-sdk-js
      description: null
      stargazerCount: 9
      forkCount: 1
      url: https://github.com/spotify/confidence-sdk-js
      openIssues:
        totalCount: 6
      watchers:
        totalCount: 10
    - name: lighthouse-audit-service
      description: null
      stargazerCount: 95
      forkCount: 30
      url: https://github.com/spotify/lighthouse-audit-service
      openIssues:
        totalCount: 17
      watchers:
        totalCount: 19
    - name: basic-pitch-ts
      description: A lightweight yet powerful audio-to-MIDI converter with pitch bend
        detection.
      stargazerCount: 228
      forkCount: 18
      url: https://github.com/spotify/basic-pitch-ts
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 20
    - name: soa-reference-integration
      description: SOA Reference Integration
      stargazerCount: 11
      forkCount: 2
      url: https://github.com/spotify/soa-reference-integration
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 9
    - name: web-api-examples
      description: Basic examples to authenticate and fetch data using the Spotify Web API
      stargazerCount: 1980
      forkCount: 1654
      url: https://github.com/spotify/web-api-examples
      openIssues:
        totalCount: 50
      watchers:
        totalCount: 68
- config:
    id: kotlin
    name: Kotlin
    icons_html: |
      <i class="devicon-kotlin-plain"></i>
  projects:
    - name: ruler
      description: Gradle plugin which helps you analyze the size of your Android apps.
      stargazerCount: 1129
      forkCount: 58
      url: https://github.com/spotify/ruler
      openIssues:
        totalCount: 13
      watchers:
        totalCount: 25
    - name: confidence-sdk-android
      description: null
      stargazerCount: 8
      forkCount: 1
      url: https://github.com/spotify/confidence-sdk-android
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 10
- config:
    id: objective-c
    name: Objective-C
    icons_html: |
      <i class="devicon-objectivec-plain"></i>
  projects:
    - name: ios-style
      description: Guidelines for iOS development in use at Spotify
      stargazerCount: 242
      forkCount: 36
      url: https://github.com/spotify/ios-style
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 130
    - name: ios-sdk
      description: Spotify SDK for iOS
      stargazerCount: 653
      forkCount: 186
      url: https://github.com/spotify/ios-sdk
      openIssues:
        totalCount: 205
      watchers:
        totalCount: 58
    - name: ios-auth
      description: Spotify authentication and authorization for iOS.
      stargazerCount: 10
      forkCount: 3
      url: https://github.com/spotify/ios-auth
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 4
    - name: SPTDataLoader
      description: The HTTP library used by the Spotify iOS client
      stargazerCount: 629
      forkCount: 74
      url: https://github.com/spotify/SPTDataLoader
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 114
    - name: SPTPersistentCache
      description: Everyone tries to implement a cache at some point in their iOS
        app’s lifecycle, and this is ours.
      stargazerCount: 1245
      forkCount: 78
      url: https://github.com/spotify/SPTPersistentCache
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 56
- config:
    id: python
    name: Python
    icons_html: |
      <i class="devicon-python-plain alt-colored"></i>
  projects:
    - name: annoy
      description: Approximate Nearest Neighbors in C++/Python optimized for memory
        usage and loading/saving to disk
      stargazerCount: 13101
      forkCount: 1156
      url: https://github.com/spotify/annoy
      openIssues:
        totalCount: 55
      watchers:
        totalCount: 319
    - name: basic-pitch
      description: A lightweight yet powerful audio-to-MIDI converter with pitch bend
        detection
      stargazerCount: 3338
      forkCount: 257
      url: https://github.com/spotify/basic-pitch
      openIssues:
        totalCount: 24
      watchers:
        totalCount: 49
    - name: confidence
      description: null
      stargazerCount: 255
      forkCount: 32
      url: https://github.com/spotify/confidence
      openIssues:
        totalCount: 8
      watchers:
        totalCount: 15
    - name: realbook
      description: Easier audio-based machine learning with TensorFlow.
      stargazerCount: 112
      forkCount: 5
      url: https://github.com/spotify/realbook
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 8
    - name: ulogger
      description: A simple micrologging framework
      stargazerCount: 15
      forkCount: 8
      url: https://github.com/spotify/ulogger
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 15
    - name: cstar
      description: Apache Cassandra cluster orchestration tool for the command line
      stargazerCount: 254
      forkCount: 37
      url: https://github.com/spotify/cstar
      openIssues:
        totalCount: 8
      watchers:
        totalCount: 22
    - name: dh-virtualenv
      description: Python virtualenvs in Debian packages
      stargazerCount: 1612
      forkCount: 186
      url: https://github.com/spotify/dh-virtualenv
      openIssues:
        totalCount: 35
      watchers:
        totalCount: 41
    - name: luigi
      description: "Luigi is a Python module that helps you build complex pipelines of
        batch jobs. It handles dependency resolution, workflow management,
        visualization etc. It also comes with Hadoop support built in. "
      stargazerCount: 17705
      forkCount: 2391
      url: https://github.com/spotify/luigi
      openIssues:
        totalCount: 105
      watchers:
        totalCount: 474
    - name: pythonflow
      description: ":snake: Dataflow programming for python."
      stargazerCount: 284
      forkCount: 48
      url: https://github.com/spotify/pythonflow
      openIssues:
        totalCount: 2
      watchers:
        totalCount: 74
    - name: spotify-tensorflow
      description: Provides Spotify-specific TensorFlow helpers
      stargazerCount: 124
      forkCount: 25
      url: https://github.com/spotify/spotify-tensorflow
      openIssues:
        totalCount: 12
      watchers:
        totalCount: 27
    - name: chartify
      description: Python library that makes it easy for data scientists to create charts.
      stargazerCount: 3519
      forkCount: 324
      url: https://github.com/spotify/chartify
      openIssues:
        totalCount: 43
      watchers:
        totalCount: 86
    - name: klio
      description: Smarter data pipelines for audio.
      stargazerCount: 835
      forkCount: 48
      url: https://github.com/spotify/klio
      openIssues:
        totalCount: 5
      watchers:
        totalCount: 20
    - name: pedalboard
      description: 🎛 🔊 A Python library for audio.
      stargazerCount: 5127
      forkCount: 259
      url: https://github.com/spotify/pedalboard
      openIssues:
        totalCount: 54
      watchers:
        totalCount: 55
    - name: postgresql-metrics
      description: Tool that extracts and provides metrics on your PostgreSQL database
      stargazerCount: 588
      forkCount: 51
      url: https://github.com/spotify/postgresql-metrics
      openIssues:
        totalCount: 2
      watchers:
        totalCount: 113
    - name: sparkey-python
      description: Python bindings for the sparkey key value store
      stargazerCount: 38
      forkCount: 7
      url: https://github.com/spotify/sparkey-python
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 10
- config:
    id: ruby
    name: Ruby
    icons_html: |
      <i class="devicon-ruby-plain"></i>
  projects:
    - name: homebrew-public
      description: Homebrew formula for open-source software developed by Spotify
      stargazerCount: 55
      forkCount: 27
      url: https://github.com/spotify/homebrew-public
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 36
    - name: talos
      description: Hiera secrets distribution over HTTP
      stargazerCount: 26
      forkCount: 6
      url: https://github.com/spotify/talos
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 14
- config:
    id: scala
    name: Scala
    icons_html: |
      <i class="devicon-scala-plain"></i>
  projects:
    - name: big-data-rosetta-code
      description: Code snippets for solving common big data problems in various
        platforms. Inspired by Rosetta Code
      stargazerCount: 290
      forkCount: 33
      url: https://github.com/spotify/big-data-rosetta-code
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 29
    - name: featran
      description: A Scala feature transformation library for data science and machine
        learning
      stargazerCount: 466
      forkCount: 68
      url: https://github.com/spotify/featran
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 29
    - name: noether
      description: Scala Aggregators used for ML Model metrics monitoring
      stargazerCount: 91
      forkCount: 16
      url: https://github.com/spotify/noether
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 22
    - name: ratatool
      description: A tool for data sampling, data generation, and data diffing
      stargazerCount: 341
      forkCount: 55
      url: https://github.com/spotify/ratatool
      openIssues:
        totalCount: 18
      watchers:
        totalCount: 27
    - name: scio
      description: A Scala API for Apache Beam and Google Cloud Dataflow.
      stargazerCount: 2550
      forkCount: 513
      url: https://github.com/spotify/scio
      openIssues:
        totalCount: 119
      watchers:
        totalCount: 117
    - name: magnolify
      description: A collection of Magnolia add-on modules
      stargazerCount: 165
      forkCount: 26
      url: https://github.com/spotify/magnolify
      openIssues:
        totalCount: 19
      watchers:
        totalCount: 21
    - name: tfreader
      description: TensorFlow TFRecord reader CLI tool
      stargazerCount: 61
      forkCount: 16
      url: https://github.com/spotify/tfreader
      openIssues:
        totalCount: 2
      watchers:
        totalCount: 18
    - name: scio-idea-plugin
      description: Scio IDEA plugin
      stargazerCount: 30
      forkCount: 18
      url: https://github.com/spotify/scio-idea-plugin
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 24
    - name: gcs-tools
      description: GCS support for avro-tools, parquet-tools and protobuf
      stargazerCount: 73
      forkCount: 15
      url: https://github.com/spotify/gcs-tools
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 17
    - name: elitzur
      description: null
      stargazerCount: 22
      forkCount: 20
      url: https://github.com/spotify/elitzur
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 15
    - name: scio.g8
      description: A Giter8 template for scio
      stargazerCount: 29
      forkCount: 20
      url: https://github.com/spotify/scio.g8
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 20
- config:
    id: go
    name: Go
    icons_html: |
      <i class="devicon-go-original-wordmark"></i>
  projects:
    - name: flyte-flink-plugin
      description: Flyte Flink k8s plugin.
      stargazerCount: 18
      forkCount: 12
      url: https://github.com/spotify/flyte-flink-plugin
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 19
    - name: flink-on-k8s-operator
      description: Kubernetes operator for managing the lifecycle of Apache Flink and
        Beam applications.
      stargazerCount: 188
      forkCount: 68
      url: https://github.com/spotify/flink-on-k8s-operator
      openIssues:
        totalCount: 32
      watchers:
        totalCount: 11
    - name: confidence-sdk-go
      description: null
      stargazerCount: 2
      forkCount: 1
      url: https://github.com/spotify/confidence-sdk-go
      openIssues:
        totalCount: 2
      watchers:
        totalCount: 12
- config:
    id: swift
    name: Swift
    icons_html: |
      <i class="devicon-swift-plain"></i>
  projects:
    - name: Mobius.swift
      description: A functional reactive framework for managing state evolution and
        side-effects [Swift implementation]
      stargazerCount: 556
      forkCount: 42
      url: https://github.com/spotify/Mobius.swift
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 27
    - name: XCRemoteCache
      description: null
      stargazerCount: 833
      forkCount: 52
      url: https://github.com/spotify/XCRemoteCache
      openIssues:
        totalCount: 24
      watchers:
        totalCount: 25
    - name: XCMetrics
      description: XCMetrics is the easiest way to collect Xcode build metrics and
        improve developer productivity.
      stargazerCount: 1101
      forkCount: 78
      url: https://github.com/spotify/XCMetrics
      openIssues:
        totalCount: 14
      watchers:
        totalCount: 25
    - name: confidence-sdk-swift
      description: null
      stargazerCount: 18
      forkCount: 3
      url: https://github.com/spotify/confidence-sdk-swift
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 11
    - name: diffuser
      description: Declaratively define relationships between data and side effects
      stargazerCount: 45
      forkCount: 13
      url: https://github.com/spotify/diffuser
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 14
- config:
    id: other
    name: Other
  projects:
    - name: git-test
      description: test your commits
      stargazerCount: 203
      forkCount: 41
      url: https://github.com/spotify/git-test
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 111
    - name: cstar
      description: Apache Cassandra cluster orchestration tool for the command line
      stargazerCount: 254
      forkCount: 37
      url: https://github.com/spotify/cstar
      openIssues:
        totalCount: 8
      watchers:
        totalCount: 22
    - name: dockerfile-mode
      description: An emacs mode for handling Dockerfiles
      stargazerCount: 535
      forkCount: 79
      url: https://github.com/spotify/dockerfile-mode
      openIssues:
        totalCount: 9
      watchers:
        totalCount: 20
    - name: foss-root
      description: Maven POM that serves as a parent for Spotify FOSS projects
      stargazerCount: 1
      forkCount: 7
      url: https://github.com/spotify/foss-root
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 10
        cmake -B {
  // See https://go.microsoft.com/fwlink/?LinkId=827846 to learn about workspace recommendations.
  // Extension identifier format: ${publisher}.${name}. Example: vscode.csharp
  // List of extensions which should be recommended for users of this workspace.
  "recommendations": [
    "dbaeumer.vscode-eslint",
    "rust-lang.rust-analyzer",
    "bradlc.vscode-tailwindcss",
    "tauri-apps.tauri-vscode",
    "esbenp.prettier-vscode"
  ],
  // List of extensions recommended by VS Code that should not be recommended for users of this workspace.
  "unwantedRecommendations": []
}
-DCMAKE_CXX_COMPILER=${{ CC := gcc
ifeq ($(USE_GPU),1)
CUCC := nvcc
endif

# Select backend files based on selected backend
# Supported values: naive, onednn
BACKEND ?= onednn

# Set to 1 to use accelerated matrix products when using naive backend
USE_AVX ?= 0

# The root directory of the oneDNN library, only needed when using
# onednn backend
ONEDNN_ROOT_DIR ?= lib/onednn
ONEDNN_INCLUDE_DIR := $(ONEDNN_ROOT_DIR)/include
ONEDNN_SHARED_DIR := $(ONEDNN_ROOT_DIR)/lib/

# Can be set in case the directory where libcudart.so is located is not
# in the default directories
CUDA_LIB_DIR ?=

# Select log level
# Supported values: 1(error), 2(warn), 3(info), 4(trace)
LOG_LEVEL ?= 3

# Set to 1 to enable gdb support
DEBUG ?= 0


ifeq ($(DEBUG),1)
ifeq ($(USE_AVX),1)
$(error Can not have DEBUG=1 and USE_AVX=1 at the same time)
endif
endif


CFLAGS :=
CUFLAGS :=
ifdef LOG_LEVEL
CFLAGS += -DLOG_LEVEL=$(LOG_LEVEL)
CUFLAGS += -DLOG_LEVEL=$(LOG_LEVEL)
endif
ifeq ($(USE_AVX),1)
CFLAGS += -march=haswell -DUSE_AVX
endif
ifeq ($(USE_GPU),1)
CFLAGS += -DUSE_GPU
CUFLAGS += -DUSE_GPU
endif
ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG
CUFLAGS += -g -DDEBUG
else
CFLAGS += -O3 -Ofast
CUFLAGS += -O3
endif


# math library
LDFLAGS := -lm


SOURCEDIR := src

# INCLUDE and SOURCE file located in the src directory
INCLUDE := -I$(SOURCEDIR)/lib -I$(SOURCEDIR)/common
SRC := $(shell find $(SOURCEDIR)/common -name '*.c')
SRC += $(SOURCEDIR)/lib/log.c $(SOURCEDIR)/lib/config_info.c $(SOURCEDIR)/lib/random.c
# Also add the target source file
SRC += $(TARGET).c


# Select backend files based on selected backend
ifeq ($(BACKEND),naive)
INCLUDE += -I$(SOURCEDIR)/naive -I$(SOURCEDIR)/include
SRC += $(shell find $(SOURCEDIR)/naive -name '*.c')
ifeq ($(USE_GPU),1)
SRC += $(shell find $(SOURCEDIR)/naive -name '*.cu')
ifneq ($(CUDA_LIB_DIR),)
LDFLAGS += -L$(CUDA_LIB_DIR)
endif
LDFLAGS += -lcudart
endif
CFLAGS += -DBACKEND_NAIVE
CUFLAGS += -DBACKEND_NAIVE
else ifeq ($(BACKEND),onednn)
INCLUDE += -I$(SOURCEDIR)/onednn -I$(ONEDNN_INCLUDE_DIR)
SRC += $(shell find $(SOURCEDIR)/onednn -name '*.c')
LDFLAGS += -L$(ONEDNN_SHARED_DIR) -ldnnl 
CFLAGS += -DBACKEND_ONEDNN
else
$(error Only naive and onednn implementation available.)
endif


# Object files are placed in same directory as src files, just with different file extension
OBJ := $(SRC:.c=.o)
ifeq ($(USE_GPU),1)
OBJ := $(OBJ:.cu=.o)
endif }}
        -DCMAKE_C_COMPILER=${{ include config/defines.mk


# Link all object files into a source file
$(TARGET): $(OBJ)
        $(CC) $^ -o $@ $(LDFLAGS)


# Rule to compile a single translation unit
%.o: %.c
        $(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@

# Rule to compile a single cuda translation unit
ifeq ($(USE_GPU),1)
%.o: %.cu
        $(CUCC) $(INCLUDE) $(CUFLAGS) -c $< -o $@
endif


clean:
        @$(RM) -rv $(TARGET) $(OBJ)


rebuild:
        make clean && make


run: $(TARGET)
# since oneDNN is built as a shared library, need to add its location
# to LD_LIBRARY_PATH so that the target executable can find it
ifeq ($(BACKEND),onednn)
run: export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(ONEDNN_SHARED_DIR)
endif
run:
        @$(TARGET)


.PHONY: clean rebuild run }}
        -DCMAKE_BUILD_TYPE=${{ /**
 * @file lenet5_mnist.c
 * @brief Training a CNN on Fashion MNIST
 * 
 * Training a CNN on Fashion MNIST.
 */


#include <inttypes.h>
#include <math.h>
#include <stdio.h>


#include "core/layer.h"
#include "core/loss.h"
#include "core/optimizer.h"

#include "optimizer/adam.h"
#include "optimizer/rmsprop.h"
#include "optimizer/sgd.h"

#include "sequential/model_desc.h"
#include "sequential/sequential_model.h"

#include "dataset/dataset.h"
#include "dataset/dataset_utils.h"
#include "dataset/mnist.h"

#include "augment/augment_pipeline.h"
#include "augment/image_flip.h"
#include "augment/random_crop.h"
#include "augment/color_augment.h"

#include "util/training_utils.h"

#include "config_info.h"
#include "log.h"
#include "tensor.h"


/* set to location of mnist or fashion_mnist root folder */
static const char* mnist_path = "datasets/fashion_mnist";

static const size_t batch_size = 32;
static const size_t num_epochs = 10000;
static const size_t test_every = 5;

/* learning rate schedule parameters */
static const float initial_lr = 0.2f;
static const float final_lr = 0.01f;
static const size_t decay_begin = 0;
static const size_t decay_end = 20;
static const int32_t reduce_lr_after = 7;
static const float reduce_lr_by_factor = 10.0f;

static const float dropout_rate = 0.25f;

/* augmentation config */
static const bool augment = true;
static const size_t padding = 2;

/* optimizer config */
static const optimizer_impl_t* optimizer = &sgd_optimizer;
static const sgd_config_t optimizer_config = {
    .learning_rate = 0.0f, /* overwritten by lr_schedule */
    .weight_reg_kind = SGD_WEIGHT_REG_L2,
    .weight_reg_strength = 1e-4f,
};

/* conv -> batchnorm -> relu -> conv -> batchnorm -> relu -> pool */
void model_desc_add_conv_block(model_desc_t* desc, size_t out_channels, size_t filter_size)
{
    static const float bn_momentum = 0.9f;
    static const float bn_eps = 1e-8f;

    const size_t padding = filter_size / 2;

    model_desc_add_convolutional_layer(desc, out_channels, filter_size, 1, padding, winit_he, winit_zeros);
    model_desc_add_batch_norm_layer(desc, bn_momentum, bn_eps);
    model_desc_add_activation_layer(desc, ACTIVATION_FUNCTION_RELU);

    model_desc_add_convolutional_layer(desc, out_channels, filter_size, 1, padding, winit_he, winit_zeros);
    model_desc_add_batch_norm_layer(desc, bn_momentum, bn_eps);
    model_desc_add_activation_layer(desc, ACTIVATION_FUNCTION_RELU);

    model_desc_add_pooling_layer(desc, 2, 1, 0, POOLING_MAX);
}

/* linear -> relu -> ?dropout */
void model_desc_add_linear_dropout(model_desc_t* desc, size_t out_channels, float dropout_rate)
{
    model_desc_add_linear_layer(desc, out_channels, winit_he, winit_zeros);
    model_desc_add_activation_layer(desc, ACTIVATION_FUNCTION_RELU);
    if (dropout_rate > 0.0f) {
        model_desc_add_dropout_layer(desc, dropout_rate);
    }
}


/* crop1: 93.51% */
layer_t create_small_cnn(const tensor_shape_t* input_shape, float dropout_rate, size_t batch_size)
{
    layer_t model = NULL;
    model_desc_t* desc = NULL;

    model_desc_create(&desc);

    model_desc_add_conv_block(desc, 64, 3);
    model_desc_add_conv_block(desc, 64, 3);
    model_desc_add_linear_dropout(desc, 512, dropout_rate);
    model_desc_add_linear_dropout(desc, 128, dropout_rate);
    model_desc_add_linear_layer(desc, 10, winit_he, winit_zeros);

    model_desc_dump(desc);

    const sequential_model_create_info_t create_info = {
        .desc = desc,
        .max_batch_size = batch_size,
    };
    layer_create(&model, &sequential_model_impl, &create_info, input_shape, batch_size);

    model_desc_destroy(desc);
    return model;
}


/* crop: 2, dropout: 0.5 - 93.60%, dropout: 0.25 - 94.10% */
layer_t create_cnn(const tensor_shape_t* input_shape, float dropout_rate, size_t batch_size)
{
    layer_t model = NULL;
    model_desc_t* desc = NULL;

    model_desc_create(&desc);

    model_desc_add_conv_block(desc, 128, 3);
    model_desc_add_conv_block(desc, 128, 3);
    model_desc_add_linear_dropout(desc, 1024, dropout_rate);
    model_desc_add_linear_dropout(desc, 256, dropout_rate);
    model_desc_add_linear_layer(desc, 10, winit_he, winit_zeros);


    /* Print a model overview to stdout. */
    model_desc_dump(desc);

    const sequential_model_create_info_t create_info = {
        .desc = desc,
        .max_batch_size = batch_size,
    };
    layer_create(&model, &sequential_model_impl, &create_info, input_shape, batch_size);

    model_desc_destroy(desc);
    return model;
}


augment_pipeline_t setup_augment_pipeline()
{
    const image_flip_config_t flip_config = {
        .horizontal_flip_prob = 0.5f,
        .vertical_flip_prob = 0.0f,
    };

    const random_crop_config_t crop_config = {
        .padding = padding,
    };

    const color_augment_config_t color_config = {
        .brightness_augment_prob = 1.0f,
        .brightness_std = 0.1f,
        .contrast_augment_prob = 1.0f,
        .contrast_std = 0.1f,
        .contrast_midpoint = 0.0f /* since images are normalized */
    };


    const augment_pipeline_config_entry_t pipeline_entries[] = {
        { .impl = &aug_image_flip, .config = &flip_config },
        { .impl = &aug_random_crop, .config = &crop_config },
        //{ .impl = &aug_color, .config = &color_config }
    };

    const augment_pipeline_config_t pipeline_config = {
        .entries = pipeline_entries,
        .num_entries = sizeof(pipeline_entries) / sizeof(*pipeline_entries),
    };

    augment_pipeline_t augment_pipeline = NULL;
    augment_pipeline_create(&augment_pipeline, &pipeline_config);
    return augment_pipeline;
}


static dataset_t train_set = NULL, test_set = NULL;
static Loss loss;
void train_callback(const training_state_t* state)
{
    float test_accuracy = NAN;
    float test_loss = NAN;

    if (state->epoch % test_every == 0) {    
        module_test(state->model, test_set, batch_size, &loss, &test_accuracy, &test_loss);
        printf("Epoch %" PRIi32 " | Train loss %f | Train accuracy %5.2f%% | Test loss %f "
            "| Test accuracy %5.2f%% | lr %.2e\n",
            state->epoch,
            state->train_loss,
            state->train_accuracy * 100.0f,
            test_loss,
            test_accuracy * 100.0f,
            optimizer_get_learning_rate(state->optimizer)
        );
    }
}


float linear_lr_schedule(const training_state_t* state)
{
    static float min_train_loss = INFINITY;
    static int32_t min_train_loss_epoch = 0;

    if (state->train_loss < min_train_loss) {
        min_train_loss = state->train_loss;
        min_train_loss_epoch = state->epoch;
    }

    if (state->epoch < decay_begin) {
        return initial_lr;
    } else if (state->epoch <= decay_end) {
        return final_lr + (initial_lr - final_lr) * (decay_end - state->epoch)
            / (decay_end - decay_begin);
    } else {
        if (reduce_lr_after != 0 && state->epoch - min_train_loss_epoch >= reduce_lr_after) {
            min_train_loss = state->train_loss;
            min_train_loss_epoch = state->epoch;
            return optimizer_get_learning_rate(state->optimizer) / reduce_lr_by_factor;
        } else {
            return optimizer_get_learning_rate(state->optimizer);
        }
    }
}


int main()
{
    /* load the dataset */
    const mnist_create_info_t dataset_config = {
        .path = mnist_path,
        .padding = 0,
    };

    if (dataset_create_train_and_test(&mnist_dataset, &dataset_config, true, &train_set,
                                      &test_set) != 0) {
        LOG_ERROR("There was an error loading the mnist dataset\n");
        return 1;
    }
    LOG_INFO("Successfully loaded mnist\n");


    /* initialize augmentation pipeline */
    augment_pipeline_t augment_pipeline = augment ? setup_augment_pipeline() : NULL;
    if (augment && augment_pipeline == NULL) {
        LOG_ERROR("There was an error setting up the augmentation pipeline\n");
        return 1;
    }
    LOG_INFO("Successfully set up the augmentation pipeline\n");


    /* create the model */
    const tensor_shape_t* data_shape = dataset_get_shape(train_set);
    layer_t model = create_cnn(data_shape, dropout_rate, batch_size);
    size_t num_params = module_get_num_params(model);
    LOG_INFO("Created the model. #params = %zu. Training starts now\n", num_params);


    /* initialize loss */
    LossInit(&loss, layer_get_output_shape(model), batch_size, LOSS_FUNCTION_CROSS_ENTROPY);

    /* train */
    module_train(model, train_set, augment_pipeline, num_epochs, batch_size, optimizer,
        &optimizer_config, linear_lr_schedule, &loss, train_callback);


    /* free resources */
    layer_destroy(model);
    dataset_destroy(train_set);
    dataset_destroy(test_set);
    if (augment_pipeline != NULL) {
        augment_pipeline_destroy(augment_pipeline);
    }
    LossDeinit(&loss);

    return 0;
} }}
        -S ${{ /**
 * @file lenet5_mnist.c
 * @brief Train LeNet-5 on the MNIST dataset
 * 
 * This example illustrates how this library can be used to implement the LeNet-5 architecture
 * and train it on the MNIST dataset.
 */

#include <inttypes.h>
#include <math.h>

#include "neuralnet.h"
#include "sequential/model_desc.h"
#include "sequential/sequential_model.h"
#include "dataset/mnist.h"
#include "optimizer/sgd.h"

/* path to MNIST or Fashion MNIST dataset */
const char* mnist_path = "datasets/mnist";

#if defined(USE_GPU)
static const device_t device = device_gpu;
#else
static const device_t device = device_cpu;
#endif

/* When training on mnist with this configuration, the model should reach an accuracy of 90%+
    after one epoch and an accuracy of ~98% after 10 epochs */
static const size_t num_epochs = 50;
static const size_t batch_size = 32;
static const sgd_config_t optimizer_config = {
    .learning_rate = 1e-2f,
    .weight_reg_kind = WEIGHT_REG_NONE,
};


layer_t create_lenet5(const tensor_shape_t* input_shape, size_t batch_size)
{
    model_desc_t* desc = NULL;
    layer_t model = NULL;

    /* Some default configurations */
    const activation_layer_create_info_t act_config = {
        .activation_function = ACTIVATION_FUNCTION_TANH,
    };
    const pooling_layer_create_info_t pool_config = {
        .kernel_width = 2,
        .pooling_operation = POOLING_AVERAGE,
    };
    const linear_layer_create_info_t linear_default_config = {
        .weight_init = winit_xavier,
        .bias_init = winit_zeros,
    };

    model_desc_create(&desc);

    convolutional_layer_create_info_t conv1_config = conv_default_config;
    conv1_config.output_channels = 6;
    conv1_config.filter_height = 5;
    conv1_config.filter_width = 5;
    conv1_config.padding_y = 2;
    conv1_config.padding_x = 2;
    model_desc_add_layer(desc, &convolutional_layer_impl, &conv1_config);
    model_desc_add_layer(desc, &activation_layer_impl, &act_config);
    model_desc_add_layer(desc, &pooling_layer_impl, &pool_config);

    convolutional_layer_create_info_t conv2_config = conv_default_config;
    conv2_config.output_channels = 16;
    conv2_config.filter_height = 5;
    conv2_config.filter_width = 5;
    model_desc_add_layer(desc, &convolutional_layer_impl, &conv2_config);
    model_desc_add_layer(desc, &activation_layer_impl, &act_config);
    model_desc_add_layer(desc, &pooling_layer_impl, &pool_config);

    linear_layer_create_info_t linear1_config = linear_default_config;
    linear1_config.output_size = 120;
    model_desc_add_layer(desc, &linear_layer_impl, &linear1_config);
    model_desc_add_layer(desc, &activation_layer_impl, &act_config);

    linear_layer_create_info_t linear2_config = linear_default_config;
    linear2_config.output_size = 84;
    model_desc_add_layer(desc, &linear_layer_impl, &linear2_config);
    model_desc_add_layer(desc, &activation_layer_impl, &act_config);

    linear_layer_create_info_t linear3_config = linear_default_config;
    linear3_config.output_size = 10;
    model_desc_add_layer(desc, &linear_layer_impl, &linear3_config);

    model_desc_dump(desc);

    const sequential_model_create_info_t create_info = {
        .desc = desc,
        .max_batch_size = batch_size,
    };
    layer_create(&model, &sequential_model_impl, &create_info, input_shape, device, batch_size);
    model_desc_destroy(desc);

    return model;
}


dataset_t train_set = NULL, test_set = NULL;
void train_callback(const training_state_t* state)
{
    float test_accuracy = NAN;
    float test_loss = NAN;

    module_test(state->model, test_set, batch_size, state->loss, &test_accuracy, &test_loss);
    LOG_INFO("Epoch %" PRIi32 " | Train loss %f | Train accuracy %5.3f%% | Test loss %f "
        "| Test accuracy %5.3f%%\n",
        state->epoch,
        state->train_loss,
        state->train_accuracy * 100.0f,
        test_loss,
        test_accuracy * 100.0f
    );
}


int main()
{
    /* load the dataset */
    const mnist_create_info_t dataset_config = {
        .path = mnist_path,
        .padding = 0,
    };
    if (dataset_create_train_and_test(&mnist_dataset, &dataset_config, true, &train_set,
                                      &test_set) != 0) {
        LOG_ERROR("There was an error loading the mnist dataset\n");
        return 1;
    }
    LOG_INFO("Successfully loaded mnist\n");


    layer_t lenet5 = create_lenet5(dataset_get_shape(train_set), batch_size);

    const size_t num_params = module_get_num_params(lenet5);
    LOG_INFO("Created the model. #parameters %d. Start training...\n", num_params);

    /* create the loss */
    Loss loss;
    LossInit(&loss, layer_get_output_shape(lenet5), batch_size, LOSS_FUNCTION_CROSS_ENTROPY);


    module_train(lenet5, train_set, NULL, num_epochs, batch_size, &sgd_optimizer, &optimizer_config,
                 NULL, &loss, train_callback);


    /* Free resources */
    dataset_destroy(train_set);
    dataset_destroy(test_set);
    layer_destroy(lenet5);
    LossDeinit(&loss);

    return 0;
} }}

    - name: /**
 * @file logistic_regression.c
 * @brief Multi-class logistic regression with linear classifier
 * 
 */


#if defined(BACKEND_ONEDNN)
#error "onednn backend is not supported by this example" 
#endif


#include <inttypes.h>
#include <math.h>
#include <stddef.h>

#include "neuralnet.h"
#include "dataset/mnist.h"
#include "layer/linear_layer.h"
#include "optimizer/sgd.h"


static const char* mnist_path = "datasets/mnist";

#if defined(USE_GPU)
static const device_t device = device_gpu;
#else
static const device_t device = device_cpu;
#endif

/* config */
static const size_t num_epochs = 1000;
static const float learning_rate = 0.1f;
static const float l2_reg_strength = 0.0f; /* ridge regression */

dataset_t train_set, test_set;
void train_callback(const training_state_t* state)
{
    float test_accuracy = NAN;
    float test_loss = NAN;

    const tensor_shape_t* test_set_shape = dataset_get_shape(test_set);
    const size_t test_samples = tensor_shape_get_dim(test_set_shape, TENSOR_BATCH_DIM);

    if (state->epoch != 0) {
        module_test(state->model, test_set, test_samples, state->loss, &test_accuracy, &test_loss);
        LOG_INFO("Epoch %" PRIi32 " | Train loss %f | Train accuracy %5.3f%% | Test loss %f "
            "| Test accuracy %5.3f%%\n",
            state->epoch,
            state->train_loss,
            state->train_accuracy * 100.0f,
            test_loss,
            test_accuracy * 100.0f
        );
    }
}


int main()
{
    /* load the dataset */
    const mnist_create_info_t dataset_config = {
        .path = mnist_path,
        .padding = 0,
    };
    if (dataset_create_train_and_test(&mnist_dataset, &dataset_config, true, &train_set,
                                      &test_set) != 0) {
        LOG_ERROR("There was an error loading the mnist dataset\n");
        return 1;
    }
    LOG_INFO("Successfully loaded mnist\n");

    const tensor_shape_t* train_set_shape = dataset_get_shape(train_set);
    const size_t num_samples = tensor_shape_get_dim(train_set_shape, TENSOR_BATCH_DIM);


    /* create classifier as simple linear layer */
    layer_t classifier;
    const linear_layer_create_info_t classifier_config = {
        .output_size = 10,
        .weight_init = winit_xavier,
        .bias_init = winit_zeros
    };
    layer_create(&classifier, &linear_layer_impl, &classifier_config, train_set_shape, device, num_samples);
    if (classifier == NULL) {
        LOG_ERROR("There was an error creating the model\n");
        return 1;
    }
    LOG_INFO("Created the classifier. Start training...\n");


    /* create the loss */
    Loss loss;
    LossInit(&loss, layer_get_output_shape(classifier), num_samples, LOSS_FUNCTION_CROSS_ENTROPY);


    /* training loop */
    const sgd_config_t optimizer_config = {
        .learning_rate = learning_rate,
        .weight_reg_kind = WEIGHT_REG_L2,
        .weight_reg_strength = l2_reg_strength
    };
    module_train(classifier, train_set, NULL, num_epochs, num_samples, &sgd_optimizer, &optimizer_config,
                 NULL, &loss, train_callback);


    dataset_destroy(train_set);
    dataset_destroy(test_set);
    layer_destroy(classifier);
    LossDeinit(&loss);

    return 0;
}
      # Build your program with the given configuration. Note that --config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      run: cmake --build ${{ /**
 * @file two_layer_mlp_mnist.c
 * @brief Train a two-layered MLP on the MNIST dataset
 * 
 * This example illustrates how this library can be used to implement a two layer deep MLP and
 * trains it on the MNIST dataset.
 */

#include <inttypes.h>
#include <math.h>
#include <stddef.h>

#include "neuralnet.h"

#include "sequential/model_desc.h"
#include "sequential/sequential_model.h"

#include "dataset/mnist.h"
#include "optimizer/sgd.h"


#if defined(USE_GPU)
static const device_t device = device_gpu;
#else
static const device_t device = device_cpu;
#endif


static const char* mnist_path = "datasets/mnist";

/* config */
static const size_t num_epochs = 100;
static const size_t hidden_size = 300;
static const size_t batch_size = 32;
static const float learning_rate = 0.5f;


/* FC(hidden_size) -> Sigmoid -> FC(10) */
layer_t create_mlp(const tensor_shape_t* input_shape, size_t hidden_size, size_t batch_size)
{
    model_desc_t* desc;
    layer_t model;

    model_desc_create(&desc);

    model_desc_add_linear_layer(desc, hidden_size, winit_xavier, winit_zeros);
    model_desc_add_activation_layer(desc, ACTIVATION_FUNCTION_SIGMOID);
    model_desc_add_linear_layer(desc, 10, winit_xavier, winit_zeros);

    /* Print a model overview to stdout. */
    model_desc_dump(desc);

    const sequential_model_create_info_t config = {
        .desc = desc,
        .max_batch_size = batch_size
    };
    layer_create(&model, &sequential_model_impl, &config, input_shape, device, batch_size);

    /* Model desc not needed anymore */
    model_desc_destroy(desc);

    return model;
}


dataset_t train_set, test_set;
void train_callback(const training_state_t* state)
{
    float test_accuracy = NAN;
    float test_loss = NAN;

    module_test(state->model, test_set, batch_size, state->loss, &test_accuracy, &test_loss);
    LOG_INFO("Epoch %" PRIi32 " | Train loss %f | Train accuracy %5.3f%% | Test loss %f "
        "| Test accuracy %5.3f%%\n",
        state->epoch,
        state->train_loss,
        state->train_accuracy * 100.0f,
        test_loss,
        test_accuracy * 100.0f
    );
}


int main()
{
    /* load the dataset */
    const mnist_create_info_t dataset_config = {
        .path = mnist_path,
        .padding = 0,
    };
    if (dataset_create_train_and_test(&mnist_dataset, &dataset_config, true, &train_set,
                                      &test_set) != 0) {
        LOG_ERROR("There was an error loading the mnist dataset\n");
        return 1;
    }
    LOG_INFO("Successfully loaded mnist\n");


    /* create the model */
    layer_t mlp = create_mlp(dataset_get_shape(train_set), hidden_size, batch_size);
    if (mlp == NULL) {
        LOG_ERROR("There was an error creating the model\n");
        return 1;
    }
    LOG_INFO("Created the model. Start training...\n");


    /* create the loss */
    Loss loss;
    LossInit(&loss, layer_get_output_shape(mlp), batch_size, LOSS_FUNCTION_CROSS_ENTROPY);


    /* Training loop */
    const sgd_config_t optimizer_config = {
        .learning_rate = learning_rate,
        .weight_reg_kind = WEIGHT_REG_NONE
    };
    module_train(mlp, train_set, NULL, num_epochs, batch_size, &sgd_optimizer, &optimizer_config,
                 NULL, &loss, train_callback);


    /* Free resources */
    dataset_destroy(train_set);
    dataset_destroy(test_set);
    layer_destroy(mlp);
    LossDeinit(&loss);

    return 0;
} }} --config ${{ #include "tensor.h"
#include "tensor_impl.h"

#include "context.h"

#include "layer/activation_layer.h"
#include "core/layer.h"

#include "random.h"

#include "log.h"
#include <stdio.h>

void test_tensor()
{
    tensor_shape_t shape = make_tensor_shape(4, 1, 2, 3, 4);
    LOG_INFO("Created shape\n");

    size_t size = tensor_size_from_shape(&shape);
    LOG_INFO("Tensor size is %zu\n", size);

    tensor_t tensor; 
    tensor_allocate(&tensor, &shape);
    LOG_INFO("Allocated tensor\n");

    LOG_INFO("Tensor data:\n");
    float* data = tensor_get_data(&tensor);
    for (size_t i = 0; i < size; i++) {
        printf("%f ", data[i]);
    }
    printf("\n");

    LOG_INFO("Filling tensor\n");
    tensor_fill(&tensor, 42.0f);

    LOG_INFO("Tensor data:\n");
    for (size_t i = 0; i < size; i++) {
        printf("%f ", data[i]);
    }
    printf("\n");

    LOG_INFO("Destroying tensor\n");
    tensor_destory(&tensor);
}


void test_activation_layer()
{
    layer_t layer;

    tensor_shape_t input_shape = make_tensor_shape(4, 1, 1, 2, 2);
    size_t size = tensor_size_from_shape(&input_shape);

    activation_layer_create_info_t act_config = {
        .activation_function = ACTIVATION_FUNCTION_RELU
    };

    LOG_INFO("Creating activation layer\n");
    layer_create(&layer, &activation_layer_impl, &act_config, &input_shape, 0);

    LOG_INFO("Allocating input tensor\n");
    tensor_t input;
    tensor_allocate(&input, &input_shape);
    float* input_data = tensor_get_data(&input);
    for (size_t i = 0; i < size; i++) {
        input_data[i] = RandomNormal(0.0f, 1.0f);
        printf("%f ", input_data[i]);
    }
    printf("\n");

    LOG_INFO("Applying activation\n");
    tensor_t* output;
    layer_forward(layer, LAYER_FORWARD_TRAINING, &input, &output);

    float* output_data = tensor_get_data(output);
    for (size_t i = 0; i < size; i++) {
        printf("%f ", output_data[i]);
    }
    printf("\n");

    LOG_INFO("Destroying input\n");
    tensor_destory(&input);
    LOG_INFO("Destroying layer\n");
    layer_destroy(layer);
}


int main() {

    if (backend_context_init() != 0) {
        LOG_ERROR("Backend context init failed\n");
    }
    LOG_INFO("Initialized backend context\n");

    test_activation_layer();
} }}

    - name: # !/bin/bash

export CC=gcc
export CXX=g++


ROOT_PWD=$(pwd)

# Go to temporary folder
mkdir -p tmp && cd tmp

# Clone oneDNN source code
git clone https://github.com/oneapi-src/oneDNN.git
cd oneDNN

# Library is tested at this commit, but feel free to use different version
git checkout 9ef80d1732d054b7f12f0475d7181b37ffeba662

# Create build directory
mkdir -p build && cd build

# Configure CMake and generate makefiles
cmake .. -DCMAKE_INSTALL_PREFIX=${ROOT_PWD}/lib/onednn

# Build the library with half of the available cores to not overload the system.
make -j $((($(nproc) + 1) / 2 ))


# Install the library and headers
cmake --build . --target install

# Remove temporary folder
cd ${ROOT_PWD}
rm -rf tmp
      working-directory: ${{ extern crate clap;
extern crate rand;

use std::collections::LinkedList;
use std::time::{Duration, Instant};
use std::cmp::{max, min};

use clap::Parser;
use rand::Rng;

mod term;
use term::{Input, Terminal};

#[derive(Parser, Debug)]
struct Args {
    #[clap(short = 'w', long = "width", help = "The width of the board (default is terminal width)")]
    width: Option<usize>,
    #[clap(short = 'h', long = "height", help = "The height of the board (default is terminal height)")]
    height: Option<usize>,
    #[clap(short = 'f', long = "frequency", help = "The amount of steps the snake makes per second")]
    freq: Option<u64>,
    #[clap(short = 's', long = "start_length", help = "The length the snake starts with")]
    start_length: Option<usize>,
    #[clap(short = 'r', long = "reward", help = "The length the snake gains from eating food")]
    reward: Option<usize>,
}

// Display constants

const EMPTY: u8 = ' ' as u8;
const BORDER: u8 = '#' as u8;
const FOOD: u8 = 'F' as u8;
const HEAD: u8 = '@' as u8;
const BODY: u8 = 'B' as u8;


#[derive(Clone, Copy)]
enum Direction {
    Right, Up, Left, Down
}

impl Direction {
    pub fn velocity(&self) -> (i32, i32) {
        match *self {
            Direction::Right => (0, 1),
            Direction::Up => (-1, 0),
            Direction::Left => (0, -1),
            Direction::Down => (1, 0)
        }
    }
}

// height + space for border
fn board_height(height: usize) -> usize {
    height + 2
}

// width + space for border and \n\r
fn board_width(width: usize) -> usize {
    width + 4
}

// initialize the board and draw boarder and \n\r
fn draw_border(board: &mut Vec<u8>, width: usize, height: usize) {
    board.clear();
    board.resize(board_height(height) * board_width(width), EMPTY);
    for w in 0..width {
        board[w + 1] = BORDER;
        board[(board_height(height) - 1) * board_width(width) + w + 1] = BORDER;
    }

    for h in 0..board_height(height) {
        board[h * board_width(width) + 0] = BORDER;
        board[h * board_width(width) + width + 1] = BORDER;
        board[h * board_width(width) + width + 2] = '\n' as u8;
        board[h * board_width(width) + width + 3] = '\r' as u8;
    }
}

fn draw_snake(board: &mut Vec<u8>, width: usize, snake: &LinkedList<(usize, usize)>) {
    let mut it = snake.into_iter();
    if let Some((h, w)) = it.next() {
        board[h * board_width(width) + w] = HEAD;
    } else {
        panic!("Snake has no head!");
    }
    while let Some((h, w)) = it.next() {
        board[h * board_width(width) + w] = BODY;
    }
}

fn draw_food(terminal: &Terminal, board: &mut Vec<u8>, width: usize, food: &(usize, usize)) {
    board[food.0 * board_width(width) + food.1] = FOOD;
    terminal.write_cell(FOOD, food.1, food.0).unwrap();
}

// move snake in direction and update board. return ({crashed into wall or myself}, {eaten food})
fn advance_snake(terminal: &Terminal, board: &mut Vec<u8>, width: usize, snake: &mut LinkedList<(usize, usize)>, direction: &Direction, target_length: &mut usize, reward: usize) -> (bool, bool) {
    if let Some(&(old_h, old_w)) = snake.front() {
        let new_head_h = (old_h as i32 + direction.velocity().0) as usize;
        let new_head_w = (old_w as i32 + direction.velocity().1) as usize;

        // advance
        snake.push_front((new_head_h, new_head_w));

        // check for collision
        let out = match board[new_head_h * board_width(width) + new_head_w] {
            FOOD => (true, false),
            BORDER => (false, true),
            BODY => (false, true),
            EMPTY => (false, false),
            _ => panic!("Impossible")
        };

        // write new head
        terminal.write_cell(HEAD, new_head_w, new_head_h).unwrap();
        terminal.write_cell(BODY, old_w, old_h).unwrap();
        board[new_head_h * board_width(width) + new_head_w] = HEAD;
        board[old_h * board_width(width) + old_w] = BODY;

        if out.0 { // eaten food
            *target_length += reward;
        }

        // remove tail if snake length is as big as target_length
        if snake.len() > *target_length {
            if let Some((h, w)) = snake.pop_back() {
                terminal.write_cell(EMPTY, w, h).unwrap();
                board[h * board_width(width) + w] = EMPTY;
            }
        }
        out
    } else {
        panic!("Snake has no head!");
    }
}

// find random free spot on the board in O(n) guaranteed
fn random_free_spot(board: &Vec<u8>, width: usize) -> Option<(usize, usize)> {
    let num_free = board.into_iter().filter(|c| -> bool {**c == EMPTY}).count();
    if num_free == 0 {
        return None;
    }
    let nth_free_i = rand::thread_rng().gen_range(0, num_free);
    let mut free_cnt = 0;
    for i in 0..board.len() {
        if board[i] == EMPTY {
            if free_cnt == nth_free_i {
                return Some((i / board_width(width), i % board_width(width)))
            } else {
                free_cnt += 1;
            }
        }
    }
    None
}

fn quit(term: &Terminal) -> ! {
    term.clean().unwrap();
    std::process::exit(0);
}

fn main() {

    let args = Args::parse();
    let terminal = Terminal::new();

    // default is terminal width / height
    let default_width = || -> usize {terminal.get_size().unwrap().0 - 2};
    let default_height = || -> usize {terminal.get_size().unwrap().1 - 3};

    let width = args.width.unwrap_or_else(default_width);
    let height = args.height.unwrap_or_else(default_height);

    // default is chosen so it takes the snake 4 seconds across the board 
    let freq = args.freq.unwrap_or(max(1, (min(width, height) / 4) as u64));
    let start_length = args.start_length.unwrap_or(5);
    let reward = args.reward.unwrap_or(5);

    let mut board : Vec<u8> = std::vec::Vec::with_capacity(board_height(height) * board_width(width));
    let mut target_length = 1 + start_length;
    let mut snake: LinkedList<(usize, usize)> = LinkedList::new();
    let mut direction = Direction::Right;
    let mut food: (usize, usize);


    terminal.setup().unwrap();

    // only draw border once
    draw_border(&mut board, width, height);

    // draw snake and food the first time
    snake.push_back((height / 2 + 1, width / 2 + 1));
    draw_snake(&mut board, width, &snake);
    food = match random_free_spot(&board, width) {
        Some(food) => food,
        None => quit(&terminal)
    };

    draw_food(&terminal, &mut board, width, &food);

    terminal.display(board.as_slice()).unwrap();


    let update_interval = Duration::from_micros((1000 * 1000) / freq);
    let mut update_deadline = Instant::now() + update_interval;

    let mut pause = false;

    loop {
        // user input
        if let Ok(Some(key)) = terminal.user_input(&update_deadline) {
            direction = match (key, direction) {
                (Input::Right, Direction::Right) => Direction::Down,
                (Input::Right, Direction::Down) => Direction::Left,
                (Input::Right, Direction::Left) => Direction::Up,
                (Input::Right, Direction::Up) => Direction::Right,
                (Input::Left, Direction::Right) => Direction::Up,
                (Input::Left, Direction::Up) => Direction::Left,
                (Input::Left, Direction::Left) => Direction::Down,
                (Input::Left, Direction::Down) => Direction::Right,
                (Input::Exit, _) => quit(&terminal),
                (Input::Pause, _) => {
                    pause = !pause;
                    continue;
                }
            };
        }

        if pause {
            continue;
        }

        update_deadline = Instant::now() + update_interval;

        // step: redraw snake and food if eaten
        let (eaten, crashed) = advance_snake(&terminal, &mut board, width, &mut snake, &direction, &mut target_length, reward);
        if eaten {
            food = match random_free_spot(&board, width) {
                Some(food) => food,
                None => quit(&terminal)
            };
            draw_food(&terminal, &mut board, width, &food);
        }
        if crashed {
            quit(&terminal);
        }
    }

} }}
      # Execute tests defined by the CMake configuration. Note that --build-config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest --build-config ${{ extern crate terminal;

use std::io::Write;
use terminal::{error, TerminalLock, Clear, Action, Value, Retrieved, Event, KeyCode, KeyEvent};

use std::time::Instant;

#[derive(PartialEq, Clone, Copy)]
pub enum Input {
    Left, Right, Exit, Pause
}


pub struct Terminal {
    terminal: terminal::Terminal<std::io::Stdout>,
}


impl Terminal {

    pub fn new() -> Terminal {
        Terminal{
            terminal: terminal::stdout()
        }
    }

    fn lock(&self) -> error::Result<TerminalLock<std::io::Stdout>> {
        self.terminal.lock_mut()
    }

    // enter new screen and hide cursor
    pub fn setup(&self) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::EnterAlternateScreen)?;
        lock.batch(Action::EnableRawMode)?;
        lock.batch(Action::HideCursor)?;
        lock.flush_batch()
    }

    // return terminal size in (width, height)
    pub fn get_size(&self) -> error::Result<(usize, usize)> {
        if let Ok(Retrieved::TerminalSize(w, h)) = self.lock()?.get(Value::TerminalSize) {
            Ok((w as usize, h as usize))
        } else {
            Err(error::ErrorKind::ActionNotSupported(std::string::String::from("")))
        }
    }

    // return entered keys until exit is entered or specified deadline is met
    pub fn user_input(&self, until: &Instant) -> error::Result<Option<Input>> {
        let lock = self.lock()?;

        let mut num_left = 0;
        let mut num_right = 0;

        loop {
            let now = Instant::now();
            if let Ok(Retrieved::Event(Some(Event::Key(key)))) = lock.get(Value::Event(Some(*until - now))) {
                match key {
                    KeyEvent{code: KeyCode::Left, ..} => {
                        num_left += 1;
                    },
                    KeyEvent{code: KeyCode::Right, ..} => {
                        num_right += 1;
                    },
                    KeyEvent{code: KeyCode::Char('q'), ..} => {
                        return Ok(Some(Input::Exit));
                    },
                    KeyEvent{code: KeyCode::Char('p'), ..} => {
                        return Ok(Some(Input::Pause));
                    }
                    _ => continue
                };
            } else {
                break;
            }
        }
        return Ok(if num_left > num_right {Some(Input::Left)} else if num_left < num_right {Some(Input::Right)} else {None})
    }

    // write board to screen
    pub fn display(&self, board: &[u8]) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.act(Action::ClearTerminal(Clear::All))?;
        lock.act(Action::MoveCursorTo(0, 0))?;
        lock.write(board)?;
        lock.flush_batch()
    }

    // rewrite a single cell
    pub fn write_cell(&self, symbol: u8, x: usize, h: usize) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::MoveCursorTo(x as u16, h as u16))?;
        lock.write(&[symbol])?;
        lock.flush_batch()
    }

    // show cursor again and return to old screen
    pub fn clean(&self) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::MoveCursorTo(0, 0))?;
        lock.batch(Action::ShowCursor)?;
        lock.batch(Action::DisableRawMode)?;
        lock.batch(Action::LeaveAlternateScreen)?;
        lock.flush_batch()
    }

} }}
