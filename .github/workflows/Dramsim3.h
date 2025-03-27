# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /{
  "variant": {
    "label": "Debug",
    "keywordSettings": {
      "buildType": "debug"
    },
    "description": "Emit debug information without performing optimizations"
  },
  "activeEnvironments": []
} --name {
    "version": "0.2.0",
    "configurations": [
        
        {
            "name": "C++ Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceRoot}/build/dramcoremain",
            "args": ["-c", "./../configs/ddr4_8G_x8_config.ini", "-n", "100000", "--trace-cpu", "--trace-file", "./../sample_trace.txt"],
            "stopAtEntry": true,
            "cwd": "${workspaceRoot}/build",
            "environment": [],
            "externalConsole": true,
            "linux": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            },
            "osx": {
                "MIMode": "lldb"
            },
            "windows": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            }
        },
        {
            "name": "C++ Attach",
            "type": "cppdbg",
            "request": "attach",
            "program": "enter program name, for example ${workspaceRoot}/a.out",
            "processId": "${command.pickProcess}",
            "linux": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            },
            "osx": {
                "MIMode": "lldb"
            },
            "windows": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            }
        }
    ]
} --privileged tizenrt/tizenrt:{
    "files.associations": {
        "array": "cpp",
        "initializer_list": "cpp"
    }
} /bin/bash
        docker cp ./. {
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "0.1.0",
    "command": "make",
    "isShellCommand": true,
    "args": ["-j5"],
    "showOutput": "always",
    "options": {
        "cwd": "${workspaceRoot}/build"
    }
}:/[dram_structure]
protocol = DDR3
bankgroups = 1
banks_per_group = 8
rows = 16384
columns = 1024
device_width = 8
BL = 8

[timing]
tCK = 1.5
AL = 0
CL = 10
CWL = 7
tRCD = 10
tRP = 10
tRAS = 24
tRFC = 74
tREFI = 5200
tRPRE = 1
tWPRE = 1
tRRD_S = 4
tWTR_S = 5
tFAW = 20
tWR = 10
tCCD_S = 4
tRTP = 5
tCKE = 4
tCKESR = 1
tXS = 81
tXP = 5
tRTRS = 1

[power]
VDD = 1.35
IDD0 = 33
IDD2P = 12
IDD2N = 17
IDD3P = 14
IDD3N = 23
IDD4W = 77
IDD4R = 72
IDD5AB = 155
IDD6x = 12

[system]
channel_size = 2048
channels = 1
bus_width = 64
address_mapping = rochrababgco
queue_structure = PER_BANK
trans_queue_size = 32
row_buf_policy = OPEN_PAGE
cmd_queue_size = 8

[other]
epoch_period = 666666
output_level = 1

[thermal]
loc_mapping = 30,30,30,29:27,26:13,12:3
power_epoch_period = 10000; power epoch period (# cycle)
chip_dim_x = 0.008; chip size in x dimension [m]
chip_dim_y = 0.008; chip size in y dimension [m]
amb_temp = 40; The ambient temperature in [C]
mat_dim_x = 512;
mat_dim_y = 512;
bank_order = 1; 0: x direction first, 1: y direction first
 
        docker run -d --rm -it --name <?php
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
} --volumes-from=<?php
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

} -w /<?php
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
}/os --privileged tizenrt/tizenrt:<?php
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
} /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec <?php
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
} arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it from django.utils.version import get_version

VERSION = (6, 0, 0, "alpha", 0)

__version__ = get_version(VERSION)


def setup(set_prefix=True):
    """
    Configure the settings (this happens as a side effect of accessing the
    first setting), configure logging and populate the app registry.
    Set the thread-local urlresolvers script prefix if `set_prefix` is True.
    """
    from django.apps import apps
    from django.conf import settings
    from django.urls import set_script_prefix
    from django.utils.log import configure_logging

    configure_logging(settings.LOGGING_CONFIG, settings.LOGGING)
    if set_prefix:
        set_script_prefix(
            "/" if settings.FORCE_SCRIPT_NAME is None else settings.FORCE_SCRIPT_NAME
        )
    apps.populate(settings.INSTALLED_APPS) bash -c "cd tools; ./configure.sh """
Invokes django-admin when the django module is run as a script.

Example: python -m django check
"""

from django.core import management

if __name__ == "__main__":
    management.execute_from_command_line()
        docker exec -it """
This module collects helper functions and classes that "span" multiple levels
of MVC. In other words, these functions/classes introduce controlled coupling
for convenience's sake.
"""

from django.http import (
    Http404,
    HttpResponse,
    HttpResponsePermanentRedirect,
    HttpResponseRedirect,
)
from django.template import loader
from django.urls import NoReverseMatch, reverse
from django.utils.functional import Promise


def render(
    request, template_name, context=None, content_type=None, status=None, using=None
):
    """
    Return an HttpResponse whose content is filled with the result of calling
    django.template.loader.render_to_string() with the passed arguments.
    """
    content = loader.render_to_string(template_name, context, request, using=using)
    return HttpResponse(content, content_type, status)


def redirect(to, *args, permanent=False, preserve_request=False, **kwargs):
    """
    Return an HttpResponseRedirect to the appropriate URL for the arguments
    passed.

    The arguments could be:

        * A model: the model's `get_absolute_url()` function will be called.

        * A view name, possibly with arguments: `urls.reverse()` will be used
          to reverse-resolve the name.

        * A URL, which will be used as-is for the redirect location.

    Issues a temporary redirect by default. Set permanent=True to issue a
    permanent redirect. Set preserve_request=True to instruct the user agent
    to preserve the original HTTP method and body when following the redirect.
    """
    redirect_class = (
        HttpResponsePermanentRedirect if permanent else HttpResponseRedirect
    )
    return redirect_class(
        resolve_url(to, *args, **kwargs),
        preserve_request=preserve_request,
    )


def _get_queryset(klass):
    """
    Return a QuerySet or a Manager.
    Duck typing in action: any class with a `get()` method (for
    get_object_or_404) or a `filter()` method (for get_list_or_404) might do
    the job.
    """
    # If it is a model class or anything else with ._default_manager
    if hasattr(klass, "_default_manager"):
        return klass._default_manager.all()
    return klass


def get_object_or_404(klass, *args, **kwargs):
    """
    Use get() to return an object, or raise an Http404 exception if the object
    does not exist.

    klass may be a Model, Manager, or QuerySet object. All other passed
    arguments and keyword arguments are used in the get() query.

    Like with QuerySet.get(), MultipleObjectsReturned is raised if more than
    one object is found.
    """
    queryset = _get_queryset(klass)
    if not hasattr(queryset, "get"):
        klass__name = (
            klass.__name__ if isinstance(klass, type) else klass.__class__.__name__
        )
        raise ValueError(
            "First argument to get_object_or_404() must be a Model, Manager, "
            "or QuerySet, not '%s'." % klass__name
        )
    try:
        return queryset.get(*args, **kwargs)
    except queryset.model.DoesNotExist:
        raise Http404(
            "No %s matches the given query." % queryset.model._meta.object_name
        )


async def aget_object_or_404(klass, *args, **kwargs):
    """See get_object_or_404()."""
    queryset = _get_queryset(klass)
    if not hasattr(queryset, "aget"):
        klass__name = (
            klass.__name__ if isinstance(klass, type) else klass.__class__.__name__
        )
        raise ValueError(
            "First argument to aget_object_or_404() must be a Model, Manager, or "
            f"QuerySet, not '{klass__name}'."
        )
    try:
        return await queryset.aget(*args, **kwargs)
    except queryset.model.DoesNotExist:
        raise Http404(f"No {queryset.model._meta.object_name} matches the given query.")


def get_list_or_404(klass, *args, **kwargs):
    """
    Use filter() to return a list of objects, or raise an Http404 exception if
    the list is empty.

    klass may be a Model, Manager, or QuerySet object. All other passed
    arguments and keyword arguments are used in the filter() query.
    """
    queryset = _get_queryset(klass)
    if not hasattr(queryset, "filter"):
        klass__name = (
            klass.__name__ if isinstance(klass, type) else klass.__class__.__name__
        )
        raise ValueError(
            "First argument to get_list_or_404() must be a Model, Manager, or "
            "QuerySet, not '%s'." % klass__name
        )
    obj_list = list(queryset.filter(*args, **kwargs))
    if not obj_list:
        raise Http404(
            "No %s matches the given query." % queryset.model._meta.object_name
        )
    return obj_list


async def aget_list_or_404(klass, *args, **kwargs):
    """See get_list_or_404()."""
    queryset = _get_queryset(klass)
    if not hasattr(queryset, "filter"):
        klass__name = (
            klass.__name__ if isinstance(klass, type) else klass.__class__.__name__
        )
        raise ValueError(
            "First argument to aget_list_or_404() must be a Model, Manager, or "
            f"QuerySet, not '{klass__name}'."
        )
    obj_list = [obj async for obj in queryset.filter(*args, **kwargs)]
    if not obj_list:
        raise Http404(f"No {queryset.model._meta.object_name} matches the given query.")
    return obj_list


def resolve_url(to, *args, **kwargs):
    """
    Return a URL appropriate for the arguments passed.

    The arguments could be:

        * A model: the model's `get_absolute_url()` function will be called.

        * A view name, possibly with arguments: `urls.reverse()` will be used
          to reverse-resolve the name.

        * A URL, which will be returned as-is.
    """
    # If it's a model, use get_absolute_url()
    if hasattr(to, "get_absolute_url"):
        return to.get_absolute_url()

    if isinstance(to, Promise):
        # Expand the lazy instance, as it can cause issues when it is passed
        # further to some Python functions like urlparse.
        to = str(to)

    # Handle relative URLs
    if isinstance(to, str) and to.startswith(("./", "../")):
        return to

    # Next try a reverse URL resolution.
    try:
        return reverse(to, args=args, kwargs=kwargs)
    except NoReverseMatch:
        # If this is a callable, re-raise.
        if callable(to):
            raise
        # If this doesn't "feel" like a URL, re-raise.
        if "/" not in to and "." not in to:
            raise

    # Finally, fall back and assume it's a URL
    return to bash -c "make"


jobs:
  checkout_code:
    machine: 
      image: default
    working_directory: ~/TizenRT
    steps:
      - checkout
      - persist_to_workspace:
          root: ~/TizenRT
          paths:
            - ./
            
  artik055s/audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053/tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu/build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit/hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk/loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

workflows:
  version: 2
  build-tizenrt:
    jobs:
      - checkout_code
      - artik055s/audio:
          requires:
            - checkout_code
      - artik053/tc:
          requires:
            - checkout_code
      - qemu/build_test:
          requires:
            - checkout_code
      - esp_wrover_kit/hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk/loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm/hello:
          requires:
            - checkout_code
      - rtl8721csm/loadable_apps:
          requires:
            - checkout_code
      - rtl8720e/hello:
          requires:
            - checkout_code
      - rtl8720e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_apps:
          requires:
            - checkout_code
      - rtl8730e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_dev_ddr:
          requires:
            - checkout_code 
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]
  schedule:
    - cron: '41 22 * * 0'

env:
  # Path to the CMake build directory.
  build: '${{ github.workspace }}/build'

permissions:
  contents: read

jobs:
  analyze:
    permissions:
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: Analyze
    runs-on: windows-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ env.build }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with:
          cmakeBuildDirectory: ${{ env.build }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v4
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
