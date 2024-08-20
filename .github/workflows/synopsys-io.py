# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Synopsys Intelligent Security Scan

on: #!usr/bin/env python
__author__ = ""
__email__ = "@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Jan 8, 2020"
__version__ = "0.1.2"
__status__ = "Production"

# -----------------------------
# Simple code to set ID for BEAR

from pybear import Manager

bear = Manager.BEAR(port="COM7", baudrate=8000000)
m_id = int(input("Enter the present ID and press enter.\n"))
print("Present ID entered is: %d" % m_id)
if bear.ping(m_id):
    print("BEAR connected.")
    m_id_new = int(input("Enter the new ID and press enter.\n"))
    bear.set_id((m_id, m_id_new))
    bear.save_config(m_id_new)
    if bear.ping(m_id_new):
        print("BEAR ID has been changed from %d to %d" % (m_id, m_id_new))
    else:
        print("BEAR ID change unsuccessful. Please try again.")
else:
    print("Seems like that BEAR is offline, please double check your entry and connections.")
  push: #!usr/bin/env python
__author__ = ""
__email__ = "@westwoodrobotics.io"
__copyright__ = "Copyright 2022 Westwood Robotics"
__date__ = "July 18, 2022"
__version__ = "0.0.1"
__status__ = "Prototype"

# -----------------------------
# Search and list all motors in the chain
# Prompt to change ID for each motor

# Ping and search for available BEARs
from pybear import Manager

# Define port and baud rate
bear_port = "COM11"
bear_baudrate = 8000000
# Define ID search range
id_range = range(0, 253)

bear = Manager.BEAR(port=bear_port, baudrate=bear_baudrate)
bear_list = []
found = False
for i in id_range:
    m_id = i
    data = bear.ping(m_id)[0]
    if data:
        found = True
        bear_list.append(m_id)
if found:
    print("Search done. Total of %d BEARs found. And their IDs are:" % len(bear_list))
    print(bear_list)
else:
    print("Search done. No BEAR found.")
    exit()

# Change ID if found BEAR(s).
if found:
    print("Changing ID...")
    for idx, m_id in enumerate(bear_list):
        print("Current object: %d of %d" % (idx+1, len(bear_list)))
        change_in_progress = True
        while change_in_progress:
            usr = input("Present ID: %d, enter new id or 'N/n' to skip:" % m_id)
            if usr in ['n', 'N']:
                change_in_progress = False
            else:
                try:
                    usr = int(usr)
                    if -1 < usr < 254:
                        if usr not in bear_list:
                            bear.set_id((m_id, usr))
                            bear.save_config(usr)
                            if bear.ping(usr):
                                print("Present BEAR's ID has been changed from %d to %d" % (m_id, usr))
                                change_in_progress = False
                            else:
                                print("BEAR ID change unsuccessful. Please debug.")
                                exit()
                        else:
                            print("Entered ID already exists in chain. Please try again.")
                    else:
                        print("Invalid entrance. Only integers from 0 to 253 accepted.")
                except:
                    print("Invalid entrance. Only integers from 0 to 253 accepted.")
    print("All done.")
    branches: [ "main" ]
  pull_request: #!usr/bin/env python
__author__ = ""
__email__ = "@westwoodrobotics.io"
__copyright__ = "Copyright 2022 Westwood Robotics"
__date__ = "July 18, 2022"
__version__ = "0.0.1"
__status__ = "Prototype"

# Test bulk_comm speed

from pybear import Manager
import time

BEAR_LIST = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

port = 'COM11'
baud = 8000000
pbm = Manager.BEAR(port=port, baudrate=baud, bulk_timeout=0.002)


start_time = time.time()
for i in range(1000):
    pbm.bulk_read(BEAR_LIST, ['present_position', 'present_velocity', 'present_iq'])
end_time = time.time()
freq = 1000/(end_time-start_time)

print("Bulk_comm frequency: %2.4f" % freq)

    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule: #!usr/bin/env python
__author__ = ""
__email__ = "@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Jan 8, 2021"
__version__ = "0.1.3"
__status__ = "Production"

# Ping and search for available BEARs
from pybear import Manager

# Define port and baud rate
bear_port = "COM7"
bear_baudrate = 8000000
# Define ID search range
id_range = range(0, 9)

bear = Manager.BEAR(port=bear_port, baudrate=bear_baudrate)
bear_list = []
found = False
for i in id_range:
    m_id = i
    print("Pinging BEAR with ID %d" % m_id)
    data = bear.ping(m_id)[0]
    if data:
        print("Found BEAR with ID %d." % m_id)
        found = True
        bear_list.append(m_id)
if found:
    print("Search done. Total of %d BEARs found. And their IDs are:\n" % len(bear_list))
    print(bear_list)
else:
    print("Search done. No BEAR found.")
    - cron: <component name="InspectionProjectProfileManager">
  <settings>
    <option name="USE_PROJECT_PROFILE" value="false" />
    <version value="1.0" />
  </settings>
</component>

jobs: # Default ignored files
/shelf/
/workspace.xml
  analyze: <?xml version="1.0" encoding="UTF-8"?>
<module type="PYTHON_MODULE" version="4">
  <component name="NewModuleRootManager">
    <content url="file://$MODULE_DIR$">
      <excludeFolder url="file://$MODULE_DIR$/venv" />
    </content>
    <orderEntry type="jdk" jdkName="Python 3.10" jdkType="Python SDK" />
    <orderEntry type="sourceFolder" forTests="false" />
  </component>
  <component name="PyDocumentationSettings">
    <option name="format" value="PLAIN" />
    <option name="myDocStringFormat" value="Plain" />
  </component>
</module>
    name: <?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="ProjectRootManager" version="2" project-jdk-name="Python 3.10" project-jdk-type="Python SDK" />
</project>
    runs-on: <?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="ProjectModuleManager">
    <modules>
      <module fileurl="file://$PROJECT_DIR$/.idea/PyBEAR.iml" filepath="$PROJECT_DIR$/.idea/PyBEAR.iml" />
    </modules>
  </component>
</project>
    permissions: #!usr/bin/env python
__author__ = ""
__email__ = "@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Aug 20, 2020"
__version__ = "0.1.0"
__status__ = "Production"

# -----------------------------
# Move BEAR from the current angle to a specified angle
# For more details, please check the SDK manual (https://www.westwoodrobotics.io/support/)

from pybear import Manager
import time

error = False
bear = Manager.BEAR(port="/dev/ttyUSB0", baudrate=8000000)  # need to identify the port name on your PC

m_id = 1  # BEAR ID (default is 1)

p_gain = 5.0  # Set P gain as spring stiffness
d_gain = 0.2  # Set D gain as damper strength
i_gain = 0.0  # I gain is usually not needed
iq_max = 1.5  # Max iq

BEAR_connected = bear.ping(m_id)[0]
if not BEAR_connected:
    # BEAR is offline
    print("BEAR is offline. Check power and communication.")
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
    bear.set_limit_iq_max((m_id, iq_max))

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
    print("Thanks for using BEAR!")
      actions: #!usr/bin/env python
__author__ = ""
__email__ = "@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Aug 20, 2020"
__version__ = "0.1.0"
__status__ = "Production"

# -----------------------------
# BEAR with a bar, spring on one side, damper to the other side, with the 90 degree in the center being free.

from pybear import Manager
import sys
import select
import os

error = False
bear = Manager.BEAR(port="COM9", baudrate=8000000)

m_id = 1  # Set motor ID

p_gain = 5  # Set P gain as the K of spring
d_gain = 2  # Set D gain as damper strength
iq_max = 1.5  # Max iq

if not(bear.ping(m_id)[0]):
    # BEAR is offline
    print("BEAR is offline. Check power and communication.")
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
    bear.set_limit_iq_max((m_id, iq_max))

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
    print("Thanks for using BEAR!")
      contents: #!usr/bin/env python


################################################################################
# Copyright 2020 Westwood Robotics Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

__author__ = "Westwood Robotics Corporation"
__email__ = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Jan. 01, 2020"

__version__ = "0.1.2"
__status__ = "Production"

"""
Control Table
"""


class INSTRUCTION:
    """
    Instruction Constants
    ---------------------
    This is the field that defines the purpose of the Packet.
    """
    PING = 0x01
    READ_STAT = 0x02
    WRITE_STAT = 0x03
    READ_CFG = 0x04
    WRITE_CFG = 0x05
    SAVE_CFG = 0x06
    BULK_COMM = 0x12


class CFG_REG:
    """Configuration Registers"""
    ID = 0x00
    MODE = 0x01
    BAUDRATE = 0x02
    HOMING_OFFSET = 0x03
    # Gains for Id current loop
    P_GAIN_ID = 0x04
    I_GAIN_ID = 0x05
    D_GAIN_ID = 0x06
    # Gains for Iq current loop
    P_GAIN_IQ = 0x07
    I_GAIN_IQ = 0x08
    D_GAIN_IQ = 0x09
    # Gains for velocity loop
    P_GAIN_VEL = 0x0A
    I_GAIN_VEL = 0x0B
    D_GAIN_VEL = 0x0C
    # Gains for position loop
    P_GAIN_POS = 0x0D
    I_GAIN_POS = 0x0E
    D_GAIN_POS = 0x0F
    # Gains for direct force loop
    P_GAIN_FORCE = 0x10
    I_GAIN_FORCE = 0x11
    D_GAIN_FORCE = 0x12
    # Limits
    LIMIT_ACC_MAX = 0x13
    LIMIT_I_MAX = 0x14
    LIMIT_VEL_MAX = 0x15
    LIMIT_POS_MIN = 0x16
    LIMIT_POS_MAX = 0x17
    MIN_VOLTAGE = 0x18
    MAX_VOLTAGE = 0x19
    # LOW_VOLTAGE_WARNING = 0x1A
    WATCHDOG_TIMEOUT = 0x1A
    TEMP_LIMIT_LOW = 0x1B  # Motor will start to limit power
    TEMP_LIMIT_HIGH = 0x1C  # Motor will shutdown

    UINT_REG = [ID, MODE, BAUDRATE, WATCHDOG_TIMEOUT]


CFG_REG_DIC = {'id': CFG_REG.ID,
               'mode': CFG_REG.MODE,
               'baudrate': CFG_REG.BAUDRATE,
               'homing_offset': CFG_REG.HOMING_OFFSET,
               'p_gain_id': CFG_REG.P_GAIN_ID,
               'i_gain_id': CFG_REG.I_GAIN_ID,
               'd_gain_id': CFG_REG.D_GAIN_ID,
               'p_gain_iq': CFG_REG.P_GAIN_IQ,
               'i_gain_iq': CFG_REG.I_GAIN_IQ,
               'd_gain_iq': CFG_REG.D_GAIN_IQ,
               'p_gain_velocity': CFG_REG.P_GAIN_VEL,
               'i_gain_velocity': CFG_REG.I_GAIN_VEL,
               'd_gain_velocity': CFG_REG.D_GAIN_VEL,
               'p_gain_position': CFG_REG.P_GAIN_POS,
               'i_gain_position': CFG_REG.I_GAIN_POS,
               'd_gain_position': CFG_REG.D_GAIN_POS,
               'p_gain_direct_force': CFG_REG.P_GAIN_FORCE,
               'i_gain_direct_force': CFG_REG.I_GAIN_FORCE,
               'd_gain_direct_force': CFG_REG.D_GAIN_FORCE,
               'limit_acc_max': CFG_REG.LIMIT_ACC_MAX,
               'limit_i_max': CFG_REG.LIMIT_I_MAX,
               'limit_iq_max': CFG_REG.LIMIT_I_MAX,
               'limit_velocity_max': CFG_REG.LIMIT_VEL_MAX,
               'limit_position_min': CFG_REG.LIMIT_POS_MIN,
               'limit_position_max': CFG_REG.LIMIT_POS_MAX,
               'min_voltage': CFG_REG.MIN_VOLTAGE,
               'max_voltage': CFG_REG.MAX_VOLTAGE,
               # 'low_voltage_warning': CFG_REG.LOW_VOLTAGE_WARNING,
               'watchdog_timeout': CFG_REG.WATCHDOG_TIMEOUT,
               'temp_limit_low': CFG_REG.TEMP_LIMIT_LOW,
               'temp_limit_high': CFG_REG.TEMP_LIMIT_HIGH}


class STAT_REG:
    """Status Registers"""
    TORQUE_ENABLE = 0x00  # Enable output
    HOMING_COMPLETE = 0x01
    GOAL_ID = 0x02
    GOAL_IQ = 0x03
    GOAL_VEL = 0x04
    GOAL_POS = 0x05
    PRESENT_ID = 0x06
    PRESENT_IQ = 0x07
    PRESENT_VEL = 0x08
    PRESENT_POS = 0x09
    INPUT_VOLTAGE = 0x0A
    WINDING_TEMP = 0x0B
    POWERSTAGE_TEMP = 0x0C
    IC_TEMP = 0x0D
    ERROR_STATUS = 0x0E
    WARNING_STATUS = 0x0F


STAT_REG_DIC = {'torque_enable': STAT_REG.TORQUE_ENABLE,
                'homing_complete': STAT_REG.HOMING_COMPLETE,
                'goal_id': STAT_REG.GOAL_ID,
                'goal_iq': STAT_REG.GOAL_IQ,
                'goal_velocity': STAT_REG.GOAL_VEL,
                'goal_position': STAT_REG.GOAL_POS,
                'present_id': STAT_REG.PRESENT_ID,
                'present_iq': STAT_REG.PRESENT_IQ,
                'present_velocity': STAT_REG.PRESENT_VEL,
                'present_position': STAT_REG.PRESENT_POS,
                'input_voltage': STAT_REG.INPUT_VOLTAGE,
                'winding_temperature': STAT_REG.WINDING_TEMP,
                'powerstage_temperature': STAT_REG.POWERSTAGE_TEMP,
                'ic_temperature': STAT_REG.IC_TEMP,
                'error_status': STAT_REG.ERROR_STATUS,
                'warning_status': STAT_REG.WARNING_STATUS
      security-events: #!usr/bin/env python


################################################################################
# Copyright 2020 Westwood Robotics Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

__author__ = "Westwood Robotics Corporation"
__email__ = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Jan. 01, 2020"

__version__ = "0.1.2"
__status__ = "Production"

class CRC:
        TABLE = ( 
        0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011, 
        0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
        0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
        0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
        0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
        0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
        0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
        0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
        0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
        0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
        0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
        0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
        0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
        0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
        0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
        0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
        0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
        0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
        0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
        0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
        0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
        0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
        0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
        0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
        0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
        0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
        0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
        0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
        0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
        0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
        0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
        0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
        )

    steps: #!usr/bin/env python

################################################################################
# Copyright 2020 Westwood Robotics Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

__author__ = "Westwood Robotics Corporation"
__email__ = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Jan. 01, 2020"

__version__ = "0.1.2"
__status__ = "Production"

import os

import pdb

import serial

import time

# import Packet
from pybear import Packet

from pybear.CONTROL_TABLE import *


class BEAR(Packet.PKT):
    """
    Provides control of Dynamixel servos using PySerial
    """
    def __init__(self,
                 port = '/dev/ttyUSB0',
                 baudrate = '8000000',
                 bytesize = serial.EIGHTBITS,
                 parity = serial.PARITY_NONE,
                 stopbits = serial.STOPBITS_ONE,
                 timeout = None,
                 bulk_timeout = None,
                 debug = False):
        """
        Constructor for opening up the serial port.
        :param port: Port address; Should be specified per object if using multiple chains
        :param baudrate: Specified baudrate
        """

        self.port = port
        self.baudrate = baudrate
        self.bytesize = bytesize
        self.parity = parity
        self.stopbits = stopbits


        self.bulk_timeout = bulk_timeout
        self.timeout = timeout

        self.connected = False
        self.motors = {}

        self.ascii_art = True

        self.debug = debug # Option for ease of debugging. Set to false for proper operation.

        super(BEAR, self).__init__(self.port, self.baudrate, timeout=timeout, bulk_timeout=bulk_timeout)

        self.welcome_msg()

    def welcome_msg(self):
        if self.ascii_art:
            os.system('clear')
            print("==============================================================")
            print("   __        __          _                               _ ")
            print("   \ \      / /___  ___ | |_ __      __ ___    ___    __| | ")
            print("    \ \ /\ / // _ \/ __|| __|\ \ /\ / // _ \  / _ \  / _` | ")
            print("     \ V  V /|  __/\__ \| |_  \ V  V /| (_) || (_) || (_| | ")
            print("      \_/\_/  \___||___/ \__|  \_/\_/  \___/  \___/  \__,_| ")
            print("              ____         _             _    _             ")
            print("             |  _ \  ___  | |__    ___  | |_ (_)  ___  ___  ")
            print("             | |_) |/ _ \ | '_ \  / _ \ | __|| | / __|/ __| ")
            print("             |  _ <| (_) || |_) || (_) || |_ | || (__ \__ \ ")
            print("             |_| \_\\\___/ |_.__/  \___/  \__||_| \___||___/ ")
            print("==============================================================")
            print("=== PyBEAR by Westwood Robotics -- Last Updated 2020.06.29 ===")
            print("==============================================================")

    # =================================================================================================================
    # ===== Configuration Registers

    def get_id(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.ID) for idx in range(len(argv))]

    def set_id(self, *argv):
        self.multi_write_cfg_data(CFG_REG.ID, argv)

    def get_mode(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.MODE) for idx in range(len(argv))]

    def set_mode(self, *argv):
        self.multi_write_cfg_data(CFG_REG.MODE, argv)

    def get_baudrate(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.BAUDRATE) for idx in range(len(argv))]

    def set_baudrate(self, *argv):
        self.multi_write_cfg_data(CFG_REG.BAUDRATE, argv)

    def get_homing_offset(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.HOMING_OFFSET) for idx in range(len(argv))]

    def set_homing_offset(self, *argv):
        self.multi_write_cfg_data(CFG_REG.HOMING_OFFSET, argv)

    def get_p_gain_id(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.P_GAIN_ID) for idx in range(len(argv))]

    def set_p_gain_id(self, *argv):
        self.multi_write_cfg_data(CFG_REG.P_GAIN_ID, argv)

    def get_i_gain_id(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.I_GAIN_ID) for idx in range(len(argv))]

    def set_i_gain_id(self, *argv):
        self.multi_write_cfg_data(CFG_REG.I_GAIN_ID, argv)

    def get_d_gain_id(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.D_GAIN_ID) for idx in range(len(argv))]

    def set_d_gain_id(self, *argv):
        self.multi_write_cfg_data(CFG_REG.D_GAIN_ID, argv)

    def get_p_gain_iq(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.P_GAIN_IQ) for idx in range(len(argv))]

    def set_p_gain_iq(self, *argv):
        self.multi_write_cfg_data(CFG_REG.P_GAIN_IQ, argv)

    def get_i_gain_iq(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.I_GAIN_IQ) for idx in range(len(argv))]

    def set_i_gain_iq(self, *argv):
        self.multi_write_cfg_data(CFG_REG.I_GAIN_IQ, argv)

    def get_d_gain_iq(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.D_GAIN_IQ) for idx in range(len(argv))]

    def set_d_gain_iq(self, *argv):
        self.multi_write_cfg_data(CFG_REG.D_GAIN_IQ, argv)

    def get_p_gain_velocity(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.P_GAIN_VEL) for idx in range(len(argv))]

    def set_p_gain_velocity(self, *argv):
        self.multi_write_cfg_data(CFG_REG.P_GAIN_VEL, argv)

    def get_i_gain_velocity(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.I_GAIN_VEL) for idx in range(len(argv))]

    def set_i_gain_velocity(self, *argv):
        self.multi_write_cfg_data(CFG_REG.I_GAIN_VEL, argv)

    def get_d_gain_velocity(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.D_GAIN_VEL) for idx in range(len(argv))]

    def set_d_gain_velocity(self, *argv):
        self.multi_write_cfg_data(CFG_REG.D_GAIN_VEL, argv)

    def get_p_gain_position(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.P_GAIN_POS) for idx in range(len(argv))]

    def set_p_gain_position(self, *argv):
        self.multi_write_cfg_data(CFG_REG.P_GAIN_POS, argv)

    def get_i_gain_position(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.I_GAIN_POS) for idx in range(len(argv))]

    def set_i_gain_position(self, *argv):
        self.multi_write_cfg_data(CFG_REG.I_GAIN_POS, argv)

    def get_d_gain_position(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.D_GAIN_POS) for idx in range(len(argv))]

    def set_d_gain_position(self, *argv):
        self.multi_write_cfg_data(CFG_REG.D_GAIN_POS, argv)

    def get_p_gain_force(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.P_GAIN_FORCE) for idx in range(len(argv))]

    def set_p_gain_force(self, *argv):
        self.multi_write_cfg_data(CFG_REG.P_GAIN_FORCE, argv)

    def get_i_gain_force(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.I_GAIN_FORCE) for idx in range(len(argv))]

    def set_i_gain_force(self, *argv):
        self.multi_write_cfg_data(CFG_REG.I_GAIN_FORCE, argv)

    def get_d_gain_force(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.D_GAIN_FORCE) for idx in range(len(argv))]

    def set_d_gain_force(self, *argv):
        self.multi_write_cfg_data(CFG_REG.D_GAIN_FORCE, argv)

    def get_limit_id_max(self, *argv):
        print("This function is no longer valid.")

    def set_limit_id_max(self, *argv):
        print("This function is no longer valid.")

    def get_limit_acc_max(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.LIMIT_ACC_MAX) for idx in range(len(argv))]

    def set_limit_acc_max(self, *argv):
        self.multi_write_cfg_data(CFG_REG.LIMIT_ACC_MAX, argv)

    def get_limit_iq_max(self, *argv):
        """
        This function is depreciated. Use get_limit_i_max() instead.
        """
        return [self.read_cfg_data(argv[idx], CFG_REG.LIMIT_I_MAX) for idx in range(len(argv))]

    def set_limit_iq_max(self, *argv):
        """
        This function is depreciated. Use set_limit_i_max() instead.
        """
        self.multi_write_cfg_data(CFG_REG.LIMIT_I_MAX, argv)

    def get_limit_i_max(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.LIMIT_I_MAX) for idx in range(len(argv))]

    def set_limit_i_max(self, *argv):
        self.multi_write_cfg_data(CFG_REG.LIMIT_I_MAX, argv)

    def get_limit_velocity_max(self,*argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.LIMIT_VEL_MAX) for idx in range(len(argv))]

    def set_limit_velocity_max(self, *argv):
        self.multi_write_cfg_data(CFG_REG.LIMIT_VEL_MAX, argv)

    def get_limit_position_min(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.LIMIT_POS_MIN) for idx in range(len(argv))]

    def set_limit_position_min(self, *argv):
        self.multi_write_cfg_data(CFG_REG.LIMIT_POS_MIN, argv)

    def get_limit_position_max(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.LIMIT_POS_MAX) for idx in range(len(argv))]

    def set_limit_position_max(self, *argv):
        self.multi_write_cfg_data(CFG_REG.LIMIT_POS_MAX, argv)

    def get_min_voltage(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.MIN_VOLTAGE) for idx in range(len(argv))]

    def set_min_voltage(self, *argv):
        self.multi_write_cfg_data(CFG_REG.MIN_VOLTAGE, argv)

    def get_max_voltage(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.MAX_VOLTAGE) for idx in range(len(argv))]

    def set_max_voltage(self, *argv):
        self.multi_write_cfg_data(CFG_REG.MAX_VOLTAGE, argv)

    # def get_low_voltage_warning(self, *argv):
    #     return [self.read_cfg_data(argv[idx], CFG_REG.LOW_VOLTAGE_WARNING) for idx in range(len(argv))]
    #
    # def set_low_voltage_warning(self, *argv):
    #     self.multi_write_cfg_data(CFG_REG.LOW_VOLTAGE_WARNING, argv)
    def get_watchdog_timeout(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.WATCHDOG_TIMEOUT) for idx in range(len(argv))]

    def set_watchdog_timeout(self, *argv):
        self.multi_write_cfg_data(CFG_REG.WATCHDOG_TIMEOUT, argv)

    def get_temp_limit_low(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.TEMP_LIMIT_LOW) for idx in range(len(argv))]

    def set_temp_limit_low(self, *argv):
        self.multi_write_cfg_data(CFG_REG.TEMP_LIMIT_LOW, argv)

    def get_temp_limit_high(self, *argv):
        return [self.read_cfg_data(argv[idx], CFG_REG.TEMP_LIMIT_HIGH) for idx in range(len(argv))]

    def set_temp_limit_high(self, *argv):
        self.multi_write_cfg_data(CFG_REG.TEMP_LIMIT_HIGH, argv)

    def get_bulk_config(self, *argv):
        #TODO: Verbally missleading as this is not BULK_COMM. Replace with get_config() and depreciate.
        """
        Read multiple config registers from a single motor in one packet.
        Multiple target motors can be visited but goes through them one-by-one
        argv = (ID, reg1, reg2 ...), (ID, reg1, reg2 ...) ...
        """
        return [self.read_bulk_cfg_data(argv[idx]) for idx in range(len(argv))]

    def set_bulk_config(self, *argv):
        # TODO: Verbally missleading as this is not BULK_COMM. Replace with set_config() and depreciate.
        """
        Write to multiple config registers on a single motor in one packet.
        Multiple target motors can be visited but goes through them one-by-one
        argv = (ID, reg1, data1, reg2 data2...), (ID, reg1, data1 ...) ...
        """
        for data in argv:
            self.write_bulk_cfg_data(data[0], data[1:])

    def get_config(self, *argv):
        """
        Read multiple config registers from a single motor in one packet.

        Multiple target motors can be visited but goes through them one-by-one
        argv = (ID, reg1, reg2 ...), (ID, reg1, reg2 ...) ...

        CAUTION: Will not work for: ID, Mode, Baudrate, Watchdog_timeout
        """
        return [self.read_bulk_cfg_data(argv[idx]) for idx in range(len(argv))]

    def set_config(self, *argv):
        """
        Write to multiple config registers on a single motor in one packet.

        Multiple target motors can be visited but goes through them one-by-one
        argv = (ID, reg1, data1, reg2 data2...), (ID, reg1, data1 ...) ...

        CAUTION: Will not work for: ID, Mode, Baudrate, Watchdog_timeout
        """
        for data in argv:
            self.write_bulk_cfg_data(data[0], data[1:])


    # =================================================================================================================
    # ===== Status Registers
    def ping(self, *argv: int):
        """
        Function used to detect BEAR(s). Will return firmware and hardware version info.
        Return None if pinging unsuccessful.
        """
        return [self._ping(i) for i in argv]

    def get_torque_enable(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.TORQUE_ENABLE) for idx in range(len(argv))]

    def set_torque_enable(self, *argv):
        self.multi_write_status_data(STAT_REG.TORQUE_ENABLE, argv)

    def get_goal_id(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.GOAL_ID) for idx in range(len(argv))]

    def set_goal_id(self, *argv):
        self.multi_write_status_data(STAT_REG.GOAL_ID, argv)

    def get_goal_iq(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.GOAL_IQ) for idx in range(len(argv))]

    def set_goal_iq(self, *argv):
        self.multi_write_status_data(STAT_REG.GOAL_IQ, argv)

    def get_goal_velocity(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.GOAL_VEL) for idx in range(len(argv))]

    def set_goal_velocity(self, *argv):
        self.multi_write_status_data(STAT_REG.GOAL_VEL, argv)

    def get_goal_position(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.GOAL_POS) for idx in range(len(argv))]

    def set_goal_position(self, *argv):
        self.multi_write_status_data(STAT_REG.GOAL_POS, argv)

    def get_present_id(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.PRESENT_ID) for idx in range(len(argv))]

    def get_present_iq(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.PRESENT_IQ) for idx in range(len(argv))]

    def get_present_velocity(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.PRESENT_VEL) for idx in range(len(argv))]

    def get_present_position(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.PRESENT_POS) for idx in range(len(argv))]

    def get_input_voltage(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.INPUT_VOLTAGE) for idx in range(len(argv))]

    def get_winding_temperature(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.WINDING_TEMP) for idx in range(len(argv))]

    def get_powerstage_temperature(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.POWERSTAGE_TEMP) for idx in range(len(argv))]

    def get_ic_temperature(self, *argv):
        return [self.read_status_data(argv[idx], STAT_REG.IC_TEMP) for idx in range(len(argv))]

    # Not implemented yet
    # def get_error_status(self, m_id):
    #     val = self.read_status_data(m_id, STAT_REG.ERROR_STATUS)
    #     return val
    #
    # def get_warning_status(self, m_id):
    #     val = self.read_status_data(m_id, STAT_REG.WARNING_STATUS)
    #     return val

    def get_bulk_status(self, *argv):
        # TODO: Verbally missleading as this is not BULK_COMM. Replace with get_status() and depreciate.
        """
        Read multiple status registers from a single motor in one packet.
        Multiple target motors can be visited but goes through them one-by-one
        e.g.: get_bulk_status((ID, reg1, reg2), (ID, reg1, reg2, reg3))
        """
        return [self.read_bulk_status_data(data) for data in argv]

    def set_bulk_status(self, *argv):
        # TODO: Verbally missleading as this is not BULK_COMM. Replace with set_status() and depreciate.
        """
        Write to multiple status registers on a single motor in one packet.
        Multiple target motors can be visited but goes through them one-by-one
        e.g.: set_bulk_status((ID, reg1, data1, reg2, data2), (ID, reg1, data1 ...))
        """
        for data in argv:
            self.write_bulk_status_data(data[0], data[1:])

    def get_status(self, *argv):
        """
        |  Read multiple status registers from a single motor in one packet.
        |  Multiple target motors can be visited but goes through them one-by-one
        |  e.g.: get_bulk_status((ID, reg1, reg2), (ID, reg1, reg2, reg3))
        |
        |  CAUTION: Will not work for Torque Enable
        """
        return [self.read_bulk_status_data(data) for data in argv]

    def set_status(self, *argv):
        """
        |  Write to multiple status registers on a single motor in one packet.
        |  Multiple target motors can be visited but goes through them one-by-one
        |  e.g.: set_bulk_status((ID, reg1, data1, reg2, data2), (ID, reg1, data1 ...))

        |  CAUTION: Will not work for Torque Enable
        """
        for data in argv:
            self.write_bulk_status_data(data[0], data[1:])

    def bulk_read(self, m_ids, read_registers, error_mode = 0):
        """
        Up to 16 registers

        Parameters
        ----------
        m_ids
            list of IDs
        read_registers
            list of regirsters to read
        error_mode
            0(default): return[None, -99] for BEAR with corrupted data;
            1: return None as long as there is any error
        """
        return self.bulk_comm(m_ids, read_registers, [], [], error_mode)

    def bulk_write(self, m_ids, write_registers, write_data):
        """
        Up to 16 registers

        Parameters
        ----------
        m_ids
            list of IDs
        write_registers
            list of regirsters to write to
        write_data
            list of data-list to write [[ID1-data1, ID1-data2 ...],
                                        [ID2-data1, ID2-data2 ...] ...]
        ----------
        """
        return self.bulk_comm(m_ids, [], write_registers, write_data)

    def bulk_read_write(self, m_ids, read_reg, write_reg, write_data, error_mode = 0):
        """
        Read up to 16 registers and write to up to 16 registers

        Parameters
        ----------
        m_ids
            list of IDs [ID1, ID2 ...]
        read_reg
            list of regirsters to read [reg1, reg2 ...]
        write_reg
            list of regirsters to write to [reg1, reg2 ...]
        write_data
            list of data-list to write [[ID1-data1, ID1-data2 ...],
                                        [ID2-data1, ID2-data2 ...] ...]
        error_mode
            0(default): return[None, -99] for BEAR with corrupted data;
            1: return None as long as there is any error
        ----------
        """
        return self.bulk_comm(m_ids, read_reg, write_reg, write_data, error_mode) 

          #------------------------------------------BLACKDUCK----------------------------------------
    - name: #!usr/bin/env python

################################################################################
# Copyright 2020 Westwood Robotics Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

__author__ = "Westwood Robotics Corporation"
__email__ = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Jan. 01, 2020"

__version__ = "0.1.2"
__status__ = "Production"

import pdb
import struct
import time
import sys
import ctypes

from itertools import chain

import numpy as np
import serial

from pybear.TIMING_TABLE import *
from pybear.CONTROL_TABLE import *
from pybear.CRC import *


class PKT(object):
    def __init__(self, port, baudrate, timeout=None, bulk_timeout=None):
        if not self.debug:
            self.ser = serial.Serial(
                port,
                baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=timeout,
            )
        else:
            pass
        if bulk_timeout is None:
            self._bulk_timeout = BULK_TIMEOUT_DEFAULT
        else:
            self._bulk_timeout = bulk_timeout

        if timeout is None:
            self._timeout = TIMEOUT_DEFAULT
        else:
            self._timeout = timeout

        if sys.version_info[0] < 3: # Python 2
            self.ord_adapt = lambda x : ord(x)
            self.sustr_adapt = lambda val: ''.join(chr(idx) for idx in val)
            self.sustr_loop_adapt = lambda idx, val: ''.join(chr(idx) for idx in val[idx:idx+4])
            pass
        else: # Python 3
            self.ord_adapt = lambda x : x
            self.sustr_adapt = lambda val: bytearray(val)
            self.sustr_loop_adapt = lambda idx, val: bytearray(val[idx:idx+4])

    def close(self):
        """
        Close the serial port
        """
        if self.ser:
            self.ser.close()

    def __del__(self):
        self.close()

    def __write_packet(self, packet):
        if not self.debug:
            self.ser.reset_input_buffer()
            self.ser.write(bytearray(packet))
        else:
            print("[DEBUG] :: __write_packet(): {}".format(packet))
            pdb.set_trace()

    # def __read_packet(self, motor_id):
    #     status_packet = []
    #     status_packet.extend(self.ser.read(4))
    #     # print("Waiting in... : {}".format(status_packet))
    #     if status_packet:
    #         extlen = ord(status_packet[3])
    #         while self.ser.inWaiting() < extlen:
    #             pass
    #         status_packet.extend(self.ser.read(extlen))
    #         status_packet = [ord(idx) for idx in status_packet[5:-1]]
    #     return status_packet

    def __read_packet(self, motor_id):
        status_packet = []
        status_packet.extend(self.ser.read(4))
        if status_packet:
            extlen = self.ord_adapt(status_packet[3])
            while self.ser.in_waiting < extlen:
                pass
            status_packet.extend(self.ser.read(extlen))
            # Check for checksum
            checksum = ctypes.c_ubyte(~(ctypes.c_ubyte(sum(status_packet[2:-1])).value))
            if checksum.value == status_packet[-1]:
                error_code = status_packet[4]
                status_packet = [self.ord_adapt(idx) for idx in status_packet[5:-1]]
                return status_packet, error_code
            else:
                # checksum is wrong
                return None, None

    def __read_bulk_packet(self, motor_id):
        """
        Returning multiple register values in the order of the initial status packet.
        """
        pass

    def __write_data(self, m_id, address, data, reg_type=None):
        """
        Write to singel register
        """
        if reg_type == 'cfg':
            instruction = INSTRUCTION.WRITE_CFG
        elif reg_type == 'stat':
            instruction = INSTRUCTION.WRITE_STAT

        if (address not in CFG_REG.UINT_REG and reg_type == 'cfg') or (address > 1 and reg_type == 'stat'):
            data = self.__float32_to_hex(data)
            data = [data[i:i+2] for i in range(2,len(data),2)]
            data = tuple([int(x, 16) for x in data])
            n_add = len((address,))+len((data))+2
            checksum = self.chksum(m_id, n_add, instruction, (address, sum(data)))
        else:
            data = (data & 0xFF, (data >> 8) & 0xFF, (data >> 16) & 0xFF, (data >> 24) & 0xFF)
            n_add = len((address,))+len((data))+2
            checksum = self.chksum(m_id, n_add, instruction, (address, sum(data)))

        # Generate packet
        packet = self.__packet_generator(m_id, n_add, instruction, address, data, checksum)

        # Write packet
        self.__write_packet(packet)

    def write_status_data(self, m_id, address, data):
        """
        This command is to write data to single status register.
        """
        self.__write_data(m_id, address, data, reg_type='stat')

    def multi_write_status_data(self, add, data):
        """
        Convenient loop function for writing to multiple motors and to a single register.
        # TODO: To be merged into self.write_cfg_data() and deprecated.
        """
        for idx in range(len(data)):
            self.write_status_data(data[idx][0], add, data[idx][1])

    def write_cfg_data(self, m_id, address, data):
        """
        This command is to write data to single configuration register.
        """
        self.__write_data(m_id, address, data, reg_type='cfg')

    def multi_write_cfg_data(self, add, data):
        """
        Convenient loop function for writing to multiple motors and to a single register.
        # TODO: To be merged into self.write_cfg_data() and deprecated.
        """
        for idx in range(len(data)):
            self.write_cfg_data(data[idx][0], add, data[idx][1])

    def write_bulk_cfg_data(self, m_id, adpair):
        """
        Write multiple configuration registers on a single motor in one packet.
        m_id: Motor ID
        adpair: ('address', data) pairs
        """
        self.__write_bulk_data(m_id, adpair, reg_type='cfg')

    # def multi_write_bulk_cfg_data(self, argv):
    #     """
    #     Convenient loop function for writing to multiple motors with bulk writes to registers.
    #     """
    #     for idx in range(len(argv)):
    #         self.write_bulk_cfg_data(argv[idx][0], argv[idx][1:])

    def write_bulk_status_data(self, m_id, adpair):
        """
        Write multiple configuration registers on a single motor in one packet.
        m_id: Motor ID
        adpair: ('address', data) pairs
        """
        self.__write_bulk_data(m_id, adpair, reg_type='stat')

    def __write_bulk_data(self, m_id, adpair, reg_type=None):
        if reg_type == 'cfg':
            instruction = INSTRUCTION.WRITE_CFG
        elif reg_type == 'stat':
            instruction = INSTRUCTION.WRITE_STAT

        n_add = 0
        n_adpair = len(adpair)//2 # Identify the number of registers to write to
        adpair_hex = []

        for idx in range(n_adpair):
            idx_ptr = 2*idx
            if reg_type == 'cfg':
                addr = CFG_REG_DIC[adpair[idx_ptr]]
            elif reg_type == 'stat':
                addr = STAT_REG_DIC[adpair[idx_ptr]]

            data = adpair[idx_ptr+1]

            # if addr > 2 and reg_type == 'cfg' or addr > 1 and reg_type == 'stat':
            if (addr not in CFG_REG.UINT_REG and reg_type == 'cfg') or (addr > 1 and reg_type == 'stat'):
                data = self.__float32_to_hex(data)
                data = [data[i:i+2] for i in range(2,len(data),2)]
                data = [int(x, 16) for x in data]
                if reg_type == 'cfg':
                    n_add += len((CFG_REG_DIC[adpair[idx_ptr]],)) + len(data)
                elif reg_type == 'stat':
                    n_add += len((STAT_REG_DIC[adpair[idx_ptr]],)) + len(data)
            else:
                data = list((data & 0xFF, (data >> 8) & 0xFF, (data >> 16) & 0xFF, (data >> 24) & 0xFF))
                if reg_type == 'cfg':
                    n_add += len((CFG_REG_DIC[adpair[idx_ptr]],))+len(data)
                elif reg_type == 'stat':
                    n_add += len((STAT_REG_DIC[adpair[idx_ptr]],))+len(data)

            adpair_hex = adpair_hex + [addr] + data

        n_add += 2
        checksum = self.chksum(m_id, n_add, instruction, adpair_hex)

        # Generate packet
        packet = self.__packet_generator(m_id, n_add, instruction, adpair_hex, None, checksum)

        # Write packet
        self.__write_packet(packet)

    def __read_data(self, m_id, add_list, reg_type=None, data_type=None):
        """
        Read Single data
        """
        if reg_type == 'cfg':
            instruction = INSTRUCTION.READ_CFG
        elif reg_type == 'stat':
            instruction = INSTRUCTION.READ_STAT

        # pkt_len = np.add(len((add_list,)), 2)
        pkt_len = len((add_list,))+2

        checksum = self.chksum(m_id, pkt_len, instruction, (add_list,))

        packet = self.__packet_generator(m_id, pkt_len, instruction, add_list, None, checksum)

        # while self.ser.in_waiting > 0:
        #     self.ser.reset_input_buffer()

        while True:
            # Corrupted data prevention if checksum is wrong
            # Timeout prevention if communication error starts occuring
            self.ser.reset_input_buffer()
            self.__write_packet(packet)

            t_bus_init = time.time()
            while True:
                if self.ser.in_waiting > 6:
                    break
                if time.time() - t_bus_init > self._timeout:
                    print("[PyBEAR | WARNING] ::  Read response timed out. Re-sending the same packet.")
                    self.ser.reset_input_buffer()
                    self.__write_packet(packet)
                    t_bus_init = time.time()
            status, error_code = self.__read_packet(m_id)
            if error_code:
                # checksum was right
                break
            else:
                # checksum was wrong and __read_packet() returned None
                print("[PyBEAR | WARNING] :: Data corrupted. Re-sending the same packet.")

        if data_type == 'f32':
            return self.__hex_to_float32(status), error_code
        elif data_type == 'u32':
            # return status[0]|status[1]<<8|status[2]<<16|status[3]<<24, error_code
            return self.__hex_to_int32(status), error_code


    # def __read_bulk_data(self, m_id, add_list, reg_type=None, data_type=None):
    #     """
    #     m_id: Motor id
    #     add_list: List of register addresses to read
    #     """
    #     if reg_type == 'cfg':
    #         instruction = INSTRUCTION.READ_CFG
    #     elif reg_type == 'stat':
    #         instruction = INSTRUCTION.READ_STAT
    #
    #     # pkt_len = np.add(len(add_list), 2)
    #     pkt_len = len(add_list)+2
    #
    #     checksum = self.chksum(m_id, pkt_len, instruction, add_list)
    #
    #     packet = self.__packet_generator(m_id, pkt_len, instruction, add_list, None, checksum)
    #
    #     self.__write_packet(packet)
    #
    #     while self.ser.in_waiting < 4:
    #         pass
    #
    #     # status = self.__read_bulk_packet(m_id)
    #     status, error_code = self.__read_packet(m_id)
    #
    #     # Place holder for multiple reads later.
    #     # read_val = status[3]
    #
    #     if data_type == 'f32':
    #         # return self.__hex_to_float32(status[5:-1])
    #         return self.__hex_to_float32(status), error_code
    #     elif data_type == 'u32':
    #         # return status[5]
    #         return status[0], error_code
    #
    def __read_bulk_data(self, m_id, add_list, reg_type=None, data_type=None):
        """
        m_id: Motor id
        add_list: List of register addresses to read
        CAUTION: Will not work for: ID, Mode, Baudrate, Watchdog_timeout and Torque Enable
        """
        if reg_type == 'cfg':
            instruction = INSTRUCTION.READ_CFG
        elif reg_type == 'stat':
            instruction = INSTRUCTION.READ_STAT

        # pkt_len = np.add(len(add_list), 2)
        pkt_len = len(add_list)+2

        checksum = self.chksum(m_id, pkt_len, instruction, add_list)

        packet = self.__packet_generator(m_id, pkt_len, instruction, add_list, None, checksum)

        while True:

            self.ser.reset_input_buffer()
            self.__write_packet(packet)

            t_bus_init = time.time()

            while True:
                if self.ser.in_waiting > 6:
                    # At least we have 0xFF, oxFF, Motor_ID, Length, Error, Data0 in the buffer
                    break
                if time.time() - t_bus_init > self._timeout:
                    # Timeout prevention if communication error starts occuring
                    print("[PyBEAR | WARNING] :: Status response timed out. Re-sending the same packet.")
                    self.ser.reset_input_buffer()
                    self.__write_packet(packet)
                    t_bus_init = time.time()

            status, error_code = self.__read_packet(m_id)
            if error_code:
                # checksum was right
                break
            else:
                # Corrupted data prevention if checksum is wrong
                # checksum was wrong and __read_packet() returned None
                print("[PyBEAR | WARNING] :: Data corrupted. Re-sending the same packet.")

        # Place holder for multiple reads later.
        # read_val = status[3]

        if data_type == 'f32':
            # return self.__hex_to_float32(status[5:-1])
            return self.__hex_to_float32(status), error_code
        elif data_type == 'u32':
            # return status[5]
            return self.__hex_to_int32(status), error_code

    # def __bulk_communication_save(self,):
    #
    #
    #     instruction = INSTRUCTION.BULK_COMM
    #
    #     m_ids = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
    #
    #     num_stat_read = 3
    #     num_stat_write = 0
    #     num_total_stat = num_stat_write + num_stat_read*16
    #     read_addr = [STAT_REG.PRESENT_POS, STAT_REG.PRESENT_VEL, STAT_REG.WINDING_TEMP]
    #     pkt_length = 3+num_stat_read+num_stat_write+len(m_ids)+1
    #
    #     data = [len(m_ids), num_total_stat, sum(read_addr), sum(m_ids)]
    #
    #     checksum = self.chksum(0xFE, pkt_length, instruction, data)
    #
    #     packet = self.__packet_generator_bulk(m_ids, pkt_length, instruction, num_total_stat, read_addr, checksum)
    #
    #     self.__write_packet(packet)
    #
    #     while self.ser.inWaiting() < 4:
    #         pass
    #
    #     status_packet = []
    #     status_packet.extend(self.ser.read(4))
    #     # print("Status packet: {}".format(status_packet))
    #     if status_packet:
    #         retlen = ord(status_packet[3])
    #         totlen = retlen + (4+retlen)*(len(m_ids)-1)
    #         while self.ser.inWaiting() < totlen:
    #             pass
    #         status_packet.extend(self.ser.read(totlen))
    #         output = []
    #         for x in range(len(m_ids)):
    #             for idx in status_packet[5+x*(4+retlen):-1+(x+1)*(4+retlen)]:
    #                 output.append(ord(idx))
    #         status_packet = [ord(idx) for index in range(len(m_ids))
    #                          for idx in status_packet[5+index*(4+retlen):-1+(index+1)*(4+retlen)]]
    #
    #         return np.reshape(self.__hex_to_float32(status_packet), (len(m_ids),num_stat_read))

    # def __bulk_communication(self, m_ids, read_addr, write_addr, write_data):
    #
    #     instruction = INSTRUCTION.BULK_COMM
    #
    #     num_motors = len(m_ids)
    #     num_read_regs = len(read_addr)
    #     num_write_regs = len(write_addr)
    #     num_total_regs = num_write_regs|num_read_regs<<4  # lower 4 bits for writes, higher 4 bits for reads
    #
    #     pkt_length = 3 + num_read_regs + num_write_regs + num_motors + num_write_regs*4*num_motors + 1
    #
    #     if num_write_regs is 0:
    #         data_packets = [len(m_ids), num_total_regs, sum(read_addr), sum(write_addr), sum(m_ids)]
    #         checksum = self.chksum(0xFE, pkt_length, instruction, data_packets)
    #         packet = self.__packet_generator_bulk(m_ids, pkt_length, instruction, num_total_regs, read_addr, write_addr, None, checksum)
    #     else:
    #         data = tuple()
    #         for i in range(num_motors):
    #             command = self.__float32_to_hex(write_data[i])
    #             command = [command[j:j+2] for j in range(2,len(command),2)]
    #             data += (m_ids[i],) + tuple([int(x, 16) for x in command])
    #         data_packets = [len(m_ids), num_total_regs, sum(read_addr), sum(write_addr), sum(data)]
    #         checksum = self.chksum(0xFE, pkt_length, instruction, data_packets)
    #         packet = self.__packet_generator_bulk(m_ids, pkt_length, instruction, num_total_regs, read_addr, write_addr, data, checksum)
    #
    #     num_retries = 3    # set this number to change how many times PyBEAR will try to resend the packet
    #     error_id = 0
    #     error_status = 0
    #
    #     for i in range(num_retries):
    #         t0 = time.time()
    #
    #         self.__write_packet(packet)
    #         found_packet = True
    #
    #         while self.ser.in_waiting < 4:
    #             if self._bulk_timeout is None:
    #                 pass
    #             elif time.time() - t0 > self._bulk_timeout:
    #                 found_packet = False
    #                 break
    #
    #         if found_packet:
    #             status_packet = []
    #             status_packet.extend(self.ser.read(4))
    #             if status_packet:
    #                 #pdb.set_trace()
    #                 retlen = ord(status_packet[3])  # length of the data in a single packet
    #                 totlen = retlen + (4+retlen)*(len(m_ids)-1)  # total length of all data coming back
    #                 found_data = True
    #                 while self.ser.in_waiting < totlen:
    #                     if self._bulk_timeout is None:
    #                         pass
    #                     elif time.time() - t0 > self._bulk_timeout:
    #                         buffer_length = self.ser.in_waiting
    #                         error_id = int((buffer_length - retlen)/(4+retlen)) + 2
    #                         found_data = False
    #                         break
    #
    #                 # read from the buffer either way so that it gets cleared out
    #                 status_packet.extend(self.ser.read(totlen))
    #
    #                 if found_data:
    #                     data_output = [ord(idx) for index in range(len(m_ids))
    #                                    for idx in status_packet[5+index*(4+retlen):-1+(index+1)*(4+retlen)]]
    #                     motor_status = [ord(idx) for index in range(len(m_ids)) for idx in status_packet[4+index*(4+retlen)]]
    #                     return error_status, motor_status, np.reshape(self.__hex_to_float32(data_output), (len(m_ids), num_read_regs))
    #
    #     # Communication has been attempted but has failed!
    #     # Return an error_status and zeros for everything else
    #     if error_id == 0:
    #         # No packets are being returned
    #         error_status = -99
    #     else:
    #         # Return the motor id number that is not communicating
    #         error_status = -error_id
    #     motor_status = np.zeros((len(m_ids), 1))
    #     data_ouput = np.zeros((len(m_ids), num_read_regs))
    #     return error_status, motor_status, data_ouput

    def __bulk_communication(self, m_ids, read_addr, write_addr, write_data, error_mode):
        """
        CAUTION: Will not work for: ID, Mode, Baudrate, Watchdog_timeout and Torque Enable

        Parameters
        ----------
        m_ids
            tuple of IDs
        read_registers
            tuple of regirsters to read
        write_registers
            tuple of regirsters to write to
        write_data
            tuple of data to write
        error_mode
            0(default): return[None, -99] for BEAR with corrupted data;
            1: return None as long as there is any error
        """

        instruction = INSTRUCTION.BULK_COMM

        num_motors = len(m_ids)
        num_read_regs = len(read_addr)
        num_write_regs = len(write_addr)
        num_total_regs = num_write_regs | num_read_regs << 4  # lower 4 bits for writes, higher 4 bits for reads

        pkt_length = 3 + num_read_regs + num_write_regs + num_motors + num_write_regs * 4 * num_motors + 1

        if num_write_regs == 0:
            data_packets = [num_motors, num_total_regs, sum(read_addr), sum(m_ids)]
            checksum = self.chksum(0xFE, pkt_length, instruction, data_packets)
            packet = self.__packet_generator_bulk(m_ids, pkt_length, instruction, num_total_regs, read_addr, write_addr,
                                                  None, checksum)
        else:
            data = tuple()
            for i in range(num_motors):
                command = [self.__float32_to_hex(k) for k in write_data[i]]
                command = [k[j:j+2] for k in command for j in range(2, len(k), 2)]
                data += (m_ids[i],) + tuple([int(x, 16) for x in command])
            data_packets = [num_motors, num_total_regs, sum(read_addr), sum(write_addr), sum(data)]
            checksum = self.chksum(0xFE, pkt_length, instruction, data_packets)
            packet = self.__packet_generator_bulk(m_ids, pkt_length, instruction, num_total_regs, read_addr, write_addr,
                                                  data, checksum)
        if num_read_regs == 0:
            # If there is nothing to read:
            self.__write_packet(packet)
            return True
        else:
            # This is something to read back
            for i in range(BULK_COMM_RETRIES):

                self.ser.reset_input_buffer()
                self.__write_packet(packet)

                t_bus_init = time.time()
                found_packet = False
                while True:
                    if self.ser.in_waiting > 3:
                        found_packet = True
                        break
                    elif (self._bulk_timeout is not None) and (time.time() - t_bus_init > self._bulk_timeout):
                        print("[PyBEAR | WARNING] :: BULK_COMM response timed out. Re-sending the same packet.")
                        break

                if found_packet:
                    status_packet = []
                    status_packet.extend(self.ser.read(4))
                    if status_packet:

                        retlen = self.ord_adapt(status_packet[3])  # length of the data in a single packet
                        totlen = retlen + (4 + retlen) * (num_motors - 1)  # total length of all rest data coming back

                        found_data = True
                        while self.ser.in_waiting < totlen:
                            if self._bulk_timeout is None:
                                pass
                            elif time.time() - t_bus_init > self._bulk_timeout:

                                print("[PyBEAR | WARNING] :: BULK_COMM return packet timed out. Retrying BULK_COMM...")
                                found_data = False
                                break

                        if found_data:
                            # All BEARs returned
                            status_packet.extend(self.ser.read(totlen))

                            status_packet = [status_packet[j:j+retlen+4]
                                             for j in range(0, len(status_packet), retlen+4)]
                            bear_rtn = []
                            err_id = []
                            for idx, packet in enumerate(status_packet):
                                # Check for checksum
                                checksum = ctypes.c_ubyte(~(ctypes.c_ubyte(sum(packet[2:-1])).value))
                                if checksum.value == packet[-1]:
                                    bear_rtn.append([self.__hex_to_float32([self.ord_adapt(idx)
                                                     for idx in packet[5:-1]]), packet[4]])
                                else:
                                    err_id.append(m_ids[idx])
                                    bear_rtn.append([None, -99])
                            if len(err_id):
                                print("[PyBEAR | WARNING] :: Return data corrupted from these BEARs:", err_id)
                                if error_mode == 1:
                                    return None
                            else:
                                return bear_rtn


        # Communication has been attempted but has failed!

        if found_packet:
            # Not all BEARs returned
            print("[PyBEAR | ERROR] :: BULK_COMM only get partial return.")
        #     buffer_length = self.ser.in_waiting
        #     rtn_count = int((buffer_length+4) / (4 + retlen))
        #     status_packet.extend(self.ser.read(rtn_count*(4 + retlen)-4))
        #     self.ser.reset_input_buffer()
        #     no_rtn_id = m_ids[rtn_count:]
        #     print("[PyBEAR | ERROR] :: No reply from these BEARs:", no_rtn_id)
        else:
            # No return at all
            print("[PyBEAR | ERROR] :: BULK_COMM no return.")
        #     print("No return")

        return None

    def bulk_comm(self, m_ids, read_registers, write_registers, write_data, error_mode=0):
        """
        Read and write from multiple motors in a single shot
        CAUTION: Will not work for: ID, Mode, Baudrate, Watchdog_timeout and Torque Enable

        Parameters
        ----------
        m_ids
            tuple of IDs
        read_registers
            tuple of regirsters to read
        write_registers
            tuple of regirsters to write to
        write_data
            tuple of data to write
        error_mode
            0(default): return[None, -99] for BEAR with corrupted data;
            1: return None as long as there is any error
        """
        read_addr_list = []
        write_addr_list = []
        if len(read_registers) != 0:
            for read_add in read_registers:
                read_addr_list.append(STAT_REG_DIC[read_add])

        if len(write_registers) != 0:
            for write_add in write_registers:
                write_addr_list.append(STAT_REG_DIC[write_add])

        return self.__bulk_communication(m_ids, read_addr_list, write_addr_list, write_data, error_mode)

    def read_cfg_data(self, m_id, address):
        """
        This command is to read data from the configuration registers.
        """
        if address not in CFG_REG.UINT_REG:
            return self.__read_data(m_id, address, reg_type='cfg', data_type='f32')
        else:
            return self.__read_data(m_id, address, reg_type='cfg', data_type='u32')

    def read_bulk_cfg_data(self, argv):
        """
        Read multiple configuration registers from a single motor in one packet.
        """
        m_id = argv[0]
        addr_list = []
        for idx in range(1, len(argv)):
            addr_list.append(CFG_REG_DIC[argv[idx]])
        return self.__read_bulk_data(m_id, addr_list, reg_type='cfg', data_type='f32')

    def read_status_data(self, m_id, add_list):
        """
        This command is to read data from the status registers.
        """
        if add_list > 1:
            return self.__read_data(m_id, add_list, reg_type='stat', data_type='f32')
        else:
            return self.__read_data(m_id, add_list, reg_type='stat', data_type='u32')

    def read_bulk_status_data(self, argv):
        """
        Read multiple status registers from a single motor in one packet.
        """
        m_id = argv[0]
        addr_list = []
        for idx in range(1, len(argv)):
            addr_list.append(STAT_REG_DIC[argv[idx]])
        return self.__read_bulk_data(m_id, addr_list, reg_type='stat', data_type='f32')

    def _ping(self, m_id):
        """
        Function used to detect a motor. Will return firmware and hardware version info.
        Return None if pinging unsuccessful.
        """
        # Type of instruction
        instruction = INSTRUCTION.PING

        # Number of parameters
        pkt_len = 2

        # Create checksum
        checksum = self.chksum(m_id, pkt_len, instruction, (0,))

        # Write packet
        self.__write_packet((0xFF, 0xFF, m_id, pkt_len, instruction, checksum))
        start_time = time.time()
        while self.ser.in_waiting < 1:
            if time.time() - start_time > PING_TIMEOUT:
                # Timeout
                return None
            else:
                pass

        # Read status
        ping_status, error_code = self.__read_packet(m_id)

        return ping_status, error_code

    def save_config(self, m_id):
        instruction = INSTRUCTION.SAVE_CFG

        n_add = 2

        checksum = self.chksum(m_id, n_add, instruction, (0,))

        self.__write_packet((0xFF, 0xFF, m_id, n_add, instruction, checksum))

    # ===== Utility functions
    def __packet_generator(self, m_id, length, instruction, param_n, data, checksum):
        # if isinstance(param_n, list) and len(param_n) > 1:
        if isinstance(param_n, list):
            if data is None:
                l = tuple([0xFF, 0xFF, m_id, length, instruction] + param_n + [checksum])
            else:
                l = tuple([0xFF, 0xFF, m_id, length, instruction] + param_n + data + [checksum])
        else:
            if data is None:
                l = (0xFF, 0xFF, m_id, length, instruction, param_n, checksum)
            else:
                l = (0xFF, 0xFF, m_id, length, instruction, param_n, data, checksum)
        # TODO: Clean up the sequence below
        return tuple((chain(*(i if isinstance(i, tuple) else (i,) for i in l))))
        # return (0xFF, 0xFF, m_id, length, instruction, param_n, data, checksum)

    def __packet_generator_bulk(self, m_ids, length, instruction, num_regs, read_addrs, write_addrs, data, checksum):
        if data is None:
            l = tuple(chain([0xFF, 0xFF, 0xFE, length, instruction, len(m_ids), num_regs],
                            read_addrs, m_ids, [checksum]))
        else:
            l = tuple(chain([0xFF, 0xFF, 0xFE, length, instruction, len(m_ids), num_regs],
                            read_addrs, write_addrs, data, [checksum]))
        # return tuple((chain(*(i if isinstance(i, tuple) else (i,) for i in l))))
        return l

    def __float32_to_hex(self, val):
        retval = hex(struct.unpack('<I', struct.pack('>f', val))[0])
        if retval[-1] == 'L':
            retval = retval[:-1]
        if len(retval) < 10:
            length = 10-len(retval)
            retval = '0x'+length*'0'+retval[2:]
        return retval

    def __int32_to_hex(self, val):
        retval = hex(struct.unpack('<I', struct.pack('>I', val))[0])
        if len(retval) < 10:
            length = 10-len(retval)
            retval = '0x'+length*'0'+retval[2:]
        return retval

    def __hex_to_float32(self, val):
        # if len(val) > 4:
        #     tmpval = []
        #     for idx in range(0, len(val), 4):
        #         tmpval.append(struct.unpack('<f', self.sustr_loop_adapt(idx, val))[0])
        # else:
        #     tmpval = struct.unpack('<f', self.sustr_adapt(val))[0]
        tmpval = []
        for idx in range(0, len(val), 4):
            tmpval.append(struct.unpack('<f', self.sustr_loop_adapt(idx, val))[0])
        return tmpval

    def __hex_to_int32(self, val):
        if len(val) > 4:
            tmpval = []
            for idx in range(0, len(val), 4):
                tmpval.append(val[idx]|val[idx+1]<<8|val[idx+2]<<16|val[idx+3]<<24)
        else:
            tmpval = val[0]|val[1]<<8|val[2]<<16|val[3]<<24
        return tmpval

    def chksum(self, m_id, length, instruction, param_n):
        return 255 - ((m_id + length + instruction + sum(param_n)) % 256) 
      uses: #!usr/bin/env python

################################################################################
# Copyright 2020 Westwood Robotics Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

__author__ = "Westwood Robotics Corporation"
__email__ = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Jan. 01, 2020"

__version__ = "0.1.2"
__status__ = "Production"

TIMEOUT_DEFAULT = 0.001  # In seconds
BULK_TIMEOUT_DEFAULT = 0.001  # In seconds
PING_TIMEOUT = 2*TIMEOUT_DEFAULT
BULK_COMM_RETRIES = 3  # set this number to change how many times PyBEAR will try to resend the packet

#
# TIMEOUT_MAX = 0.000125 # In seconds 

          #------------------------------------------POLARIS------------------------------------------

    - name: 
################################################################################
# Copyright 2020 Westwood Robotics Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################ 
      id: ################################################################################
# Copyright 2020 Westwood Robotics Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################ 
      uses: # See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.191.1/containers/typescript-node/.devcontainer/base.Dockerfile

# [Choice] Node.js version: 16, 14, 12
ARG VARIANT="16-buster"
FROM mcr.microsoft.com/vscode/devcontainers/typescript-node:0-${VARIANT}

# [Optional] Uncomment this section to install additional OS packages.
RUN apt-get update \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        openjdk-11-jdk \
        firefox-esr

# Chromium and chrome-driver
ARG CHROMIUM_DEB_URL=https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
RUN wget -qO - $CHROMIUM_DEB_URL > /tmp/chrome_linux64.deb \
    && apt -y install /tmp/chrome_linux64.deb -f \
    && FULL_CHROME_VERSION=$(google-chrome --product-version) \
    && CHROME_VERSION=${FULL_CHROME_VERSION%.*} \
    && CHROMEDRIVER_DIR="/usr/local/share/chrome_driver" \
    && CHROMEDRIVER_BIN="$CHROMEDRIVER_DIR/chromedriver" \
    && LATEST_CHROMEDRIVER_VERSION=$(curl -sL "https://chromedriver.storage.googleapis.com/LATEST_RELEASE_$CHROME_VERSION") \
    && CHROMEDRIVER_URL="https://chromedriver.storage.googleapis.com/$LATEST_CHROMEDRIVER_VERSION/chromedriver_linux64.zip" \
    && wget -qO - $CHROMEDRIVER_URL > /tmp/chromedriver_linux64.zip \
    && mkdir -p $CHROMEDRIVER_DIR \
    && unzip -qq /tmp/chromedriver_linux64.zip -d $CHROMEDRIVER_DIR \
    && chmod +x $CHROMEDRIVER_BIN \
    && ln -s "$CHROMEDRIVER_BIN" /usr/bin/ \
    && rm -rf /tmp/chrome*

# [Optional] Uncomment if you want to install an additional version of node using nvm
# ARG EXTRA_NODE_VERSION=10
# RUN su node -c "source /usr/local/share/nvm/nvm.sh && nvm install ${EXTRA_NODE_VERSION}"

# [Optional] Uncomment if you want to install more global node packages
# RUN su node -c "npm install -g <your-package-list -here>"
      with: // For format details, see https://aka.ms/devcontainer.json. For config options, see the README at:
// https://github.com/microsoft/vscode-dev-containers/tree/v0.191.1/containers/typescript-node
{
  "name": "Node.js & TypeScript",
  "build": {
    "dockerfile": "Dockerfile",
    // Update 'VARIANT' to pick a Node version: 12, 14, 16
    "args": {
      "VARIANT": "16"
    }
  },
  // Set *default* container specific settings.json values on container create.
  "settings": {},
  // Add the IDs of extensions you want installed when the container is created.
  "extensions": [
    "dbaeumer.vscode-eslint"
  ],
  // Use 'forwardPorts' to make a list of ports inside the container available locally.
  // "forwardPorts": [],
  // Use 'postCreateCommand' to run commands after the container is created.
  "postCreateCommand": "yarn install && yarn build",
  // Comment out connect as root instead. More info: https://aka.ms/vscode-remote/containers/non-root.
  "remoteUser": "node"
}
        ioServerUrl: #!/usr/bin/env bash
set -eux

DEV_BUILD_REPO_NAME="hotwired/dev-builds"
DEV_BUILD_ORIGIN_URL="https://${1}@github.com/${DEV_BUILD_REPO_NAME}.git"
BUILD_PATH="$HOME/publish-dev-build"

mkdir "$BUILD_PATH"

cd "$GITHUB_WORKSPACE"
package_name="$(jq -r .name package.json)"
package_files=( dist package.json )
tag="${package_name}/${GITHUB_SHA:0:7}"

name="$(git log -n 1 --format=format:%cn)"
email="$(git log -n 1 --format=format:%ce)"
subject="$(git log -n 1 --format=format:%s)"
date="$(git log -n 1 --format=format:%ai)"
url="https://github.com/${GITHUB_REPOSITORY}/tree/${GITHUB_SHA}"
message="$tag $subject"$'\n\n'"$url"

cp -R "${package_files[@]}" "$BUILD_PATH"

cd "$BUILD_PATH"
git init .
git remote add origin "$DEV_BUILD_ORIGIN_URL"
git symbolic-ref HEAD refs/heads/publish-dev-build
git add "${package_files[@]}"

GIT_AUTHOR_DATE="$date" GIT_COMMITTER_DATE="$date" \
GIT_AUTHOR_NAME="$name" GIT_COMMITTER_NAME="$name" \
GIT_AUTHOR_EMAIL="$email" GIT_COMMITTER_EMAIL="$email" \
  git commit -m "$message"

git tag "$tag"
[ "$GITHUB_REF" != "refs/heads/main" ] || git tag -f "${package_name}/latest"
git push -f --tags

echo done
    runs-on: name: CI

on: [push, pull_request]

jobs:
  build:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v3
    - uses: actions/setup-node@v3
      with:
        node-version: '16'
        cache: 'yarn'
    - run: yarn install --frozen-lockfile
    - run: yarn run playwright install --with-deps
    - run: yarn build

    - name: Set Chrome Version
      run: |
        CHROMEVER="$(chromedriver --version | cut -d' ' -f2)"
        echo "Actions ChromeDriver is $CHROMEVER"
        echo "CHROMEVER=${CHROMEVER}" >> $GITHUB_ENV

    - name: Lint
      run: yarn lint

    - name: Unit Test
      run: yarn test:unit

    - name: Chrome Test
      run: yarn test:browser --project=chrome

    - name: Firefox Test
      run: yarn test:browser --project=firefox

    - uses: actions/upload-artifact@v3
      with:
        name: turbo-dist
        path: dist/*
    permissions: name: dev-builds

on:
  workflow_dispatch:
  push:
    branches:
      - main
      - 'builds/**'

jobs: 
  // Use IntelliSense to learn about possible attributes.
  // Hover to view descriptions of existing attributes.
  // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
  "version": "0.2.0",
  "configurations": [
    {
      "type": "node",
      "request": "launch",
      "name": "Turbo: Debug browser tests",
      "cwd": "${workspaceFolder}",
      "port": 9229,
      "outputCapture": "std",
      "internalConsoleOptions": "openOnSessionStart",
      "runtimeExecutable": "yarn",
      "runtimeArgs": ["test"]
    }
  ]
}
  build: // See https://go.microsoft.com/fwlink/?LinkId=733558
  // for the documentation about the tasks.json format
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Turbo: Build dist directory",
      "type": "shell",
      "command": "yarn build",
      "group": {
        "kind": "build",
        "isDefault": true
      }
    },
    {
      "label": "Turbo: Run tests",
      "type": "shell",
      "dependsOn": "Turbo: Build dist directory",
      "command": "yarn test",
      "group": {
        "kind": "test",
        "isDefault": true
      }
    },
    {
      "label": "Turbo: Start dev server",
      "type": "shell",
      "dependsOn": "Turbo: Build dist directory",
      "command": "yarn start",
      "problemMatcher": []
    }
  ]
}
    runs-on: "Akrapovič"
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: '16'
          cache: 'yarn'

      - run: yarn install --frozen-lockfile
      - run: yarn build

      - name: Publish dev build
        run: .github/scripts/publish-dev-build '${{ secrets.DEV_BUILD_GITHUB_TOKEN }}'
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
      uses: github/codeql-action/upload-sarif@v2
      with:
        # Path to SARIF file relative to the root of the repository
        sarif_file: workflowengine-results.sarif.json
