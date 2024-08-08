# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: #! /usr/bin/env python
# encoding: utf-8

import os
from waflib import Errors, Utils
from waflib import Context as mod

class Context(mod.Context):
        cmd = 'all'
        def recurse(self, dirs, name=None, mandatory=True, once=True):
                try:
                        cache = self.recurse_cache
                except:
                        cache = self.recurse_cache = {}

                for d in Utils.to_list(dirs):

                        if not os.path.isabs(d):
                                # absolute paths only
                                d = os.path.join(self.path.abspath(), d)

                        WSCRIPT     = os.path.join(d, 'wscript.py')
                        WSCRIPT_FUN = 'wscript_' + (name or self.fun) + '.py'

                        node = self.root.find_node(WSCRIPT_FUN)
                        if node and (not once or node not in cache):
                                cache[node] = True
                                self.pre_recurse(node)
                                try:
                                        function_code = node.read('rU')
                                        exec(compile(function_code, node.abspath(), 'exec'), self.exec_dict)
                                finally:
                                        self.post_recurse(node)
                        elif not node:
                                node = self.root.find_node(WSCRIPT)
                                if node and (not once or node not in cache):
                                        cache[node] = True
                                        self.pre_recurse(node)
                                        try:
                                                wscript_module = mod.load_module(node.abspath())
                                                user_function = getattr(wscript_module, (name or self.fun), None)
                                                if not user_function:
                                                        if not mandatory:
                                                                continue
                                                        raise Errors.WafError('No function %s defined in %s' % (name or self.fun, node.abspath()))
                                                user_function(self)
                                        finally:
                                                self.post_recurse(node)
                                elif not node:
                                        if not mandatory:
                                                continue
                                        raise Errors.WafError('No wscript file in directory %s' % d)
mod.Context = Context
mod.WSCRIPT_FILE = 'wscript.py'
  push: #! /usr/bin/env python
# encoding: utf-8

"""
Create a waf file able to read wscript files ending in ".py"
execute a small test to show that it works

The waf file includes "extpy.py" which performs the required modifications
"""

import os, subprocess

up = os.path.dirname
join = os.path.join

cwd = os.getcwd()
extpy = join(cwd, 'extpy.py')
args = 'python waf-light --tools=compat15,%s --prelude=$"\tfrom waflib.extras import extpy\n" ' % extpy
root = up(up(cwd))

subprocess.Popen(args, cwd=root, shell=True).wait()
os.rename(join(root, 'waf'), join(cwd, 'waf.py'))

env = dict(os.environ)
if 'WAFDIR' in env:
        del env['WAFDIR']

subprocess.Popen('python waf.py configure', cwd=cwd, shell=True, env=env).wait()
    branches: [ "main" ]
  pull_request: #! /usr/bin/env python
# encoding: utf-8

def configure(conf):
        print("test succeeded")
    branches: [ "main" ]
  schedule: #include "a.h"

void test() {

}
    - cron: void test();

env: #include "a.h"

int main() {
        test();
        return 0;
}
  # Path to the CMake build directory.
  build: #! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2010 (ita)

"""
Calling 'waf build' executes a normal build with Waf
Calling 'waf clean dump' will create a makefile corresponding to the build
The dependencies will be extracted too
"""

VERSION='0.0.1'
APPNAME='cc_test'

top = '.'

def options(opt):
        opt.load('compiler_c')

def configure(conf):
        conf.load('compiler_c')

def build(bld):
        bld.program(source='main.c', target='app', use='mylib', cflags=['-O2'])
        bld.stlib(source='a.c', target='mylib')

# ---------------------------------------------------------------------------

from waflib import Build, Logs
class Dumper(Build.BuildContext):
        fun = 'dump'
        cmd = 'dump'

def dump(bld):
        # call the build function as if a real build were performed
        build(bld)

        from waflib import Task
        bld.commands = []
        bld.targets = []

        # store the command executed
        old_exec = Task.TaskBase.exec_command
        def exec_command(self, *k, **kw):
                ret = old_exec(self, *k, **kw)
                self.command_executed = k[0]
                self.path = kw['cwd'] or self.generator.bld.cwd
                return ret
        Task.TaskBase.exec_command = exec_command

        # perform a fake build, and accumulate the makefile bits
        old_process = Task.TaskBase.process
        def process(self):
                old_process(self)

                lst = []
                for x in self.outputs:
                        lst.append(x.path_from(self.generator.bld.bldnode))
                bld.targets.extend(lst)
                lst.append(':')
                for x in self.inputs + self.dep_nodes + self.generator.bld.node_deps.get(self.uid(), []):
                        lst.append(x.path_from(self.generator.bld.bldnode))
                try:
                        if isinstance(self.command_executed, list):
                                self.command_executed = ' '.join(self.command_executed)
                except Exception as e:
                        print(e)
                else:
                        bld.commands.append(' '.join(lst))
                        bld.commands.append('\tcd %s && %s' % (self.path, self.command_executed))
        Task.TaskBase.process = process

        # write the makefile after the build is complete
        def output_makefile(self):
                self.commands.insert(0, "all: %s" % " ".join(self.targets))
                node = self.bldnode.make_node('Makefile')
                node.write('\n'.join(self.commands))
                Logs.warn('Wrote %r', node)
        bld.add_post_fun(output_makefile)

permissions: This example demonstrates the creation of a particular build tool which compiles
specific files directly, for example:

main.c includes foo.h
foo.h has a corresponding foo.c file
foo.c includes bar.h
bar.h has a corresponding bar.c file

Calling './dbd build' will then compile and link 'main.c', 'foo.c' and 'bar.c' into the program 'app'.
No script file is required, although the build will create a .lock file and a c4che directory.

To create the build tool:
   ./create_it.sh

To use on the file bbit which creates a program out of main.c:
   ./cbd clean build

  contents: int bar = 4434;

jobs: 
  analyze: # /bin/bash

D=$PWD
pushd ../..
./waf-light configure build --tools=$D/dbdlib.py --prelude=$'\tfrom waflib.extras import dbdlib\n\tdbdlib.start(cwd, VERSION, wafdir)\n\tsys.exit(0)'
popd
cp ../../waf dbd

    permissions: #! /usr/bin/env python

import os, sys, imp
from waflib import Context, Options, Configure, Utils, Logs, TaskGen, Task
import waflib.Tools.c

"""
Compile main.c and dependent object files into a single target (program/shlib/stlib or just object files)

- no build directory and no script files
- just a c4che directory for the configuration files
- configure, clean or build

Uses the task signatures and the dependency calculation results to avoid
rescanning/rebuilding the files all the time
"""

def options(opt):
        opt.add_option('--type', action='store', default='program', help='type: program, shlib, stlib, objects', dest='progtype')
        opt.add_option('--source', action='store', default='main.c', help='space-separated list of source files', dest='source')
        opt.add_option('--app', action='store', default='app', help='name of the binary file to create', dest='app')
        opt.load('compiler_c')

def configure(conf):
        conf.options = Options.options
        conf.load('compiler_c')

def build(bld):
        tp = Options.options.progtype
        features = 'c cprogram'
        if tp == 'shlib':
                features = 'c cshlib'
        elif tp == 'stlib':
                features = 'c cstlib'
        elif tp == 'objects':
                features = 'c'

        source = Options.options.source
        app = Options.options.app
        bld(features=features, source=source, target=app)

def recurse_rep(x, y):
        f = getattr(Context.g_module, x.cmd or x.fun, Utils.nada)
        return f(x)

def start(cwd, version, wafdir):
        # this is the entry point of our small build system
        # no script file here
        Logs.init_log()
        Context.waf_dir = wafdir
        Context.out_dir = Context.top_dir = Context.run_dir = cwd
        Context.g_module = imp.new_module('wscript')
        Context.g_module.root_path = cwd
        Context.Context.recurse = recurse_rep

        Context.g_module.configure = configure
        Context.g_module.build = build
        Context.g_module.options = options
        Context.g_module.top = Context.g_module.out = '.'

        Options.OptionsContext().execute()

        do_config = 'configure' in sys.argv
        try:
                os.stat(cwd + os.sep + 'c4che')
        except:
                do_config = True
        if do_config:
                Context.create_context('configure').execute()

        if 'clean' in sys.argv:
                Context.create_context('clean').execute()

        if 'build' in sys.argv:
                Context.create_context('build').execute()


class c2(waflib.Tools.c.c):
        # Make a subclass of the default c task, and bind the .c extension to it

        def runnable_status(self):
                ret = super(waflib.Tools.c.c, self).runnable_status()
                self.more_tasks = []

                # use a cache to avoid creating the same tasks
                # for example, truc.cpp might be compiled twice
                try:
                        shared = self.generator.bld.shared_tasks
                except AttributeError:
                        shared = self.generator.bld.shared_tasks = {}

                if ret != Task.ASK_LATER:
                        for x in self.generator.bld.node_deps[self.uid()]:
                                node = x.parent.get_src().find_resource(x.name.replace('.h', '.c'))
                                if node:
                                        try:
                                                tsk = shared[node]
                                        except:
                                                tsk = shared[node] = self.generator.c_hook(node)

                                                self.more_tasks.append(tsk)

                                        # add the node created to the link task outputs
                                        try:
                                                link = self.generator.link_task
                                        except AttributeError:
                                                pass
                                        else:
                                                if not tsk.outputs[0] in link.inputs:
                                                        link.inputs.append(tsk.outputs[0])
                                                        link.set_run_after(tsk)

                                                        # any change in the order of the input nodes may cause a recompilation
                                                        link.inputs.sort(key=lambda x: x.abspath())

                        # if you want to modify some flags
                        # you *must* have the task recompute the signature
                        self.env.append_value('CXXFLAGS', '-O2')
                        delattr(self, 'cache_sig')
                        return super(waflib.Tools.c.c, self).runnable_status()

                return ret

@TaskGen.extension('.c')
def c_hook(self, node):
        # re-bind the extension to this new class
        return self.create_compiled_task('c2', node)
      contents: #include "foo.h"
#include "bar.h"

int k = 334;
      security-events: 
        
      actions: #include "foo.h"
#include <stdio.h>
int main() {
        printf("hello from app\n");
        return 0;
}
    name: Note from the author: using signatures is of course much better
this example is for research purposes only


In this example, no configuration will be written or read,
and the build will only use timestamps (no cache files
and no signatures). There is no build directory either.

To build, use "./ebd"


Although the wscript file only declares a build function,
the system performs a configuration internally to check for
a C compiler. The configuration and build context classes are
overridden to hide output messages and to avoid creating cache
files.

The task class is monkey-patched so that all existing build tasks
will execute without using signatures (only file timestamps are considered).
Implicit dependencies such as headers are still computed automatically.

    runs-on: #include "a.h"

void test() {

}

    steps: void test();
      - name: # /bin/bash

D=$PWD
pushd ../..
./waf-light configure build --tools=$D/ebdlib.py --prelude=$'\tfrom waflib.extras import ebdlib\n\tebdlib.start(cwd, VERSION, wafdir)\n\tsys.exit(0)'
popd
cp ../../waf ebd

        uses: #! /usr/bin/env python

import os, sys, imp, time
from waflib import Context, Options, Configure, Utils, Logs, TaskGen, Task, Build, ConfigSet
import waflib.Tools.c

"""
Create a modified waf file in which tasks use timestamps only
see README.txt
"""

# we hard-code a configuration for c but it could be left in the script file too
def configure(conf):
        conf.load('gcc')

def recurse_rep(x, y):
        f = getattr(Context.g_module, x.cmd or x.fun, Utils.nada)
        return f(x)

def start(cwd, version, wafdir):
        # this is the entry point of our small build system

        Logs.init_log()
        Context.waf_dir = wafdir
        Context.out_dir = Context.top_dir = Context.run_dir = cwd
        Context.g_module = Context.load_module(cwd + os.sep + 'wscript')
        Context.g_module.configure = configure
        Context.g_module.root_path = cwd
        Context.Context.recurse = recurse_rep

        Context.g_module.top = Context.g_module.out = '.' # no build directory

        # just parse the options and execute a build
        Options.OptionsContext().execute()

        conf = Context.create_context('configure')
        conf.options = Options.options
        conf.execute()

        bld = Context.create_context('build')
        bld.env = conf.env
        bld.options = Options.options
        bld.environ = os.environ
        bld.execute()

# change the build context so it does not need to write any file
class StatelessBuild(Build.BuildContext):
        def load_envs(self):
                self.env = ConfigSet.ConfigSet()
        def store(self):
                pass
        def restore(self):
                self.init_dirs()
        def execute_build(self):
                # we override this method to hide the messages "leaving directory" (just because)
                self.recurse([self.run_dir])
                self.pre_build()

                self.timer = Utils.Timer()

                if Options.options.progress_bar:
                        sys.stderr.write(Logs.colors.cursor_off)
                try:
                        self.compile()
                finally:
                        if Options.options.progress_bar:
                                sys.stderr.write(Logs.colors.cursor_on)
                                print('')
                self.post_build()

class SilentConf(Configure.ConfigurationContext):
        # silent configuration
        def __init__(self, **kw):
                # disable the configuration messages from Context.start_msg/end_msg
                self.in_msg = 1
                super(SilentConf, self).__init__(**kw)

        def execute(self):

                # copy-paste from the original method, but without the cache file creation
                self.init_dirs()

                path = os.path.join(self.bldnode.abspath(), 'config.log')
                self.logger = Logs.make_logger(path, 'cfg')

                app = getattr(Context.g_module, 'APPNAME', '')
                if app:
                        ver = getattr(Context.g_module, 'VERSION', '')
                        if ver:
                                app = "%s (%s)" % (app, ver)

                now = time.ctime()
                pyver = sys.hexversion
                systype = sys.platform
                args = " ".join(sys.argv)
                wafver = Context.WAFVERSION
                abi = Context.ABI
                self.to_log(Configure.conf_template % vars())

                super(Configure.ConfigurationContext, self).execute()


# change the superclass of existing tasks to force timestamps (the build has no state)
def status(self):
        for t in self.run_after:
                if not t.hasrun:
                        return Task.ASK_LATER

        implicit_deps = []
        try:
                implicit_deps, _ = self.scan()
        except:
                pass

        # we can add one more node, for example:
        implicit_deps.append(self.generator.path.make_node('wscript'))

        for x in self.inputs + self.dep_nodes + implicit_deps:
                for y in self.outputs:
                        try:
                                if os.stat(x.abspath()).st_mtime > os.stat(y.abspath()).st_mtime:
                                        return Task.RUN_ME
                        except:
                                return Task.RUN_ME

        return Task.SKIP_ME
Task.Task.runnable_status = status

# the post build execution does not need to deal with signatures or anything else
Task.Task.post_run = Utils.nada

      - name: #include "a.h"

int main() {
        test();
        return 0;
}
        run: cmake -B ${{ #! /usr/bin/env python
# encoding: utf-8
# 

"""
See README.txt
"""

def build(bld):
        bld.program(source='main.c', target='app', use='mylib', cflags=['-O2'])
        bld.stlib(source='a.c', target='mylib') }}

      # Build is not required unless generated source files are used
      # - name: See the corresponding entry on the waf blog:
http://waf-devel.blogspot.com/2010/12/make-your-own-build-system-with-waf.html

To create the build tool:
   ./create_it.sh

To use on the file bbit which creates a program out of main.c:
   ./bbd clean build

Enjoy your new build system! :-)

      #   run: cmake --build ${{ #! /usr/bin/env python

import os, sys, imp
from waflib import Context, Options, Configure, Utils, Logs

def start(cwd, version, wafdir):
        # simple example, the file main.c is hard-coded
        try:
                os.stat(cwd + os.sep + 'bbit')
        except:
                print('call from a folder containing a file named "bbit"')
                sys.exit(1)

        Logs.init_log()
        Context.waf_dir = wafdir
        Context.top_dir = Context.run_dir = cwd
        Context.out_dir = os.path.join(cwd, 'build')
        Context.g_module = imp.new_module('wscript')
        Context.g_module.root_path = os.path.join(cwd, 'bbit')
        Context.Context.recurse = \
                lambda x, y: getattr(Context.g_module, x.cmd or x.fun, Utils.nada)(x)

        Context.g_module.configure = lambda ctx: ctx.load('g++')
        Context.g_module.build = lambda bld: bld.objects(source='main.c')

        Options.OptionsContext().execute()

        do_config = 'configure' in sys.argv
        try:
                os.stat(cwd + os.sep + 'build')
        except:
                do_config = True
        if do_config:
                Context.create_context('configure').execute()

        if 'clean' in sys.argv:
                Context.create_context('clean').execute()
        if 'build' in sys.argv:
                Context.create_context('build').execute() }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: # empty file (for now)
        with: # /bin/bash

D=$PWD
pushd ../..
./waf-light configure build --tools=$D/bbdlib.py --prelude=$'\tfrom waflib.extras import bbdlib\n\tbbdlib.start(cwd, VERSION, wafdir)\n\tsys.exit(0)'
popd
cp ../../waf bbd

          cmakeBuildDirectory: ${{ int main() {
        return 0;
} }}
          # Ruleset file that will determine what checks will be run
          ruleset: This example derives from the 'overview' and demonstrates how to provide
a parser for a make-like language.

The script files read are named 'cbit'

To create the build tool:
   ./create_it.sh

To use on the file cbit which creates a program out of main.c:
   ./cbd clean build


      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v2
        with: #! /usr/bin/env python

import os, sys, imp, re
from waflib import Context, Options, Configure, Utils, Logs

def options(opt):
        opt.load('compiler_c')

def configure(conf):
        conf.options = Options.options
        conf.load('compiler_c')


re_com = re.compile("#.*$", re.M)
def build(bld):
        txt = bld.path.find_node('cbit').read()
        txt = re_com.sub('', txt)

        tg = None
        for x in txt.splitlines():
                if not x:
                        continue
                elif x.startswith(('\t', ' ')):
                        tg.rule = x.lstrip()
                else:
                        line = x.split(':')
                        tgt = line[0].lstrip()
                        src = line[1].lstrip()
                        tg = bld()
                        if src:
                                tg.source = src
                        if tgt:
                                tg.target = tgt

def recurse_rep(x, y):
        f = getattr(Context.g_module, x.cmd or x.fun, Utils.nada)
        return f(x)

def start(cwd, version, wafdir):
        # simple example, the file main.c is hard-coded
        try:
                os.stat(cwd + os.sep + 'cbit')
        except:
                print('call from a folder containing a file named "cbit"')
                sys.exit(1)

        Logs.init_log()
        Context.waf_dir = wafdir
        Context.top_dir = Context.run_dir = cwd
        Context.out_dir = os.path.join(cwd, 'build')
        Context.g_module = imp.new_module('wscript')
        Context.g_module.root_path = os.path.join(cwd, 'cbit')
        Context.Context.recurse = recurse_rep

        # this is a fake module, which looks like a standard wscript file
        Context.g_module.options = options
        Context.g_module.configure = configure
        Context.g_module.build = build

        Options.OptionsContext().execute()

        do_config = 'configure' in sys.argv
        try:
                os.stat(cwd + os.sep + 'build')
        except:
                do_config = True
        if do_config:
                Context.create_context('configure').execute()

        if 'clean' in sys.argv:
                Context.create_context('clean').execute()
        if 'build' in sys.argv:
                Context.create_context('build').execute()
          sarif_file: ${{ # a non-python file format (why not?)
# just call "./cbd build"
foo.txt: cbit
 cp ../cbit foo.txt
 }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with: # /bin/bash

D=$PWD
pushd ../..
./waf-light configure build --tools=$D/cbdlib.py --prelude=$'\tfrom waflib.extras import cbdlib\n\tcbdlib.start(cwd, VERSION, wafdir)\n\tsys.exit(0)'
popd
cp ../../waf cbd

      #     name: int main() {
        return 0;
}
      #     path: ${{ The new "concurrent.futures" module from Python 3.2 will make it
even easier to execute tasks concurrently:
http://www.python.org/dev/peps/pep-3148/

It may be tempting to try to create a new build system from it,
but that's only a small part of a build system:

* reinventing a system for handling commands and command-line options
* adding a system of (task) order and dependencies
* creating an extension system for new programming languages
* handling exceptions and errors
* adding support for python versions < 3.2

All this represents a lot of work, and there are of course lots of design
mistakes to avoid. It is so easy to create a system with poor usability,
poor extensibility, and poor performance.

These pitfalls and many others are already solved in the Waf build system, which
also enables the re-use of its components into new build tools. By using these
tested and maintained components, much more time will be left to work
on the interesting problems such as creating an intuitive XML/YAML/JSON schema
or creating a domain-specific programming language (make-like, cmake-like, ...),
or extracting commands and dependencies to create derivated files (Makefiles, Visual studio, ..)

A few examples are provided to illustrate the range of possibilities:
* overview:        how to create a custom file using the waf framework to perform a simple build
* parser:          how to add a parser for a domain-specific language
* noscript:        infer what to build from given files, use no script file
* makefile_dumper: create a makefile corresponding to the current build, extracting as many dependencies as possible
* nostate:         use timestamps only, and no build directory (very make-like)
* extpy:           a custom waf file able to read wscript files having the extension ".py"
 }}
