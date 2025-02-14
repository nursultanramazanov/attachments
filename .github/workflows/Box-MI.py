# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: version: 2.1

executors:
  default:
    docker:
      - image: cimg/node:18.18
        user: root
    resource_class: large
    working_directory: ~/buie
  cypress-executor:
    docker:
      - image: cypress/included:13.13.0
        user: root
    resource_class: large
    working_directory: ~/buie

commands:
  yarn-install:
    steps:
      - run:
          name: Installing dependencies
          command: yarn install --non-interactive --frozen-lockfile --cache-folder /node_modules

  build-locales:
    steps:
      - run:
          name: Building locales
          command: yarn --cwd /buie build:i18n

  build-project:
    steps:
      - run:
          name: Building project
          command: yarn --cwd /buie build:prod:npm

  restore-yarn-cache:
    steps:
      - restore_cache:
          keys:
            - yarn-cache-v5-{{ checksum "yarn.lock" }}

  save-yarn-cache:
    steps:
      - save_cache:
          key: yarn-cache-v5-{{ checksum "yarn.lock" }}
          paths:
            - ./node_modules

  setup-workspace:
    steps:
      - attach_workspace:
          at: /buie

jobs:
  setup:
    executor: default
    steps:
      - checkout
      - restore-yarn-cache
      - yarn-install
      - save-yarn-cache
      - persist_to_workspace:
          root: .
          paths:
            - .

  lint:
    executor: default
    steps:
      - checkout
      - setup-workspace
      - build-locales
      - run:
          name: Commit lint
          command: /buie/scripts/commitlint.sh
      - run:
          name: Code lint
          command: yarn --cwd /buie lint

  flow:
    executor: default
    steps:
      - checkout
      - setup-workspace
      - run:
          name: Flow
          command: yarn --cwd /buie flow check

  build-unit-tests:
    executor: default
    steps:
      - checkout
      - setup-workspace
      - build-locales
      - run:
          name: Babel build
          command: yarn --cwd /buie build:ci:es
      - run:
          name: Checking locales and styles
          command: /buie/scripts/check_generated_files.sh
      - run:
          name: Webpack build
          command: yarn --cwd /buie build:ci:dist
      - run: echo 'export TZ=America/Los_Angeles' >> $BASH_ENV
      - run:
          name: Unit tests
          command: yarn --cwd /buie test --maxWorkers=3

  e2e-tests:
   executor: cypress-executor
   steps:
     - checkout
     - setup-workspace
     - run:
         name: Cypress run
         command: yarn --cwd /buie test:e2e

  chromatic-deploy:
    executor: default
    steps:
      - checkout
      - setup-workspace
      - build-locales
      - build-project
      - run:
          name: Chromatic
          command: yarn --cwd /buie chromatic

workflows:
  version: 2
  lint_test_build:
    jobs:
      - setup
      - lint:
          requires:
            - setup
      - flow:
          requires:
            - setup
      - build-unit-tests:
          requires:
            - setup
      - e2e-tests:
          requires:
            - setup
      - chromatic-deploy:
          requires:
            - setup 
  push: What:		/proc/sys/vm/nr_pdflush_threads
Date:		June 2012
Contact:	Wanpeng Li <liwp@linux.vnet.ibm.com>
Description: Since pdflush is replaced by per-BDI flusher, the interface of old pdflush
             exported in /proc/sys/vm/ should be removed. 
    branches: [ "main" ]
  pull_request: What:		/sys/bus/usb/devices/.../power/level
Date:		March 2007
KernelVersion:	2.6.21
Contact:	Alan Stern <stern@rowland.harvard.edu>
Description:
		Each USB device directory will contain a file named
		power/level.  This file holds a power-level setting for
		the device, either "on" or "auto".

		"on" means that the device is not allowed to autosuspend,
		although normal suspends for system sleep will still
		be honored.  "auto" means the device will autosuspend
		and autoresume in the usual manner, according to the
		capabilities of its driver.

		During normal use, devices should be left in the "auto"
		level.  The "on" level is meant for administrative uses.
		If you want to suspend a device immediately but leave it
		free to wake up in response to I/O requests, you should
		write "0" to power/autosuspend.

		Device not capable of proper suspend and resume should be
		left in the "on" level.  Although the USB spec requires
		devices to support suspend/resume, many of them do not.
		In fact so many don't that by default, the USB core
		initializes all non-hub devices in the "on" level.  Some
		drivers may change this setting when they are bound.

		This file is deprecated and will be removed after 2010.
		Use the power/control file instead; it does exactly the
		same thing. 
    branches: [ "main" ]
  schedule: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/actual_profile
Date:		Januar 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 1-5.
		When read, this attribute returns the number of the actual
		profile which is also the profile that's active on device startup.
		When written this attribute activates the selected profile
		immediately.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/button
Date:		Januar 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The keyboard can store short macros with consist of 1 button with
		several modifier keys internally.
		When written, this file lets one set the sequence for a specific
		button for a specific profile. Button and profile numbers are
		included in written data. The data has to be 24 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/info
Date:		Januar 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns some info about the device like the
		installed firmware version.
		The size of the data is 8 bytes in size.
		This file is readonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/key_mask
Date:		Januar 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The keyboard lets the user deactivate 5 certain keys like the
		windows and application keys, to protect the user from the outcome
		of accidentally pressing them.
		The integer value of this attribute has bits 0-4 set depending
		on the state of the corresponding key.
		When read, this file returns the current state of the buttons.
		When written, the given buttons are activated/deactivated
		immediately.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/mode_key
Date:		Januar 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The keyboard has a condensed layout without num-lock key.
		Instead it uses a mode-key which activates a gaming mode where
		the assignment of the number block changes.
		The integer value of this attribute ranges from 0 (OFF) to 1 (ON).
		When read, this file returns the actual state of the key.
		When written, the key is activated/deactivated immediately.
Users:		http://roccat.sourceforge.net 
    - cron: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/actual_profile
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 0-4.
		When read, this attribute returns the number of the actual
		profile. This value is persistent, so its equivalent to the
		profile that's active when the device is powered on next time.
		When written, this file sets the number of the startup profile
		and the device activates this profile immediately.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/info
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns general data like firmware version.
		The data is 6 bytes long.
		This file is readonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/key_mask
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one deactivate certain keys like
		windows and application keys, to prevent accidental presses.
		Profile number for which this settings occur is included in
		written data. The data has to be 6 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_capslock
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the
		capslock key for a specific profile. Profile number is included
		in written data. The data has to be 6 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_easyzone
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the
		easyzone keys for a specific profile. Profile number is included
		in written data. The data has to be 65 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_function
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the
		function keys for a specific profile. Profile number is included
		in written data. The data has to be 41 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_macro
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the macro
		keys for a specific profile. Profile number is included in
		written data. The data has to be 35 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_media
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the media
		keys for a specific profile. Profile number is included in
		written data. The data has to be 29 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_thumbster
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the
		thumbster keys for a specific profile. Profile number is included
		in written data. The data has to be 23 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/last_set
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the time in secs since
		epoch in which the last configuration took place.
		The data has to be 20 bytes long.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/light
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the backlight intensity for
		a specific profile. Profile number is included in written data.
		The data has to be 10 bytes long for Isku, IskuFX needs	16 bytes
		of data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/macro
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one store macros with max 500
		keystrokes for a specific button for a specific profile.
		Button and profile numbers are included in written data.
		The data has to be 2083 bytes long.
		Before reading this file, control has to be written to select
		which profile and key to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/reset
Date:		November 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one reset the device.
		The data has to be 3 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/control
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one select which data from which
		profile will be	read next. The data has to be 3 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/talk
Date:		June 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one trigger easyshift functionality
		from the host.
		The data has to be 16 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/talkfx
Date:		February 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one trigger temporary color schemes
		from the host.
		The data has to be 16 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net 

env: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/actual_profile
Date:		October 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the actual
                profile. This value is persistent, so its equivalent to the
                profile that's active when the mouse is powered on next time.
		When written, this file sets the number of the startup profile
		and the mouse activates this profile immediately.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/startup_profile
Date:		October 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the actual
                profile. This value is persistent, so its equivalent to the
                profile that's active when the mouse is powered on next time.
		When written, this file sets the number of the startup profile
		and the mouse activates this profile immediately.
		Please use actual_profile, it does the same thing.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/firmware_version
Date:		October 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns the raw integer version number of the
		firmware reported by the mouse. Using the integer value eases
		further usage in other programs. To receive the real version
		number the decimal point has to be shifted 2 positions to the
		left. E.g. a returned value of 121 means 1.21
		This file is readonly.
		Please read binary attribute info which contains firmware version.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/info
Date:		November 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns general data like firmware version.
		When written, the device can be reset.
		The data is 8 bytes long.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/macro
Date:		October 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store a macro with max 500 key/button strokes
		internally.
		When written, this file lets one set the sequence for a specific
		button for a specific profile. Button and profile numbers are
		included in written data. The data has to be 2082 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/profile_buttons
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_buttons holds information about button layout.
		When written, this file lets one write the respective profile
		buttons back to the mouse. The data has to be 77 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/profile[1-5]_buttons
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_buttons holds information about button layout.
		When read, these files return the respective profile buttons.
		The returned data is 77 bytes in size.
		This file is readonly.
		Write control to select profile and read profile_buttons instead.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/profile_settings
Date:		October 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_settings holds information like resolution, sensitivity
		and light effects.
		When written, this file lets one write the respective profile
		settings back to the mouse. The data has to be 43 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/profile[1-5]_settings
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_settings holds information like resolution, sensitivity
		and light effects.
		When read, these files return the respective profile settings.
		The returned data is 43 bytes in size.
		This file is readonly.
		Write control to select profile and read profile_settings instead.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/sensor
Date:		October 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse has a tracking- and a distance-control-unit. These
		can be activated/deactivated and the lift-off distance can be
		set. The data has to be 6 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/talk
Date:		May 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	Used to active some easy* functions of the mouse from outside.
		The data has to be 16 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/tcu
Date:		October 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written a calibration process for the tracking control unit
		can be initiated/cancelled. Also lets one read/write sensor
		registers.
		The data has to be 4 bytes long.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/tcu_image
Date:		October 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read the mouse returns a 30x30 pixel image of the
		sampled underground. This works only in the course of a
		calibration process initiated with tcu.
		The returned data is 1028 bytes in size.
		This file is readonly.
Users:		http://roccat.sourceforge.net 
  # Path to the CMake build directory.
  build: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/actual_profile
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. actual_profile holds number of actual profile.
		This value is persistent, so its value determines the profile
		that's active when the mouse is powered on next time.
		When written, the mouse activates the set profile immediately.
		The data has to be 3 bytes long.
		The mouse will reject invalid data.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/control
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one select which data from which
		profile will be	read next. The data has to be 3 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/info
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns general data like firmware version.
		When written, the device can be reset.
		The data is 6 bytes long.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/macro
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store a macro with max 500 key/button strokes
		internally.
		When written, this file lets one set the sequence for a specific
		button for a specific profile. Button and profile numbers are
		included in written data. The data has to be 2082 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/profile_buttons
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_buttons holds information about button layout.
		When written, this file lets one write the respective profile
		buttons back to the mouse. The data has to be 59 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/profile_settings
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_settings holds information like resolution, sensitivity
		and light effects.
		When written, this file lets one write the respective profile
		settings back to the mouse. The data has to be 31 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/sensor
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse has a tracking- and a distance-control-unit. These
		can be activated/deactivated and the lift-off distance can be
		set. The data has to be 6 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/talk
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	Used to active some easy* functions of the mouse from outside.
		The data has to be 16 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/tcu
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written a calibration process for the tracking control unit
		can be initiated/cancelled. Also lets one read/write sensor
		registers.
		The data has to be 4 bytes long.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/tcu_image
Date:		December 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read the mouse returns a 30x30 pixel image of the
		sampled underground. This works only in the course of a
		calibration process initiated with tcu.
		The returned data is 1028 bytes in size.
		This file is readonly.
Users:		http://roccat.sourceforge.net 

permissions: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_cpi
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 1-4.
		When read, this attribute returns the number of the active
		cpi level.
		This file is readonly.
		Has never been used. If bookkeeping is done, it's done in userland tools.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_profile
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 0-4.
		When read, this attribute returns the number of the active
		profile.
		When written, the mouse activates this profile immediately.
		The profile that's active when powered down is the same that's
		active when the mouse is powered on.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_sensitivity_x
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 1-10.
		When read, this attribute returns the number of the actual
		sensitivity in x direction.
		This file is readonly.
		Has never been used. If bookkeeping is done, it's done in userland tools.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_sensitivity_y
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 1-10.
		When read, this attribute returns the number of the actual
		sensitivity in y direction.
		This file is readonly.
		Has never been used. If bookkeeping is done, it's done in userland tools.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/firmware_version
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns the raw integer version number of the
		firmware reported by the mouse. Using the integer value eases
		further usage in other programs. To receive the real version
		number the decimal point has to be shifted 2 positions to the
		left. E.g. a returned value of 121 means 1.21
		This file is readonly.
		Obsoleted by binary sysfs attribute "info".
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/info
Date:		November 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns general data like firmware version.
		When written, the device can be reset.
		The data is 6 bytes long.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/profile_buttons
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_buttons holds information about button layout.
		When written, this file lets one write the respective profile
		buttons back to the mouse. The data has to be 23 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/profile[1-5]_buttons
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_buttons holds information about button layout.
		When read, these files return the respective profile buttons.
		The returned data is 23 bytes in size.
		This file is readonly.
		Write control to select profile and read profile_buttons instead.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/profile_settings
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_settings holds information like resolution, sensitivity
		and light effects.
		When written, this file lets one write the respective profile
		settings back to the mouse. The data has to be 16 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/profile[1-5]_settings
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_settings holds information like resolution, sensitivity
		and light effects.
		When read, these files return the respective profile settings.
		The returned data is 16 bytes in size.
		This file is readonly.
		Write control to select profile and read profile_settings instead.
Users:		http://roccat.sourceforge.net 
  contents: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/control
Date:		October 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, cpi, button and light settings can be configured.
		When read, actual cpi setting and sensor data are returned.
		The data has to be 8 bytes long.
Users:		http://roccat.sourceforge.net 

jobs: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/actual_cpi
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	It is possible to switch the cpi setting of the mouse with the
		press of a button.
		When read, this file returns the raw number of the actual cpi
		setting reported by the mouse. This number has to be further
		processed to receive the real dpi value.

		VALUE DPI
		1     400
		2     800
		4     1600

		This file is readonly.
		Has never been used. If bookkeeping is done, it's done in userland tools.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/actual_profile
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns the number of the actual profile in
		range 0-4.
		This file is readonly.
		Please use binary attribute "settings" which provides this information.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/firmware_version
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns the raw integer version number of the
		firmware reported by the mouse. Using the integer value eases
		further usage in other programs. To receive the real version
		number the decimal point has to be shifted 2 positions to the
		left. E.g. a returned value of 138 means 1.38
		This file is readonly.
		Please use binary attribute "info" which provides this information.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/info
Date:		November 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns general data like firmware version.
		When written, the device can be reset.
		The data is 6 bytes long.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/profile_buttons
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_buttons holds information about button layout.
		When written, this file lets one write the respective profile
		buttons back to the mouse. The data has to be 19 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/profile[1-5]_buttons
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_buttons holds information about button layout.
		When read, these files return the respective profile buttons.
		The returned data is 19 bytes in size.
		This file is readonly.
		Write control to select profile and read profile_buttons instead.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/profile_settings
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_settings holds information like resolution, sensitivity
		and light effects.
		When written, this file lets one write the respective profile
		settings back to the mouse. The data has to be 13 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/profile[1-5]_settings
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split in settings and buttons.
		profile_settings holds information like resolution, sensitivity
		and light effects.
		When read, these files return the respective profile settings.
		The returned data is 13 bytes in size.
		This file is readonly.
		Write control to select profile and read profile_settings instead.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/settings
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns the settings stored in the mouse.
		The size of the data is 3 bytes and holds information on the
		startup_profile.
		When written, this file lets write settings back to the mouse.
		The data has to be 3 bytes long. The mouse will reject invalid
		data.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/startup_profile
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the profile
                that's active when the mouse is powered on.
		This file is readonly.
		Please use binary attribute "settings" which provides this information.
Users:		http://roccat.sourceforge.net 
  analyze: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/control
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one select which data from which
		profile will be	read next. The data has to be 3 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/profile
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. profile holds index of actual profile.
		This value is persistent, so its value determines the profile
		that's active when the device is powered on next time.
		When written, the device activates the set profile immediately.
		The data has to be 3 bytes long.
		The device will reject invalid data.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_primary
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the default of all keys for
		a specific profile. Profile index is included in written data.
		The data has to be 125 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_function
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the
		function keys for a specific profile. Profile index is included
		in written data. The data has to be 95 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_macro
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the macro
		keys for a specific profile. Profile index is included in
		written data. The data has to be 35 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_thumbster
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the
		thumbster keys for a specific profile. Profile index is included
		in written data. The data has to be 23 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_extra
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the
		capslock and function keys for a specific profile. Profile index
		is included in written data. The data has to be 8 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_easyzone
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the function of the
		easyzone keys for a specific profile. Profile index is included
		in written data. The data has to be 294 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/key_mask
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one deactivate certain keys like
		windows and application keys, to prevent accidental presses.
		Profile index for which this settings occur is included in
		written data. The data has to be 6 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/light
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the backlight intensity for
		a specific profile. Profile index is included in written data.
		This attribute is only valid for the glow and pro variant.
		The data has to be 16 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/macro
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one store macros with max 480
		keystrokes for a specific button for a specific profile.
		Button and profile indexes are included in written data.
		The data has to be 2002 bytes long.
		Before reading this file, control has to be written to select
		which profile and key to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/info
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns general data like firmware version.
		The data is 8 bytes long.
		This file is readonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/reset
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one reset the device.
		The data has to be 3 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/talk
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one trigger easyshift functionality
		from the host.
		The data has to be 16 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/light_control
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one switch between stored and custom
		light settings.
		This attribute is only valid for the pro variant.
		The data has to be 8 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/stored_lights
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set per-key lighting for different
		layers.
		This attribute is only valid for the pro variant.
		The data has to be 1382 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/custom_lights
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set the actual per-key lighting.
		This attribute is only valid for the pro variant.
		The data has to be 20 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/light_macro
Date:		October 2013
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one set a light macro that is looped
		whenever the device gets in dimness mode.
		This attribute is only valid for the pro variant.
		The data has to be 2002 bytes long.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net 
    permissions: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/buttons
Date:		Mai 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split into general settings and
		button settings. The buttons variable holds information about
		button layout. When written, this file lets one write the
		respective profile buttons to the mouse. The data has to be
		47 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		Before reading this file, control has to be written to select
		which profile to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/control
Date:		Mai 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one select which data from which
		profile will be	read next. The data has to be 3 bytes long.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/general
Date:		Mai 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. A profile is split into general settings and
		button settings. A profile holds information like resolution,
		sensitivity and light effects.
		When written, this file lets one write the respective profile
		settings back to the mouse. The data has to be 43 bytes long.
		The mouse will reject invalid data.
		Which profile to write is determined by the profile number
		contained in the data.
		This file is writeonly.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/info
Date:		Mai 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When read, this file returns general data like firmware version.
		When written, the device can be reset.
		The data is 8 bytes long.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/macro
Date:		Mai 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, this file lets one store macros with max 500
		keystrokes for a specific button for a specific profile.
		Button and profile numbers are included in written data.
		The data has to be 2083 bytes long.
		Before reading this file, control has to be written to select
		which profile and key to read.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/profile
Date:		Mai 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse can store 5 profiles which can be switched by the
		press of a button. profile holds number of actual profile.
		This value is persistent, so its value determines the profile
		that's active when the mouse is powered on next time.
		When written, the mouse activates the set profile immediately.
		The data has to be 3 bytes long.
		The mouse will reject invalid data.
Users:		http://roccat.sourceforge.net

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/sensor
Date:		July 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The mouse has a Avago ADNS-3090 sensor.
		This file allows reading and writing of the mouse sensors registers.
		The data has to be 4 bytes long.
Users:		http://roccat.sourceforge.net 
      contents: What:		/sys/firmware/acpi/hotplug/force_remove
Date:		Mar 2017
Contact:	Rafael J. Wysocki <rafael.j.wysocki@intel.com>
Description:
		Since the force_remove is inherently broken and dangerous to
		use for some hotplugable resources like memory (because ignoring
		the offline failure might lead to memory corruption and crashes)
		enabling this knob is not safe and thus unsupported. # for actions/checkout to fetch code
      security-events: What:		/sys/class/gpio/
Date:		July 2008
KernelVersion:	2.6.27
Contact:	Linus Walleij <linusw@kernel.org>
Description:

  As a Kconfig option, individual GPIO signals may be accessed from
  userspace.  GPIOs are only made available to userspace by an explicit
  "export" operation.  If a given GPIO is not claimed for use by
  kernel code, it may be exported by userspace (and unexported later).
  Kernel code may export it for complete or partial access.

  GPIOs are identified as they are inside the kernel, using integers in
  the range 0..INT_MAX.  See Documentation/gpio.txt for more information.

    /sys/class/gpio
	/export ... asks the kernel to export a GPIO to userspace
	/unexport ... to return a GPIO to the kernel
	/gpioN ... for each exported GPIO #N OR
	/<LINE-NAME> ... for a properly named GPIO line
	    /value ... always readable, writes fail for input GPIOs
	    /direction ... r/w as: in, out (default low); write: high, low
	    /edge ... r/w as: none, falling, rising, both
	/gpiochipN ... for each gpiochip; #N is its first GPIO
	    /base ... (r/o) same as N
	    /label ... (r/o) descriptive, not necessarily unique
	    /ngpio ... (r/o) number of GPIOs; numbered N to N + (ngpio - 1)

  This ABI is deprecated and will be removed after 2020. It is
  replaced with the GPIO character device. # for github/codeql-action/upload-sarif to upload SARIF results
      actions: What:		devfs
Date:		July 2005 (scheduled), finally removed in kernel v2.6.18
Contact:	Greg Kroah-Hartman <gregkh@linuxfoundation.org>
Description:
	devfs has been unmaintained for a number of years, has unfixable
	races, contains a naming policy within the kernel that is
	against the LSB, and can be replaced by using udev.
	The files fs/devfs/*, include/linux/devfs_fs*.h were removed,
	along with the assorted devfs function calls throughout the
	kernel tree.

Users: # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: What:		dv1394 (a.k.a. "OHCI-DV I/O support" for FireWire)
Date:		May 2010 (scheduled), finally removed in kernel v2.6.37
Contact:	linux1394-devel@lists.sourceforge.net
Description:
	/dev/dv1394/* were character device files, one for each FireWire
	controller and for NTSC and PAL respectively, from which DV data
	could be received by read() or transmitted by write().  A few
	ioctl()s allowed limited control.
	This special-purpose interface has been superseded by libraw1394 +
	libiec61883 which are functionally equivalent, support HDV, and
	transparently work on top of the newer firewire kernel drivers.

Users:
	ffmpeg/libavformat (if configured for DV1394) 
    runs-on: What:		ip_queue
Date:		finally removed in kernel v3.5.0
Contact:	Pablo Neira Ayuso <pablo@netfilter.org>
Description:
	ip_queue has been replaced by nfnetlink_queue which provides
	more advanced queueing mechanism to user-space. The ip_queue
	module was already announced to become obsolete years ago.

Users: 

    steps: What:		tcp_dma_copybreak sysctl
Date:		Removed in kernel v3.13
Contact:	Dan Williams <dan.j.williams@intel.com>
Description:
	Formerly the lower limit, in bytes, of the size of socket reads
	that will be offloaded to a DMA copy engine.  Removed due to
	coherency issues of the cpu potentially touching the buffers
	while dma is in flight. 
      - name: What:		/sys/o2cb symlink
Date:		May 2011
KernelVersion:	3.0
Contact:	ocfs2-devel@oss.oracle.com
Description:	This is a symlink: /sys/o2cb to /sys/fs/o2cb. The symlink is
		removed when new versions of ocfs2-tools which know to look
		in /sys/fs/o2cb are sufficiently prevalent. Don't code new
		software to look here, it should try /sys/fs/o2cb instead.
Users:		ocfs2-tools. It's sufficient to mail proposed changes to
		ocfs2-devel@oss.oracle.com. 
        uses: What:		raw1394 (a.k.a. "Raw IEEE1394 I/O support" for FireWire)
Date:		May 2010 (scheduled), finally removed in kernel v2.6.37
Contact:	linux1394-devel@lists.sourceforge.net
Description:
	/dev/raw1394 was a character device file that allowed low-level
	access to FireWire buses.  Its major drawbacks were its inability
	to implement sensible device security policies, and its low level
	of abstraction that required userspace clients to duplicate much
	of the kernel's ieee1394 core functionality.
	Replaced by /dev/fw*, i.e. the <linux/firewire-cdev.h> ABI of
	firewire-core.

Users:
	libraw1394 (works with firewire-cdev too, transparent to library ABI
	users) 

      - name: rfkill - radio frequency (RF) connector kill switch support

For details to this subsystem look at Documentation/rfkill.txt.

What:		/sys/class/rfkill/rfkill[0-9]+/claim
Date:		09-Jul-2007
KernelVersion	v2.6.22
Contact:	linux-wireless@vger.kernel.org
Description:	This file was deprecated because there no longer was a way to
		claim just control over a single rfkill instance.
		This file was scheduled to be removed in 2012, and was removed
		in 2016.
Values: 	0: Kernel handles events 
        run: What:		video1394 (a.k.a. "OHCI-1394 Video support" for FireWire)
Date:		May 2010 (scheduled), finally removed in kernel v2.6.37
Contact:	linux1394-devel@lists.sourceforge.net
Description:
	/dev/video1394/* were character device files, one for each FireWire
	controller, which were used for isochronous I/O.  It was added as an
	alternative to raw1394's isochronous I/O functionality which had
	performance issues in its first generation.  Any video1394 user had
	to use raw1394 + libraw1394 too because video1394 did not provide
	asynchronous I/O for device discovery and configuration.
	Replaced by /dev/fw*, i.e. the <linux/firewire-cdev.h> ABI of
	firewire-core.

Users:
	libdc1394 (works with firewire-cdev too, transparent to library ABI
	users) 

      # Build is not required unless generated source files are used
      # - name: .ad-author, .ad-city {
    font-style: italic;
    font-weight: bold;
    float: left;
    margin-right: 30px;
}
      #   run: .nofloat {
    float: none;
}
.nopaddings {
    padding: 0;
}
.main-menu {
    padding-top: 15px;
}
.footer-content {
    padding: 20px;
    text-align: center;
    border-top: 1px solid #e7e7e7;
}
.content {
    margin-bottom: 30px;
} 

      - name: .category-select {
    width: 100%;
}
.first-level,
.second-level,
.third-level {
    vertical-align: top;
    box-sizing: border-box;
    -moz-box-sizing: border-box;
    width: 33%;
    padding: 0px 15px;
}
.first-level,
.second-level {
    border-right: 1px solid #C9E0ED;
}
.category-select li {
    margin-bottom: 5px;
}
#id_region, #id_city {
    width: 150px;
} 
        uses: .form-inline .form-control {
    width: 100%;
}
input, select {
    margin-bottom: 5px;
}
.roots h3 {
    font-size: 1.25em;
    font-weight: bold;
    margin: 10px 0;
}
.roots {
    padding: 15px 0 20px;
}
.child {
    margin: 5px 0;
}
.children {
    border-top: 1px solid #ccc;
    padding: 15px 0;
    display: none;
}
/*************************************/
.new-ads h3 {
    font-size: 1em;
    margin: 10px 0 2px;
}
.new-ads h4 {
    margin: 5px 0 0;
}
.preview {
    width: 172px;
    float: left;
    margin: 8px;
}

.preview img {
    min-width: 160px;
    min-height: 120px;
}
 
        # Provide a unique ID to access the sarif output path
        id: .navbar-default {
    background-color: #337ab7;
    border-color: #216cac;
}
.navbar-default .navbar-brand {
    color: #ecf0f1;
}
.navbar-default .navbar-brand:hover, .navbar-default .navbar-brand:focus {
    color: #ffffff;
}
.navbar-default .navbar-text {
    color: #ecf0f1;
}
.navbar-default .navbar-nav > li > a {
    color: #ecf0f1;
}
.navbar-default .navbar-nav > li > a:hover, .navbar-default .navbar-nav > li > a:focus {
    color: #ffffff;
}
.navbar-default .navbar-nav > .active > a, .navbar-default .navbar-nav > .active > a:hover, .navbar-default .navbar-nav > .active > a:focus {
    color: #ffffff;
    background-color: #216cac;
}
.navbar-default .navbar-nav > .open > a, .navbar-default .navbar-nav > .open > a:hover, .navbar-default .navbar-nav > .open > a:focus {
    color: #ffffff;
    background-color: #216cac;
}
.navbar-default .navbar-toggle {
    border-color: #216cac;
}
.navbar-default .navbar-toggle:hover, .navbar-default .navbar-toggle:focus {
    background-color: #216cac;
}
.navbar-default .navbar-toggle .icon-bar {
    background-color: #ecf0f1;
}
.navbar-default .navbar-collapse,
.navbar-default .navbar-form {
    border-color: #ecf0f1;
}
.navbar-default .navbar-link {
    color: #ecf0f1;
}
.navbar-default .navbar-link:hover {
    color: #ffffff;
}

@media (max-width: 767px) {
    .navbar-default .navbar-nav .open .dropdown-menu > li > a {
        color: #ecf0f1;
    }
    .navbar-default .navbar-nav .open .dropdown-menu > li > a:hover, .navbar-default .navbar-nav .open .dropdown-menu > li > a:focus {
        color: #ffffff;
    }
    .navbar-default .navbar-nav .open .dropdown-menu > .active > a, .navbar-default .navbar-nav .open .dropdown-menu > .active > a:hover, .navbar-default .navbar-nav .open .dropdown-menu > .active > a:focus {
        color: #ffffff;
        background-color: #216cac;
    }
} 
        with: .form-buttons {
    margin: 20px 20px 0;
}
.eav-wrapper, .list-view {
    margin-top: 20px;
}
.eav-attribute {
    margin-bottom: 10px;
}
.ad-item {
    border-bottom: 1px solid #ccc;
    padding: 15px 15px 50px 190px;
    position: relative;
    min-height: 140px;
}
.ad-preview {
    margin-left: -175px;
    max-width: 160px;
    max-height: 120px;
}
.ad-city-added {
    position: absolute;
    bottom: 15px;
}
.ad-price {
    font-weight: bold;
    font-size: 1.4em;
    text-align: right;
}
.ad-added {
    font-size: 0.9em;
}
h3 {
    font-size: 1.4em;
    margin-top: 5px;
}
.pagination>li:nth-child(2)>a {
    border-top-left-radius: 4px;
    border-bottom-left-radius: 4px;
}
.pagination>li:nth-last-child(2)>a {
    border-top-right-radius: 4px;
    border-bottom-right-radius: 4px;
}
.my-pager {
    text-align: center;
}

@media (max-width: 767px) {
    .ad-price {
        text-align: left;
        font-size: 1.2em;
    }
} 
          cmakeBuildDirectory: cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css ../css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js ../js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot ../fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg ../fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf ../fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff ../fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 ../fonts/glyphicons-halflings-regular.woff2;

cp vendor/select2/select2/select2.js ../js/select2.js;
cp vendor/select2/select2/select2_locale_ru.js ../js/select2_locale_ru.js;
cp vendor/select2/select2/select2.css ../css/select2.css;
cp vendor/select2/select2/select2.png ../css/select2.png;
cp vendor/select2/select2/select2-spinner.gif ../css/select2-spinner.gif;

cp vendor/silverfire/select2-bootstrap3-css/select2-bootstrap.min.css ../css/select2-bootstrap.min.css; 
          # Ruleset file that will determine what checks will be run
          ruleset: <?php

class FakerCommand extends CConsoleCommand
{
    const PER_INSERT = 100; // how many rows should be inserted in one query
    const PER_TRANSACTION = 100; // how many queries should contain one transaction
    const PASSWORD = 'demo';
    const PUBLISHED = 'published';

    private $fakerData = array();
    private $builder;
    private $password;

    public function init()
    {
        $this->fakerData = new FakerData(self::PER_INSERT);
        $this->builder = Yii::app()->db->getSchema()->getCommandBuilder();
        $this->password = CPasswordHelper::hashPassword(self::PASSWORD);
    }

    public function actionUser($cnt = 1000)
    {
        $table = 'user'; 
        $insertCount = intval(floor($cnt/self::PER_INSERT));
        $txn = Yii::app()->db->beginTransaction();
        for ($i=0; $i < $insertCount; $i++) {
            $this->progressMsg($i * self::PER_INSERT, $cnt);
            $data = $this->collectUserData();
            $this->multipleInsert($table, $data);
            if ($i % self::PER_TRANSACTION == 0 and $i != 0) {
                $txn->commit();
                $txn = Yii::app()->db->beginTransaction();
            }
        }
        $remainder = $cnt % self::PER_INSERT;
        if ($remainder) {
            $data = $this->collectUserData($remainder);
            $this->multipleInsert($table, $data);
        }
        $txn->commit();
    }

    public function actionAd($cnt=1000, $eav=false, $photos=false)
    {
        echo date('H:i:s') . "\n";
        $insertCount = intval(floor($cnt/self::PER_INSERT));
        $txn = Yii::app()->db->beginTransaction();
        for ($i=0; $i < $insertCount; $i++) {
            $this->progressMsg($i * self::PER_INSERT, $cnt);
            $this->insertAd($eav, $photos);
            if ($i % self::PER_TRANSACTION == 0 and $i != 0) {
                $txn->commit();
                $txn = Yii::app()->db->beginTransaction();
            }
        }
        $remainder = $cnt % self::PER_INSERT;
        if ($remainder) {
            $this->insertAd($eav, $photos, $remainder);
        }
        $txn->commit();
        echo date('H:i:s') . "\n";
    }

    private function multipleInsert($table, $data)
    {
        $command = $this->builder->createMultipleInsertCommand($table, $data);
        $command->execute();
    }

    private function eavMultipleInsert($table, $data)
    {
        $sql = "INSERT INTO {$table} (eav_attribute_id, entity_id, entity, value) VALUES ";
        $i = 0;
        foreach ($data as $row) {
            if ($i == 0) {
                $sql .= "(:a{$i},:e{$i},'ad',:v{$i})";
            } else {
                $sql .= ",(:a{$i},:e{$i},'ad',:v{$i})";
            }
            $params[":e{$i}"] = $row['entity_id'];
            $params[":a{$i}"] = $row['eav_attribute_id'];
            $params[":v{$i}"] = $row['value'];
            $i++;
        }
        Yii::app()->db->createCommand($sql)->execute($params);
    }

    private function insertAd($eav, $photos, $cnt = self::PER_INSERT)
    {
        $table = 'ad';
        $data = $this->collectAdData($cnt);
        $this->multipleInsert($table, $data);
        $this->attachEav($eav, $cnt);
        $this->attachPhoto($photos, $cnt);
    }

    private function collectUserData($cnt = self::PER_INSERT)
    {
        $data = array();
        for ($i=0; $i < $cnt; $i++) {
            $data[] = array(
                'email' => $this->fakerData->getEmail(),
                // use 'demo' to login
                'password' => $this->password,
                'name' => $this->fakerData->getUserName(),
                'phone' => $this->fakerData->getPhoneNumber(),
            );
        }
        return $data;
    }

    private function collectAdData($cnt = self::PER_INSERT)
    {
        $data = array();
        for ($i=0; $i < $cnt; $i++) {
            $category = $this->fakerData->getCategory();
            $data[] = array(
                'title' => $this->fakerData->getTitle(),
                'description' => $this->fakerData->getDescription(),
                'author_id' => $this->fakerData->getAuthor(),
                'city_id' => $this->fakerData->getCity(),
                'category_id' => $category['id'],
                'eav_set_id' => $category['set_id'],
                'status' => self::PUBLISHED,
            );
        }
        return $data;
    }

    private function attachEav($eav, $cnt = self::PER_INSERT)
    {
        if (!$eav) return;
        $data = $this->collectEavData($cnt);
        if (!empty($data['int'])) {
            $this->eavMultipleInsert('eav_attribute_int', $data['int']);
        }
        if (!empty($data['varchar'])) {
            $this->eavMultipleInsert('eav_attribute_varchar', $data['varchar']);
        }
    }

    private function collectEavData($cnt)
    {
        $ads = $this->getAdsCommand($cnt)->queryAll();
        foreach ($ads as $ad) {
            $set = $this->fakerData->getSet($ad['eav_set_id']);
            foreach ($set as $attr) {
                if ($attr['data_type'] == 'IntDataType') {
                    $value = $this->getEavIntValue($attr);
                    $data['int'][] = array(
                        'eav_attribute_id' => $attr['eav_attribute_id'],
                        'entity_id' => $ad['id'],
                        'entity' => 'ad',
                        'value' => $value,
                    );
                } elseif ($attr['data_type'] == 'VarcharDataType') {
                    $value = $this->getEavVarcharValue($attr);
                    $data['varchar'][] = array(
                        'eav_attribute_id' => $attr['eav_attribute_id'],
                        'entity_id' => $ad['id'],
                        'entity' => 'ad',
                        'value' => $value,
                    );
                }
            }
        }
        return $data;
    }

    private function attachPhoto($photos, $cnt = self::PER_INSERT)
    {
        if (!$photos) return;
        $data = $this->collectPhotoData($cnt);
        $this->multipleInsert('photo', $data);
    }

    private function collectPhotoData($cnt)
    {
        $data = array();
        $ads = $this->getAdsCommand($cnt)->queryAll();
        foreach ($ads as $ad) {
            $photoCount = rand(0, 5);
            for ($i=0; $i < $photoCount; $i++) {
                $data[] = array(
                    'name' => $this->fakerData->getPhotoName(),
                    'ad_id' => $ad['id'],
                );
            }
        }
        return $data;
    }

    private function getEavIntValue($attr)
    {
        $rawData = unserialize($attr['data']);
        $min = (isset($rawData['rules']['numerical']['min']))
                ? intval($rawData['rules']['numerical']['min'])
                : 0;
        $max = (isset($rawData['rules']['numerical']['max']))
                ? intval($rawData['rules']['numerical']['max'])
                : 100000000;
        if ($attr['name'] == 'price') {
            $value = rand($min, intval($max/100));
        } elseif ($attr['name'] == 'modelYear') {
            $max = intval(date('Y'));
            $value = rand($min, $max);
        } else {
            $value = rand($min, $max);
        }
        return $value;
    }

    private function getEavVarcharValue($attr)
    {
        $rawData = unserialize($attr['data']);
        return $rawData['values'][array_rand($rawData['values'])];
    }

    private function getAdsCommand($cnt)
    {
        $sql = "SELECT id, eav_set_id FROM ad ORDER BY id DESC LIMIT $cnt";
        return Yii::app()->db->createCommand($sql);
    }

    private function progressMsg($cur, $cnt)
    {
        echo round($cur * 100 / $cnt, 2) . "%\n";
    }
} 

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: cc_binary {
    name: "audioadsprpcd",

    srcs: ["adsprpcd.c"],

    shared_libs: [
        "liblog",
        "libdl",
    ],

    init_rc: ["vendor.qti.audio-adsprpc-service.rc"],

    vendor: true,

    owner: "qti",

    sanitize: {
        integer_overflow: true,
    },
} 
        uses: /*
 * Copyright (c) 2013-2016, 2020, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VERIFY_PRINT_ERROR
#define VERIFY_PRINT_ERROR
#endif

#define VERIFY_EPRINTF ALOGE
#define VERIFY_IPRINTF ALOGI

#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

#include <log/log.h>

#ifndef ADSP_DEFAULT_LISTENER_NAME
#define ADSP_DEFAULT_LISTENER_NAME "libadsp_default_listener.so"
#endif

#define AEE_ECONNREFUSED 0x72

typedef int (*adsp_default_listener_start_t)(int argc, char *argv[]);

int main(int argc, char *argv[]) {

  int nErr = 0;
  void *adsphandler = NULL;
  adsp_default_listener_start_t listener_start;

  VERIFY_EPRINTF("audio adsp daemon starting");
  while (1) {
    if(NULL != (adsphandler = dlopen(ADSP_DEFAULT_LISTENER_NAME, RTLD_NOW))) {
      if(NULL != (listener_start =
        (adsp_default_listener_start_t)dlsym(adsphandler, "adsp_default_listener_start"))) {
        VERIFY_IPRINTF("adsp_default_listener_start called");
        nErr = listener_start(argc, argv);
      }
      if(0 != dlclose(adsphandler)) {
        VERIFY_EPRINTF("dlclose failed");
      }
    } else {
      VERIFY_EPRINTF("audio adsp daemon error %s", dlerror());
    }
    if (nErr == AEE_ECONNREFUSED) {
      VERIFY_EPRINTF("fastRPC device driver is disabled, daemon exiting...");
      break;
    }
    VERIFY_EPRINTF("audio adsp daemon will restart after 25ms...");
    usleep(25000);
  }
  VERIFY_EPRINTF("audio adsp daemon exiting %x", nErr);
  return nErr;
} 
        with: #
# Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#    * Neither the name of The Linux Foundation nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

service vendor.audioadsprpcd_audiopd /vendor/bin/audioadsprpcd audiopd
   class main
   user media
   group media 
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v4
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
