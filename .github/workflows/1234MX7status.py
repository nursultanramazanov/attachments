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
    - uses: @media (max-width: 960px) {
    h1 { font-size: 40px; }
    h2 { font-size: 24px; }
    .button-big {
        margin: 30px auto;
    }
    .service-item {
        padding: 50px 2% 0;
        float: none;
        width: 100%;
    }
    .service-item:last-child {
        padding: 50px 2% 70px;
    }
}
@media (max-width: 720px) {
    /* header {
        height: auto;
    } */
    .header-container {
        display: block;
    }
    .header-container.center {
        max-width: none;
        width: 100%;
    }
    .logo {
        display: block;
        text-align: center;
        margin-left: auto;
        margin-right: auto;
    }
    .menu {
        display: block;
        text-align: center;
        font-size: 14px;
    }
    .menu-item {
        display: block;
        margin: 0;
        border-top: 1px solid #2A2A2A;
    }
    .menu-item a:link,
    .menu-item a:active,
    .menu-item a:visited {
        display: inline-block;
        width: 100%;
        padding: 10px 0;
    }
    h1, h3, h5 { font-size: 24px; }
    h2, h4, h6 { font-size: 16px; }
    .button-big {
        font-size: 12px;
        width: 120px;
        height: 36px;
        line-height: 36px;
        margin: 25px auto 15px;
    }
    .slider {
        padding-top: 25px;
    }
    .service-item {
        padding: 30px 2% 0;
    }
    .service-item:last-child {
        padding: 30px 2%;
    }
    .portfolio {
        padding: 10px 0 30px;
    }
    h3, h4 {
        margin: 30px 0;
    }
    .portfolio label {
        display: block;
        width: auto;
        margin: 5px;
    }
    .footer-container {
        padding: 10px 0;
    }
    .address, .phone {
        display: block;
        margin: 0;
        padding: 0;
    }
    .phone::before, .address::before {
        position: relative;
        left: -4px;
        top: 2px;
    }
    .social {
        margin: 18px 0;
    }
    .copyright-container {
        padding: 24px 0;
    }
}
    - name: @font-face {
    font-family: "Lato Regular";
    src: local("Lato Regular"), url(/fonts/Lato-Regular.ttf);
}
@font-face {
    font-family: "Lato Black";
    src: local("Lato Black"), url(/fonts/Lato-Black.ttf);
}
@font-face {
    font-family: "Lato Light";
    src: local("Lato Light"), url(/fonts/Lato-Light.ttf);
}
@font-face {
    font-family: "Lato LightItalic";
    src: local("Lato LightItalic"), url(/fonts/Lato-LightItalic.ttf);
}
      run: html, body {
    margin: 0;
    padding: 0;
    background-color: #FFF;
    font: 16px/1.3 "Lato Regular", arial;
}
.center {
    max-width: 1120px;
    width: 90%;
    margin: 0 auto;
}

/* HEADER */
header {
    background-color: #262626;
    color: #D9D9D9;
    font-family: "ReklameScript RegularDEMO";
}
.header-container {
    height: 100%;
    display: table;
    padding-top: 1px;
}
.logo {
    background-image: url(/img/logo.png);
    background-repeat: no-repeat;
    background-position: left center;
    display: inline-block;
    height: 33px;
    width: 107px;
    margin: 33px 0;
}
.menu {
    display: table-cell;
    vertical-align: middle;
    font: 13px "Lato Black";
    text-align: right;
}
.menu-item {
    display: inline-block;
    margin-left: 50px;
    text-transform: uppercase;
}
.menu-item a:link,
.menu-item a:active,
.menu-item a:visited {
    text-decoration: none;
    color: #D9D9D9;
    padding: 10px 0;
}
.menu-item.active a:link,
.menu-item.active a:active,
.menu-item.active a:visited {
    color: #FF6760;
}
.menu-item a:hover { color: #FCC; }
.menu-item.active a:hover { color: #FCC; }

/* SLIDER SECTION */
.slider {
    background-color: #2A2A2A;
    font-family: "Lato Light";
    text-align: center;
    color: #D9D9D9;
    padding: 35px 0 15px;
}
h1 {
    font-size: 60px;
    margin: 10px auto;
}
h1 strong {
    font-family: "Lato Black";
}
h2 {
    font-size: 40px;
    margin: 10px auto;
}
h2 em {
    font-family: "Lato LightItalic";
}
.button-big {
    display: block;
    width: 200px;
    height: 50px;
    margin: 45px auto;
    line-height: 50px;
    /* font: 14px "Lato Black"; 
    почему-то не работает, очень странно;
    а вот по одному все правильно применяется */
    font-family: "Lato Black";
    font-size: 14px;
    text-transform: uppercase;
    user-select: none;
    -webkit-user-select: none;
    -moz-user-select: none;
}
a.button-big:link,
a.button-big:visited,
a.button-bit:active {
    text-decoration: none;
    color: #FFF;
}
.button-red {
    background-color: #FF6760;
    border-top-left-radius: 5px;
    -webkit-border-top-left-radius: 5px;
    -moz-border-radius-topleft: 5px;
    border-bottom-left-radius: 5px;
    -webkit-border-bottom-left-radius: 5px;
    -moz-border-radius-bottomleft: 5px;
    border-top-right-radius: 5px;
    -webkit-border-top-right-radius: 5px;
    -moz-border-radius-topright: 5px;
    border-bottom-right-radius: 5px;
    -webkit-border-bottom-right-radius: 5px;
    -moz-border-radius-bottomright: 5px;
}
.button-red:hover {
    background-color: #F77;
}

/* SERVICES SECTION */
.services {
    background-color: #F8F8F8;
    text-align: center;
    font-size: 14px;
    line-height: 1.8;
    color: #616161;
}
.services-container::after {
    content: "";
    display: block;
    clear: both;
}
.service-item {
    float: left;
    width: 21%;
    padding: 80px 2%;
}
.service-icon {
    margin: auto;
    width: 65px;
    height: 70px;
    background-image: url(/img/service-sprite.png);
}
.service-icon-1 { background-position: left 0px center; }
.service-icon-2 { background-position: left 65px center; }
.service-icon-3 { background-position: left 130px center; }
.service-icon-4 { background-position: left 195px center; }
.service-name {
    font: 18px "Lato Black";
    margin: 10px 0;
}

/* PORTFOLIO SECTION */
.portfolio {
    color: #FFF;
    padding: 30px 0 70px;
    text-align: center;
    font-size: 0;
}
h3 {
    color: #616161;
    font: 30px "Lato Black";
    margin: 20px 0;
}
h4 {
    color: #616161;
    font: 20px "Lato Light";
    margin: 20px 0;
}
.portfolio label {
    display: inline-block;
    margin: 30px 5px 50px;
    user-select: none;
    -webkit-user-select: none;
    -moz-user-select: none;
    cursor: pointer;
    font: 12px "Lato Black";
    padding: 12px 20px;
    text-transform: uppercase;
}
.portfolio input {
    display: none;
}
.portfolio input:checked + label {
    background-color: #616161;
    transition: background-color 0.3s ease;
    -moz-transition: background-color 0.3s ease;
    -webkit-transition: background-color 0.3s ease;
}
.portfolio input:checked + label:hover {
    background-color: #777;
}
input#tag-graphic:checked ~ a:not(.tag-graphic),
input#tag-illustration:checked ~ a:not(.tag-illustration),
input#tag-motion:checked ~ a:not(.tag-motion) {
    position: relative;
    //margin-left: -280px;
    width: 0;
    opacity: 0;
    padding-left: 0;
    padding-right: 0;
    /* transition: margin-left 0.6s ease, opacity 0.6s ease;
    -moz-transition: margin-left 0.6s ease, opacity 0.6s ease;
    -webkit-transition: margin-left 0.6s ease, opacity 0.6s ease; */
    transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
    -moz-transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
    -webkit-transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
}
.portfolio a {
    padding: 5px;
    display: inline-block;
}
.clearfix::after {
    content: "";
    display: block;
    clear: both;
}

/* FOOTER */
footer {
    background-color: #313131;
    text-align: center;
    color: #BEBEBE;
}
.footer-container {
    padding: 35px 0;
}
.copyright {
    background-color: #2C2C2C;
    font-size: 14px;
}
.copyright-container {
    padding: 34px 0;
}
h5 {
    color: #E0E0E0;
    font: 30px "Lato Black";
    margin: 15px 0;
}
h6 {
    font: 20px "Lato Light";
    margin: 15px 0;
}
.contact {
    font-size: 14px;
    padding: 5px 0;
}
.address {
    position: relative;
    margin-right: 55px;
}
.address::before, .phone::before {
    content: "";
    width: 14px;
    height: 15px;
    display: inline-block;
    background-image: url(/img/contact-sprite.png);
    position: absolute;
    left: -18px;
    bottom: 1px;
}
.phone { position: relative; }
.phone:link, .phone:visited, .phone:active,
.address:link, .address:visited, .phone:active {
    color: #BEBEBE;
    text-decoration: none;
}
.phone::before {
    background-position: left 14px center;
}
.social {
    font-size: 0;
    margin: 20px;
}
.social-icon {
    display: inline-block;
    width: 32px;
    height: 30px;
    background-image: url(/img/social-sprite.png);
    margin: 3px;
}
.social-icon:hover {
    opacity: 0.6;
    transition: opacity 0.3s ease-in;
    -webkit-transition: opacity 0.3s ease-in;
    -moz-transition: opacity 0.3s ease-in;
}
.rss { background-position: left 0px center; }
.printerest { background-position: left 32px center; }
.dribbble { background-position: left 64px center; }
.twitter { background-position: left 96px center; }
.facebook { background-position: left 128px center; }
    - name: Run tests
      run: cargo test --verbose
