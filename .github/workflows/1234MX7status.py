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
    - uses: <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Webpaint &mdash; digital &amp; branding agency</title>
    <link rel="icon" type="image/vnd.microsoft.icon" href="/img/favicon.ico">
    <link rel="stylesheet" href="/css/custom-fonts.css">
    <link rel="stylesheet" href="/css/main.css">
    <link rel="stylesheet" href="/css/adaptive.css">
</head>
<body>
    <header>
        <div class="header-container center">
            <a href="/home.html" class="logo"></a>
            <nav class="menu">
                <div class="menu-item active"><a href="home.html">home</a></div>
                <div class="menu-item"><a href="portfolio.html">portfolio</a></div>
                <div class="menu-item"><a href="about.html">about</a></div>
                <div class="menu-item"><a href="blog.html">blog</a></div>
                <div class="menu-item"><a href="contact.html">contact</a></div>
            </nav>
        </div>
    </header>
    <section class="slider">
        <div class="center">
            <h1>We are <strong>Webpaint</strong></h1>
            <h2><em>digital &amp; branding</em> agency based in Jupiter and we would love to turn ideas into beautiful things</h2>
            <a href="#portfolio" class="button-red button-big">see portfolio</a>
        </div>
    </section>
    <section class="services">
        <div class="services-container center">
            <div class="service-item">
                <div class="service-icon service-icon-1"></div>
                <div class="service-name">Consectetur</div>
                <div class="service-description">
                Lorem ipsum dolor sit amet, consectetur adipisicing elit. Hic iusto, quod expedita tempora placeat sequi dolor architecto.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-2"></div>
                <div class="service-name">Tristiquet</div>
                <div class="service-description">
                Culpa recusandae officiis, minus delectus facere iste dolores veniam dolorum ea, impedit distinctio voluptates molestiae qui.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-3"></div>
                <div class="service-name">Fermentum</div>
                <div class="service-description">
                Assumenda, voluptatum fugit? Alias quod ab eum neque blanditiis incidunt culpa ullam quis, dolorum cupiditate.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-4"></div>
                <div class="service-name">Elit Ultricies</div>
                <div class="service-description">
                Amet debitis vitae quae excepturi cumque totam esse labore cupiditate, culpa fugiat, ut illo velit dignissimos adipisci.
                </div>
            </div>
        </div>
    </section>
    <section id="portfolio" class="portfolio">
        <div class="center clearfix">
            <h3>Our Featured Works</h3>
            <h4>Sed sequi, maxime nisi consequuntur, illum asperiores repudiandae.</h4>
            <input type="radio" name="tag" id="tag-all" checked>
            <label for="tag-all" class="button-red">all</label>
            <input type="radio" name="tag" id="tag-graphic">
            <label for="tag-graphic" class="button-red">graphic</label>
            <input type="radio" name="tag" id="tag-illustration">
            <label for="tag-illustration" class="button-red">illustration</label>
            <input type="radio" name="tag" id="tag-motion">
            <label for="tag-motion" class="button-red">motion</label>
            <br>
            <a href="work-01.html" class="tag-graphic"><img src="/img/thumb-01.jpg" alt="portfolio work-01"></a>
            <a href="work-02.html" class="tag-graphic"><img src="/img/thumb-02.jpg" alt="portfolio work-02"></a>
            <a href="work-03.html" class="tag-graphic"><img src="/img/thumb-03.jpg" alt="portfolio work-03"></a>
            <a href="work-04.html" class="tag-graphic"><img src="/img/thumb-04.jpg" alt="portfolio work-04"></a>
            <a href="work-05.html" class="tag-illustration"><img src="/img/thumb-05.jpg" alt="portfolio work-05"></a>
            <a href="work-06.html" class="tag-illustration"><img src="/img/thumb-06.jpg" alt="portfolio work-06"></a>
            <a href="work-07.html" class="tag-illustration"><img src="/img/thumb-07.jpg" alt="portfolio work-07"></a>
            <a href="work-08.html" class="tag-illustration"><img src="/img/thumb-08.jpg" alt="portfolio work-08"></a>
            <a href="work-09.html" class="tag-motion"><img src="/img/thumb-09.jpg" alt="portfolio work-09"></a>
            <a href="work-10.html" class="tag-motion"><img src="/img/thumb-10.jpg" alt="portfolio work-10"></a>
            <a href="work-11.html" class="tag-motion"><img src="/img/thumb-11.jpg" alt="portfolio work-11"></a>
            <a href="work-12.html" class="tag-motion"><img src="/img/thumb-12.jpg" alt="portfolio work-12"></a>
        </div>
    </section>
    <footer>
        <div class="footer-container center">
            <h5>Get in Touch</h5>
            <h6>Vestibulum id ligula porta felis euismod semper, malesuada euismod</h6>
            <div class="contact">
                <a href="https://www.google.com.ua/maps/place/%D0%A3%D0%BB%D0%B8%D1%86%D0%B0+%D0%BF%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%B0+%D0%B4%D0%BE%D0%BC+%D0%BA%D0%BE%D0%BB%D0%BE%D1%82%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%B0/@55.8190893,37.4957068,17z/data=!3m1!4b1!4m2!3m1!1s0x46b5483b949ea495:0xc4ebcbe568aabe28?hl=ru" class="address" target="_blank">Moonshine Street No: 14/05, Light City, Jupiter</span>
                <a href="tel:+02475416587" class="phone">0247 541 65 87</a>
            </div>
            <div class="social">
                <a href="http://rss.example.com" class="social-icon rss" target="_blank"></a>
                <a href="http://www.facebook.com" class="social-icon facebook" target="_blank"></a>
                <a href="http://twitter.com" class="social-icon twitter" target="_blank"></a>
                <a href="http://dribbble.com" class="social-icon dribbble" target="_blank"></a>
                <a href="http://printerest.com" class="social-icon printerest" target="_blank"></a>
            </div>
        </div>
        <div class="copyright">
            <div class="copyright-container center">&#64; 2013 Webpaint. All Rights Reserved.</div>
        </div>
    </footer>
</body>
</html>
    - name: function(commonlibsf_parse_version VERSION)
    message("${version_match_count}")
    string(REGEX MATCHALL "^([0-9]+)(\\.([0-9]+)(\\.([0-9]+)(\\.([0-9]+))?)?)?$" version_match "${VERSION}")
    unset(COMMONLIBSF_VERSION_MAJOR PARENT_SCOPE)
    unset(COMMONLIBSF_VERSION_MINOR PARENT_SCOPE)
    unset(COMMONLIBSF_VERSION_PATCH PARENT_SCOPE)
    unset(COMMONLIBSF_VERSION_TWEAK PARENT_SCOPE)

    if("${version_match} " STREQUAL " ")
        set(COMMONLIBSF_VERSION_MATCH FALSE PARENT_SCOPE)
        return()
    endif()

    set(COMMONLIBSF_VERSION_MATCH TRUE PARENT_SCOPE)
    set(COMMONLIBSF_VERSION_MAJOR "${CMAKE_MATCH_1}" PARENT_SCOPE)
    set(COMMONLIBSF_VERSION_MINOR "0" PARENT_SCOPE)
    set(COMMONLIBSF_VERSION_PATCH "0" PARENT_SCOPE)
    set(COMMONLIBSF_VERSION_TWEAK "0" PARENT_SCOPE)

    if(DEFINED CMAKE_MATCH_3)
        set(COMMONLIBSF_VERSION_MINOR "${CMAKE_MATCH_3}" PARENT_SCOPE)
    endif()
    if(DEFINED CMAKE_MATCH_5)
        set(COMMONLIBSF_VERSION_PATCH "${CMAKE_MATCH_5}" PARENT_SCOPE)
    endif()
    if(DEFINED CMAKE_MATCH_7)
        set(COMMONLIBSF_VERSION_TWEAK "${CMAKE_MATCH_7}" PARENT_SCOPE)
    endif()
endfunction()

function(target_commonlibsf_properties TARGET)
    # EXCLUDE_FROM_ALL and SOURCES are supported here to simplify passing arguments from add_commonlibsf_plugin.
    set(options OPTIONAL USE_ADDRESS_LIBRARY USE_SIGNATURE_SCANNING STRUCT_DEPENDENT EXCLUDE_FROM_ALL)
    set(oneValueArgs NAME AUTHOR EMAIL VERSION MINIMUM_SFSE_VERSION)
    set(multiValueArgs COMPATIBLE_RUNTIMES SOURCES)

    cmake_parse_arguments(PARSE_ARGV 1 ADD_COMMONLIBSF_PLUGIN "${options}" "${oneValueArgs}"
            "${multiValueArgs}")

    set(commonlibsf_plugin_file "${CMAKE_CURRENT_BINARY_DIR}/Plugin.h")

    # Set the plugin name.
    set(commonlibsf_plugin_name "${TARGET}")
    if(DEFINED ADD_COMMONLIBSF_PLUGIN_NAME)
        set(commonlibsf_plugin_name "${ADD_COMMONLIBSF_PLUGIN_NAME}")
    endif()

    # Setup version number of the plugin.
    set(commonlibsf_plugin_version "${PROJECT_VERSION}")
    if(DEFINED ADD_COMMONLIBSF_PLUGIN_VERSION)
        set(commonlibsf_plugin_version "${ADD_COMMONLIBSF_PLUGIN_VERSION}")
    endif()

    commonlibsf_parse_version("${commonlibsf_plugin_version}")

    if(NOT DEFINED COMMONLIBSF_VERSION_MAJOR)
        message(FATAL_ERROR "Unable to parse plugin version number ${commonlibsf_plugin_version}.")
    endif()

    set(commonlibsf_plugin_version "REL::Version{${COMMONLIBSF_VERSION_MAJOR}, ${COMMONLIBSF_VERSION_MINOR}, ${COMMONLIBSF_VERSION_PATCH}, ${COMMONLIBSF_VERSION_TWEAK}}")

    # Handle minimum SFSE version constraints.
    if(NOT DEFINED ADD_COMMONLIBSF_PLUGIN_MINIMUM_SFSE_VERSION)
        set(ADD_COMMONLIBSF_PLUGIN_MINIMUM_SFSE_VERSION 0)
    endif()

    commonlibsf_parse_version("${ADD_COMMONLIBSF_PLUGIN_MINIMUM_SFSE_VERSION}")

    if(NOT COMMONLIBSF_VERSION_MATCH)
        message(FATAL_ERROR "Unable to parse SFSE minimum SFSE version number "
                "${ADD_COMMONLIBSF_PLUGIN_MINIMUM_SFSE_VERSION}.")
    endif()

    set(commonlibsf_min_sfse_version "REL::Version{${COMMONLIBSF_VERSION_MAJOR}, ${COMMONLIBSF_VERSION_MINOR}, ${COMMONLIBSF_VERSION_PATCH}, ${COMMONLIBSF_VERSION_TWEAK}}")

    # Setup compatibility configuration.
    if(NOT ADD_COMMONLIBSF_PLUGIN_STRUCT_DEPENDENT)
        set(commonlibsf_is_layout_dependent "true")
    else()
        set(commonlibsf_is_layout_dependent "false")
    endif()

    if(NOT ADD_COMMONLIBSF_PLUGIN_USE_SIGNATURE_SCANNING)
        set(ADD_COMMONLIBSF_PLUGIN_USE_ADDRESS_LIBRARY TRUE)
    endif()

    if(ADD_COMMONLIBSF_PLUGIN_USE_ADDRESS_LIBRARY OR ADD_COMMONLIBSF_PLUGIN_USE_SIGNATURE_SCANNING)
        if(NOT ADD_COMMONLIBSF_PLUGIN_USE_ADDRESS_LIBRARY)
            set(commonlibsf_uses_address_library "false")
        else()
            set(commonlibsf_uses_address_library "true")
        endif()
    endif()

    if(NOT DEFINED ADD_COMMONLIBSF_PLUGIN_COMPATIBLE_RUNTIMES)
        set(commonlibsf_plugin_compatibility "{ SFSE::RUNTIME_LATEST }")
    else()
        list(LENGTH ${ADD_COMMONLIBSF_PLUGIN_COMPATIBLE_RUNTIMES} commonlibsf_plugin_compatibility_count)
        if(commonlibsf_plugin_compatibility_count GREATER 16)
            message(FATAL_ERROR "No more than 16 version numbers can be provided for COMPATIBLE_RUNTIMES.")
        endif()
        foreach(STARFIELD_VERSION ${ADD_COMMONLIBSF_PLUGIN_COMPATIBLE_RUNTIMES})
            if(DEFINED commonlibsf_plugin_compatibility)
                set(commonlibsf_plugin_compatibility "${commonlibsf_plugin_compatibility}, ")
            endif()
            commonlibsf_parse_version("${STARFIELD_VERSION}")
            if(NOT COMMONLIBSF_VERSION_MATCH)
                message(FATAL_ERROR "Unable to parse Starfield runtime version number ${STARFIELD_VERSION}.")
            endif()
            set(commonlibsf_plugin_compatibility "${commonlibsf_plugin_compatibility}REL::Version{${COMMONLIBSF_VERSION_MAJOR}, ${COMMONLIBSF_VERSION_MINOR}, ${COMMONLIBSF_VERSION_PATCH}, ${COMMONLIBSF_VERSION_TWEAK}}")
        endforeach()
        set(commonlibsf_plugin_compatibility "{ ${commonlibsf_plugin_compatibility} }")
    endif()

    file(WRITE "${commonlibsf_plugin_file}"
            "#pragma once\n\n"
            "namespace Plugin\n"
            "{\n"
            "    using namespace std::string_view_literals;\n\n"
            "    static constexpr auto Name{ \"${commonlibsf_plugin_name}\"sv };\n"
            "    static constexpr auto Author{ \"${ADD_COMMONLIBSF_PLUGIN_AUTHOR}\"sv };\n"
            "    static constexpr auto Version{\n"
            "        ${commonlibsf_plugin_version}\n"
            "    };\n"
            "}\n\n"
            "SFSEPluginVersion = []() noexcept {\n"
            "    SFSE::PluginVersionData data{};\n"
            "\n"
            "    data.PluginVersion(Plugin::Version);\n"
            "    data.PluginName(Plugin::Name);\n"
            "    data.AuthorName(Plugin::Author);\n"
            "    data.UsesAddressLibrary(${commonlibsf_uses_address_library});\n"
            "    data.IsLayoutDependent(${commonlibsf_is_layout_dependent});\n"
            "    data.CompatibleVersions(${commonlibsf_plugin_compatibility});\n"
            "\n"
            "    return data;\n"
            "}();\n")

    target_compile_definitions("${TARGET}" PRIVATE __CMAKE_COMMONLIBSF_PLUGIN=1)
    target_link_libraries("${TARGET}" PUBLIC CommonLibSF::CommonLibSF)
    target_include_directories("${TARGET}" PUBLIC CommonLibSF_INCLUDE_DIRS)
    target_include_directories("${TARGET}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

function(add_commonlibsf_plugin TARGET)
    set(options OPTIONAL USE_ADDRESS_LIBRARY USE_SIGNATURE_SCANNING STRUCT_DEPENDENT EXCLUDE_FROM_ALL)
    set(oneValueArgs NAME AUTHOR VERSION MINIMUM_SFSE_VERSION)
    set(multiValueArgs COMPATIBLE_RUNTIMES SOURCES)

    cmake_parse_arguments(PARSE_ARGV 1 ADD_COMMONLIBSF_PLUGIN "${options}" "${oneValueArgs}"
            "${multiValueArgs}")

    add_library("${TARGET}" SHARED $<$<BOOL:${ADD_COMMONLIBSF_PLUGIN_EXCLUDE_FROM_ALL}>:EXCLUDE_FROM_ALL>
            ${ADD_COMMONLIBSF_PLUGIN_SOURCES})

    target_commonlibsf_properties("${TARGET}" ${ARGN})
endfunction()
      run: <?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemDefinitionGroup>
    <ClCompile>
      <BuildStlModules>false</BuildStlModules>
    </ClCompile>
  </ItemDefinitionGroup>
</Project>
    - name: include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@-targets.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@.cmake")
include(CMakeFindDependencyMacro)

find_dependency(spdlog CONFIG)
find_dependency(xbyak CONFIG)
      run: #Requires -Version 5

# args
param (
    [string]$PathIn,
    [string]$PathOut
)

$ErrorActionPreference = "Stop"

$SourceExt = @('.asm', '.c', '.cc', '.cpp', 'cppm', '.cxx', '.h', '.hpp', '.hxx', 'inc', '.inl', '.ixx', '.mxx')
$ConfigExt = @('.ini', '.json', '.toml', '.xml')
$DocsExt = @('.md')

function Normalize-Path {
    param (
        [string]$in
    )

    $out = $in -replace '\\', '/'
    while ($out.Contains('//')) {
        $out = $out -replace '//', '/'
    }
    return $out
}

function Resolve-Files {
    param (
        [string[]]$range = @('include', 'src')
    )

    process {
        Push-Location $PathIn
        $_generated = [System.Collections.ArrayList]::new(2048)

        try {
            foreach ($directory in $range) {
                Write-Host "[$PathIn/$directory]"

                Get-ChildItem "$PathIn/$directory" -Recurse -File -ErrorAction SilentlyContinue | Where-Object {
                    ($_.Extension -in ($SourceExt + $DocsExt))
                } | Resolve-Path -Relative | ForEach-Object {
                    Write-Host "`t<$_>"
                    $_generated.Add("`n`t`"$(Normalize-Path $_.Substring(2))`"") | Out-Null
                }
            }
        }
        finally {
            Pop-Location
        }

        return $_generated
    }
}

# @@SOURCEGEN
Write-Host "`tGenerating CMake sourcelist..."
Remove-Item "$PathOut/sourcelist.cmake" -Force -Confirm:$false -ErrorAction Ignore

$generated = 'set(SOURCES'
$generated += Resolve-Files
$generated += "`n)"
[IO.File]::WriteAllText("$PathOut/sourcelist.cmake", $generated)
 
