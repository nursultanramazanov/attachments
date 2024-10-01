# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: MSBuild

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
  pull_request: Copyright 2003
Persian Gulf
http://cr.yp.to/daemontools.html
    branches: [ "main" ]

env: 
SV:123456:respawn:/command/svscanboot
  # Path to the solution file relative to the root of the project.
  SOLUTION_FILE_PATH: .
  csh -cf '/command/svscanboot &'

  # Configuration type to build.
  # You can convert this to a build matrix if you need coverage of multiple configuration types.
  # https://docs.github.com/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
  BUILD_CONFIGURATION: svscan
svscanboot
supervise
svc
svok
svstat
fghack
pgrphack
readproctitle
multilog
tai64n
tai64nlocal
softlimit
setuidgid
envuidgid
envdir
setlock

permissions: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

here=`env - PATH=$PATH pwd`

mkdir -p compile command
test -r compile/home || echo $here > compile/home
test -h compile/src || ln -s $here/src compile/src

echo 'Linking ./src/* into ./compile...'
for i in `ls src`
do
  test -h compile/$i || ln -s src/$i compile/$i
done

echo 'Compiling everything in ./compile...'
( cd compile; exec make )

echo 'Copying commands into ./command...'
for i in `cat package/commands`
do
  rm -f command/$i'{new}'
  cp -p compile/$i command/$i'{new}'
  mv -f command/$i'{new}' command/$i
done
  contents: package
package/README
package/files
package/sharing
package/commands
package/install
package/compile
package/upgrade
package/run
package/run.inittab
package/boot.inittab
package/run.rclocal
package/boot.rclocal
src
src/CHANGES
src/TODO
src/Makefile
src/svscanboot.sh
src/svscan.c
src/supervise.c
src/svc.c
src/svok.c
src/svstat.c
src/fghack.c
src/pgrphack.c
src/readproctitle.c
src/multilog.c
src/tai64n.c
src/tai64nlocal.c
src/softlimit.c
src/setuidgid.c
src/envuidgid.c
src/envdir.c
src/setlock.c
src/match.c
src/match.h
src/matchtest.c
src/timestamp.c
src/timestamp.h
src/deepsleep.c
src/deepsleep.h
src/conf-cc
src/conf-ld
src/print-cc.sh
src/print-ld.sh
src/print-ar.sh
src/choose.sh
src/warn-auto.sh
src/find-systype.sh
src/trycpp.c
src/x86cpuid.c
src/rts.tests
src/buffer.h
src/buffer.c
src/byte.h
src/str.h
src/str_len.c
src/byte_copy.c
src/byte_cr.c
src/error.h
src/error.c
src/buffer_put.c
src/buffer_read.c
src/buffer_write.c
src/buffer_1.c
src/direntry.h1
src/direntry.h2
src/trydrent.c
src/strerr.h
src/error_str.c
src/strerr_sys.c
src/buffer_2.c
src/strerr_die.c
src/wait.h
src/trywaitp.c
src/haswaitp.h1
src/haswaitp.h2
src/wait_pid.c
src/coe.h
src/coe.c
src/fd.h
src/fd_copy.c
src/fd_move.c
src/str_start.c
src/env.h
src/env.c
src/alloc.h
src/alloc.c
src/alloc_re.c
src/gen_alloc.h
src/gen_allocdefs.h
src/stralloc.h
src/stralloc_eady.c
src/stralloc_opyb.c
src/stralloc_catb.c
src/stralloc_cats.c
src/stralloc_pend.c
src/str_chr.c
src/pathexec.h
src/pathexec_run.c
src/stralloc_opys.c
src/stralloc_cat.c
src/byte_diff.c
src/pathexec_env.c
src/wait_nohang.c
src/sig.h
src/sig.c
src/sig_block.c
src/sig_catch.c
src/sig_pause.c
src/hassgact.h1
src/hassgact.h2
src/hassgprm.h1
src/hassgprm.h2
src/trysgact.c
src/trysgprm.c
src/select.h1
src/select.h2
src/trysysel.c
src/uint64.h1
src/uint64.h2
src/tryulong64.c
src/tai.h
src/taia.h
src/taia_now.c
src/taia_sub.c
src/taia_less.c
src/taia_frac.c
src/taia_approx.c
src/iopause.h1
src/iopause.h2
src/iopause.c
src/trypoll.c
src/fifo.h
src/fifo.c
src/hasmkffo.h1
src/hasmkffo.h2
src/trymkffo.c
src/open.h
src/open_read.c
src/open_write.c
src/open_append.c
src/lock.h
src/hasflock.h1
src/hasflock.h2
src/tryflock.c
src/lock_exnb.c
src/lock_ex.c
src/ndelay.h
src/ndelay_on.c
src/ndelay_off.c
src/tai_pack.c
src/taia_pack.c
src/taia_add.c
src/open_trunc.c
src/taia_uint.c
src/subgetopt.h
src/subgetopt.c
src/sgetopt.h
src/sgetopt.c
src/byte_chr.c
src/fmt.h
src/scan.h
src/fmt_ulong.c
src/fmt_uint.c
src/fmt_uint0.c
src/tai_unpack.c
src/tai_now.c
src/tai_sub.c
src/seek.h
src/seek_set.c
src/str_diff.c
src/byte_rchr.c
src/scan_ulong.c
src/buffer_get.c
src/buffer_0.c
src/prot.h
src/prot.c
src/hasshsgr.h1
src/hasshsgr.h2
src/chkshsgr.c
src/warn-shsgr
src/tryshsgr.c
src/readclose.h
src/readclose.c
src/openreadclose.h
src/openreadclose.c

jobs: #!/bin/sh -e
package/compile
package/upgrade
package/run
  build: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

if test -d /service
then
  echo '/service exists. I assume that svscan is already running.'
  exit 0
fi

echo 'Creating /service...'
mkdir /service

if test -r /etc/inittab
then
  /command/setlock /etc/inittab package/run.inittab
else
  /command/setlock /etc/rc.local package/run.rclocal
fi
    runs-on: #!/bin/sh -e

if grep svscanboot /etc/inittab >/dev/null
then
  echo 'inittab contains an svscanboot line. I assume that svscan is already running.'
else
  echo 'Adding svscanboot to inittab...'
  rm -f /etc/inittab'{new}'
  cat /etc/inittab package/boot.inittab > /etc/inittab'{new}'
  mv -f /etc/inittab'{new}' /etc/inittab
  kill -HUP 1
  echo 'init should start svscan now.'
fi

    steps: #!/bin/sh -e

if grep svscanboot /etc/rc.local >/dev/null
then
  echo 'rc.local contains an svscanboot line. I assume that svscan is already running.'
else
  echo 'Adding svscanboot to /etc/rc.local...'
  rm -f /etc/rc.local'{new}'
  cat /etc/rc.local package/boot.rclocal > /etc/rc.local'{new}'
  mv -f /etc/rc.local'{new}' /etc/rc.local
  echo 'Reboot now to start svscan.'
fi
    - uses: command:syst
compile:syst
package:dist
src:dist

    - name: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

here=`env - PATH=$PATH pwd`
parent=`dirname $here`

echo 'Creating symlink daemontools -> daemontools-0.76...'
rm -f daemontools
ln -s daemontools-0.76 daemontools
mv -f daemontools ..

echo 'Making command links in /command...'
mkdir -p /command
for i in `cat package/commands`
do
  rm -f /command/$i'{new}'
  ln -s $parent/daemontools/command/$i /command/$i'{new}'
  mv -f /command/$i'{new}' /command/$i
done
echo 'Making compatibility links in /usr/local/bin...'
mkdir -p /usr/local/bin
for i in `cat package/commands`
do
  rm -f /usr/local/bin/$i'{new}'
  ln -s /command/$i /usr/local/bin/$i'{new}'
  mv -f /usr/local/bin/$i'{new}' /usr/local/bin/$i
done
      uses: 
to build the hex:

make mega2560

To flash the firmware, use avrdude version 5.11 or above

SERIAL_PORT=/dev/tty.usbserial

# erase the chip (necessary before unlocking can happen), then unlock the boot loader area
avrdude -e -c stk500v2 -p m2560 -P $SERIAL_PORT -U lock:w:0x3f:m

# flash the hex file (whatever.hex)
avrdude -v -c stk500v2 -p m2560 -P $SERIAL_PORT  -U flash:w:stk500boot_v2_mega2560.hex

# re-lock the bootloader area of the chip
avrdude -c stk500v2 -p m2560 -P $SERIAL_PORT -U lock:w:0x0f:m

    - name: # ----------------------------------------------------------------------------
# Makefile to compile and link stk500boot bootloader
# 
# File:   $Id: Makefile,v 1.3 2006/03/04 19:26:17 peter Exp $
# based on WinAVR Makefile Template written by Eric B. Weddington, Jörg Wunsch, et al.
#
# Adjust F_CPU below to the clock frequency in Mhz of your AVR target
# Adjust BOOTLOADER_ADDRESS to your AVR target
#
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make debug = Start either simulavr or avarice as specified for debugging, 
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------
#        <MLS> = Mark Sproul msproul-at-skychariot.com


# MCU name
#MCU = atmega128


# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the 
#     processor frequency. You can then use this symbol in your source code to 
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
#F_CPU = 16000000


# Bootloader
# Please adjust if using a different AVR
# 0x0e00*2=0x1C00 for ATmega8  512 words Boot Size
# 0xFC00*2=0x1F800 for ATmega128  1024 words Boot Size
# 0xF800*2=0x1F000 for ATmega1280
# 0xF000*2=0x1E000 for ATmega1280
#BOOTLOADER_ADDRESS = 1E000


# Output format. (can be srec, ihex, binary)
FORMAT = ihex


# Target file name (without extension).
TARGET = stk500boot


# List C source files here. (C dependencies are automatically generated.)
SRC = stk500boot.c 


# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
ASRC = 


# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s


# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2


# List any extra directories to look for include files here.
#     Each directory must be separated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS = 


# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99


# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)UL


# Place -I options here
CINCS =



#---------------- Compiler Options ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mno-tablejump 
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)


#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


MATH_LIB = -lm



#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =




#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)


#--------------- bootloader linker Options -------
# BOOTLOADER_ADDRESS (=Start of Boot Loader section
# in bytes - not words) is defined above.
#LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS) -nostartfiles -nodefaultlibs
#LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS) -nostartfiles
LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS)

#---------------- Programming Options (avrdude) ----------------

# Programming hardware: alf avr910 avrisp bascom bsd 
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
#
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = stk500v2

# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = com1    # programmer connected to serial device

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)



#---------------- Debugging Options ----------------

# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(F_CPU)

# Set the DEBUG_UI to either gdb or insight.
# DEBUG_UI = gdb
DEBUG_UI = insight

# Set the debugging back-end to either avarice, simulavr.
DEBUG_BACKEND = avarice
#DEBUG_BACKEND = simulavr

# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit

# When using avarice settings for the JTAG
JTAG_DEV = /dev/com1

# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242

# Debugging host used to communicate between GDB / avarice / simulavr, normally
#     just set to localhost unless doing some sort of crazy debugging when 
#     avarice is running on a different computer.
DEBUG_HOST = localhost



#============================================================================


# Define programs and commands.
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
COPY = cp
WINSHELL = cmd


# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:




# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o) 

# Define all listing files.
LST = $(SRC:.c=.lst) $(ASRC:.S=.lst) 


# Compiler flags to generate dependency files.
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)



############################################################
#        May 25,        2010        <MLS> Adding 1280 support
mega1280: MCU = atmega1280
mega1280: F_CPU = 16000000
mega1280: BOOTLOADER_ADDRESS = 1E000
mega1280: CFLAGS += -D_MEGA_BOARD_
mega1280: begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_mega1280.hex


############################################################
#        Jul 6,        2010        <MLS> Adding 2560 support
mega2560:        MCU = atmega2560
mega2560:        F_CPU = 16000000
mega2560:        BOOTLOADER_ADDRESS = 3E000
mega2560:        CFLAGS += -D_MEGA_BOARD_
mega2560:        begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_mega2560.hex


############################################################
#Initial config on Amber128 board
#        avrdude: Device signature = 0x1e9702
#        avrdude: safemode: lfuse reads as 8F
#        avrdude: safemode: hfuse reads as CB
#        avrdude: safemode: efuse reads as FF
#        Jul 17,        2010        <MLS> Adding 128 support
############################################################
amber128: MCU = atmega128
#amber128: F_CPU = 16000000
amber128: F_CPU = 14745600
amber128: BOOTLOADER_ADDRESS = 1E000
amber128: CFLAGS += -D_BOARD_AMBER128_
amber128: begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_amber128.hex

############################################################
#        Aug 23, 2010         <MLS> Adding atmega2561 support
m2561_8mhz: MCU = atmega2561
m2561_8mhz: F_CPU = 8000000
m2561_8mhz: BOOTLOADER_ADDRESS = 3E000
m2561_8mhz: CFLAGS += -D_ANDROID_2561_ -DBAUDRATE=57600
m2561_8mhz: begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_m2561_8mhz.hex


############################################################
#        Aug 23, 2010         <MLS> Adding atmega2561 support
m2561_16mhz: MCU = atmega2561
m2561_16mhz: F_CPU = 16000000
m2561_16mhz: BOOTLOADER_ADDRESS = 3E000
m2561_16mhz: CFLAGS += -D_ANDROID_2561_ -DBAUDRATE=115200
m2561_16mhz: begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_m2561_16mhz.hex

############################################################
#        avrdude: Device signature = 0x1e9801
#        avrdude: safemode: lfuse reads as EC
#        avrdude: safemode: hfuse reads as 18
#        avrdude: safemode: efuse reads as FD
#        Aug 23,        2010        <MLS> Adding cerebot 2560 @ 8mhz
#avrdude -P usb -c usbtiny -p m2560 -v -U flash:w:/Arduino/WiringBootV2_upd1/stk500boot_v2_cerebotplus.hex 
############################################################
cerebot:        MCU = atmega2560
cerebot:        F_CPU = 8000000
cerebot:        BOOTLOADER_ADDRESS = 3E000
cerebot:        CFLAGS += -D_CEREBOTPLUS_BOARD_ -DBAUDRATE=38400 -DUART_BAUDRATE_DOUBLE_SPEED=1
cerebot:        begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_cerebotplus.hex


############################################################
#        Aug 23, 2010         <MLS> Adding atmega32 support
penguino: MCU = atmega32
penguino: F_CPU = 16000000
penguino: BOOTLOADER_ADDRESS = 7800
penguino: CFLAGS += -D_PENGUINO_ -DBAUDRATE=57600
penguino: begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_penguino.hex


############################################################
#        Sep 8, 2010         <MLS> Adding atmega16 support
atmega16: MCU = atmega16
atmega16: F_CPU = 8000000
atmega16: BOOTLOADER_ADDRESS = 3800
atmega16: CFLAGS += -DBOARD_MEGA16 -DBAUDRATE=57600
atmega16: begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_atmega16.hex


############################################################
#        Sep 17, 2010         <MLS> Adding BahBots 1284p
bahbot: MCU = atmega1284p
bahbot: F_CPU = 18432000
bahbot: BOOTLOADER_ADDRESS = 1E000
bahbot: CFLAGS += -D_BOARD_BAHBOT_ -DBAUDRATE=38400
bahbot: begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_bahbots1284.hex


############################################################
#        Sep 24, 2010         <MLS> custom bootloader for ro-bot-x
robotx: MCU = atmega640
robotx: F_CPU = 16000000
robotx: BOOTLOADER_ADDRESS = E000
robotx: CFLAGS += -D_BOARD_ROBOTX_ -DBAUDRATE=115200 -DENABLE_MONITOR
robotx: begin gccversion sizebefore build sizeafter end 
                        mv $(TARGET).hex stk500boot_v2_robotx640uart1.hex


# Default target.
all: begin gccversion sizebefore build sizeafter end

build: elf hex eep lss sym
#build:  hex eep lss sym

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym



# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
        @echo
        @echo $(MSG_BEGIN)

end:
        @echo $(MSG_END)
        @echo


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) --format=avr --mcu=$(MCU) $(TARGET).elf

sizebefore:
        @if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
        2>/dev/null; echo; fi

sizeafter:
        @if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
        2>/dev/null; echo; fi



# Display compiler version information.
gccversion : 
        @$(CC) --version



# Program the device.  
program: $(TARGET).hex $(TARGET).eep
        $(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set 
#     a breakpoint at main().
gdb-config: 
        @$(REMOVE) $(GDBINIT_FILE)
        @echo define reset >> $(GDBINIT_FILE)
        @echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
        @echo end >> $(GDBINIT_FILE)
        @echo file $(TARGET).elf >> $(GDBINIT_FILE)
        @echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
        @echo load  >> $(GDBINIT_FILE)
endif        
        @echo break main >> $(GDBINIT_FILE)

debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
        @echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
        @$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
        $(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
        @$(WINSHELL) /c pause

else
        @$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
        $(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
        @$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 



coff: $(TARGET).elf
        @echo
        @echo $(MSG_COFF) $(TARGET).cof
        $(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
        @echo
        @echo $(MSG_EXTENDED_COFF) $(TARGET).cof
        $(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof


# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
        @echo
        @echo $(MSG_FLASH) $@
        $(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
        @echo
        @echo $(MSG_EEPROM) $@
        -$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
        --change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
        @echo
        @echo $(MSG_EXTENDED_LISTING) $@
        $(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
        @echo
        @echo $(MSG_SYMBOL_TABLE) $@
        $(NM) -n $< > $@



# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
        @echo
        @echo $(MSG_LINKING) $@
        $(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
%.o : %.c
        @echo
        @echo $(MSG_COMPILING) $<
        $(CC) -c $(ALL_CFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
%.s : %.c
        $(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
%.o : %.S
        @echo
        @echo $(MSG_ASSEMBLING) $<
        $(CC) -c $(ALL_ASFLAGS) $< -o $@

# Create preprocessed source for use in sending a bug report.
%.i : %.c
        $(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@ 


# Target: clean project.
clean: begin clean_list end

clean_list :
        @echo
        @echo $(MSG_CLEANING)
        $(REMOVE) *.hex
        $(REMOVE) *.eep
        $(REMOVE) *.cof
        $(REMOVE) *.elf
        $(REMOVE) *.map
        $(REMOVE) *.sym
        $(REMOVE) *.lss
        $(REMOVE) $(OBJ)
        $(REMOVE) $(LST)
        $(REMOVE) $(SRC:.c=.s)
        $(REMOVE) $(SRC:.c=.d)
        $(REMOVE) .dep/*



# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list program debug gdb-config

      working-directory: Modification of the stock Arduino STK500v2 bootloader specifically for APM2 boards

There are several modifications

1. Correct sketch start if the board was rebooted due to watchdog timer

2. Fast sketch start if the USB cable is not sensed as being connected to the APM2 board

3. Flash the correct (ie, visible) LEDs in a more informative pattern in the bootloader

4. Removal of the !!! CLI feature of the stk500v2 bootloader

LED patterns to look for:

USB Connected, no traffic:
six slow blue flashes (while waiting for a serial character that does not come)
then.. sketch starts

USB Connected, some traffic:
any slow blue flashing is ceased and a short timer starts waiting for valid boot loader protocol
then.. sketch starts

USB Connected, valid boot loader traffic:
STK500 protocol packets (eg flashing a new sketch) cause rapid flashing of the blue LED for each packet

USB Not Connected
quick double blue flash
then.. quick single yellow flash

The sketch will have its own LED flashing pattern.

BUGS

If a USB cable is connected to power up the board, the boot loader starts the sketch instantly.
This is because the USB cable detection takes a short time to stabilize, by then, the bootloader
has moved on. This is not much of an issue as USB connection for the purposes of flashing the
firmware causes another reset when the USB port is opened, thus, the bootloader starts again
anyway.
      run: //**************************************************************************************************
//*
//*        interrupt vector names
//*
//*                It is important to note that the vector numbers listed here
//*                are the ATMEL documentation numbers. The Arduino numbers are 1 less
//*                This is because the Atmel docs start numbering the interrupts at 1
//*                when it is actually vector #0 in the table.
//**************************************************************************************************
//*        Jun  1,        2010        <MLS> Added support for ATmega1281
//*        Jun 30,        2010        <MLS> Putting in more ifdefs to conserve space
//*        Jul  3,        2010        <MLS> More #ifdefs to conserve space and testing on most of my boards
//*        Jul  4,        2010        <MLS> Started using vector defs for #ifdefs as defined in <avr/io.h>
//*        Jul 13,        2010        <MLS> Added support for __AVR_ATmega128__
//*        Aug 26,        2010        <MLS> Added support for __AVR_ATmega2561__
//**************************************************************************************************

//#include        "avrinterruptnames.h"

//**************************************************************************************************
//*        this defines the interrupt vectors and allows us to compile ONLY those strings that are actually 
//*        in the target CPU. This way we do not have to keep making changes based on cpu, it will be 
//*        automatic even if we add a new CPU
#ifndef _AVR_IO_H_
        #include        <avr/io.h>
#endif
//**************************************************************************************************

#ifdef __MWERKS__
        #define        prog_char        char
        #define        PGM_P                char *
#endif

        prog_char        gAvrInt_RESET[]                        PROGMEM        =        "RESET";
#ifdef INT0_vect
        prog_char        gAvrInt_INT0[]                        PROGMEM        =        "INT0";
#endif
#ifdef INT1_vect
        prog_char        gAvrInt_INT1[]                        PROGMEM        =        "INT1";
#endif
#ifdef INT2_vect
        prog_char        gAvrInt_INT2[]                        PROGMEM        =        "INT2";
#endif
#ifdef INT3_vect
        prog_char        gAvrInt_INT3[]                        PROGMEM        =        "INT3";
#endif
#ifdef INT4_vect
        prog_char        gAvrInt_INT4[]                        PROGMEM        =        "INT4";
#endif
#ifdef INT5_vect
        prog_char        gAvrInt_INT5[]                        PROGMEM        =        "INT5";
#endif
#ifdef INT6_vect
        prog_char        gAvrInt_INT6[]                        PROGMEM        =        "INT6";
#endif
#ifdef INT7_vect
        prog_char        gAvrInt_INT7[]                        PROGMEM        =        "INT7";
#endif
#ifdef PCINT0_vect
        prog_char        gAvrInt_PCINT0[]                PROGMEM        =        "PCINT0";
#endif
#ifdef PCINT1_vect
        prog_char        gAvrInt_PCINT1[]                PROGMEM        =        "PCINT1";
#endif
#ifdef PCINT2_vect
        prog_char        gAvrInt_PCINT2[]                PROGMEM        =        "PCINT2";
#endif
#ifdef PCINT3_vect
        prog_char        gAvrInt_PCINT3[]                PROGMEM        =        "PCINT3";
#endif
#ifdef WDT_vect
        prog_char        gAvrInt_WDT[]                        PROGMEM        =        "WDT";
#endif
#ifdef TIMER0_COMP_vect
        prog_char        gAvrInt_TIMER0_COMP[]        PROGMEM        =        "TIMER0 COMP";
#endif
#ifdef TIMER0_COMPA_vect
        prog_char        gAvrInt_TIMER0_COMPA[]        PROGMEM        =        "TIMER0 COMPA";
#endif
#ifdef TIMER0_COMPB_vect
        prog_char        gAvrInt_TIMER0_COMPB[]        PROGMEM        =        "TIMER0 COMPB";
#endif
#ifdef TIMER0_OVF_vect
        prog_char        gAvrInt_TIMER0_OVF[]        PROGMEM        =        "TIMER0 OVF";
#endif
#ifdef TIMER1_CAPT_vect
        prog_char        gAvrInt_TIMER1_CAPT[]        PROGMEM        =        "TIMER1 CAPT";
#endif
#ifdef TIMER1_COMPA_vect
        prog_char        gAvrInt_TIMER1_COMPA[]        PROGMEM        =        "TIMER1 COMPA";
#endif
#ifdef TIMER1_COMPB_vect
        prog_char        gAvrInt_TIMER1_COMPB[]        PROGMEM        =        "TIMER1 COMPB";
#endif
#ifdef TIMER1_COMPC_vect
        prog_char        gAvrInt_TIMER1_COMPC[]        PROGMEM        =        "TIMER1 COMPC";
#endif
#ifdef TIMER1_OVF_vect
        prog_char        gAvrInt_TIMER1_OVF[]        PROGMEM        =        "TIMER1 OVF";
#endif
#ifdef TIMER2_COMP_vect
        prog_char        gAvrInt_TIMER2_COMP[]        PROGMEM        =        "TIMER2 COMP";
#endif
#ifdef TIMER2_COMPA_vect
        prog_char        gAvrInt_TIMER2_COMPA[]        PROGMEM        =        "TIMER2 COMPA";
#endif
#ifdef TIMER2_COMPB_vect
        prog_char        gAvrInt_TIMER2_COMPB[]        PROGMEM        =        "TIMER2 COMPB";
#endif
#ifdef TIMER2_OVF_vect
        prog_char        gAvrInt_TIMER2_OVF[]        PROGMEM        =        "TIMER2 OVF";
#endif
#ifdef TIMER3_CAPT_vect
        prog_char        gAvrInt_TIMER3_CAPT[]        PROGMEM        =        "TIMER3 CAPT";
#endif
#ifdef TIMER3_COMPA_vect
        prog_char        gAvrInt_TIMER3_COMPA[]        PROGMEM        =        "TIMER3 COMPA";
#endif
#ifdef TIMER3_COMPB_vect
        prog_char        gAvrInt_TIMER3_COMPB[]        PROGMEM        =        "TIMER3 COMPB";
#endif
#ifdef TIMER3_COMPC_vect
        prog_char        gAvrInt_TIMER3_COMPC[]        PROGMEM        =        "TIMER3 COMPC";
#endif
#ifdef TIMER3_OVF_vect
        prog_char        gAvrInt_TIMER3_OVF[]        PROGMEM        =        "TIMER3 OVF";
#endif
#ifdef TIMER4_CAPT_vect
        prog_char        gAvrInt_TIMER4_CAPT[]        PROGMEM        =        "TIMER4 CAPT";
#endif
#ifdef TIMER4_COMPA_vect
        prog_char        gAvrInt_TIMER4_COMPA[]        PROGMEM        =        "TIMER4 COMPA";
#endif
#ifdef TIMER4_COMPB_vect
        prog_char        gAvrInt_TIMER4_COMPB[]        PROGMEM        =        "TIMER4 COMPB";
#endif
#ifdef TIMER4_COMPC_vect
        prog_char        gAvrInt_TIMER4_COMPC[]        PROGMEM        =        "TIMER4 COMPC";
#endif
#ifdef TIMER4_COMPD_vect
        prog_char        gAvrInt_TIMER4_COMPD[]        PROGMEM        =        "TIMER4 COMPD";
#endif
#ifdef TIMER4_OVF_vect
        prog_char        gAvrInt_TIMER4_OVF[]        PROGMEM        =        "TIMER4 OVF";
#endif
#ifdef TIMER4_FPF_vect
        prog_char        gAvrInt_TIMER4_FPF[]        PROGMEM        =        "TIMER4 Fault Protection";
#endif
#ifdef TIMER5_CAPT_vect
        prog_char        gAvrInt_TIMER5_CAPT[]        PROGMEM        =        "TIMER5 CAPT";
#endif
#ifdef TIMER5_COMPA_vect
        prog_char        gAvrInt_TIMER5_COMPA[]        PROGMEM        =        "TIMER5 COMPA";
#endif
#ifdef TIMER5_COMPB_vect
        prog_char        gAvrInt_TIMER5_COMPB[]        PROGMEM        =        "TIMER5 COMPB";
#endif
#ifdef TIMER5_COMPC_vect
        prog_char        gAvrInt_TIMER5_COMPC[]        PROGMEM        =        "TIMER5 COMPC";
#endif
#ifdef TIMER5_OVF_vect
        prog_char        gAvrInt_TIMER5_OVF[]        PROGMEM        =        "TIMER5 OVF";
#endif

//*        when there is only 1 usart
#if defined(USART_RX_vect) || defined(USART_RXC_vect)
        prog_char        gAvrInt_USART_RX[]                PROGMEM        =        "USART RX";
#endif
#if defined(USART_UDRE_vect)
        prog_char        gAvrInt_USART_UDRE[]        PROGMEM        =        "USART UDRE";
#endif
#if defined(USART_TX_vect) || defined(USART_TXC_vect)
        prog_char        gAvrInt_USART_TX[]                PROGMEM        =        "USART TX";
#endif


//*        usart 0
#if defined(USART0_RX_vect)
        prog_char        gAvrInt_USART0_RX[]                PROGMEM        =        "USART0 RX";
#endif
#if defined(USART0_UDRE_vect)
        prog_char        gAvrInt_USART0_UDRE[]        PROGMEM        =        "USART0 UDRE";
#endif
#if defined(USART0_TX_vect)
        prog_char        gAvrInt_USART0_TX[]                PROGMEM        =        "USART0 TX";
#endif


//*        usart 1
#ifdef USART1_RX_vect
        prog_char        gAvrInt_USART1_RX[]                PROGMEM        =        "USART1 RX";
#endif
#ifdef USART1_UDRE_vect
        prog_char        gAvrInt_USART1_UDRE[]        PROGMEM        =        "USART1 UDRE";
#endif
#ifdef USART1_TX_vect
        prog_char        gAvrInt_USART1_TX[]                PROGMEM        =        "USART1 TX";
#endif

//*        usart 2
#ifdef USART2_RX_vect
        prog_char        gAvrInt_USART2_RX[]                PROGMEM        =        "USART2 RX";
#endif
#ifdef USART2_UDRE_vect
        prog_char        gAvrInt_USART2_UDRE[]        PROGMEM        =        "USART2 UDRE";
#endif
#ifdef USART2_TX_vect
        prog_char        gAvrInt_USART2_TX[]                PROGMEM        =        "USART2 TX";
#endif

//*        usart 3
#ifdef USART3_RX_vect
        prog_char        gAvrInt_USART3_RX[]                PROGMEM        =        "USART3 RX";
#endif
#ifdef USART3_UDRE_vect
        prog_char        gAvrInt_USART3_UDRE[]        PROGMEM        =        "USART3 UDRE";
#endif
#ifdef USART3_TX_vect
        prog_char        gAvrInt_USART3_TX[]                PROGMEM        =        "USART3 TX";
#endif
#ifdef SPI_STC_vect
        prog_char        gAvrInt_SPI_STC[]                PROGMEM        =        "SPI STC";
#endif
#ifdef ADC_vect
        prog_char        gAvrInt_ADC[]                        PROGMEM        =        "ADC";
#endif
#if defined(ANALOG_COMP_vect) || defined(ANA_COMP_vect)
        prog_char        gAvrInt_ANALOG_COMP[]        PROGMEM        =        "ANALOG COMP";
#endif
#if defined(EE_READY_vect) || defined(EE_RDY_vect)
        prog_char        gAvrInt_EE_READY[]                PROGMEM        =        "EE READY";
#endif
#ifdef TWI_vect
        prog_char        gAvrInt_TWI[]                        PROGMEM        =        "TWI";
#endif
#if defined(SPM_READY_vect) || defined(SPM_RDY_vect)
        prog_char        gAvrInt_SPM_READY[]                PROGMEM        =        "SPM READY";
#endif
#ifdef USI_START_vect
        prog_char        gAvrInt_USI_START[]                PROGMEM        =        "USI START";
#endif
#ifdef USI_OVERFLOW_vect
        prog_char        gAvrInt_USI_OVERFLOW[]        PROGMEM        =        "USI OVERFLOW";
#endif
#ifdef USB_GEN_vect
        prog_char        gAvrInt_USB_General[]        PROGMEM        =        "USB General";
#endif
#ifdef USB_COM_vect
        prog_char        gAvrInt_USB_Endpoint[]        PROGMEM        =        "USB Endpoint";
#endif

#ifdef LCD_vect
        prog_char        gAvrInt_LCD_StartFrame[]        PROGMEM        =        "LCD Start of Frame";
#endif


//**************************************************************************************************
//*        these do not have vector defs and have to be done by CPU type
#if defined(__AVR_ATmega645__ ) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__)
        prog_char        gAvrInt_NOT_USED[]                PROGMEM        =        "NOT_USED";
#endif
#if defined(__AVR_ATmega32U4__)
        prog_char        gAvrInt_RESERVED[]                PROGMEM        =        "Reserved";
#endif

        prog_char        gAvrInt_END[]                        PROGMEM        =        "*";





//**************************************************************************************************
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#pragma mark __AVR_ATmega168__ / __AVR_ATmega328P__

#define        _INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_INT1,                        //        3
        gAvrInt_PCINT0,                        //        4
        gAvrInt_PCINT1,                        //        5
        gAvrInt_PCINT2,                        //        6
        gAvrInt_WDT,                        //        7
        gAvrInt_TIMER2_COMPA,        //        8
        gAvrInt_TIMER2_COMPB,        //        9
        gAvrInt_TIMER2_OVF,                //        10
        gAvrInt_TIMER1_CAPT,        //        11
        gAvrInt_TIMER1_COMPA,        //        12
        gAvrInt_TIMER1_COMPB,        //        13
        gAvrInt_TIMER1_OVF,                //        14
        gAvrInt_TIMER0_COMPA,        //        15
        gAvrInt_TIMER0_COMPB,        //        16
        gAvrInt_TIMER0_OVF,                //        17
        gAvrInt_SPI_STC,                //        18
        gAvrInt_USART_RX,                //        19
        gAvrInt_USART_UDRE,        //        20
        gAvrInt_USART_TX,                //        21
        gAvrInt_ADC,                        //        22
        gAvrInt_EE_READY,                //        23
        gAvrInt_ANALOG_COMP,        //        24
        gAvrInt_TWI,                        //        25
        gAvrInt_SPM_READY,                //        26
};

#endif

//**************************************************************************************************
#pragma mark __AVR_ATmega169__
#if defined(__AVR_ATmega169__)

#define        _INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_PCINT0,                        //        3
        gAvrInt_PCINT1,                        //        4
        gAvrInt_TIMER2_COMP,        //        5
        gAvrInt_TIMER2_OVF,                //        6
        gAvrInt_TIMER1_CAPT,        //        7
        gAvrInt_TIMER1_COMPA,        //        8
        gAvrInt_TIMER1_COMPB,        //        9
        gAvrInt_TIMER1_OVF,                //        10
        gAvrInt_TIMER0_COMP,        //        11
        gAvrInt_TIMER0_OVF,                //        12
        gAvrInt_SPI_STC,                //        13
        gAvrInt_USART0_RX,                //        14
        gAvrInt_USART0_UDRE,        //        15
        gAvrInt_USART0_TX,                //        16
        gAvrInt_USI_START,                //        17
        gAvrInt_USI_OVERFLOW,        //        18
        gAvrInt_ANALOG_COMP,        //        19
        gAvrInt_ADC,                        //        20
        gAvrInt_EE_READY,                //        21
        gAvrInt_SPM_READY,                //        22
        gAvrInt_LCD_StartFrame,        //        23

};

#endif


//**************************************************************************************************
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
#pragma mark __AVR_ATmega640__ __AVR_ATmega1280__ __AVR_ATmega1281__ __AVR_ATmega2560__ __AVR_ATmega2561__

#define        _INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_INT1,                        //        3
        gAvrInt_INT2,                        //        4
        gAvrInt_INT3,                        //        5
        gAvrInt_INT4,                        //        6
        gAvrInt_INT5,                        //        7
        gAvrInt_INT6,                        //        8
        gAvrInt_INT7,                        //        9
        gAvrInt_PCINT0,                        //        10
        gAvrInt_PCINT1,                        //        11
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        gAvrInt_PCINT2,                        //        12
#else
        gAvrInt_NOT_USED,                //        12
#endif
        gAvrInt_WDT,                        //        13
        gAvrInt_TIMER2_COMPA,        //        14
        gAvrInt_TIMER2_COMPB,        //        15
        gAvrInt_TIMER2_OVF,                //        16
        gAvrInt_TIMER1_CAPT,        //        17
        gAvrInt_TIMER1_COMPA,        //        18
        gAvrInt_TIMER1_COMPB,        //        19
        gAvrInt_TIMER1_COMPC,        //        20
        gAvrInt_TIMER1_OVF,                //        21
        gAvrInt_TIMER0_COMPA,        //        22
        gAvrInt_TIMER0_COMPB,        //        23
        gAvrInt_TIMER0_OVF,                //        24
        gAvrInt_SPI_STC,                //        25

        gAvrInt_USART0_RX,                //        26
        gAvrInt_USART0_UDRE,        //        27
        gAvrInt_USART0_TX,                //        28
        gAvrInt_ANALOG_COMP,        //        29
        gAvrInt_ADC,                        //        30
        gAvrInt_EE_READY,                //        31

        gAvrInt_TIMER3_CAPT,        //        32
        gAvrInt_TIMER3_COMPA,        //        33
        gAvrInt_TIMER3_COMPB,        //        34
        gAvrInt_TIMER3_COMPC,        //        35
        gAvrInt_TIMER3_OVF,                //        36

        gAvrInt_USART1_RX,                //        37
        gAvrInt_USART1_UDRE,        //        38
        gAvrInt_USART1_TX,                //        39
        gAvrInt_TWI,                        //        40
        gAvrInt_SPM_READY,                //        41
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        gAvrInt_TIMER4_CAPT,        //        42
#else
        gAvrInt_NOT_USED,                //        42
#endif
        gAvrInt_TIMER4_COMPA,        //        43
        gAvrInt_TIMER4_COMPB,        //        44
        gAvrInt_TIMER4_COMPC,        //        45
        gAvrInt_TIMER4_OVF,                //        46
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        gAvrInt_TIMER5_CAPT,        //        47
#else
        gAvrInt_NOT_USED,                //        47
#endif
        gAvrInt_TIMER5_COMPA,        //        48
        gAvrInt_TIMER5_COMPB,        //        49
        gAvrInt_TIMER5_COMPC,        //        50
        gAvrInt_TIMER5_OVF,                //        51

#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        gAvrInt_USART2_RX,                //        52
        gAvrInt_USART2_UDRE,        //        53
        gAvrInt_USART2_TX,                //        54

        gAvrInt_USART3_RX,                //        55
        gAvrInt_USART3_UDRE,        //        56
        gAvrInt_USART3_TX,                //        57
#endif

};

#endif



//**************************************************************************************************
#if defined(__AVR_ATmega324P__ ) || defined(__AVR_ATmega644__ ) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
#pragma mark __AVR_ATmega324P__  __AVR_ATmega644__ __AVR_ATmega644P__ __AVR_ATmega1284P__

#define        _INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_INT1,                        //        3
        gAvrInt_INT2,                        //        4
        gAvrInt_PCINT0,                        //        5
        gAvrInt_PCINT1,                        //        6
        gAvrInt_PCINT2,                        //        7
        gAvrInt_PCINT3,                        //        8
        gAvrInt_WDT,                        //        9
        gAvrInt_TIMER2_COMPA,        //        10
        gAvrInt_TIMER2_COMPB,        //        11
        gAvrInt_TIMER2_OVF,                //        12
        gAvrInt_TIMER1_CAPT,        //        13
        gAvrInt_TIMER1_COMPA,        //        14
        gAvrInt_TIMER1_COMPB,        //        15
        gAvrInt_TIMER1_OVF,                //        16
        gAvrInt_TIMER0_COMPA,        //        17
        gAvrInt_TIMER0_COMPB,        //        18
        gAvrInt_TIMER0_OVF,                //        19
        gAvrInt_SPI_STC,                //        20
        gAvrInt_USART0_RX,                //        21
        gAvrInt_USART0_UDRE,        //        22
        gAvrInt_USART0_TX,                //        23
        gAvrInt_ANALOG_COMP,        //        24
        gAvrInt_ADC,                        //        25
        gAvrInt_EE_READY,                //        26
        gAvrInt_TWI,                        //        27
        gAvrInt_SPM_READY,                //        28

#if defined(__AVR_ATmega324P__ ) || defined(__AVR_ATmega644P__)
        gAvrInt_USART1_RX,                //        29
        gAvrInt_USART1_UDRE,        //        30
        gAvrInt_USART1_TX,                //        31
#endif

};


#endif

//**************************************************************************************************
#if defined(__AVR_ATmega645__ )
#pragma mark __AVR_ATmega645__

#define        _INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_PCINT0,                        //        3
        gAvrInt_PCINT1,                        //        4
        gAvrInt_TIMER2_COMP,        //        5
        gAvrInt_TIMER2_OVF,                //        6
        gAvrInt_TIMER1_CAPT,        //        7
        gAvrInt_TIMER1_COMPA,        //        8
        gAvrInt_TIMER1_COMPB,        //        9
        gAvrInt_TIMER1_OVF,                //        10
        gAvrInt_TIMER0_COMP,        //        11
        gAvrInt_TIMER0_OVF,                //        12
        gAvrInt_SPI_STC,                //        13
        gAvrInt_USART0_RX,                //        14
        gAvrInt_USART0_UDRE,        //        15
        gAvrInt_USART0_TX,                //        16
        gAvrInt_USI_START,                //        17
        gAvrInt_USI_OVERFLOW,        //        18
        gAvrInt_ANALOG_COMP,        //        19
        gAvrInt_ADC,                        //        20
        gAvrInt_EE_READY,                //        21
        gAvrInt_SPM_READY,                //        22
        gAvrInt_NOT_USED,                //        23

#if defined(__AVR_ATmega3250__) || defined(__AVR_ATmega6450__)
        gAvrInt_PCINT2,                        //        24
        gAvrInt_PCINT3,                        //        25
#endif
};


#endif


//**************************************************************************************************
#if defined(__AVR_ATmega32__ )
#pragma mark __AVR_ATmega32__

#define        _INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_INT1,                        //        3
        gAvrInt_INT2,                        //        4
        gAvrInt_TIMER2_COMP,        //        5
        gAvrInt_TIMER2_OVF,                //        6
        gAvrInt_TIMER1_CAPT,        //        7
        gAvrInt_TIMER1_COMPA,        //        8
        gAvrInt_TIMER1_COMPB,        //        9
        gAvrInt_TIMER1_OVF,                //        10
        gAvrInt_TIMER0_COMP,        //        11
        gAvrInt_TIMER0_OVF,                //        12
        gAvrInt_SPI_STC,                //        13
        gAvrInt_USART_RX,                //        14
        gAvrInt_USART_UDRE,                //        15
        gAvrInt_USART_TX,                //        16
        gAvrInt_ADC,                        //        17
        gAvrInt_EE_READY,                //        18
        gAvrInt_ANALOG_COMP,        //        19
        gAvrInt_TWI,                        //        20
        gAvrInt_SPM_READY,                //        21

};


#endif

//**************************************************************************************************
#if defined(__AVR_ATmega32U4__)
#pragma mark __AVR_ATmega32U4__
//*        teensy 2.0
//*        http://www.pjrc.com/teensy/pinout.html
#define        _INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_INT1,                        //        3
        gAvrInt_INT2,                        //        4
        gAvrInt_INT3,                        //        5
        gAvrInt_RESERVED,                //        6
        gAvrInt_RESERVED,                //        7
        gAvrInt_INT6,                        //        8
        gAvrInt_RESERVED,                //        9
        gAvrInt_PCINT0,                        //        10
        gAvrInt_USB_General,        //        11
        gAvrInt_USB_Endpoint,        //        12
        gAvrInt_WDT,                        //        13
        gAvrInt_RESERVED,                //        14
        gAvrInt_RESERVED,                //        15
        gAvrInt_RESERVED,                //        16
        gAvrInt_TIMER1_CAPT,        //        17
        gAvrInt_TIMER1_COMPA,        //        18
        gAvrInt_TIMER1_COMPB,        //        19
        gAvrInt_TIMER1_COMPC,        //        20
        gAvrInt_TIMER1_OVF,                //        21
        gAvrInt_TIMER0_COMPA,        //        22
        gAvrInt_TIMER0_COMPB,        //        23
        gAvrInt_TIMER0_OVF,                //        24
        gAvrInt_SPI_STC,                //        25

        gAvrInt_USART1_RX,                //        26
        gAvrInt_USART1_UDRE,        //        27
        gAvrInt_USART1_TX,                //        28
        gAvrInt_ANALOG_COMP,        //        29

        gAvrInt_ADC,                        //        30
        gAvrInt_EE_READY,                //        31

        gAvrInt_TIMER3_CAPT,        //        32
        gAvrInt_TIMER3_COMPA,        //        33
        gAvrInt_TIMER3_COMPB,        //        34
        gAvrInt_TIMER3_COMPC,        //        35
        gAvrInt_TIMER3_OVF,                //        36
        gAvrInt_TWI,                        //        37
        gAvrInt_SPM_READY,                //        38

        gAvrInt_TIMER4_COMPA,        //        39
        gAvrInt_TIMER4_COMPB,        //        40
        gAvrInt_TIMER4_COMPD,        //        41
        gAvrInt_TIMER4_OVF,                //        42
        gAvrInt_TIMER4_FPF,                //        43
};

#endif

//**************************************************************************************************
#if defined(__AVR_AT90USB1286__)
#pragma mark __AVR_AT90USB1286__
//*        teensy++ 2.0
//*        http://www.pjrc.com/teensy/pinout.html
#define        _INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_INT1,                        //        3
        gAvrInt_INT2,                        //        4
        gAvrInt_INT3,                        //        5
        gAvrInt_INT4,                        //        6
        gAvrInt_INT5,                        //        7
        gAvrInt_INT6,                        //        8
        gAvrInt_INT7,                        //        9
        gAvrInt_PCINT0,                        //        10
        gAvrInt_USB_General,        //        11
        gAvrInt_USB_Endpoint,        //        12
        gAvrInt_WDT,                        //        13
        gAvrInt_TIMER2_COMPA,        //        14
        gAvrInt_TIMER2_COMPB,        //        15
        gAvrInt_TIMER2_OVF,                //        16
        gAvrInt_TIMER1_CAPT,        //        17
        gAvrInt_TIMER1_COMPA,        //        18
        gAvrInt_TIMER1_COMPB,        //        19
        gAvrInt_TIMER1_COMPC,        //        20
        gAvrInt_TIMER1_OVF,                //        21
        gAvrInt_TIMER0_COMPA,        //        22
        gAvrInt_TIMER0_COMPB,        //        23
        gAvrInt_TIMER0_OVF,                //        24
        gAvrInt_SPI_STC,                //        25

        gAvrInt_USART1_RX,                //        26
        gAvrInt_USART1_UDRE,        //        27
        gAvrInt_USART1_TX,                //        28
        gAvrInt_ANALOG_COMP,        //        29

        gAvrInt_ADC,                        //        30
        gAvrInt_EE_READY,                //        31

        gAvrInt_TIMER3_CAPT,        //        32
        gAvrInt_TIMER3_COMPA,        //        33
        gAvrInt_TIMER3_COMPB,        //        34
        gAvrInt_TIMER3_COMPC,        //        35
        gAvrInt_TIMER3_OVF,                //        36
        gAvrInt_TWI,                        //        37
        gAvrInt_SPM_READY,                //        38

};

#endif




//**************************************************************************************************
#if defined(__AVR_ATmega128__)
#pragma mark __AVR_ATmega128__
#define        _INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]        PROGMEM =
{

        gAvrInt_RESET,                        //        1
        gAvrInt_INT0,                        //        2
        gAvrInt_INT1,                        //        3
        gAvrInt_INT2,                        //        4
        gAvrInt_INT3,                        //        5
        gAvrInt_INT4,                        //        6
        gAvrInt_INT5,                        //        7
        gAvrInt_INT6,                        //        8
        gAvrInt_INT7,                        //        9
        gAvrInt_TIMER2_COMP,        //        10
        gAvrInt_TIMER2_OVF,                //        11
        gAvrInt_TIMER1_CAPT,        //        12
        gAvrInt_TIMER1_COMPA,        //        13
        gAvrInt_TIMER1_COMPB,        //        14
        gAvrInt_TIMER1_OVF,                //        15
        gAvrInt_TIMER0_COMP,        //        16
        gAvrInt_TIMER0_OVF,                //        17
        gAvrInt_SPI_STC,                //        18
        gAvrInt_USART0_RX,                //        19
        gAvrInt_USART0_UDRE,        //        20
        gAvrInt_USART0_TX,                //        21
        gAvrInt_ADC,                        //        22
        gAvrInt_EE_READY,                //        23
        gAvrInt_ANALOG_COMP,        //        24
        gAvrInt_TIMER1_COMPC,        //        25
        gAvrInt_TIMER3_CAPT,        //        26
        gAvrInt_TIMER3_COMPA,        //        27
        gAvrInt_TIMER3_COMPB,        //        28
        gAvrInt_TIMER3_COMPC,        //        29
        gAvrInt_TIMER3_OVF,                //        30
        gAvrInt_USART1_RX,                //        31
        gAvrInt_USART1_UDRE,        //        32
        gAvrInt_USART1_TX,                //        33
        gAvrInt_TWI,                        //        34
        gAvrInt_SPM_READY,                //        35

};

#endif

#if !defined(_INTERRUPT_NAMES_DEFINED_)
        #warning        No interrupt string defs for this cpu
#endif

    - name: //**** ATMEL AVR - A P P L I C A T I O N   N O T E  ************************
//*
//* Title:                AVR068 - STK500 Communication Protocol
//* Filename:                command.h
//* Version:                1.0
//* Last updated:        31.01.2005
//*
//* Support E-mail:        avr@atmel.com
//*
//**************************************************************************

// *****************[ STK message constants ]***************************

#define MESSAGE_START                       0x1B        //= ESC = 27 decimal
#define TOKEN                               0x0E

// *****************[ STK general command constants ]**************************

#define CMD_SIGN_ON                         0x01
#define CMD_SET_PARAMETER                   0x02
#define CMD_GET_PARAMETER                   0x03
#define CMD_SET_DEVICE_PARAMETERS           0x04
#define CMD_OSCCAL                          0x05
#define CMD_LOAD_ADDRESS                    0x06
#define CMD_FIRMWARE_UPGRADE                0x07


// *****************[ STK ISP command constants ]******************************

#define CMD_ENTER_PROGMODE_ISP              0x10
#define CMD_LEAVE_PROGMODE_ISP              0x11
#define CMD_CHIP_ERASE_ISP                  0x12
#define CMD_PROGRAM_FLASH_ISP               0x13
#define CMD_READ_FLASH_ISP                  0x14
#define CMD_PROGRAM_EEPROM_ISP              0x15
#define CMD_READ_EEPROM_ISP                 0x16
#define CMD_PROGRAM_FUSE_ISP                0x17
#define CMD_READ_FUSE_ISP                   0x18
#define CMD_PROGRAM_LOCK_ISP                0x19
#define CMD_READ_LOCK_ISP                   0x1A
#define CMD_READ_SIGNATURE_ISP              0x1B
#define CMD_READ_OSCCAL_ISP                 0x1C
#define CMD_SPI_MULTI                       0x1D

// *****************[ STK PP command constants ]*******************************

#define CMD_ENTER_PROGMODE_PP               0x20
#define CMD_LEAVE_PROGMODE_PP               0x21
#define CMD_CHIP_ERASE_PP                   0x22
#define CMD_PROGRAM_FLASH_PP                0x23
#define CMD_READ_FLASH_PP                   0x24
#define CMD_PROGRAM_EEPROM_PP               0x25
#define CMD_READ_EEPROM_PP                  0x26
#define CMD_PROGRAM_FUSE_PP                 0x27
#define CMD_READ_FUSE_PP                    0x28
#define CMD_PROGRAM_LOCK_PP                 0x29
#define CMD_READ_LOCK_PP                    0x2A
#define CMD_READ_SIGNATURE_PP               0x2B
#define CMD_READ_OSCCAL_PP                  0x2C    

#define CMD_SET_CONTROL_STACK               0x2D

// *****************[ STK HVSP command constants ]*****************************

#define CMD_ENTER_PROGMODE_HVSP             0x30
#define CMD_LEAVE_PROGMODE_HVSP             0x31
#define CMD_CHIP_ERASE_HVSP                 0x32
#define CMD_PROGRAM_FLASH_HVSP  `     0x33
#define CMD_READ_FLASH_HVSP                 0x34
#define CMD_PROGRAM_EEPROM_HVSP             0x35
#define CMD_READ_EEPROM_HVSP                0x36
#define CMD_PROGRAM_FUSE_HVSP               0x37
#define CMD_READ_FUSE_HVSP                  0x38
#define CMD_PROGRAM_LOCK_HVSP               0x39
#define CMD_READ_LOCK_HVSP                  0x3A
#define CMD_READ_SIGNATURE_HVSP             0x3B
#define CMD_READ_OSCCAL_HVSP                0x3C

// *****************[ STK status constants ]***************************

// Success
#define STATUS_CMD_OK                       0x00

// Warnings
#define STATUS_CMD_TOUT                     0x80
#define STATUS_RDY_BSY_TOUT                 0x81
#define STATUS_SET_PARAM_MISSING            0x82

// Errors
#define STATUS_CMD_FAILED                   0xC0
#define STATUS_CKSUM_ERROR                  0xC1
#define STATUS_CMD_UNKNOWN                  0xC9

// *****************[ STK parameter constants ]***************************
#define PARAM_BUILD_NUMBER_LOW              0x80
#define PARAM_BUILD_NUMBER_HIGH             0x81
#define PARAM_HW_VER                        0x90
#define PARAM_SW_MAJOR                      0x91
#define PARAM_SW_MINOR                      0x92
#define PARAM_VTARGET                       0x94
#define PARAM_VADJUST                       0x95
#define PARAM_OSC_PSCALE                    0x96
#define PARAM_OSC_CMATCH                    0x97
#define PARAM_SCK_DURATION                  0x98
#define PARAM_TOPCARD_DETECT                0x9A
#define PARAM_STATUS                        0x9C
#define PARAM_DATA                          0x9D
#define PARAM_RESET_POLARITY                0x9E
#define PARAM_CONTROLLER_INIT               0x9F

// *****************[ STK answer constants ]***************************

#define ANSWER_CKSUM_ERROR                  0xB0
      working-directory: /*****************************************************************************
Title:     STK500v2 compatible bootloader
           Modified for Wiring board ATMega128-16MHz
Author:    Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:      $Id: stk500boot.c,v 1.11 2006/06/25 12:39:17 peter Exp $
Compiler:  avr-gcc 3.4.5 or 4.1 / avr-libc 1.4.3
Hardware:  All AVRs with bootloader support, tested with ATmega8
License:   GNU General Public License

Modified:  Worapoht Kornkaewwattanakul <dev@avride.com>   http://www.avride.com
Date:      17 October 2007
Update:    1st, 29 Dec 2007 : Enable CMD_SPI_MULTI but ignore unused command by return 0x00 byte response..
Compiler:  WINAVR20060421
Description: add timeout feature like previous Wiring bootloader

DESCRIPTION:
    This program allows an AVR with bootloader capabilities to
    read/write its own Flash/EEprom. To enter Programming mode
    an input pin is checked. If this pin is pulled low, programming mode
    is entered. If not, normal execution is done from $0000
    "reset" vector in Application area.
    Size fits into a 1024 word bootloader section
        when compiled with avr-gcc 4.1
        (direct replace on Wiring Board without fuse setting changed)

USAGE:
    - Set AVR MCU type and clock-frequency (F_CPU) in the Makefile.
    - Set baud rate below (AVRISP only works with 115200 bps)
    - compile/link the bootloader with the supplied Makefile
    - program the "Boot Flash section size" (BOOTSZ fuses),
      for boot-size 1024 words:  program BOOTSZ01
    - enable the BOOT Reset Vector (program BOOTRST)
    - Upload the hex file to the AVR using any ISP programmer
    - Program Boot Lock Mode 3 (program BootLock 11 and BootLock 12 lock bits) // (leave them)
    - Reset your AVR while keeping PROG_PIN pulled low // (for enter bootloader by switch)
    - Start AVRISP Programmer (AVRStudio/Tools/Program AVR)
    - AVRISP will detect the bootloader
    - Program your application FLASH file and optional EEPROM file using AVRISP

Note:
    Erasing the device without flashing, through AVRISP GUI button "Erase Device"
    is not implemented, due to AVRStudio limitations.
    Flash is always erased before programming.

        AVRdude:
        Please uncomment #define REMOVE_CMD_SPI_MULTI when using AVRdude.
        Comment #define REMOVE_PROGRAM_LOCK_BIT_SUPPORT to reduce code size
        Read Fuse Bits and Read/Write Lock Bits is not supported

NOTES:
    Based on Atmel Application Note AVR109 - Self-programming
    Based on Atmel Application Note AVR068 - STK500v2 Protocol

LICENSE:
    Copyright (C) 2006 Peter Fleury

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*****************************************************************************/

//************************************************************************
//*        Edit History
//************************************************************************
//*        Jul  7,        2010        <MLS> = Mark Sproul msproul@skycharoit.com
//*        Jul  7,        2010        <MLS> Working on mega2560. No Auto-restart
//*        Jul  7,        2010        <MLS> Switched to 8K bytes (4K words) so that we have room for the monitor
//*        Jul  8,        2010        <MLS> Found older version of source that had auto restart, put that code back in
//*        Jul  8,        2010        <MLS> Adding monitor code
//*        Jul 11,        2010        <MLS> Added blinking LED while waiting for download to start
//*        Jul 11,        2010        <MLS> Added EEPROM test
//*        Jul 29,        2010        <MLS> Added recchar_timeout for timing out on bootloading
//*        Aug 23,        2010        <MLS> Added support for atmega2561
//*        Aug 26,        2010        <MLS> Removed support for BOOT_BY_SWITCH
//*        Sep  8,        2010        <MLS> Added support for atmega16
//************************************************************************



#include        <inttypes.h>
#include        <avr/io.h>
#include        <avr/interrupt.h>
#include        <avr/boot.h>
#include        <avr/pgmspace.h>
#include        <util/delay.h>
#include        <avr/eeprom.h>
#include        <avr/common.h>
#include        <stdlib.h>
#include        "command.h"


//#if defined(_MEGA_BOARD_) || defined(_BOARD_AMBER128_) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) || defined(__AVR_ATmega1284P__) || defined(ENABLE_MONITOR)
//        #define                ENABLE_MONITOR
//        static void        RunMonitor(void);
//#endif

//#define        _DEBUG_SERIAL_
#define        _DEBUG_WITH_LEDS_

/*
 * Uncomment the following lines to save code space
 */
//#define        REMOVE_PROGRAM_LOCK_BIT_SUPPORT                // disable program lock bits
//#define        REMOVE_BOOTLOADER_LED                                // no LED to show active bootloader
//#define        REMOVE_CMD_SPI_MULTI                                // disable processing of SPI_MULTI commands, Remark this line for AVRDUDE <Worapoht>
//



//************************************************************************
//*        LED on pin "PROGLED_PIN" on port "PROGLED_PORT"
//*        indicates that bootloader is active
//*        PG2 -> LED on Wiring board
//************************************************************************
#define                BLINK_LED_WHILE_WAITING

#ifdef _MEGA_BOARD_
/*
        #define PROGLED_PORT        PORTB
        #define PROGLED_DDR                DDRB
        #define PROGLED_PIN                PINB7
*/
// APM2 -- 27,26,25 
//    PA  , // PA 3 ** 25 ** D25
//    PA  , // PA 4 ** 26 ** D26
//    PA  , // PA 5 ** 27 ** D27
    #define PROGLED_PORT    PORTA
    #define PROGLED_DDR     DDRA
    #define PROGLED_PIN     PINB3  // LED C the bright blue one
#elif defined( _BOARD_AMBER128_ )
        //*        this is for the amber 128 http://www.soc-robotics.com/
        //*        onbarod led is PORTE4
        #define PROGLED_PORT        PORTD
        #define PROGLED_DDR                DDRD
        #define PROGLED_PIN                PINE7
#elif defined( _CEREBOTPLUS_BOARD_ )
        //*        this is for the Cerebot 2560 board
        //*        onbarod leds are on PORTE4-7
        #define PROGLED_PORT        PORTE
        #define PROGLED_DDR                DDRE
        #define PROGLED_PIN                PINE7
#elif defined( _PENGUINO_ )
        //*        this is for the Penguino
        //*        onbarod led is PORTE4
        #define PROGLED_PORT        PORTC
        #define PROGLED_DDR                DDRC
        #define PROGLED_PIN                PINC6
#elif defined( _ANDROID_2561_ ) || defined( __AVR_ATmega2561__ )
        //*        this is for the Boston Android 2561
        //*        onbarod led is PORTE4
        #define PROGLED_PORT        PORTA
        #define PROGLED_DDR                DDRA
        #define PROGLED_PIN                PINA3
#elif defined( BOARD_MEGA16 )
        //*        onbarod led is PORTA7
        #define PROGLED_PORT        PORTA
        #define PROGLED_DDR                DDRA
        #define PROGLED_PIN                PINA7
        #define UART_BAUDRATE_DOUBLE_SPEED 0

#elif defined( _BOARD_BAHBOT_ )
        //*        dosent have an onboard LED but this is what will probably be added to this port
        #define PROGLED_PORT        PORTB
        #define PROGLED_DDR                DDRB
        #define PROGLED_PIN                PINB0

#elif defined( _BOARD_ROBOTX_ )
        #define PROGLED_PORT        PORTB
        #define PROGLED_DDR                DDRB
        #define PROGLED_PIN                PINB6
#else
        #define PROGLED_PORT        PORTG
        #define PROGLED_DDR                DDRG
        #define PROGLED_PIN                PING2
#endif



/*
 * define CPU frequency in Mhz here if not defined in Makefile
 */
#ifndef F_CPU
        #define F_CPU 16000000UL
#endif

/*
 * UART Baudrate, AVRStudio AVRISP only accepts 115200 bps
 */

#ifndef BAUDRATE
        #define BAUDRATE 115200
#endif

/*
 *  Enable (1) or disable (0) USART double speed operation
 */
#ifndef UART_BAUDRATE_DOUBLE_SPEED
        #if defined (__AVR_ATmega32__)
                #define UART_BAUDRATE_DOUBLE_SPEED 0
        #else
                #define UART_BAUDRATE_DOUBLE_SPEED 1
        #endif
#endif

/*
 * HW and SW version, reported to AVRISP, must match version of AVRStudio
 */
#define CONFIG_PARAM_BUILD_NUMBER_LOW        0
#define CONFIG_PARAM_BUILD_NUMBER_HIGH        0
#define CONFIG_PARAM_HW_VER                                0x0F
#define CONFIG_PARAM_SW_MAJOR                        2
#define CONFIG_PARAM_SW_MINOR                        0x0A

/*
 * Calculate the address where the bootloader starts from FLASHEND and BOOTSIZE
 * (adjust BOOTSIZE below and BOOTLOADER_ADDRESS in Makefile if you want to change the size of the bootloader)
 */
//#define BOOTSIZE 1024
#if FLASHEND > 0x0F000
        #define BOOTSIZE 8192
#else
        #define BOOTSIZE 2048
#endif

#define APP_END  (FLASHEND -(2*BOOTSIZE) + 1)

/*
 * Signature bytes are not available in avr-gcc io_xxx.h
 */
#if defined (__AVR_ATmega8__)
        #define SIGNATURE_BYTES 0x1E9307
#elif defined (__AVR_ATmega16__)
        #define SIGNATURE_BYTES 0x1E9403
#elif defined (__AVR_ATmega32__)
        #define SIGNATURE_BYTES 0x1E9502
#elif defined (__AVR_ATmega8515__)
        #define SIGNATURE_BYTES 0x1E9306
#elif defined (__AVR_ATmega8535__)
        #define SIGNATURE_BYTES 0x1E9308
#elif defined (__AVR_ATmega162__)
        #define SIGNATURE_BYTES 0x1E9404
#elif defined (__AVR_ATmega128__)
        #define SIGNATURE_BYTES 0x1E9702
#elif defined (__AVR_ATmega1280__)
        #define SIGNATURE_BYTES 0x1E9703
#elif defined (__AVR_ATmega2560__)
        #define SIGNATURE_BYTES 0x1E9801
#elif defined (__AVR_ATmega2561__)
        #define SIGNATURE_BYTES 0x1e9802
#elif defined (__AVR_ATmega1284P__)
        #define SIGNATURE_BYTES 0x1e9705
#elif defined (__AVR_ATmega640__)
        #define SIGNATURE_BYTES  0x1e9608
#else
        #error "no signature definition for MCU available"
#endif



#if defined(_BOARD_ROBOTX_)
        #define        UART_BAUD_RATE_LOW                        UBRR1L
        #define        UART_STATUS_REG                                UCSR1A
        #define        UART_CONTROL_REG                        UCSR1B
        #define        UART_ENABLE_TRANSMITTER                TXEN1
        #define        UART_ENABLE_RECEIVER                RXEN1
        #define        UART_TRANSMIT_COMPLETE                TXC1
        #define        UART_RECEIVE_COMPLETE                RXC1
        #define        UART_DATA_REG                                UDR1
        #define        UART_DOUBLE_SPEED                        U2X1

#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
        || defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
        /* ATMega8 with one USART */
        #define        UART_BAUD_RATE_LOW                        UBRRL
        #define        UART_STATUS_REG                                UCSRA
        #define        UART_CONTROL_REG                        UCSRB
        #define        UART_ENABLE_TRANSMITTER                TXEN
        #define        UART_ENABLE_RECEIVER                RXEN
        #define        UART_TRANSMIT_COMPLETE                TXC
        #define        UART_RECEIVE_COMPLETE                RXC
        #define        UART_DATA_REG                                UDR
        #define        UART_DOUBLE_SPEED                        U2X

#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega162__) \
         || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
        /* ATMega with two USART, use UART0 */
        #define        UART_BAUD_RATE_LOW                        UBRR0L
        #define        UART_STATUS_REG                                UCSR0A
        #define        UART_CONTROL_REG                        UCSR0B
        #define        UART_ENABLE_TRANSMITTER                TXEN0
        #define        UART_ENABLE_RECEIVER                RXEN0
        #define        UART_TRANSMIT_COMPLETE                TXC0
        #define        UART_RECEIVE_COMPLETE                RXC0
        #define        UART_DATA_REG                                UDR0
        #define        UART_DOUBLE_SPEED                        U2X0
#elif defined(UBRR0L) && defined(UCSR0A) && defined(TXEN0)
        /* ATMega with two USART, use UART0 */
        #define        UART_BAUD_RATE_LOW                        UBRR0L
        #define        UART_STATUS_REG                                UCSR0A
        #define        UART_CONTROL_REG                        UCSR0B
        #define        UART_ENABLE_TRANSMITTER                TXEN0
        #define        UART_ENABLE_RECEIVER                RXEN0
        #define        UART_TRANSMIT_COMPLETE                TXC0
        #define        UART_RECEIVE_COMPLETE                RXC0
        #define        UART_DATA_REG                                UDR0
        #define        UART_DOUBLE_SPEED                        U2X0
#else
        #error "no UART definition for MCU available"
#endif



/*
 * Macro to calculate UBBR from XTAL and baudrate
 */
#if defined(__AVR_ATmega32__) && UART_BAUDRATE_DOUBLE_SPEED
        #define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu / 4 / baudRate - 1) / 2)
#elif defined(__AVR_ATmega32__)
        #define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu / 8 / baudRate - 1) / 2)
#elif UART_BAUDRATE_DOUBLE_SPEED
        #define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*8.0)-1.0+0.5)
#else
        #define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*16.0)-1.0+0.5)
#endif


/*
 * States used in the receive state machine
 */
#define        ST_START                0
#define        ST_GET_SEQ_NUM        1
#define ST_MSG_SIZE_1        2
#define ST_MSG_SIZE_2        3
#define ST_GET_TOKEN        4
#define ST_GET_DATA                5
#define        ST_GET_CHECK        6
#define        ST_PROCESS                7

/*
 * use 16bit address variable for ATmegas with <= 64K flash
 */
#if defined(RAMPZ)
        typedef uint32_t address_t;
#else
        typedef uint16_t address_t;
#endif

/*
 * function prototypes
 */
static void sendchar(char c);
static unsigned char recchar(void);

/*
 * since this bootloader is not linked against the avr-gcc crt1 functions,
 * to reduce the code size, we need to provide our own initialization
 */
void __jumpMain        (void) __attribute__ ((naked)) __attribute__ ((section (".init9")));
#include <avr/sfr_defs.h>

//#define        SPH_REG        0x3E
//#define        SPL_REG        0x3D

//*****************************************************************************
void __jumpMain(void)
{
//*        July 17, 2010        <MLS> Added stack pointer initialzation
//*        the first line did not do the job on the ATmega128

        asm volatile ( ".set __stack, %0" :: "i" (RAMEND) );

//        ldi r16,high(RAMEND)
//        out SPH,r16 ; Set stack pointer to top of RAM

//        asm volatile ( "ldi        16, 0x10");
        asm volatile ( "ldi        16, %0" :: "i" (RAMEND >> 8) );
//        asm volatile ( "out 0x3E,16");
//        asm volatile ( "out %0,16" :: "i" (SPH_REG) );
        asm volatile ( "out %0,16" :: "i" (AVR_STACK_POINTER_HI_ADDR) );

//        asm volatile ( "ldi        16, 0x00");
        asm volatile ( "ldi        16, %0" :: "i" (RAMEND & 0x0ff) );
//        asm volatile ( "out 0x3d,16");
//        asm volatile ( "out %0,16" :: "i" (SPL_REG) );
        asm volatile ( "out %0,16" :: "i" (AVR_STACK_POINTER_LO_ADDR) );



        asm volatile ( "clr __zero_reg__" );                                                                        // GCC depends on register r1 set to 0
        asm volatile ( "out %0, __zero_reg__" :: "I" (_SFR_IO_ADDR(SREG)) );        // set SREG to 0
//        asm volatile ( "rjmp main");                                                                                        // jump to main()
        asm volatile ( "jmp main");                                                                                                // jump to main()
}


//*****************************************************************************
void delay_ms(unsigned int timedelay)
{
        unsigned int i;
        for (i=0;i<timedelay;i++)
        {
                _delay_ms(0.5);
        }
}


//*****************************************************************************
/*
 * send single byte to USART, wait until transmission is completed
 */
static void sendchar(char c)
{
        UART_DATA_REG        =        c;                                                                                // prepare transmission
        while (!(UART_STATUS_REG & (1 << UART_TRANSMIT_COMPLETE)));        // wait until byte sent
        UART_STATUS_REG |= (1 << UART_TRANSMIT_COMPLETE);                        // delete TXCflag
}


//************************************************************************
static int        Serial_Available(void)
{
        return(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE));        // wait for data
}


//*****************************************************************************
/*
 * Read single byte from USART, block if no data available
 */
static unsigned char recchar(void)
{
        while (!(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE)))
        {
                // wait for data
        }
        return UART_DATA_REG;
}

#define        MAX_TIME_COUNT        (F_CPU >> 1)
//*****************************************************************************
static unsigned char recchar_timeout(void)
{
uint32_t count = 0;

        while (!(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE)))
        {
                // wait for data
                count++;
                if (count > MAX_TIME_COUNT)
                {
                unsigned int        data;
                #if (FLASHEND > 0x10000)
                        data        =        pgm_read_word_far(0);        //*        get the first word of the user program
                #else
                        data        =        pgm_read_word_near(0);        //*        get the first word of the user program
                #endif
                        if (data != 0xffff)                                        //*        make sure it's valid before jumping to it.
                        {
                                asm volatile(
                                                "clr        r30                \n\t"
                                                "clr        r31                \n\t"
                                                "ijmp        \n\t"
                                                );
                        }
                        count        =        0;
                }
        }
        return UART_DATA_REG;
}

void (*app_start)(void) = 0x0000;

//*****************************************************************************
int main(void)
{
        address_t                address                        =        0;
        address_t                eraseAddress        =        0;
        unsigned char        msgParseState;
        unsigned int        ii                                =        0;
        unsigned char        checksum                =        0;
        unsigned char        seqNum                        =        0;
        unsigned int        msgLength                =        0;
        unsigned char        msgBuffer[285];
        unsigned char        c, *p;
        unsigned char   isLeave = 0;

        unsigned long        boot_timeout;
        unsigned long        boot_timer;
        unsigned int        boot_state;
#ifdef ENABLE_MONITOR
        unsigned int        exPointCntr        =        0;
#endif

        uint8_t ch;
        ch = MCUSR;

        __asm__ __volatile__ ("cli");
        __asm__ __volatile__ ("wdr");
        MCUSR = 0;
        WDTCSR |= _BV(WDCE) | _BV(WDE);
        WDTCSR = 0;
        __asm__ __volatile__ ("sei");

        // check if WDT generated the reset, if so, go straight to app
        if (ch & _BV(WDRF)) {
            asm volatile(
                    "clr        r30                \n\t"
                    "clr        r31                \n\t"
                    "ijmp        \n\t"
                    );
    }

        boot_timer        =        0;
        boot_state        =        0;

#ifdef BLINK_LED_WHILE_WAITING
        boot_timeout        =         90000;                //*        should be about 4 seconds
#else
        boot_timeout        =        3500000;    // 7 seconds , approx 2us per step when optimize "s"
#endif
        /*
         * Branch to bootloader or application code ?
         */

        /*
         * Init UART
         * set baudrate and enable USART receiver and transmiter without interrupts
         */
#if UART_BAUDRATE_DOUBLE_SPEED
        UART_STATUS_REG                |=        (1 <<UART_DOUBLE_SPEED);
#endif
        UART_BAUD_RATE_LOW        =        UART_BAUD_SELECT(BAUDRATE,F_CPU);
        UART_CONTROL_REG        =        (1 << UART_ENABLE_RECEIVER) | (1 << UART_ENABLE_TRANSMITTER);

        asm volatile ("nop");                        // wait until port has changed

#ifndef REMOVE_BOOTLOADER_LED
        /* PROG_PIN pulled low, indicate with LED that bootloader is active */
        PROGLED_DDR                |=        ((1<<PINB3)|(1<<PINB4)|(1<<PINB5));
    // turn them all off
        PROGLED_PORT        |=        ((1<<PINB3)|(1<<PINB4)|(1<<PINB5));

#endif

#if defined(__AVR_ATmega2560__)
    // Check for hardware ident as an APM2 board
    // if (PINK & 3) { // 2560 A8 and A9 are HIGH
        // pin 23 is PA1,23,D23  and is BV(1) --> DDRA

        int usb_connected = !(PINA & 2);
        if (!usb_connected) {
            // Start sketch immediately if a USB connection is not detected
            for (ii=0; ii<2; ii++)
            {
                PROGLED_PORT        &=        ~(1<<PROGLED_PIN);        // turn LED on
                delay_ms(100);
                PROGLED_PORT        |=        (1<<PROGLED_PIN);        // turn LED off
                delay_ms(100);
            }
            for (ii=0; ii<2; ii++)
            {
                PROGLED_PORT        &=        ~(1<<PINB4);            // turn LED on
                delay_ms(100);
                PROGLED_PORT        |=        (1<<PINB4);        // turn LED off
            }
            delay_ms(500);
            asm volatile(
                    "clr        r30                \n\t"
                    "clr        r31                \n\t"
                    "ijmp        \n\t"
                    );
            //app_start();
                for(;;);
        } 
    //}
#endif

#ifdef _DEBUG_SERIAL_
//        delay_ms(500);

        sendchar('s');
        sendchar('t');
        sendchar('k');
//        sendchar('5');
//        sendchar('0');
//        sendchar('0');
        sendchar('v');
        sendchar('2');
        sendchar(0x0d);
        sendchar(0x0a);

        delay_ms(100);
#endif

        while (boot_state==0)
        {
                while ((!(Serial_Available())) && (boot_state == 0))                // wait for data
                {
                        _delay_ms(0.001);
                        boot_timer++;
                        if (boot_timer > boot_timeout)
                        {
                                boot_state        =        1; // (after ++ -> boot_state=2 bootloader timeout, jump to main 0x00000 )
                        }
                #ifdef BLINK_LED_WHILE_WAITING
                        if ((boot_timer % 7000) == 0)
                        {
                                //*        toggle the LED
                                PROGLED_PORT        ^=        (1<<PROGLED_PIN);        // turn LED ON
                        }
                #endif
                }
        boot_state++; // ( if boot_state=1 bootloader received byte from UART, enter bootloader mode)
        }

    boot_timer = 0;
        if (boot_state==1)
        {
                //*        main loop
                while (!isLeave)
                {
                        /*
                         * Collect received bytes to a complete message
                         */
                        msgParseState        =        ST_START;
                        while ( msgParseState != ST_PROCESS )
                        {
                if (boot_timer > 32) {
                    isLeave = 1;
                    msgParseState = 99;
                    break;
                }

                                if (boot_state==1)
                                {
                                        boot_state        =        0;
                                        c                        =        UART_DATA_REG;
                                }
                                else
                                {
                                //        c        =        recchar();
                                        c        =        recchar_timeout();
                                }

                        #ifdef ENABLE_MONITOR
                                if (c == '!')
                                {
                                        exPointCntr++;
                                        if (exPointCntr == 3)
                                        {
                                                RunMonitor();
                                                exPointCntr                =        0;        //        reset back to zero so we don't get in an endless loop
                                                isLeave                        =        1;
                                                msgParseState        =        99;        //*        we don't want it do anything
                                                break;
                                        }
                                }
                                else
                                {
                                        exPointCntr        =        0;
                                }
                        #endif

                                switch (msgParseState)
                                {
                                        case ST_START:
                                                if ( c == MESSAGE_START )
                                                {
                                                        msgParseState        =        ST_GET_SEQ_NUM;
                                                        checksum                =        MESSAGE_START^0;
                                                } else {
                            boot_timer++;
                        }
                                                break;

                                        case ST_GET_SEQ_NUM:
                                                if ( (c == 1) || (c == seqNum) )
                                                {
                                                        seqNum                        =        c;
                                                        msgParseState        =        ST_MSG_SIZE_1;
                                                        checksum                ^=        c;
                                                }
                                                else
                                                {
                                                        msgParseState        =        ST_START;
                                                }
                                                break;

                                        case ST_MSG_SIZE_1:
                                                msgLength                =        c<<8;
                                                msgParseState        =        ST_MSG_SIZE_2;
                                                checksum                ^=        c;
                                                break;

                                        case ST_MSG_SIZE_2:
                                                msgLength                |=        c;
                                                msgParseState        =        ST_GET_TOKEN;
                                                checksum                ^=        c;
                                                break;

                                        case ST_GET_TOKEN:
                                                if ( c == TOKEN )
                                                {
                                                        msgParseState        =        ST_GET_DATA;
                                                        checksum                ^=        c;
                                                        ii                                =        0;
                                                }
                                                else
                                                {
                                                        msgParseState        =        ST_START;
                                                }
                                                break;

                                        case ST_GET_DATA:
                                                msgBuffer[ii++]        =        c;
                                                checksum                ^=        c;
                                                if (ii == msgLength )
                                                {
                                                        msgParseState        =        ST_GET_CHECK;
                                                }
                                                break;

                                        case ST_GET_CHECK:
                                                if ( c == checksum )
                                                {
                                                        msgParseState        =        ST_PROCESS;
                                                }
                                                else
                                                {
                                                        msgParseState        =        ST_START;
                                                }
                                                break;
                                }        //        switch
                        }        //        while(msgParseState)

            if (msgParseState == 99) {
                continue;
            }

                        /*
                         * Now process the STK500 commands, see Atmel Appnote AVR068
                         */

                        switch (msgBuffer[0])
                        {
        #ifndef REMOVE_CMD_SPI_MULTI
                                case CMD_SPI_MULTI:
                                        {
                                                unsigned char answerByte;
                                                unsigned char flag=0;

                                                if ( msgBuffer[4]== 0x30 )
                                                {
                                                        unsigned char signatureIndex        =        msgBuffer[6];

                                                        if ( signatureIndex == 0 )
                                                                answerByte        =        (SIGNATURE_BYTES >>16) & 0x000000FF;
                                                        else if ( signatureIndex == 1 )
                                                                answerByte        =        (SIGNATURE_BYTES >> 8) & 0x000000FF;
                                                        else
                                                                answerByte        =        SIGNATURE_BYTES & 0x000000FF;
                                                }
                                                else if ( msgBuffer[4] & 0x50 )
                                                {
                                                        answerByte        =        0; //read fuse/lock bits not implemented, return dummy value
                                                }
                                                else
                                                {
                                                        answerByte        =        0; // for all others command are not implemented, return dummy value for AVRDUDE happy <Worapoht>
        //                                                flag        =        1; // Remark this line for AVRDUDE <Worapoht>
                                                }
                                                if ( !flag )
                                                {
                                                        msgLength                =        7;
                                                        msgBuffer[1]        =        STATUS_CMD_OK;
                                                        msgBuffer[2]        =        0;
                                                        msgBuffer[3]        =        msgBuffer[4];
                                                        msgBuffer[4]        =        0;
                                                        msgBuffer[5]        =        answerByte;
                                                        msgBuffer[6]        =        STATUS_CMD_OK;
                                                }
                                        }
                                        break;
        #endif
                                case CMD_SIGN_ON:
                                        msgLength                =        11;
                                        msgBuffer[1]         =        STATUS_CMD_OK;
                                        msgBuffer[2]         =        8;
                                        msgBuffer[3]         =        'A';
                                        msgBuffer[4]         =        'V';
                                        msgBuffer[5]         =        'R';
                                        msgBuffer[6]         =        'I';
                                        msgBuffer[7]         =        'S';
                                        msgBuffer[8]         =        'P';
                                        msgBuffer[9]         =        '_';
                                        msgBuffer[10]        =        '2';
                                        break;

                                case CMD_GET_PARAMETER:
                                        {
                                                unsigned char value;

                                                switch(msgBuffer[1])
                                                {
                                                case PARAM_BUILD_NUMBER_LOW:
                                                        value        =        CONFIG_PARAM_BUILD_NUMBER_LOW;
                                                        break;
                                                case PARAM_BUILD_NUMBER_HIGH:
                                                        value        =        CONFIG_PARAM_BUILD_NUMBER_HIGH;
                                                        break;
                                                case PARAM_HW_VER:
                                                        value        =        CONFIG_PARAM_HW_VER;
                                                        break;
                                                case PARAM_SW_MAJOR:
                                                        value        =        CONFIG_PARAM_SW_MAJOR;
                                                        break;
                                                case PARAM_SW_MINOR:
                                                        value        =        CONFIG_PARAM_SW_MINOR;
                                                        break;
                                                default:
                                                        value        =        0;
                                                        break;
                                                }
                                                msgLength                =        3;
                                                msgBuffer[1]        =        STATUS_CMD_OK;
                                                msgBuffer[2]        =        value;
                                        }
                                        break;

                                case CMD_LEAVE_PROGMODE_ISP:
                                        isLeave        =        1;
                                        //*        fall through

                                case CMD_SET_PARAMETER:
                                case CMD_ENTER_PROGMODE_ISP:
                                        msgLength                =        2;
                                        msgBuffer[1]        =        STATUS_CMD_OK;
                                        break;

                                case CMD_READ_SIGNATURE_ISP:
                                        {
                                                unsigned char signatureIndex        =        msgBuffer[4];
                                                unsigned char signature;

                                                if ( signatureIndex == 0 )
                                                        signature        =        (SIGNATURE_BYTES >>16) & 0x000000FF;
                                                else if ( signatureIndex == 1 )
                                                        signature        =        (SIGNATURE_BYTES >> 8) & 0x000000FF;
                                                else
                                                        signature        =        SIGNATURE_BYTES & 0x000000FF;

                                                msgLength                =        4;
                                                msgBuffer[1]        =        STATUS_CMD_OK;
                                                msgBuffer[2]        =        signature;
                                                msgBuffer[3]        =        STATUS_CMD_OK;
                                        }
                                        break;

                                case CMD_READ_LOCK_ISP:
                                        msgLength                =        4;
                                        msgBuffer[1]        =        STATUS_CMD_OK;
                                        msgBuffer[2]        =        boot_lock_fuse_bits_get( GET_LOCK_BITS );
                                        msgBuffer[3]        =        STATUS_CMD_OK;
                                        break;

                                case CMD_READ_FUSE_ISP:
                                        {
                                                unsigned char fuseBits;

                                                if ( msgBuffer[2] == 0x50 )
                                                {
                                                        if ( msgBuffer[3] == 0x08 )
                                                                fuseBits        =        boot_lock_fuse_bits_get( GET_EXTENDED_FUSE_BITS );
                                                        else
                                                                fuseBits        =        boot_lock_fuse_bits_get( GET_LOW_FUSE_BITS );
                                                }
                                                else
                                                {
                                                        fuseBits        =        boot_lock_fuse_bits_get( GET_HIGH_FUSE_BITS );
                                                }
                                                msgLength                =        4;
                                                msgBuffer[1]        =        STATUS_CMD_OK;
                                                msgBuffer[2]        =        fuseBits;
                                                msgBuffer[3]        =        STATUS_CMD_OK;
                                        }
                                        break;

        #ifndef REMOVE_PROGRAM_LOCK_BIT_SUPPORT
                                case CMD_PROGRAM_LOCK_ISP:
                                        {
                                                unsigned char lockBits        =        msgBuffer[4];

                                                lockBits        =        (~lockBits) & 0x3C;        // mask BLBxx bits
                                                boot_lock_bits_set(lockBits);                // and program it
                                                boot_spm_busy_wait();

                                                msgLength                =        3;
                                                msgBuffer[1]        =        STATUS_CMD_OK;
                                                msgBuffer[2]        =        STATUS_CMD_OK;
                                        }
                                        break;
        #endif
                                case CMD_CHIP_ERASE_ISP:
                                        eraseAddress        =        0;
                                        msgLength                =        2;
                                        msgBuffer[1]        =        STATUS_CMD_OK;
                                        break;

                                case CMD_LOAD_ADDRESS:
        #if defined(RAMPZ)
                                        address        =        ( ((address_t)(msgBuffer[1])<<24)|((address_t)(msgBuffer[2])<<16)|((address_t)(msgBuffer[3])<<8)|(msgBuffer[4]) )<<1;
        #else
                                        address        =        ( ((msgBuffer[3])<<8)|(msgBuffer[4]) )<<1;                //convert word to byte address
        #endif
                                        msgLength                =        2;
                                        msgBuffer[1]        =        STATUS_CMD_OK;
                                        break;

                                case CMD_PROGRAM_FLASH_ISP:
                                case CMD_PROGRAM_EEPROM_ISP:
                                        {
                                                unsigned int        size        =        ((msgBuffer[1])<<8) | msgBuffer[2];
                                                unsigned char        *p        =        msgBuffer+10;
                                                unsigned int        data;
                                                unsigned char        highByte, lowByte;
                                                address_t                tempaddress        =        address;


                                                if ( msgBuffer[0] == CMD_PROGRAM_FLASH_ISP )
                                                {
                                                        // erase only main section (bootloader protection)
                                                        if (eraseAddress < APP_END )
                                                        {
                                                                boot_page_erase(eraseAddress);        // Perform page erase
                                                                boot_spm_busy_wait();                // Wait until the memory is erased.
                                                                eraseAddress += SPM_PAGESIZE;        // point to next page to be erase
                                                        }

                                                        /* Write FLASH */
                                                        do {
                                                                lowByte                =        *p++;
                                                                highByte         =        *p++;

                                                                data                =        (highByte << 8) | lowByte;
                                                                boot_page_fill(address,data);

                                                                address        =        address + 2;        // Select next word in memory
                                                                size        -=        2;                                // Reduce number of bytes to write by two
                                                        } while (size);                                        // Loop until all bytes written

                                                        boot_page_write(tempaddress);
                                                        boot_spm_busy_wait();
                                                        boot_rww_enable();                                // Re-enable the RWW section
                                                }
                                                else
                                                {
                                                #if (!defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)  && !defined(__AVR_ATmega2561__)  && !defined(__AVR_ATmega1284P__)  && !defined(__AVR_ATmega640__))
                                        //        #if (defined(EEARL) && defined(EEARH)  && defined(EEMWE)  && defined(EEWE)  && defined(EEDR))
                                                        /* write EEPROM */
                                                        do {
                                                                EEARL        =        address;                        // Setup EEPROM address
                                                                EEARH        =        (address >> 8);
                                                                address++;                                                // Select next EEPROM byte

                                                                EEDR        =        *p++;                                // get byte from buffer
                                                                EECR        |=        (1<<EEMWE);                        // Write data into EEPROM
                                                                EECR        |=        (1<<EEWE);

                                                                while (EECR & (1<<EEWE));        // Wait for write operation to finish
                                                                size--;                                                // Decrease number of bytes to write
                                                        } while (size);                                        // Loop until all bytes written
                                                #endif
                                                }
                                                        msgLength        =        2;
                                                msgBuffer[1]        =        STATUS_CMD_OK;
                                        }
                                        break;

                                case CMD_READ_FLASH_ISP:
                                case CMD_READ_EEPROM_ISP:
                                        {
                                                unsigned int        size        =        ((msgBuffer[1])<<8) | msgBuffer[2];
                                                unsigned char        *p                =        msgBuffer+1;
                                                msgLength                                =        size+3;

                                                *p++        =        STATUS_CMD_OK;
                                                if (msgBuffer[0] == CMD_READ_FLASH_ISP )
                                                {
                                                        unsigned int data;

                                                        // Read FLASH
                                                        do {
                                                //#if defined(RAMPZ)
                                                #if (FLASHEND > 0x10000)
                                                                data        =        pgm_read_word_far(address);
                                                #else
                                                                data        =        pgm_read_word_near(address);
                                                #endif
                                                                *p++        =        (unsigned char)data;                //LSB
                                                                *p++        =        (unsigned char)(data >> 8);        //MSB
                                                                address        +=        2;                                                        // Select next word in memory
                                                                size        -=        2;
                                                        }while (size);
                                                }
                                                else
                                                {
                                                        /* Read EEPROM */
                                                        do {
                                                                EEARL        =        address;                        // Setup EEPROM address
                                                                EEARH        =        ((address >> 8));
                                                                address++;                                        // Select next EEPROM byte
                                                                EECR        |=        (1<<EERE);                        // Read EEPROM
                                                                *p++        =        EEDR;                                // Send EEPROM data
                                                                size--;
                                                        } while (size);
                                                }
                                                *p++        =        STATUS_CMD_OK;
                                        }
                                        break;

                                default:
                                        msgLength                =        2;
                                        msgBuffer[1]        =        STATUS_CMD_FAILED;
                                        break;
                        }

                        /*
                         * Now send answer message back
                         */
                        sendchar(MESSAGE_START);
                        checksum        =        MESSAGE_START^0;

                        sendchar(seqNum);
                        checksum        ^=        seqNum;

                        c                        =        ((msgLength>>8)&0xFF);
                        sendchar(c);
                        checksum        ^=        c;

                        c                        =        msgLength&0x00FF;
                        sendchar(c);
                        checksum ^= c;

                        sendchar(TOKEN);
                        checksum ^= TOKEN;

                        p        =        msgBuffer;
                        while ( msgLength )
                        {
                                c        =        *p++;
                                sendchar(c);
                                checksum ^=c;
                                msgLength--;
                        }
                        sendchar(checksum);
                        seqNum++;

                #ifndef REMOVE_BOOTLOADER_LED
                        //*        <MLS>        toggle the LED
                        PROGLED_PORT        ^=        (1<<PROGLED_PIN);        // active high LED ON
                #endif

                }
        }

#ifdef _DEBUG_SERIAL_
        sendchar('j');
//        sendchar('u');
//        sendchar('m');
//        sendchar('p');
//        sendchar(' ');
//        sendchar('u');
//        sendchar('s');
//        sendchar('r');
        sendchar(0x0d);
        sendchar(0x0a);

        delay_ms(100);
#endif


#ifndef REMOVE_BOOTLOADER_LED
        PROGLED_DDR                &=        ~(1<<PROGLED_PIN);        // set to default
        PROGLED_PORT        &=        ~(1<<PROGLED_PIN);        // active low LED OFF
//        PROGLED_PORT        |=        (1<<PROGLED_PIN);        // active high LED OFf
        delay_ms(100);                                                        // delay after exit
#endif

        asm volatile ("nop");                        // wait until port has changed

        /*
         * Now leave bootloader
         */

        UART_STATUS_REG        &=        0xfd;
        boot_rww_enable();                                // enable application section

        asm volatile(
                        "clr        r30                \n\t"
                        "clr        r31                \n\t"
                        "ijmp        \n\t"
                        );
//        asm volatile ( "push r1" "\n\t"                // Jump to Reset vector in Application Section
//                                        "push r1" "\n\t"
//                                        "ret"         "\n\t"
//                                        ::);

         /*
         * Never return to stop GCC to generate exit return code
         * Actually we will never reach this point, but the compiler doesn't
         * understand this
         */
        for(;;);
}

/*
base address = f800

avrdude: Device signature = 0x1e9703
avrdude: safemode: lfuse reads as FF
avrdude: safemode: hfuse reads as DA
avrdude: safemode: efuse reads as F5
avrdude>


base address = f000
avrdude: Device signature = 0x1e9703
avrdude: safemode: lfuse reads as FF
avrdude: safemode: hfuse reads as D8
avrdude: safemode: efuse reads as F5
avrdude>
*/

//************************************************************************
#ifdef ENABLE_MONITOR
#include        <math.h>

unsigned long        gRamIndex;
unsigned long        gFlashIndex;
unsigned long        gEepromIndex;


#define        true        1
#define        false        0

#if defined(__AVR_ATmega128__)
        #define        kCPU_NAME        "ATmega128"
#elif defined(__AVR_ATmega1280__)
        #define        kCPU_NAME        "ATmega1280"
#elif defined(__AVR_ATmega1281__)
        #define        kCPU_NAME        "ATmega1281"
#elif defined(__AVR_ATmega2560__)
        #define        kCPU_NAME        "ATmega2560"
#elif defined(__AVR_ATmega2561__)
        #define        kCPU_NAME        "ATmega2561"
#elif defined(__AVR_ATmega1284P__)
        #define        kCPU_NAME        "ATmega1284P"
#elif defined(__AVR_ATmega640__)
        #define        kCPU_NAME        "ATmega640"
#else
        #error cpu name not defined
#endif

#ifdef _VECTORS_SIZE
        #define        kInterruptVectorCount (_VECTORS_SIZE / 4)
#else
        #define        kInterruptVectorCount 23
#endif


void        PrintDecInt(int theNumber, int digitCnt);

#ifdef kCPU_NAME
        prog_char        gTextMsg_CPU_Name[]                        PROGMEM        =        kCPU_NAME;
#else
        prog_char        gTextMsg_CPU_Name[]                        PROGMEM        =        "UNKNOWN";
#endif

        prog_char        gTextMsg_Explorer[]                        PROGMEM        =        "Arduino explorer stk500V2 by MLS";
        prog_char        gTextMsg_Prompt[]                        PROGMEM        =        "Bootloader>";
        prog_char        gTextMsg_HUH[]                                PROGMEM        =        "Huh?";
        prog_char        gTextMsg_COMPILED_ON[]                PROGMEM        =        "Compiled on  = ";
        prog_char        gTextMsg_CPU_Type[]                        PROGMEM        =        "CPU Type     = ";
        prog_char        gTextMsg_AVR_ARCH[]                        PROGMEM        =        "__AVR_ARCH__ = ";
        prog_char        gTextMsg_AVR_LIBC[]                        PROGMEM        =        "AVR LibC Ver = ";
        prog_char        gTextMsg_GCC_VERSION[]                PROGMEM        =        "GCC Version  = ";
        prog_char        gTextMsg_CPU_SIGNATURE[]        PROGMEM        =        "CPU signature= ";
        prog_char        gTextMsg_FUSE_BYTE_LOW[]        PROGMEM        =        "Low fuse     = ";
        prog_char        gTextMsg_FUSE_BYTE_HIGH[]        PROGMEM        =        "High fuse    = ";
        prog_char        gTextMsg_FUSE_BYTE_EXT[]        PROGMEM        =        "Ext fuse     = ";
        prog_char        gTextMsg_FUSE_BYTE_LOCK[]        PROGMEM        =        "Lock fuse    = ";
        prog_char        gTextMsg_GCC_DATE_STR[]                PROGMEM        =        __DATE__;
        prog_char        gTextMsg_AVR_LIBC_VER_STR[]        PROGMEM        =        __AVR_LIBC_VERSION_STRING__;
        prog_char        gTextMsg_GCC_VERSION_STR[]        PROGMEM        =        __VERSION__;
        prog_char        gTextMsg_VECTOR_HEADER[]        PROGMEM        =        "V#   ADDR   op code     instruction addr   Interrupt";
        prog_char        gTextMsg_noVector[]                        PROGMEM        =        "no vector";
        prog_char        gTextMsg_rjmp[]                                PROGMEM        =        "rjmp  ";
        prog_char        gTextMsg_jmp[]                                PROGMEM        =        "jmp ";
        prog_char        gTextMsg_WHAT_PORT[]                PROGMEM        =        "What port:";
        prog_char        gTextMsg_PortNotSupported[]        PROGMEM        =        "Port not supported";
        prog_char        gTextMsg_MustBeLetter[]                PROGMEM        =        "Must be a letter";
        prog_char        gTextMsg_SPACE[]                        PROGMEM        =        " ";
        prog_char        gTextMsg_WriteToEEprom[]        PROGMEM        =        "Writting EE";
        prog_char        gTextMsg_ReadingEEprom[]        PROGMEM        =        "Reading EE";
        prog_char        gTextMsg_EEPROMerrorCnt[]        PROGMEM        =        "eeprom error count=";
        prog_char        gTextMsg_PORT[]                                PROGMEM        =        "PORT";


//************************************************************************
//*        Help messages
        prog_char        gTextMsg_HELP_MSG_0[]                PROGMEM        =        "0=Zero address ctrs";
        prog_char        gTextMsg_HELP_MSG_QM[]                PROGMEM        =        "?=CPU stats";
        prog_char        gTextMsg_HELP_MSG_AT[]                PROGMEM        =        "@=EEPROM test";
        prog_char        gTextMsg_HELP_MSG_B[]                PROGMEM        =        "B=Blink LED";
        prog_char        gTextMsg_HELP_MSG_E[]                PROGMEM        =        "E=Dump EEPROM";
        prog_char        gTextMsg_HELP_MSG_F[]                PROGMEM        =        "F=Dump FLASH";
        prog_char        gTextMsg_HELP_MSG_H[]                PROGMEM        =        "H=Help";
        prog_char        gTextMsg_HELP_MSG_L[]                PROGMEM        =        "L=List I/O Ports";
        prog_char        gTextMsg_HELP_MSG_Q[]                PROGMEM        =        "Q=Quit & jump to user pgm";
        prog_char        gTextMsg_HELP_MSG_R[]                PROGMEM        =        "R=Dump RAM";
        prog_char        gTextMsg_HELP_MSG_V[]                PROGMEM        =        "V=show interrupt Vectors";
        prog_char        gTextMsg_HELP_MSG_Y[]                PROGMEM        =        "Y=Port blink";

        prog_char        gTextMsg_END[]                                PROGMEM        =        "*";


//************************************************************************
void        PrintFromPROGMEM(void *dataPtr, unsigned char offset)
{
uint8_t        ii;
char        theChar;

        ii                        =        offset;
        theChar                =        1;

        while (theChar != 0)
        {
        #if (FLASHEND > 0x10000)
                theChar        =        pgm_read_byte_far((uint32_t)dataPtr + ii);
        #else
                theChar        =        pgm_read_byte_near((uint32_t)dataPtr + ii);
        #endif
                if (theChar != 0)
                {
                        sendchar(theChar);
                }
                ii++;
        }
}

//************************************************************************
void        PrintNewLine(void)
{
        sendchar(0x0d);
        sendchar(0x0a);
}


//************************************************************************
void        PrintFromPROGMEMln(void *dataPtr, unsigned char offset)
{
        PrintFromPROGMEM(dataPtr, offset);

        PrintNewLine();
}


//************************************************************************
void        PrintString(char *textString)
{
char        theChar;
int                ii;

        theChar                =        1;
        ii                        =        0;
        while (theChar != 0)
        {
                theChar        =        textString[ii];
                if (theChar != 0)
                {
                        sendchar(theChar);
                }
                ii++;
        }
}

//************************************************************************
void        PrintHexByte(unsigned char theByte)
{
char        theChar;

        theChar        =        0x30 + ((theByte >> 4) & 0x0f);
        if (theChar > 0x39)
        {
                theChar        +=        7;
        }
        sendchar(theChar );

        theChar        =        0x30 + (theByte & 0x0f);
        if (theChar > 0x39)
        {
                theChar        +=        7;
        }
        sendchar(theChar );
}

//************************************************************************
void        PrintDecInt(int theNumber, int digitCnt)
{
int        theChar;
int        myNumber;

        myNumber        =        theNumber;

        if ((myNumber > 100) || (digitCnt >= 3))
        {
                theChar                =        0x30 + myNumber / 100;
                sendchar(theChar );
        }

        if ((myNumber > 10) || (digitCnt >= 2))
        {
                theChar        =        0x30  + ((myNumber % 100) / 10 );
                sendchar(theChar );
        }
        theChar        =        0x30 + (myNumber % 10);
        sendchar(theChar );
}




//************************************************************************
static void        PrintCPUstats(void)
{
unsigned char fuseByte;

        PrintFromPROGMEMln(gTextMsg_Explorer, 0);

        PrintFromPROGMEM(gTextMsg_COMPILED_ON, 0);
        PrintFromPROGMEMln(gTextMsg_GCC_DATE_STR, 0);

        PrintFromPROGMEM(gTextMsg_CPU_Type, 0);
        PrintFromPROGMEMln(gTextMsg_CPU_Name, 0);

        PrintFromPROGMEM(gTextMsg_AVR_ARCH, 0);
        PrintDecInt(__AVR_ARCH__, 1);
        PrintNewLine();

        PrintFromPROGMEM(gTextMsg_GCC_VERSION, 0);
        PrintFromPROGMEMln(gTextMsg_GCC_VERSION_STR, 0);

        //*        these can be found in avr/version.h
        PrintFromPROGMEM(gTextMsg_AVR_LIBC, 0);
        PrintFromPROGMEMln(gTextMsg_AVR_LIBC_VER_STR, 0);

#if defined(SIGNATURE_0)
        PrintFromPROGMEM(gTextMsg_CPU_SIGNATURE, 0);
        //*        these can be found in avr/iomxxx.h
        PrintHexByte(SIGNATURE_0);
        PrintHexByte(SIGNATURE_1);
        PrintHexByte(SIGNATURE_2);
        PrintNewLine();
#endif


#if defined(GET_LOW_FUSE_BITS)
        //*        fuse settings
        PrintFromPROGMEM(gTextMsg_FUSE_BYTE_LOW, 0);
        fuseByte        =        boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
        PrintHexByte(fuseByte);
        PrintNewLine();

        PrintFromPROGMEM(gTextMsg_FUSE_BYTE_HIGH, 0);
        fuseByte        =        boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
        PrintHexByte(fuseByte);
        PrintNewLine();

        PrintFromPROGMEM(gTextMsg_FUSE_BYTE_EXT, 0);
        fuseByte        =        boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
        PrintHexByte(fuseByte);
        PrintNewLine();

        PrintFromPROGMEM(gTextMsg_FUSE_BYTE_LOCK, 0);
        fuseByte        =        boot_lock_fuse_bits_get(GET_LOCK_BITS);
        PrintHexByte(fuseByte);
        PrintNewLine();

#endif

}

#ifndef sbi
        #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//************************************************************************
int analogRead(uint8_t pin)
{
uint8_t low, high;

        // set the analog reference (high two bits of ADMUX) and select the
        // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
        // to 0 (the default).
//        ADMUX        =        (analog_reference << 6) | (pin & 0x07);
        ADMUX        =        (1 << 6) | (pin & 0x07);

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        // the MUX5 bit of ADCSRB selects whether we're reading from channels
        // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
        ADCSRB        =        (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

        // without a delay, we seem to read from the wrong channel
        //delay(1);

        // start the conversion
        sbi(ADCSRA, ADSC);

        // ADSC is cleared when the conversion finishes
        while (bit_is_set(ADCSRA, ADSC));

        // we have to read ADCL first; doing so locks both ADCL
        // and ADCH until ADCH is read.  reading ADCL second would
        // cause the results of each conversion to be discarded,
        // as ADCL and ADCH would be locked when it completed.
        low                =        ADCL;
        high        =        ADCH;

        // combine the two bytes
        return (high << 8) | low;
}

//************************************************************************
static void BlinkLED(void)
{
        PROGLED_DDR                |=        (1<<PROGLED_PIN);
        PROGLED_PORT        |=        (1<<PROGLED_PIN);        // active high LED ON

        while (!Serial_Available())
        {
                PROGLED_PORT        &=        ~(1<<PROGLED_PIN);        // turn LED off
                delay_ms(100);
                PROGLED_PORT        |=        (1<<PROGLED_PIN);        // turn LED on
                delay_ms(100);
        }
        recchar();        //        get the char out of the buffer
}

enum
{
        kDUMP_FLASH        =        0,
        kDUMP_EEPROM,
        kDUMP_RAM
};

//************************************************************************
static void        DumpHex(unsigned char dumpWhat, unsigned long startAddress, unsigned char numRows)
{
unsigned long        myAddressPointer;
uint8_t                        ii;
unsigned char        theValue;
char                        asciiDump[18];
unsigned char        *ramPtr;


        ramPtr                                =        0;
        theValue                        =        0;
        myAddressPointer        =        startAddress;
        while (numRows > 0)
        {
                if (myAddressPointer > 0x10000)
                {
                        PrintHexByte((myAddressPointer >> 16) & 0x00ff);
                }
                PrintHexByte((myAddressPointer >> 8) & 0x00ff);
                PrintHexByte(myAddressPointer & 0x00ff);
                sendchar(0x20);
                sendchar('-');
                sendchar(0x20);

                asciiDump[0]                =        0;
                for (ii=0; ii<16; ii++)
                {
                        switch(dumpWhat)
                        {
                                case kDUMP_FLASH:
                                #if (FLASHEND > 0x10000)
                                        theValue        =        pgm_read_byte_far(myAddressPointer);
                                #else
                                        theValue        =        pgm_read_byte_near(myAddressPointer);
                                #endif
                                        break;

                                case kDUMP_EEPROM:
                                        theValue        =        eeprom_read_byte((void *)myAddressPointer);
                                        break;

                                case kDUMP_RAM:
                                        theValue        =        ramPtr[myAddressPointer];
                                        break;

                        }
                        PrintHexByte(theValue);
                        sendchar(0x20);
                        if ((theValue >= 0x20) && (theValue < 0x7f))
                        {
                                asciiDump[ii % 16]        =        theValue;
                        }
                        else
                        {
                                asciiDump[ii % 16]        =        '.';
                        }

                        myAddressPointer++;
                }
                asciiDump[16]        =        0;
                PrintString(asciiDump);
                PrintNewLine();

                numRows--;
        }
}



//************************************************************************
//*        returns amount of extended memory
static void        EEPROMtest(void)
{
int                ii;
char        theChar;
char        theEEPROMchar;
int                errorCount;

        PrintFromPROGMEMln(gTextMsg_WriteToEEprom, 0);
        PrintNewLine();
        ii                        =        0;
#if (FLASHEND > 0x10000)
        while (((theChar = pgm_read_byte_far(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
#else
        while (((theChar = pgm_read_byte_near(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
#endif
        {
                eeprom_write_byte((uint8_t *)ii, theChar);
                if (theChar == 0)
                {
                        PrintFromPROGMEM(gTextMsg_SPACE, 0);
                }
                else
                {
                        sendchar(theChar);
                }
                ii++;
        }

        //*        no go back through and test
        PrintNewLine();
        PrintNewLine();
        PrintFromPROGMEMln(gTextMsg_ReadingEEprom, 0);
        PrintNewLine();
        errorCount        =        0;
        ii                        =        0;
#if (FLASHEND > 0x10000)
        while (((theChar = pgm_read_byte_far(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
#else
        while (((theChar = pgm_read_byte_near(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
#endif
        {
                theEEPROMchar        =        eeprom_read_byte((uint8_t *)ii);
                if (theEEPROMchar == 0)
                {
                        PrintFromPROGMEM(gTextMsg_SPACE, 0);
                }
                else
                {
                        sendchar(theEEPROMchar);
                }
                if (theEEPROMchar != theChar)
                {
                        errorCount++;
                }
                ii++;
        }
        PrintNewLine();
        PrintNewLine();
        PrintFromPROGMEM(gTextMsg_EEPROMerrorCnt, 0);
        PrintDecInt(errorCount, 1);
        PrintNewLine();
        PrintNewLine();

        gEepromIndex        =        0;        //*        set index back to zero for next eeprom dump

}



#if (FLASHEND > 0x08000)
        #include        "avrinterruptnames.h"
        #ifndef _INTERRUPT_NAMES_DEFINED_
                #warning Interrupt vectors not defined
        #endif
#endif

//************************************************************************
static void        VectorDisplay(void)
{
unsigned long        byte1;
unsigned long        byte2;
unsigned long        byte3;
unsigned long        byte4;
unsigned long        word1;
unsigned long        word2;
int                                vectorIndex;
unsigned long        myMemoryPtr;
unsigned long        wordMemoryAddress;
unsigned long        realitiveAddr;
unsigned long        myFullAddress;
unsigned long        absoluteAddr;
#if defined(_INTERRUPT_NAMES_DEFINED_)
        long                stringPointer;
#endif

        myMemoryPtr                =        0;
        vectorIndex                =        0;
        PrintFromPROGMEMln(gTextMsg_CPU_Name, 0);
        PrintFromPROGMEMln(gTextMsg_VECTOR_HEADER, 0);
        //                                         V#   ADDR   op code
        //                                          1 - 0000 = C3 BB 00 00 rjmp 03BB >000776 RESET
        while (vectorIndex < kInterruptVectorCount)
        {
                wordMemoryAddress        =        myMemoryPtr / 2;
                //                                         01 - 0000 = 12 34
                PrintDecInt(vectorIndex + 1, 2);
                sendchar(0x20);
                sendchar('-');
                sendchar(0x20);
                PrintHexByte((wordMemoryAddress >> 8) & 0x00ff);
                PrintHexByte((wordMemoryAddress) & 0x00ff);
                sendchar(0x20);
                sendchar('=');
                sendchar(0x20);


                //*        the AVR is LITTLE ENDIAN, swap the byte order
        #if (FLASHEND > 0x10000)
                byte1        =        pgm_read_byte_far(myMemoryPtr++);
                byte2        =        pgm_read_byte_far(myMemoryPtr++);
                byte3        =        pgm_read_byte_far(myMemoryPtr++);
                byte4        =        pgm_read_byte_far(myMemoryPtr++);
        #else
                byte1        =        pgm_read_byte_near(myMemoryPtr++);
                byte2        =        pgm_read_byte_near(myMemoryPtr++);
                byte3        =        pgm_read_byte_near(myMemoryPtr++);
                byte4        =        pgm_read_byte_near(myMemoryPtr++);
        #endif
                word1        =        (byte2 << 8) + byte1;
                word2        =        (byte4 << 8) + byte3;


                PrintHexByte(byte2);
                sendchar(0x20);
                PrintHexByte(byte1);
                sendchar(0x20);
                PrintHexByte(byte4);
                sendchar(0x20);
                PrintHexByte(byte3);
                sendchar(0x20);

                if (word1 == 0xffff)
                {
                        PrintFromPROGMEM(gTextMsg_noVector, 0);
                }
                else if ((word1 & 0xc000) == 0xc000)
                {
                        //*        rjmp instruction
                        realitiveAddr        =        word1 & 0x3FFF;
                        absoluteAddr        =        wordMemoryAddress + realitiveAddr;        //*        add the offset to the current address
                        absoluteAddr        =        absoluteAddr << 1;                                        //*        multiply by 2 for byte address

                        PrintFromPROGMEM(gTextMsg_rjmp, 0);
                        PrintHexByte((realitiveAddr >> 8) & 0x00ff);
                        PrintHexByte((realitiveAddr) & 0x00ff);
                        sendchar(0x20);
                        sendchar('>');
                        PrintHexByte((absoluteAddr >> 16) & 0x00ff);
                        PrintHexByte((absoluteAddr >> 8) & 0x00ff);
                        PrintHexByte((absoluteAddr) & 0x00ff);

                }
                else if ((word1 & 0xfE0E) == 0x940c)
                {
                        //*        jmp instruction, this is REALLY complicated, refer to the instruction manual (JMP)
                        myFullAddress        =        ((byte1 & 0x01) << 16) +
                                                                ((byte1 & 0xf0) << 17) +
                                                                ((byte2 & 0x01) << 21) +
                                                                word2;

                        absoluteAddr        =        myFullAddress << 1;

                        PrintFromPROGMEM(gTextMsg_jmp, 0);
                        PrintHexByte((myFullAddress >> 16) & 0x00ff);
                        PrintHexByte((myFullAddress >> 8) & 0x00ff);
                        PrintHexByte((myFullAddress) & 0x00ff);
                        sendchar(0x20);
                        sendchar('>');
                        PrintHexByte((absoluteAddr >> 16) & 0x00ff);
                        PrintHexByte((absoluteAddr >> 8) & 0x00ff);
                        PrintHexByte((absoluteAddr) & 0x00ff);
                }

        #if defined(_INTERRUPT_NAMES_DEFINED_)
                sendchar(0x20);
        #if (FLASHEND > 0x10000)
                stringPointer        =        pgm_read_word_far(&(gInterruptNameTable[vectorIndex]));
        #else
                stringPointer        =        pgm_read_word_near(&(gInterruptNameTable[vectorIndex]));
        #endif
                PrintFromPROGMEM((char *)stringPointer, 0);
        #endif
                PrintNewLine();

                vectorIndex++;
        }
}

//************************************************************************
static void        PrintAvailablePort(char thePortLetter)
{
        PrintFromPROGMEM(gTextMsg_PORT, 0);
        sendchar(thePortLetter);
        PrintNewLine();
}

//************************************************************************
static void        ListAvailablePorts(void)
{

#ifdef DDRA
        PrintAvailablePort('A');
#endif

#ifdef DDRB
        PrintAvailablePort('B');
#endif

#ifdef DDRC
        PrintAvailablePort('C');
#endif

#ifdef DDRD
        PrintAvailablePort('D');
#endif

#ifdef DDRE
        PrintAvailablePort('E');
#endif

#ifdef DDRF
        PrintAvailablePort('F');
#endif

#ifdef DDRG
        PrintAvailablePort('G');
#endif

#ifdef DDRH
        PrintAvailablePort('H');
#endif

#ifdef DDRI
        PrintAvailablePort('I');
#endif

#ifdef DDRJ
        PrintAvailablePort('J');
#endif

#ifdef DDRK
        PrintAvailablePort('K');
#endif

#ifdef DDRL
        PrintAvailablePort('L');
#endif

}

//************************************************************************
static void        AVR_PortOutput(void)
{
char        portLetter;
char        getCharFlag;

        PrintFromPROGMEM(gTextMsg_WHAT_PORT, 0);

        portLetter        =        recchar();
        portLetter        =        portLetter & 0x5f;
        sendchar(portLetter);
        PrintNewLine();

        if ((portLetter >= 'A') && (portLetter <= 'Z'))
        {
                getCharFlag        =        true;
                switch(portLetter)
                {
                #ifdef DDRA
                        case 'A':
                                DDRA        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTA        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTA        =        0;
                                break;
                #endif

                #ifdef DDRB
                        case 'B':
                                DDRB        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTB        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTB        =        0;
                                break;
                #endif

                #ifdef DDRC
                        case 'C':
                                DDRC        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTC        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTC        =        0;
                                break;
                #endif

                #ifdef DDRD
                        case 'D':
                                DDRD        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTD        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTD        =        0;
                                break;
                #endif

                #ifdef DDRE
                        case 'E':
                                DDRE        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTE        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTE        =        0;
                                break;
                #endif

                #ifdef DDRF
                        case 'F':
                                DDRF        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTF        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTF        =        0;
                                break;
                #endif

                #ifdef DDRG
                        case 'G':
                                DDRG        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTG        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTG        =        0;
                                break;
                #endif

                #ifdef DDRH
                        case 'H':
                                DDRH        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTH        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTH        =        0;
                                break;
                #endif

                #ifdef DDRI
                        case 'I':
                                DDRI        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTI        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTI        =        0;
                                break;
                #endif

                #ifdef DDRJ
                        case 'J':
                                DDRJ        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTJ        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTJ        =        0;
                                break;
                #endif

                #ifdef DDRK
                        case 'K':
                                DDRK        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTK        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTK        =        0;
                                break;
                #endif

                #ifdef DDRL
                        case 'L':
                                DDRL        =        0xff;
                                while (!Serial_Available())
                                {
                                        PORTL        ^=        0xff;
                                        delay_ms(200);
                                }
                                PORTL        =        0;
                                break;
                #endif

                        default:
                                PrintFromPROGMEMln(gTextMsg_PortNotSupported, 0);
                                getCharFlag        =        false;
                                break;
                }
                if (getCharFlag)
                {
                        recchar();
                }
        }
        else
        {
                PrintFromPROGMEMln(gTextMsg_MustBeLetter, 0);
        }
}


//*******************************************************************
static void PrintHelp(void)
{
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_0, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_QM, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_AT, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_B, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_E, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_F, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_H, 0);

        PrintFromPROGMEMln(gTextMsg_HELP_MSG_L, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_Q, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_R, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_V, 0);
        PrintFromPROGMEMln(gTextMsg_HELP_MSG_Y, 0);
}

//************************************************************************
static void        RunMonitor(void)
{
char                        keepGoing;
unsigned char        theChar;
int                                ii, jj;

        for (ii=0; ii<5; ii++)
        {
                for (jj=0; jj<25; jj++)
                {
                        sendchar('!');
                }
                PrintNewLine();
        }

        gRamIndex                        =        0;
        gFlashIndex                        =        0;
        gEepromIndex                =        0;

        PrintFromPROGMEMln(gTextMsg_Explorer, 0);

        keepGoing        =        1;
        while (keepGoing)
        {
                PrintFromPROGMEM(gTextMsg_Prompt, 0);
                theChar        =        recchar();
                if (theChar >= 0x60)
                {
                        theChar        =        theChar & 0x5F;
                }
        #if defined( _CEREBOTPLUS_BOARD_ )
                if (theChar == 0x5F)
                {

                }
                else
        #endif
                if (theChar >= 0x20)
                {
                        sendchar(theChar);
                        sendchar(0x20);
                }

                switch(theChar)
                {
                        case '0':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_0, 2);
                                gFlashIndex                =        0;
                                gRamIndex                =        0;
                                gEepromIndex        =        0;
                                break;

                        case '?':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_QM, 2);
                                PrintCPUstats();
                                break;

                        case '@':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_AT, 2);
                                EEPROMtest();
                                break;

                        case 'B':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_B, 2);
                                BlinkLED();
                                break;

                        case 'E':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_E, 2);
                                DumpHex(kDUMP_EEPROM, gEepromIndex, 16);
                                gEepromIndex        +=        256;
                                if (gEepromIndex > E2END)
                                {
                                        gEepromIndex        =        0;
                                }
                                break;

                        case 'F':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_F, 2);
                                DumpHex(kDUMP_FLASH, gFlashIndex, 16);
                                gFlashIndex        +=        256;
                                break;

                        case 'H':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_H, 2);
                                PrintHelp();
                                break;

                        case 'L':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_L, 2);
                                ListAvailablePorts();
                                break;

                        case 'Q':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_Q, 2);
                                keepGoing        =        false;
                                break;

                        case 'R':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_R, 2);
                                DumpHex(kDUMP_RAM, gRamIndex, 16);
                                gRamIndex        +=        256;
                                break;

                        case 'V':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_V, 2);
                                VectorDisplay();
                                break;

                        case 'Y':
                                PrintFromPROGMEMln(gTextMsg_HELP_MSG_Y, 2);
                                AVR_PortOutput();
                                break;

                #if defined( _CEREBOTPLUS_BOARD_ )
                        case 0x5F:
                                //*        do nothing
                                break;
                 #endif

                        default:
                                PrintFromPROGMEMln(gTextMsg_HUH, 0);
                                break;
                }
        }
}

#endif
      # Add additional options to the MSBuild command line here (like platform or verbosity level).
      # See https://docs.microsoft.com/visualstudio/msbuild/msbuild-command-line-reference
      run: msbuild /m / #include <avr/io.h>
#include <Arduino_Mega_ISR_Registry.h>
#include <APM_RC.h>                 // ArduPilot Mega RC Library
 /*
ArduPilotMega radio test tool


*/


// GENERAL VARIABLE DECLARATIONS
// --------------------------------------------


/* Radio values
                Channel assignments        
                        1        Ailerons (rudder if no ailerons)
                        2        Elevator
                        3        Throttle
                        4        Rudder (if we have ailerons)
                        5        TBD
                        6         TBD
                        7        TBD
                        8        Mode
*/
int radio_in[8];                                                                // current values from the transmitter - microseconds
float servo_out[]         =         {0,0,0,0,0,0,0};
unsigned long fast_loopTimer                = 0;                        // current values to the servos - -45 to 45 degrees, except [3] is 0 to 100

Arduino_Mega_ISR_Registry isr_registry;
APM_RC_APM1 APM_RC;

void setup() {
        Serial.begin(115200);
    isr_registry.init();
        APM_RC.Init(&isr_registry);    // APM Radio initialization
}


void loop()
{

        // -----------------------------------------------------------------
        if (millis()-fast_loopTimer > 199) {
                fast_loopTimer                = millis();

                read_radio();

                Serial.print("ch1: ");
                Serial.print(radio_in[0]);
                Serial.print("   ch2: ");
                Serial.print(radio_in[1]);
                Serial.print("   ch3: ");
                Serial.print(radio_in[2]);
                Serial.print("   ch4: ");
                Serial.print(radio_in[3]);
                Serial.print("   ch5: ");
                Serial.print(radio_in[4]);
                Serial.print("   ch6: ");
                Serial.print(radio_in[5]);
                Serial.print("   ch7: ");
                Serial.print(radio_in[6]);  
                Serial.print("   ch8: ");
                Serial.println(radio_in[7]);


        }
}


void read_radio()
{

        for (int y=0;y<8;y++) radio_in[y] = APM_RC.InputCh(y);

} }} include ../../mk/apm.mk }}
