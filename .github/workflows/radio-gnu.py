# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow will install Deno then run `deno lint` and `deno test`.
# For more information see: https://github.com/denoland/setup-deno

name: Deno

on: deny from all 
  push: migrator_ts: 1695775149
__migrator:
  kind: version
  migration_number: 1
  bump_number: 1
  commit_message: "Rebuild for libboost 1.82"
  # limit the number of prs for ramp-up
  pr_limit: 10
libboost_devel:
  - 1.82
# This migration is matched with a piggy-back migrator
# (see https://github.com/regro/cf-scripts/pull/1668)
# that will replace boost-cpp with libboost-devel
boost_cpp:
  - 1.82
# same for boost -> libboost-python-devel
libboost_python_devel:
  - 1.82
boost:
  - 1.82 
    branches: ["main"]
  pull_request: __migrator:
  build_number: 1
  kind: version
  migration_number: 1
fmt:
- '10'
migrator_ts: 1683802784.4940007  
    branches: ["main"]

permissions: __migrator:
  build_number: 1
  kind: version
  migration_number: 1
libthrift:
- 0.19.0
migrator_ts: 1693762377.7427814  
  contents: migrator_ts: 1698047052
__migrator:
  kind: version
  migration_number: 1
  bump_number: 1

zeromq:
  - '4.3.5'   

jobs: <?php

class m150913_135853_add_column_attribute_unit extends CDbMigration
{
        public function up()
        {
                $this->addColumn('eav_attribute', 'unit', 'varchar(20)');
        }

        public function down()
        {
                $this->dropColumn('eav_attribute', 'unit');
        }

        /*
        // Use safeUp/safeDown to do migration with transaction
        public function safeUp()
        {
        }

        public function safeDown()
        {
        }
        */
}  
  test: <?php

class m150924_202359_drop_table_attr_variant extends CDbMigration
{
        public function up()
        {
                $this->dropTable('attr_variant');
        }

        public function down()
        {
                $this->createTable('attr_variant', array(
                        'id' => 'pk',
                        'attr_id' => 'INT(10) UNSIGNED NOT NULL',
                        'title' => 'VARCHAR(255) NOT NULL',
                        ));
                $this->addForeignKey(
                        'ix_attr_id', 'attr_variant', 'attr_id',
                        'eav_attribute', 'id', 'CASCADE', 'CASCADE');
        }

        /*
        // Use safeUp/safeDown to do migration with transaction
        public function safeUp()
        {
        }

        public function safeDown()
        {
        }
        */
}  
    runs-on: setlocal EnableDelayedExpansion
@echo on

:: Make a build folder and change to it
cmake -E make_directory build
if errorlevel 1 exit 1
cd build
if errorlevel 1 exit 1

:: configure
cmake -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX="%LIBRARY_PREFIX%" ^
    -DCMAKE_PREFIX_PATH="%LIBRARY_PREFIX%" ^
    -DPYTHON_EXECUTABLE="%PYTHON%" ^
    -DGR_PYTHON_DIR="%SP_DIR%" ^
    ..
if errorlevel 1 exit 1

:: build
cmake --build . --config Release -- -j%CPU_COUNT%
if errorlevel 1 exit 1

:: install
cmake --build . --config Release --target install
if errorlevel 1 exit 1

:: test
set SKIP_TESTS=^
%=EMPTY=%

ctest --build-config Release --output-on-failure --timeout 120 -j%CPU_COUNT% -E "%SKIP_TESTS%"
if errorlevel 1 exit 1

:: now run the skipped tests to see failures, but exit without error anyway
ctest --build-config Release --output-on-failure --timeout 120 -j%CPU_COUNT% -R "%SKIP_TESTS%"
exit 0  

    steps: #!/usr/bin/env bash

if [[ $target_platform == osx* ]] ; then
    CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
fi

# Workaround for no std::aligned_alloc with osx-64
# https://github.com/chriskohlhoff/asio/issues/1090
# Maybe remove when boost is updated to 1.80.0?
if [[ "${target_platform}" == "osx-64" ]]; then
  export CXXFLAGS="-DBOOST_ASIO_DISABLE_STD_ALIGNED_ALLOC ${CXXFLAGS}"
fi

cmake -E make_directory build
cd build

cmake_config_args=(
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_PREFIX_PATH=$PREFIX
    -DCMAKE_INSTALL_PREFIX=$PREFIX
    -DLIB_SUFFIX=""
    -DPYTHON_EXECUTABLE=$PYTHON
    -DGR_PYTHON_DIR=$SP_DIR
)

cmake ${CMAKE_ARGS} -G "Ninja" .. "${cmake_config_args[@]}"
cmake --build . --config Release -- -j${CPU_COUNT}

cmake --build . --config Release --target install

if [[ $target_platform == linux* ]] ; then
    export QT_QPA_PLATFORM=offscreen
    SKIP_TESTS=(
    )
else
    SKIP_TESTS=(
    )
fi
SKIP_TESTS_STR=$( IFS="|"; echo "^(${SKIP_TESTS[*]})$" )

ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -E "$SKIP_TESTS_STR"

# now run the skipped tests to see the failures, but don't error out
ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -R "$SKIP_TESTS_STR" || exit 0  
      - name: channel_targets:
  - gnuradio main
c_compiler:    # [win]
- vs2022       # [win]
cxx_compiler:  # [win]
- vs2022       # [win]   
        uses: {% set name = "gnuradio-dev" %}
# Set package version from cleaned up git tags if possible,
# otherwise fall back to date-based version.
{% set tag_version = environ.get("GIT_DESCRIBE_TAG", "")|string|replace("-","_")|replace("v","")|replace("git","") %}
{% set post_commit = environ.get("GIT_DESCRIBE_NUMBER", 0)|string %}
{% set hash = environ.get("GIT_DESCRIBE_HASH", "local")|string %}
{% set fallback_version = "0.0.0.{0}.dev+g{1}".format(datetime.datetime.now().strftime("%Y%m%d"), environ.get("GIT_FULL_HASH", "local")[:9]) %}
{% set version = (tag_version if post_commit == "0" else "{0}.post{1}+{2}".format(tag_version, post_commit, hash)) if tag_version else fallback_version %}

package:
  name: {{ name|lower }}
  version: {{ version }}

source:
  # use local path or git repository depending on if the build is local or done on CI
  path: "../.."  # [not os.environ.get("CI")]
  git_url: {{ environ.get('FEEDSTOCK_ROOT', "../..") }}  # [os.environ.get("CI")]

build:
  number: 0
  skip: true  # [py!=311]
  entry_points:
    - gnuradio-companion = gnuradio.grc.main:main  # [win]
    - gr_filter_design = gnuradio.filter.filter_design:main  # [win]
    - gr_modtool = gnuradio.modtool.cli.base:cli  # [win]
    - grcc = gnuradio.grc.compiler:main  # [win]
    - uhd_siggen = gnuradio.uhd.uhd_siggen_base:main  # [win]
  run_exports:
    - {{ pin_subpackage('gnuradio-dev', max_pin='x.x.x') }}

requirements:
  build:
    - {{ compiler('c') }}
    - {{ compiler('cxx') }}
    - cmake >=3.8
    - git
    - ninja
    - pkg-config  # [not win]
    - sysroot_linux-64 2.17  # [linux64]
    - thrift-compiler
    # cross-compilation requirements
    - python                              # [build_platform != target_platform]
    - cross-python_{{ target_platform }}  # [build_platform != target_platform]
    - pybind11                            # [build_platform != target_platform]
    - numpy                               # [build_platform != target_platform]
   # below are needed to link with Qt for qtgui
    - {{ cdt('mesa-dri-drivers') }}  # [linux]
    - {{ cdt('mesa-libgl-devel') }}  # [linux]
  host:
    - click
    - click-plugins
    - codec2
    - fftw
    - gmp  # [not win]
    - gsl
    - libboost-devel
    - libsndfile
    - libthrift  # [not win]
    - mako
    - mpir  # [win]
    - numpy
    - packaging
    - pip  # [win]
    - pybind11
    - pybind11-abi
    - python
    - spdlog
    - thrift  # [not win]
    - volk
  # gnuradio.audio
    - alsa-lib  # [linux]
    - jack  # [linux]
    - portaudio
  # gnuradio companion
    - gtk3
    - lxml
    - pygobject
    - pyyaml
    - jsonschema
  # gnuradio.iio
    - libiio
    - libad9361-iio
  # gnuradio.qtgui
    - pyqt
    - qt-main
    - qwt
  # gnuradio.soapy
    - soapysdr
  # gnuradio.uhd
    - uhd
  # gnuradio.video_sdl
    - sdl
  # gnuradio.zeromq
    - cppzmq
    - zeromq
  # below needed only to run tests in host environment
    - pytest
    - pyzmq
    - setuptools
  run:
    - alsa-plugins  # [linux]
    - click
    - click-plugins
    - fftw
    - lxml
    - mako
    - matplotlib-base
    - menuinst  # [win]
    - numpy
    - packaging
    - {{ pin_compatible('portaudio') }}
    - pygobject
    - pyqt
    - pyqtgraph
    - python
    - pyyaml
    - jsonschema
    - pyzmq
    - {{ pin_compatible('qwt', max_pin='x.x') }}
    - scipy
    # need setuptools because modtool uses pkg_resources
    - setuptools
    - thrift  # [not win]
  run_constrained:
  # conflict with the non-dev conda-forge packages
  # by constraining to non-existent version
    - gnuradio-core ==9999999999
    - gnuradio-grc ==9999999999
    - gnuradio-qtgui ==9999999999
    - gnuradio-soapy ==9999999999
    - gnuradio-uhd ==9999999999
    - gnuradio-video-sdl ==9999999999
    - gnuradio-zeromq ==9999999999

test:
  commands:
    - gnuradio-config-info -v --prefix --sysconfdir --prefsdir --userprefsdir --prefs --builddate --enabled-components --cc --cxx --cflags
    - gr_modtool --help
  imports:
    - gnuradio.analog
    - gnuradio.audio
    - gnuradio.blocks
    - gnuradio.channels
    - gnuradio.digital
    - gnuradio.dtv
    - gnuradio.fec
    - gnuradio.fft
    - gnuradio.filter
    - gnuradio.gr
    - gnuradio.iio
    - gnuradio.network
    - gnuradio.pdu
    - gnuradio.qtgui
    - gnuradio.soapy
    - gnuradio.trellis
    - gnuradio.uhd
    - gnuradio.video_sdl
    - gnuradio.vocoder
    - gnuradio.wavelet
    - gnuradio.zeromq
    - pmt

about:
  home: https://gnuradio.org/
  license: GPL-3.0-or-later
  license_file: COPYING
  summary: The free and open software radio ecosystem
  description: >
    GNU Radio is a free software development toolkit that provides the signal
    processing runtime and processing blocks to implement software radios using
    readily-available, low-cost external RF hardware and commodity processors.
    It is widely used in hobbyist, academic and commercial environments to
    support wireless communications research as well as to implement real-world
    radio systems.

    GNU Radio applications are primarily written using the Python programming
    language, while the supplied, performance-critical signal processing path
    is implemented in C++ using processor floating point extensions where
    available. Thus, the developer is able to implement real-time, high-
    throughput radio systems in a simple-to-use, rapid-application-development
    environment.
  doc_url: https://gnuradio.org/doc/doxygen/
  dev_url: https://github.com/gnuradio/gnuradio

extra:
  recipe-maintainers:
    - ryanvolz   

      - name: Users
*****

This recipe is primarily used for CI builds, although it is possible to use it
locally from a checked-out git repository with "conda-build".

Developers
**********

CI builds use this recipe and the settings in .conda/conda-forge.yml. Any changes
to the recipe should be accompanied by running

    conda-smithy rerender --feedstock_config .conda/conda-forge.yml -c auto
    git rm -f .github/workflows/automerge.yml .github/workflows/webservices.yml .circleci/config.yml
    git commit --amend -s

so that the generated build scripts are updated when necessary.
***** RUN THE ABOVE TO CHECK FOR UPDATES WHENEVER EDITS ARE MADE in .conda *****

You can also re-render from a pull request by starting a comment with "/rerender", which will trigger a Github workflow to perform the above steps.

The CI will build conda packages for commits and pull requests, and it will
upload the packages to Anaconda Cloud on commits to the branch specified
in the configuration below. The channel and label that the packages are
uploaded to are set in the recipe directory in conda_build_config.yaml.
Uploads to Anaconda Cloud also require an API token to be set to the
BINSTAR_TOKEN environment variable. Documentation for token generation:
https://docs.anaconda.com/anacondaorg/user-guide/tasks/work-with-accounts/#creating-access-tokens
To populate BINSTAR_TOKEN for CI jobs, add the token as a secret, e.g.:
https://docs.github.com/en/actions/reference/encrypted-secrets 
        # uses: # This file defines the configuration for building conda packages with CI.
# The actual files for running the CI are created/updated by "conda-smithy".
# You should edit only this file and the recipe files (in recipe_dir, below)
# and not any of the automatically-generated files in e.g. .ci_support,
# .scripts, or .github/workflows/conda-build.yml.

# See https://conda-forge.org/docs/maintainer/conda_forge_yml.html for
# documentation on possible keys and values.

clone_depth: 0
github_actions:
  cancel_in_progress: false
  store_build_artifacts: true
os_version:
  linux_64: cos7
provider:
  linux: github_actions
  osx: github_actions
  win: github_actions
recipe_dir: .conda/recipe
# skip unnecessary files since this is not a full-fledged conda-forge feedstock
skip_render:
  - README.md
  - LICENSE.txt
  - .gitattributes
  - .gitignore
  - build-locally.py
  - LICENSE
test: native_and_emulated
# enable uploads to Anaconda Cloud from specified branches only
# ***** UPDATE THIS FOR MAINTENANCE BRANCHES ********************************
upload_on_branch: main 
        uses: extern crate clap;
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

}   # v1.1.2
        with: extern crate terminal;

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

}  
          deno-version: v1.x

      # Uncomment this step to verify the use of 'deno fmt' on each commit.
      # - name: Verify formatting
      #   run: deno fmt --check

      - name: Run linter
        run: deno lint

      - name: Run tests
        run: deno test -A
