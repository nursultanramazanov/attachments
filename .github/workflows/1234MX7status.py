name: Rust

on: #!/usr/bin/env python3

# Usage
#
# fetchprice {YOUR_TICKER_NAME}
# or
# python3 fetchprice {YOUR_TICKER_NAME}
# 
# Example
# fetchprice NVAX
#
# Output
# {regularMarketPrice} {regularMarketChangePercent}
# 

import requests, sys

try:
    resp = requests.get(
        f'https://query1.finance.yahoo.com/v7/finance/quote?symbols={sys.argv[1]}',
        headers= {'User-agent': 'Mozilla/5.0'}
    )
    data = resp.json()['quoteResponse']['result'][0]

    market_state = data['marketState']
    if market_state == 'PRE':
        print('PRE', data['preMarketPrice'], data['preMarketChangePercent'])
    elif market_state == 'POST' or market_state == 'POSTPOST':
        print('POST', data['postMarketPrice'], data['postMarketChangePercent'])
    else:
        print('REGULAR', data['regularMarketPrice'], data['regularMarketChangePercent'])
except:
    print('Error')
  push: #include "sblocks.h"

#include <stdio.h>
#include <alsa/asoundlib.h>
#include <math.h>

#include "util.h"


static const char* volsymb[] = {
    "", "", "", "", ""
};


typedef struct smixer_t {
    snd_mixer_t *handle;
    snd_mixer_elem_t* elem;
} smixer_t;


static int smixer_setup(smixer_t* mixer, const char* selem_name, const char* card, int index, snd_mixer_elem_callback_t cb, void* arg)
{
    snd_mixer_selem_id_t *sid;

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, index);
    snd_mixer_selem_id_set_name(sid, selem_name);

    if (snd_mixer_open(&mixer->handle, 0) || 
        snd_mixer_attach(mixer->handle, card) ||
        snd_mixer_selem_register(mixer->handle, NULL, NULL) ||
        snd_mixer_load(mixer->handle)) {
        if (mixer->handle) {
            snd_mixer_close(mixer->handle);
        }
        mixer->handle = NULL;
        return 1;
    }
    mixer->elem = snd_mixer_find_selem(mixer->handle, sid);
    if (!mixer->elem) {
        snd_mixer_close(mixer->handle);
        mixer->handle = NULL;
        return 1;
    }

    snd_mixer_elem_set_callback(mixer->elem, cb);
    snd_mixer_elem_set_callback_private(mixer->elem, arg);
    return 0;
}


static void smixer_handle_next_event(smixer_t* mixer)
{
    if (!snd_mixer_wait(mixer->handle, -1)) {
        snd_mixer_handle_events(mixer->handle);
    }
}


static void smixer_destroy(smixer_t* mixer)
{
    snd_mixer_close(mixer->handle);
    mixer->handle = NULL;
    mixer->elem = NULL;
}


static int getvolume(snd_mixer_elem_t* elem, int* out_muted)
{
    long min, max, volume;
    if (!snd_mixer_selem_has_playback_switch(elem)) {
        *out_muted = 1;
    } else {
        snd_mixer_selem_get_playback_switch(elem, 0, out_muted);
        *out_muted = !*out_muted;
    }
    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_get_playback_volume(elem, 0, &volume);
    return (int)roundf(((float)(volume - min) / (float)(max - min)) * 100);
}


static int updatestatus(snd_mixer_elem_t* elem)
{
    sblock_t* block = (sblock_t*)snd_mixer_elem_get_callback_private(elem);

    int muted;
    int volume = getvolume(elem, &muted);
    volume = volume > 100 ? 100 : volume;

    int symbi;
    if (muted) {
        symbi = 0;
    } else if (volume == 0) {
        symbi = 1;
    } else if (volume < 33) {
        symbi = 2;
    } else if (volume < 66) {
        symbi = 3;
    } else {
        symbi = 4;
    }

    sblock_lock(block);
    if (block->status) {
        free(block->status);
    }
    block->status = smprintf("%s %d", volsymb[symbi], volume);
    sblock_unlock(block);
    sblock_signal_main(block);
    return 0;
}


static int volume_cb(snd_mixer_elem_t* elem, unsigned int mask)
{
    (void)mask;
    return updatestatus(elem);
}                           


void saudio_routine(sblock_t* block)
{
    msleep(2000); // give alsa time to set up
    smixer_t mixer;
    while(smixer_setup(&mixer, "Master", "default", 0, volume_cb, (void*)block)) {
        msleep(1000);
    }

    updatestatus(mixer.elem);

    for (;;) {
        smixer_handle_next_event(&mixer);
    }

    smixer_destroy(&mixer);
    if (block->status) {
        free(block->status);
    }
    block->status = NULL;
}
    branches: [ "main" ]
  pull_request: #include "sblocks.h"

#include "util.h"

#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define BATTERY_PATH "/sys/class/power_supply/BAT0"


static const int interval = 5000;


static const char* levelsymb[] = {
    "", "", "", "", "", ""
};

static const char* chargesymb[] = {
    "", ""
};


static int level_to_levelsymbi(int level)
{
    if (level < 0) return 0;
    if (level < 10) return 1;
    if (level < 30) return 2;
    if (level < 60) return 3;
    if (level < 90) return 4;
    return 5;
}

static int charge_to_chargesymbi(int level, int charge)
{
    if (level < 0 || charge == 0) return 0;
    return 1;
}


static int read_file_property(const char* filename, const char* type_modifier, void* property)
{
    FILE* fd = fopen(filename, "r");
    if (fd == NULL) {
        return -1;
    }
    int c = fscanf(fd, type_modifier, property);
    fclose(fd);
    return c == 1 ? 0 : -1;
}


static int getbattery(int* out_charging)
{
    char charging_str[12];
    int present, capacity, charge;

    // check if battery is present        
    if (read_file_property(BATTERY_PATH "/present", "%d", &present) < 0 || present != 1 // battery present
        || read_file_property(BATTERY_PATH "/energy_full", "%d", &capacity) < 0 // get capacity
        || read_file_property(BATTERY_PATH "/energy_now", "%d", &charge) < 0 // get current charge
        || read_file_property(BATTERY_PATH "/status", "%11s", charging_str) < 0) { // check if charging
        return -1;
    }

    *out_charging = strncmp(charging_str, "Charging", 8) == 0;
    return (int)roundf(((float)charge / (float)capacity) * 100.0f);
}


void sbattery_routine(sblock_t* block)
{
    int last_level = -1;
    int last_charging = 0;

    for (;;msleep(interval)) {
        int charging = 0;
        int level = getbattery(&charging);
        if (level == last_level && charging == last_charging) {
            continue;
        }

        int levelsymbi = level_to_levelsymbi(level);
        int chargesymbi = charge_to_chargesymbi(level, charging);

        sblock_lock(block);
        if (block->status) {
            free(block->status);
        }
        block->status = smprintf("%s %d %s", levelsymb[levelsymbi], level, chargesymb[chargesymbi]);
        sblock_unlock(block);
        sblock_signal_main(block);

        last_level = level;
        last_charging = charging;
    }

    if (block->status) {
        free(block->status);
    }
    block->status = NULL;
}
    branches: [ "main" ]

env: #pragma once

#include "sblock.h"

// shows the date and time
void stime_routine(sblock_t* block);


// shows the wifi connection
void swifi_routine(sblock_t* block);


// shows the audio volume
void saudio_routine(sblock_t* block);


// shows if microphone is muted
void smic_routine(sblock_t* block);


// shows battery status
void sbattery_routine(sblock_t* block);

// shows cpu, ram, disk usage
void ssysinfo_routine(sblock_t* block);


// gets stock quotes
void squotes_routine(sblock_t* block);
  CARGO_TERM_COLOR: #include "sblocks.h"

#include "util.h"

#include <alsa/asoundlib.h>
#include <math.h>


static const char* micsymb[] = {
    "", ""
};


typedef struct smixer_t {
    snd_mixer_t *handle;
    snd_mixer_elem_t* elem;
} smixer_t;


static int smixer_setup(smixer_t* mixer, const char* selem_name, const char* card, int index, snd_mixer_elem_callback_t cb, void* arg)
{
    snd_mixer_selem_id_t *sid;

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, index);
    snd_mixer_selem_id_set_name(sid, selem_name);

    if (snd_mixer_open(&mixer->handle, 0) || 
        snd_mixer_attach(mixer->handle, card) ||
        snd_mixer_selem_register(mixer->handle, NULL, NULL) ||
        snd_mixer_load(mixer->handle)) {
        if (mixer->handle) {
            snd_mixer_close(mixer->handle);
        }
        mixer->handle = NULL;
        return 1;
    }
    mixer->elem = snd_mixer_find_selem(mixer->handle, sid);
    if (!mixer->elem) {
        snd_mixer_close(mixer->handle);
        mixer->handle = NULL;
        return 1;
    }

    snd_mixer_elem_set_callback(mixer->elem, cb);
    snd_mixer_elem_set_callback_private(mixer->elem, arg);
    return 0;
}


static void smixer_handle_next_event(smixer_t* mixer)
{
    if (!snd_mixer_wait(mixer->handle, -1)) {
        snd_mixer_handle_events(mixer->handle);
    }
}


static void smixer_destroy(smixer_t* mixer)
{
    snd_mixer_close(mixer->handle);
    mixer->handle = NULL;
    mixer->elem = NULL;
}


static int ismuted(snd_mixer_elem_t* elem)
{
    if (!snd_mixer_selem_has_capture_switch(elem)) {
        return 1;
    }
    int capture = 0;
    snd_mixer_selem_get_capture_switch(elem, 0, &capture);
    return !capture;
}


static int updatestatus(snd_mixer_elem_t* elem)
{
    sblock_t* block = (sblock_t*)snd_mixer_elem_get_callback_private(elem);

    int symbi = ismuted(elem) ? 0 : 1;

    sblock_lock(block);
    if (block->status) {
        free(block->status);
    }
    block->status = smprintf("%s", micsymb[symbi]);
    sblock_unlock(block);
    sblock_signal_main(block);
    return 0;
}


static int mic_cb(snd_mixer_elem_t* elem, unsigned int mask)
{
    (void)mask;
    return updatestatus(elem);
}                           


void smic_routine(sblock_t* block)
{
    smixer_t mixer;
    while(smixer_setup(&mixer, "Capture", "default", 0, mic_cb, (void*)block)) {
        msleep(1000);
    }

    updatestatus(mixer.elem);
    for (;;) {
        smixer_handle_next_event(&mixer);
    }
    smixer_destroy(&mixer);
    if (block->status) {
        free(block->status);
    }
    block->status = NULL;
}

jobs: #include "sblocks.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "util.h"

static const int interval = 15 * 1000;


typedef enum market_state_t {
    regular, pre, post
} market_state_t;

int getquote(const char* stock, market_state_t* out_state, float* out_quote, float* out_change_percent)
{
    char state[8];

    char* cmd = smprintf("~/.status-scripts/fetchprice.py %s", stock);

    FILE* fd = popen(cmd, "r");
    if (fd == NULL) {
        goto error;
    }
    if (fscanf(fd, "%7s %f %f", state, out_quote, out_change_percent) != 3) {
        goto error;
    }

    if (strncmp(state, "PRE", 7) == 0) {
        *out_state = pre;
    } else if (strncmp(state, "POST", 7) == 0) {
        *out_state = post;
    } else {
        *out_state = regular;
    }

    pclose(fd);
    free(cmd);
    return 0
    ;
error:
    if (cmd != NULL) {
        free(cmd);
    }
    if (fd != NULL) {
        pclose(fd);
    }
    return -1;
}


void squotes_routine(sblock_t* block)
{
    float last_quote = -1;
    for(;;msleep(interval)) {
        market_state_t state;
        float quote, change_percent;
        if (getquote("NVAX", &state, &quote, &change_percent) || quote == last_quote) {
            continue;
        }
        sblock_lock(block);
        if (block->status) {
            free(block->status);
        }
        block->status = smprintf("$NVAX %s%.2f %s%.2f%%",
            state == pre ? "(pre) " : state == post ? "(post) " : "",
            quote,
            change_percent < 0 ? "" : "",
            fabs(change_percent));
        sblock_unlock(block);
        sblock_signal_main(block);
        last_quote = quote;
    }
}
  build: #include "sblocks.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/sysinfo.h>

#include "util.h"


static const int interval = 2000;


static int get_cpu_usage(long long* last_total, long long* last_work)
{
    long long user, nice, system, idle, iowait, irq, softirq;
    FILE* fd;

    if ((fd = fopen("/proc/stat", "r")) == NULL
        || fscanf(fd, "cpu %lld %lld %lld %lld %lld %lld %lld", &user, &nice, &system, &idle, &iowait, &irq, &softirq) != 7) {
        goto error;
    }
    fclose(fd);

    long long total = user + nice + system + idle + iowait + irq + softirq;
    long long work = user + nice + system;

    int ret = -1;
    if (*last_total != -1) {
        ret = round_int(*last_work - work, *last_total - total);
    }
    *last_total = total;
    *last_work = work;
    return ret;

error:
    if (fd != NULL) {
        fclose(fd);
    }
    *last_total = -1;
    *last_work = -1;
    return -1;
}


static int get_ram_usage(unsigned long* out_usedmib, unsigned long* out_totalmib, unsigned long* out_percent)
{
    FILE* fd = popen("free -m", "r");
    if (fd == NULL) {
        goto error;
    }
    char buff[1024];
    fgets(buff, 1024, fd);
    if (fscanf(fd, "Mem: %lu %lu", out_totalmib, out_usedmib) != 2) {
        goto error;
    }
    pclose(fd);
    *out_percent = *out_usedmib * 100 / *out_totalmib;
    return 0;
error:
    if (fd != NULL) {
        pclose(fd);
    }
    return -1;
}


void ssysinfo_routine(sblock_t* block)
{
    long long cpu_total = -1, cpu_work = -1;
    long long last_cpu_usage = -1;
    unsigned long last_ram_usedmib = -1;

    for (;;msleep(interval)) {
        int cpu_usage = get_cpu_usage(&cpu_total, &cpu_work);
        unsigned long ram_usedmib = -1, ram_totalmib = -1, ram_used_percent = -1;
        get_ram_usage(&ram_usedmib, &ram_totalmib, &ram_used_percent);
        if (cpu_usage == last_cpu_usage && ram_usedmib == last_ram_usedmib) {
            continue;
        }
        sblock_lock(block);
        if (block->status) {
            free(block->status);
        }
        block->status = smprintf("CPU %d%%  RAM %lu/%luMiB(%lu%%)", cpu_usage, ram_usedmib, ram_totalmib, ram_used_percent);
        sblock_unlock(block);
        sblock_signal_main(block);

        last_cpu_usage = cpu_usage;
        last_ram_usedmib = ram_usedmib;
    }
    if (block->status) {
        free(block->status);
    }
    block->status = NULL;
}

    runs-on: #include "sblocks.h"

#include <stdlib.h>
#include <stdio.h>

#include "util.h"


static const char* fmt = "%H:%M %d.%m.%Y";
static const size_t max_block_size = 128;


static void updatestatus(sblock_t* block)
{
    time_t tim;
    struct tm *timtm;
    tim = time(NULL);
    timtm = localtime(&tim);
    if (timtm == NULL) {
        return;
    }
    strftime(block->status, max_block_size, fmt, timtm);
}


static void waitminute()
{
    time_t tim = time(NULL);
    struct tm *timtm = localtime(&tim);
    msleep((60 - timtm->tm_sec) * 1000);
}


void stime_routine(sblock_t* block)
{
    block->status = (char*)malloc(max_block_size + 1);
    if (block->status == NULL) {
        fprintf(stderr, "Failed to allocate status in stime_routine.\n");
        return;
    }

    for(;;waitminute()) {
        sblock_lock(block);
        updatestatus(block);
        sblock_unlock(block);
        sblock_signal_main(block);
    }

    if (block->status) {
        free(block->status);
        block->status = NULL;
    }
}

    steps: #include "sblocks.h"

#include <stdlib.h>
#include <stdio.h>

#include "util.h"

static const int interval = 5 * 1000;

static const char* wifisymb[] = {
    "", "", "", "", ""
};


static int link_to_symbi(int link)
{
    if (link < 0)   return 0;
    if (link == 0)  return 1;
    if (link <= 20) return 2;
    if (link <= 50) return 3;
    return 4;
}


static int get_wifi_link()
{
    FILE* file = fopen("/proc/net/wireless", "r");
    if (file == NULL) {
        return -1;
    }
    char line[512];
    for (int i = 0; i < 3; i++) {
        if (fgets(line, sizeof(line) - 1, file) == NULL) {
            fclose(file);
            return -1;
        }
    }
    int link = 0;
    if (sscanf(line, "%*s %*s %d", &link) == EOF) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return link;
}


void swifi_routine(sblock_t* block)
{
    const int max_status_length = sizeof("") - 1;
    char status[sizeof("")];
    block->status = status;

    int last_symbi = 0;
    for(;;msleep(interval)) {
        int link = get_wifi_link();
        int symbi = link_to_symbi(link);
        if (symbi == last_symbi) {
            continue;
        }

        sblock_lock(block);
        snprintf(block->status, max_status_length + 1, "%s", wifisymb[symbi]);
        sblock_unlock(block);
        sblock_signal_main(block);

        last_symbi = symbi;
    }

    block->status = NULL;
}
        - uses: <?xml version='1.0' encoding='utf-8'?>
<resources>
    <array name="qt_sources">
        <item>https://download.qt-project.org/ministro/android/qt5/qt-5.4</item>
    </array>

    <!-- The following is handled automatically by the deployment tool. It should
         not be edited manually. -->

    <array name="bundled_libs">
        <!-- %%INSERT_EXTRA_LIBS%% -->
    </array>

     <array name="qt_libs">
         <!-- %%INSERT_QT_LIBS%% -->
     </array>

    <array name="bundled_in_lib">
        <!-- %%INSERT_BUNDLED_IN_LIB%% -->
    </array>
    <array name="bundled_in_assets">
        <!-- %%INSERT_BUNDLED_IN_ASSETS%% -->
    </array>

</resources>
    - name: package labs.mavlink.VideoReceiverApp;

/* Copyright 2013 Google Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: http://code.google.com/p/usb-serial-for-android/
 */
///////////////////////////////////////////////////////////////////////////////////////////
//  Written by: Mike Goza April 2014
//
//  These routines interface with the Android USB Host devices for serial port communication.
//  The code uses the usb-serial-for-android software library.  The QGCActivity class is the
//  interface to the C++ routines through jni calls.  Do not change the functions without also
//  changing the corresponding calls in the C++ routines or you will break the interface.
//
////////////////////////////////////////////////////////////////////////////////////////////

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Timer;
import java.util.TimerTask;
import java.io.IOException;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.widget.Toast;
import android.util.Log;
import android.os.PowerManager;
import android.os.Bundle;
import android.app.PendingIntent;
import android.view.WindowManager;
import android.os.Bundle;
import android.bluetooth.BluetoothDevice;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;

public class QGLSinkActivity extends QtActivity
{
    public native void nativeInit();

    // QGLSinkActivity singleton
    public QGLSinkActivity() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        nativeInit();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public void onInit(int status) {
    }

    public void jniOnLoad() {
        nativeInit();
    }
}
      run: /*
 * Copyright (C) 2012, Collabora Ltd.
 *   
 *
 * Copyright (C) 2015, Collabora Ltd.
 *   
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

package org.freedesktop.gstreamer.androidmedia;

import android.hardware.Camera;

public class GstAhcCallback implements Camera.PreviewCallback,
                                       Camera.ErrorCallback,
                                       Camera.AutoFocusCallback {
    public long mUserData;
    public long mCallback;

    public static native void gst_ah_camera_on_preview_frame(byte[] data, Camera camera,
                                                             long callback, long user_data);
    public static native void gst_ah_camera_on_error(int error, Camera camera,
                                                     long callback, long user_data);
    public static native void gst_ah_camera_on_auto_focus(boolean success, Camera camera,
                                                             long callback, long user_data);

    public GstAhcCallback(long callback, long user_data) {
        mCallback = callback;
        mUserData = user_data;
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        gst_ah_camera_on_preview_frame(data, camera, mCallback, mUserData);
    }

    @Override
    public void onError(int error, Camera camera) {
        gst_ah_camera_on_error(error, camera, mCallback, mUserData);
    }

    @Override
    public void onAutoFocus(boolean success, Camera camera) {
        gst_ah_camera_on_auto_focus(success, camera, mCallback, mUserData);
    }
}
    - name: /*
 * Copyright (C) 2016 SurroundIO
 *   
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

package org.freedesktop.gstreamer.androidmedia;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;

public class GstAhsCallback implements SensorEventListener {
    public long mUserData;
    public long mSensorCallback;
    public long mAccuracyCallback;

    public static native void gst_ah_sensor_on_sensor_changed(SensorEvent event,
                                                              long callback, long user_data);
    public static native void gst_ah_sensor_on_accuracy_changed(Sensor sensor, int accuracy,
                                                                long callback, long user_data);

    public GstAhsCallback(long sensor_callback,
        long accuracy_callback, long user_data) {
        mSensorCallback = sensor_callback;
        mAccuracyCallback = accuracy_callback;
        mUserData = user_data;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
      gst_ah_sensor_on_sensor_changed(event, mSensorCallback, mUserData);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
      gst_ah_sensor_on_accuracy_changed(sensor, accuracy,
          mAccuracyCallback, mUserData);
    }
}
      run: /*
 * Copyright (C) 2015, Collabora Ltd.
 *   
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

package org.freedesktop.gstreamer.androidmedia;

import android.graphics.SurfaceTexture;
import android.graphics.SurfaceTexture.OnFrameAvailableListener;

public class GstAmcOnFrameAvailableListener implements OnFrameAvailableListener
{
    private long context = 0;

    public synchronized void onFrameAvailable (SurfaceTexture surfaceTexture) {
        native_onFrameAvailable(context, surfaceTexture);
    }

    public synchronized long getContext () {
        return context;
    }

    public synchronized void setContext (long c) {
        context = c;
    }

    private native void native_onFrameAvailable (long context, SurfaceTexture surfaceTexture);
}
