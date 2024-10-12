name: Rust

on: extern crate clap;
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

}
  push: extern crate terminal;

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
    branches: [ "main" ]
  pull_request: FROM mcr.microsoft.com/vscode/devcontainers/rust:1-bullseye

# Install cmake
RUN apt-get update \
  && apt-get install -y cmake \
  && rm -rf /var/lib/apt/lists/*

# Install Deno
ENV DENO_INSTALL=/usr/local
RUN curl -fsSL https://deno.land/x/install/install.sh | sh

    branches: [ "main" ]

env: {
  "name": "Rust",
  "build": {
    "dockerfile": "Dockerfile"
  },
  "runArgs": ["--cap-add=SYS_PTRACE", "--security-opt", "seccomp=unconfined"],
  "customizations": {
    "vscode": {
      "settings": {
        "lldb.executable": "/usr/bin/lldb",
        // VS Code don't watch files under ./target
        "files.watcherExclude": {
          "**/target/**": true
        },
        "extensions": [
          "rust-lang.rust-analyzer",
          "tamasfe.even-better-toml",
          "vadimcn.vscode-lldb",
          "mutantdino.resourcemonitor"
        ]
      }
    }
  },
  "postCreateCommand": "git submodule update --init",
  "remoteUser": "vscode"
}
  CARGO_TERM_COLOR: deny from all

jobs: {
  "repositories": [
    {
      "type": "vcs",
      "url": "https://github.com/nsdvw/PreviewGenerator"
    },
    {
      "type": "package",
      "package": {
        "name": "select2/select2",
        "version": "3.5.0",
        "source": {
          "url": "https://github.com/select2/select2",
          "type": "git",
          "reference": "stable/3.5"
        }
      }
    },
    {
      "type": "package",
      "package": {
        "name": "yiiext/twig-renderer",
        "version": "1.1.15",
        "source": {
          "type": "git",
          "url": "https://github.com/yiiext/twig-renderer",
          "reference": "v1.1.15"
        }
      }
    },
    {
      "type": "package",
      "package": {
        "name": "Joblo/cascadedropdown",
        "version": "1.0",
        "dist": {
          "url": "http://www.yiiframework.com/extension/cascadedropdown/files/cascadedropdown.1.0.zip",
          "type": "zip"
        }
      }
    }
  ],
  "require": {
    "php": ">=5.3.0",
    "yiisoft/yii": "dev-master",
    "twig/twig": "1.*",
    "yiiext/twig-renderer": "1.1.*",
    "yiiext/nested-set-behavior": "dev-master",
    "iachilles/eavactiverecord": "dev-master",
    "smottt/wideimage": "~1.1",
    "fzaninotto/faker": "^1.5",
    "nsdvw/PreviewGenerator": "dev-master",
    "twbs/bootstrap": "^3.3",
    "select2/select2": "3.5.0",
    "silverfire/select2-bootstrap3-css": "^1.5"
  },
  "scripts": {
    "post-install-cmd": [
      "bash commands/shell/post-update-cmd.sh"
    ],
    "post-update-cmd": [
      "bash commands/shell/post-update-cmd.sh"
    ]
  }
}
  build: {
    "_readme": [
        "This file locks the dependencies of your project to a known state",
        "Read more about it at https://getcomposer.org/doc/01-basic-usage.md#composer-lock-the-lock-file",
        "This file is @generated automatically"
    ],
    "hash": "e8e03efd2c60e9ca434971fdeaceed3b",
    "content-hash": "b92ffa155ff083846900df268c26e302",
    "packages": [
        {
            "name": "fzaninotto/faker",
            "version": "v1.5.0",
            "source": {
                "type": "git",
                "url": "https://github.com/fzaninotto/Faker.git",
                "reference": "d0190b156bcca848d401fb80f31f504f37141c8d"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/fzaninotto/Faker/zipball/d0190b156bcca848d401fb80f31f504f37141c8d",
                "reference": "d0190b156bcca848d401fb80f31f504f37141c8d",
                "shasum": ""
            },
            "require": {
                "php": ">=5.3.3"
            },
            "require-dev": {
                "phpunit/phpunit": "~4.0",
                "squizlabs/php_codesniffer": "~1.5"
            },
            "suggest": {
                "ext-intl": "*"
            },
            "type": "library",
            "extra": {
                "branch-alias": {
                    "dev-master": "1.5.x-dev"
                }
            },
            "autoload": {
                "psr-4": {
                    "Faker\\": "src/Faker/"
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "MIT"
            ],
            "authors": [
                {
                    "name": ""
                }
            ],
            "description": "Faker is a PHP library that generates fake data for you.",
            "keywords": [
                "data",
                "faker",
                "fixtures"
            ],
            "time": "2015-05-29 06:29:14"
        },
        {
            "name": "iachilles/eavactiverecord",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/iAchilles/eavactiverecord.git",
                "reference": "d118a9509fc3f8f9bac1f271f96e00071b716986"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/iAchilles/eavactiverecord/zipball/d118a9509fc3f8f9bac1f271f96e00071b716986",
                "reference": "d118a9509fc3f8f9bac1f271f96e00071b716986",
                "shasum": ""
            },
            "require": {
                "php": ">=5.1.0"
            },
            "type": "yii-extension",
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "MIT"
            ],
            "authors": [
                {
                    "name": "",
                    "email": ""
                }
            ],
            "description": "Implements entity-attribute-value pattern and provides a simple way to work with EAV-attributes.",
            "homepage": "https://github.com/iAchilles/eavactiverecord",
            "keywords": [
                "activerecord",
                "eav",
                "extension",
                "object-attribute-value",
                "yii"
            ],
            "time": "2014-11-19 11:27:20"
        },
        {
            "name": "nsdvw/PreviewGenerator",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/nsdvw/PreviewGenerator.git",
                "reference": "d9d83200092c03a6de1cbf709be600c3bae50520"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/nsdvw/PreviewGenerator/zipball/d9d83200092c03a6de1cbf709be600c3bae50520",
                "reference": "d9d83200092c03a6de1cbf709be600c3bae50520",
                "shasum": ""
            },
            "type": "library",
            "autoload": {
                "psr-4": {
                    "PreviewGenerator\\": "lib/"
                }
            },
            "support": {
                "source": "https://github.com/nsdvw/PreviewGenerator/tree/master",
                "issues": "https://github.com/nsdvw/PreviewGenerator/issues"
            },
            "time": "2016-01-19 02:52:09"
        },
        {
            "name": "select2/select2",
            "version": "3.5.0",
            "source": {
                "type": "git",
                "url": "https://github.com/select2/select2",
                "reference": "stable/3.5"
            },
            "type": "library"
        },
        {
            "name": "silverfire/select2-bootstrap3-css",
            "version": "1.5.0",
            "source": {
                "type": "git",
                "url": "https://github.com/SilverFire/select2-bootstrap3-css.git",
                "reference": "77bf214ae0022265948bd2c52d739ea1c81853a3"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/SilverFire/select2-bootstrap3-css/zipball/77bf214ae0022265948bd2c52d739ea1c81853a3",
                "reference": "77bf214ae0022265948bd2c52d739ea1c81853a3",
                "shasum": ""
            },
            "type": "library",
            "notification-url": "https://packagist.org/downloads/",
            "description": "Bootstrap3 theme for select2 plugin",
            "homepage": "https://github.com/SilverFire/select2-bootstrap3-css",
            "keywords": [
                "bootstrap",
                "select2",
                "styles"
            ],
            "time": "2015-05-25 14:31:02"
        },
        {
            "name": "smottt/wideimage",
            "version": "1.1.1",
            "source": {
                "type": "git",
                "url": "https://github.com/smottt/WideImage.git",
                "reference": "6cbdaa6eb186c4a618b2ef5a7f38fcb50f0f2b1b"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/smottt/WideImage/zipball/6cbdaa6eb186c4a618b2ef5a7f38fcb50f0f2b1b",
                "reference": "6cbdaa6eb186c4a618b2ef5a7f38fcb50f0f2b1b",
                "shasum": ""
            },
            "require": {
                "php": ">=5.3"
            },
            "type": "library",
            "autoload": {
                "psr-0": {
                    "WideImage": "lib/"
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "GPL-2.0",
                "LGPL-2.1"
            ],
            "description": "An open-source PHP library for image manipulation. (With namespaces, PHP 5.3+)",
            "homepage": "http://wideimage.sourceforge.net",
            "time": "2015-10-05 20:32:34"
        },
        {
            "name": "twbs/bootstrap",
            "version": "v3.3.6",
            "source": {
                "type": "git",
                "url": "https://github.com/twbs/bootstrap.git",
                "reference": "81df608a40bf0629a1dc08e584849bb1e43e0b7a"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/twbs/bootstrap/zipball/81df608a40bf0629a1dc08e584849bb1e43e0b7a",
                "reference": "81df608a40bf0629a1dc08e584849bb1e43e0b7a",
                "shasum": ""
            },
            "replace": {
                "twitter/bootstrap": "self.version"
            },
            "type": "library",
            "extra": {
                "branch-alias": {
                    "dev-master": "3.3.x-dev"
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "MIT"
            ],
            "authors": [
                {
                    "name": "",
                    "email": ""
                },
                {
                    "name": "",
                    "email": ""
                }
            ],
            "description": "The most popular front-end framework for developing responsive, mobile first projects on the web.",
            "homepage": "http://getbootstrap.com",
            "keywords": [
                "JS",
                "css",
                "framework",
                "front-end",
                "less",
                "mobile-first",
                "responsive",
                "web"
            ],
            "time": "2015-11-24 19:37:05"
        },
        {
            "name": "twig/twig",
            "version": "v1.24.0",
            "source": {
                "type": "git",
                "url": "https://github.com/twigphp/Twig.git",
                "reference": "3e5aa30ebfbafd5951fb1b01e338e1800ce7e0e8"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/twigphp/Twig/zipball/3e5aa30ebfbafd5951fb1b01e338e1800ce7e0e8",
                "reference": "3e5aa30ebfbafd5951fb1b01e338e1800ce7e0e8",
                "shasum": ""
            },
            "require": {
                "php": ">=5.2.7"
            },
            "require-dev": {
                "symfony/debug": "~2.7",
                "symfony/phpunit-bridge": "~2.7"
            },
            "type": "library",
            "extra": {
                "branch-alias": {
                    "dev-master": "1.24-dev"
                }
            },
            "autoload": {
                "psr-0": {
                    "Twig_": "lib/"
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "BSD-3-Clause"
            ],
            "authors": [
                {
                    "name": "",
                    "email": "",
                    "homepage": "",
                    "role": "Lead Developer"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Project Founder"
                },
                {
                    "name": "Twig Team",
                    "homepage": "http://twig.sensiolabs.org/contributors",
                    "role": "Contributors"
                }
            ],
            "description": "Twig, the flexible, fast, and secure template language for PHP",
            "homepage": "http://twig.sensiolabs.org",
            "keywords": [
                "templating"
            ],
            "time": "2016-01-25 21:22:18"
        },
        {
            "name": "yiiext/nested-set-behavior",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/yiiext/nested-set-behavior.git",
                "reference": "c0f82f5d1de1ac05d96b1959d7fba248ef3bf959"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/yiiext/nested-set-behavior/zipball/c0f82f5d1de1ac05d96b1959d7fba248ef3bf959",
                "reference": "c0f82f5d1de1ac05d96b1959d7fba248ef3bf959",
                "shasum": ""
            },
            "require": {
                "php": ">=5.1.0"
            },
            "type": "yii-extension",
            "autoload": {
                "classmap": [
                    "NestedSetBehavior.php"
                ]
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "BSD-3-Clause"
            ],
            "authors": [
                {
                    "name": "",
                    "email": ""
                }
            ],
            "description": "AR models behavior that allows to work with nested sets tree.",
            "homepage": "https://github.com/yiiext/nested-set-behavior",
            "keywords": [
                "extension",
                "nested set",
                "yii"
            ],
            "time": "2015-01-01 06:28:50"
        },
        {
            "name": "yiiext/twig-renderer",
            "version": "1.1.15",
            "source": {
                "type": "git",
                "url": "https://github.com/yiiext/twig-renderer",
                "reference": "v1.1.15"
            },
            "type": "library"
        },
        {
            "name": "yiisoft/yii",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/yiisoft/yii.git",
                "reference": "57d4b9775fbe36f968a2d94d92ef8d804599f9be"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/yiisoft/yii/zipball/57d4b9775fbe36f968a2d94d92ef8d804599f9be",
                "reference": "57d4b9775fbe36f968a2d94d92ef8d804599f9be",
                "shasum": ""
            },
            "require": {
                "php": ">=5.1.0"
            },
            "bin": [
                "framework/yiic"
            ],
            "type": "library",
            "autoload": {
                "classmap": [
                    "framework/YiiBase.php",
                    "framework/yii.php"
                ]
            },
            "notification-url": "https://packagist.org/downloads/",
            "license": [
                "BSD-3-Clause"
            ],
            "authors": [
                {
                    "name": "",
                    "email": "",
                    "homepage": "http://www.yiiframework.com/",
                    "role": "Founder and project lead"
                },
                {
                    "name": "",
                    "email": "",
                    "homepage": "",
                    "role": "Core framework development"
                },
                {
                    "name": "",
                    "homepage": "",
                    "role": "Core framework development"
                },
                {
                    "name": "",
                    "email": "",
                    "homepage": "",
                    "role": "Core framework development"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Project site maintenance and development"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Component development"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Documentation and marketing"
                },
                {
                    "name": "",
                    "email": "",
                    "homepage": "http://resurtm.com/",
                    "role": "Core framework development"
                },
                {
                    "name": "",
                    "email": "",
                    "role": "Core framework development"
                }
            ],
            "description": "Yii Web Programming Framework",
            "homepage": "http://www.yiiframework.com/",
            "keywords": [
                "framework",
                "yii"
            ],
            "time": "2016-01-15 11:43:35"
        }
    ],
    "packages-dev": [],
    "aliases": [],
    "minimum-stability": "stable",
    "stability-flags": {
        "yiisoft/yii": 20,
        "yiiext/nested-set-behavior": 20,
        "iachilles/eavactiverecord": 20,
        "nsdvw/previewgenerator": 20
    },
    "prefer-stable": false,
    "prefer-lowest": false,
    "platform": {
        "php": ">=5.3.0"
    },
    "platform-dev": []
}

    runs-on: #!/usr/bin/env php
<?php

require_once(dirname(__FILE__).'/yiic.php');

    steps: @echo off

rem -------------------------------------------------------------
rem  Yii command line script for Windows.
rem  This is the bootstrap script for running yiic on Windows.
rem -------------------------------------------------------------

@setlocal

set BIN_PATH=%~dp0

if "%PHP_COMMAND%" == "" set PHP_COMMAND=php.exe

"%PHP_COMMAND%" "%BIN_PATH%yiic.php" %*

@endlocal
    - uses: <?php

// enable composer autoload
require dirname(__FILE__).'/vendor/autoload.php';

// change the following paths if necessary
$yiic=dirname(__FILE__).'/vendor/yiisoft/yii/framework/yiic.php';
$config=dirname(__FILE__).'/config/console.php';

require_once($yiic);
    - name: Build
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose
