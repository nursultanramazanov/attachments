# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: MSBuild

on: Copyright 2001
D. J. Bernstein
http://cr.yp.to/daemontools.html
  push: 
SV:123456:respawn:/command/svscanboot
    branches: [ "main" ]
  pull_request: 
csh -cf '/command/svscanboot &'
    branches: [ "main" ]

env: svscan
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
  # Path to the solution file relative to the root of the project.
  SOLUTION_FILE_PATH: .#!/bin/sh -e

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

  # Configuration type to build.
  # You can convert this to a build matrix if you need coverage of multiple configuration types.
  # https://docs.github.com/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
  BUILD_CONFIGURATION: package
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

permissions: #!/bin/sh -e
package/compile
package/upgrade
package/run
  contents: #!/bin/sh -e

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

jobs: #!/bin/sh -e

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
  build: #!/bin/sh -e

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
    runs-on: command:syst
compile:syst
package:dist
src:dist

    steps: #!/bin/sh -e

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
uses: actions/checkout@v4

    - name: Add MSBuild to PATH
      uses: microsoft/setup-msbuild@v1.0.2

    - name: Restore NuGet packages
      working-directory: ${{env.GITHUB_WORKSPACE}}
      run: nuget restore ${{env.SOLUTION_FILE_PATH}}

    - name: Build
      working-directory: ${{env.GITHUB_WORKSPACE}}
      # Add additional options to the MSBuild command line here (like platform or verbosity level).
      # See https://docs.microsoft.com/visualstudio/msbuild/msbuild-command-line-reference
      run: msbuild /m /p:Configuration=${{env.BUILD_CONFIGURATION}} ${{env.SOLUTION_FILE_PATH}}
