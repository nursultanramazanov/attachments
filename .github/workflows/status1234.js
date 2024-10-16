name: C/C++ CI

on: cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css ../css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js ../js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot ../fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg ../fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf ../fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff ../fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 ../fonts/glyphicons-halflings-regular.woff2;

cp vendor/select2/select2/select2.js ../js/select2.js;
cp vendor/select2/select2/select2_locale_ru.js ../js/select2_locale_ru.js;
cp vendor/select2/select2/select2.css ../css/select2.css;
cp vendor/select2/select2/select2.png ../css/select2.png;
cp vendor/select2/select2/select2-spinner.gif ../css/select2-spinner.gif;

cp vendor/silverfire/select2-bootstrap3-css/select2-bootstrap.min.css ../css/select2-bootstrap.min.css;
  push: /*
@author:t0pl
This tool isn't affiliated to SchoolMouv in any way
*/
try { var browser = chrome } catch {}

window.headers_ = {
    'Accept': '*/*',
    'Accept-Encoding': 'gzip, deflate, br',
    'Accept-Language': 'en-US,en;q=0.9',
    'DNT': '1',
    'Referer': '',
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4130.0 Safari/537.36'
}

const clean_url = (url) => {
    return url.replace('/eleves','').split("#")[0].split("?")[0]
}

const is_website_valid = (url) => {
    return url.startsWith("https://www.schoolmouv.fr") && url.split('/').length >= 6;
}

const see_in_new_tab = (final_hidden_resource) => {
    browser.tabs.create({ "url": final_hidden_resource })
}

const isFirefox = () => {
    return typeof browser.webRequest.getSecurityInfo !== "undefined"
}

const resource_type = () => {
    if (window.url.endsWith('cours-video')) return "video"
    if (!window.url.includes("/qcm") && !window.url.includes("/exercice") && window.url.split('/').length) return "pdf"
    return ""
}
    branches: [ "main" ]
  pull_request: /*
@author:t0pl
This tool isn't affiliated to SchoolMouv in any way
*/
const get_pdf = () => {
    see_in_new_tab(url_to_direct_pdf_link())
}

const url_to_direct_pdf_link = () => {
    return window.url.replace("www.schoolmouv.fr", "pdf-schoolmouv.s3.eu-west-1.amazonaws.com") + ".pdf";
}
    branches: [ "main" ]

jobs: /*
@author:t0pl
This tool isn't affiliated to SchoolMouv in any way
*/

/* Error handling */

function check_status_code(response) {

    if (!response.ok) (console.warn(`${url} returned wrong status code: ${response.status}`));
    return response.text();

}

/* Requests */
const vimeo_player = (url) => {
    if (url) {
        set_listener();
        fetch(url).then(check_status_code)
            .then(function (res) {
                //Locate part of response containing mp4s
                for (const part of res.split('};')) {
                    if (part.includes('.mp4')) {

                        const cleared_JSON = vimeo_JSON(part);
                        const all_mp4s_found = direct_links(cleared_JSON);

                        console.log(all_mp4s_found);

                        //Display results in new tab
                        see_in_new_tab(all_mp4s_found[0])
                    }
                }
            })
            .catch(function (err) {
                console.error(`${err} :: ${url}`);
            }).finally(() => remove_listener());
    }
}

const get_video = () => {
    set_referer_in_headers()

    //Fetch schoolmouv webpage
    fetch(window.url)
        .then(check_status_code)
        .then(function (data) {

            const clear_json = schoolmouv_JSON(data);
            const urls = source_2_Vimeo_URL(clear_json);

            console.log(urls);

            //Fetch all vimeo webpages
            for (const to_get of urls) {
                vimeo_player(to_get)
            }
        }).catch(function (err) {
            console.error('Error during request: ', err);
        })
}

/* Parsing */
const vimeo_JSON = (part) => {

    //Locate JSON in response and Convert from Vimeo WebPage
    let located_json = part.split('= {')[1];
    let cleared_json = JSON.parse(`{${located_json}}`);

    return cleared_json;
}

const schoolmouv_JSON = (res) => {

    //Locate JSON in response and Convert from Schoolmouv WebPage
    const to_json = `{${res.split('window.__INITIAL_STATE__={')[1].split('};(function(')[0]}}`;
    const clear_json = JSON.parse(to_json);

    return clear_json;
}

const source_2_Vimeo_URL = (clear_json) => {
    var first_step_urls = [];
    let parsed_data = clear_json.sheet.resources;

    Object.keys(parsed_data).forEach(function (key) {

        let source = parsed_data[key].source;
        first_step_urls.push(`https://player.vimeo.com/video/${source}`);
    })

    if (first_step_urls === []) {
        console.warn("'Source id' not found")
        return []
    }
    return first_step_urls
}

const key_to_get_source_id = () => {
    //Needed if registered user, to find right key in {}
    return window.url.split('/').splice(-2).join('-'); //des-cartes-pour-comprendre-le-monde-cours-video
}

const direct_links = (cleared_JSON) => {
    let direct_links = [];
    for (var _ = 0; _ < cleared_JSON.request.files.progressive.length; _++) {
        direct_links.push(cleared_JSON.request.files.progressive[_].url);
    }
    return direct_links;
}

/* Header stuff */
const set_referer_in_headers = () => {
    window.headers_['Referer'] = window.url;
}

const set_listener = () => {
    browser.webRequest.onBeforeSendHeaders.addListener(
        onBeforeSendHeaders_callback, { urls: ["https://player.vimeo.com/*"] }, OnBeforeRequestOptions()
    );
}

const remove_listener = () => {
    browser.webRequest.onBeforeSendHeaders.removeListener(onBeforeSendHeaders_callback);
}

const modify_headers = (header_array, _name, _value) => { // Credits: https://stackoverflow.com/a/11602753
    var did_set = false;
    for (var i in header_array) {
        var header = header_array[i];
        var name = header.name;
        if (name == _name) {
            header.value = _value;
            did_set = true;
        }
    }
    if (!did_set) header_array.push({ name: _name, value: _value })
}

const onBeforeSendHeaders_callback = (details) => {
    //Fired to modify request
    Object.keys(window.headers_).forEach(function (key) {
        modify_headers(details.requestHeaders, key, window.headers_[key]);
    });

    return { requestHeaders: details.requestHeaders };
}

const OnBeforeRequestOptions = () => {
    //Options differ in Chrome/Firefox
    return isFirefox() ? ['blocking', 'requestHeaders'] : ['blocking', 'requestHeaders', 'extraHeaders']
}
  build: """
@author:t0pl
Warning: gibberish code
This tool isn't affiliated to SchoolMouv in any way
"""
import requests
from bs4 import BeautifulSoup
import json
import os
import webbrowser


class resource:
    """Parent class
    Argument : url (str)
    Functions : validate -> bool : validation tests
                download -> None : download 'url' into 'into' (folder) as 'save_as' (filename, default:None)
                relevant_filename -> str : suggests a nice formatted filename
    """

    def __init__(self, url: str):
        self.url = url.split("#")[0].split("?")[0]

    def validate(self, url: str):
        # no to be used for security improvement
        pass

    def download(self, url: str, into: str, save_as=None, overwrite=False):
        if type(url) != str:
            print(f'URL needs to be a string, not a {type(url).__name__}')
            return False
        if not os.path.exists(into):
            print(
                f'No such folder, consider changing the value passed in the \'into\' parameter')
            return False
        assert self.validate(url)
        _ = requests.get(url)
        assert _.status_code == 200
        save_as = self.relevant_filename(url) if save_as == None else save_as
        abs_path = os.path.abspath(os.path.join(into, save_as))
        if os.path.exists(abs_path) and not overwrite:
            print("File already exists, set overwrite to True to overwrite it anyway")
            return False
        with open(abs_path, 'wb') as file_to_write:
            file_to_write.write(_.content)

    def relevant_filename(self, url: str):
        _ = ''
        __ = url.split('/')[-2].capitalize().replace('-', ' ')
        already_done = 0
        for caract in __:
            if already_done == 0 and caract.isnumeric():
                _ += ' '+caract
                already_done += 1
            else:
                _ += caract
        _ = _.replace('  ', ' ')
        return _+'.pdf' if url.endswith('.pdf') else _+'.mp4'

    def see(self, url: str):
        success = webbrowser.open_new_tab(url)
        if not success:
            return success


class video(resource):
    """
    Argument : url (str), for eg: https://www.schoolmouv.fr/cours/little-red-riding-hood/cours-video
    Functions : extract_json
                get_source
                get_direct_links
                get_soup
                ----------------
                validate
                download
    """

    def __init__(self, url: str):
        super().__init__(url)
        self.url = url
        self.useful_headers = {
            'Accept': '*/*',
            'Accept-Encoding': 'gzip, deflate, br',
            'Accept-Language': 'en-US,en;q=0.9,zh-CN;q=0.6,zh;q=0.5',
            'DNT': '1',
            'Referer': self.url,
            'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4130.0 Safari/537.36'
        }
        self.basic_headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4130.0 Safari/537.36 Edg/84.0.502.0'
        }

    def get_soup(self, url, headers={}):
        headers = self.basic_headers if headers == {} else headers
        for _ in range(5):
            po = requests.get(url, headers=headers)
            if po.status_code == 200:
                break
        return BeautifulSoup(po.content, 'html.parser')

    def extract_json(self, mess_up):
        mess_up = str(mess_up)
        mess_up = mess_up.split('};')[0]+'}'
        try:
            mess_up = '{'+mess_up.split('= {')[1]
        except IndexError:
            mess_up = '{'+mess_up.split('={')[1]
        mess_up = json.loads(mess_up)
        return mess_up

    def get_source(self, mess_up):
        assert 'sheet' in mess_up.keys()
        _r = []
        for _ in mess_up['sheet']['resources']:
            _source = mess_up['sheet']['resources'][_]['source']
            _r.append(_source)
        return list(set(_r))

    def get_direct_links(self, json_):
        # print(json_)
        assert 'request' in json_.keys()
        return list(set([i['url'] for i in json_['request']['files']['progressive']]))
        #keys : ['profile', 'width', 'mime', 'fps', 'url', 'cdn', 'quality', 'id', 'origin', 'height']

    def run(self):
        # 1st part
        # GET https://www.schoolmouv.fr/cours/little-red-riding-hood/cours-video
        soup = self.get_soup(self.url)
        script_tag_containing_json = [
            i for i in soup.find_all('script') if str(i).count("{") > 9]
        #assert len(script_tag_containing_json)==1
        # parsing json from <script> tag
        self.script_tag_containing_json = self.extract_json(
            script_tag_containing_json[0])
        # extract unique 9-digit id, different for each video
        sources = self.get_source(self.script_tag_containing_json)

        # 2nd part
        for _source in sources:
            assert len(_source) == 9 and _source.isalnum()
            # GET https://player.vimeo.com/video/9-DIGIT_ID?app_id=schoolmouv_app_id
            soup__ = self.get_soup(
                f'https://player.vimeo.com/video/{_source}', headers=self.useful_headers)  # ?app_id=122963
            script_tag_containing_mp4 = [
                i for i in soup__.find_all('script') if '.mp4' in str(i)]
            #assert len(script_tag_containing_mp4_content) == 1
            self.script_tag_containing_mp4 = self.extract_json(
                script_tag_containing_mp4)
            self.result = self.get_direct_links(self.script_tag_containing_mp4)

    def validate(self, url: str) -> bool:
        return url.endswith('.mp4') and url.startswith('https://vod-progressive.akamaized.net')


class pdf(resource):
    """
    Argument : url (str), for eg: https://www.schoolmouv.fr/cours/little-red-riding-hood/fiche-de-cours
    Functions : run
                download
    """

    def __init__(self, url):
        super().__init__(url)
        self.valids = [
            'scientifique',
            'mouvement-litteraire',
            'schema-bilan',
            'fiche-methode-bac',
            'fiche-de-revision',
            'demonstration',
            'repere',
            'personnages-historique',
            'lecon',
            'fiche-materiel',
            'evenement-historique',
            'savoir-faire',
            'fiche-methode',
            'bien-rediger',
            'fiche-pratique',
            'auteur',
            'philosophe',
            'formule-ses',
            'figure-de-style',
            'fiche-annale',
            'definition',
            'algorithme',
            'fiche-calculatrice',
            'courant-philosophique',
            'fiche-methode-brevet',
            'fiche-de-cours',
            'genre-litteraire',
            'registre-litteraire',
            'carte',
            'fiche-de-lecture',
            'fiche-oeuvre',
            'notion'
        ]
        self.url = url

    def run(self):
        if len([i for i in self.valids if i in self.url]) > 0:
            if self.url.startswith("https://www.schoolmouv.fr/"):
                TO_BE_REPLACED = "www.schoolmouv.fr/eleves" if "/eleves/" in self.url else "www.schoolmouv.fr"
                self.result = f"{self.url.replace(TO_BE_REPLACED,'pdf-schoolmouv.s3.eu-west-1.amazonaws.com')}.pdf"

    def validate(self, url: str) -> bool:
        return url.endswith('.pdf') and url.startswith('https://pdf-schoolmouv.s3.eu-west-1.amazonaws.com/')

    runs-on: keyword = "apache"
template = """
parse "* - * [*] \\"* * *\\" * *" as ip, name, timestamp, method, url, protocol, status, contentlength
"""

    steps: keyword = "k8singressnginx"
template = """
parse "* - * [*] \\"* * *\\" * * \\"*\\" \\"*\\" * * [*] [*] * * * * *" as remote_addr, remote_user, timestamp, method, url, protocol, status, body_bytes_sent, http_referer, http_user_agent, request_length, request_time, proxy_upstream_name, proxy_alternative_upstream_name, upstream_addr, upstream_response_length, upstream_response_time, upstream_status, req_id
"""
    - uses: keyword = "testmultioperator"
template = """
json | count
"""
    - name: keyword = "nginx"
template = """
parse "* - * [*] \\"* * *\\" * * \\"*\\" \\"*\\" \\"*\\"" as addr, user, timestamp, method, url, protocol, status, bytes_sent, http_referer, http_user_agent, gzip_ratio
"""
      run: ./#include "sbar.h"

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
    - name: #include "sbar.h"

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
      run: #pragma once

#include <pthread.h>

#include <X11/Xlib.h>


#include "sblock.h"


typedef enum selem_type_t {
    ELEM_TYPE_BLOCK,
    ELEM_TYPE_DELIM
} selem_type_t;


typedef struct selem_t {
    selem_type_t type;
    void* data; // a string if type = ELEM_TYPE_DELIM or a block_t* if type = ELEM_TYPE_BLOCK
} selem_t;


typedef struct sbar_t {
    Display *dpy; // X11 status display

    selem_t* elems;
    size_t num_elems;

    pthread_mutex_t mut; // protects cond
    pthread_cond_t cond; // used by blocks to force the bar to update
    int32_t update; // set to 1 to force bar update and signal cond
} sbar_t;



int sbar_init(sbar_t* bar);


// adds a block to the bar, elements are added in the order they are added in
int sbar_add_block(sbar_t* bar, sblock_t* block);


// adds a delimiter to the bar
int sbar_add_delim(sbar_t* bar, const char* delim);


// called by a block if he updated and wants the bar to redraw, displays new status
int sbar_update(sbar_t* bar);


// blocks and bar waites for calls to sbar_update()
int sbar_run(sbar_t* bar);


void sbar_deinit(sbar_t* bar);
    - name: #include "sblock.h"

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
      run: #pragma once

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
    - name: #include "util.h"

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
      run: #pragma once


// sleep for msec milliseconds
void msleep(long msec);

// rounds (num / denom) to nearest integer without using float arithmetic
int round_int(int num, int denom);

// returns new formated string according to fmt and args
char* smprintf(const char *fmt, ...);
