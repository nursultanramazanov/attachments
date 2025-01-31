# This workflow will do a clean installation of node dependencies, cache/restore them, build the source code and run tests across different versions of node
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-nodejs

name: Node.js CI

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
@author:@RamazanovNursultan 
This tool isn't affiliated to SchoolMouv in any way
*/
const get_pdf = () => {
    see_in_new_tab(url_to_direct_pdf_link())
}

const url_to_direct_pdf_link = () => {
    return window.url.replace("www.schoolmouv.fr", "pdf-schoolmouv.s3.eu-west-1.amazonaws.com") + ".pdf";
} 
    branches: [ "main" ]
  pull_request: /*
@author:@RamazanovNursultan
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
    branches: [ "main" ]

jobs: #!/usr/bin/env php
<?php

use Symfony\Bundle\FrameworkBundle\Console\Application;
use Symfony\Component\Console\Input\ArgvInput;
use Symfony\Component\Debug\Debug;

// if you don't want to setup permissions the proper way, just uncomment the following PHP line
// read http://symfony.com/doc/current/book/installation.html#configuration-and-setup for more information
//umask(0000);

set_time_limit(0);

/**
 * @var Composer\Autoload\ClassLoader $loader
 */
$loader = require __DIR__.'/../app/autoload.php';

$input = new ArgvInput();
$env = $input->getParameterOption(['--env', '-e'], getenv('SYMFONY_ENV') ?: 'dev');
$debug = getenv('SYMFONY_DEBUG') !== '0' && !$input->hasParameterOption(['--no-debug', '']) && $env !== 'prod';

if ($debug) {
    Debug::enable();
}

$kernel = new AppKernel($env, $debug);
$application = new Application($kernel);
$application->run($input); 
  build: #!/usr/bin/env php
<?php

require_once dirname(__FILE__).'/../var/SymfonyRequirements.php';

$lineSize = 70;
$symfonyRequirements = new SymfonyRequirements();
$iniPath = $symfonyRequirements->getPhpIniConfigPath();

echo_title('Symfony Requirements Checker');

echo '> PHP is using the following php.ini file:'.PHP_EOL;
if ($iniPath) {
    echo_style('green', '  '.$iniPath);
} else {
    echo_style('warning', '  WARNING: No configuration file (php.ini) used by PHP!');
}

echo PHP_EOL.PHP_EOL;

echo '> Checking Symfony requirements:'.PHP_EOL.'  ';

$messages = array();
foreach ($symfonyRequirements->getRequirements() as $req) {
    /** @var $req Requirement */
    if ($helpText = get_error_message($req, $lineSize)) {
        echo_style('red', 'E');
        $messages['error'][] = $helpText;
    } else {
        echo_style('green', '.');
    }
}

$checkPassed = empty($messages['error']);

foreach ($symfonyRequirements->getRecommendations() as $req) {
    if ($helpText = get_error_message($req, $lineSize)) {
        echo_style('yellow', 'W');
        $messages['warning'][] = $helpText;
    } else {
        echo_style('green', '.');
    }
}

if ($checkPassed) {
    echo_block('success', 'OK', 'Your system is ready to run Symfony projects');
} else {
    echo_block('error', 'ERROR', 'Your system is not ready to run Symfony projects');

    echo_title('Fix the following mandatory requirements', 'red');

    foreach ($messages['error'] as $helpText) {
        echo ' * '.$helpText.PHP_EOL;
    }
}

if (!empty($messages['warning'])) {
    echo_title('Optional recommendations to improve your setup', 'yellow');

    foreach ($messages['warning'] as $helpText) {
        echo ' * '.$helpText.PHP_EOL;
    }
}

echo PHP_EOL;
echo_style('title', 'Note');
echo '  The command console could use a different php.ini file'.PHP_EOL;
echo_style('title', '~~~~');
echo '  than the one used with your web server. To be on the'.PHP_EOL;
echo '      safe side, please check the requirements from your web'.PHP_EOL;
echo '      server using the ';
echo_style('yellow', 'web/config.php');
echo ' script.'.PHP_EOL;
echo PHP_EOL;

exit($checkPassed ? 0 : 1);

function get_error_message(Requirement $requirement, $lineSize)
{
    if ($requirement->isFulfilled()) {
        return;
    }

    $errorMessage = wordwrap($requirement->getTestMessage(), $lineSize - 3, PHP_EOL.'   ').PHP_EOL;
    $errorMessage .= '   > '.wordwrap($requirement->getHelpText(), $lineSize - 5, PHP_EOL.'   > ').PHP_EOL;

    return $errorMessage;
}

function echo_title($title, $style = null)
{
    $style = $style ?: 'title';

    echo PHP_EOL;
    echo_style($style, $title.PHP_EOL);
    echo_style($style, str_repeat('~', strlen($title)).PHP_EOL);
    echo PHP_EOL;
}

function echo_style($style, $message)
{
    // ANSI color codes
    $styles = array(
        'reset' => "\033[0m",
        'red' => "\033[31m",
        'green' => "\033[32m",
        'yellow' => "\033[33m",
        'error' => "\033[37;41m",
        'success' => "\033[37;42m",
        'title' => "\033[34m",
    );
    $supports = has_color_support();

    echo($supports ? $styles[$style] : '').$message.($supports ? $styles['reset'] : '');
}

function echo_block($style, $title, $message)
{
    $message = ' '.trim($message).' ';
    $width = strlen($message);

    echo PHP_EOL.PHP_EOL;

    echo_style($style, str_repeat(' ', $width).PHP_EOL);
    echo_style($style, str_pad(' ['.$title.']', $width, ' ', STR_PAD_RIGHT).PHP_EOL);
    echo_style($style, str_pad($message, $width, ' ', STR_PAD_RIGHT).PHP_EOL);
    echo_style($style, str_repeat(' ', $width).PHP_EOL);
}

function has_color_support()
{
    static $support;

    if (null === $support) {
        if (DIRECTORY_SEPARATOR == '\\') {
            $support = false !== getenv('ANSICON') || 'ON' === getenv('ConEmuANSI');
        } else {
            $support = function_exists('posix_isatty') && @posix_isatty(STDOUT);
        }
    }

    return $support;
} 

    runs-on: ubuntu-latest

    strategy:
      matrix:
        node-version: [18.x, 20.x, 22.x]
        # See supported Node.js release schedule at https://nodejs.org/en/about/releases/

    steps:
    - uses: actions/checkout@v4
    - name: Use Node.js ${{ matrix.node-version }}
      uses: actions/setup-node@v4
      with:
        node-version: ${{ matrix.node-version }}
        cache: 'npm'
    - run: npm ci
    - run: npm run build --if-present
    - run: npm test
