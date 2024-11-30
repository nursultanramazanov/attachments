# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: What:                /proc/sys/vm/nr_pdflush_threads
Date:                June 2012
Contact:        Wanpeng Li <liwp@linux.vnet.ibm.com>
Description: Since pdflush is replaced by per-BDI flusher, the interface of old pdflush
             exported in /proc/sys/vm/ should be removed.  
  push: What:                /sys/bus/usb/devices/.../power/level
Date:                March 2007
KernelVersion:        2.6.21
Contact:        Alan Stern <stern@rowland.harvard.edu>
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
  pull_request: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/actual_profile
Date:                Januar 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 1-5.
                When read, this attribute returns the number of the actual
                profile which is also the profile that's active on device startup.
                When written this attribute activates the selected profile
                immediately.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/button
Date:                Januar 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The keyboard can store short macros with consist of 1 button with
                several modifier keys internally.
                When written, this file lets one set the sequence for a specific
                button for a specific profile. Button and profile numbers are
                included in written data. The data has to be 24 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/info
Date:                Januar 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns some info about the device like the
                installed firmware version.
                The size of the data is 8 bytes in size.
                This file is readonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/key_mask
Date:                Januar 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The keyboard lets the user deactivate 5 certain keys like the
                windows and application keys, to protect the user from the outcome
                of accidentally pressing them.
                The integer value of this attribute has bits 0-4 set depending
                on the state of the corresponding key.
                When read, this file returns the current state of the buttons.
                When written, the given buttons are activated/deactivated
                immediately.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/mode_key
Date:                Januar 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The keyboard has a condensed layout without num-lock key.
                Instead it uses a mode-key which activates a gaming mode where
                the assignment of the number block changes.
                The integer value of this attribute ranges from 0 (OFF) to 1 (ON).
                When read, this file returns the actual state of the key.
                When written, the key is activated/deactivated immediately.
Users:                http://roccat.sourceforge.net   
    branches: [ "main" ]
  schedule: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/actual_profile
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the actual
                profile. This value is persistent, so its equivalent to the
                profile that's active when the device is powered on next time.
                When written, this file sets the number of the startup profile
                and the device activates this profile immediately.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/info
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns general data like firmware version.
                The data is 6 bytes long.
                This file is readonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/key_mask
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one deactivate certain keys like
                windows and application keys, to prevent accidental presses.
                Profile number for which this settings occur is included in
                written data. The data has to be 6 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_capslock
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the
                capslock key for a specific profile. Profile number is included
                in written data. The data has to be 6 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_easyzone
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the
                easyzone keys for a specific profile. Profile number is included
                in written data. The data has to be 65 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_function
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the
                function keys for a specific profile. Profile number is included
                in written data. The data has to be 41 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_macro
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the macro
                keys for a specific profile. Profile number is included in
                written data. The data has to be 35 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_media
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the media
                keys for a specific profile. Profile number is included in
                written data. The data has to be 29 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/keys_thumbster
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the
                thumbster keys for a specific profile. Profile number is included
                in written data. The data has to be 23 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/last_set
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the time in secs since
                epoch in which the last configuration took place.
                The data has to be 20 bytes long.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/light
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the backlight intensity for
                a specific profile. Profile number is included in written data.
                The data has to be 10 bytes long for Isku, IskuFX needs        16 bytes
                of data.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/macro
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one store macros with max 500
                keystrokes for a specific button for a specific profile.
                Button and profile numbers are included in written data.
                The data has to be 2083 bytes long.
                Before reading this file, control has to be written to select
                which profile and key to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/reset
Date:                November 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one reset the device.
                The data has to be 3 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/control
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one select which data from which
                profile will be        read next. The data has to be 3 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/talk
Date:                June 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one trigger easyshift functionality
                from the host.
                The data has to be 16 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/talkfx
Date:                February 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one trigger temporary color schemes
                from the host.
                The data has to be 16 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net    
    - cron: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/actual_profile
Date:                October 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the actual
                profile. This value is persistent, so its equivalent to the
                profile that's active when the mouse is powered on next time.
                When written, this file sets the number of the startup profile
                and the mouse activates this profile immediately.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/startup_profile
Date:                October 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the actual
                profile. This value is persistent, so its equivalent to the
                profile that's active when the mouse is powered on next time.
                When written, this file sets the number of the startup profile
                and the mouse activates this profile immediately.
                Please use actual_profile, it does the same thing.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/firmware_version
Date:                October 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns the raw integer version number of the
                firmware reported by the mouse. Using the integer value eases
                further usage in other programs. To receive the real version
                number the decimal point has to be shifted 2 positions to the
                left. E.g. a returned value of 121 means 1.21
                This file is readonly.
                Please read binary attribute info which contains firmware version.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/info
Date:                November 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns general data like firmware version.
                When written, the device can be reset.
                The data is 8 bytes long.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/macro
Date:                October 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store a macro with max 500 key/button strokes
                internally.
                When written, this file lets one set the sequence for a specific
                button for a specific profile. Button and profile numbers are
                included in written data. The data has to be 2082 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/profile_buttons
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_buttons holds information about button layout.
                When written, this file lets one write the respective profile
                buttons back to the mouse. The data has to be 77 bytes long.
                The mouse will reject invalid data.
                Which profile to write is determined by the profile number
                contained in the data.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/profile[1-5]_buttons
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_buttons holds information about button layout.
                When read, these files return the respective profile buttons.
                The returned data is 77 bytes in size.
                This file is readonly.
                Write control to select profile and read profile_buttons instead.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/profile_settings
Date:                October 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
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
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/profile[1-5]_settings
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_settings holds information like resolution, sensitivity
                and light effects.
                When read, these files return the respective profile settings.
                The returned data is 43 bytes in size.
                This file is readonly.
                Write control to select profile and read profile_settings instead.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/sensor
Date:                October 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse has a tracking- and a distance-control-unit. These
                can be activated/deactivated and the lift-off distance can be
                set. The data has to be 6 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/talk
Date:                May 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        Used to active some easy* functions of the mouse from outside.
                The data has to be 16 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/tcu
Date:                October 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written a calibration process for the tracking control unit
                can be initiated/cancelled. Also lets one read/write sensor
                registers.
                The data has to be 4 bytes long.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/tcu_image
Date:                October 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read the mouse returns a 30x30 pixel image of the
                sampled underground. This works only in the course of a
                calibration process initiated with tcu.
                The returned data is 1028 bytes in size.
                This file is readonly.
Users:                http://roccat.sourceforge.net   

env: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/actual_profile
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. actual_profile holds number of actual profile.
                This value is persistent, so its value determines the profile
                that's active when the mouse is powered on next time.
                When written, the mouse activates the set profile immediately.
                The data has to be 3 bytes long.
                The mouse will reject invalid data.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/control
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one select which data from which
                profile will be        read next. The data has to be 3 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/info
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns general data like firmware version.
                When written, the device can be reset.
                The data is 6 bytes long.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/macro
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store a macro with max 500 key/button strokes
                internally.
                When written, this file lets one set the sequence for a specific
                button for a specific profile. Button and profile numbers are
                included in written data. The data has to be 2082 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/profile_buttons
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_buttons holds information about button layout.
                When written, this file lets one write the respective profile
                buttons back to the mouse. The data has to be 59 bytes long.
                The mouse will reject invalid data.
                Which profile to write is determined by the profile number
                contained in the data.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/profile_settings
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
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
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/sensor
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse has a tracking- and a distance-control-unit. These
                can be activated/deactivated and the lift-off distance can be
                set. The data has to be 6 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/talk
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        Used to active some easy* functions of the mouse from outside.
                The data has to be 16 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/tcu
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written a calibration process for the tracking control unit
                can be initiated/cancelled. Also lets one read/write sensor
                registers.
                The data has to be 4 bytes long.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/tcu_image
Date:                December 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read the mouse returns a 30x30 pixel image of the
                sampled underground. This works only in the course of a
                calibration process initiated with tcu.
                The returned data is 1028 bytes in size.
                This file is readonly.
Users:                http://roccat.sourceforge.net   
  # Path to the CMake build directory.
  build: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_cpi
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 1-4.
                When read, this attribute returns the number of the active
                cpi level.
                This file is readonly.
                Has never been used. If bookkeeping is done, it's done in userland tools.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_profile
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the active
                profile.
                When written, the mouse activates this profile immediately.
                The profile that's active when powered down is the same that's
                active when the mouse is powered on.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_sensitivity_x
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 1-10.
                When read, this attribute returns the number of the actual
                sensitivity in x direction.
                This file is readonly.
                Has never been used. If bookkeeping is done, it's done in userland tools.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_sensitivity_y
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 1-10.
                When read, this attribute returns the number of the actual
                sensitivity in y direction.
                This file is readonly.
                Has never been used. If bookkeeping is done, it's done in userland tools.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/firmware_version
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns the raw integer version number of the
                firmware reported by the mouse. Using the integer value eases
                further usage in other programs. To receive the real version
                number the decimal point has to be shifted 2 positions to the
                left. E.g. a returned value of 121 means 1.21
                This file is readonly.
                Obsoleted by binary sysfs attribute "info".
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/info
Date:                November 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns general data like firmware version.
                When written, the device can be reset.
                The data is 6 bytes long.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/profile_buttons
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_buttons holds information about button layout.
                When written, this file lets one write the respective profile
                buttons back to the mouse. The data has to be 23 bytes long.
                The mouse will reject invalid data.
                Which profile to write is determined by the profile number
                contained in the data.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/profile[1-5]_buttons
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_buttons holds information about button layout.
                When read, these files return the respective profile buttons.
                The returned data is 23 bytes in size.
                This file is readonly.
                Write control to select profile and read profile_buttons instead.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/profile_settings
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
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
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/profile[1-5]_settings
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_settings holds information like resolution, sensitivity
                and light effects.
                When read, these files return the respective profile settings.
                The returned data is 16 bytes in size.
                This file is readonly.
                Write control to select profile and read profile_settings instead.
Users:                http://roccat.sourceforge.net    

permissions: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/control
Date:                October 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, cpi, button and light settings can be configured.
                When read, actual cpi setting and sensor data are returned.
                The data has to be 8 bytes long.
Users:                http://roccat.sourceforge.net  
  contents: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/actual_cpi
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        It is possible to switch the cpi setting of the mouse with the
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
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/actual_profile
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns the number of the actual profile in
                range 0-4.
                This file is readonly.
                Please use binary attribute "settings" which provides this information.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/firmware_version
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns the raw integer version number of the
                firmware reported by the mouse. Using the integer value eases
                further usage in other programs. To receive the real version
                number the decimal point has to be shifted 2 positions to the
                left. E.g. a returned value of 138 means 1.38
                This file is readonly.
                Please use binary attribute "info" which provides this information.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/info
Date:                November 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns general data like firmware version.
                When written, the device can be reset.
                The data is 6 bytes long.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/profile_buttons
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_buttons holds information about button layout.
                When written, this file lets one write the respective profile
                buttons back to the mouse. The data has to be 19 bytes long.
                The mouse will reject invalid data.
                Which profile to write is determined by the profile number
                contained in the data.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/profile[1-5]_buttons
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_buttons holds information about button layout.
                When read, these files return the respective profile buttons.
                The returned data is 19 bytes in size.
                This file is readonly.
                Write control to select profile and read profile_buttons instead.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/profile_settings
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
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
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/profile[1-5]_settings
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split in settings and buttons.
                profile_settings holds information like resolution, sensitivity
                and light effects.
                When read, these files return the respective profile settings.
                The returned data is 13 bytes in size.
                This file is readonly.
                Write control to select profile and read profile_settings instead.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/settings
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns the settings stored in the mouse.
                The size of the data is 3 bytes and holds information on the
                startup_profile.
                When written, this file lets write settings back to the mouse.
                The data has to be 3 bytes long. The mouse will reject invalid
                data.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/startup_profile
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the profile
                that's active when the mouse is powered on.
                This file is readonly.
                Please use binary attribute "settings" which provides this information.
Users:                http://roccat.sourceforge.net   

jobs: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/control
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one select which data from which
                profile will be        read next. The data has to be 3 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/profile
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. profile holds index of actual profile.
                This value is persistent, so its value determines the profile
                that's active when the device is powered on next time.
                When written, the device activates the set profile immediately.
                The data has to be 3 bytes long.
                The device will reject invalid data.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_primary
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the default of all keys for
                a specific profile. Profile index is included in written data.
                The data has to be 125 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_function
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the
                function keys for a specific profile. Profile index is included
                in written data. The data has to be 95 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_macro
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the macro
                keys for a specific profile. Profile index is included in
                written data. The data has to be 35 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_thumbster
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the
                thumbster keys for a specific profile. Profile index is included
                in written data. The data has to be 23 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_extra
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the
                capslock and function keys for a specific profile. Profile index
                is included in written data. The data has to be 8 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/keys_easyzone
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the function of the
                easyzone keys for a specific profile. Profile index is included
                in written data. The data has to be 294 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/key_mask
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one deactivate certain keys like
                windows and application keys, to prevent accidental presses.
                Profile index for which this settings occur is included in
                written data. The data has to be 6 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/light
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the backlight intensity for
                a specific profile. Profile index is included in written data.
                This attribute is only valid for the glow and pro variant.
                The data has to be 16 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/macro
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one store macros with max 480
                keystrokes for a specific button for a specific profile.
                Button and profile indexes are included in written data.
                The data has to be 2002 bytes long.
                Before reading this file, control has to be written to select
                which profile and key to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/info
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns general data like firmware version.
                The data is 8 bytes long.
                This file is readonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/reset
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one reset the device.
                The data has to be 3 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/talk
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one trigger easyshift functionality
                from the host.
                The data has to be 16 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/light_control
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one switch between stored and custom
                light settings.
                This attribute is only valid for the pro variant.
                The data has to be 8 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/stored_lights
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set per-key lighting for different
                layers.
                This attribute is only valid for the pro variant.
                The data has to be 1382 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/custom_lights
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set the actual per-key lighting.
                This attribute is only valid for the pro variant.
                The data has to be 20 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/light_macro
Date:                October 2013
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one set a light macro that is looped
                whenever the device gets in dimness mode.
                This attribute is only valid for the pro variant.
                The data has to be 2002 bytes long.
                Before reading this file, control has to be written to select
                which profile to read.
Users:                http://roccat.sourceforge.net    
  analyze: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/buttons
Date:                Mai 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
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
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/control
Date:                Mai 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one select which data from which
                profile will be        read next. The data has to be 3 bytes long.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/general
Date:                Mai 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. A profile is split into general settings and
                button settings. A profile holds information like resolution,
                sensitivity and light effects.
                When written, this file lets one write the respective profile
                settings back to the mouse. The data has to be 43 bytes long.
                The mouse will reject invalid data.
                Which profile to write is determined by the profile number
                contained in the data.
                This file is writeonly.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/info
Date:                Mai 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When read, this file returns general data like firmware version.
                When written, the device can be reset.
                The data is 8 bytes long.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/macro
Date:                Mai 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        When written, this file lets one store macros with max 500
                keystrokes for a specific button for a specific profile.
                Button and profile numbers are included in written data.
                The data has to be 2083 bytes long.
                Before reading this file, control has to be written to select
                which profile and key to read.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/profile
Date:                Mai 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse can store 5 profiles which can be switched by the
                press of a button. profile holds number of actual profile.
                This value is persistent, so its value determines the profile
                that's active when the mouse is powered on next time.
                When written, the mouse activates the set profile immediately.
                The data has to be 3 bytes long.
                The mouse will reject invalid data.
Users:                http://roccat.sourceforge.net

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/sensor
Date:                July 2012
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The mouse has a Avago ADNS-3090 sensor.
                This file allows reading and writing of the mouse sensors registers.
                The data has to be 4 bytes long.
Users:                http://roccat.sourceforge.net   
    permissions: What:                /sys/firmware/acpi/hotplug/force_remove
Date:                Mar 2017
Contact:        Rafael J. Wysocki <rafael.j.wysocki@intel.com>
Description:
                Since the force_remove is inherently broken and dangerous to
                use for some hotplugable resources like memory (because ignoring
                the offline failure might lead to memory corruption and crashes)
                enabling this knob is not safe and thus unsupported.   
      contents: What:                /sys/class/gpio/
Date:                July 2008
KernelVersion:        2.6.27
Contact:        Linus Walleij <linusw@kernel.org>
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
  replaced with the GPIO character device. # for actions/checkout to fetch code
      security-events: What:                devfs
Date:                July 2005 (scheduled), finally removed in kernel v2.6.18
Contact:        Greg Kroah-Hartman <gregkh@linuxfoundation.org>
Description:
        devfs has been unmaintained for a number of years, has unfixable
        races, contains a naming policy within the kernel that is
        against the LSB, and can be replaced by using udev.
        The files fs/devfs/*, include/linux/devfs_fs*.h were removed,
        along with the assorted devfs function calls throughout the
        kernel tree.

Users: # for github/codeql-action/upload-sarif to upload SARIF results
      actions: What:                dv1394 (a.k.a. "OHCI-DV I/O support" for FireWire)
Date:                May 2010 (scheduled), finally removed in kernel v2.6.37
Contact:        linux1394-devel@lists.sourceforge.net
Description:
        /dev/dv1394/* were character device files, one for each FireWire
        controller and for NTSC and PAL respectively, from which DV data
        could be received by read() or transmitted by write().  A few
        ioctl()s allowed limited control.
        This special-purpose interface has been superseded by libraw1394 +
        libiec61883 which are functionally equivalent, support HDV, and
        transparently work on top of the newer firewire kernel drivers.

Users:
        ffmpeg/libavformat (if configured for DV1394) # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: What:                ip_queue
Date:                finally removed in kernel v3.5.0
Contact:        Pablo Neira Ayuso <pablo@netfilter.org>
Description:
        ip_queue has been replaced by nfnetlink_queue which provides
        more advanced queueing mechanism to user-space. The ip_queue
        module was already announced to become obsolete years ago.

Users:     
    runs-on: What:                tcp_dma_copybreak sysctl
Date:                Removed in kernel v3.13
Contact:        Dan Williams <dan.j.williams@intel.com>
Description:
        Formerly the lower limit, in bytes, of the size of socket reads
        that will be offloaded to a DMA copy engine.  Removed due to
        coherency issues of the cpu potentially touching the buffers
        while dma is in flight.   

    steps: What:                /sys/o2cb symlink
Date:                May 2011
KernelVersion:        3.0
Contact:        ocfs2-devel@oss.oracle.com
Description:        This is a symlink: /sys/o2cb to /sys/fs/o2cb. The symlink is
                removed when new versions of ocfs2-tools which know to look
                in /sys/fs/o2cb are sufficiently prevalent. Don't code new
                software to look here, it should try /sys/fs/o2cb instead.
Users:                ocfs2-tools. It's sufficient to mail proposed changes to
                ocfs2-devel@oss.oracle.com.   
      - name: What:                raw1394 (a.k.a. "Raw IEEE1394 I/O support" for FireWire)
Date:                May 2010 (scheduled), finally removed in kernel v2.6.37
Contact:        linux1394-devel@lists.sourceforge.net
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
        uses: rfkill - radio frequency (RF) connector kill switch support

For details to this subsystem look at Documentation/rfkill.txt.

What:                /sys/class/rfkill/rfkill[0-9]+/claim
Date:                09-Jul-2007
KernelVersion        v2.6.22
Contact:        linux-wireless@vger.kernel.org
Description:        This file was deprecated because there no longer was a way to
                claim just control over a single rfkill instance.
                This file was scheduled to be removed in 2012, and was removed
                in 2016.
Values:         0: Kernel handles events   

      - name: What:                video1394 (a.k.a. "OHCI-1394 Video support" for FireWire)
Date:                May 2010 (scheduled), finally removed in kernel v2.6.37
Contact:        linux1394-devel@lists.sourceforge.net
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
        run: cc_binary {
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

      # Build is not required unless generated source files are used
      # - name: /*
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
      #   run: #
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

      - name: LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

ifneq (,$(findstring $(PLATFORM_VERSION), 5.0 5.1 5.1.1))
include external/stlport/libstlport.mk
endif

LOCAL_SRC_FILES:= \
        audiod_main.cpp \
        AudioDaemon.cpp \

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        libbinder \
        libmedia

ifneq (,$(findstring $(PLATFORM_VERSION), 5.0 5.1 5.1.1))
LOCAL_SHARED_LIBRARIES += libstlport
endif

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_MODULE:= audiod

ifneq ($(filter kona lahaina holi,$(TARGET_BOARD_PLATFORM)),)
LOCAL_SANITIZE := integer_overflow
endif
include $(BUILD_EXECUTABLE)      
        uses: /* AudioDaemon.cpp
Copyright (c) 2012-2016, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#define LOG_TAG "AudioDaemon"
#define LOG_NDEBUG 0
#define LOG_NDDEBUG 0

#include <dirent.h>
#include <media/AudioSystem.h>
#include <sys/poll.h>

#include "AudioDaemon.h"

#define CPE_MAGIC_NUM 0x2000
#define MAX_CPE_SLEEP_RETRY 2
#define CPE_SLEEP_WAIT 100

#define MAX_SLEEP_RETRY 100
#define AUDIO_INIT_SLEEP_WAIT 100 /* 100 ms */

int bootup_complete = 0;
bool cpe_bootup_complete = false;

namespace android {

    AudioDaemon::AudioDaemon() : Thread(false) {
    }

    AudioDaemon::~AudioDaemon() {
        putStateFDs(mSndCardFd);
    }

    void AudioDaemon::onFirstRef() {
        ALOGV("Start audiod daemon");
        run("AudioDaemon", PRIORITY_URGENT_AUDIO);
    }

    void AudioDaemon::binderDied(const wp<IBinder>& who)
    {
        requestExit();
    }

    bool AudioDaemon::getStateFDs(std::vector<std::pair<int,int> > &sndcardFdPair)
    {
        FILE *fp;
        int fd;
        char *ptr, *saveptr, *card_id = NULL;
        char buffer[128];
        int line = 0;
        String8 path;
        int sndcard;
        const char* cards = "/proc/asound/cards";

        if ((fp = fopen(cards, "r")) == NULL) {
            ALOGE("Cannot open %s file to get list of sound cars", cards);
            return false;
        }

        sndcardFdPair.clear();
        memset(buffer, 0x0, sizeof(buffer));
        while ((fgets(buffer, sizeof(buffer), fp) != NULL)) {
            if (line++ % 2)
                continue;
            ptr = strtok_r(buffer, " [", &saveptr);
            if (!ptr)
                continue;

            card_id = strtok_r(saveptr+1, "]", &saveptr);
            if (!card_id)
                continue;
            //Only consider sound cards associated with ADSP
            if ((strncasecmp(card_id, "msm", 3) != 0) &&
                (strncasecmp(card_id, "sdm", 3) != 0) &&
                (strncasecmp(card_id, "sdc", 3) != 0) &&
                (strncasecmp(card_id, "apq", 3) != 0)) {
                ALOGD("Skipping non-ADSP sound card %s", card_id);
                continue;
            }
            if (ptr) {
                path = "/proc/asound/card";
                path += ptr;
                path += "/state";
                ALOGD("Opening sound card state : %s", path.string());
                fd = open(path.string(), O_RDONLY);
                if (fd == -1) {
                    ALOGE("Open %s failed : %s", path.string(), strerror(errno));
                } else {
                    /* returns vector of pair<sndcard, fd> */
                    sndcard = atoi(ptr);
                    sndcardFdPair.push_back(std::make_pair(sndcard, fd));
                }
            }
        }

        ALOGV("%s: %d sound cards detected", __func__, sndcardFdPair.size());
        fclose(fp);

        return sndcardFdPair.size() > 0 ? true : false;
    }

    void AudioDaemon::putStateFDs(std::vector<std::pair<int,int> > &sndcardFdPair)
    {
        unsigned int i;
        for (i = 0; i < sndcardFdPair.size(); i++)
            close(sndcardFdPair[i].second);
        sndcardFdPair.clear();
    }

    bool AudioDaemon::getDeviceEventFDs()
    {
        const char* events_dir = "/sys/class/switch/";
        DIR *dp;
        struct dirent* in_file;
        int fd;
        String8 path;
        String8 d_name;

        if ((dp = opendir(events_dir)) == NULL) {
            ALOGE("Cannot open switch directory to get list of audio events %s", events_dir);
            return false;
        }

        mAudioEvents.clear();
        mAudioEventsStatus.clear();

        while ((in_file = readdir(dp)) != NULL) {

            if (!strstr(in_file->d_name, "qc_"))
                continue;
            ALOGD(" Found event file = %s", in_file->d_name);
            path = "/sys/class/switch/";
            path += in_file->d_name;
            path += "/state";

            ALOGE("Opening audio event state : %s ", path.string());
            fd = open(path.string(), O_RDONLY);
            if (fd == -1) {
                ALOGE("Open %s failed : %s", path.string(), strerror(errno));
            } else {
                d_name = in_file->d_name;
                mAudioEvents.push_back(std::make_pair(d_name, fd));
                mAudioEventsStatus.push_back(std::make_pair(d_name, 0));
                ALOGD("event status mAudioEventsStatus= %s",
                          mAudioEventsStatus[0].first.string());
            }
        }

        ALOGV("%s: %d audio device event detected",
                  __func__,
                  mAudioEvents.size());

        closedir(dp);
        return mAudioEvents.size() > 0 ? true : false;

    }

    void  AudioDaemon::putDeviceEventFDs()
    {
        unsigned int i;
        for (i = 0; i < mAudioEvents.size(); i++) {
            close(mAudioEvents[i].second);
            delete(mAudioEvents[i].first);
        }
        mAudioEvents.clear();
        mAudioEventsStatus.clear();
    }

    void AudioDaemon::checkEventState(int fd, int index)
    {
        char state_buf[2];
        audio_event_status event_cur_state = audio_event_off;

        if (!read(fd, (void *)state_buf, 1)) {
            ALOGE("Error receiving device state event (%s)", strerror(errno));
        } else {
             state_buf[1] = '\0';
            if (atoi(state_buf) != mAudioEventsStatus[index].second) {
                ALOGD("notify audio HAL %s",
                        mAudioEvents[index].first.string());
                mAudioEventsStatus[index].second = atoi(state_buf);

                if (mAudioEventsStatus[index].second == 1)
                    event_cur_state = audio_event_on;
                else
                    event_cur_state = audio_event_off;
                notifyAudioSystemEventStatus(
                               mAudioEventsStatus[index].first.string(),
                               event_cur_state);
            }
        }
        lseek(fd, 0, SEEK_SET);
    }

    status_t AudioDaemon::readyToRun() {

        ALOGV("readyToRun: open snd card state node files");
        return NO_ERROR;
    }

    bool AudioDaemon::threadLoop()
    {
        int max = -1;
        unsigned int i;
        bool ret = true;
        notify_status cur_state = snd_card_offline;
        struct pollfd *pfd = NULL;
        char rd_buf[9];
        unsigned int sleepRetry = 0;
        bool audioInitDone = false;
        int fd = 0;
        char path[50];
        notify_status cur_cpe_state = cpe_offline;
        notify_status prev_cpe_state = cpe_offline;
        unsigned int cpe_cnt = CPE_MAGIC_NUM;
        unsigned int num_snd_cards = 0;

        ALOGV("Start threadLoop()");
        while (audioInitDone == false && sleepRetry < MAX_SLEEP_RETRY) {
            if (mSndCardFd.empty() && !getStateFDs(mSndCardFd)) {
                ALOGE("Sleeping for 100 ms");
                usleep(AUDIO_INIT_SLEEP_WAIT*1000);
                sleepRetry++;
            } else {
                audioInitDone = true;
            }
        }

        if (!getDeviceEventFDs()) {
            ALOGE("No audio device events detected");
        }

        if (audioInitDone == false) {
            ALOGE("Sound Card is empty!!!");
            goto thread_exit;
        }

        /* soundcards are opened, now get the cpe state nodes */
        num_snd_cards = mSndCardFd.size();
        for (i = 0; i < num_snd_cards; i++) {
            snprintf(path, sizeof(path), "/proc/asound/card%d/cpe0_state", mSndCardFd[i].first);
            ALOGD("Opening cpe0_state : %s", path);
            sleepRetry = 0;
            do {
                fd = open(path, O_RDONLY);
                if (fd == -1)  {
                    sleepRetry++;
                    ALOGE("CPE state open %s failed %s, Retrying %d",
                          path, strerror(errno), sleepRetry);
                    usleep(CPE_SLEEP_WAIT*1000);
                } else {
                    ALOGD("cpe state opened: %s", path);
                    mSndCardFd.push_back(std::make_pair(cpe_cnt++, fd));
                }
            }while ((fd == -1) &&  sleepRetry < MAX_CPE_SLEEP_RETRY);
        }
        ALOGD("number of sndcards %d CPEs %d", i, cpe_cnt - CPE_MAGIC_NUM);

        pfd = new pollfd[mSndCardFd.size() + mAudioEvents.size()];
        bzero(pfd, (sizeof(*pfd) * mSndCardFd.size() +
                    sizeof(*pfd) * mAudioEvents.size()));
        for (i = 0; i < mSndCardFd.size(); i++) {
            pfd[i].fd = mSndCardFd[i].second;
            pfd[i].events = POLLPRI;
        }

        /*insert all audio events*/
        for(i = 0; i < mAudioEvents.size(); i++) {
            pfd[i+mSndCardFd.size()].fd = mAudioEvents[i].second;
            pfd[i+mSndCardFd.size()].events = POLLPRI;
        }

        ALOGD("read for sound card state change before while");
        for (i = 0; i < mSndCardFd.size(); i++) {
            if (!read(pfd[i].fd, (void *)rd_buf, 8)) {
               ALOGE("Error receiving sound card state event (%s)", strerror(errno));
               ret = false;
            } else {
               rd_buf[8] = '\0';
               lseek(pfd[i].fd, 0, SEEK_SET);

               if(mSndCardFd[i].first >= CPE_MAGIC_NUM) {
                   ALOGD("CPE %d state file content: %s before while",
                         mSndCardFd[i].first - CPE_MAGIC_NUM, rd_buf);
                   if (strstr(rd_buf, "OFFLINE")) {
                       ALOGD("CPE state offline");
                       cur_cpe_state = cpe_offline;
                   } else if (strstr(rd_buf, "ONLINE")){
                       ALOGD("CPE state online");
                       cur_cpe_state = cpe_online;
                   } else {
                       ALOGE("ERROR CPE rd_buf %s", rd_buf);
                   }
                   if (cur_cpe_state == cpe_online && !cpe_bootup_complete) {
                       cpe_bootup_complete = true;
                       ALOGD("CPE boot up completed before polling");
                   }
                   prev_cpe_state = cur_cpe_state;
               }
               else {
                   ALOGD("sound card state file content: %s before while",rd_buf);
                   if (strstr(rd_buf, "OFFLINE")) {
                       ALOGE("put cur_state to offline");
                       cur_state = snd_card_offline;
                   } else if (strstr(rd_buf, "ONLINE")){
                       ALOGE("put cur_state to online");
                       cur_state = snd_card_online;
                   } else {
                       ALOGE("ERROR rd_buf %s", rd_buf);
                   }

                   ALOGD("cur_state=%d, bootup_complete=%d", cur_state, cur_state );
                   if (cur_state == snd_card_online && !bootup_complete) {
                       bootup_complete = 1;
                       ALOGE("sound card up is deteced before while");
                       ALOGE("bootup_complete set to 1");
                   }
               }
            }
        }

       ALOGE("read for event state change before while");
       for (i = 0; i < mAudioEvents.size(); i++){
           checkEventState(pfd[i+mSndCardFd.size()].fd, i);
       }

        while (1) {
           ALOGD("poll() for sound card state change ");
           if (poll(pfd, (mSndCardFd.size() + mAudioEvents.size()), -1) < 0) {
              ALOGE("poll() failed (%s)", strerror(errno));
              ret = false;
              break;
           }

           ALOGD("out of poll() for sound card state change, SNDCARD size=%d", mSndCardFd.size());
           for (i = 0; i < mSndCardFd.size(); i++) {
               if (pfd[i].revents & POLLPRI) {
                   if (!read(pfd[i].fd, (void *)rd_buf, 8)) {
                       ALOGE("Error receiving sound card %d state event (%s)",
                             mSndCardFd[i].first, strerror(errno));
                       ret = false;
                   } else {
                       rd_buf[8] = '\0';
                       lseek(pfd[i].fd, 0, SEEK_SET);

                       if(mSndCardFd[i].first >= CPE_MAGIC_NUM) {
                           if (strstr(rd_buf, "OFFLINE"))
                               cur_cpe_state = cpe_offline;
                           else if (strstr(rd_buf, "ONLINE"))
                               cur_cpe_state = cpe_online;
                           else
                               ALOGE("ERROR CPE rd_buf %s", rd_buf);

                           if (cpe_bootup_complete && (prev_cpe_state != cur_cpe_state)) {
                               ALOGD("CPE state is %d, nofity AudioSystem", cur_cpe_state);
                               notifyAudioSystem(mSndCardFd[i].first, cur_cpe_state, CPE_STATE);
                           }
                           if (!cpe_bootup_complete && (cur_cpe_state == cpe_online)) {
                               cpe_bootup_complete = true;
                               ALOGD("CPE boot up completed");
                           }
                           prev_cpe_state = cur_cpe_state;
                       }
                       else {
                           ALOGV("sound card state file content: %s, bootup_complete=%d",rd_buf, bootup_complete);
                           if (strstr(rd_buf, "OFFLINE")) {
                               cur_state = snd_card_offline;
                           } else if (strstr(rd_buf, "ONLINE")){
                               cur_state = snd_card_online;
                           }

                           if (bootup_complete) {
                               ALOGV("bootup_complete, so NofityAudioSystem");
                               notifyAudioSystem(mSndCardFd[i].first, cur_state, SND_CARD_STATE);
                           }

                           if (cur_state == snd_card_online && !bootup_complete) {
                               bootup_complete = 1;
                           }
                       }
                   }
               }
           }
           for (i = 0; i < mAudioEvents.size(); i++) {
               if (pfd[i + mSndCardFd.size()].revents & POLLPRI) {
                   ALOGE("EVENT recieved pfd[i].revents= 0x%x %d",
                       pfd[i + mSndCardFd.size()].revents,
                       mAudioEvents[i].second);

                   checkEventState(pfd[i + mSndCardFd.size()].fd, i);
               }
           }
       }

       putStateFDs(mSndCardFd);
       putDeviceEventFDs();
       delete [] pfd;

    thread_exit:
       ALOGV("Exiting Poll ThreadLoop");
       return ret;
    }

    void AudioDaemon::notifyAudioSystem(int snd_card,
                                        notify_status status,
                                        notify_status_type type)
    {

        String8 str;
        char buf[4] = {0,};

        if (type == CPE_STATE) {
            str = "CPE_STATUS=";
            snprintf(buf, sizeof(buf), "%d", snd_card - CPE_MAGIC_NUM);
            str += buf;
            if (status == cpe_online)
                str += ",ONLINE";
            else
                str += ",OFFLINE";
        }
        else {
            str = "SND_CARD_STATUS=";
            snprintf(buf, sizeof(buf), "%d", snd_card);
            str += buf;
            if (status == snd_card_online)
                str += ",ONLINE";
            else
                str += ",OFFLINE";
        }
        ALOGV("%s: notifyAudioSystem : %s", __func__, str.string());
        AudioSystem::setParameters(0, str);
    }

    void AudioDaemon::notifyAudioSystemEventStatus(const char* event,
                                            audio_event_status status) {

        String8 str;
        str += AUDIO_PARAMETER_KEY_EXT_AUDIO_DEVICE;
        str += "=";
        str += event;

        if (status == audio_event_on)
            str += ",ON";
        else
            str += ",OFF";
        ALOGD("%s: notifyAudioSystemEventStatus : %s", __func__, str.string());
        AudioSystem::setParameters(0, str);
    }
}          
        # Provide a unique ID to access the sarif output path
        id: /* AudioDaemon.h

Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <vector>

#include <utils/threads.h>
#include <utils/String8.h>


namespace android {

enum notify_status {
    snd_card_online,
    snd_card_offline,
    cpe_online,
    cpe_offline
};

enum notify_status_type {
    SND_CARD_STATE,
    CPE_STATE
};

enum audio_event_status {audio_event_on, audio_event_off};

#define AUDIO_PARAMETER_KEY_EXT_AUDIO_DEVICE "ext_audio_device"

class AudioDaemon:public Thread, public IBinder :: DeathRecipient
{
    /*Overrides*/
    virtual bool        threadLoop();
    virtual status_t    readyToRun();
    virtual void        onFirstRef();
    virtual void        binderDied(const wp < IBinder > &who);

    bool processUeventMessage();
    void notifyAudioSystem(int snd_card,
                           notify_status status,
                           notify_status_type type);
    void notifyAudioSystemEventStatus(const char* event, audio_event_status status);
    int mUeventSock;
    bool getStateFDs(std::vector<std::pair<int,int> > &sndcardFdPair);
    void putStateFDs(std::vector<std::pair<int,int> > &sndcardFdPair);
    bool getDeviceEventFDs();
    void putDeviceEventFDs();
    void checkEventState(int fd, int index);

public:
    AudioDaemon();
    virtual ~AudioDaemon();

private:
    std::vector<std::pair<int,int> > mSndCardFd;

    //file descriptors for audio device events and their statuses
    std::vector<std::pair<String8, int> > mAudioEvents;
    std::vector<std::pair<String8, int> > mAudioEventsStatus;

};

}        
        with: /* Copyright (C) 2007 The Android Open Source Project

Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.

Not a Contribution, Apache license notifications and license are retained
for attribution purposes only.

* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#define LOG_TAG "AudioDaemonMain"
#define LOG_NDEBUG 0
#define LOG_NDDEBUG 0

#include <cutils/properties.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <utils/Log.h>
#include <utils/threads.h>

#if defined(HAVE_PTHREADS)
# include <pthread.h>
# include <sys/resource.h>
#endif

#include "AudioDaemon.h"

using namespace android;

// ---------------------------------------------------------------------------

int main(int argc, char** argv)
{
#if defined(HAVE_PTHREADS)
    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_AUDIO);
#endif


    ALOGV("Audio daemon starting sequence..");
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();

    sp<AudioDaemon> audioService = new AudioDaemon();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}       
          cmakeBuildDirectory: ${{ env.build }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
