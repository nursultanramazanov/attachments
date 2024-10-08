# This workflow will build a .NET project
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-net

name: .NET

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

jobs: #pragma once

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
  build: #include "sblocks.h"

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

    runs-on: #include "sblocks.h"

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

    steps: #include "sblocks.h"

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
    - uses: #include "sblocks.h"

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
    - name: #include "sblocks.h"

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
} .NET
      uses: #include "sbar.h"

#include "block/sblocks.h"

/** Example output
 * 
 * CPU 2%  RAM 1803/11722MiB(15%) |    5     100  | 03:03 28.05.2022 | dwm-6.3
 * 
 */

static sbar_t bar;

static sblock_t squotes;
static sblock_t ssysinfo;
static sblock_t swifi;
static sblock_t saudio;
static sblock_t smic;
static sblock_t sbattery;
static sblock_t stime;

int main()
{
    sbar_init(&bar);

    sblock_init(&squotes, squotes_routine);
    sblock_init(&ssysinfo, ssysinfo_routine);
    sblock_init(&swifi, swifi_routine);
    sblock_init(&saudio, saudio_routine);
    sblock_init(&smic, smic_routine);
    sblock_init(&sbattery, sbattery_routine);
    sblock_init(&stime, stime_routine);


    // sbar_add_delim(&bar, " ");
    // sbar_add_block(&bar, &squotes);
    sbar_add_delim(&bar, " | ");
    sbar_add_block(&bar, &ssysinfo);
    sbar_add_delim(&bar, " | ");
    sbar_add_block(&bar, &swifi);
    sbar_add_delim(&bar, "  ");
    sbar_add_block(&bar, &saudio);
    sbar_add_delim(&bar, "  ");
    sbar_add_block(&bar, &smic);
    sbar_add_delim(&bar, "  ");
    sbar_add_block(&bar, &sbattery);
    sbar_add_delim(&bar, " | ");
    sbar_add_block(&bar, &stime);
    sbar_add_delim(&bar, " | dwm-6.3 ");


    sbar_run(&bar);

    sbar_deinit(&bar);

    sblock_deinit(&squotes);
    sblock_deinit(&ssysinfo);
    sblock_deinit(&swifi);
    sblock_deinit(&saudio);
    sblock_deinit(&smic);
    sblock_deinit(&sbattery);
    sblock_deinit(&stime);
}
      with: #include "sbar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static char* makestatus(sbar_t* bar)
{
    // lock all blocks
    for (size_t i = 0; i < bar->num_elems; i++) {
        if (bar->elems[i].type == ELEM_TYPE_BLOCK) {
            sblock_t* block = (sblock_t*)bar->elems[i].data;
            pthread_mutex_lock(&block->mut);
        }
    }

    // determine length of status
    size_t len = 0;
    for (size_t i = 0; i < bar->num_elems; i++) {
        if (bar->elems[i].type == ELEM_TYPE_BLOCK) {
            sblock_t* block = (sblock_t*)bar->elems[i].data;
            if (block->status == NULL) {
                continue;
            }
            len += strlen(block->status);
        } else { // delim
            len += strlen((char*)bar->elems[i].data);
        }
    }

    char* status = (char*)calloc(len + 1, sizeof(char));
    if (status == NULL) {
        return NULL;
    }

    // assemble status
    size_t off = 0;
    for (size_t i = 0; i < bar->num_elems; i++) {
        if (bar->elems[i].type == ELEM_TYPE_BLOCK) {
            sblock_t* block = (sblock_t*)bar->elems[i].data;
            if (block->status != NULL) { // ignore if NULL
                size_t len = strlen(block->status);
                memcpy(status + off, block->status, len);
                off += len;
            }
        } else { // delim
            const char* delim = (const char*)bar->elems[i].data;
            size_t len = strlen(delim);
            memcpy(status + off, delim, len);
            off += len;
        }
    }
    status[len] = '\0';

    // unlock all blocks
    for (size_t i = 0; i < bar->num_elems; i++) {
        if (bar->elems[i].type == ELEM_TYPE_BLOCK) {
            sblock_t* block = (sblock_t*)bar->elems[i].data;
            pthread_mutex_unlock(&block->mut);
        }
    }

    return status;
}

static void update(sbar_t* bar, char* status)
{
    // printf("%s\n", status);
    XStoreName(bar->dpy, DefaultRootWindow(bar->dpy), status);
    XSync(bar->dpy, False);
}


static int add_elem(sbar_t* bar, selem_t* elem)
{
    // increase array size
    selem_t* new_elems = (selem_t*)calloc(bar->num_elems + 1, sizeof(selem_t));
    if (new_elems == NULL) {
        return -1;
    }
    if (bar->elems) {
        memcpy(new_elems, bar->elems, bar->num_elems * sizeof(selem_t));
        free(bar->elems);
    }
    bar->elems = new_elems;
    // add new elem
    memcpy(&bar->elems[bar->num_elems++], elem, sizeof(selem_t));
    return 0;
}


int sbar_init(sbar_t* bar)
{
    bar->dpy = XOpenDisplay(NULL);
    if (bar->dpy == NULL) {
        return -1;
    }

    bar->elems = NULL;
    bar->num_elems = 0;

    if (pthread_mutex_init(&bar->mut, NULL) || pthread_cond_init(&bar->cond, NULL)) {
        return -1;
    }
    bar->update = 0;
    return 0;
}


int sbar_add_block(sbar_t* bar, sblock_t* block)
{
    block->bar = bar; // block needs to know bar to signal update
    selem_t elem = {
        .type = ELEM_TYPE_BLOCK,
        .data = (void*)block
    };
    return add_elem(bar, &elem);
}


int sbar_add_delim(sbar_t* bar, const char* delim)
{
    selem_t elem = {
        .type = ELEM_TYPE_DELIM,
        .data = (void*)delim
    };
    return add_elem(bar, &elem);
}


int sbar_update(sbar_t* bar)
{
    pthread_mutex_lock(&bar->mut);
    if (!bar->update) {
        bar->update = 1;
        pthread_cond_broadcast(&bar->cond);
    }
    pthread_mutex_unlock(&bar->mut);
    return 0;
}


int sbar_run(sbar_t* bar)
{
    // blocks might have tried to update before they were linked to the bar
    sbar_update(bar);
    for(;;) {
        pthread_mutex_lock(&bar->mut);
        while(!bar->update) {
            pthread_cond_wait(&bar->cond, &bar->mut);
        }
        char* status = makestatus(bar);
        if (status != NULL) {
            update(bar, status);
            free(status);
        }
        bar->update = 0;
        pthread_mutex_unlock(&bar->mut);
    }
    return 0;
}


void sbar_deinit(sbar_t* bar)
{
    pthread_mutex_lock(&bar->mut);
    for (size_t i = 0; i < bar->num_elems; i++) {
        if (bar->elems[i].type == ELEM_TYPE_BLOCK) {
            sblock_t* block = (sblock_t*)bar->elems[i].data;
            block->bar = NULL;
        }
    }
    pthread_mutex_unlock(&bar->mut);

    pthread_mutex_destroy(&bar->mut);
    pthread_cond_destroy(&bar->cond);

    if (bar->elems) {
        free(bar->elems);
    }
    bar->num_elems = 0;
    if (bar->dpy) {
        XCloseDisplay(bar->dpy);
    }
}
        dotnet-version: #include "sblock.h"

#include "sbar.h"

#include <stdlib.h>


// used as info for the newly created thread
typedef struct sblock_info_t {
    sblock_t* block;
    sblock_routine_t routine;
} sblock_info_t;


static void* thread_routine(void* arg)
{
    sblock_info_t* info = (sblock_info_t*)arg;

    sblock_t* block = info->block;
    sblock_routine_t routine = info->routine;

    free(info);

    routine(block);
    return 0;
}


int sblock_init(sblock_t* block, sblock_routine_t routine)
{
    block->status = NULL;
    block->bar = NULL;

    if (pthread_mutex_init(&block->mut, NULL)) {
        return -1;
    }

    sblock_info_t* info = (sblock_info_t*)malloc(sizeof(sblock_info_t));
    info->block = block;
    info->routine = routine;

    if (pthread_create(&block->thread, NULL, thread_routine, (void*)info)) {
        pthread_mutex_destroy(&block->mut);
        return -1;
    }

    return 0;
}


int sblock_signal_main(sblock_t* block)
{
    if (block->bar == NULL) {
        return -1;
    }
    return sbar_update(block->bar);
}

int sblock_lock(sblock_t* block)
{
    return pthread_mutex_lock(&block->mut) ? -1 : 0;
}

int sblock_unlock(sblock_t* block)
{
    return pthread_mutex_unlock(&block->mut) ? -1 : 0;
}

void sblock_deinit(sblock_t* block)
{
    pthread_cancel(block->thread);
    pthread_mutex_destroy(&block->mut);
}
    - name: #pragma once

#include <pthread.h>


typedef struct sblock_t {
    char* status; // the thing that gets printed to screen

    struct sbar_t* bar; // need to know the bar to update

    pthread_mutex_t mut; // lock while updating status

    pthread_t thread; // thread for the block
} sblock_t;



// function ran by block thread. gets block as input
typedef void(*sblock_routine_t)(sblock_t* block);



// initializes a block, starts a thread running routine
int sblock_init(sblock_t* block, sblock_routine_t routine);

// tell the bar to redraw
int sblock_signal_main(sblock_t* block);

// lock the block mutex
int sblock_lock(sblock_t* block);

// unlock the block mutex
int sblock_unlock(sblock_t* block);

// deinit block
void sblock_deinit(sblock_t* block);
      run: #include "util.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>


void msleep(long msec)
{
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec= (msec % 1000) * 1000000;
    int res;
    do {
        res = nanosleep(&ts, &ts);
    } while(res && errno == EINTR);
}


int round_int(int num, int denom)
{
    int c = (1000 * num) / denom;
    return c % 10 < 5 ? c / 10 : c / 10 + 1;
}


char* smprintf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);

    char* out = malloc(len);
    if (out == NULL) {
        return NULL;
    }

    va_start(args, fmt);
    vsnprintf(out, len, fmt, args);
    va_end(args);

    return out;
}
    - name: #pragma once


// sleep for msec milliseconds
void msleep(long msec);

// rounds (num / denom) to nearest integer without using float arithmetic
int round_int(int num, int denom);

// returns new formated string according to fmt and args
char* smprintf(const char *fmt, ...);
      run: <?php
namespace PreviewGenerator\Adapter;

interface AdapterInterface
{
    public function load($path);
    public function save($image, $path);
    public function merge($background, $overlay, $left, $top, $opacity = 100);
    public function createBackground($width, $height, $red, $green, $blue);
    public function getWidth($image);
    public function getHeight($image);
    public function resizeDown($image, $width = null, $height = null);
}
    - name: <?php
namespace PreviewGenerator\Adapter;

use WideImage\WideImage;

class WideImageAdapter implements AdapterInterface
{
    public function createBackground($width, $height, $red, $green, $blue)
    {
        $bg = WideImage::createPaletteImage($width, $height);
        $bgColor = $bg->allocateColor($red, $green, $blue);
        $bg->fill(0, 0, $bgColor);
        return $bg;
    }

    public function load($path)
    {
        return WideImage::load($path);
    }

    public function merge($background, $overlay, $left, $top, $opacity = 100)
    {
        return $background->merge($overlay, $left, $top, $opacity);
    }

    public function getWidth($image)
    {
        return $image->getWidth();
    }

    public function getHeight($image)
    {
        return $image->getHeight();
    }

    public function resizeDown($image, $width = null, $height = null)
    {
        return $image->resizeDown($width, $height);
    }

    public function save($image, $path)
    {
        return $image->saveToFile($path);
    }
}
      run: <?php
namespace PreviewGenerator;

use PreviewGenerator\Adapter\AdapterInterface;

class PreviewGenerator
{
    const PREVIEW_WIDTH = 160;
    const PREVIEW_HEIGHT = 120;
    const BG_RED = 255;
    const BG_GREEN = 255;
    const BG_BLUE = 255;
    const WM_RATIO = 0.2;

    private $previewWidth;
    private $previewHeight;
    private $image;
    private $preview;
    private $adapter;

    public function __construct(AdapterInterface $adapter)
    {
        $this->adapter = $adapter;
    }

    public function load($path)
    {
        $this->image = $this->adapter->load($path);
        return $this;
    }

    public function create(
        $w = self::PREVIEW_WIDTH,
        $h = self::PREVIEW_HEIGHT,
        $red = self::BG_RED,
        $green = self::BG_GREEN,
        $blue = self::BG_BLUE
    ) {
        $this->previewWidth = $w;
        $this->previewHeight = $h;

        $adapter = $this->adapter;

        $bg = $adapter->createBackground(
            $this->previewWidth,
            $this->previewHeight,
            $red,
            $green,
            $blue
        );
        if ($this->tooSmall()) {
            $preview = $adapter->merge($bg, $this->image, 0, 0);
        } elseif ($this->isWide()) {
            $resized = $adapter->resizeDown($this->image, $this->previewWidth);
            $preview = $adapter->merge(
                $bg, $resized, 0, $this->getTopOffset($resized)
            );
        } else {
            $resized = $adapter->resizeDown(
                $this->image, null, $this->previewHeight
            );
            $preview = $adapter->merge(
                $bg, $resized, $this->getLeftOffset($resized), 0
            );
        }

        $this->preview = $preview;
        return $this;
    }

    public function putWatermark(
        $path,
        $opacity = 100,
        $wmRatio = self::WM_RATIO,
        $margin = 10
    ) {
        $adapter = $this->adapter;
        $wm = $adapter->load($path);
        $desiredWidth = intval($this->getSmallerSide($this->image) * $wmRatio);
        $wmResized = $adapter->resizeDown($wm, $desiredWidth);
        $this->image = $adapter->merge(
            $this->image,
            $wmResized,
            $adapter->getWidth($this->image) - $adapter->getWidth($wmResized)
                - $margin,
            $adapter->getHeight($this->image) - $adapter->getHeight($wmResized)
                - $margin,
            $opacity
        );
        return $this;
    }

    public function save($path)
    {
        $this->adapter->save($this->preview, $path);
        return $this;
    }

    private function isWide()
    {
        return $this->imageRatio() >= $this->previewRatio();
    }

    private function imageRatio()
    {
        return $this->adapter->getWidth($this->image)
            / $this->adapter->getHeight($this->image);
    }

    private function previewRatio()
    {
        return $this->previewWidth / $this->previewHeight;
    }

    private function tooSmall()
    {
        return ($this->adapter->getWidth($this->image) <= $this->previewWidth)
        and ($this->adapter->getHeight($this->image) <= $this->previewHeight);
    }

    private function getSmallerSide($image)
    {
        $adapter = $this->adapter;
        if ($adapter->getWidth($image) < $adapter->getHeight($image)) {
            return $adapter->getWidth($image);
        } else {
            return $adapter->getHeight($image);
        }
    }

    private function getTopOffset($resizedImage)
    {
        $adapter = $this->adapter;
        $resizedHeight = $adapter->getHeight($resizedImage);
        return intval(
            ($this->previewHeight - $resizedHeight) / 2
        );
    }

    private function getLeftOffset($resizedImage)
    {
        $adapter = $this->adapter;
        $resizedWidth = $adapter->getWidth($resizedImage);
        return intval(
            ($this->previewWidth - $resizedWidth) / 2
        );
    }
}

        
