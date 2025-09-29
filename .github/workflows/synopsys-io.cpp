# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Synopsys Intelligent Security Scan

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
    print("Thanks for using BEAR!") --privileged tizenrt/tizenrt:#!usr/bin/env python
__author__    = "Westwood Robotics Corporation"
__email__     = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2025 Westwood Robotics Corporation"
__date__      = "July 29, 2025"
__project__   = "PyBEAR"
__version__   = "0.1.3"
__status__    = "Production"

'''
BEAR Control Table
'''

from collections import defaultdict


INSTRUCTION = defaultdict()
INSTRUCTION['PING']       = 0x01
INSTRUCTION['READ_STAT']  = 0x02
INSTRUCTION['WRITE_STAT'] = 0x03
INSTRUCTION['READ_CFG']   = 0x04
INSTRUCTION['WRITE_CFG']  = 0x05
INSTRUCTION['SAVE_CFG']   = 0x06
INSTRUCTION['SET_POSI']   = 0x08
INSTRUCTION['BULK_STAT']  = 0x12


REGISTER = defaultdict(lambda: defaultdict())

# Configuration
REGISTER['REG_TYPE']['id']                 = 'cfg'
REGISTER['REG_TYPE']['mode']               = 'cfg'
REGISTER['REG_TYPE']['baudrate']           = 'cfg'
REGISTER['REG_TYPE']['homing_offset']      = 'cfg'
REGISTER['REG_TYPE']['p_gain_id']          = 'cfg'
REGISTER['REG_TYPE']['i_gain_id']          = 'cfg'
REGISTER['REG_TYPE']['d_gain_id']          = 'cfg'
REGISTER['REG_TYPE']['p_gain_iq']          = 'cfg'
REGISTER['REG_TYPE']['i_gain_iq']          = 'cfg'
REGISTER['REG_TYPE']['d_gain_iq']          = 'cfg'
REGISTER['REG_TYPE']['p_gain_velocity']    = 'cfg'
REGISTER['REG_TYPE']['i_gain_velocity']    = 'cfg'
REGISTER['REG_TYPE']['d_gain_velocity']    = 'cfg'
REGISTER['REG_TYPE']['p_gain_position']    = 'cfg'
REGISTER['REG_TYPE']['i_gain_position']    = 'cfg'
REGISTER['REG_TYPE']['d_gain_position']    = 'cfg'
REGISTER['REG_TYPE']['p_gain_force']       = 'cfg'
REGISTER['REG_TYPE']['i_gain_force']       = 'cfg'
REGISTER['REG_TYPE']['d_gain_force']       = 'cfg'
REGISTER['REG_TYPE']['limit_acc_max']      = 'cfg'
REGISTER['REG_TYPE']['limit_i_max']        = 'cfg'
REGISTER['REG_TYPE']['limit_velocity_max'] = 'cfg'
REGISTER['REG_TYPE']['limit_position_min'] = 'cfg'
REGISTER['REG_TYPE']['limit_position_max'] = 'cfg'
REGISTER['REG_TYPE']['min_voltage']        = 'cfg'
REGISTER['REG_TYPE']['max_voltage']        = 'cfg'
REGISTER['REG_TYPE']['watchdog_timeout']   = 'cfg'
REGISTER['REG_TYPE']['temp_limit_low']     = 'cfg'
REGISTER['REG_TYPE']['temp_limit_high']    = 'cfg'

REGISTER['ADDRESS']['id']                 = 0x00
REGISTER['ADDRESS']['mode']               = 0x01
REGISTER['ADDRESS']['baudrate']           = 0x02
REGISTER['ADDRESS']['homing_offset']      = 0x03
REGISTER['ADDRESS']['p_gain_id']          = 0x04
REGISTER['ADDRESS']['i_gain_id']          = 0x05
REGISTER['ADDRESS']['d_gain_id']          = 0x06
REGISTER['ADDRESS']['p_gain_iq']          = 0x07
REGISTER['ADDRESS']['i_gain_iq']          = 0x08
REGISTER['ADDRESS']['d_gain_iq']          = 0x09
REGISTER['ADDRESS']['p_gain_velocity']    = 0x0A
REGISTER['ADDRESS']['i_gain_velocity']    = 0x0B
REGISTER['ADDRESS']['d_gain_velocity']    = 0x0C
REGISTER['ADDRESS']['p_gain_position']    = 0x0D
REGISTER['ADDRESS']['i_gain_position']    = 0x0E
REGISTER['ADDRESS']['d_gain_position']    = 0x0F
REGISTER['ADDRESS']['p_gain_force']       = 0x10
REGISTER['ADDRESS']['i_gain_force']       = 0x11
REGISTER['ADDRESS']['d_gain_force']       = 0x12
REGISTER['ADDRESS']['limit_acc_max']      = 0x13
REGISTER['ADDRESS']['limit_i_max']        = 0x14
REGISTER['ADDRESS']['limit_velocity_max'] = 0x15
REGISTER['ADDRESS']['limit_position_min'] = 0x16
REGISTER['ADDRESS']['limit_position_max'] = 0x17
REGISTER['ADDRESS']['min_voltage']        = 0x18
REGISTER['ADDRESS']['max_voltage']        = 0x19
REGISTER['ADDRESS']['watchdog_timeout']   = 0x1A
REGISTER['ADDRESS']['temp_limit_low']     = 0x1B
REGISTER['ADDRESS']['temp_limit_high']    = 0x1C

REGISTER['DATA_TYPE']['id']                 = 'u32'
REGISTER['DATA_TYPE']['mode']               = 'u32'
REGISTER['DATA_TYPE']['baudrate']           = 'u32'
REGISTER['DATA_TYPE']['homing_offset']      = 'f32'
REGISTER['DATA_TYPE']['p_gain_id']          = 'f32'
REGISTER['DATA_TYPE']['i_gain_id']          = 'f32'
REGISTER['DATA_TYPE']['d_gain_id']          = 'f32'
REGISTER['DATA_TYPE']['p_gain_iq']          = 'f32'
REGISTER['DATA_TYPE']['i_gain_iq']          = 'f32'
REGISTER['DATA_TYPE']['d_gain_iq']          = 'f32'
REGISTER['DATA_TYPE']['p_gain_velocity']    = 'f32'
REGISTER['DATA_TYPE']['i_gain_velocity']    = 'f32'
REGISTER['DATA_TYPE']['d_gain_velocity']    = 'f32'
REGISTER['DATA_TYPE']['p_gain_position']    = 'f32'
REGISTER['DATA_TYPE']['i_gain_position']    = 'f32'
REGISTER['DATA_TYPE']['d_gain_position']    = 'f32'
REGISTER['DATA_TYPE']['p_gain_force']       = 'f32'
REGISTER['DATA_TYPE']['i_gain_force']       = 'f32'
REGISTER['DATA_TYPE']['d_gain_force']       = 'f32'
REGISTER['DATA_TYPE']['limit_acc_max']      = 'f32'
REGISTER['DATA_TYPE']['limit_i_max']        = 'f32'
REGISTER['DATA_TYPE']['limit_velocity_max'] = 'f32'
REGISTER['DATA_TYPE']['limit_position_min'] = 'f32'
REGISTER['DATA_TYPE']['limit_position_max'] = 'f32'
REGISTER['DATA_TYPE']['min_voltage']        = 'f32'
REGISTER['DATA_TYPE']['max_voltage']        = 'f32'
REGISTER['DATA_TYPE']['watchdog_timeout']   = 'u32'
REGISTER['DATA_TYPE']['temp_limit_low']     = 'f32'
REGISTER['DATA_TYPE']['temp_limit_high']    = 'f32'

# Status
REGISTER['REG_TYPE']['torque_enable']          = 'stat'
REGISTER['REG_TYPE']['homing_complete']        = 'stat'
REGISTER['REG_TYPE']['goal_id']                = 'stat'
REGISTER['REG_TYPE']['goal_iq']                = 'stat'
REGISTER['REG_TYPE']['goal_velocity']          = 'stat'
REGISTER['REG_TYPE']['goal_position']          = 'stat'
REGISTER['REG_TYPE']['present_id']             = 'stat'
REGISTER['REG_TYPE']['present_iq']             = 'stat'
REGISTER['REG_TYPE']['present_velocity']       = 'stat'
REGISTER['REG_TYPE']['present_position']       = 'stat'
REGISTER['REG_TYPE']['input_voltage']          = 'stat'
REGISTER['REG_TYPE']['winding_temperature']    = 'stat'
REGISTER['REG_TYPE']['powerstage_temperature'] = 'stat'
REGISTER['REG_TYPE']['ic_temperature']         = 'stat'
REGISTER['REG_TYPE']['error_status']           = 'stat'
REGISTER['REG_TYPE']['warning_status']         = 'stat'

REGISTER['ADDRESS']['torque_enable']          = 0x00
REGISTER['ADDRESS']['homing_complete']        = 0x01
REGISTER['ADDRESS']['goal_id']                = 0x02
REGISTER['ADDRESS']['goal_iq']                = 0x03
REGISTER['ADDRESS']['goal_velocity']          = 0x04
REGISTER['ADDRESS']['goal_position']          = 0x05
REGISTER['ADDRESS']['present_id']             = 0x06
REGISTER['ADDRESS']['present_iq']             = 0x07
REGISTER['ADDRESS']['present_velocity']       = 0x08
REGISTER['ADDRESS']['present_position']       = 0x09
REGISTER['ADDRESS']['input_voltage']          = 0x0A
REGISTER['ADDRESS']['winding_temperature']    = 0x0B
REGISTER['ADDRESS']['powerstage_temperature'] = 0x0C
REGISTER['ADDRESS']['ic_temperature']         = 0x0D
REGISTER['ADDRESS']['error_status']           = 0x0E
REGISTER['ADDRESS']['warning_status']         = 0x0F

REGISTER['DATA_TYPE']['torque_enable']          = 'u32'
REGISTER['DATA_TYPE']['homing_complete']        = 'u32'
REGISTER['DATA_TYPE']['goal_id']                = 'f32'
REGISTER['DATA_TYPE']['goal_iq']                = 'f32'
REGISTER['DATA_TYPE']['goal_velocity']          = 'f32'
REGISTER['DATA_TYPE']['goal_position']          = 'f32'
REGISTER['DATA_TYPE']['present_id']             = 'f32'
REGISTER['DATA_TYPE']['present_iq']             = 'f32'
REGISTER['DATA_TYPE']['present_velocity']       = 'f32'
REGISTER['DATA_TYPE']['present_position']       = 'f32'
REGISTER['DATA_TYPE']['input_voltage']          = 'f32'
REGISTER['DATA_TYPE']['winding_temperature']    = 'f32'
REGISTER['DATA_TYPE']['powerstage_temperature'] = 'f32'
REGISTER['DATA_TYPE']['ic_temperature']         = 'f32'
REGISTER['DATA_TYPE']['error_status']           = 'u32'
REGISTER['DATA_TYPE']['warning_status']         = 'u32' /bin/bash
        docker cp ./. #!usr/bin/env python
__author__    = "Westwood Robotics Corporation"
__email__     = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2025 Westwood Robotics Corporation"
__date__      = "July 29, 2025"
__project__   = "PyBEAR"
__version__   = "0.1.3"
__status__    = "Production"

'''
BEAR Manager
'''

from pybear import Packet
from termcolor import cprint
from pybear.CONTROL_TABLE import *


class BEAR(Packet.PKT):
    def __init__(self, 
                 port='/dev/ttyUSB0', 
                 baudrate=8000000,
                 single_timeout=None, 
                 single_try_num=None, 
                 bulk_timeout=None, 
                 bulk_try_num=None):
        # serial communication
        self.port           = port
        self.baudrate       = baudrate
        self.single_timeout = single_timeout
        self.single_try_num = single_try_num
        self.bulk_timeout   = bulk_timeout
        self.bulk_try_num   = bulk_try_num
        super(BEAR, self).__init__(self.port, 
                                   self.baudrate, 
                                   single_timeout=self.single_timeout, 
                                   single_try_num=self.single_try_num, 
                                   bulk_timeout=self.bulk_timeout, 
                                   bulk_try_num=self.bulk_try_num)

        # BEAR info
        self.enable_status  = {'disable': 0, 'enable': 1, 'error': 2, 'damping': 3}
        self.operating_mode = {'torque': 0, 'velocity': 1, 'position': 2, 'force': 3}
        self.error_status   = {'Communication': 0, 'Overheat': 1, 'Absolute Position': 2, 'E-STOP': 3, 'Joint Limit': 4, 'Hardware Fault': 5, 'Initialization': 6}

        # startup
        self.print_welcome_msg()

    # =================================================================================================================
    # ===== Configuration Registers
    def get_id(self, *argv):
        return [self.single_read(data, ['id']) for data in argv]
    
    def set_id(self, *argv):
        for data in argv:
            self.single_write(data[0], ['id'], [data[1]])

    def get_mode(self, *argv):
        return [self.single_read(data, ['mode']) for data in argv]
    
    def set_mode(self, *argv):
        for data in argv:
            self.single_write(data[0], ['mode'], [data[1]])

    def get_baudrate(self, *argv):
        return [self.single_read(data, ['baudrate']) for data in argv]
    
    def set_baudrate(self, *argv):
        for data in argv:
            self.single_write(data[0], ['baudrate'], [data[1]])

    def get_homing_offset(self, *argv):
        return [self.single_read(data, ['homing_offset']) for data in argv]
    
    def set_homing_offset(self, *argv):
        for data in argv:
            self.single_write(data[0], ['homing_offset'], [data[1]])

    def get_p_gain_id(self, *argv):
        return [self.single_read(data, ['p_gain_id']) for data in argv]
    
    def set_p_gain_id(self, *argv):
        for data in argv:
            self.single_write(data[0], ['p_gain_id'], [data[1]])
    
    def get_i_gain_id(self, *argv):
        return [self.single_read(data, ['i_gain_id']) for data in argv]
    
    def set_i_gain_id(self, *argv):
        for data in argv:
            self.single_write(data[0], ['i_gain_id'], [data[1]])

    def get_d_gain_id(self, *argv):
        return [self.single_read(data, ['d_gain_id']) for data in argv]
    
    def set_d_gain_id(self, *argv):
        for data in argv:
            self.single_write(data[0], ['d_gain_id'], [data[1]])

    def get_p_gain_iq(self, *argv):
        return [self.single_read(data, ['p_gain_iq']) for data in argv]
    
    def set_p_gain_iq(self, *argv):
        for data in argv:
            self.single_write(data[0], ['p_gain_iq'], [data[1]])
    
    def get_i_gain_iq(self, *argv):
        return [self.single_read(data, ['i_gain_iq']) for data in argv]
    
    def set_i_gain_iq(self, *argv):
        for data in argv:
            self.single_write(data[0], ['i_gain_iq'], [data[1]])

    def get_d_gain_iq(self, *argv):
        return [self.single_read(data, ['d_gain_iq']) for data in argv]
    
    def set_d_gain_iq(self, *argv):
        for data in argv:
            self.single_write(data[0], ['d_gain_iq'], [data[1]])

    def get_p_gain_velocity(self, *argv):
        return [self.single_read(data, ['p_gain_velocity']) for data in argv]
    
    def set_p_gain_velocity(self, *argv):
        for data in argv:
            self.single_write(data[0], ['p_gain_velocity'], [data[1]])
    
    def get_i_gain_velocity(self, *argv):
        return [self.single_read(data, ['i_gain_velocity']) for data in argv]
    
    def set_i_gain_velocity(self, *argv):
        for data in argv:
            self.single_write(data[0], ['i_gain_velocity'], [data[1]])

    def get_d_gain_velocity(self, *argv):
        return [self.single_read(data, ['d_gain_velocity']) for data in argv]
    
    def set_d_gain_velocity(self, *argv):
        for data in argv:
            self.single_write(data[0], ['d_gain_velocity'], [data[1]])

    def get_p_gain_position(self, *argv):
        return [self.single_read(data, ['p_gain_position']) for data in argv]
    
    def set_p_gain_position(self, *argv):
        for data in argv:
            self.single_write(data[0], ['p_gain_position'], [data[1]])
    
    def get_i_gain_position(self, *argv):
        return [self.single_read(data, ['i_gain_position']) for data in argv]
    
    def set_i_gain_position(self, *argv):
        for data in argv:
            self.single_write(data[0], ['i_gain_position'], [data[1]])

    def get_d_gain_position(self, *argv):
        return [self.single_read(data, ['d_gain_position']) for data in argv]
    
    def set_d_gain_position(self, *argv):
        for data in argv:
            self.single_write(data[0], ['d_gain_position'], [data[1]])

    def get_p_gain_force(self, *argv):
        return [self.single_read(data, ['p_gain_force']) for data in argv]
    
    def set_p_gain_force(self, *argv):
        for data in argv:
            self.single_write(data[0], ['p_gain_force'], [data[1]])
    
    def get_i_gain_force(self, *argv):
        return [self.single_read(data, ['i_gain_force']) for data in argv]
    
    def set_i_gain_force(self, *argv):
        for data in argv:
            self.single_write(data[0], ['i_gain_force'], [data[1]])

    def get_d_gain_force(self, *argv):
        return [self.single_read(data, ['d_gain_force']) for data in argv]
    
    def set_d_gain_force(self, *argv):
        for data in argv:
            self.single_write(data[0], ['d_gain_force'], [data[1]])

    def get_limit_acc_max(self, *argv):
        return [self.single_read(data, ['limit_acc_max']) for data in argv]
    
    def set_limit_acc_max(self, *argv):
        for data in argv:
            self.single_write(data[0], ['limit_acc_max'], [data[1]])

    def get_limit_i_max(self, *argv):
        return [self.single_read(data, ['limit_i_max']) for data in argv]
    
    def set_limit_i_max(self, *argv):
        for data in argv:
            self.single_write(data[0], ['limit_i_max'], [data[1]])
    
    def get_limit_velocity_max(self, *argv):
        return [self.single_read(data, ['limit_velocity_max']) for data in argv]
    
    def set_limit_velocity_max(self, *argv):
        for data in argv:
            self.single_write(data[0], ['limit_velocity_max'], [data[1]])

    def get_limit_position_min(self, *argv):
        return [self.single_read(data, ['limit_position_min']) for data in argv]
    
    def set_limit_position_min(self, *argv):
        for data in argv:
            self.single_write(data[0], ['limit_position_min'], [data[1]])

    def get_limit_position_max(self, *argv):
        return [self.single_read(data, ['limit_position_max']) for data in argv]
    
    def set_limit_position_max(self, *argv):
        for data in argv:
            self.single_write(data[0], ['limit_position_max'], [data[1]])

    def get_min_voltage(self, *argv):
        return [self.single_read(data, ['min_voltage']) for data in argv]
    
    def set_min_voltage(self, *argv):
        for data in argv:
            self.single_write(data[0], ['min_voltage'], [data[1]])

    def get_max_voltage(self, *argv):
        return [self.single_read(data, ['max_voltage']) for data in argv]
    
    def set_max_voltage(self, *argv):
        for data in argv:
            self.single_write(data[0], ['max_voltage'], [data[1]])

    def get_watchdog_timeout(self, *argv):
        return [self.single_read(data, ['watchdog_timeout']) for data in argv]
    
    def set_watchdog_timeout(self, *argv):
        for data in argv:
            self.single_write(data[0], ['watchdog_timeout'], [data[1]])
    
    def get_temp_limit_low(self, *argv):
        return [self.single_read(data, ['temp_limit_low']) for data in argv]
    
    def set_temp_limit_low(self, *argv):
        for data in argv:
            self.single_write(data[0], ['temp_limit_low'], [data[1]])
    
    def get_temp_limit_high(self, *argv):
        return [self.single_read(data, ['temp_limit_high']) for data in argv]
    
    def set_temp_limit_high(self, *argv):
        for data in argv:
            self.single_write(data[0], ['temp_limit_high'], [data[1]])
        
    # =================================================================================================================
    # ===== Status Registers
    def get_torque_enable(self, *argv):
        return [self.single_read(data, ['torque_enable']) for data in argv]
    
    def set_torque_enable(self, *argv):
        for data in argv:
            self.single_write(data[0], ['torque_enable'], [data[1]])
    
    def get_goal_id(self, *argv):
        return [self.single_read(data, ['goal_id']) for data in argv]
    
    def set_goal_id(self, *argv):
        for data in argv:
            self.single_write(data[0], ['goal_id'], [data[1]])

    def get_goal_iq(self, *argv):
        return [self.single_read(data, ['goal_iq']) for data in argv]
    
    def set_goal_iq(self, *argv):
        for data in argv:
            self.single_write(data[0], ['goal_iq'], [data[1]])

    def get_goal_velocity(self, *argv):
        return [self.single_read(data, ['goal_velocity']) for data in argv]
    
    def set_goal_velocity(self, *argv):
        for data in argv:
            self.single_write(data[0], ['goal_velocity'], [data[1]])

    def get_goal_position(self, *argv):
        return [self.single_read(data, ['goal_position']) for data in argv]
    
    def set_goal_position(self, *argv):
        for data in argv:
            self.single_write(data[0], ['goal_position'], [data[1]])
    
    def get_present_id(self, *argv):
        return [self.single_read(data, ['present_id']) for data in argv]

    def get_present_iq(self, *argv):
        return [self.single_read(data, ['present_iq']) for data in argv]

    def get_present_velocity(self, *argv):
        return [self.single_read(data, ['present_velocity']) for data in argv]

    def get_present_position(self, *argv):
        return [self.single_read(data, ['present_position']) for data in argv]

    def get_input_voltage(self, *argv):
        return [self.single_read(data, ['input_voltage']) for data in argv]

    def get_winding_temperature(self, *argv):
        return [self.single_read(data, ['winding_temperature']) for data in argv]

    def get_powerstage_temperature(self, *argv):
        return [self.single_read(data, ['powerstage_temperature']) for data in argv]

    def get_ic_temperature(self, *argv):
        return [self.single_read(data, ['ic_temperature']) for data in argv]
    
    # =================================================================================================================
    # ===== Others
    def print_welcome_msg(self):
        print("==============================================================")
        print("   __        __          _                               _    ")
        print("   \ \      / /___  ___ | |_ __      __ ___    ___    __| |   ")
        print("    \ \ /\ / // _ \/ __|| __|\ \ /\ / // _ \  / _ \  / _` |   ")
        print("     \ V  V /|  __/\__ \| |_  \ V  V /| (_) || (_) || (_| |   ")
        print("      \_/\_/  \___||___/ \__|  \_/\_/  \___/  \___/  \__,_|   ")
        print("              ____         _             _    _               ")
        print("             |  _ \  ___  | |__    ___  | |_ (_)  ___  ___    ")
        print("             | |_) |/ _ \ | '_ \  / _ \ | __|| | / __|/ __|   ")
        print("             |  _ <| (_) || |_) || (_) || |_ | || (__ \__ \   ")
        print("             |_| \_\\___/ |_.__/  \___/  \__||_| \___||___/   ")
        print("                                                              ")
        print("==============================================================")
        print("=== PyBEAR by Westwood Robotics -- Last Updated 2025.07.29 ===")
        print("==============================================================")
        print()
    
    def ping(self, *argv: int):
        """
        Function used to detect BEAR(s). Will return firmware and hardware version info, e.g., ping(id1, id2)
        Return None if pinging unsuccessful.
        """
        return [self._ping(data) for data in argv]
    
    def save_config(self, *argv: int):
        """
        Save BEAR configuration registers, e.g., save_config(id1, id2)
        """
        for data in argv:
            self._save_config(data)

    def set_posi(self, *argv):
        """
        Set BEAR absolute position, e.g., set_posi((id1, pos1, tol1), (id2, pos2, tol2))
        """
        for data in argv:
            self._set_posi(data[0], data[1], data[2])

    def single_read(self, bear, reg_name_list):
        """
        Read multiple registers of a single BEAR, e.g., single_read(id, [name1, name2])
        return: list of register data, e.g., [data1, data2], error
        """
        cfg_name_list = []
        stat_name_list = []
        for idx, reg_name in enumerate(reg_name_list):
            if REGISTER['REG_TYPE'][reg_name] == 'cfg':
                cfg_name_list.append(reg_name)
            elif REGISTER['REG_TYPE'][reg_name] == 'stat':
                stat_name_list.append(reg_name)

        if cfg_name_list:
            cfg_data, error = self._single_read(bear, cfg_name_list, reg_type='cfg')

        if stat_name_list:
            stat_data, error = self._single_read(bear, stat_name_list, reg_type='stat')

        reg_data = []
        for reg_name in reg_name_list:
            if REGISTER['REG_TYPE'][reg_name] == 'cfg':
                reg_data.append(cfg_data[0])
                cfg_data.pop(0)
            elif REGISTER['REG_TYPE'][reg_name] == 'stat':
                reg_data.append(stat_data[0])
                stat_data.pop(0)
        return reg_data, error

    def single_write(self, bear, reg_name_list, data_list):
        """
        Write multiple registers of a single BEAR, e.g., single_write(id, [name1, name2], [data1, data2])
        """
        cfg_name_list = []
        cfg_data_list = []
        stat_name_list = []
        stat_data_list = []
        for idx, reg_name in enumerate(reg_name_list):
            data = data_list[idx]
            if REGISTER['REG_TYPE'][reg_name] == 'cfg':
                cfg_name_list.append(reg_name)
                cfg_data_list.append(data)
            elif REGISTER['REG_TYPE'][reg_name] == 'stat':
                stat_name_list.append(reg_name)
                stat_data_list.append(data)

        if cfg_name_list:
            self._single_write(bear, cfg_name_list, cfg_data_list, reg_type='cfg')

        if stat_data_list:
            self._single_write(bear, stat_name_list, stat_data_list, reg_type='stat')

    def bulk_read(self, bear_list, read_stat_name_list):
        """
        Read multiple *STATUS* registers of multiple BEARs, e.g., bulk_read([id1, id2], [read_name1, read_name2])
        return: list of status data, e.g., [([read_data11, read_data12], error1), ([read_data21, read_data22], error2)]
        """
        read_stat_data_list, error_list = self._bulk_read_write_stat(bear_list, read_stat_name_list, [], [])
        return [(read_stat_data_list[idx], error_list[idx]) for idx in bear_list]

    def bulk_write(self, bear_list, write_stat_name_list, data_list):
        """
        Write multiple *STATUS* registers of multiple BEARs, e.g., bulk_write([id1, id2], [write_name1, write_name2], [[write_data11, write_data12], [write_data21, write_data22]])
        """
        self._bulk_read_write_stat(bear_list, [], write_stat_name_list, data_list)

    def bulk_read_write(self, bear_list, read_stat_name_list, write_stat_name_list, write_stat_data_list):
        """
        Read and write multiple *STATUS* registers of multiple BEARs, e.g., bulk_read_write([id1, id2], [read_name1, read_name2], [write_name1, write_name2], [[write_data11, write_data12], [write_data21, write_data22]])
        return: list of status data, e.g., [([read_data11, read_data12], error1), ([read_data21, read_data22], error2)]
        """
        read_stat_data_list, error_list = self._bulk_read_write_stat(bear_list, read_stat_name_list, write_stat_name_list, write_stat_data_list)
        return [(read_stat_data_list[idx], error_list[idx]) for idx in bear_list]
    
    def get_register(self, *argv):
        """
        Read multiple registers from multiple BEARs in one packet but goes through them one-by-one, e.g. get_reg((id1, reg11, reg12, ...), (id2, reg21, reg22, ...), ...)
        """
        return [self.single_read(data[0], [data[1:]]) for data in argv]
    
    def set_register(self, *argv):
        """
        Write to multiple registers on multiple BEARs in one packet but goes through them one-by-one, e.g., set_reg((id1, reg11, data11, reg12, data12, ...), (id2, reg21, data21, reg22, data22, ...), ...)
        """
        for data in argv:
            self.single_write(data[0], [data[1::2]], [data[2::2]])

    def decode_error(self, error_code):
        """
        Decode BEAR error code, e.g., decode_error(error_code)
        return: error names
        """
        msg = ''
        if error_code >> 7 != 1:
            cprint("Invalid Error Code!!!", 'red')
            return msg
        else:
            error_num = 0
            for idx in range(7):
                if error_code >> idx & 1:
                    error_num += 1
                    if error_num > 1:
                        msg += ' & '
                    msg += list(self.error_status.keys())[list(self.error_status.values()).index(idx)]
            if error_num:
                return msg
            else:
                return "No Error!":/#!usr/bin/env python
__author__    = "Westwood Robotics Corporation"
__email__     = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2025 Westwood Robotics Corporation"
__date__      = "July 29, 2025"
__project__   = "PyBEAR"
__version__   = "0.1.3"
__status__    = "Production"

'''
BEAR Serial Communication
'''

import time
import serial
import struct
from termcolor import cprint
from pybear.CONTROL_TABLE import *


class PKT(object):
    def __init__(self, port, baudrate, single_timeout=None, single_try_num=None, bulk_timeout=None, bulk_try_num=None):
        # serial communication
        self.port = port
        self.baudrate = baudrate
        self.open_port()

        # timeout/number of retries for single/multiple BEARs communication 
        self.single_timeout = 0.002 if single_timeout is None else single_timeout
        self.single_try_num = 2     if single_try_num is None else single_try_num
        self.bulk_timeout   = 0.002 if bulk_timeout   is None else bulk_timeout
        self.bulk_try_num   = 2     if bulk_try_num   is None else bulk_try_num

    def open_port(self):
        """
        Open the serial port
        """
        self.ser = serial.Serial(self.port,
                                 self.baudrate,
                                 bytesize=serial.EIGHTBITS,
                                 parity=serial.PARITY_NONE,
                                 stopbits=serial.STOPBITS_ONE,
                                 timeout=0)

    def close_port(self):
        """
        Close the serial port
        """
        if self.ser:
            self.ser.close()

    def send_packet(self, packet):
        """
        Send packet to the serial port
        """
        self.ser.reset_input_buffer()
        self.ser.write(bytearray(packet))

    def single_read_packet(self, instruction_packet):
        """
        Single read packet from the serial port (only one BEAR each time)
        Instruction: [0xFF, 0xFF, ID, Length, Instruction, P_1, P_2, ..., P_N, Checksum]
                     Length = # of rest bytes (i.e., from Instruction to Checksum)
                        P_i = Read_Address_i or [Write_Address_i, Write_Data_i (4-byte)]
             Return: [0xFF, 0xFF, ID, Length, Error, P_1, P_2, ..., P_N, Checksum]
                     Length = # of rest bytes (i.e., from Error to Checksum)
                        P_i = Read_Data_i (4-byte)
        """
        # send the instruction packet first
        self.send_packet(instruction_packet)

        # now waiting for response
        t0 = time.time()
        c0 = 1  # try count
        while self.ser.in_waiting < 4:                  # so at least we got the packet length info
            if time.time() - t0 > self.single_timeout:  # if timeout
                if c0 >= self.single_try_num:           # if max try number
                    self.print_warning(warning='max_try_num')
                    return None, None
                self.print_warning(warning='single_timeout')
                self.send_packet(instruction_packet)    # resend the instruction packet
                t0 = time.time()
                c0 += 1

        # ok we have something in the buffer
        return_packet = []
        return_packet.extend(self.ser.read(4))
        rest_length = return_packet[3]
        t0 = time.time()
        while self.ser.in_waiting < rest_length:          # make sure the packet is complete
            if time.time() - t0 > self.single_timeout:    # if timeout here, packet is incomplete
                self.print_warning(warning='bad_data')
                return None, None
        return_packet.extend(self.ser.read(rest_length))  # get the whole packet

        # check return packet
        if self.checksum(return_packet) == return_packet[-1]:
            error_code  = return_packet[4]
            data_packet = return_packet[5:-1]
            return data_packet, error_code
        else:
            self.print_warning(warning='bad_data')
            return None, None

    def bulk_read_packet(self, instruction_packet, bear_num):
        """
        Bulk read packet from the serial port (multiple BEARs each time)
        Instruction: [0xFF, 0xFF, 0xFE, Length, 0x08, IDs, R/Ws, R_1, R_2, ..., R_L, W_1, W_2, ..., W_N, P_1, P_2, ..., P_M, Checksum]
                     Length = # of rest bytes (i.e., from 0x08 to Checksum)
                        IDs = # of BEARs, i.e., M
                       R/Ws = # of Read/Write Addresses, i.e., 0xLN
                        R_i = Read_Address_i, i = 1, ..., L
                        W_i = Write_Address_i, i = 1, ..., N
                        P_i = ID_i or [ID_i, Write_Data_1, Write_Data_2, ..., Write_Data_N (4-byte)], i = 1, ..., M
             Return: [0xFF, 0xFF, ID 1, Length, Error 1, P_11, P_12, ..., P_1L, Checksum 1,
                      0xFF, 0xFF, ID 2, Length, Error 2, P_21, P_22, ..., P_2L, Checksum 2,
                      ...
                      0xFF, 0xFF, ID M, Length, Error M, P_M1, P_M2, ..., P_ML, Checksum M]
                     Length = # of rest bytes for each component packet (i.e., from Error to Checksum)
                       P_ij = Read_Data_ij (4-byte), i = 1, ..., M, j = 1, ..., L
        """
        # send the instruction packet first
        self.send_packet(instruction_packet)

        # now waiting for response
        t0 = time.time()
        c0 = 1  # try count
        while self.ser.in_waiting < 4:                # so at least we got the component packet length info
            if time.time() - t0 > self.bulk_timeout:  # if timeout
                if c0 == self.single_try_num:         # if max try number
                    self.print_warning(warning='max_try_num')
                    return None, None
                self.print_warning(warning='bulk_timeout')
                self.send_packet(instruction_packet)  # resend the instruction packet
                t0 = time.time()
                c0 += 1

        # ok we have something in the buffer
        return_packet = []
        return_packet.extend(self.ser.read(4))
        component_packet_length = return_packet[3] + 4
        rest_length = component_packet_length * bear_num - 4
        t0 = time.time()
        while self.ser.in_waiting < rest_length:          # make sure the packet is complete
            if time.time() - t0 > self.bulk_timeout:      # if timeout here, packet is incomplete
                self.print_warning(warning='bad_data')
                return None, None
        return_packet.extend(self.ser.read(rest_length))  # get the whole packet

        data_packet = []
        error_code = []
        for idx in range(bear_num):
            id0 = idx * component_packet_length
            id1 = id0 + 4
            id2 = id1 + 1
            id4 = id0 + component_packet_length
            id3 = id4 - 1
            if self.checksum(return_packet[id0:id4]) == return_packet[id3]:
                error_code.append(return_packet[id1])
                data_packet.append(return_packet[id2:id3])
            else:
                error_code.append(None)
                data_packet.append(None)
        return data_packet, error_code

    def _ping(self, m_id):
        """
        Ping a single BEAR
        """
        instruction_packet      = [0xFF, 0xFF, m_id, 2, INSTRUCTION['PING'], 0]
        instruction_packet[-1]  = self.checksum(instruction_packet)
        data_packet, error_code = self.single_read_packet(instruction_packet)
        data = self.hex_to_int32(data_packet) if data_packet is not None else None
        return [data], error_code

    def _save_config(self, m_id):
        """
        Save configuration registers of a single BEAR
        """
        instruction_packet     = [0xFF, 0xFF, m_id, 2, INSTRUCTION['SAVE_CFG'], 0]
        instruction_packet[-1] = self.checksum(instruction_packet)
        self.send_packet(instruction_packet)

    def _set_posi(self, m_id, position, tolerance):
        """
        Set absolute position of a single BEAR
        """
        instruction_packet     = [0xFF, 0xFF, m_id, 10, INSTRUCTION['SET_POSI']] + self.float32_to_hex(position) + self.float32_to_hex(tolerance) + [0]
        instruction_packet[-1] = self.checksum(instruction_packet)
        self.send_packet(instruction_packet)

    def _single_read(self, m_id, reg_name_list, reg_type=None):
        """
        Read multiple registers (of same register type) of a single BEAR
        reg_name_list: list of register names, e.g., [name1, name2]
        """
        if reg_type == 'cfg':
            instruction = INSTRUCTION['READ_CFG']
        elif reg_type == 'stat':
            instruction = INSTRUCTION['READ_STAT']

        reg_num = len(reg_name_list)
        reg_add_list = []
        for reg_name in reg_name_list:
            reg_add_list.append(REGISTER['ADDRESS'][reg_name])
        instruction_packet     = [0xFF, 0xFF, m_id, reg_num + 2, instruction] + reg_add_list + [0]
        instruction_packet[-1] = self.checksum(instruction_packet)

        data_packet, error_code = self.single_read_packet(instruction_packet)
        if data_packet is not None:
            data = []
            for idx, name in enumerate(reg_name_list):
                id0 = idx * 4
                id1 = id0 + 4
                if REGISTER['DATA_TYPE'][name] == 'f32':
                    data.append(self.hex_to_float32(data_packet[id0:id1]))
                elif REGISTER['DATA_TYPE'][name] == 'u32':
                    data.append(self.hex_to_int32(data_packet[id0:id1]))
            return data, error_code
        else:
            return [None] * reg_num, None

    def _single_write(self, m_id, reg_name_list, data_list, reg_type=None):
        """
        Write multiple registers (of same register type) of a single BEAR
        e.g., _single_write(id, [name1, name2], [data1, data2], reg_type)
        """
        if reg_type == 'cfg':
            instruction = INSTRUCTION['WRITE_CFG']
        elif reg_type == 'stat':
            instruction = INSTRUCTION['WRITE_STAT']

        reg_num = len(reg_name_list)
        reg_add_and_data_list = []
        for idx, reg_name in enumerate(reg_name_list):
            reg_add_and_data_list.append(REGISTER['ADDRESS'][reg_name])
            if REGISTER['DATA_TYPE'][reg_name] == 'f32':
                reg_add_and_data_list += self.float32_to_hex(data_list[idx])
            elif REGISTER['DATA_TYPE'][reg_name] == 'u32':
                reg_add_and_data_list += self.int32_to_hex(data_list[idx])
        instruction_packet     = [0xFF, 0xFF, m_id, reg_num * 5 + 2, instruction] + reg_add_and_data_list + [0]
        instruction_packet[-1] = self.checksum(instruction_packet)
        self.send_packet(instruction_packet)
        
    def _bulk_read_write_stat(self, m_ids, read_stat_name_list, write_stat_name_list, data_list):
        """
        Read and write multiple status registers of multiple BEARs
        e.g., _bulk_read_write([id1, id2], [read_name1, read_name2], [write_name1, write_name2], [[data11, data12], [data21, data22]])
        """
        instruction = INSTRUCTION['BULK_STAT']

        m_num = len(m_ids)
        read_stat_num = len(read_stat_name_list)
        write_stat_num = len(write_stat_name_list)
        total_stat_num = write_stat_num | read_stat_num << 4

        read_stat_add_list = []
        if read_stat_num:
            for read_stat_name in read_stat_name_list:
                read_stat_add_list.append(REGISTER['ADDRESS'][read_stat_name])

        write_stat_add_list = []
        if write_stat_num:
            for write_stat_name in write_stat_name_list:
                write_stat_add_list.append(REGISTER['ADDRESS'][write_stat_name])

            m_id_and_data_list = []
            for idx in range(m_num):
                m_id_and_data_list.append(m_ids[idx])
                for jdx, write_stat_name in enumerate(write_stat_name_list):
                    if REGISTER['DATA_TYPE'][write_stat_name] == 'f32':
                        m_id_and_data_list += self.float32_to_hex(data_list[idx][jdx])
                    elif REGISTER['DATA_TYPE'][write_stat_name] == 'u32':
                        m_id_and_data_list += self.int32_to_hex(data_list[idx][jdx])
        else:
            m_id_and_data_list = m_ids

        length = m_num + read_stat_num + write_stat_num + write_stat_num * m_num * 4 + 4
        instruction_packet = [0xFF, 0xFF, 0xFE, length, instruction, m_num, total_stat_num] + read_stat_add_list + write_stat_add_list + m_id_and_data_list + [0]
        instruction_packet[-1] = self.checksum(instruction_packet)

        if read_stat_num:
            data_packet, error_code = self.bulk_read_packet(instruction_packet, m_num)
            if data_packet is not None:
                data = []
                for idx in range(m_num):
                    data_i = []
                    if data_packet[idx] is not None:
                        for jdx, read_stat_name in enumerate(read_stat_name_list):
                            jd0 = jdx * 4
                            jd1 = jd0 + 4
                            if REGISTER['DATA_TYPE'][read_stat_name] == 'f32':
                                data_i.append(self.hex_to_float32(data_packet[idx][jd0:jd1]))
                            elif REGISTER['DATA_TYPE'][read_stat_name] == 'u32':
                                data_i.append(self.hex_to_int32(data_packet[idx][jd0:jd1]))
                        data.append(data_i)
                    else:
                        data.append([None] * read_stat_num)
                return data, error_code
            else:
                return [[None] * read_stat_num] * m_num, [None] * m_num
        else:
            self.send_packet(instruction_packet)

    @staticmethod
    def checksum(packet):
        return 255 - sum(packet[2:-1]) % 256

    @staticmethod
    def hex_to_float32(byte):
        return struct.unpack('f', bytearray(byte))[0]

    @staticmethod
    def float32_to_hex(data):
        byte = struct.pack('f', data)
        return [byte[0], byte[1], byte[2], byte[3]]

    @staticmethod
    def hex_to_int32(byte):
        return byte[0] | byte[1] << 8 | byte[2] << 16 | byte[3] << 24

    @staticmethod
    def int32_to_hex(data):
        byte = struct.pack('i', data)
        return [byte[0], byte[1], byte[2], byte[3]]

    @staticmethod
    def print_warning(warning):
        msg = "[PyBEAR | WARNING] :: "
        if warning == 'single_timeout':
            msg += "Single read timed out. Now try again ..."
            cprint(msg, 'yellow')
        elif warning == 'bulk_timeout':
            msg += "Bulk read timed out. Now try again ..."
            cprint(msg, 'yellow')
        elif warning == 'max_try_num':
            msg += "Maximum try number reached. Just move on."
            cprint(msg, 'red')
        elif warning == 'bad_data':
            msg += "Data corrupted. Just move on."
            cprint(msg, 'red')
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
    branches: [ "main" ]
  pull_request:
    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule:
    - cron: '21 3 * * 1'

jobs:
  analyze:
    name: Analyze
    runs-on: ubuntu-latest
    permissions:
      actions: read
      contents: read
      security-events: write

    steps:
    - name: Checkout repository
      uses: actions/checkout@v4

    - name: Synopsys Intelligent Security Scan
      id: prescription
      uses: synopsys-sig/intelligent-security-scan@48eedfcd42bc342a294dc495ac452797b2d9ff08
      with:
        ioServerUrl: ${{secrets.IO_SERVER_URL}}
        ioServerToken: ${{secrets.IO_SERVER_TOKEN}}
        workflowServerUrl: ${{secrets.WORKFLOW_SERVER_URL}}
        additionalWorkflowArgs: --polaris.url=${{secrets.POLARIS_SERVER_URL}} --polaris.token=${{secrets.POLARIS_ACCESS_TOKEN}}
        stage: "IO"

    # Please note that the ID in previous step was set to prescription
    # in order for this logic to work also make sure that POLARIS_ACCESS_TOKEN
    # is defined in settings
    - name: Static Analysis with Polaris
      if: ${{steps.prescription.outputs.sastScan == 'true' }}
      run: |
          export POLARIS_SERVER_URL=${{ secrets.POLARIS_SERVER_URL}}
          export POLARIS_ACCESS_TOKEN=${{ secrets.POLARIS_ACCESS_TOKEN}}
          wget -q ${{ secrets.POLARIS_SERVER_URL}}/api/tools/polaris_cli-linux64.zip
          unzip -j polaris_cli-linux64.zip -d /tmp
          /tmp/polaris analyze -w

    # Please note that the ID in previous step was set to prescription
    # in order for this logic to work
    - name: Software Composition Analysis with Black Duck
      if: ${{steps.prescription.outputs.scaScan == 'true' }}
      uses: blackducksoftware/github-action@9ea442b34409737f64743781e9adc71fd8e17d38
      with:
         args: '--blackduck.url="${{ secrets.BLACKDUCK_URL}}" --blackduck.api.token="${{ secrets.BLACKDUCK_TOKEN}}" --detect.tools="SIGNATURE_SCAN,DETECTOR"'

    - name: Synopsys Intelligent Security Scan
      if: ${{ steps.prescription.outputs.sastScan == 'true' || steps.prescription.outputs.scaScan == 'true' }}
      uses: synopsys-sig/intelligent-security-scan@48eedfcd42bc342a294dc495ac452797b2d9ff08
      with:
        ioServerUrl: ${{secrets.IO_SERVER_URL}}
        ioServerToken: ${{secrets.IO_SERVER_TOKEN}}
        workflowServerUrl: ${{secrets.WORKFLOW_SERVER_URL}}
        additionalWorkflowArgs: --IS_SAST_ENABLED=${{steps.prescription.outputs.sastScan}} --IS_SCA_ENABLED=${{steps.prescription.outputs.scaScan}}
                --polaris.project.name={{PROJECT_NAME}} --polaris.url=${{secrets.POLARIS_SERVER_URL}} --polaris.token=${{secrets.POLARIS_ACCESS_TOKEN}}
                --blackduck.project.name={{PROJECT_NAME}}:{{PROJECT_VERSION}} --blackduck.url=${{secrets.BLACKDUCK_URL}} --blackduck.api.token=${{secrets.BLACKDUCK_TOKEN}}
        stage: "WORKFLOW"

    - name: Upload SARIF file
      if: ${{steps.prescription.outputs.sastScan == 'true' }}
      uses: github/codeql-action/upload-sarif@v3
      with:
        # Path to SARIF file relative to the root of the repository
        sarif_file: workflowengine-results.sarif.json
