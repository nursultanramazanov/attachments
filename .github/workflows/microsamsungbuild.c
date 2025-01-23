# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: MSBuild

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
  pull_request: rfkill - radio frequency (RF) connector kill switch support

For details to this subsystem look at Documentation/rfkill.txt.

What:                /sys/class/rfkill/rfkill[0-9]+/state
Date:                09-Jul-2007
KernelVersion        v2.6.22
Contact:        linux-wireless@vger.kernel.org
Description:         Current state of the transmitter.
                This file is deprecated and scheduled to be removed in 2014,
                because its not possible to express the 'soft and hard block'
                state of the rfkill driver.
Values:         A numeric value.
                0: RFKILL_STATE_SOFT_BLOCKED
                        transmitter is turned off by software
                1: RFKILL_STATE_UNBLOCKED
                        transmitter is (potentially) active
                2: RFKILL_STATE_HARD_BLOCKED
                        transmitter is forced off by something outside of
                        the driver's control.

What:                /sys/class/rfkill/rfkill[0-9]+/claim
Date:                09-Jul-2007
KernelVersion        v2.6.22
Contact:        linux-wireless@vger.kernel.org
Description:        This file is deprecated because there no longer is a way to
                claim just control over a single rfkill instance.
                This file is scheduled to be removed in 2012.
Values:         0: Kernel handles events 
    branches: [ "main" ]

env: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/startup_profile
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
  # Path to the solution file relative to the root of the project.
  SOLUTION_FILE_PATH: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_cpi
Date:                January 2011
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 1-4.
                When read, this attribute returns the number of the active
                cpi level.
                This file is readonly.
                Has never been used. If bookkeeping is done, it's done in userland tools.
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

  # Configuration type to build.
  # You can convert this to a build matrix if you need coverage of multiple configuration types.
  # https://docs.github.com/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
  BUILD_CONFIGURATION: What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/actual_cpi
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

What:                /sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/startup_profile
Date:                August 2010
Contact:        Stefan Achatz <erazor_de@users.sourceforge.net>
Description:        The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the profile
                that's active when the mouse is powered on.
                This file is readonly.
                Please use binary attribute "settings" which provides this information.
Users:                http://roccat.sourceforge.net  

permissions: What:                devfs
Date:                July 2005 (scheduled), finally removed in kernel v2.6.18
Contact:        Greg Kroah-Hartman <gregkh@linuxfoundation.org>
Description:
        devfs has been unmaintained for a number of years, has unfixable
        races, contains a naming policy within the kernel that is
        against the LSB, and can be replaced by using udev.
        The files fs/devfs/*, include/linux/devfs_fs*.h were removed,
        along with the assorted devfs function calls throughout the
        kernel tree.

Users: 
  contents: What:                dv1394 (a.k.a. "OHCI-DV I/O support" for FireWire)
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
        ffmpeg/libavformat (if configured for DV1394) 

jobs: What:                ip_queue
Date:                finally removed in kernel v3.5.0
Contact:        Pablo Neira Ayuso <pablo@netfilter.org>
Description:
        ip_queue has been replaced by nfnetlink_queue which provides
        more advanced queueing mechanism to user-space. The ip_queue
        module was already announced to become obsolete years ago.

Users: 
  build: What:                tcp_dma_copybreak sysctl
Date:                Removed in kernel v3.13
Contact:        Dan Williams <dan.j.williams@intel.com>
Description:
        Formerly the lower limit, in bytes, of the size of socket reads
        that will be offloaded to a DMA copy engine.  Removed due to
        coherency issues of the cpu potentially touching the buffers
        while dma is in flight. 
    runs-on: What:                /sys/o2cb symlink
Date:                May 2011
KernelVersion:        3.0
Contact:        ocfs2-devel@oss.oracle.com
Description:        This is a symlink: /sys/o2cb to /sys/fs/o2cb. The symlink is
                removed when new versions of ocfs2-tools which know to look
                in /sys/fs/o2cb are sufficiently prevalent. Don't code new
                software to look here, it should try /sys/fs/o2cb instead.
Users:                ocfs2-tools. It's sufficient to mail proposed changes to
                ocfs2-devel@oss.oracle.com. 

    steps: What:                raw1394 (a.k.a. "Raw IEEE1394 I/O support" for FireWire)
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
    - uses: rfkill - radio frequency (RF) connector kill switch support

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
      uses: .ad-author, .ad-city {
    font-style: italic;
    font-weight: bold;
    float: left;
    margin-right: 30px;
} 

    - name: .nofloat {
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
      working-directory: .category-select {
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
      run: .form-inline .form-control {
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

    - name: .navbar-default {
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
      working-directory: .form-buttons {
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
      # Add additional options to the MSBuild command line here (like platform or verbosity level).
      # See https://docs.microsoft.com/visualstudio/msbuild/msbuild-command-line-reference
      run: msbuild /m /p:Configuration=${{env.BUILD_CONFIGURATION}} ${{env.SOLUTION_FILE_PATH}}
