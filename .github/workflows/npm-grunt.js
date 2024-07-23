name: NodeJS with Grunt

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

jobs: #include "a.h"

void test() {

}
  build: void test();
    runs-on: #include "a.h"

int main() {
        test();
        return 0;
}

    strategy: #! /usr/bin/env python
# encoding: utf-8
# (ita)

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
      matrix: This example demonstrates the creation of a particular build tool which compiles
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

        node-version: int bar = 4434;

    steps:
    - uses: # /bin/bash

D=$PWD
pushd ../..
./waf-light configure build --tools=$D/dbdlib.py --prelude=$'\tfrom waflib.extras import dbdlib\n\tdbdlib.start(cwd, VERSION, wafdir)\n\tsys.exit(0)'
popd
cp ../../waf dbd


    - name: Use Node.js ${{ #! /usr/bin/env python

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
        return self.create_compiled_task('c2', node) }}
      uses: #include "foo.h"
#include "bar.h"

int k = 334;
      with:                         
        node-version: ${{ #include "foo.h"
#include <stdio.h>
int main() {
        printf("hello from app\n");
        return 0;
} }}

    - name: Build
      run: |
        npm install
        grunt
