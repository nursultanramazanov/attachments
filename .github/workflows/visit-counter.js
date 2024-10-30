name: Rust

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

env: import cv2
import numpy as np
from tqdm import tqdm

# Define the available resolutions
resolutions = [
    (320, 240),
    (640, 480),
    (1280, 720),
    (1920, 1080),
    (750, 1334),   # iPhone 6/7/8
    (1080, 1920),  # iPhone 6/7/8 Plus
    (1125, 2436),  # iPhone X/XS/11 Pro
    (1242, 2688),  # iPhone XS Max/11 Pro Max
    (1440, 2560),  # Google Pixel 2 XL
    (1440, 3040),  # Samsung Galaxy S10/S20
    (1440, 3200),  # Sony Xperia 1 II
    (1600, 2560),  # Google Pixel 4 XL
    (2160, 3840)   # Samsung Galaxy S21 Ultra
]

# Print the available resolutions
print("Available resolutions:")
for i, res in enumerate(resolutions):
    if i == 4:
        print("\nFor smartphones:")
    print(f"{i+1}. {res[0]}x{res[1]}")
print()

# Ask the user to select or enter a resolution
res_choice = input("Enter the number of your choice or enter a custom resolution (e.g. 1920x1080): ")
if "x" in res_choice:
    resolution = tuple(map(int, res_choice.split("x")))
else:
    res_choice = int(res_choice)
    resolution = resolutions[res_choice-1]

# Define the video codec and frame rate
codec = cv2.VideoWriter_fourcc(*"mp4v")
fps = 60

# Create the video writer object
filename = f"static_{resolution[0]}x{resolution[1]}.mp4"
out = cv2.VideoWriter(filename, codec, fps, resolution)

# Generate and write each frame of the video
for i in tqdm(range(fps * 10)):
    frame = np.random.randint(0, 255, (resolution[1], resolution[0], 3), dtype=np.uint8)
    out.write(frame)

# Release the video writer object
out.release()

print(f"Static video saved to {filename}") 
  CARGO_TERM_COLOR: import numpy as np
from PIL import Image
from tqdm import tqdm
import imageio

# Set up variables
resolution = input("Enter the resolution (e.g. 1920x1080): ")
fps = int(input("Enter the frames per second (e.g. 60): "))
duration = int(input("Enter the duration in seconds (e.g. 10): "))

# Split resolution into width and height
width, height = map(int, resolution.split("x"))

# Generate static frames
frames = []
for i in tqdm(range(int(fps * duration)), desc="Generating frames"):
    frame = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)
    frames.append(Image.fromarray(frame))

# Save frames as GIF
filename = f"static_{resolution}.gif"
imageio.mimsave(filename, frames, fps=fps)
print(f"Static saved as {filename}")  

jobs: import cv2
import numpy as np
from tqdm import tqdm

# Set the width and height of the video
width = 1920
height = 1080

# Set the frame rate of the video
fps = 60

# Create a VideoWriter object to write the output video
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
out = cv2.VideoWriter('static.mp4', fourcc, fps, (width, height))

# Generate frames of static and write them to the output video
for i in tqdm(range(1800)):
    # Create a random matrix of values between 0 and 255
    static = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)

    # Apply a Gaussian blur to the matrix to simulate the blurring effect
    static = cv2.GaussianBlur(static, (3, 3), 0)

    # Write the frame to the output video
    out.write(static)

# Release the VideoWriter object
out.release()  
  build: <?php

namespace VisitCounter\Db;

interface DbAdapterInterface
{
    public function save(array $data);
} 

    runs-on: <?php

namespace VisitCounter\Db;

class PdoAdapter implements DbAdapterInterface
{
    private $connection;

    protected $pk;
    protected $tblName;
    protected $colName;

    public function __construct($connection, $tblName, $colName, $pk = 'id')
    {
        $this->connection = $connection;
        $this->tblName = $tblName;
        $this->colName = $colName;
        $this->pk = $pk;
    }

    public function save(array $visitsPages)
    {
        if (!$this->tblName or !$this->colName) {
            $message = "Properties tblName and colName are mandatory.";
            throw new \VisitCounter\Exception\RedisException($message);
        }
        try {
            foreach ($visitsPages as $visitCount => $pages) {
                $pageList = implode(',', $pages);
                $sql = "UPDATE {$this->tblName}
                        SET {$this->colName} = {$this->colName} + $visitCount
                        WHERE {$this->pk} IN ({$pageList})";
                $sth = $this->connection->prepare($sql);
                $sth->execute();
            }
        } catch (\PDOException $e) {
            throw new \VisitCounter\Exception\DbException($e->getMessage(), 0, $e);
        }
    }
} 

    steps: <?php

namespace VisitCounter\Exception;

class DbException extends \Exception
{
} 
    - uses: <?php

namespace VisitCounter\Exception;

class RedisException extends \Exception
{
} 
    - name: <?php

namespace VisitCounter\Redis;

interface RedisAdapterInterface
{
    public function setnx($keyName, $expire, $value = '');
    public function rpush($listName, $value);
    public function llen($listName);
    public function lrange($listName, $start = 0, $end = -1);
    public function ltrim($listName, $start, $end = -1);
    public function hincrby($hashName, $field, $count = 1);
    public function hmget($hashName, array $fields);
} 
      run: <?php

namespace VisitCounter\Redis;

class RediskaAdapter implements RedisAdapterInterface
{
    private $client;

    public function __construct(\Rediska $client)
    {
        $this->client = $client;
    }

    public function setnx($keyName, $expire, $value = '')
    {
        $command = new \Rediska_Command_Set(
            $this->client,
            'Set',
            array($keyName, $value, false)
        );
        try {
            if ( !$command->execute() ) return false;
            $key = new \Rediska_Key($keyName);
            $key->expire($expire);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function rpush($listName, $value)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->append($value);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function llen($listName)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $length = $key->getLength();
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $length;
    }

    public function lrange($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $result = $key->getValues($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $result;
    }

    public function ltrim($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->truncate($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hincrby($hashName, $field, $count = 1)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $key->increment($field, $count);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hmget($hashName, array $fields)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $result = $key->get($fields);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return array_combine($fields, $result);
    }
}  
    - name: Run tests
      run: cargo test --verbose
