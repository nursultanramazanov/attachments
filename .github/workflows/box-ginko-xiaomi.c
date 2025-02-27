name: C/C++ CI

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /${What:		/proc/sys/vm/nr_pdflush_threads
Date:		June 2012
Contact:	Wanpeng Li <liwp@linux.vnet.ibm.com>
Description: Since pdflush is replaced by per-BDI flusher, the interface of old pdflush
             exported in /proc/sys/vm/ should be removed. } --name ${What:		/sys/bus/usb/devices/.../power/level
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
		same thing. } --privileged tizenrt/tizenrt:${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/arvo/roccatarvo<minor>/actual_profile
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
Users:		http://roccat.sourceforge.net } /bin/bash
        docker cp ./. ${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/isku/roccatisku<minor>/actual_profile
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
Users:		http://roccat.sourceforge.net }:/${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/actual_profile
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
Users:		http://roccat.sourceforge.net }
        docker run -d --rm -it --name ${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/konepure/roccatkonepure<minor>/actual_profile
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
Users:		http://roccat.sourceforge.net } --volumes-from=${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_cpi
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
Users:		http://roccat.sourceforge.net } -w /${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/control
Date:		October 2012
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	When written, cpi, button and light settings can be configured.
		When read, actual cpi setting and sensor data are returned.
		The data has to be 8 bytes long.
Users:		http://roccat.sourceforge.net }/os --privileged tizenrt/tizenrt:${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/actual_cpi
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
Users:		http://roccat.sourceforge.net } /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec ${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/ryos/roccatryos<minor>/control
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
Users:		http://roccat.sourceforge.net } arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/savu/roccatsavu<minor>/buttons
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
Users:		http://roccat.sourceforge.net } bash -c "cd tools; ./configure.sh ${What:		/sys/firmware/acpi/hotplug/force_remove
Date:		Mar 2017
Contact:	Rafael J. Wysocki <rafael.j.wysocki@intel.com>
Description:
		Since the force_remove is inherently broken and dangerous to
		use for some hotplugable resources like memory (because ignoring
		the offline failure might lead to memory corruption and crashes)
		enabling this knob is not safe and thus unsupported. }"
        docker exec -it ${What:		/sys/class/gpio/
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
  replaced with the GPIO character device. } bash -c "make"


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
  push: /*
 *
 *  Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *  Not a Contribution, Apache license notifications and license are retained
 *  for attribution purposes only.
 *
 * Copyright (C) 2012 The Android Open Source Project
 * Copyright (C) 2018-2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H

// Disables read remote device feature
#define MAX_ACL_CONNECTIONS   16
#define MAX_L2CAP_CHANNELS    32
#define BLE_VND_INCLUDED   TRUE
#define GATT_MAX_PHY_CHANNEL  10

// Increasing SEPs to 12 from 6 to support SHO/MCast i.e. two streams per codec
#define AVDT_NUM_SEPS 35

#endif 
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
    - name: configure
      run: ./configure
    - name: make
      run: make
    - name: make check
      run: make check
    - name: make distcheck
      run: make distcheck
