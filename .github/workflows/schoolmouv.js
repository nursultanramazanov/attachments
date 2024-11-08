# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow will install Deno then run `deno lint` and `deno test`.
# For more information see: https://github.com/denoland/setup-deno

name: Deno

on: [alias]
build-man = "run --package xtask-build-man --"
stale-label = "run --package xtask-stale-label --"
bump-check = "run --package xtask-bump-check --"
lint-docs = "run --package xtask-lint-docs --"

[env]
# HACK: Until this is stabilized, `snapbox`s polyfill could get confused
# inside of the rust-lang/rust repo because it looks for the furthest-away `Cargo.toml`
CARGO_RUSTC_CURRENT_DIR = { value = "", relative = true }   
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
    branches: ["main"]
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
    branches: ["main"]

permissions: /*
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
  contents: /*
author : t0pl
This tool isn't affiliated with SchoolMouv in any way
*/
browser.browserAction.onClicked.addListener(function (activeTab) {
    const url = clean_url(activeTab.url);
    window.url = url;
    if (is_website_valid(url)) {
        const type_ = resource_type();
        console.log(url, type_);
        if (type_ === 'pdf') {
            get_pdf();
        } else if (type_ === 'video') {
            get_video();
        }
    }
}) 

jobs: {
    "name": "SchoolMouv's premium resources made free",
    "version": "1.4",
    "manifest_version": 2,

    "description": "Gets schoolmouv.fr's premium videos and pdfs for free, no need to be a registered user",
    "author": "t0pl",
    "background": {
        "scripts": ["./scripts/common.js","background.js", "./scripts/pdf.js","./scripts/video.js"],
        "persistent": true
    },
    "icons": {
        "16": "images/icon16.png",
        "48": "images/icon48.png",
        "128": "images/icon128.png"
    },
    "permissions": [
        "https://www.schoolmouv.fr/*", "https://*.vimeo.com/*", "https://*.akamaized.net/*", "webRequest", "webRequestBlocking", "tabs", "activeTab"
    ],
    "browser_action": {
        "default_title": "SchoolMouv's premium resources made free"
    }
}   
  test: <?php
namespace Tests\TestHubBundle\Controller;

use Tests\TestHubBundle\TestCase;

class TestControllerTest extends TestCase
{
    public function testIndex()
    {
        $client = static::createClient();

        $crawler = $client->request('GET', '/');

        $this->assertEquals(200, $client->getResponse()->getStatusCode());
        $this->assertGreaterThan(0, $crawler->filter('.test-container')->count());
    }

    public function testPreface()
    {
        $client = static::createClient();

        $crawler = $client->request('GET', '/test/1/preface');

        $this->assertEquals(200, $client->getResponse()->getStatusCode());
        $this->assertEquals(1, $crawler->filter('.test-container')->count());
        $this->assertEquals(1, $crawler->filter('.test-start-button')->count());

        $client->request('GET', '/test/0/preface');
        $this->assertEquals(404, $client->getResponse()->getStatusCode());
    }
}   
    runs-on: <?php
namespace Tests\TestHubBundle\Helper;

use TestHubBundle\Helper\StringGenerator;
use Tests\TestHubBundle\TestCase;

class StringGeneratorTest extends TestCase
{
    /**
     * What to test:
     *
     * return value type
     * return value length (between 10 and 100 chars)
     * return value uniqueness (at least 2 must differ)
     */
    public function testGenerateString()
    {
        $string = StringGenerator::generateString();
        $this->assertInternalType('string', $string);

        $iterations = 5;
        $randoms = [];
        for ($i = 0; $i < $iterations; $i++) {
            $randoms[] = StringGenerator::generateString();
        }
        $this->assertTrue(count(array_unique($randoms)) > 1);

        $string = StringGenerator::generateString();
        $this->assertGreaterThan(10, mb_strlen($string));
        $this->assertLessThan(100, mb_strlen($string));
    }

}    

    steps: <?php
namespace Tests\TestHubBundle\Service;

use Symfony\Bundle\FrameworkBundle\Console\Application;
use TestHubBundle\Service\Calculator;
use Tests\TestHubBundle\TestCase;

class CalculatorTest extends TestCase
{
    protected $em;

    protected function setUp()
    {
        parent::setUp();

        $container = self::getApplication()->getKernel()->getContainer();
        $this->em = $container->get('doctrine')->getManager();
    }

    public function testCountCorrectAnswers()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);

        $calculator = new Calculator();
        $correctAnswersCount = $calculator->countCorrectAnswers($attempt);
        $this->assertInternalType('integer', $correctAnswersCount);
        $this->assertEquals(2, $correctAnswersCount);
    }

    public function testCalculateMark()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);

        $calculator = new Calculator();
        $mark = $calculator->calculateMark($attempt);
        $this->assertInternalType('integer', $mark);
        $this->assertEquals(15, $mark);
    }

    public function testCalculateMaxMark()
    {
        $test = $this->em->find('TestHubBundle:Test', 1);

        $calculator = new Calculator();
        $maxMark = $calculator->calculateMaxMark($test);
        $this->assertInternalType('integer', $maxMark);
        $this->assertEquals(30, $maxMark);
    }
}   
      - name: <?php
namespace Tests\TestHubBundle\Service;

use Tests\TestHubBundle\TestCase;

class TestServiceTest extends TestCase
{
    private $em;
    private $testService;

    protected function setUp()
    {
        parent::setUp();

        $container = $this->getApplication()->getKernel()->getContainer();
        $this->testService = $container->get('test_service');
        $this->em = $container->get('doctrine')->getManager();
    }

    public function testGetUnansweredCount()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);

        $count = $this->testService->getUnansweredCount($attempt);
        $this->assertInternalType('integer', $count);
        $this->assertEquals(1, $count);
    }

    public function testQuestionAlreadyHasAnswer()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);
        $expectHasAnswer = $this->em->find('TestHubBundle:Question', 4);
        $expectNoAnswer = $this->em->find('TestHubBundle:Question', 1);

        $true = $this->testService->questionAlreadyHasAnswer($attempt, $expectHasAnswer);
        $false = $this->testService->questionAlreadyHasAnswer($attempt, $expectNoAnswer);

        $this->assertTrue($true);
        $this->assertFalse($false);
    }

    public function testGetFirstUnanswered()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);
        $question = $this->testService->getFirstUnanswered($attempt);

        $this->assertInstanceOf('TestHubBundle\Entity\Question', $question);
        $this->assertEquals(1, $question->getId());
    }

    public function testGetNextUnansweredNumber()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);
        $number = 1;

        $nextNumber = $this->testService->getNextUnansweredNumber($attempt, $number);

        $this->assertEquals(4, $nextNumber);
    }

    public function testGetQuestionsCount()
    {
        $test = $this->em->find('TestHubBundle:Test', 1);

        $count = $this->testService->getQuestionsCount($test);
        $this->assertEquals(4, $count);
    }
}   
        uses: <?php
namespace Tests\TestHubBundle\Twig;

use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;
use TestHubBundle\Twig\AppExtension;

class AppExtensionTest extends WebTestCase
{
    private $ext;

    protected function setUp()
    {
        $this->ext = new AppExtension();
    }

    public function testWordCaseFilter()
    {
        $wordForms = ['день', 'дня', 'дней'];

        $this->assertEquals('1 день', $this->ext->wordCase(1, $wordForms));
        $this->assertEquals('3 дня', $this->ext->wordCase(3, $wordForms));
        $this->assertEquals('5 дней', $this->ext->wordCase(5, $wordForms));
        $this->assertEquals('17 дней', $this->ext->wordCase(17, $wordForms));
        $this->assertEquals('22 дня', $this->ext->wordCase(22, $wordForms));
        $this->assertEquals('113 дней', $this->ext->wordCase(113, $wordForms));
        $this->assertEquals('269 дней', $this->ext->wordCase(269, $wordForms));

        $wordForms = ['вопрос', 'вопроса', 'вопросов'];

        $this->assertEquals('1 вопрос', $this->ext->wordCase(1, $wordForms));
        $this->assertEquals('3 вопроса', $this->ext->wordCase(3, $wordForms));
        $this->assertEquals('5 вопросов', $this->ext->wordCase(5, $wordForms));
        $this->assertEquals('17 вопросов', $this->ext->wordCase(17, $wordForms));
        $this->assertEquals('22 вопроса', $this->ext->wordCase(22, $wordForms));
        $this->assertEquals('113 вопросов', $this->ext->wordCase(113, $wordForms));
        $this->assertEquals('269 вопросов', $this->ext->wordCase(269, $wordForms));

        $wordForms = ['минута', 'минуты', 'минут'];

        $this->assertEquals('1 минута', $this->ext->wordCase(1, $wordForms));
        $this->assertEquals('3 минуты', $this->ext->wordCase(3, $wordForms));
        $this->assertEquals('5 минут', $this->ext->wordCase(5, $wordForms));
        $this->assertEquals('17 минут', $this->ext->wordCase(17, $wordForms));
        $this->assertEquals('22 минуты', $this->ext->wordCase(22, $wordForms));
        $this->assertEquals('113 минут', $this->ext->wordCase(113, $wordForms));
        $this->assertEquals('269 минут', $this->ext->wordCase(269, $wordForms));
    }

    public function testFormatTimeLeftFilter()
    {
        /*
         * 1 day 4 hours 18 minutes 23 seconds
         */
        $time = 3600 * 24 + 4 * 3600 + 18 * 60 + 23;
        $this->assertEquals('1 день 04:18:23', $this->ext->formatTimeLeftFilter($time));
    }

    public function testPercentage()
    {
        $this->assertEquals('66%', $this->ext->percentage(2, 3));
        $this->assertEquals('0%', $this->ext->percentage(0, 3));
        $this->assertEquals('100%', $this->ext->percentage(3, 3));
    }
}    

      - name: <?php
namespace Tests\TestHubBundle;

use Symfony\Bundle\FrameworkBundle\Console\Application;
use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;
use Symfony\Component\Console\Input\StringInput;

class TestCase extends WebTestCase
{
    protected static $application;

    protected function setUp()
    {
        self::runCommand('doctrine:fixtures:load -e test --no-interaction --purge-with-truncate');
    }

    protected static function runCommand($command)
    {
        $command = sprintf('%s --quiet', $command);

        return self::getApplication()->run(new StringInput($command));
    }

    protected static function getApplication()
    {
        if (null === self::$application) {
            $client = static::createClient();

            self::$application = new Application($client->getKernel());
            self::$application->setAutoExit(false);
        }

        return self::$application;
    }
}   
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
