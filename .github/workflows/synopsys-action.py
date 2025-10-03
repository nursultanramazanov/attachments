# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
name: Synopsys Security Testing

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /#!usr/bin/env python
__author__    = "Westwood Robotics Corporation"
__email__     = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2025 Westwood Robotics Corporation"
__date__      = "July 29, 2025"
__project__   = "PyBEAR"
__version__   = "0.1.3"
__status__    = "Production"

'''
Move BEAR from the current angle to a specified angle
'''

import time
from pybear import Manager


error = False
bear = Manager.BEAR(port="/dev/ttyUSB0", baudrate=8000000)  # need to identify the port name on your PC

m_id = 1  # BEAR ID (default is 1)

p_gain = 5.0  # Set P gain as spring stiffness
d_gain = 0.2  # Set D gain as damper strength
i_gain = 0.0  # I gain is usually not needed
iq_max = 1.5  # Max iq

BEAR_connected = bear.ping(m_id)[0][1] is not None
if not BEAR_connected:
    # BEAR is offline
    print("BEAR is offline. Check power and connection.")
    error = True
    exit()

if not error:
    # BEAR is online
    # Set PID, mode, and limit
    print("Welcome aboard, Captain!")
    # PID id/iq
    bear.set_p_gain_iq((m_id, 0.02))
    bear.set_i_gain_iq((m_id, 0.02))
    bear.set_d_gain_iq((m_id, 0))
    bear.set_p_gain_id((m_id, 0.02))
    bear.set_i_gain_id((m_id, 0.02))
    bear.set_d_gain_id((m_id, 0))

    # PID position mode
    bear.set_p_gain_position((m_id, p_gain))
    bear.set_i_gain_position((m_id, i_gain))
    bear.set_d_gain_position((m_id, d_gain))

    # Put into position mode
    bear.set_mode((m_id, 2))

    # Set iq limit
    bear.set_limit_i_max((m_id, iq_max))

    # Start demo
    input('Press Enter to start demo. ')

    # Get home position
    home = bear.get_present_position(m_id)[0][0][0]
    print(home)

    # Set goal position before enabling BEAR
    bear.set_goal_position((m_id, home))

    # Enable BEAR
    bear.set_torque_enable((m_id, 1))

    # Demo spring-damping system
    print('You can play with BEAR now! It is simulating a spring-damping system.')
    time.sleep(2)

    # Get command position
    angle = float(input('Input the angle in radians you want to move BEAR to (e.g., -0.2): '))

    # Let's move to the target angle smoothly
    num = 100                  # split it into 100 pieces
    delta_angle = angle / num  # angle for each time
    for i in range(num):
        goal_pos = home + delta_angle * (i + 1)
        bear.set_goal_position((m_id, goal_pos))
        time.sleep(0.01)

    print('BEAR arrived target angle!')
    time.sleep(2)

    # Turn off BEAR
    input('Press Enter to turn off BEAR.')

    # Disable BEAR
    bear.set_torque_enable((m_id, 0))
    print("Thanks for using BEAR!") --name #!usr/bin/env python
__author__    = "Westwood Robotics Corporation"
__email__     = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2025 Westwood Robotics Corporation"
__date__      = "July 29, 2025"
__project__   = "PyBEAR"
__version__   = "0.1.3"
__status__    = "Production"

'''
BEAR with a bar, spring on one side, damper on the other side, with the 90 degree in the center being free
'''

import os
import sys
import select
from pybear import Manager


error = False
bear = Manager.BEAR(port="COM9", baudrate=8000000)

m_id = 1  # Set motor ID

p_gain = 5  # Set P gain as the K of spring
d_gain = 2  # Set D gain as damper strength
iq_max = 1.5  # Max iq

if bear.ping(m_id)[0][1] is None:
    # BEAR is offline
    print("BEAR is offline. Check power and connection.")
    error = True

if not error:
    # BEAR is online
    # Set PID, mode and limit
    print("Welcome aboard, Captain!")
    # PID idiq
    bear.set_p_gain_iq((m_id, 0.02))
    bear.set_i_gain_iq((m_id, 0.02))
    bear.set_d_gain_iq((m_id, 0))
    bear.set_p_gain_id((m_id, 0.02))
    bear.set_i_gain_id((m_id, 0.02))
    bear.set_d_gain_id((m_id, 0))

    # Clear PID direct force
    bear.set_p_gain_force((m_id, 0))
    bear.set_i_gain_force((m_id, 0))
    bear.set_d_gain_force((m_id, 0))

    # Put into torque mode
    bear.set_mode((m_id, 3))

    # Set iq limit
    bear.set_limit_i_max((m_id, iq_max))

    # Put bar in middle
    # usr = input("Please move the bar to upright position, then press enter to continue.")
    # Get home position
    home = bear.get_present_position(m_id)[0][0][0]
    print("Demo started.")
    run = True
    while run:
        os.system('cls' if os.name == 'nt' else 'clear')
        pos = bear.get_present_position(m_id)[0][0][0]
        if abs(pos - home) < 0.78539816339:  # If distance between present_pos and home is lower than 45 deg
            # Disable and do nothing
            print("BEAR in free range.")
            bear.set_torque_enable((m_id, 0))
        elif (pos - home) > 0.78539816339: # If BEAR on the right side of free range
            # Enable and get into damping only
            print("BEAR in damping.")
            bear.set_p_gain_force((m_id, 0))
            bear.set_i_gain_force((m_id, 0))
            bear.set_d_gain_force((m_id, d_gain))
            bear.set_torque_enable((m_id, 1))

        elif (pos - home) < -0.78539816339:  # If BEAR on the left side of free range
            # Enable and get into spring only
            print("BEAR is now a spring.")
            bear.set_p_gain_force((m_id, p_gain))
            bear.set_i_gain_force((m_id, 0))
            bear.set_d_gain_force((m_id, 0))
            bear.set_torque_enable((m_id, 1))
            bear.set_goal_position((m_id, (home-0.78539816339)))
            # Check if iq_max reached
            iq = bear.get_present_iq(m_id)[0][0]
            if iq > iq_max:
                print("iq max reached.")
        print("Press any key to stop.")
        if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            line = input()
            run = False
            print("Demo terminated by user.")
            break
    print("Thanks for using BEAR!") --privileged tizenrt/tizenrt:#! /usr/bin/env python
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
mod.WSCRIPT_FILE = 'wscript.py' /bin/bash
        docker cp ./. #! /usr/bin/env python
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
:/#! /usr/bin/env python
# encoding: utf-8

def configure(conf):
	print("test succeeded")
        docker run -d --rm -it --name ${BUILDER} --volumes-from=${VOLUME} -w /${WORKING_DIR}/os --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec ${BUILDER} arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${BUILDER} bash -c "cd tools; ./configure.sh ${CIRCLE_JOB}"
        docker exec -it ${BUILDER} bash -c "make"


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

  rtl8730e/loadable_ext_ddr:
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
      - rtl8730e/loadable_ext_ddr:
          requires:
            - checkout_code
  push:
    # At this time, it is recommended to run Polaris only on pushes to main branches
    # Pull request analysis will be supported by Polaris in the future
    branches: [ "main" ]

  pull_request:
    branches: [ "main" ]

jobs:
  build:
    runs-on: ubuntu-latest
    permissions:
      contents: read
    steps:
      - name: Checkout
        uses: actions/checkout@v4
      - name: Synopsys Action
        uses: synopsys-sig/synopsys-action@v1.6.0
        with:
          #------------------------------------------COVERITY-----------------------------------------
          coverity_url: ${{ secrets.COVERITY_URL }}
          coverity_user: ${{ secrets.COVERITY_USER }}
          coverity_passphrase: ${{ secrets.COVERITY_PASSPHRASE }}

          #------------------------------------------BLACKDUCK----------------------------------------
          blackduck_token: ${{ secrets.BLACKDUCK_API_TOKEN }}
          blackduck_url: ${{ secrets.BLACKDUCK_URL }}

          #------------------------------------------POLARIS------------------------------------------
          polaris_server_url: ${{ secrets.POLARIS_SERVER_URL }}
          polaris_access_token: ${{ secrets.POLARIS_ACCESS_TOKEN }}
          polaris_assessment_types: "SCA,SAST"


