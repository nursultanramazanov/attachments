# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: MSBuild

on: What:		/proc/sys/vm/nr_pdflush_threads
Date:		June 2012
Contact:	Wanpeng Li <liwp@linux.vnet.ibm.com>
Description: Since pdflush is replaced by per-BDI flusher, the interface of old pdflush
             exported in /proc/sys/vm/ should be removed.  
  push: What:		/sys/bus/usb/devices/.../power/level
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
  pull_request: rfkill - radio frequency (RF) connector kill switch support

For details to this subsystem look at Documentation/rfkill.txt.

What:		/sys/class/rfkill/rfkill[0-9]+/state
Date:		09-Jul-2007
KernelVersion	v2.6.22
Contact:	linux-wireless@vger.kernel.org
Description: 	Current state of the transmitter.
		This file is deprecated and scheduled to be removed in 2014,
		because its not possible to express the 'soft and hard block'
		state of the rfkill driver.
Values: 	A numeric value.
		0: RFKILL_STATE_SOFT_BLOCKED
			transmitter is turned off by software
		1: RFKILL_STATE_UNBLOCKED
			transmitter is (potentially) active
		2: RFKILL_STATE_HARD_BLOCKED
			transmitter is forced off by something outside of
			the driver's control.

What:		/sys/class/rfkill/rfkill[0-9]+/claim
Date:		09-Jul-2007
KernelVersion	v2.6.22
Contact:	linux-wireless@vger.kernel.org
Description:	This file is deprecated because there no longer is a way to
		claim just control over a single rfkill instance.
		This file is scheduled to be removed in 2012.
Values: 	0: Kernel handles events      
    branches: [ "main" ]

env: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/koneplus/roccatkoneplus<minor>/startup_profile
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
  # Path to the solution file relative to the root of the project.
  SOLUTION_FILE_PATH: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/kovaplus/roccatkovaplus<minor>/actual_cpi
Date:		January 2011
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 1-4.
		When read, this attribute returns the number of the active
		cpi level.
		This file is readonly.
		Has never been used. If bookkeeping is done, it's done in userland tools.
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

  # Configuration type to build.
  # You can convert this to a build matrix if you need coverage of multiple configuration types.
  # https://docs.github.com/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
  BUILD_CONFIGURATION: What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/actual_cpi
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

What:		/sys/bus/usb/devices/<busnum>-<devnum>:<config num>.<interface num>/<hid-bus>:<vendor-id>:<product-id>.<num>/pyra/roccatpyra<minor>/startup_profile
Date:		August 2010
Contact:	Stefan Achatz <erazor_de@users.sourceforge.net>
Description:	The integer value of this attribute ranges from 0-4.
                When read, this attribute returns the number of the profile
                that's active when the mouse is powered on.
		This file is readonly.
		Please use binary attribute "settings" which provides this information.
Users:		http://roccat.sourceforge.net           

permissions: What:		devfs
Date:		July 2005 (scheduled), finally removed in kernel v2.6.18
Contact:	Greg Kroah-Hartman <gregkh@linuxfoundation.org>
Description:
	devfs has been unmaintained for a number of years, has unfixable
	races, contains a naming policy within the kernel that is
	against the LSB, and can be replaced by using udev.
	The files fs/devfs/*, include/linux/devfs_fs*.h were removed,
	along with the assorted devfs function calls throughout the
	kernel tree.

Users:          
  contents: What:		dv1394 (a.k.a. "OHCI-DV I/O support" for FireWire)
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

jobs: What:		ip_queue
Date:		finally removed in kernel v3.5.0
Contact:	Pablo Neira Ayuso <pablo@netfilter.org>
Description:
	ip_queue has been replaced by nfnetlink_queue which provides
	more advanced queueing mechanism to user-space. The ip_queue
	module was already announced to become obsolete years ago.

Users:        
  build: What:		/sys/o2cb symlink
Date:		May 2011
KernelVersion:	3.0
Contact:	ocfs2-devel@oss.oracle.com
Description:	This is a symlink: /sys/o2cb to /sys/fs/o2cb. The symlink is
		removed when new versions of ocfs2-tools which know to look
		in /sys/fs/o2cb are sufficiently prevalent. Don't code new
		software to look here, it should try /sys/fs/o2cb instead.
Users:		ocfs2-tools. It's sufficient to mail proposed changes to
		ocfs2-devel@oss.oracle.com.          
    runs-on: What:		raw1394 (a.k.a. "Raw IEEE1394 I/O support" for FireWire)
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

    steps: What:		video1394 (a.k.a. "OHCI-1394 Video support" for FireWire)
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
    - uses: #
# UAVCAN DSDL compiler for libuavcan
#
# Copyright (C) 'Ramazanov'
#

'''
This module implements the core functionality of the UAVCAN DSDL compiler for libuavcan.
Supported Python versions: 3.2+, 2.7.
It accepts a list of root namespaces and produces the set of C++ header files for libuavcan.
It is based on the DSDL parsing package from pyuavcan.
'''

from __future__ import division, absolute_import, print_function, unicode_literals
import sys, os, logging, errno, re
from .pyratemp import Template
from uavcan import dsdl

# Python 2.7 compatibility
try:
    str = unicode
except NameError:
    pass

OUTPUT_FILE_EXTENSION = 'hpp'
OUTPUT_FILE_PERMISSIONS = 0o444  # Read only for all
TEMPLATE_FILENAME = os.path.join(os.path.dirname(__file__), 'data_type_template.tmpl')

__all__ = ['run', 'logger', 'DsdlCompilerException']

class DsdlCompilerException(Exception):
    pass

logger = logging.getLogger(__name__)

def run(source_dirs, include_dirs, output_dir):
    '''
    This function takes a list of root namespace directories (containing DSDL definition files to parse), a
    possibly empty list of search directories (containing DSDL definition files that can be referenced from the types
    that are going to be parsed), and the output directory path (possibly nonexistent) where the generated C++
    header files will be stored.
    
    Note that this module features lazy write, i.e. if an output file does already exist and its content is not going
    to change, it will not be overwritten. This feature allows to avoid unnecessary recompilation of dependent object
    files.
    
    Args:
        source_dirs    List of root namespace directories to parse.
        include_dirs   List of root namespace directories with referenced types (possibly empty). This list is
                       automaitcally extended with source_dirs.
        output_dir     Output directory path. Will be created if doesn't exist.
    '''
    assert isinstance(source_dirs, list)
    assert isinstance(include_dirs, list)
    output_dir = str(output_dir)

    types = run_parser(source_dirs, include_dirs + source_dirs)
    if not types:
        die('No type definitions were found')

    logger.info('%d types total', len(types))
    run_generator(types, output_dir)

# -----------------

def pretty_filename(filename):
    try:
        a = os.path.abspath(filename)
        r = os.path.relpath(filename)
        return a if '..' in r else r
    except ValueError:
        return filename

def type_output_filename(t):
    assert t.category == t.CATEGORY_COMPOUND
    return t.full_name.replace('.', os.path.sep) + '.' + OUTPUT_FILE_EXTENSION

def makedirs(path):
    try:
        try:
            os.makedirs(path, exist_ok=True)  # May throw "File exists" when executed as root, which is wrong
        except TypeError:
            os.makedirs(path)  # Python 2.7 compatibility
    except OSError as ex:
        if ex.errno != errno.EEXIST:  # http://stackoverflow.com/questions/12468022
            raise

def die(text):
    raise DsdlCompilerException(str(text))

def run_parser(source_dirs, search_dirs):
    try:
        types = dsdl.parse_namespaces(source_dirs, search_dirs)
    except dsdl.DsdlException as ex:
        logger.info('Parser failure', exc_info=True)
        die(ex)
    return types

def run_generator(types, dest_dir):
    try:
        template_expander = make_template_expander(TEMPLATE_FILENAME)
        dest_dir = os.path.abspath(dest_dir)  # Removing '..'
        makedirs(dest_dir)
        for t in types:
            logger.info('Generating type %s', t.full_name)
            filename = os.path.join(dest_dir, type_output_filename(t))
            text = generate_one_type(template_expander, t)
            write_generated_data(filename, text)
    except Exception as ex:
        logger.info('Generator failure', exc_info=True)
        die(ex)

def write_generated_data(filename, data):
    dirname = os.path.dirname(filename)
    makedirs(dirname)

    # Lazy update - file will not be rewritten if its content is not going to change
    if os.path.exists(filename):
        with open(filename) as f:
            existing_data = f.read()
        if data == existing_data:
            logger.info('Up to date [%s]', pretty_filename(filename))
            return
        logger.info('Rewriting [%s]', pretty_filename(filename))
        os.remove(filename)
    else:
        logger.info('Creating [%s]', pretty_filename(filename))

    # Full rewrite
    with open(filename, 'w') as f:
        f.write(data)
    try:
        os.chmod(filename, OUTPUT_FILE_PERMISSIONS)
    except (OSError, IOError) as ex:
        logger.warning('Failed to set permissions for %s: %s', pretty_filename(filename), ex)

def type_to_cpp_type(t):
    if t.category == t.CATEGORY_PRIMITIVE:
        cast_mode = {
            t.CAST_MODE_SATURATED: '::uavcan::CastModeSaturate',
            t.CAST_MODE_TRUNCATED: '::uavcan::CastModeTruncate',
        }[t.cast_mode]
        if t.kind == t.KIND_FLOAT:
            return '::uavcan::FloatSpec< %d, %s >' % (t.bitlen, cast_mode)
        else:
            signedness = {
                t.KIND_BOOLEAN: '::uavcan::SignednessUnsigned',
                t.KIND_UNSIGNED_INT: '::uavcan::SignednessUnsigned',
                t.KIND_SIGNED_INT: '::uavcan::SignednessSigned',
            }[t.kind]
            return '::uavcan::IntegerSpec< %d, %s, %s >' % (t.bitlen, signedness, cast_mode)
    elif t.category == t.CATEGORY_ARRAY:
        value_type = type_to_cpp_type(t.value_type)
        mode = {
            t.MODE_STATIC: '::uavcan::ArrayModeStatic',
            t.MODE_DYNAMIC: '::uavcan::ArrayModeDynamic',
        }[t.mode]
        return '::uavcan::Array< %s, %s, %d >' % (value_type, mode, t.max_size)
    elif t.category == t.CATEGORY_COMPOUND:
        return '::' + t.full_name.replace('.', '::')
    elif t.category == t.CATEGORY_VOID:
        return '::uavcan::IntegerSpec< %d, ::uavcan::SignednessUnsigned, ::uavcan::CastModeSaturate >' % t.bitlen
    else:
        raise DsdlCompilerException('Unknown type category: %s' % t.category)

def generate_one_type(template_expander, t):
    t.short_name = t.full_name.split('.')[-1]
    t.cpp_type_name = t.short_name + '_'
    t.cpp_full_type_name = '::' + t.full_name.replace('.', '::')
    t.include_guard = t.full_name.replace('.', '_').upper() + '_HPP_INCLUDED'

    # Dependencies (no duplicates)
    def fields_includes(fields):
        def detect_include(t):
            if t.category == t.CATEGORY_COMPOUND:
                return type_output_filename(t)
            if t.category == t.CATEGORY_ARRAY:
                return detect_include(t.value_type)
        return list(sorted(set(filter(None, [detect_include(x.type) for x in fields]))))

    if t.kind == t.KIND_MESSAGE:
        t.cpp_includes = fields_includes(t.fields)
    else:
        t.cpp_includes = fields_includes(t.request_fields + t.response_fields)

    t.cpp_namespace_components = t.full_name.split('.')[:-1]
    t.has_default_dtid = t.default_dtid is not None

    # Attribute types
    def inject_cpp_types(attributes):
        void_index = 0
        for a in attributes:
            a.cpp_type = type_to_cpp_type(a.type)
            a.void = a.type.category == a.type.CATEGORY_VOID
            if a.void:
                assert not a.name
                a.name = '_void_%d' % void_index
                void_index += 1

    if t.kind == t.KIND_MESSAGE:
        inject_cpp_types(t.fields)
        inject_cpp_types(t.constants)
        t.all_attributes = t.fields + t.constants
        t.union = t.union and len(t.fields)
    else:
        inject_cpp_types(t.request_fields)
        inject_cpp_types(t.request_constants)
        inject_cpp_types(t.response_fields)
        inject_cpp_types(t.response_constants)
        t.all_attributes = t.request_fields + t.request_constants + t.response_fields + t.response_constants
        t.request_union = t.request_union and len(t.request_fields)
        t.response_union = t.response_union and len(t.response_fields)

    # Constant properties
    def inject_constant_info(constants):
        for c in constants:
            if c.type.kind == c.type.KIND_FLOAT:
                float(c.string_value)  # Making sure that this is a valid float literal
                c.cpp_value = c.string_value
            else:
                int(c.string_value)  # Making sure that this is a valid integer literal
                c.cpp_value = c.string_value
                if c.type.kind == c.type.KIND_UNSIGNED_INT:
                    c.cpp_value += 'U'

    if t.kind == t.KIND_MESSAGE:
        inject_constant_info(t.constants)
    else:
        inject_constant_info(t.request_constants)
        inject_constant_info(t.response_constants)

    # Data type kind
    t.cpp_kind = {
        t.KIND_MESSAGE: '::uavcan::DataTypeKindMessage',
        t.KIND_SERVICE: '::uavcan::DataTypeKindService',
    }[t.kind]

    # Generation
    text = template_expander(t=t)  # t for Type
    text = '\n'.join(x.rstrip() for x in text.splitlines())
    text = text.replace('\n\n\n\n\n', '\n\n').replace('\n\n\n\n', '\n\n').replace('\n\n\n', '\n\n')
    text = text.replace('{\n\n ', '{\n ')
    return text

def make_template_expander(filename):
    '''
    Templating is based on pyratemp (http://www.simple-is-better.org/template/pyratemp.html).
    The pyratemp's syntax is rather verbose and not so human friendly, so we define some
    custom extensions to make it easier to read and write.
    The resulting syntax somewhat resembles Mako (which was used earlier instead of pyratemp):
        Substitution:
            ${expression}
        Line joining through backslash (replaced with a single space):
            ${foo(bar(very_long_arument=42, \
                      second_line=72))}
        Blocks:
            % for a in range(10):
                % if a == 5:
                    ${foo()}
                % endif
            % endfor
    The extended syntax is converted into pyratemp's through regexp substitution.
    '''
    with open(filename) as f:
        template_text = f.read()

    # Backslash-newline elimination
    template_text = re.sub(r'\\\r{0,1}\n\ *', r' ', template_text)

    # Substitution syntax transformation: ${foo} ==> $!foo!$
    template_text = re.sub(r'([^\$]{0,1})\$\{([^\}]+)\}', r'\1$!\2!$', template_text)

    # Flow control expression transformation: % foo: ==> <!--(foo)-->
    template_text = re.sub(r'(?m)^(\ *)\%\ *(.+?):{0,1}$', r'\1<!--(\2)-->', template_text)

    # Block termination transformation: <!--(endfoo)--> ==> <!--(end)-->
    template_text = re.sub(r'\<\!--\(end[a-z]+\)--\>', r'<!--(end)-->', template_text)

    # Pyratemp workaround.
    # The problem is that if there's no empty line after a macro declaration, first line will be doubly indented.
    # Workaround:
    #  1. Remove trailing comments
    #  2. Add a newline after each macro declaration
    template_text = re.sub(r'\ *\#\!.*', '', template_text)
    template_text = re.sub(r'(\<\!--\(macro\ [a-zA-Z0-9_]+\)--\>.*?)', r'\1\n', template_text)

    # Preprocessed text output for debugging
#   with open(filename + '.d', 'w') as f:
#       f.write(template_text)

    template = Template(template_text)

    def expand(**args):
        # This function adds one indentation level (4 spaces); it will be used from the template
        args['indent'] = lambda text, idnt = '    ': idnt + text.replace('\n', '\n' + idnt)
        # This function works like enumerate(), telling you whether the current item is the last one
        def enum_last_value(iterable, start=0):
            it = iter(iterable)
            count = start
            last = next(it)
            for val in it:
                yield count, False, last
                last = val
                count += 1
            yield count, True, last
        args['enum_last_value'] = enum_last_value
        return template(**args)

    return expand               

    - name: /*
 * UAVCAN data structure definition for libuavcan.
 *
 * Autogenerated, do not edit.
 *
 * Source file: ${t.source_file}
 */

#ifndef ${t.include_guard}
#define ${t.include_guard}

#include <uavcan/build_config.hpp>
#include <uavcan/node/global_data_type_registry.hpp>
#include <uavcan/marshal/types.hpp>

% for inc in t.cpp_includes:
#include <${inc}>
% endfor

/******************************* Source text **********************************
% for line in t.source_text.strip().splitlines():
${line}
% endfor
******************************************************************************/

/********************* DSDL signature source definition ***********************
% for line in t.get_dsdl_signature_source_definition().splitlines():
${line}
% endfor
******************************************************************************/

% for a in t.all_attributes:
#undef ${a.name}
% endfor

% for nsc in t.cpp_namespace_components:
namespace ${nsc}
{
% endfor

% if t.kind != t.KIND_SERVICE:
template <int _tmpl>
% endif
struct UAVCAN_EXPORT ${t.cpp_type_name}
{
<!--(macro generate_primary_body)--> #! type_name, max_bitlen, fields, constants, union
    typedef const ${type_name}<_tmpl>& ParameterType;
    typedef ${type_name}<_tmpl>& ReferenceType;

    <!--(macro expand_attr_types)--> #! group_name, attrs
    struct ${group_name}
    {
        % for a in attrs:
        typedef ${a.cpp_type} ${a.name};
        % endfor
    };
    <!--(end)-->
    ${expand_attr_types(group_name='ConstantTypes', attrs=constants)}
    ${expand_attr_types(group_name='FieldTypes', attrs=fields)}

    % if union:
    
    struct Tag
    {
        enum Type
        {
        % for idx,last,a in enum_last_value(fields):
            ${a.name}${',' if not last else ''}
        % endfor
        };
    };

    typedef ::uavcan::IntegerSpec< ::uavcan::IntegerBitLen< ${len(fields)} >::Result,
                                   ::uavcan::SignednessUnsigned, ::uavcan::CastModeTruncate > TagType;

        <!--(macro expand_enum_per_field)--> #! enum_name, enum_comparator
    enum
    {
        ${enum_name} = TagType::BitLen +
            % for idx,last,a in enum_last_value(fields):
                % if not last:
            ::uavcan::${enum_comparator}<FieldTypes::${a.name}::${enum_name},
                % else:
                FieldTypes::${a.name}::${enum_name} ${'>::Result' * (len(fields) - 1)}
                % endif
            % endfor
    };
        <!--(end)-->

    ${expand_enum_per_field(enum_name='MinBitLen', enum_comparator='EnumMin')}
    ${expand_enum_per_field(enum_name='MaxBitLen', enum_comparator='EnumMax')}

    % else:

        <!--(macro expand_enum_per_field)--> #! enum_name
    enum
    {
        ${enum_name}
            % for idx,a in enumerate(fields):
            ${'=' if idx == 0 else '+'} FieldTypes::${a.name}::${enum_name}
            % endfor
    };
        <!--(end)-->

    ${expand_enum_per_field(enum_name='MinBitLen')}
    ${expand_enum_per_field(enum_name='MaxBitLen')}

    % endif

    // Constants
    % for a in constants:
    static const typename ::uavcan::StorageType< typename ConstantTypes::${a.name} >::Type ${a.name}; // ${a.init_expression}
    % endfor

    // Fields
    % for a in [x for x in fields if not x.void]:
    typename ::uavcan::StorageType< typename FieldTypes::${a.name} >::Type ${a.name};
    % endfor

    % if union:
private:
    typename ::uavcan::StorageType< TagType >::Type _tag_;  // The name is mangled to avoid clashing with fields

    template <typename Tag::Type T>
    struct TagToType;

public:
    % endif

    ${type_name}()
    % for idx,a in enumerate([x for x in fields if not x.void]):
        ${':' if idx == 0 else ','} ${a.name}()
    % endfor
    % if union:
        , _tag_()
    % endif
    {
        ::uavcan::StaticAssert<_tmpl == 0>::check();  // Usage check

#if UAVCAN_DEBUG
        /*
         * Cross-checking MaxBitLen provided by the DSDL compiler.
         * This check shall never be performed in user code because MaxBitLen value
         * actually depends on the nested types, thus it is not invariant.
         */
        ::uavcan::StaticAssert<${max_bitlen} == MaxBitLen>::check();
#endif
    }

    bool operator==(ParameterType rhs) const;
    bool operator!=(ParameterType rhs) const { return !operator==(rhs); }

    /**
     * This comparison is based on @ref uavcan::areClose(), which ensures proper comparison of
     * floating point fields at any depth.
     */
    bool isClose(ParameterType rhs) const;

    static int encode(ParameterType self, ::uavcan::ScalarCodec& codec,
                      ::uavcan::TailArrayOptimizationMode tao_mode = ::uavcan::TailArrayOptEnabled);

    static int decode(ReferenceType self, ::uavcan::ScalarCodec& codec,
                      ::uavcan::TailArrayOptimizationMode tao_mode = ::uavcan::TailArrayOptEnabled);

    % if union:
    /**
     * Explicit access to the tag.
     * It is safer to use is()/as()/to() instead.
     */
    typename Tag::Type getTag() const { return typename Tag::Type(_tag_); }
    void setTag(typename Tag::Type x) { _tag_ = typename ::uavcan::StorageType< TagType >::Type(x); }

    /**
     * Whether the union is set to the given type.
     * Access by tag; this will work even if there are non-unique types within the union.
     */
    bool is(typename Tag::Type x) const { return typename Tag::Type(_tag_) == x; }

    /**
     * If the union is currently set to the type T, returns pointer to the appropriate field.
     * If the union is set to another type, returns null pointer.
     */
    template <typename Tag::Type T>
    inline const typename TagToType<T>::StorageType* as() const;

    /**
     * Switches the union to the given type and returns a mutable reference to the appropriate field.
     * If the previous type was different, a default constructor will be called first.
     */
    template <typename Tag::Type T>
    inline typename TagToType<T>::StorageType& to();
    % endif
<!--(end)-->

% if t.kind == t.KIND_SERVICE:
    template <int _tmpl>
    struct Request_
    {
        ${indent(generate_primary_body(type_name='Request_', max_bitlen=t.get_max_bitlen_request(), \
                                       fields=t.request_fields, constants=t.request_constants, \
                                       union=t.request_union))}
    };

    template <int _tmpl>
    struct Response_
    {
        ${indent(generate_primary_body(type_name='Response_', max_bitlen=t.get_max_bitlen_response(), \
                                       fields=t.response_fields, constants=t.response_constants, \
                                       union=t.response_union))}
    };

    typedef Request_<0> Request;
    typedef Response_<0> Response;
% else:
    ${generate_primary_body(type_name=t.cpp_type_name, max_bitlen=t.get_max_bitlen(), \
                            fields=t.fields, constants=t.constants, union=t.union)}
% endif

    /*
     * Static type info
     */
    enum { DataTypeKind = ${t.cpp_kind} };
% if t.has_default_dtid:
    enum { DefaultDataTypeID = ${t.default_dtid} };
% else:
    // This type has no default data type ID
% endif

    static const char* getDataTypeFullName()
    {
        return "${t.full_name}";
    }

    static void extendDataTypeSignature(::uavcan::DataTypeSignature& signature)
    {
        signature.extend(getDataTypeSignature());
    }

    static ::uavcan::DataTypeSignature getDataTypeSignature();

% if t.kind == t.KIND_SERVICE:
private:
    ${t.cpp_type_name}(); // Don't create objects of this type. Use Request/Response instead.
% endif
};

/*
 * Out of line struct method definitions
 */
<!--(macro define_out_of_line_struct_methods)--> #! scope_prefix, fields, union

template <int _tmpl>
bool ${scope_prefix}<_tmpl>::operator==(ParameterType rhs) const
{
    % if union:
    if (_tag_ != rhs._tag_)
    {
        return false;
    }
        % for idx,a in enumerate(fields):
    if (_tag_ == ${idx})
    {
        return ${a.name} == rhs.${a.name};
    }
        % endfor
    UAVCAN_ASSERT(0);   // Invalid tag
    return false;
    % else:
        % if fields:
    return
            % for idx,last,a in enum_last_value([x for x in fields if not x.void]):
        ${a.name} == rhs.${a.name}${' &&' if not last else ';'}
            % endfor
        % else:
    (void)rhs;
    return true;
        % endif
    % endif
}

template <int _tmpl>
bool ${scope_prefix}<_tmpl>::isClose(ParameterType rhs) const
{
    % if union:
    if (_tag_ != rhs._tag_)
    {
        return false;
    }
        % for idx,a in enumerate(fields):
    if (_tag_ == ${idx})
    {
        return ::uavcan::areClose(${a.name}, rhs.${a.name});
    }
        % endfor
    UAVCAN_ASSERT(0);   // Invalid tag
    return false;
    % else:
        % if fields:
    return
            % for idx,last,a in enum_last_value([x for x in fields if not x.void]):
        ::uavcan::areClose(${a.name}, rhs.${a.name})${' &&' if not last else ';'}
            % endfor
        % else:
    (void)rhs;
    return true;
        % endif
    % endif
}

    <!--(macro generate_codec_calls_per_field)--> #! call_name, self_parameter_type
template <int _tmpl>
int ${scope_prefix}<_tmpl>::${call_name}(${self_parameter_type} self, ::uavcan::ScalarCodec& codec,
    ::uavcan::TailArrayOptimizationMode tao_mode)
{
    (void)self;
    (void)codec;
    (void)tao_mode;
        % if union:
    const int res = TagType::${call_name}(self._tag_, codec, ::uavcan::TailArrayOptDisabled);
    if (res <= 0)
    {
        return res;
    }
            % for idx,a in enumerate(fields):
    if (self._tag_ == ${idx})
    {
        return FieldTypes::${a.name}::${call_name}(self.${a.name}, codec, tao_mode);
    }
            % endfor
    return -1;          // Invalid tag value
        % else:
            % for a in [x for x in fields if x.void]:
    typename ::uavcan::StorageType< typename FieldTypes::${a.name} >::Type ${a.name} = 0;
            % endfor
    int res = 1;
            % for idx,last,a in enum_last_value(fields):
    res = FieldTypes::${a.name}::${call_name}(${'self.' * (not a.void)}${a.name}, codec, \
${'::uavcan::TailArrayOptDisabled' if not last else 'tao_mode'});
                % if not last:
    if (res <= 0)
    {
        return res;
    }
                % endif
            % endfor
    return res;
        % endif
}
    <!--(end)-->
${generate_codec_calls_per_field(call_name='encode', self_parameter_type='ParameterType')}
${generate_codec_calls_per_field(call_name='decode', self_parameter_type='ReferenceType')}

    % if union:
        % for idx,a in enumerate(fields):
template <>
template <>
struct ${scope_prefix}<0>::TagToType<${scope_prefix}<0>::Tag::${a.name}>
{
    typedef typename ${scope_prefix}<0>::FieldTypes::${a.name} Type;
    typedef typename ::uavcan::StorageType<Type>::Type StorageType;
};

template <>
template <>
inline const typename ${scope_prefix}<0>::TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType*
${scope_prefix}<0>::as< ${scope_prefix}<0>::Tag::${a.name} >() const
{
    return is(${scope_prefix}<0>::Tag::${a.name}) ? &${a.name} : UAVCAN_NULLPTR;
}

template <>
template <>
inline typename ${scope_prefix}<0>::TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType&
${scope_prefix}<0>::to< ${scope_prefix}<0>::Tag::${a.name} >()
{
    if (_tag_ != ${idx})
    {
        _tag_ = ${idx};
        ${a.name} = typename TagToType< ${scope_prefix}<0>::Tag::${a.name} >::StorageType();
    }
    return ${a.name};
}

        % endfor
    % endif
<!--(end)-->

% if t.kind == t.KIND_SERVICE:
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name + '::Request_', fields=t.request_fields, \
                                    union=t.request_union)}
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name + '::Response_', fields=t.response_fields, \
                                    union=t.response_union)}
% else:
${define_out_of_line_struct_methods(scope_prefix=t.cpp_type_name, fields=t.fields, union=t.union)}
% endif

/*
 * Out of line type method definitions
 */
% if t.kind == t.KIND_SERVICE:
inline ::uavcan::DataTypeSignature ${t.cpp_type_name}::getDataTypeSignature()
% else:
template <int _tmpl>
::uavcan::DataTypeSignature ${t.cpp_type_name}<_tmpl>::getDataTypeSignature()
% endif
{
    ::uavcan::DataTypeSignature signature(${'0x%08X' % t.get_dsdl_signature()}ULL);
<!--(macro extend_signature_per_field)--> #! scope_prefix, fields
    % for a in fields:
    ${scope_prefix}FieldTypes::${a.name}::extendDataTypeSignature(signature);
    % endfor
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${extend_signature_per_field(scope_prefix='Request::', fields=t.request_fields)}
${extend_signature_per_field(scope_prefix='Response::', fields=t.response_fields)}
% else:
${extend_signature_per_field(scope_prefix='', fields=t.fields)}
% endif
    return signature;
}

/*
 * Out of line constant definitions
 */
<!--(macro define_out_of_line_constants)--> #! scope_prefix, constants
    % for a in constants:
template <int _tmpl>
const typename ::uavcan::StorageType< typename ${scope_prefix}<_tmpl>::ConstantTypes::${a.name} >::Type
    ${scope_prefix}<_tmpl>::${a.name} = ${a.cpp_value}; // ${a.init_expression}

    % endfor
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_out_of_line_constants(scope_prefix=t.cpp_type_name + '::Request_', constants=t.request_constants)}
${define_out_of_line_constants(scope_prefix=t.cpp_type_name + '::Response_', constants=t.response_constants)}
% else:
${define_out_of_line_constants(scope_prefix=t.cpp_type_name, constants=t.constants)}
% endif

/*
 * Final typedef
 */
% if t.kind == t.KIND_SERVICE:
typedef ${t.cpp_type_name} ${t.short_name};
% else:
typedef ${t.cpp_type_name}<0> ${t.short_name};
% endif

% if t.has_default_dtid:
namespace
{

const ::uavcan::DefaultDataTypeRegistrator< ${t.cpp_full_type_name} > _uavcan_gdtr_registrator_${t.short_name};

}
% else:
// No default registration
% endif

% for nsc in t.cpp_namespace_components[::-1]:
} // Namespace ${nsc}
% endfor

/*
 * YAML streamer specialization
 */
namespace uavcan
{

<!--(macro define_yaml_streamer)--> #! type_name, fields, union
template <>
class UAVCAN_EXPORT YamlStreamer< ${type_name} >
{
public:
    template <typename Stream>
    static void stream(Stream& s, ${type_name}::ParameterType obj, const int level);
};

template <typename Stream>
void YamlStreamer< ${type_name} >::stream(Stream& s, ${type_name}::ParameterType obj, const int level)
{
    (void)s;
    (void)obj;
    (void)level;
    % if union:
    if (level > 0)
    {
        s << '\n';
        for (int pos = 0; pos < level; pos++)
        {
            s << "  ";
        }
    }
        % for idx,a in enumerate(fields):
    if (static_cast<int>(obj.getTag()) == ${idx})
    {
        s << "${a.name}: ";
        YamlStreamer< ${type_name}::FieldTypes::${a.name} >::stream(s, obj.${a.name}, level + 1);
    }
        % endfor
    % else:
        % for idx,a in enumerate([x for x in fields if not x.void]):
            % if idx == 0:
    if (level > 0)
    {
        s << '\n';
        for (int pos = 0; pos < level; pos++)
        {
            s << "  ";
        }
    }
            % else:
    s << '\n';
    for (int pos = 0; pos < level; pos++)
    {
        s << "  ";
    }
            % endif
    s << "${a.name}: ";
    YamlStreamer< ${type_name}::FieldTypes::${a.name} >::stream(s, obj.${a.name}, level + 1);
        % endfor
    % endif
}
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_yaml_streamer(type_name=t.cpp_full_type_name + '::Request', fields=t.request_fields, union=t.request_union)}
${define_yaml_streamer(type_name=t.cpp_full_type_name + '::Response', fields=t.response_fields, union=t.response_union)}
% else:
${define_yaml_streamer(type_name=t.cpp_full_type_name, fields=t.fields, union=t.union)}
% endif

}

% for nsc in t.cpp_namespace_components:
namespace ${nsc}
{
% endfor

<!--(macro define_streaming_operator)--> #! type_name
template <typename Stream>
inline Stream& operator<<(Stream& s, ${type_name}::ParameterType obj)
{
    ::uavcan::YamlStreamer< ${type_name} >::stream(s, obj, 0);
    return s;
}
<!--(end)-->
% if t.kind == t.KIND_SERVICE:
${define_streaming_operator(type_name=t.cpp_full_type_name + '::Request')}
${define_streaming_operator(type_name=t.cpp_full_type_name + '::Response')}
% else:
${define_streaming_operator(type_name=t.cpp_full_type_name)}
% endif

% for nsc in t.cpp_namespace_components[::-1]:
} // Namespace ${nsc}
% endfor

#endif // ${t.include_guard}          
      uses: #!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Small, simple and powerful template-engine for Python.

A template-engine for Python, which is very simple, easy to use, small,
fast, powerful, modular, extensible, well documented and pythonic.

See documentation for a list of features, template-syntax etc.

:Version:   0.3.0
:Requires:  Python >=2.6 / 3.x

:Usage:
    see class ``Template`` and examples below.

:Example:

    Note that the examples are in Python 2; they also work in
    Python 3 if you replace u"..." by "...", unicode() by str()
    and partly "..." by b"...".

    quickstart::
        >>> t = Template("hello @!name!@")
        >>> print(t(name="marvin"))
        hello marvin

    quickstart with a template-file::
        # >>> t = Template(filename="mytemplate.tmpl")
        # >>> print(t(name="marvin"))
        # hello marvin

    generic usage::
        >>> t = Template(u"output is in Unicode \\xe4\\xf6\\xfc\\u20ac")
        >>> t                                           #doctest: +ELLIPSIS
        <...Template instance at 0x...>
        >>> t()
        u'output is in Unicode \\xe4\\xf6\\xfc\\u20ac'
        >>> unicode(t)
        u'output is in Unicode \\xe4\\xf6\\xfc\\u20ac'

    with data::
        >>> t = Template("hello @!name!@", data={"name":"world"})
        >>> t()
        u'hello world'
        >>> t(name="worlds")
        u'hello worlds'

        # >>> t(note="data must be Unicode or ASCII", name=u"\\xe4")
        # u'hello \\xe4'

    escaping::
        >>> t = Template("hello escaped: @!name!@, unescaped: $!name!$")
        >>> t(name='''<>&'"''')
        u'hello escaped: &lt;&gt;&amp;&#39;&quot;, unescaped: <>&\\'"'

    result-encoding::
        # encode the unicode-object to your encoding with encode()
        >>> t = Template(u"hello \\xe4\\xf6\\xfc\\u20ac")
        >>> result = t()
        >>> result
        u'hello \\xe4\\xf6\\xfc\\u20ac'
        >>> result.encode("utf-8")
        'hello \\xc3\\xa4\\xc3\\xb6\\xc3\\xbc\\xe2\\x82\\xac'
        >>> result.encode("ascii")
        Traceback (most recent call last):
          ...
        UnicodeEncodeError: 'ascii' codec can't encode characters in position 6-9: ordinal not in range(128)
        >>> result.encode("ascii", 'xmlcharrefreplace')
        'hello &#228;&#246;&#252;&#8364;'

    Python-expressions::
        >>> Template('formatted: @! "%8.5f" % value !@')(value=3.141592653)
        u'formatted:  3.14159'
        >>> Template("hello --@!name.upper().center(20)!@--")(name="world")
        u'hello --       WORLD        --'
        >>> Template("calculate @!var*5+7!@")(var=7)
        u'calculate 42'

    blocks (if/for/macros/...)::
        >>> t = Template("<!--(if foo == 1)-->bar<!--(elif foo == 2)-->baz<!--(else)-->unknown(@!foo!@)<!--(end)-->")
        >>> t(foo=2)
        u'baz'
        >>> t(foo=5)
        u'unknown(5)'

        >>> t = Template("<!--(for i in mylist)-->@!i!@ <!--(else)-->(empty)<!--(end)-->")
        >>> t(mylist=[])
        u'(empty)'
        >>> t(mylist=[1,2,3])
        u'1 2 3 '

        >>> t = Template("<!--(for i,elem in enumerate(mylist))--> - @!i!@: @!elem!@<!--(end)-->")
        >>> t(mylist=["a","b","c"])
        u' - 0: a - 1: b - 2: c'

        >>> t = Template('<!--(macro greetings)-->hello <strong>@!name!@</strong><!--(end)-->  @!greetings(name=user)!@')
        >>> t(user="monty")
        u'  hello <strong>monty</strong>'

    exists::
        >>> t = Template('<!--(if exists("foo"))-->YES<!--(else)-->NO<!--(end)-->')
        >>> t()
        u'NO'
        >>> t(foo=1)
        u'YES'
        >>> t(foo=None)       # note this difference to 'default()'
        u'YES'

    default-values::
        # non-existing variables raise an error
        >>> Template('hi @!optional!@')()
        Traceback (most recent call last):
          ...
        TemplateRenderError: Cannot eval expression 'optional'. (NameError: name 'optional' is not defined)

        >>> t = Template('hi @!default("optional","anyone")!@')
        >>> t()
        u'hi anyone'
        >>> t(optional=None)
        u'hi anyone'
        >>> t(optional="there")
        u'hi there'

        # the 1st parameter can be any eval-expression
        >>> t = Template('@!default("5*var1+var2","missing variable")!@')
        >>> t(var1=10)
        u'missing variable'
        >>> t(var1=10, var2=2)
        u'52'

        # also in blocks
        >>> t = Template('<!--(if default("opt1+opt2",0) > 0)-->yes<!--(else)-->no<!--(end)-->')
        >>> t()
        u'no'
        >>> t(opt1=23, opt2=42)
        u'yes'

        >>> t = Template('<!--(for i in default("optional_list",[]))-->@!i!@<!--(end)-->')
        >>> t()
        u''
        >>> t(optional_list=[1,2,3])
        u'123'


        # but make sure to put the expression in quotation marks, otherwise:
        >>> Template('@!default(optional,"fallback")!@')()
        Traceback (most recent call last):
          ...
        TemplateRenderError: Cannot eval expression 'default(optional,"fallback")'. (NameError: name 'optional' is not defined)

    setvar::
        >>> t = Template('$!setvar("i", "i+1")!$@!i!@')
        >>> t(i=6)
        u'7'

        >>> t = Template('''<!--(if isinstance(s, (list,tuple)))-->$!setvar("s", '"\\\\\\\\n".join(s)')!$<!--(end)-->@!s!@''')
        >>> t(isinstance=isinstance, s="123")
        u'123'
        >>> t(isinstance=isinstance, s=["123", "456"])
        u'123\\n456'

:Author: 
:Copyright:
:License:   MIT/X11-like, see __license__

:RCS:       $Id: pyratemp.py,v 1.12 2013/04/02 20:26:06 rk Exp $
"""
from __future__ import unicode_literals

__version__ = "0.3.0'
__author__   =
__license__  =

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE."""

#=========================================

import os, re, sys
if sys.version_info[0] >= 3:
    import builtins
    unicode = str
    long = int
else:
    import __builtin__ as builtins
    from codecs import open

#=========================================
# some useful functions

#----------------------
# string-position: i <-> row,col

def srow(string, i):
    """Get line numer of ``string[i]`` in `string`.

    :Returns: row, starting at 1
    :Note:    This works for text-strings with ``\\n`` or ``\\r\\n``.
    """
    return string.count('\n', 0, max(0, i)) + 1

def scol(string, i):
    """Get column number of ``string[i]`` in `string`.

    :Returns: column, starting at 1 (but may be <1 if i<0)
    :Note:    This works for text-strings with ``\\n`` or ``\\r\\n``.
    """
    return i - string.rfind('\n', 0, max(0, i))

def sindex(string, row, col):
    """Get index of the character at `row`/`col` in `string`.

    :Parameters:
        - `row`: row number, starting at 1.
        - `col`: column number, starting at 1.
    :Returns:    ``i``, starting at 0 (but may be <1 if row/col<0)
    :Note:       This works for text-strings with '\\n' or '\\r\\n'.
    """
    n = 0
    for _ in range(row-1):
        n = string.find('\n', n) + 1
    return n+col-1

#----------------------

def dictkeyclean(d):
    """Convert all keys of the dict `d` to strings.
    """
    new_d = {}
    for k, v in d.items():
        new_d[str(k)] = v
    return new_d

#----------------------

def dummy(*_, **__):
    """Dummy function, doing nothing.
    """
    pass

def dummy_raise(exception, value):
    """Create an exception-raising dummy function.

    :Returns: dummy function, raising ``exception(value)``
    """
    def mydummy(*_, **__):
        raise exception(value)
    return mydummy

#=========================================
# escaping

(NONE, HTML, LATEX, MAIL_HEADER) = range(0, 4)
ESCAPE_SUPPORTED = {"NONE":None, "HTML":HTML, "LATEX":LATEX, "MAIL_HEADER":MAIL_HEADER}

def escape(s, format=HTML):
    """Replace special characters by their escape sequence.

    :Parameters:
        - `s`: unicode-string to escape
        - `format`:

          - `NONE`:  nothing is replaced
          - `HTML`:  replace &<>'" by &...;
          - `LATEX`: replace \#$%&_{}~^
          - `MAIL_HEADER`: escape non-ASCII mail-header-contents
    :Returns:
        the escaped string in unicode
    :Exceptions:
        - `ValueError`: if `format` is invalid.

    :Uses:
        MAIL_HEADER uses module email
    """
    #Note: If you have to make sure that every character gets replaced
    #      only once (and if you cannot achieve this with the following code),
    #      use something like "".join([replacedict.get(c,c) for c in s])
    #      which is about 2-3 times slower (but maybe needs less memory).
    #Note: This is one of the most time-consuming parts of the template.
    if format is None or format == NONE:
        pass
    elif format == HTML:
        s = s.replace("&", "&amp;") # must be done first!
        s = s.replace("<", "&lt;")
        s = s.replace(">", "&gt;")
        s = s.replace('"', "&quot;")
        s = s.replace("'", "&#39;")
    elif format == LATEX:
        s = s.replace("\\", "\\x")    #must be done first!
        s = s.replace("#",  "\\#")
        s = s.replace("$",  "\\$")
        s = s.replace("%",  "\\%")
        s = s.replace("&",  "\\&")
        s = s.replace("_",  "\\_")
        s = s.replace("{",  "\\{")
        s = s.replace("}",  "\\}")
        s = s.replace("\\x","\\textbackslash{}")
        s = s.replace("~",  "\\textasciitilde{}")
        s = s.replace("^",  "\\textasciicircum{}")
    elif format == MAIL_HEADER:
        import email.header
        try:
            s.encode("ascii")
            return s
        except UnicodeEncodeError:
            return email.header.make_header([(s, "utf-8")]).encode()
    else:
        raise ValueError('Invalid format (only None, HTML, LATEX and MAIL_HEADER are supported).')
    return s

#=========================================

#-----------------------------------------
# Exceptions

class TemplateException(Exception):
    """Base class for template-exceptions."""
    pass

class TemplateParseError(TemplateException):
    """Template parsing failed."""
    def __init__(self, err, errpos):
        """
        :Parameters:
            - `err`:    error-message or exception to wrap
            - `errpos`: ``(filename,row,col)`` where the error occured.
        """
        self.err = err
        self.filename, self.row, self.col = errpos
        TemplateException.__init__(self)
    def __str__(self):
        if not self.filename:
            return "line %d, col %d: %s" % (self.row, self.col, str(self.err))
        else:
            return "file %s, line %d, col %d: %s" % (self.filename, self.row, self.col, str(self.err))

class TemplateSyntaxError(TemplateParseError, SyntaxError):
    """Template syntax-error."""
    pass

class TemplateIncludeError(TemplateParseError):
    """Template 'include' failed."""
    pass

class TemplateRenderError(TemplateException):
    """Template rendering failed."""
    pass

#-----------------------------------------
# Loader

class LoaderString:
    """Load template from a string/unicode.

    Note that 'include' is not possible in such templates.
    """
    def __init__(self, encoding='utf-8'):
        self.encoding = encoding

    def load(self, s):
        """Return template-string as unicode.
        """
        if isinstance(s, unicode):
            u = s
        else:
            u = s.decode(self.encoding)
        return u

class LoaderFile:
    """Load template from a file.

    When loading a template from a file, it's possible to including other
    templates (by using 'include' in the template). But for simplicity
    and security, all included templates have to be in the same directory!
    (see ``allowed_path``)
    """
    def __init__(self, allowed_path=None, encoding='utf-8'):
        """Init the loader.

        :Parameters:
            - `allowed_path`: path of the template-files
            - `encoding`: encoding of the template-files
        :Exceptions:
            - `ValueError`: if `allowed_path` is not a directory
        """
        if allowed_path and not os.path.isdir(allowed_path):
            raise ValueError("'allowed_path' has to be a directory.")
        self.path     = allowed_path
        self.encoding = encoding

    def load(self, filename):
        """Load a template from a file.

        Check if filename is allowed and return its contens in unicode.

        :Parameters:
            - `filename`: filename of the template without path
        :Returns:
            the contents of the template-file in unicode
        :Exceptions:
            - `ValueError`: if `filename` contains a path
        """
        if filename != os.path.basename(filename):
            raise ValueError("No path allowed in filename. (%s)" %(filename))
        filename = os.path.join(self.path, filename)

        f = open(filename, 'r', encoding=self.encoding)
        u = f.read()
        f.close()

        return u

#-----------------------------------------
# Parser

class Parser(object):
    """Parse a template into a parse-tree.

    Includes a syntax-check, an optional expression-check and verbose
    error-messages.

    See documentation for a description of the parse-tree.
    """
    # template-syntax
    _comment_start = "#!"
    _comment_end   = "!#"
    _sub_start     = "$!"
    _sub_end       = "!$"
    _subesc_start  = "@!"
    _subesc_end    = "!@"
    _block_start   = "<!--("
    _block_end     = ")-->"

    # build regexps
    # comment
    #   single-line, until end-tag or end-of-line.
    _strComment = r"""%s(?P<content>.*?)(?P<end>%s|\n|$)""" \
                    % (re.escape(_comment_start), re.escape(_comment_end))
    _reComment = re.compile(_strComment, re.M)

    # escaped or unescaped substitution
    #   single-line ("|$" is needed to be able to generate good error-messges)
    _strSubstitution = r"""
                    (
                    %s\s*(?P<sub>.*?)\s*(?P<end>%s|$)       #substitution
                    |
                    %s\s*(?P<escsub>.*?)\s*(?P<escend>%s|$) #escaped substitution
                    )
                """ % (re.escape(_sub_start),    re.escape(_sub_end),
                       re.escape(_subesc_start), re.escape(_subesc_end))
    _reSubstitution = re.compile(_strSubstitution, re.X|re.M)

    # block
    #   - single-line, no nesting.
    #   or
    #   - multi-line, nested by whitespace indentation:
    #       * start- and end-tag of a block must have exactly the same indentation.
    #       * start- and end-tags of *nested* blocks should have a greater indentation.
    # NOTE: A single-line block must not start at beginning of the line with
    #       the same indentation as the enclosing multi-line blocks!
    #       Note that "       " and "\t" are different, although they may
    #       look the same in an editor!
    _s = re.escape(_block_start)
    _e = re.escape(_block_end)
    _strBlock = r"""
                    ^(?P<mEnd>[ \t]*)%send%s(?P<meIgnored>.*)\r?\n?   # multi-line end  (^   <!--(end)-->IGNORED_TEXT\n)
                    |
                    (?P<sEnd>)%send%s                               # single-line end (<!--(end)-->)
                    |
                    (?P<sSpace>[ \t]*)                              # single-line tag (no nesting)
                    %s(?P<sKeyw>\w+)[ \t]*(?P<sParam>.*?)%s
                    (?P<sContent>.*?)
                    (?=(?:%s.*?%s.*?)??%send%s)                     # (match until end or i.e. <!--(elif/else...)-->)
                    |
                                                                    # multi-line tag, nested by whitespace indentation
                    ^(?P<indent>[ \t]*)                             #   save indentation of start tag
                    %s(?P<mKeyw>\w+)\s*(?P<mParam>.*?)%s(?P<mIgnored>.*)\r?\n
                    (?P<mContent>(?:.*\n)*?)
                    (?=(?P=indent)%s(?:.|\s)*?%s)                   #   match indentation
                """ % (_s, _e,
                       _s, _e,
                       _s, _e, _s, _e, _s, _e,
                       _s, _e, _s, _e)
    _reBlock = re.compile(_strBlock, re.X|re.M)

    # "for"-block parameters: "var(,var)* in ..."
    _strForParam = r"""^(?P<names>\w+(?:\s*,\s*\w+)*)\s+in\s+(?P<iter>.+)$"""
    _reForParam  = re.compile(_strForParam)

    # allowed macro-names
    _reMacroParam = re.compile(r"""^\w+$""")


    def __init__(self, loadfunc=None, testexpr=None, escape=HTML):
        """Init the parser.

        :Parameters:
            - `loadfunc`: function to load included templates
              (i.e. ``LoaderFile(...).load``)
            - `testexpr`: function to test if a template-expressions is valid
              (i.e. ``EvalPseudoSandbox().compile``)
            - `escape`:   default-escaping (may be modified by the template)
        :Exceptions:
            - `ValueError`: if `testexpr` or `escape` is invalid.
        """
        if loadfunc is None:
            self._load = dummy_raise(NotImplementedError, "'include' not supported, since no 'loadfunc' was given.")
        else:
            self._load = loadfunc

        if testexpr is None:
            self._testexprfunc = dummy
        else:
            try:    # test if testexpr() works
                testexpr("i==1")
            except Exception as err:
                raise ValueError("Invalid 'testexpr'. (%s)" %(err))
            self._testexprfunc = testexpr

        if escape not in ESCAPE_SUPPORTED.values():
            raise ValueError("Unsupported 'escape'. (%s)" %(escape))
        self.escape = escape
        self._includestack = []

    def parse(self, template):
        """Parse a template.

        :Parameters:
            - `template`: template-unicode-string
        :Returns:         the resulting parse-tree
        :Exceptions:
            - `TemplateSyntaxError`: for template-syntax-errors
            - `TemplateIncludeError`: if template-inclusion failed
            - `TemplateException`
        """
        self._includestack = [(None, template)]   # for error-messages (_errpos)
        return self._parse(template)

    def _errpos(self, fpos):
        """Convert `fpos` to ``(filename,row,column)`` for error-messages."""
        filename, string = self._includestack[-1]
        return filename, srow(string, fpos), scol(string, fpos)

    def _testexpr(self, expr,  fpos=0):
        """Test a template-expression to detect errors."""
        try:
            self._testexprfunc(expr)
        except SyntaxError as err:
            raise TemplateSyntaxError(err, self._errpos(fpos))

    def _parse_sub(self, parsetree, text, fpos=0):
        """Parse substitutions, and append them to the parse-tree.

        Additionally, remove comments.
        """
        curr = 0
        for match in self._reSubstitution.finditer(text):
            start = match.start()
            if start > curr:
                parsetree.append(("str", self._reComment.sub('', text[curr:start])))

            if match.group("sub") is not None:
                if not match.group("end"):
                    raise TemplateSyntaxError("Missing closing tag '%s' for '%s'."
                            % (self._sub_end, match.group()), self._errpos(fpos+start))
                if len(match.group("sub")) > 0:
                    self._testexpr(match.group("sub"), fpos+start)
                    parsetree.append(("sub", match.group("sub")))
            else:
                assert(match.group("escsub") is not None)
                if not match.group("escend"):
                    raise TemplateSyntaxError("Missing closing tag '%s' for '%s'."
                            % (self._subesc_end, match.group()), self._errpos(fpos+start))
                if len(match.group("escsub")) > 0:
                    self._testexpr(match.group("escsub"), fpos+start)
                    parsetree.append(("esc", self.escape, match.group("escsub")))

            curr = match.end()

        if len(text) > curr:
            parsetree.append(("str", self._reComment.sub('', text[curr:])))

    def _parse(self, template, fpos=0):
        """Recursive part of `parse()`.

        :Parameters:
            - template
            - fpos: position of ``template`` in the complete template (for error-messages)
        """
        # blank out comments
        # (So that its content does not collide with other syntax, and
        #  because removing them completely would falsify the character-
        #  position ("match.start()") of error-messages)
        template = self._reComment.sub(lambda match: self._comment_start+" "*len(match.group(1))+match.group(2), template)

        # init parser
        parsetree = []
        curr = 0            # current position (= end of previous block)
        block_type = None   # block type: if,for,macro,raw,...
        block_indent = None # None: single-line, >=0: multi-line

        # find blocks
        for match in self._reBlock.finditer(template):
            start = match.start()
            # process template-part before this block
            if start > curr:
                self._parse_sub(parsetree, template[curr:start], fpos)

            # analyze block syntax (incl. error-checking and -messages)
            keyword = None
            block = match.groupdict()
            pos__ = fpos + start                # shortcut
            if   block["sKeyw"] is not None:    # single-line block tag
                block_indent = None
                keyword = block["sKeyw"]
                param   = block["sParam"]
                content = block["sContent"]
                if block["sSpace"]:             # restore spaces before start-tag
                    if len(parsetree) > 0 and parsetree[-1][0] == "str":
                        parsetree[-1] = ("str", parsetree[-1][1] + block["sSpace"])
                    else:
                        parsetree.append(("str", block["sSpace"]))
                pos_p = fpos + match.start("sParam")    # shortcuts
                pos_c = fpos + match.start("sContent")
            elif block["mKeyw"] is not None:    # multi-line block tag
                block_indent = len(block["indent"])
                keyword = block["mKeyw"]
                param   = block["mParam"]
                content = block["mContent"]
                pos_p = fpos + match.start("mParam")
                pos_c = fpos + match.start("mContent")
                ignored = block["mIgnored"].strip()
                if ignored  and  ignored != self._comment_start:
                    raise TemplateSyntaxError("No code allowed after block-tag.", self._errpos(fpos+match.start("mIgnored")))
            elif block["mEnd"] is not None:     # multi-line block end
                if block_type is None:
                    raise TemplateSyntaxError("No block to end here/invalid indent.", self._errpos(pos__) )
                if block_indent != len(block["mEnd"]):
                    raise TemplateSyntaxError("Invalid indent for end-tag.", self._errpos(pos__) )
                ignored = block["meIgnored"].strip()
                if ignored  and  ignored != self._comment_start:
                    raise TemplateSyntaxError("No code allowed after end-tag.", self._errpos(fpos+match.start("meIgnored")))
                block_type = None
            elif block["sEnd"] is not None:     # single-line block end
                if block_type is None:
                    raise TemplateSyntaxError("No block to end here/invalid indent.", self._errpos(pos__))
                if block_indent is not None:
                    raise TemplateSyntaxError("Invalid indent for end-tag.", self._errpos(pos__))
                block_type = None
            else:
                raise TemplateException("FATAL: Block regexp error. Please contact the author. (%s)" % match.group())

            # analyze block content (mainly error-checking and -messages)
            if keyword:
                keyword = keyword.lower()
                if   'for'   == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'for'
                    cond = self._reForParam.match(param)
                    if cond is None:
                        raise TemplateSyntaxError("Invalid 'for ...' at '%s'." %(param), self._errpos(pos_p))
                    names = tuple(n.strip()  for n in cond.group("names").split(","))
                    self._testexpr(cond.group("iter"), pos_p+cond.start("iter"))
                    parsetree.append(("for", names, cond.group("iter"), self._parse(content, pos_c)))
                elif 'if'    == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block at '%s'." %(match.group()), self._errpos(pos__))
                    if not param:
                        raise TemplateSyntaxError("Missing condition for 'if' at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'if'
                    self._testexpr(param, pos_p)
                    parsetree.append(("if", param, self._parse(content, pos_c)))
                elif 'elif'  == keyword:
                    if block_type != 'if':
                        raise TemplateSyntaxError("'elif' may only appear after 'if' at '%s'." %(match.group()), self._errpos(pos__))
                    if not param:
                        raise TemplateSyntaxError("Missing condition for 'elif' at '%s'." %(match.group()), self._errpos(pos__))
                    self._testexpr(param, pos_p)
                    parsetree.append(("elif", param, self._parse(content, pos_c)))
                elif 'else'  == keyword:
                    if block_type not in ('if', 'for'):
                        raise TemplateSyntaxError("'else' may only appear after 'if' or 'for' at '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'else' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    parsetree.append(("else", self._parse(content, pos_c)))
                elif 'macro' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'macro'
                    # make sure param is "\w+" (instead of ".+")
                    if not param:
                        raise TemplateSyntaxError("Missing name for 'macro' at '%s'." %(match.group()), self._errpos(pos__))
                    if not self._reMacroParam.match(param):
                        raise TemplateSyntaxError("Invalid name for 'macro' at '%s'." %(match.group()), self._errpos(pos__))
                    #remove last newline
                    if len(content) > 0 and content[-1] == '\n':
                        content = content[:-1]
                    if len(content) > 0 and content[-1] == '\r':
                        content = content[:-1]
                    parsetree.append(("macro", param, self._parse(content, pos_c)))

                # parser-commands
                elif 'raw'   == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'raw' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'raw'
                    parsetree.append(("str", content))
                elif 'include' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'include' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'include'
                    try:
                        u = self._load(content.strip())
                    except Exception as err:
                        raise TemplateIncludeError(err, self._errpos(pos__))
                    self._includestack.append((content.strip(), u))  # current filename/template for error-msg.
                    p = self._parse(u)
                    self._includestack.pop()
                    parsetree.extend(p)
                elif 'set_escape' == keyword:
                    if block_type is not None:
                        raise TemplateSyntaxError("Missing block-end-tag before new block '%s'." %(match.group()), self._errpos(pos__))
                    if param:
                        raise TemplateSyntaxError("'set_escape' may not have parameters at '%s'." %(match.group()), self._errpos(pos__))
                    block_type = 'set_escape'
                    esc = content.strip().upper()
                    if esc not in ESCAPE_SUPPORTED:
                        raise TemplateSyntaxError("Unsupported escape '%s'." %(esc), self._errpos(pos__))
                    self.escape = ESCAPE_SUPPORTED[esc]
                else:
                    raise TemplateSyntaxError("Invalid keyword '%s'." %(keyword), self._errpos(pos__))
            curr = match.end()

        if block_type is not None:
            raise TemplateSyntaxError("Missing end-tag.", self._errpos(pos__))

        if len(template) > curr:            # process template-part after last block
            self._parse_sub(parsetree, template[curr:], fpos+curr)

        return parsetree

#-----------------------------------------
# Evaluation

# some checks
assert len(eval("dir()", {'__builtins__':{'dir':dir}})) == 1, \
    "FATAL: 'eval' does not work as expected (%s)."
assert compile("0 .__class__", "<string>", "eval").co_names == ('__class__',), \
    "FATAL: 'compile' does not work as expected."

class EvalPseudoSandbox:
    """An eval-pseudo-sandbox.

    The pseudo-sandbox restricts the available functions/objects, so the
    code can only access:

    - some of the builtin Python-functions, which are considered "safe"
      (see safe_builtins)
    - some additional functions (exists(), default(), setvar(), escape())
    - the passed objects incl. their methods.

    Additionally, names beginning with "_" are forbidden.
    This is to prevent things like '0 .__class__', with which you could
    easily break out of a "sandbox".

    Be careful to only pass "safe" objects/functions to the template,
    because any unsafe function/method could break the sandbox!
    For maximum security, restrict the access to as few objects/functions
    as possible!

    :Warning:
        Note that this is no real sandbox! (And although I don't know any
        way to break out of the sandbox without passing-in an unsafe object,
        I cannot guarantee that there is no such way. So use with care.)

        Take care if you want to use it for untrusted code!!
    """

    safe_builtins = {
        "True"      : True,
        "False"     : False,
        "None"      : None,

        "abs"       : builtins.abs,
        "chr"       : builtins.chr,
        "divmod"    : builtins.divmod,
        "hash"      : builtins.hash,
        "hex"       : builtins.hex,
        "len"       : builtins.len,
        "max"       : builtins.max,
        "min"       : builtins.min,
        "oct"       : builtins.oct,
        "ord"       : builtins.ord,
        "pow"       : builtins.pow,
        "range"     : builtins.range,
        "round"     : builtins.round,
        "sorted"    : builtins.sorted,
        "sum"       : builtins.sum,
        "unichr"    : builtins.chr,
        "zip"       : builtins.zip,

        "bool"      : builtins.bool,
        "bytes"     : builtins.bytes,
        "complex"   : builtins.complex,
        "dict"      : builtins.dict,
        "enumerate" : builtins.enumerate,
        "float"     : builtins.float,
        "int"       : builtins.int,
        "list"      : builtins.list,
        "long"      : long,
        "reversed"  : builtins.reversed,
        "str"       : builtins.str,
        "tuple"     : builtins.tuple,
        "unicode"   : unicode,
    }
    if sys.version_info[0] < 3:
        safe_builtins["unichr"] = builtins.unichr

    def __init__(self):
        self._compile_cache = {}
        self.locals_ptr = None
        self.eval_allowed_globals = self.safe_builtins.copy()
        self.register("__import__", self.f_import)
        self.register("exists",  self.f_exists)
        self.register("default", self.f_default)
        self.register("setvar",  self.f_setvar)
        self.register("escape",  self.f_escape)

    def register(self, name, obj):
        """Add an object to the "allowed eval-globals".

        Mainly useful to add user-defined functions to the pseudo-sandbox.
        """
        self.eval_allowed_globals[name] = obj

    def compile(self, expr):
        """Compile a Python-eval-expression.

        - Use a compile-cache.
        - Raise a `NameError` if `expr` contains a name beginning with ``_``.

        :Returns: the compiled `expr`
        :Exceptions:
            - `SyntaxError`: for compile-errors
            - `NameError`: if expr contains a name beginning with ``_``
        """
        if expr not in self._compile_cache:
            c = compile(expr, "", "eval")
            for i in c.co_names:    #prevent breakout via new-style-classes
                if i[0] == '_':
                    raise NameError("Name '%s' is not allowed." % i)
            self._compile_cache[expr] = c
        return self._compile_cache[expr]

    def eval(self, expr, locals):
        """Eval a Python-eval-expression.

        Sets ``self.locals_ptr`` to ``locales`` and compiles the code
        before evaluating.
        """
        sav = self.locals_ptr
        self.locals_ptr = locals
        x = eval(self.compile(expr), {"__builtins__":self.eval_allowed_globals}, locals)
        self.locals_ptr = sav
        return x

    def f_import(self, name, *_, **__):
        """``import``/``__import__()`` for the sandboxed code.

        Since "import" is insecure, the PseudoSandbox does not allow to
        import other modules. But since some functions need to import
        other modules (e.g. "datetime.datetime.strftime" imports "time"),
        this function replaces the builtin "import" and allows to use
        modules which are already accessible by the sandboxed code.

        :Note:
            - This probably only works for rather simple imports.
            - For security, it may be better to avoid such (complex) modules
              which import other modules. (e.g. use time.localtime and
              time.strftime instead of datetime.datetime.strftime,
              or write a small wrapper.)

        :Example:

            >>> from datetime import datetime
            >>> import pyratemp
            >>> t = pyratemp.Template('@!mytime.strftime("%H:%M:%S")!@')

            # >>> print(t(mytime=datetime.now()))
            # Traceback (most recent call last):
            #   ...
            # ImportError: import not allowed in pseudo-sandbox; try to import 'time' yourself and pass it to the sandbox/template

            >>> import time
            >>> print(t(mytime=datetime.strptime("13:40:54", "%H:%M:%S"), time=time))
            13:40:54

            # >>> print(t(mytime=datetime.now(), time=time))
            # 13:40:54
        """
        import types
        if self.locals_ptr is not None  and  name in self.locals_ptr  and  isinstance(self.locals_ptr[name], types.ModuleType):
            return self.locals_ptr[name]
        else:
            raise ImportError("import not allowed in pseudo-sandbox; try to import '%s' yourself (and maybe pass it to the sandbox/template)" % name)

    def f_exists(self, varname):
        """``exists()`` for the sandboxed code.

        Test if the variable `varname` exists in the current locals-namespace.

        This only works for single variable names. If you want to test
        complicated expressions, use i.e. `default`.
        (i.e. `default("expr",False)`)

        :Note:      the variable-name has to be quoted! (like in eval)
        :Example:   see module-docstring
        """
        return (varname in self.locals_ptr)

    def f_default(self, expr, default=None):
        """``default()`` for the sandboxed code.

        Try to evaluate an expression and return the result or a
        fallback-/default-value; the `default`-value is used
        if `expr` does not exist/is invalid/results in None.

        This is very useful for optional data.

        :Parameter:
            - expr: eval-expression
            - default: fallback-falue if eval(expr) fails or is None.
        :Returns:
            the eval-result or the "fallback"-value.

        :Note:      the eval-expression has to be quoted! (like in eval)
        :Example:   see module-docstring
        """
        try:
            r = self.eval(expr, self.locals_ptr)
            if r is None:
                return default
            return r
        #TODO: which exceptions should be catched here?
        except (NameError, LookupError, TypeError):
            return default

    def f_setvar(self, name, expr):
        """``setvar()`` for the sandboxed code.

        Set a variable.

        :Example:   see module-docstring
        """
        self.locals_ptr[name] = self.eval(expr, self.locals_ptr)
        return ""

    def f_escape(self, s, format="HTML"):
        """``escape()`` for the sandboxed code.
        """
        if isinstance(format, (str, unicode)):
            format = ESCAPE_SUPPORTED[format.upper()]
        return escape(unicode(s), format)

#-----------------------------------------
# basic template / subtemplate

class TemplateBase:
    """Basic template-class.

    Used both for the template itself and for 'macro's ("subtemplates") in
    the template.
    """

    def __init__(self, parsetree, renderfunc, data=None):
        """Create the Template/Subtemplate/Macro.

        :Parameters:
            - `parsetree`: parse-tree of the template/subtemplate/macro
            - `renderfunc`: render-function
            - `data`: data to fill into the template by default (dictionary).
              This data may later be overridden when rendering the template.
        :Exceptions:
            - `TypeError`: if `data` is not a dictionary
        """
        #TODO: parameter-checking?
        self.parsetree = parsetree
        if isinstance(data, dict):
            self.data = data
        elif data is None:
            self.data = {}
        else:
            raise TypeError('"data" must be a dict (or None).')
        self.current_data = data
        self._render = renderfunc

    def __call__(self, **override):
        """Fill out/render the template.

        :Parameters:
            - `override`: objects to add to the data-namespace, overriding
              the "default"-data.
        :Returns:    the filled template (in unicode)
        :Note:       This is also called when invoking macros
                     (i.e. ``$!mymacro()!$``).
        """
        self.current_data = self.data.copy()
        self.current_data.update(override)
        u = "".join(self._render(self.parsetree, self.current_data))
        self.current_data = self.data       # restore current_data
        return _dontescape(u)               # (see class _dontescape)

    def __unicode__(self):
        """Alias for __call__()."""
        return self.__call__()
    def __str__(self):
        """Alias for __call__()."""
        return self.__call__()

#-----------------------------------------
# Renderer

class _dontescape(unicode):
    """Unicode-string which should not be escaped.

    If ``isinstance(object,_dontescape)``, then don't escape the object in
    ``@!...!@``. It's useful for not double-escaping macros, and it's
    automatically used for macros/subtemplates.

    :Note: This only works if the object is used on its own in ``@!...!@``.
           It i.e. does not work in ``@!object*2!@`` or ``@!object + "hi"!@``.
    """
    __slots__ = []


class Renderer(object):
    """Render a template-parse-tree.

    :Uses: `TemplateBase` for macros
    """

    def __init__(self, evalfunc, escapefunc):
        """Init the renderer.

        :Parameters:
            - `evalfunc`: function for template-expression-evaluation
              (i.e. ``EvalPseudoSandbox().eval``)
            - `escapefunc`: function for escaping special characters
              (i.e. `escape`)
        """
        #TODO: test evalfunc
        self.evalfunc = evalfunc
        self.escapefunc = escapefunc

    def _eval(self, expr, data):
        """evalfunc with error-messages"""
        try:
            return self.evalfunc(expr, data)
        #TODO: any other errors to catch here?
        except (TypeError,NameError,LookupError,AttributeError, SyntaxError) as err:
            raise TemplateRenderError("Cannot eval expression '%s'. (%s: %s)" %(expr, err.__class__.__name__, err))

    def render(self, parsetree, data):
        """Render a parse-tree of a template.

        :Parameters:
            - `parsetree`: the parse-tree
            - `data`:      the data to fill into the template (dictionary)
        :Returns:   the rendered output-unicode-string
        :Exceptions:
            - `TemplateRenderError`
        """
        _eval = self._eval  # shortcut
        output = []
        do_else = False     # use else/elif-branch?

        if parsetree is None:
            return ""
        for elem in parsetree:
            if   "str"   == elem[0]:
                output.append(elem[1])
            elif "sub"   == elem[0]:
                output.append(unicode(_eval(elem[1], data)))
            elif "esc"   == elem[0]:
                obj = _eval(elem[2], data)
                #prevent double-escape
                if isinstance(obj, _dontescape) or isinstance(obj, TemplateBase):
                    output.append(unicode(obj))
                else:
                    output.append(self.escapefunc(unicode(obj), elem[1]))
            elif "for"   == elem[0]:
                do_else = True
                (names, iterable) = elem[1:3]
                try:
                    loop_iter = iter(_eval(iterable, data))
                except TypeError:
                    raise TemplateRenderError("Cannot loop over '%s'." % iterable)
                for i in loop_iter:
                    do_else = False
                    if len(names) == 1:
                        data[names[0]] = i
                    else:
                        data.update(zip(names, i))   #"for a,b,.. in list"
                    output.extend(self.render(elem[3], data))
            elif "if"    == elem[0]:
                do_else = True
                if _eval(elem[1], data):
                    do_else = False
                    output.extend(self.render(elem[2], data))
            elif "elif"  == elem[0]:
                if do_else and _eval(elem[1], data):
                    do_else = False
                    output.extend(self.render(elem[2], data))
            elif "else"  == elem[0]:
                if do_else:
                    do_else = False
                    output.extend(self.render(elem[1], data))
            elif "macro" == elem[0]:
                data[elem[1]] = TemplateBase(elem[2], self.render, data)
            else:
                raise TemplateRenderError("Invalid parse-tree (%s)." %(elem))

        return output

#-----------------------------------------
# template user-interface (putting it all together)

class Template(TemplateBase):
    """Template-User-Interface.

    :Usage:
        ::
            t = Template(...)  (<- see __init__)
            output = t(...)    (<- see TemplateBase.__call__)

    :Example:
        see module-docstring
    """

    def __init__(self, string=None,filename=None,parsetree=None, encoding='utf-8', data=None, escape=HTML,
            loader_class=LoaderFile,
            parser_class=Parser,
            renderer_class=Renderer,
            eval_class=EvalPseudoSandbox,
            escape_func=escape):
        """Load (+parse) a template.

        :Parameters:
            - `string,filename,parsetree`: a template-string,
                                           filename of a template to load,
                                           or a template-parsetree.
                                           (only one of these 3 is allowed)
            - `encoding`: encoding of the template-files (only used for "filename")
            - `data`:     data to fill into the template by default (dictionary).
                          This data may later be overridden when rendering the template.
            - `escape`:   default-escaping for the template, may be overwritten by the template!
            - `loader_class`
            - `parser_class`
            - `renderer_class`
            - `eval_class`
            - `escapefunc`
        """
        if [string, filename, parsetree].count(None) != 2:
            raise ValueError('Exactly 1 of string,filename,parsetree is necessary.')

        tmpl = None
        # load template
        if filename is not None:
            incl_load = loader_class(os.path.dirname(filename), encoding).load
            tmpl = incl_load(os.path.basename(filename))
        if string is not None:
            incl_load = dummy_raise(NotImplementedError, "'include' not supported for template-strings.")
            tmpl = LoaderString(encoding).load(string)

        # eval (incl. compile-cache)
        templateeval = eval_class()

        # parse
        if tmpl is not None:
            p = parser_class(loadfunc=incl_load, testexpr=templateeval.compile, escape=escape)
            parsetree = p.parse(tmpl)
            del p

        # renderer
        renderfunc = renderer_class(templateeval.eval, escape_func).render

        #create template
        TemplateBase.__init__(self, parsetree, renderfunc, data)


#=========================================
#doctest

def _doctest():
    """doctest this module."""
    import doctest
    doctest.testmod()

#----------------------
if __name__ == '__main__':
    if sys.version_info[0] <= 2:
        _doctest()

#=========================================
                                                  

    - name: # This lets you log in via `eb ssh` and access the docker daemon.
# If we don't add the ec2-user to the docker group, then calls to docker
# (like `docker ps`) will fail with `Cannot connect to the Docker daemon`
#
# See: https://blog.cloudinvaders.com/connect-to-docker-daemon-on-aws-beanstalk-ec2-instance/
commands:
  0_add_docker_group_to_ec2_user:
    command: gpasswd -a ec2-user docker
    test: groups ec2-user | grep -qv docker           
      working-directory: {"__symbol_cache_version__":11}      
      run: nuget restore <?php

phutil_register_library('customlib', __FILE__);          

    - name: <?php

/**
 * This file is automatically generated. Use 'arc liberate' to rebuild it.
 *
 * @generated
 * @phutil-library-version 2
 */
phutil_register_library_map(array(
  '__library_version__' => 2,
  'class' => array(),
  'function' => array(),
  'xmap' => array(),
));                                     
      working-directory: cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css web/vendor-css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js web/vendor-js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot web/fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg web/fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf web/fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff web/fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 web/fonts/glyphicons-halflings-regular.woff2;

cp vendor/components/jquery/jquery.min.js web/vendor-js/jquery.min.js;
cp vendor/components/handlebars/handlebars.min.js web/vendor-js/handlebars.min.js;

cp vendor/codepo8/CustomFileInputs/css/component.css web/vendor-css/component.css;
cp vendor/codepo8/CustomFileInputs/css/normalize.css web/vendor-css/normalize.css;
cp vendor/codepo8/CustomFileInputs/js/custom-file-input.js web/vendor-js/custom-file-input.js;

cp vendor/happyworm/jplayer/dist/jplayer/jquery.jplayer.min.js web/vendor-js/jquery.jplayer.min.js;
cp vendor/happyworm/jplayer/dist/jplayer/jquery.jplayer.swf web/vendor-js/jquery.jplayer.swf;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/css/jplayer.blue.monday.min.css web/vendor-css/jplayer.blue.monday.min.css;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.jpg web/image/jplayer.blue.monday.jpg;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.seeking.gif web/image/jplayer.blue.monday.seeking.gif;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.video.play.png web/image/jplayer.blue.monday.video.play.png;           
      # Add additional options to the MSBuild command line here (like platform or verbosity level).
      # See https://docs.microsoft.com/visualstudio/msbuild/msbuild-command-line-reference
      run: msbuild /m /p:Configuration=${{env.BUILD_CONFIGURATION}} ${{env.SOLUTION_FILE_PATH}}
