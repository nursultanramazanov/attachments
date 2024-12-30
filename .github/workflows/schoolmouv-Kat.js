# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow will install Deno then run `deno lint` and `deno test`.
# For more information see: https://github.com/denoland/setup-deno

name: Deno

on: /*
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
  push: /*
@author:t0pl
This tool isn't affiliated to SchoolMouv in any way
*/
const get_pdf = () => {
    see_in_new_tab(url_to_direct_pdf_link())
}

const url_to_direct_pdf_link = () => {
    return window.url.replace("www.schoolmouv.fr", "pdf-schoolmouv.s3.eu-west-1.amazonaws.com") + ".pdf";
}   
    branches: ["main"]
  pull_request: /*
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
    branches: ["main"]

permissions: 



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
}   
  contents: read

jobs:
  test:
    runs-on: ubuntu-latest

    steps:
      - name: Setup repo
        uses: actions/checkout@v4

      - name: Setup Deno
        # uses: denoland/setup-deno@v1
        uses: denoland/setup-deno@61fe2df320078202e33d7d5ad347e7dcfa0e8f31  # v1.1.2
        with:
          deno-version: v1.x

      # Uncomment this step to verify the use of 'deno fmt' on each commit.
      # - name: Verify formatting
      #   run: deno fmt --check

      - name: Run linter
        run: deno lint

      - name: Run tests
        run: deno test -A
