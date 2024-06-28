# This workflow will install Python dependencies, run tests and lint with a variety of Python versions
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python

name: Python package

on: import sys
import os
from bluetooth.btcommon import *

__version__ = 0.30

def _dbg(*args):
    return
    sys.stderr.write(*args)
    sys.stderr.write("\n")

if sys.platform == "win32":
    from bluetooth.msbt import *

elif sys.platform.startswith("linux"):
    from bluetooth.bluez import *
elif sys.platform == "darwin":
    from .macos import *
else:
    raise Exception("This platform (%s) is currently not supported by pybluez." % sys.platform)

discover_devices.__doc__ = \
    """Perform a Bluetooth device discovery.
   
    This function uses the first available Bluetooth resource to discover Bluetooth devices.

    Parameters
    ----------
    lookup_names : bool
        When set to True :func:`discover_devices` also attempts to look up the display name of each
        detected device. (the default is False).

    lookup_class : bool 
        When set to True :func:`discover_devices` attempts to look up the class of each detected device.
        (the default is False).

    Returns
    -------
    list
        Returns a list of device addresses as strings or a list of tuples. The content of the
        tuples depends on the values of lookup_names and lookup_class as detailed below.

        ============   ============   =====================================
        lookup_class   lookup_names   Return
        ============   ============   =====================================
        False          False          list of device addresses
        False          True           list of (address, name) tuples
        True           False          list of (address, class) tuples
        True           True           list of (address, name, class) tuples
        ============   ============   =====================================

    """

lookup_name.__doc__ = \
    """Look up the friendly name of a Bluetooth device.

    This function tries to determine the friendly name (human readable) of the device with
    the specified Bluetooth address.
  
    Parameters
    ----------
    address : str
        The Bluetooth address of the device.
    
    Returns
    -------
    str or None
        The friendly name of the device on success, and None on failure.

    Raises
    ------
    BluetoothError
        When the provided address is not a valid Bluetooth address.

    """

advertise_service.__doc__ = \
    """Advertise a service with the local SDP server.
  
    Parameters
    ----------
    sock : BluetoothSocket
        The :class:`BluetoothSocket` to use for advertising a service. The socket must be a bound,
        listening socket.
        
    name : str
        The name of the service and service_id (if specified). This should be a string
        of the form "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX", where each 'X' is a hexadecimal
        digit.

    service_classes : list 
        a list of service classes belonging to the advertised service.

        Each service class is represented by a 16-bit or 128-bit UUID.
        
        ============  ====================================
        UUID Type     Format
        ------------  ------------------------------------
        Short 16-bit  XXXX
        Full 128-bit  XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
        ============  ====================================

        where each 'X' is a hexadecimal digit.

        There are some constants for standard services, e.g. SERIAL_PORT_CLASS that equals to
        "1101". Some class constants provided by PyBluez are:

        ========================   
        SERIAL_PORT_CLASS            
        DIALUP_NET_CLASS           
        CORDLESS_TELEPHONY_CLASS   
        AUDIO_SINK_CLASS             
        NAP_CLASS                  
        LAN_ACCESS_CLASS         
        HEADSET_CLASS
        AUDIO_SOURCE_CLASS
        PANU_CLASS               
        GN_CLASS
        ========================  

    profiles : list
        A list of service profiles that thie service fulfills. Each profile is a tuple with 
        (uuid, version). Most standard profiles use standard classes as UUIDs. 
        
        PyBluez offers a list of standard profiles, for example SERIAL_PORT_PROFILE. All standard profiles have
        the same name as the classes, except that _CLASS suffix is replaced by _PROFILE.

    provider : str
        A text string specifying the provider of the service

    description : str
        A text string describing the service

    protocols : list
        A list of protocols

    Note
    ----
    A note on working with Symbian smartphones: bt_discover in Python for Series 60 will only 
    detect service records with service class SERIAL_PORT_CLASS and profile SERIAL_PORT_PROFILE

    """

stop_advertising.__doc__ = \
    """Try to stop advertising a bluetooth service.

    This function instructs the local SDP server to stop advertising the service associated
    with socket.  You should typically call this right before you close socket.
    
    Parameters
    ----------
    sock : BluetoothSocket
        The :class:`BluetoothSocket` to stop advertising the service on.
    
    Raises
    ------
    BluetoothError
        When SDP fails to stop advertising for some reason.

    """ 

find_service.__doc__ = \
    """Use to find available Bluetooth services.

    This function uses the service discovery protocol (SDP) to search for Bluetooth
    services matching the specified criteria and returns the search results.

    The search criteria are defined by passing one or more parameters to the function.

    If no criteria are specified then a list of all nearby services detected is 
    returned. If more than one criteria is specified, then the search results will 
    match all the criteria specified.
    
    Parameters
    ----------
    name: str or None
        The friendly name of a Bluetooth device. 

    uuid : str or None
        A valid 16-bit or 128-bit UUID.

        ============  ====================================
        UUID Type     Format
        ------------  ------------------------------------
        Short 16-bit  XXXX
        Full 128-bit  XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
        ============  ====================================

        where each 'X' is a hexadecimal digit. 

    address : str or None
        The Bluetooth address of a device or "localhost". 
        If "localhost" is provided the function will search for Bluetooth services on the
        local machine.
    
    Returns
    -------
    list
        The search results will be a list of dictionaries.  Each dictionary represents a 
        search match having the following key/value pairs.

        =============== ===========================================================
        Key             Value
        =============== ===========================================================
        host            the bluetooth address of the device advertising the
                        service.
        name            the name of the service being advertised.
        description     a description of the service being advertised.
        provider        the name of the person/organization providing the service.
        protocol        either 'RFCOMM', 'L2CAP', None if the protocol was not specified, 

                        or 'UNKNOWN' if the protocol was specified but unrecognized.
        port            the L2CAP PSM number if the protocol is 'L2CAP', 

                        the RFCOMM channel number if the protocol is 'RFCOMM', 

                        or None if the protocol wasn't specified.
        service-classes a list of service class IDs (UUID strings). Possibly
                        empty
        profiles        a list of profiles the service claims to support.
        
                        a profile takes the form of (UUID, version) pairs. 

                        Possibly empty.
        service-id      the Service ID of the service.  None if it wasn't set

                        See the Bluetooth spec for the difference between

                        Service ID and Service Class ID List
        =============== ===========================================================

    """

BluetoothSocket.__doc__ = \
    """ A Bluetooth Socket representing one endpoint of a Bluetooth connection.

    Parameters
    ----------

    proto : int
        The protocol the socket will use. The options are HCI, L2CAP, RFCOMM, or SCO. 
        The default is RFCOMM.
        
        .. note:: RFCOMM is the only protocol available for Windows and macOS systems.

    """

BluetoothSocket.dup.__doc__ =\
    """Duplicate the socket

    Returns
    -------
    BluetoothSocket
        A new :class:`BluetoothSocket` connected to the same system resource.
        
    """

BluetoothSocket.accept.__doc__ = \
    """Accept a connection.

    Returns
    -------
    tuple
        A tuple containing a :class:`BluetoothSocket` and a Bluetooth address.

    Raises
    ------
    BluetoothError
        When an attempt to accept a connection fails.
    
    """

BluetoothSocket.bind.__doc__ = \
    """Bind the socket to a local address and port.

    Parameters
    ----------
    addrport : tuple
        A tuple of the form (address str, port int)

    Raises
    ------
    BluetoothError
        When an attempt to bind the socket fails.

    """

#BluetoothSocket.get_l2cap_options.__doc__ =\
#    """Get the L2CAP options for the specified L2CAP socket.

#    Options are: omtu, imtu, flush_to, mode, fcs, max_tx, txwin_size.

#    Returns
#    -------
#    list
#        A list of L2CAP options available for the socket.

#    """

BluetoothError.__doc__ = \
    """Raised when a bluetooth function or method fails for a Bluetooth I/O
    related reason.

    """

""" Documentation for the remaining methods of the class can be found in the file bluez/btmodule.c in the function definitions. """
  push: from gattlib import *
    branches: [ "main" ]
  pull_request: import array
import fcntl
import sys
import struct
from errno import (EADDRINUSE, EBUSY, EINVAL)

from bluetooth.btcommon import *
import bluetooth._bluetooth as _bt
from bluetooth._bluetooth import HCI, RFCOMM, L2CAP, SCO, SOL_L2CAP, \
                                    SOL_RFCOMM, L2CAP_OPTIONS

get_byte = int

# ============== SDP service registration and unregistration ============

def discover_devices (duration=8, flush_cache=True, lookup_names=False,
                      lookup_class=False, device_id=-1, iac=IAC_GIAC):
    if device_id == -1:
        device_id = _bt.hci_get_route()

    sock = _gethcisock (device_id)
    try:
        results = _bt.hci_inquiry (sock, duration=duration, flush_cache=True,
                                   lookup_class=lookup_class, device_id=device_id,
                                   iac=iac)
    except _bt.error as e:
        sock.close ()
        raise BluetoothError (e.args[0], "Error communicating with local "
        "bluetooth adapter: " + e.args[1])

    if lookup_names:
        pairs = []
        for item in results:
            if lookup_class:
                addr, dev_class = item
            else:
                addr = item
            timeoutms = int (10 * 1000)
            try:
                name = _bt.hci_read_remote_name (sock, addr, timeoutms)
            except _bt.error:
                # name lookup failed.  either a timeout, or I/O error
                continue
            pairs.append ((addr, name, dev_class) if lookup_class else (addr, name))
        sock.close ()
        return pairs
    else:
        sock.close ()
        return results

def read_local_bdaddr():
    try:
        hci_sock = _bt.hci_open_dev(0)
        old_filter = hci_sock.getsockopt( _bt.SOL_HCI, _bt.HCI_FILTER, 14)
        flt = _bt.hci_filter_new()
        opcode = _bt.cmd_opcode_pack(_bt.OGF_INFO_PARAM,
                _bt.OCF_READ_BD_ADDR)
        _bt.hci_filter_set_ptype(flt, _bt.HCI_EVENT_PKT)
        _bt.hci_filter_set_event(flt, _bt.EVT_CMD_COMPLETE)
        _bt.hci_filter_set_opcode(flt, opcode)
        hci_sock.setsockopt( _bt.SOL_HCI, _bt.HCI_FILTER, flt )

        _bt.hci_send_cmd(hci_sock, _bt.OGF_INFO_PARAM, _bt.OCF_READ_BD_ADDR )

        pkt = hci_sock.recv(255)

        status,raw_bdaddr = struct.unpack("xxxxxxB6s", pkt)
        assert status == 0

        t = [ "%02X" % get_byte(b) for b in raw_bdaddr ]
        t.reverse()
        bdaddr = ":".join(t)

        # restore old filter
        hci_sock.setsockopt( _bt.SOL_HCI, _bt.HCI_FILTER, old_filter )
        return [bdaddr]
    except _bt.error as e:
        raise BluetoothError(*e.args)

def lookup_name (address, timeout=10):
    if not is_valid_address (address):
        raise BluetoothError (EINVAL, "%s is not a valid Bluetooth address" % address)

    sock = _gethcisock ()
    timeoutms = int (timeout * 1000)
    try:
        name = _bt.hci_read_remote_name (sock, address, timeoutms)
    except _bt.error:
        # name lookup failed.  either a timeout, or I/O error
        name = None
    sock.close ()
    return name

def set_packet_timeout (address, timeout):
    """
    Adjusts the ACL flush timeout for the ACL connection to the specified
    device.  This means that all L2CAP and RFCOMM data being sent to that
    device will be dropped if not acknowledged in timeout milliseconds (maximum
    1280).  A timeout of 0 means to never drop packets.

    Since this affects all Bluetooth connections to that device, and not just
    those initiated by this process or PyBluez, a call to this method requires
    superuser privileges.

    You must have an active connection to the specified device before invoking
    this method.

    """
    n = round (timeout / 0.625)
    write_flush_timeout (address, n)

def get_l2cap_options (sock):
    """get_l2cap_options (sock, mtu)

    Gets L2CAP options for the specified L2CAP socket.
    Options are: omtu, imtu, flush_to, mode, fcs, max_tx, txwin_size.

    """
    # TODO this should be in the C module, because it depends
    # directly on struct l2cap_options layout.
    s = sock.getsockopt (SOL_L2CAP, L2CAP_OPTIONS, 12)
    options = list( struct.unpack ("HHHBBBH", s))
    return options

def set_l2cap_options (sock, options):
    """set_l2cap_options (sock, options)

    Sets L2CAP options for the specified L2CAP socket.
    The option list must be in the same format supplied by
    get_l2cap_options().

    """
    # TODO this should be in the C module, because it depends
    # directly on struct l2cap_options layout.
    s = struct.pack ("HHHBBBH", *options)
    sock.setsockopt (SOL_L2CAP, L2CAP_OPTIONS, s)

def set_l2cap_mtu (sock, mtu):
    """set_l2cap_mtu (sock, mtu)

    Adjusts the MTU for the specified L2CAP socket.  This method needs to be
    invoked on both sides of the connection for it to work!  The default mtu
    that all L2CAP connections start with is 672 bytes.

    mtu must be between 48 and 65535, inclusive.

    """
    options = get_l2cap_options (sock)
    options[0] = options[1] = mtu
    set_l2cap_options (sock, options)

def _get_available_ports(protocol):
    if protocol == RFCOMM:
        return range (1, 31)
    elif protocol == L2CAP:
        return range (0x1001, 0x8000, 2)
    else:
        return [0]

class BluetoothSocket:
    __doc__ = _bt.btsocket.__doc__

    def __init__ (self, proto = RFCOMM, _sock=None):
        if _sock is None:
            _sock = _bt.btsocket (proto)
        self._sock = _sock
        self._proto = proto

    def dup (self):
        """dup () -> socket object

        Return a new socket object connected to the same system resource.
        
        """
        return BluetoothSocket (proto=self._proto, _sock=self._sock)

    def accept (self):
        try:
            client, addr = self._sock.accept ()
        except _bt.error as e:
            raise BluetoothError (*e.args)
        newsock = BluetoothSocket (self._proto, client)
        return (newsock, addr)
    accept.__doc__ = _bt.btsocket.accept.__doc__

    def bind (self, addrport):
        if len (addrport) != 2 or addrport[1] != 0:
            try:
                return self._sock.bind (addrport)
            except _bt.error as e:
                raise BluetoothError (*e.args)
        addr, _ = addrport
        for port in _get_available_ports (self._proto):
            try:
                return self._sock.bind ((addr, port))
            except _bt.error as e:
                err = BluetoothError (*e.args)
                if err.errno != EADDRINUSE:
                    break
        raise err

    def get_l2cap_options(self):
        """get_l2cap_options (sock, mtu)

        Gets L2CAP options for the specified L2CAP socket.
        Options are: omtu, imtu, flush_to, mode, fcs, max_tx, txwin_size.

        """
        return get_l2cap_options(self)

    def set_l2cap_options(self, options):
        """set_l2cap_options (sock, options)

        Sets L2CAP options for the specified L2CAP socket.
        The option list must be in the same format supplied by
        get_l2cap_options().

        """
        return set_l2cap_options(self, options)

    def set_l2cap_mtu(self, mtu):
        """set_l2cap_mtu (sock, mtu)

        Adjusts the MTU for the specified L2CAP socket.  This method needs to be
        invoked on both sides of the connection for it to work!  The default mtu
        that all L2CAP connections start with is 672 bytes.

        mtu must be between 48 and 65535, inclusive.

        """
        return set_l2cap_mtu(self, mtu)

    # import methods from the wraapped socket object
    _s = ("""def %s (self, *args, **kwargs):
    try:
        return self._sock.%s (*args, **kwargs)
    except _bt.error as e:
        raise BluetoothError (*e.args)
    %s.__doc__ = _bt.btsocket.%s.__doc__\n""")
    for _m in ( 'connect', 'connect_ex', 'close',
        'fileno', 'getpeername', 'getsockname', 'gettimeout',
        'getsockopt', 'listen', 'makefile', 'recv', 'recvfrom', 'sendall',
        'send', 'sendto', 'setblocking', 'setsockopt', 'settimeout',
        'shutdown', 'setl2capsecurity'):
        exec( _s % (_m, _m, _m, _m))
    del _m, _s

    # import readonly attributes from the wrapped socket object
    _s = ("@property\ndef %s (self): \
    return self._sock.%s")
    for _m in ('family', 'type', 'proto', 'timeout'):
        exec( _s % (_m, _m))
    del _m, _s


def advertise_service (sock, name, service_id = "", service_classes = [], \
        profiles = [], provider = "", description = "", protocols = []):
    if service_id != "" and not is_valid_uuid (service_id):
        raise ValueError ("invalid UUID specified for service_id")
    for uuid in service_classes:
        if not is_valid_uuid (uuid):
            raise ValueError ("invalid UUID specified in service_classes")
    for uuid, version in profiles:
        if not is_valid_uuid (uuid) or  version < 0 or  version > 0xFFFF:
            raise ValueError ("Invalid Profile Descriptor")
    for uuid in protocols:
        if not is_valid_uuid (uuid):
            raise ValueError ("invalid UUID specified in protocols")

    try:
        _bt.sdp_advertise_service (sock._sock, name, service_id, \
                service_classes, profiles, provider, description, \
                protocols)
    except _bt.error as e:
        raise BluetoothError (*e.args)

def stop_advertising (sock):
    try:
        _bt.sdp_stop_advertising (sock._sock)
    except _bt.error as e:
        raise BluetoothError (*e.args)

def find_service (name = None, uuid = None, address = None):
    if not address:
        devices = discover_devices ()
    else:
        devices = [ address ]

    results = []

    if uuid is not None and not is_valid_uuid (uuid):
        raise ValueError ("invalid UUID")

    try:
        for addr in devices:
            try:
                s = _bt.SDPSession ()
                s.connect (addr)
                matches = []
                if uuid is not None:
                    matches = s.search (uuid)
                else:
                    matches = s.browse ()
            except _bt.error:
                continue

            if name is not None:
                matches = [s for s in matches if s.get ("name", "") == name]

            for m in matches:
                m["host"] = addr

            results.extend (matches)
    except _bt.error as e:
        raise BluetoothError (*e.args)

    return results

# ================ BlueZ internal methods ================
def _gethcisock (device_id = -1):
    try:
        sock = _bt.hci_open_dev (device_id)
    except _bt.error as e:
        raise BluetoothError (e.args[0], "error accessing bluetooth device: " +
                              e.args[1])
    return sock

def get_acl_conn_handle (hci_sock, addr):
    hci_fd = hci_sock.fileno ()
    reqstr = struct.pack ("6sB17s", _bt.str2ba (addr),
            _bt.ACL_LINK, b"\0" * 17)
    request = array.array ("b", reqstr)
    try:
        fcntl.ioctl (hci_fd, _bt.HCIGETCONNINFO, request, 1)
    except OSError as e:
        raise BluetoothError (e.args[0], "There is no ACL connection to %s" % addr)

    # XXX should this be "<8xH14x"?
    handle = struct.unpack ("8xH14x", request.tostring ())[0]
    return handle

def write_flush_timeout (addr, timeout):
    hci_sock = _bt.hci_open_dev ()
    # get the ACL connection handle to the remote device
    handle = get_acl_conn_handle (hci_sock, addr)
    # XXX should this be "<HH"
    pkt = struct.pack ("HH", handle, _bt.htobs (timeout))
    response = _bt.hci_send_req (hci_sock, _bt.OGF_HOST_CTL,
        0x0028, _bt.EVT_CMD_COMPLETE, 3, pkt)
    status = get_byte(response[0])
    rhandle = struct.unpack ("H", response[1:3])[0]
    assert rhandle == handle
    assert status == 0

def read_flush_timeout (addr):
    hci_sock = _bt.hci_open_dev ()
    # get the ACL connection handle to the remote device
    handle = get_acl_conn_handle (hci_sock, addr)
    # XXX should this be "<H"?
    pkt = struct.pack ("H", handle)
    response = _bt.hci_send_req (hci_sock, _bt.OGF_HOST_CTL,
        0x0027, _bt.EVT_CMD_COMPLETE, 5, pkt)
    status = get_byte(response[0])
    rhandle = struct.unpack ("H", response[1:3])[0]
    assert rhandle == handle
    assert status == 0
    fto = struct.unpack ("H", response[3:5])[0]
    return fto

# =============== DeviceDiscoverer ==================
def byte_to_signed_int(byte_):
    if byte_ > 127:
        return byte_ - 256
    else:
        return byte_

class DeviceDiscoverer:
    """
    Skeleton class for finer control of the device discovery process.

    To implement asynchronous device discovery (e.g. if you want to do
    something *as soon as* a device is discovered), subclass
    DeviceDiscoverer and override device_discovered () and
    inquiry_complete ()
    """
    def __init__ (self, device_id=-1):
        """
        __init__ (device_id=-1)

        device_id - The ID of the Bluetooth adapter that will be used
                    for discovery.
        """
        self.sock = None
        self.is_inquiring = False
        self.lookup_names = False
        self.device_id = device_id

        self.names_to_find = {}
        self.names_found = {}

    def find_devices (self, lookup_names=True,
            duration=8,
            flush_cache=True):
        """
        find_devices (lookup_names=True, service_name=None,
                       duration=8, flush_cache=True)

        Call this method to initiate the device discovery process

        lookup_names - set to True if you want to lookup the user-friendly
                       names for each device found.

        service_name - set to the name of a service you're looking for.
                       only devices with a service of this name will be
                       returned in device_discovered () NOT YET IMPLEMENTED


        ADVANCED PARAMETERS:  (don't change these unless you know what
                            you're doing)

        duration - the number of 1.2 second units to spend searching for
                   bluetooth devices.  If lookup_names is True, then the
                   inquiry process can take a lot longer.

        flush_cache - return devices discovered in previous inquiries
        """
        if self.is_inquiring:
            raise BluetoothError (EBUSY, "Already inquiring!")

        self.lookup_names = lookup_names

        self.sock = _gethcisock (self.device_id)
        flt = _bt.hci_filter_new ()
        _bt.hci_filter_all_events (flt)
        _bt.hci_filter_set_ptype (flt, _bt.HCI_EVENT_PKT)

        try:
            self.sock.setsockopt (_bt.SOL_HCI, _bt.HCI_FILTER, flt)
        except _bt.error as e:
            raise BluetoothError (*e.args)

        # send the inquiry command
        max_responses = 255
        cmd_pkt = struct.pack ("BBBBB", 0x33, 0x8b, 0x9e, \
                duration, max_responses)

        self.pre_inquiry ()

        try:
            _bt.hci_send_cmd (self.sock, _bt.OGF_LINK_CTL, \
                    _bt.OCF_INQUIRY, cmd_pkt)
        except _bt.error as e:
            raise BluetoothError (*e.args)

        self.is_inquiring = True

        self.names_to_find = {}
        self.names_found = {}

    def cancel_inquiry (self):
        """
        Call this method to cancel an inquiry in process.  inquiry_complete
        will still be called.
        """
        self.names_to_find = {}

        if self.is_inquiring:
            try:
                _bt.hci_send_cmd (self.sock, _bt.OGF_LINK_CTL, \
                        _bt.OCF_INQUIRY_CANCEL)
            except _bt.error as e:
                self.sock.close ()
                self.sock = None
                raise BluetoothError (e.args[0],
                                      "error canceling inquiry: " +
                                      e.args[1])
            self.is_inquiring = False

    def process_inquiry (self):
        """
        Repeatedly calls process_event () until the device inquiry has
        completed.
        """
        while self.is_inquiring or len (self.names_to_find) > 0:
            self.process_event ()

    def process_event (self):
        """
        Waits for one event to happen, and proceses it.  The event will be
        either a device discovery, or an inquiry completion.
        """
        self._process_hci_event ()

    def _process_hci_event (self):
        # FIXME may not wrap _bluetooth.error properly
        if self.sock is None: return
        # voodoo magic!!!
        pkt = self.sock.recv (258)
        ptype, event, plen = struct.unpack ("BBB", pkt[:3])
        pkt = pkt[3:]
        if event == _bt.EVT_INQUIRY_RESULT:
            nrsp = get_byte(pkt[0])
            for i in range (nrsp):
                addr = _bt.ba2str (pkt[1+6*i:1+6*i+6])
                psrm = pkt[ 1+6*nrsp+i ]
                pspm = pkt[ 1+7*nrsp+i ]
                devclass_raw = struct.unpack ("BBB",
                        pkt[1+9*nrsp+3*i:1+9*nrsp+3*i+3])
                devclass = (devclass_raw[2] << 16) | \
                        (devclass_raw[1] << 8) | \
                        devclass_raw[0]
                clockoff = pkt[1+12*nrsp+2*i:1+12*nrsp+2*i+2]

                self._device_discovered (addr, devclass,
                        psrm, pspm, clockoff, None, None)
        elif event == _bt.EVT_INQUIRY_RESULT_WITH_RSSI:
            nrsp = get_byte(pkt[0])
            for i in range (nrsp):
                addr = _bt.ba2str (pkt[1+6*i:1+6*i+6])
                psrm = pkt[ 1+6*nrsp+i ]
                pspm = pkt[ 1+7*nrsp+i ]
#                devclass_raw = pkt[1+8*nrsp+3*i:1+8*nrsp+3*i+3]
#                devclass = struct.unpack ("I", "%s\0" % devclass_raw)[0]
                devclass_raw = struct.unpack ("BBB",
                        pkt[1+8*nrsp+3*i:1+8*nrsp+3*i+3])
                devclass = (devclass_raw[2] << 16) | \
                        (devclass_raw[1] << 8) | \
                        devclass_raw[0]
                clockoff = pkt[1+11*nrsp+2*i:1+11*nrsp+2*i+2]
                rssi = byte_to_signed_int(get_byte(pkt[1+13*nrsp+i]))

                self._device_discovered (addr, devclass,
                        psrm, pspm, clockoff, rssi, None)
        elif _bt.HAVE_EVT_EXTENDED_INQUIRY_RESULT and event == _bt.EVT_EXTENDED_INQUIRY_RESULT:
            nrsp = get_byte(pkt[0])
            for i in range (nrsp):
                addr = _bt.ba2str (pkt[1+6*i:1+6*i+6])
                psrm = pkt[ 1+6*nrsp+i ]
                pspm = pkt[ 1+7*nrsp+i ]
                devclass_raw = struct.unpack ("BBB",
                        pkt[1+8*nrsp+3*i:1+8*nrsp+3*i+3])
                devclass = (devclass_raw[2] << 16) | \
                        (devclass_raw[1] << 8) | \
                        devclass_raw[0]
                clockoff = pkt[1+11*nrsp+2*i:1+11*nrsp+2*i+2]
                rssi = byte_to_signed_int(get_byte(pkt[1+13*nrsp+i]))

                data_len = _bt.EXTENDED_INQUIRY_INFO_SIZE - _bt.INQUIRY_INFO_WITH_RSSI_SIZE
                data = pkt[1+14*nrsp+i:1+14*nrsp+i+data_len]
                name = None
                pos = 0
                while(pos <= len(data)):
                    struct_len = get_byte(data[pos])
                    if struct_len == 0:
                        break
                    eir_type = get_byte(data[pos+1])
                    if eir_type == 0x09: # Complete local name
                        name = data[pos+2:pos+struct_len+1]
                    pos += struct_len + 2

                self._device_discovered (addr, devclass,
                        psrm, pspm, clockoff, rssi, name)
        elif event == _bt.EVT_INQUIRY_COMPLETE or event == _bt.EVT_CMD_COMPLETE:
            self.is_inquiring = False
            if len (self.names_to_find) == 0:
#                print "inquiry complete (evt_inquiry_complete)"
                self.sock.close ()
                self._inquiry_complete ()
            else:
                self._send_next_name_req ()

        elif event == _bt.EVT_CMD_STATUS:
            # XXX shold this be "<BBH"
            status, ncmd, opcode = struct.unpack ("BBH", pkt[:4])
            if status != 0:
                self.is_inquiring = False
                self.sock.close ()

#                print "inquiry complete (bad status 0x%X 0x%X 0x%X)" % \
#                        (status, ncmd, opcode)
                self.names_to_find = {}
                self._inquiry_complete ()
        elif event == _bt.EVT_REMOTE_NAME_REQ_COMPLETE:
            status = get_byte(pkt[0])
            addr = _bt.ba2str (pkt[1:7])
            if status == 0:
                try:
                    name = pkt[7:].split ('\0')[0]
                except IndexError:
                    name = ''
                if addr in self.names_to_find:
                    device_class, rssi = self.names_to_find[addr][:2]
                    self.device_discovered (addr, device_class, rssi, name)
                    del self.names_to_find[addr]
                    self.names_found[addr] = ( device_class, rssi, name)
                else:
                    pass
            else:
                if addr in self.names_to_find: del self.names_to_find[addr]
                # XXX should we do something when a name lookup fails?
#                print "name req unsuccessful %s - %s" % (addr, status)

            if len (self.names_to_find) == 0:
                self.is_inquiring = False
                self.sock.close ()
                self.inquiry_complete ()
#                print "inquiry complete (name req complete)"
            else:
                self._send_next_name_req ()
        else:
            pass
#            print "unrecognized packet type 0x%02x" % ptype

    def _device_discovered (self, address, device_class,
            psrm, pspm, clockoff, rssi, name):
        if self.lookup_names:
            if name is not None:
                self.device_discovered (address, device_class, rssi, name)
            elif address not in self.names_found and \
                address not in self.names_to_find:

                self.names_to_find[address] = \
                    (device_class, rssi, psrm, pspm, clockoff)
        else:
            self.device_discovered (address, device_class, rssi, None)

    def _send_next_name_req (self):
        assert len (self.names_to_find) > 0
        address = list(self.names_to_find.keys ())[0]
        device_class, rssi, psrm, pspm, clockoff = self.names_to_find[address]
        bdaddr = _bt.str2ba (address) #TODO not supported in python3

        cmd_pkt = "{}{}\0{}".format(bdaddr, psrm, clockoff)

        try:
            _bt.hci_send_cmd (self.sock, _bt.OGF_LINK_CTL, \
                    _bt.OCF_REMOTE_NAME_REQ, cmd_pkt)
        except _bt.error as e:
            raise BluetoothError (e.args[0],
                                  "error request name of %s - %s:" %
                    (address, e.args[1]))

    def fileno (self):
        if not self.sock: return None
        return self.sock.fileno ()

    def pre_inquiry (self):
        """
        Called just after find_devices is invoked, but just before the
        inquiry is started.

        This method exists to be overriden
        """

    def device_discovered (self, address, device_class, rssi, name):
        """
        Called when a bluetooth device is discovered.

        address is the bluetooth address of the device

        device_class is the Class of Device, as specified in [1]
                     passed in as a 3-byte string

        name is the user-friendly name of the device if lookup_names was
        set when the inquiry was started.  otherwise None

        This method exists to be overriden.

        [1] https://www.bluetooth.org/foundry/assignnumb/document/baseband
        """
        if name:
            print("found: {} - {} (class 0x{:X}, rssi {})".format(
                address, name, device_class, rssi))
        else:
            print("found: {} (class 0x{:X})".format(address, device_class))
            print("found: {} (class 0x{:X}, rssi {})".format(
                address, device_class, rssi))

    def _inquiry_complete (self):
        """
        Called when an inquiry started by find_devices has completed.
        """
        self.sock.close ()
        self.sock = None
        self.inquiry_complete()

    def inquiry_complete (self):
        """
        Called when an inquiry started by find_devices has completed.
        """
        print("inquiry complete")
    branches: [ "main" ]

jobs: import sys
import struct
import binascii

L2CAP=0
RFCOMM=3

PORT_ANY=0

# Service Class IDs
SDP_SERVER_CLASS = "1000"
BROWSE_GRP_DESC_CLASS = "1001"
PUBLIC_BROWSE_GROUP = "1002"
SERIAL_PORT_CLASS = "1101"
LAN_ACCESS_CLASS = "1102"
DIALUP_NET_CLASS = "1103"
IRMC_SYNC_CLASS = "1104"
OBEX_OBJPUSH_CLASS = "1105"
OBEX_FILETRANS_CLASS = "1106"
IRMC_SYNC_CMD_CLASS = "1107"
HEADSET_CLASS = "1108"
CORDLESS_TELEPHONY_CLASS = "1109"
AUDIO_SOURCE_CLASS = "110a"
AUDIO_SINK_CLASS = "110b"
AV_REMOTE_TARGET_CLASS = "110c"
ADVANCED_AUDIO_CLASS = "110d"
AV_REMOTE_CLASS = "110e"
VIDEO_CONF_CLASS = "110f"
INTERCOM_CLASS = "1110"
FAX_CLASS = "1111"
HEADSET_AGW_CLASS = "1112"
WAP_CLASS = "1113"
WAP_CLIENT_CLASS = "1114"
PANU_CLASS = "1115"
NAP_CLASS = "1116"
GN_CLASS = "1117"
DIRECT_PRINTING_CLASS = "1118"
REFERENCE_PRINTING_CLASS = "1119"
IMAGING_CLASS = "111a"
IMAGING_RESPONDER_CLASS = "111b"
IMAGING_ARCHIVE_CLASS = "111c"
IMAGING_REFOBJS_CLASS = "111d"
HANDSFREE_CLASS = "111e"
HANDSFREE_AGW_CLASS = "111f"
DIRECT_PRT_REFOBJS_CLASS = "1120"
REFLECTED_UI_CLASS = "1121"
BASIC_PRINTING_CLASS = "1122"
PRINTING_STATUS_CLASS = "1123"
HID_CLASS = "1124"
HCR_CLASS = "1125"
HCR_PRINT_CLASS = "1126"
HCR_SCAN_CLASS = "1127"
CIP_CLASS = "1128"
VIDEO_CONF_GW_CLASS = "1129"
UDI_MT_CLASS = "112a"
UDI_TA_CLASS = "112b"
AV_CLASS = "112c"
SAP_CLASS = "112d"
PNP_INFO_CLASS = "1200"
GENERIC_NETWORKING_CLASS = "1201"
GENERIC_FILETRANS_CLASS = "1202"
GENERIC_AUDIO_CLASS = "1203"
GENERIC_TELEPHONY_CLASS = "1204"
UPNP_CLASS = "1205"
UPNP_IP_CLASS = "1206"
UPNP_PAN_CLASS = "1300"
UPNP_LAP_CLASS = "1301"
UPNP_L2CAP_CLASS = "1302"
VIDEO_SOURCE_CLASS = "1303"
VIDEO_SINK_CLASS = "1304"

# Bluetooth Profile Descriptors
SDP_SERVER_PROFILE = ( SDP_SERVER_CLASS, 0x0100)
BROWSE_GRP_DESC_PROFILE = ( BROWSE_GRP_DESC_CLASS, 0x0100)
SERIAL_PORT_PROFILE = ( SERIAL_PORT_CLASS, 0x0100)
LAN_ACCESS_PROFILE = ( LAN_ACCESS_CLASS, 0x0100)
DIALUP_NET_PROFILE = ( DIALUP_NET_CLASS, 0x0100)
IRMC_SYNC_PROFILE = ( IRMC_SYNC_CLASS, 0x0100)
OBEX_OBJPUSH_PROFILE = ( OBEX_OBJPUSH_CLASS, 0x0100)
OBEX_FILETRANS_PROFILE = ( OBEX_FILETRANS_CLASS, 0x0100)
IRMC_SYNC_CMD_PROFILE = ( IRMC_SYNC_CMD_CLASS, 0x0100)
HEADSET_PROFILE = ( HEADSET_CLASS, 0x0100)
CORDLESS_TELEPHONY_PROFILE = ( CORDLESS_TELEPHONY_CLASS, 0x0100)
AUDIO_SOURCE_PROFILE = ( AUDIO_SOURCE_CLASS, 0x0100)
AUDIO_SINK_PROFILE = ( AUDIO_SINK_CLASS, 0x0100)
AV_REMOTE_TARGET_PROFILE = ( AV_REMOTE_TARGET_CLASS, 0x0100)
ADVANCED_AUDIO_PROFILE = ( ADVANCED_AUDIO_CLASS, 0x0100)
AV_REMOTE_PROFILE = ( AV_REMOTE_CLASS, 0x0100)
VIDEO_CONF_PROFILE = ( VIDEO_CONF_CLASS, 0x0100)
INTERCOM_PROFILE = ( INTERCOM_CLASS, 0x0100)
FAX_PROFILE = ( FAX_CLASS, 0x0100)
HEADSET_AGW_PROFILE = ( HEADSET_AGW_CLASS, 0x0100)
WAP_PROFILE = ( WAP_CLASS, 0x0100)
WAP_CLIENT_PROFILE = ( WAP_CLIENT_CLASS, 0x0100)
PANU_PROFILE = ( PANU_CLASS, 0x0100)
NAP_PROFILE = ( NAP_CLASS, 0x0100)
GN_PROFILE = ( GN_CLASS, 0x0100)
DIRECT_PRINTING_PROFILE = ( DIRECT_PRINTING_CLASS, 0x0100)
REFERENCE_PRINTING_PROFILE = ( REFERENCE_PRINTING_CLASS, 0x0100)
IMAGING_PROFILE = ( IMAGING_CLASS, 0x0100)
IMAGING_RESPONDER_PROFILE = ( IMAGING_RESPONDER_CLASS, 0x0100)
IMAGING_ARCHIVE_PROFILE = ( IMAGING_ARCHIVE_CLASS, 0x0100)
IMAGING_REFOBJS_PROFILE = ( IMAGING_REFOBJS_CLASS, 0x0100)
HANDSFREE_PROFILE = ( HANDSFREE_CLASS, 0x0100)
HANDSFREE_AGW_PROFILE = ( HANDSFREE_AGW_CLASS, 0x0100)
DIRECT_PRT_REFOBJS_PROFILE = ( DIRECT_PRT_REFOBJS_CLASS, 0x0100)
REFLECTED_UI_PROFILE = ( REFLECTED_UI_CLASS, 0x0100)
BASIC_PRINTING_PROFILE = ( BASIC_PRINTING_CLASS, 0x0100)
PRINTING_STATUS_PROFILE = ( PRINTING_STATUS_CLASS, 0x0100)
HID_PROFILE = ( HID_CLASS, 0x0100)
HCR_PROFILE = ( HCR_SCAN_CLASS, 0x0100)
HCR_PRINT_PROFILE = ( HCR_PRINT_CLASS, 0x0100)
HCR_SCAN_PROFILE = ( HCR_SCAN_CLASS, 0x0100)
CIP_PROFILE = ( CIP_CLASS, 0x0100)
VIDEO_CONF_GW_PROFILE = ( VIDEO_CONF_GW_CLASS, 0x0100)
UDI_MT_PROFILE = ( UDI_MT_CLASS, 0x0100)
UDI_TA_PROFILE = ( UDI_TA_CLASS, 0x0100)
AV_PROFILE = ( AV_CLASS, 0x0100)
SAP_PROFILE = ( SAP_CLASS, 0x0100)
PNP_INFO_PROFILE = ( PNP_INFO_CLASS, 0x0100)
GENERIC_NETWORKING_PROFILE = ( GENERIC_NETWORKING_CLASS, 0x0100)
GENERIC_FILETRANS_PROFILE = ( GENERIC_FILETRANS_CLASS, 0x0100)
GENERIC_AUDIO_PROFILE = ( GENERIC_AUDIO_CLASS, 0x0100)
GENERIC_TELEPHONY_PROFILE = ( GENERIC_TELEPHONY_CLASS, 0x0100)
UPNP_PROFILE = ( UPNP_CLASS, 0x0100)
UPNP_IP_PROFILE = ( UPNP_IP_CLASS, 0x0100)
UPNP_PAN_PROFILE = ( UPNP_PAN_CLASS, 0x0100)
UPNP_LAP_PROFILE = ( UPNP_LAP_CLASS, 0x0100)
UPNP_L2CAP_PROFILE = ( UPNP_L2CAP_CLASS, 0x0100)
VIDEO_SOURCE_PROFILE = ( VIDEO_SOURCE_CLASS, 0x0100)
VIDEO_SINK_PROFILE = ( VIDEO_SINK_CLASS, 0x0100)

# Universal Service Attribute IDs
SERVICE_RECORD_HANDLE_ATTRID = 0x0000
SERVICE_CLASS_ID_LIST_ATTRID = 0x0001
SERVICE_RECORD_STATE_ATTRID = 0x0002
SERVICE_ID_ATTRID = 0x0003
PROTOCOL_DESCRIPTOR_LIST_ATTRID = 0x0004
BROWSE_GROUP_LIST_ATTRID = 0x0005
LANGUAGE_BASE_ATTRID_LIST_ATTRID = 0x0006
SERVICE_INFO_TIME_TO_LIVE_ATTRID = 0x0007
SERVICE_AVAILABILITY_ATTRID = 0x0008
BLUETOOTH_PROFILE_DESCRIPTOR_LIST_ATTRID = 0x0009
DOCUMENTATION_URL_ATTRID = 0x000a
CLIENT_EXECUTABLE_URL_ATTRID = 0x000b
ICON_URL_ATTRID = 0x000c
SERVICE_NAME_ATTRID = 0x0100
SERVICE_DESCRIPTION_ATTRID = 0x0101
PROVIDER_NAME_ATTRID = 0x0102

# Protocol UUIDs
SDP_UUID       = "0001"
UDP_UUID       = "0002"
RFCOMM_UUID    = "0003"
TCP_UUID       = "0004"
TCS_BIN_UUID   = "0005"
TCS_AT_UUID    = "0006"
OBEX_UUID      = "0008"
IP_UUID        = "0009"
FTP_UUID       = "000a"
HTTP_UUID      = "000c"
WSP_UUID       = "000e"
BNEP_UUID      = "000f"
UPNP_UUID      = "0010"
HIDP_UUID      = "0011"
HCRP_CTRL_UUID = "0012"
HCRP_DATA_UUID = "0014"
HCRP_NOTE_UUID = "0016"
AVCTP_UUID     = "0017"
AVDTP_UUID     = "0019"
CMTP_UUID      = "001b"
UDI_UUID       = "001d"
L2CAP_UUID     = "0100"

# Inquiry Access Codes
IAC_GIAC = 0x9e8b33
IAC_LIAC = 0x9e8b00

class BluetoothError (IOError):
    pass

def is_valid_address (s):
    """returns True if address is a valid Bluetooth address.

    valid address are always strings of the form XX:XX:XX:XX:XX:XX
    where X is a hexadecimal character.  For example,
    01:23:45:67:89:AB is a valid address, but IN:VA:LI:DA:DD:RE is not.

    """
    try:
        pairs = s.split (":")
        if len (pairs) != 6: return False
        if not all(0 <= int(b, 16) <= 255 for b in pairs): return False
    except:
        return False
    return True

def is_valid_uuid (uuid):
    """
    is_valid_uuid (uuid) -> bool

    returns True if uuid is a valid 128-bit UUID.

    valid UUIDs are always strings taking one of the following forms:
    XXXX
    XXXXXXXX
    XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    where each X is a hexadecimal digit (case insensitive)

    """
    try:
        if len (uuid) == 4:
            if int (uuid, 16) < 0: return False
        elif len (uuid) == 8:
            if int (uuid, 16) < 0: return False
        elif len (uuid) == 36:
            pieces = uuid.split ("-")
            if len (pieces) != 5 or \
                    len (pieces[0]) != 8 or \
                    len (pieces[1]) != 4 or \
                    len (pieces[2]) != 4 or \
                    len (pieces[3]) != 4 or \
                    len (pieces[4]) != 12:
                return False
            [ int (p, 16) for p in pieces ]
        else:
            return False
    except ValueError: 
        return False
    except TypeError:
        return False
    return True

def to_full_uuid (uuid):
    """
    converts a short 16-bit or 32-bit reserved UUID to a full 128-bit Bluetooth
    UUID.

    """
    if not is_valid_uuid (uuid): raise ValueError ("invalid UUID")
    if len (uuid) == 4:
        return "0000%s-0000-1000-8000-00805F9B34FB" % uuid
    elif len (uuid) == 8:
        return "%s-0000-1000-8000-00805F9B34FB" % uuid
    else:
        return uuid

# =============== parsing and constructing raw SDP records ============

def sdp_parse_size_desc (data):
    dts = struct.unpack ("B", data[0:1])[0]
    dtype, dsizedesc = dts >> 3, dts & 0x7
    dstart = 1
    if   dtype == 0:     dsize = 0
    elif dsizedesc == 0: dsize = 1
    elif dsizedesc == 1: dsize = 2
    elif dsizedesc == 2: dsize = 4
    elif dsizedesc == 3: dsize = 8
    elif dsizedesc == 4: dsize = 16
    elif dsizedesc == 5:
        dsize = struct.unpack ("B", data[1:2])[0]
        dstart += 1
    elif dsizedesc == 6:
        dsize = struct.unpack ("!H", data[1:3])[0]
        dstart += 2
    elif dsizedesc == 7:
        dsize = struct.unpack ("!I", data[1:5])[0]
        dstart += 4

    if dtype > 8:
        raise ValueError ("Invalid TypeSizeDescriptor byte %s %d, %d" \
                % (binascii.hexlify (data[0:1]), dtype, dsizedesc))

    return dtype, dsize, dstart

def sdp_parse_uuid (data, size):
    if size == 2:
        return binascii.hexlify (data)
    elif size == 4:
        return binascii.hexlify (data)
    elif size == 16:
        return "%08X-%04X-%04X-%04X-%04X%08X" % struct.unpack ("!IHHHHI", data)
    else: return ValueError ("invalid UUID size")

def sdp_parse_int (data, size, signed):
    fmts = { 1 : "!b" , 2 : "!h" , 4 : "!i" , 8 : "!q" , 16 : "!qq" }
    fmt = fmts[size]
    if not signed: fmt = fmt.upper ()
    if fmt in [ "!qq", "!QQ" ]:
        upp, low = struct.unpack ("!QQ", data)
        result = ( upp << 64) | low
        if signed:
            result=- ((~ (result-1))&0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
        return result
    else:
        return struct.unpack (fmt, data)[0]

def sdp_parse_data_elementSequence (data):
    result = []
    pos = 0
    datalen = len (data)
    while pos < datalen:
        rtype, rval, consumed = sdp_parse_data_element (data[pos:])
        pos += consumed
        result.append ( (rtype, rval))
    return result

def sdp_parse_data_element (data):
    dtype, dsize, dstart = sdp_parse_size_desc (data)
    elem = data[dstart:dstart+dsize]

    if dtype == 0:
        rtype, rval = "Nil", None
    elif dtype == 1:
        rtype, rval = "UInt%d"% (dsize*8), sdp_parse_int (elem, dsize, False)
    elif dtype == 2:
        rtype, rval = "SInt%d"% (dsize*8), sdp_parse_int (elem, dsize, True)
    elif dtype == 3:
        rtype, rval = "UUID", sdp_parse_uuid (elem, dsize)
    elif dtype == 4:
        rtype, rval = "String", elem
    elif dtype == 5:
        rtype, rval = "Bool", (struct.unpack ("B", elem)[0] != 0)
    elif dtype == 6:
        rtype, rval = "ElemSeq", sdp_parse_data_elementSequence (elem)
    elif dtype == 7:
        rtype, rval = "AltElemSeq", sdp_parse_data_elementSequence (elem)
    elif dtype == 8:
        rtype, rval = "URL", elem

    return rtype, rval, dstart+dsize

def sdp_parse_raw_record (data):
    dtype, dsize, dstart = sdp_parse_size_desc (data)
    assert dtype == 6

    pos = dstart
    datalen = len (data)
    record = {}
    while pos < datalen:
        type, attrid, consumed = sdp_parse_data_element (data[pos:])
        assert type == "UInt16"
        pos += consumed
        type, attrval, consumed = sdp_parse_data_element (data[pos:])
        pos += consumed
        record[attrid] = attrval
    return record

def sdp_make_data_element (type, value):
    def maketsd (tdesc, sdesc):
        return struct.pack ("B", (tdesc << 3) | sdesc)
    def maketsdl (tdesc, size):
        if   size < (1<<8):  return struct.pack ("!BB", tdesc << 3 | 5, size)
        elif size < (1<<16): return struct.pack ("!BH", tdesc << 3 | 6, size)
        else:                return struct.pack ("!BI", tdesc << 3 | 7, size)

    easyinttypes = { "UInt8"   : (1, 0, "!B"),  "UInt16"  : (1, 1, "!H"),
                     "UInt32"  : (1, 2, "!I"),  "UInt64"  : (1, 3, "!Q"),
                     "SInt8"   : (2, 0, "!b"),  "SInt16"  : (2, 1, "!h"),
                     "SInt32"  : (2, 2, "!i"),  "SInt64"  : (2, 3, "!q"),
                     }

    if type == "Nil": 
        return maketsd (0, 0)
    elif type in easyinttypes:
        tdesc, sdesc, fmt = easyinttypes[type]
        return maketsd (tdesc, sdesc) + struct.pack (fmt, value)
    elif type == "UInt128":
        ts = maketsd (1, 4)
        upper = ts >> 64
        lower = (ts & 0xFFFFFFFFFFFFFFFF)
        return ts + struct.pack ("!QQ", upper, lower)
    elif type == "SInt128":
        ts = maketsd (2, 4)
        # FIXME
        raise NotImplementedError ("128-bit signed int NYI!")
    elif type == "UUID":
        if len (value) == 4:
            return maketsd (3, 1) + binascii.unhexlify (value)
        elif len (value) == 8:
            return maketsd (3, 2) + binascii.unhexlify (value)
        elif len (value) == 36:
            return maketsd (3, 4) + binascii.unhexlify (value.replace ("-",""))
    elif type == "String":
        return maketsdl (4, len (value)) + str.encode(value)
    elif type == "Bool":
        return maketsd (5,0) + (value and "\x01" or "\x00")
    elif type == "ElemSeq":
        packedseq = bytes()
        for subtype, subval in value:
            nextelem = sdp_make_data_element (subtype, subval)
            packedseq = packedseq + nextelem
        return maketsdl (6, len (packedseq)) + packedseq
    elif type == "AltElemSeq":
        packedseq = bytes()
        for subtype, subval in value:
            packedseq = packedseq + sdp_make_data_element (subtype, subval)
        return maketsdl (7, len (packedseq)) + packedseq
    elif type == "URL":
        return maketsdl (8, len (value)) + value
    else:
        raise ValueError ("invalid type %s" % type)
  build: import lightblue
from .btcommon import *

def discover_devices(duration=8, flush_cache=True, lookup_names=False,
        lookup_class=False, device_id=-1):
    # This is order of discovered device attributes in C-code.
    btAddresIndex = 0
    namesIndex = 1
    classIndex = 2

    # Use lightblue to discover devices on OSX.
    devices = lightblue.finddevices(getnames=lookup_names, length=duration)

    ret = list()
    for device in devices:
        item = [device[btAddresIndex], ]
        if lookup_names:
            item.append(device[namesIndex])
        if lookup_class:
            item.append(device[classIndex])

        # in case of address-only we return string not tuple
        if len(item) == 1:
            ret.append(item[0])
        else:
            ret.append(tuple(item))
    return ret


def lookup_name(address, timeout=10):
    print("TODO: implement")

# TODO: After a little looking around, it seems that we can go into some of the 
# lightblue internals and enhance the amount of SDP information that is returned 
# (e.g., CID/PSM, protocol, provider information).
#
# See: _searchservices() in _lightblue.py
def find_service(name=None, uuid=None, address=None):
    if address is not None:
        addresses = [address]
    else:
        addresses = discover_devices(lookup_names=False)

    results = []

    for address in addresses:
        # print "[DEBUG] Browsing services on %s..." % (addr)

        dresults = lightblue.findservices(addr=address, name=name)

        for tup in dresults:
            service = {}

            # LightBlue performs a service discovery and returns the found
            # services as a list of (device-address, service-port,
            # service-name) tuples.
            service["host"] = tup[0]
            service["port"] = tup[1]
            service["name"] = tup[2]

            # Add extra keys for compatibility with PyBluez API.
            service["description"] = None
            service["provider"] = None
            service["protocol"] = None
            service["service-classes"] = []
            service["profiles"] = []
            service["service-id"] = None

            results.append(service)

    return results


def read_local_bdaddr():
    return [lightblue.gethostaddr()]


def advertise_service(sock, name, service_id="", service_classes=None,
        profiles=None, provider="", description="", protocols=None):

    if protocols is None or protocols == RFCOMM:
        protocols = [lightblue.RFCOMM]

    lightblue.advertise(name, sock, protocols[0], service_id)


def stop_advertising(sock):
    lightblue.stop_advertising(sock)


# ============================= BluetoothSocket ============================== #
class BluetoothSocket:

    def __init__(self, proto=RFCOMM, _sock=None):
        if _sock is None:
            _sock = lightblue.socket()
        self._sock = _sock

        if proto != RFCOMM:
            # name the protocol
            raise NotImplementedError("Not supported protocol")
        self._proto = lightblue.RFCOMM
        self._addrport = None

    def _getport(self):
        return self._addrport[1]

    def bind(self, addrport):
        self._addrport = addrport
        return self._sock.bind(addrport)

    def listen(self, backlog):
        return self._sock.listen(backlog)

    def accept(self):
        return self._sock.accept()

    def connect(self, addrport):
        return self._sock.connect(addrport)

    def send(self, data):
        return self._sock.send(data)

    def recv(self, numbytes):
        return self._sock.recv(numbytes)

    def close(self):
        return self._sock.close()

    def getsockname(self):
        return self._sock.getsockname()

    def setblocking(self, blocking):
        return self._sock.setblocking(blocking)

    def settimeout(self, timeout):
        return self._sock.settimeout(timeout)

    def gettimeout(self):
        return self._sock.gettimeout()

    def fileno(self):
        return self._sock.fileno()

    def dup(self):
        return BluetoothSocket(self._proto, self._sock)

    def makefile(self, mode, bufsize):
        return self.makefile(mode, bufsize)

# ============================= DeviceDiscoverer ============================= #

class DeviceDiscoverer:
    def __init__ (self):
        raise NotImplementedError

    runs-on: ubuntu-latest
    strategy: from bluetooth import *
import bluetooth._msbt as bt

bt.initwinsock ()

# ============== SDP service registration and unregistration ============

def discover_devices (duration=8, flush_cache=True, lookup_names=False,
                      lookup_class=False, device_id=-1):
    #this is order of items in C-code
    btAddresIndex = 0
    namesIndex = 1
    classIndex = 2

    try:
        devices = bt.discover_devices(duration=duration, flush_cache=flush_cache)
    except OSError:
        return []
    ret = list()
    for device in devices:
        item = [device[btAddresIndex],]
        if lookup_names:
            item.append(device[namesIndex])
        if lookup_class:
            item.append(device[classIndex])

        if len(item) == 1: # in case of address-only we return string not tuple
            ret.append(item[0])
        else:
            ret.append(tuple(i for i in item))
    return ret


def read_local_bdaddr():
    return bt.list_local()


def lookup_name (address, timeout=10):
    if not is_valid_address (address): 
        raise ValueError ("Invalid Bluetooth address")
    try:
        return bt.lookup_name(address)
    except OSError:
        return None


class BluetoothSocket:
    def __init__ (self, proto = RFCOMM, sockfd = None):
        if proto not in [ RFCOMM ]:
            raise ValueError ("invalid protocol")

        if sockfd:
            self._sockfd = sockfd
        else:
            self._sockfd = bt.socket (bt.SOCK_STREAM, bt.BTHPROTO_RFCOMM)
        self._proto = proto

        # used by advertise_service and stop_advertising
        self._sdp_handle = None
        self._raw_sdp_record = None

        # used to track if in blocking or non-blocking mode (FIONBIO appears
        # write only)
        self._blocking = True
        self._timeout = False

    @property
    def family (self):
        return bt.AF_BTH

    @property
    def type (self):
        return bt.SOCK_STREAM

    @property
    def proto (self):
        return bt.BTHPROTO_RFCOMM

    def bind (self, addrport):
        if self._proto == RFCOMM:
            addr, port = addrport

            if port == 0: port = bt.BT_PORT_ANY
            bt.bind (self._sockfd, addr, port)

    def listen (self, backlog):
        bt.listen (self._sockfd, backlog)

    def accept (self):
        clientfd, addr, port = bt.accept (self._sockfd)
        client = BluetoothSocket (self._proto, sockfd=clientfd)
        return client, (addr, port)

    def connect (self, addrport):
        addr, port = addrport
        bt.connect (self._sockfd, addr, port)

    def send (self, data):
        return bt.send (self._sockfd, data)

    def recv (self, numbytes):
        return bt.recv (self._sockfd, numbytes)

    def close (self):
        return bt.close (self._sockfd)

    def getsockname (self):
        return bt.getsockname (self._sockfd)

    def getpeername (self):
        return bt.getpeername (self._sockfd)

    getpeername.__doc__ = bt.getpeername.__doc__

    def setblocking (self, blocking):
        bt.setblocking (self._sockfd, blocking)
        self._blocking = blocking

    def settimeout (self, timeout):
        if timeout < 0: raise ValueError ("invalid timeout")

        if timeout == 0:
            self.setblocking (False)
        else:
            self.setblocking (True)

        bt.settimeout (self._sockfd, timeout)
        self._timeout = timeout

    def gettimeout (self):    
        if self._blocking and not self._timeout: return None
        return bt.gettimeout (self._sockfd)

    def fileno (self):
        return self._sockfd

    def dup (self):
        return BluetoothSocket (self._proto, sockfd=bt.dup (self._sockfd))

    def makefile (self):
        # TODO
        raise Exception("Not yet implemented")


def advertise_service (sock, name, service_id = "", service_classes = [], \
        profiles = [], provider = "", description = "", protocols = []):
    if service_id != "" and not is_valid_uuid (service_id):
        raise ValueError ("invalid UUID specified for service_id")
    for uuid in service_classes:
        if not is_valid_uuid (uuid):
            raise ValueError ("invalid UUID specified in service_classes")
    for uuid, version in profiles:
        if not is_valid_uuid (uuid) or  version < 0 or  version > 0xFFFF:
            raise ValueError ("Invalid Profile Descriptor")
    for uuid in protocols:
        if not is_valid_uuid (uuid):
            raise ValueError ("invalid UUID specified in protocols")        

    if sock._raw_sdp_record is not None:
        raise OSError("service already advertised")

    avpairs = []

    # service UUID
    if len (service_id) > 0:
        avpairs.append (("UInt16", SERVICE_ID_ATTRID))
        avpairs.append (("UUID", service_id))

    # service class list
    if len (service_classes) > 0:
        seq = [ ("UUID", svc_class) for svc_class in service_classes ]
        avpairs.append (("UInt16", SERVICE_CLASS_ID_LIST_ATTRID))
        avpairs.append (("ElemSeq", seq))

    # set protocol and port information
    assert sock._proto == RFCOMM
    addr, port = sock.getsockname ()
    avpairs.append (("UInt16", PROTOCOL_DESCRIPTOR_LIST_ATTRID))
    l2cap_pd = ("ElemSeq", (("UUID", L2CAP_UUID),))
    rfcomm_pd = ("ElemSeq", (("UUID", RFCOMM_UUID), ("UInt8", port)))
    proto_list = [ l2cap_pd, rfcomm_pd ]
    for proto_uuid in protocols:
        proto_list.append (("ElemSeq", (("UUID", proto_uuid),)))
    avpairs.append (("ElemSeq", proto_list))

    # make the service publicly browseable
    avpairs.append (("UInt16", BROWSE_GROUP_LIST_ATTRID))
    avpairs.append (("ElemSeq", (("UUID", PUBLIC_BROWSE_GROUP),)))

    # profile descriptor list
    if len (profiles) > 0:
        seq = [ ("ElemSeq", (("UUID",uuid), ("UInt16",version))) \
                for uuid, version in profiles ]
        avpairs.append (("UInt16", 
            BLUETOOTH_PROFILE_DESCRIPTOR_LIST_ATTRID))
        avpairs.append (("ElemSeq", seq))

    # service name
    avpairs.append (("UInt16", SERVICE_NAME_ATTRID))
    avpairs.append (("String", name))

    # service description
    if len (description) > 0:
        avpairs.append (("UInt16", SERVICE_DESCRIPTION_ATTRID))
        avpairs.append (("String", description))

    # service provider
    if len (provider) > 0:
        avpairs.append (("UInt16", PROVIDER_NAME_ATTRID))
        avpairs.append (("String", provider))

    sock._raw_sdp_record = sdp_make_data_element ("ElemSeq", avpairs)
#    pr = sdp_parse_raw_record (sock._raw_sdp_record)
#    for attrid, val in pr.items ():
#        print "%5s: %s" % (attrid, val)
#    print binascii.hexlify (sock._raw_sdp_record)
#    print repr (sock._raw_sdp_record)

    sock._sdp_handle = bt.set_service_raw (sock._raw_sdp_record, True)

def stop_advertising (sock):
    if sock._raw_sdp_record is None:
        raise OSError("service isn't advertised, " \
                        "but trying to un-advertise")
    bt.set_service_raw (sock._raw_sdp_record, False, sock._sdp_handle)
    sock._raw_sdp_record = None
    sock._sdp_handle = None

def find_service (name = None, uuid = None, address = None):
    if address is not None:
        addresses = [ address ]
    else:
        addresses = discover_devices (lookup_names = False)

    results = []

    for addr in addresses:
        uuidstr = uuid or PUBLIC_BROWSE_GROUP
        if not is_valid_uuid (uuidstr): raise ValueError ("invalid UUID")

        uuidstr = to_full_uuid (uuidstr)

        dresults = bt.find_service (addr, uuidstr)

        for dict in dresults:
            raw = dict["rawrecord"]

            record = sdp_parse_raw_record (raw)

            if SERVICE_CLASS_ID_LIST_ATTRID in record:
                svc_class_id_list = [ t[1] for t in \
                        record[SERVICE_CLASS_ID_LIST_ATTRID] ]
                dict["service-classes"] = svc_class_id_list
            else:
                dict["services-classes"] = []

            if BLUETOOTH_PROFILE_DESCRIPTOR_LIST_ATTRID in record:
                pdl = []
                for profile_desc in \
                        record[BLUETOOTH_PROFILE_DESCRIPTOR_LIST_ATTRID]:
                    uuidpair, versionpair = profile_desc[1]
                    pdl.append ((uuidpair[1], versionpair[1]))
                dict["profiles"] = pdl
            else:
                dict["profiles"] = []

            dict["provider"] = record.get (PROVIDER_NAME_ATTRID, None)

            dict["service-id"] = record.get (SERVICE_ID_ATTRID, None)

            # XXX the C version is buggy (retrieves an extra byte or two),
            # so get the service name here even though it may have already
            # been set
            dict["name"] = record.get (SERVICE_NAME_ATTRID, None)

            dict["handle"] = record.get (SERVICE_RECORD_HANDLE_ATTRID, None)

#        if LANGUAGE_BASE_ATTRID_LIST_ATTRID in record:
#            for triple in record[LANGUAGE_BASE_ATTRID_LIST_ATTRID]:
#                code_ISO639, encoding, base_offset = triple
#
#        if SERVICE_DESCRIPTION_ATTRID in record:
#            service_description = record[SERVICE_DESCRIPTION_ATTRID]

        if name is None:
            results.extend (dresults)
        else:
            results.extend ([ d for d in dresults if d["name"] == name ])
    return results

# =============== DeviceDiscoverer ==================
class DeviceDiscoverer:
    def __init__ (self):
        raise NotImplementedError
      fail-fast: false
      matrix: /*

This module provides an interface to bluetooth.  A great deal of the code is
taken from the pyaffix project.

- there are three kinds of bluetooth addresses used here
  HCI address is a single int specifying the device id
  L2CAP address is a pair (host, port)
  RFCOMM address is a pair (host, channel)
  SCO address is just a host
  the host part of the address is always a string of the form "XX:XX:XX:XX:XX"

Local naming conventions:

- names starting with sock_ are socket object methods
- names starting with bt_ are module-level functions

*/
#define PY_SSIZE_T_CLEAN 1
#include "Python.h"
#include "btmodule.h"
#include "structmember.h"

#include "pythoncapi_compat.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <errno.h>
#include <netdb.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sco.h>

#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include "btsdp.h"

/* Socket object documentation */
PyDoc_STRVAR(sock_doc,
"BluetoothSocket(proto=RFCOMM) -> bluetooth socket object\n\
\n\
Open a socket of the given protocol.  proto must be one of\n\
HCI, L2CAP, RFCOMM, or SCO.  SCO sockets have\n\
not been tested at all yet.\n\
\n\
A BluetoothSocket object represents one endpoint of a bluetooth connection.\n\
\n\
Methods of BluetoothSocket objects (keyword arguments not allowed):\n\
\n\
accept() -- accept a connection, returning new socket and client address\n\
bind(addr) -- bind the socket to a local address\n\
close() -- close the socket\n\
connect(addr) -- connect the socket to a remote address\n\
connect_ex(addr) -- connect, return an error code instead of an exception\n\
dup() -- return a new socket object identical to the current one\n\
fileno() -- return underlying file descriptor\n\
getpeername() -- return remote address\n\
getsockname() -- return local address\n\
getsockopt(level, optname[, buflen]) -- get socket options\n\
gettimeout() -- return timeout or None\n\
listen(n) -- start listening for incoming connections\n\
makefile([mode, [bufsize]]) -- return a file object for the socket\n\
recv(buflen[, flags]) -- receive data\n\
recvfrom(buflen[, flags]) -- receive data and sender's address\n\
sendall(data[, flags]) -- send all data\n\
send(data[, flags]) -- send data, may not send all of it\n\
sendto(data[, flags], addr) -- send data to a given address\n\
setblocking(0 | 1) -- set or clear the blocking I/O flag\n\
setsockopt(level, optname, value) -- set socket options\n\
settimeout(None | float) -- set or clear the timeout\n\
shutdown(how) -- shut down traffic in one or both directions");


/* Global variable holding the exception type for errors detected
   by this module (but not argument type or memory errors, etc.). */
PyObject *bluetooth_error;
static PyObject *socket_timeout;

/* A forward reference to the socket type object.
   The sock_type variable contains pointers to various functions,
   some of which call new_sockobject(), which uses sock_type, so
   there has to be a circular reference. */
PyTypeObject sock_type;

/* Convenience function to raise an error according to errno
   and return a NULL pointer from a function. */
PyObject *
set_error(void)
{
        return PyErr_SetFromErrno(bluetooth_error);
}


/* Function to perform the setting of socket blocking mode
   internally. block = (1 | 0). */
static int
internal_setblocking(PySocketSockObject *s, int block)
{
        int delay_flag;

        Py_BEGIN_ALLOW_THREADS
        delay_flag = fcntl(s->sock_fd, F_GETFL, 0);
        if (block)
                delay_flag &= (~O_NONBLOCK);
        else
                delay_flag |= O_NONBLOCK;
        fcntl(s->sock_fd, F_SETFL, delay_flag);
        Py_END_ALLOW_THREADS

        /* Since these don't return anything */
        return 1;
}

/* Do a select() on the socket, if necessary (sock_timeout > 0).
   The argument writing indicates the direction.
   This does not raise an exception; we'll let our caller do that
   after they've reacquired the interpreter lock.
   Returns 1 on timeout, 0 otherwise. */
static int
internal_select(PySocketSockObject *s, int writing)
{
        fd_set fds;
        struct timeval tv;
        int n;

        /* Nothing to do unless we're in timeout mode (not non-blocking) */
        if (s->sock_timeout <= 0.0)
                return 0;

        /* Guard against closed socket */
        if (s->sock_fd < 0)
                return 0;

        /* Construct the arguments to select */
        tv.tv_sec = (int)s->sock_timeout;
        tv.tv_usec = (int)((s->sock_timeout - tv.tv_sec) * 1e6);
        FD_ZERO(&fds);
        FD_SET(s->sock_fd, &fds);

        /* See if the socket is ready */
        if (writing)
                n = select(s->sock_fd+1, NULL, &fds, NULL, &tv);
        else
                n = select(s->sock_fd+1, &fds, NULL, NULL, &tv);
        if (n == 0)
                return 1;
        return 0;
}

/* Initialize a new socket object. */

static double defaulttimeout = -1.0; /* Default timeout for new sockets */

static void
init_sockobject(PySocketSockObject *s, int fd, int family, int type, int proto)
{
        s->sock_fd = fd;
        s->sock_family = family;
        s->sock_type = type;
        s->sock_proto = proto;
        s->sock_timeout = defaulttimeout;

        s->errorhandler = &set_error;

        if (defaulttimeout >= 0.0)
                internal_setblocking(s, 0);
}


/* Create a new socket object.
   This just creates the object and initializes it.
   If the creation fails, return NULL and set an exception (implicit
   in NEWOBJ()). */

static PySocketSockObject *
new_sockobject(int fd, int family, int type, int proto)
{
        PySocketSockObject *s;
        s = (PySocketSockObject *) PyType_GenericNew(&sock_type, NULL, NULL);
        if (s != NULL)
                init_sockobject(s, fd, family, type, proto);
        return s;
}


/* Create an object representing the given socket address,
   suitable for passing it back to bind(), connect() etc.
   The family field of the sockaddr structure is inspected
   to determine what kind of address it really is. */

/*ARGSUSED*/
static PyObject *
makesockaddr(PySocketSockObject *s, struct sockaddr *addr, int addrlen)
{
    if (addrlen == 0) {
        /* No address -- may be recvfrom() from known socket */
        Py_RETURN_NONE;
    } else {
        char ba_name[18];

        switch(s->sock_proto) {
            case BTPROTO_HCI:
                {
                    return Py_BuildValue("H",
                            ((struct sockaddr_hci*)(addr))->hci_dev );
                }
            case BTPROTO_L2CAP:
                {
                    struct sockaddr_l2 *a = (struct sockaddr_l2*)addr;
                    ba2str( &a->l2_bdaddr, ba_name );
                    return Py_BuildValue("sH", ba_name, btohs(a->l2_psm) );
                }
            case BTPROTO_RFCOMM:
                {
                    struct sockaddr_rc *a = (struct sockaddr_rc*)addr;
                    ba2str( &a->rc_bdaddr, ba_name );
                    return Py_BuildValue("sB", ba_name, a->rc_channel );
                }
            case BTPROTO_SCO:
                {
                    struct sockaddr_sco *a = (struct sockaddr_sco*)addr;
                    ba2str( &a->sco_bdaddr, ba_name );
                    return Py_BuildValue("s", ba_name);
                }
            default:
                PyErr_SetString(bluetooth_error,
                        "getsockaddrarg: unknown Bluetooth protocol");
                return 0;
        }
    }
}


/* Parse a socket address argument according to the socket object's
   address family.  Return 1 if the address was in the proper format,
   0 of not.  The address is returned through addr_ret, its length
   through len_ret. */

static int
getsockaddrarg(PySocketSockObject *s, PyObject *args,
               struct sockaddr *addr_ret, int *len_ret)
{
    memset(addr_ret, 0, sizeof(struct sockaddr));
    addr_ret->sa_family = AF_BLUETOOTH;

    switch( s->sock_proto )
    {
        case BTPROTO_HCI:
            {
                struct sockaddr_hci *addr = (struct sockaddr_hci*) addr_ret;
                int device;
                int channel = HCI_CHANNEL_RAW;
                if ( !PyArg_ParseTuple(args, "i|H", &device, &channel) ) {
                    return 0;
                }
                if (device == -1) {
                    addr->hci_dev = HCI_DEV_NONE;
                } else {
                    addr->hci_dev = device;
                }

                addr->hci_channel = channel;

                *len_ret = sizeof(struct sockaddr_hci);
                return 1;
            }
        case BTPROTO_L2CAP:
            {
                struct sockaddr_l2* addr = (struct sockaddr_l2*) addr_ret;
                char *ba_name = 0;

                if ( !PyArg_ParseTuple(args, "sH", &ba_name, &addr->l2_psm) )
                {
                    return 0;
                }

                str2ba( ba_name, &addr->l2_bdaddr );

                // check for a valid PSM
                if( ! ( 0x1 & addr->l2_psm ) ) {
                    PyErr_SetString( PyExc_ValueError, "Invalid PSM");
                    return 0;
                }

                addr->l2_psm = htobs(addr->l2_psm);

                *len_ret = sizeof *addr;
                return 1;
            }

        case BTPROTO_RFCOMM:
            {
                struct sockaddr_rc *addr = (struct sockaddr_rc*) addr_ret;
                char *ba_name = 0;

                if( !PyArg_ParseTuple(args, "sB", &ba_name, &addr->rc_channel) )
                {
                    return 0;
                }

                str2ba( ba_name, &addr->rc_bdaddr );
                *len_ret = sizeof *addr;
                return 1;
            }
        case BTPROTO_SCO:
            {
                struct sockaddr_sco *addr = (struct sockaddr_sco*) addr_ret;
                char *ba_name = 0;

                if( !PyArg_ParseTuple(args, "s", &ba_name) )
                {
                    return 0;
                }

                str2ba( ba_name, &addr->sco_bdaddr);
                *len_ret = sizeof *addr;
                return 1;
            }
        default:
            {
                PyErr_SetString(bluetooth_error,
                        "getsockaddrarg: unknown Bluetooth protocol");
                return 0;
            }
    }
}

/* Determin device can be advertisable('UP, RUNNING, PSCAN, ISCAN' state).
   If advertisable, return 0. Else, return -1. */
static int
_adv_available(struct hci_dev_info *di)
{
    uint32_t *flags = &di->flags;

    if (hci_test_bit(HCI_RAW, &flags) && !bacmp(&di->bdaddr, BDADDR_ANY)) {
        int dd = hci_open_dev(di->dev_id);
        if (dd < 0)
            return -1;
        hci_read_bd_addr(dd, &di->bdaddr, 1000);
        hci_close_dev(dd);
    }

    return (hci_test_bit(HCI_UP, flags) &&
             hci_test_bit(HCI_RUNNING, flags) &&
             hci_test_bit(HCI_PSCAN, flags) &&
             hci_test_bit(HCI_ISCAN, flags)) != 0 ? 0 : -1;
}

/* Inspect all devices in order to know whether advertisable device exists. */
static int
_any_adv_available(void)
{
    struct hci_dev_list_req *dl    = NULL;
    struct hci_dev_req      *dr    = NULL;
    struct hci_dev_info     di     = {0,};
    int                     result = -1;
    int                     ctl    = -1;
    int                     i;

    if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0) {
        return -1;
    }

    if (!(dl = malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) +
        sizeof(uint16_t)))) {
        goto CLEAN_UP_RETURN;
    }
    dl->dev_num = HCI_MAX_DEV;
    dr = dl->dev_req;

    if (ioctl(ctl, HCIGETDEVLIST, (void *) dl) < 0) {
        goto CLEAN_UP_RETURN;
    }

    for (i = 0; i< dl->dev_num; i++) {
        di.dev_id = (dr+i)->dev_id;
        if (ioctl(ctl, HCIGETDEVINFO, (void *) &di) < 0)
            continue;

        /* if find adv-available device, return */
        if(_adv_available(&di) == 0)
        {
            result = 0;
            goto CLEAN_UP_RETURN;
        }
    }

CLEAN_UP_RETURN:
    close(ctl);
    free(dl);
    return result;
}

/* Determine the socket can be advertisable or not.
   It will work if socket is bound to specific device or not.
   If advertisable, return 0. Else, return -1 */
static int
adv_available(PySocketSockObject *socko)
{
    bdaddr_t           ba       = {{0, }}; /* GCC bug? */
    struct sockaddr    addr     = {0, };
    int                dev_id   = -1;
    socklen_t          alen     = sizeof(addr);
    struct sockaddr_l2 const * addr_l2  = (struct sockaddr_l2 const *)&addr;
    struct sockaddr_rc const * addr_rc  = (struct sockaddr_rc const *)&addr;

    /* get ba */
    if(getsockname(socko->sock_fd, &addr, &alen) < 0)
        return -1;
    switch(socko->sock_proto)
    {
    case BTPROTO_L2CAP:
        ba = addr_l2->l2_bdaddr;
        break;

    case BTPROTO_RFCOMM:
        ba = addr_rc->rc_bdaddr;
        break;

    default:
        /* The others are not yet implmented.
           In fact, I don't spec of the others.
           To have compatibility with old version, return 0(success). */
        return 0;
    }

    /* get dev_id from ba */
    if(bacmp(&ba, BDADDR_ANY) == 0)
        dev_id = -1;
    else
        dev_id = hci_get_route(&ba);

    if(dev_id == -1)
    /* if dev_id is not specified, inspect all devices. */
    {
        return _any_adv_available();
    }
    else
    /* if device specified, inspect it. */
    {
        struct hci_dev_info     di;
        if(hci_devinfo(dev_id, &di))
            return -1;

        return _adv_available(&di);
    }
}

/* Get the address length according to the socket object's address family.
   Return 1 if the family is known, 0 otherwise.  The length is returned
   through len_ret. */

int
getsockaddrlen(PySocketSockObject *s, socklen_t *len_ret)
{
    switch(s->sock_proto)
    {
        case BTPROTO_L2CAP:
            *len_ret = sizeof (struct sockaddr_l2);
            return 1;
        case BTPROTO_RFCOMM:
            *len_ret = sizeof (struct sockaddr_rc);
            return 1;
        case BTPROTO_SCO:
            *len_ret = sizeof (struct sockaddr_sco);
            return 1;
        case BTPROTO_HCI:
            *len_ret = sizeof (struct sockaddr_hci);
            return 1;
        default:
            PyErr_SetString(bluetooth_error,
                    "getsockaddrlen: unknown bluetooth protocol");
            return 0;
    }
}


int
str2uuid( const char *uuid_str, uuid_t *uuid )
{
    uint32_t uuid_int[4];
    char *endptr;

    if( strlen( uuid_str ) == 36 ) {
        // Parse uuid128 standard format: 12345678-9012-3456-7890-123456789012
        char buf[9] = { 0 };

        if( uuid_str[8] != '-' && uuid_str[13] != '-' &&
            uuid_str[18] != '-'  && uuid_str[23] != '-' ) {
            return 0;
        }
        // first 8-bytes
        strncpy(buf, uuid_str, 8);
        uuid_int[0] = htonl( strtoul( buf, &endptr, 16 ) );
        if( endptr != buf + 8 ) return 0;

        // second 8-bytes
        strncpy(buf, uuid_str+9, 4);
        strncpy(buf+4, uuid_str+14, 4);
        uuid_int[1] = htonl( strtoul( buf, &endptr, 16 ) );
        if( endptr != buf + 8 ) return 0;

        // third 8-bytes
        strncpy(buf, uuid_str+19, 4);
        strncpy(buf+4, uuid_str+24, 4);
        uuid_int[2] = htonl( strtoul( buf, &endptr, 16 ) );
        if( endptr != buf + 8 ) return 0;

        // fourth 8-bytes
        strncpy(buf, uuid_str+28, 8);
        uuid_int[3] = htonl( strtoul( buf, &endptr, 16 ) );
        if( endptr != buf + 8 ) return 0;

        if( uuid != NULL ) sdp_uuid128_create( uuid, uuid_int );
    } else if ( strlen( uuid_str ) == 8 ) {
        // 32-bit reserved UUID
        uint32_t i = strtoul( uuid_str, &endptr, 16 );
        if( endptr != uuid_str + 8 ) return 0;
        if( uuid != NULL ) sdp_uuid32_create( uuid, i );
    } else if( strlen( uuid_str ) == 4 ) {
        // 16-bit reserved UUID
        int i = strtol( uuid_str, &endptr, 16 );
        if( endptr != uuid_str + 4 ) return 0;
        if( uuid != NULL ) sdp_uuid16_create( uuid, i );
    } else {
        return 0;
    }

    return 1;
}

int
pyunicode2uuid( PyObject *item, uuid_t *uuid )
{
    PyObject* ascii = PyUnicode_AsASCIIString( item );
    int ret =  str2uuid( PyBytes_AsString( ascii ), uuid );
    Py_XDECREF( ascii );
    return ret;
}

void
uuid2str( const uuid_t *uuid, char *dest )
{
    if( uuid->type == SDP_UUID16 ) {
        sprintf(dest, "%04X", uuid->value.uuid16 );
    } else if( uuid->type == SDP_UUID32 ) {
        sprintf(dest, "%08X", uuid->value.uuid32 );
    } else if( uuid->type == SDP_UUID128 ) {
        uint32_t *data = (uint32_t*)(&uuid->value.uuid128);
        sprintf(dest, "%08X-%04X-%04X-%04X-%04X%08X",
                ntohl(data[0]),
                ntohl(data[1])>>16,
                (ntohl(data[1])<<16)>>16,
                ntohl(data[2])>>16,
                (ntohl(data[2])<<16)>>16,
                ntohl(data[3]));
    }
}

// =================== socket methods ==================== //

/* s.accept() method */

static PyObject *
sock_accept(PySocketSockObject *s)
{
    char addrbuf[256];
    int newfd;
    socklen_t addrlen;
    PyObject *sock = NULL;
    PyObject *addr = NULL;
    PyObject *res = NULL;
    int timeout;

    if (!getsockaddrlen(s, &addrlen))
        return NULL;
    memset(addrbuf, 0, addrlen);

    newfd = -1;

        Py_BEGIN_ALLOW_THREADS
        timeout = internal_select(s, 0);
        if (!timeout)
                newfd = accept(s->sock_fd, (struct sockaddr *) addrbuf, &addrlen);
        Py_END_ALLOW_THREADS

        if (timeout) {
                PyErr_SetString(socket_timeout, "timed out");
                return NULL;
        }

        if (newfd < 0)
                return s->errorhandler();

        /* Create the new object with unspecified family,
           to avoid calls to bind() etc. on it. */
        sock = (PyObject *) new_sockobject(newfd,
                                           s->sock_family,
                                           s->sock_type,
                                           s->sock_proto);

        if (sock == NULL) {
                close(newfd);
                goto finally;
        }
        addr = makesockaddr(s, (struct sockaddr *)addrbuf, addrlen);
        if (addr == NULL)
                goto finally;

        res = Py_BuildValue("OO", sock, addr);

finally:
        Py_XDECREF(sock);
        Py_XDECREF(addr);
        return res;
}

PyDoc_STRVAR(accept_doc,
"accept() -> (socket object, address info)\n\
\n\
Wait for an incoming connection.  Return a new socket representing the\n\
connection, and the address of the client.  For L2CAP sockets, the address\n\
is a (host, psm) tuple.  For RFCOMM sockets, the address is a (host, channel)\n\
tuple.  For SCO sockets, the address is just a host.");

/* s.setblocking(flag) method.  Argument:
   False -- non-blocking mode; same as settimeout(0)
   True -- blocking mode; same as settimeout(None)
*/

static PyObject *
sock_setblocking(PySocketSockObject *s, PyObject *arg)
{
        int block;

        block = PyLong_AsLong(arg);
        if (block == -1 && PyErr_Occurred())
                return NULL;

        s->sock_timeout = block ? -1.0 : 0.0;
        internal_setblocking(s, block);

        Py_RETURN_NONE;
}

PyDoc_STRVAR(setblocking_doc,
"setblocking(flag)\n\
\n\
Set the socket to blocking (flag is true) or non-blocking (false).\n\
setblocking(True) is equivalent to settimeout(None);\n\
setblocking(False) is equivalent to settimeout(0.0).");

/* s.settimeout(timeout) method.  Argument:
   None -- no timeout, blocking mode; same as setblocking(True)
   0.0  -- non-blocking mode; same as setblocking(False)
   > 0  -- timeout mode; operations time out after timeout seconds
   < 0  -- illegal; raises an exception
*/
static PyObject *
sock_settimeout(PySocketSockObject *s, PyObject *arg)
{
        double timeout;

        if (Py_IsNone(arg))
                timeout = -1.0;
        else {
                timeout = PyFloat_AsDouble(arg);
                if (timeout < 0.0) {
                        if (!PyErr_Occurred())
                                PyErr_SetString(PyExc_ValueError, "Timeout value out of range");
                        return NULL;
                }
        }

        s->sock_timeout = timeout;
        internal_setblocking(s, timeout < 0.0);

        Py_RETURN_NONE;
}

PyDoc_STRVAR(settimeout_doc,
"settimeout(timeout)\n\
\n\
Set a timeout on socket operations.  'timeout' can be a float,\n\
giving in seconds, or None.  Setting a timeout of None disables\n\
the timeout feature and is equivalent to setblocking(1).\n\
Setting a timeout of zero is the same as setblocking(0).");

/* s.gettimeout() method.
   Returns the timeout associated with a socket. */
static PyObject *
sock_gettimeout(PySocketSockObject *s)
{
    if (s->sock_timeout < 0.0)
        Py_RETURN_NONE;
    else
        return PyFloat_FromDouble(s->sock_timeout);
}

PyDoc_STRVAR(gettimeout_doc,
"gettimeout() -> timeout\n\
\n\
Returns the timeout in floating seconds associated with socket \n\
operations. A timeout of None indicates that timeouts on socket \n\
operations are disabled.");

/* s.setsockopt() method.
   With an integer third argument, sets an integer option.
   With a string third argument, sets an option from a buffer;
   use optional built-in module 'struct' to encode the string. */

static PyObject *
sock_setsockopt(PySocketSockObject *s, PyObject *args)
{
    int level;
    int optname;
    int res;
    void *buf;
    Py_ssize_t buflen;
    int flag;

    if (PyArg_ParseTuple(args, "iii:setsockopt", &level, &optname, &flag)) {
        buf = (void *) &flag;
        buflen = sizeof flag;
    } else {
        PyErr_Clear();
        if (!PyArg_ParseTuple(args, "iis#:setsockopt",
                &level, &optname, &buf, &buflen)) {
            return NULL;
        }
    }
    res = setsockopt(s->sock_fd, level, optname, buf, buflen);
    if (res < 0)
        return s->errorhandler();
    Py_RETURN_NONE;
}

PyDoc_STRVAR(setsockopt_doc,
"setsockopt(level, option, value)\n\
\n\
Set a socket option.  See the Unix manual for level and option.\n\
The value argument can either be an integer or a string.");


/* s.getsockopt() method.
   With two arguments, retrieves an integer option.
   With a third integer argument, retrieves a string buffer of that size;
   use optional built-in module 'struct' to decode the string. */

static PyObject *
sock_getsockopt(PySocketSockObject *s, PyObject *args)
{
        int level;
        int optname;
        int res;
        socklen_t buflen = 0;
        if (!PyArg_ParseTuple(args, "ii|i:getsockopt", &level, &optname, &buflen))
                return NULL;

        if (buflen == 0) {
                int flag = 0;
                socklen_t flagsize = sizeof flag;
                res = getsockopt(s->sock_fd, level, optname, (void *)&flag, &flagsize);
                if (res < 0)
                        return s->errorhandler();
                return PyLong_FromLong(flag);
    } else if (buflen <= 0 || buflen > 1024) {
        PyErr_SetString(bluetooth_error, "getsockopt buflen out of range");
        return NULL;
    } else {
        PyObject *buf = PyBytes_FromStringAndSize((char *)NULL, buflen);
        if (buf == NULL)
            return NULL;
        res = getsockopt(s->sock_fd, level, optname,
                 (void *)PyBytes_AS_STRING(buf), &buflen);
        if (res < 0) {
            Py_DECREF(buf);
            return s->errorhandler();
        }
        _PyBytes_Resize(&buf, buflen);
        return buf;
        }
    return NULL;
}

PyDoc_STRVAR(getsockopt_doc,
"getsockopt(level, option[, buffersize]) -> value\n\
\n\
Get a socket option.  See the Unix manual for level and option.\n\
If a nonzero buffersize argument is given, the return value is a\n\
string of that length; otherwise it is an integer.");

static PyObject *
sock_setl2capsecurity(PySocketSockObject *s, PyObject *args)
{
    int level;
    struct bt_security sec;

    if (! PyArg_ParseTuple(args, "i:setsockopt", &level))
        return NULL;

    memset(&sec, 0, sizeof(sec));
    sec.level = level;

    if (setsockopt(s->sock_fd, SOL_BLUETOOTH, BT_SECURITY, &sec,
                                                    sizeof(sec)) == 0)
        Py_RETURN_NONE;

    if (errno != ENOPROTOOPT)
        return s->errorhandler();

    int lm_map[] = {
            0,
            L2CAP_LM_AUTH,
            L2CAP_LM_AUTH | L2CAP_LM_ENCRYPT,
            L2CAP_LM_AUTH | L2CAP_LM_ENCRYPT | L2CAP_LM_SECURE,
    }, opt = lm_map[level];

    if (setsockopt(s->sock_fd, SOL_L2CAP, L2CAP_LM, &opt, sizeof(opt)) < 0)
        return s->errorhandler();

    Py_RETURN_NONE;
}

PyDoc_STRVAR(setl2capsecurity_doc,
"setl2capsecurity(BT_SECURITY_*) -> value\n\
\n\
Sets socket security. Levels are BT_SECURITY_SDP, LOW, MEDIUM and HIGH.");

/* s.bind(sockaddr) method */

static PyObject *
sock_bind(PySocketSockObject *s, PyObject *addro)
{
        struct sockaddr addr = { 0 };
        int addrlen;
        int res;

        if (!getsockaddrarg(s, addro, &addr, &addrlen))
                return NULL;

        Py_BEGIN_ALLOW_THREADS
        res = bind(s->sock_fd, &addr, addrlen);
        Py_END_ALLOW_THREADS
        if (res < 0)
                return s->errorhandler();
        Py_RETURN_NONE;
}

PyDoc_STRVAR(bind_doc,
"bind(address)\n\
\n\
Bind the socket to a local address.  address must always be a tuple.\n\
  HCI sockets:    ( device number, channel )\n\
                  device number should be 0, 1, 2, etc.\n\
  L2CAP sockets:  ( host, psm )\n\
                  host should be an address e.g. \"01:23:45:67:89:ab\"\n\
                  psm should be an unsigned integer\n\
  RFCOMM sockets: ( host, channel )\n\
  SCO sockets:    ( host )");

/* s.close() method.
   Set the file descriptor to -1 so operations tried subsequently
   will surely fail. */

static PyObject *
sock_close(PySocketSockObject *s)
{
        int fd;

        if ((fd = s->sock_fd) != -1) {
                s->sock_fd = -1;
                Py_BEGIN_ALLOW_THREADS
                (void) close(fd);
                Py_END_ALLOW_THREADS
        }

    if( s->sdp_session ) {
        sdp_close( s->sdp_session );
        s->sdp_record_handle = 0;
        s->sdp_session = NULL;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(close_doc,
"close()\n\
\n\
Close the socket.  It cannot be used after this call.");

static int
internal_connect(PySocketSockObject *s, struct sockaddr *addr, int addrlen,
                 int *timeoutp)
{
        int res, timeout;

        timeout = 0;
        res = connect(s->sock_fd, addr, addrlen);

        if (s->sock_timeout > 0.0) {
                if (res < 0 && errno == EINPROGRESS) {
                        timeout = internal_select(s, 1);
                        res = connect(s->sock_fd, addr, addrlen);
                        if (res < 0 && errno == EISCONN)
                                res = 0;
                }
        }

        if (res < 0)
                res = errno;

        *timeoutp = timeout;

        return res;
}

/* s.connect(sockaddr) method */

static PyObject *
sock_connect(PySocketSockObject *s, PyObject *addro)
{
        struct sockaddr addr = { 0 };
        int addrlen;
        int res;
        int timeout;

        if (!getsockaddrarg(s, addro, &addr, &addrlen))
                return NULL;

        Py_BEGIN_ALLOW_THREADS
        res = internal_connect(s, &addr, addrlen, &timeout);
        Py_END_ALLOW_THREADS

        if (timeout) {
                PyErr_SetString(socket_timeout, "timed out");
                return NULL;
        }
        if (res != 0)
                return s->errorhandler();
        Py_RETURN_NONE;
}

PyDoc_STRVAR(connect_doc,
"connect(address)\n\
\n\
Connect the socket to a remote address. For L2CAP sockets, the address is a \n\
(host,psm) tuple.  For RFCOMM sockets, the address is a (host,channel) tuple.\n\
For SCO sockets, the address is just the host.");


/* s.connect_ex(sockaddr) method */

static PyObject *
sock_connect_ex(PySocketSockObject *s, PyObject *addro)
{
        struct sockaddr addr = { 0 };
        int addrlen;
        int res;
        int timeout;

        if (!getsockaddrarg(s, addro, &addr, &addrlen))
                return NULL;

        Py_BEGIN_ALLOW_THREADS
        res = internal_connect(s, &addr, addrlen, &timeout);
        Py_END_ALLOW_THREADS

        return PyLong_FromLong((long) res);
}

PyDoc_STRVAR(connect_ex_doc,
"connect_ex(address) -> errno\n\
\n\
This is like connect(address), but returns an error code (the errno value)\n\
instead of raising an exception when an error occurs.");


/* s.fileno() method */

static PyObject *
sock_fileno(PySocketSockObject *s)
{
        return PyLong_FromLong((long) s->sock_fd);
}

PyDoc_STRVAR(fileno_doc,
"fileno() -> integer\n\
\n\
Return the integer file descriptor of the socket.");


#ifndef NO_DUP
/* s.dup() method */

static PyObject *
sock_dup(PySocketSockObject *s)
{
        int newfd;
        PyObject *sock;

        newfd = dup(s->sock_fd);
        if (newfd < 0)
                return s->errorhandler();
        sock = (PyObject *) new_sockobject(newfd,
                                           s->sock_family,
                                           s->sock_type,
                                           s->sock_proto);
        if (sock == NULL)
                close(newfd);
        return sock;
}

PyDoc_STRVAR(dup_doc,
"dup() -> socket object\n\
\n\
Return a new socket object connected to the same system resource.");

#endif


/* s.getsockname() method */

static PyObject *
sock_getsockname(PySocketSockObject *s)
{
        char addrbuf[256];
        int res;
        socklen_t addrlen;

        if (!getsockaddrlen(s, &addrlen))
                return NULL;
        memset(addrbuf, 0, addrlen);
        Py_BEGIN_ALLOW_THREADS
        res = getsockname(s->sock_fd, (struct sockaddr *) addrbuf, &addrlen);
        Py_END_ALLOW_THREADS
        if (res < 0)
                return s->errorhandler();
        return makesockaddr(s, (struct sockaddr *) addrbuf, addrlen);
}

PyDoc_STRVAR(getsockname_doc,
"getsockname() -> address info\n\
\n\
Return the address of the local endpoint.");


/* s.getpeername() method */

static PyObject *
sock_getpeername(PySocketSockObject *s)
{
        char addrbuf[256];
        int res;
        socklen_t addrlen;

        if (!getsockaddrlen(s, &addrlen))
                return NULL;
        memset(addrbuf, 0, addrlen);
        Py_BEGIN_ALLOW_THREADS
        res = getpeername(s->sock_fd, (struct sockaddr *) addrbuf, &addrlen);
        Py_END_ALLOW_THREADS
        if (res < 0)
                return s->errorhandler();
        return makesockaddr(s, (struct sockaddr *) addrbuf, addrlen);
}

PyDoc_STRVAR(getpeername_doc,
"getpeername() -> address info\n\
\n\
Return the address of the remote endpoint.  For HCI sockets, the address is a\n\
device number (0, 1, 2, etc).  For L2CAP sockets, the address is a \n\
(host,psm) tuple.  For RFCOMM sockets, the address is a (host,channel) tuple.\n\
For SCO sockets, the address is just the host.");


/* s.listen(n) method */

static PyObject *
sock_listen(PySocketSockObject *s, PyObject *arg)
{
        int backlog;
        int res;

        backlog = PyLong_AsLong(arg);
        if (backlog == -1 && PyErr_Occurred())
                return NULL;
        Py_BEGIN_ALLOW_THREADS
        if (backlog < 1)
                backlog = 1;
        res = listen(s->sock_fd, backlog);
        Py_END_ALLOW_THREADS
        if (res < 0)
                return s->errorhandler();

        s->is_listening_socket = 1;
        Py_RETURN_NONE;
}

PyDoc_STRVAR(listen_doc,
"listen(backlog)\n\
\n\
Enable a server to accept connections.  The backlog argument must be at\n\
least 1; it specifies the number of unaccepted connection that the system\n\
will allow before refusing new connections.");


#ifndef NO_DUP
/* s.makefile(mode) method.
   Create a new open file object referring to a dupped version of
   the socket's file descriptor.  (The dup() call is necessary so
   that the open file and socket objects may be closed independent
   of each other.)
   The mode argument specifies 'r' or 'w' passed to fdopen(). */

static PyObject *
sock_makefile(PySocketSockObject *s, PyObject *args)
{
        extern int fclose(FILE *);
        char *mode = "r";
        int bufsize = -1;
        int fd;
        FILE *fp;
        PyObject *f;

        if (!PyArg_ParseTuple(args, "|si:makefile", &mode, &bufsize))
                return NULL;
        if ((fd = dup(s->sock_fd)) < 0 || (fp = fdopen(fd, mode)) == NULL)
        {
                if (fd >= 0)
                        close(fd);
                return s->errorhandler();
        }
        f = PyFile_FromFd(fd, "<socket>", mode, bufsize, NULL, NULL, NULL, 1);
        return f;
}

PyDoc_STRVAR(makefile_doc,
"makefile([mode[, buffersize]]) -> file object\n\
\n\
Return a regular file object corresponding to the socket.\n\
The mode and buffersize arguments are as for the built-in open() function.");

#endif /* NO_DUP */


/* s.recv(nbytes [,flags]) method */

static PyObject *
sock_recv(PySocketSockObject *s, PyObject *args)
{
        int len, n = 0, flags = 0, timeout;
        PyObject *buf;

        if (!PyArg_ParseTuple(args, "i|i:recv", &len, &flags))
                return NULL;

        if (len < 0) {
                PyErr_SetString(PyExc_ValueError, "negative buffersize in recv");
                return NULL;
        }

        buf = PyBytes_FromStringAndSize((char *) 0, len);
        if (buf == NULL)
                return NULL;

        Py_BEGIN_ALLOW_THREADS
        timeout = internal_select(s, 0);
        if (!timeout)
                n = recv(s->sock_fd, PyBytes_AS_STRING(buf), len, flags);
        Py_END_ALLOW_THREADS

        if (timeout) {
                Py_DECREF(buf);
                PyErr_SetString(socket_timeout, "timed out");
                return NULL;
        }
        if (n < 0) {
                Py_DECREF(buf);
                return s->errorhandler();
        }
        if (n != len)
                _PyBytes_Resize(&buf, n);
        return buf;
}

PyDoc_STRVAR(recv_doc,
"recv(buffersize[, flags]) -> data\n\
\n\
Receive up to buffersize bytes from the socket.  For the optional flags\n\
argument, see the Unix manual.  When no data is available, block until\n\
at least one byte is available or until the remote end is closed.  When\n\
the remote end is closed and all data is read, return the empty string.");


/* s.recvfrom(nbytes [,flags]) method */

static PyObject *
sock_recvfrom(PySocketSockObject *s, PyObject *args)
{
        char addrbuf[256];
        PyObject *buf = NULL;
        PyObject *addr = NULL;
        PyObject *ret = NULL;
        int len, n = 0, flags = 0, timeout;
        socklen_t addrlen;

        if (!PyArg_ParseTuple(args, "i|i:recvfrom", &len, &flags))
                return NULL;

        if (!getsockaddrlen(s, &addrlen))
                return NULL;
        buf = PyUnicode_FromStringAndSize((char *) 0, len);
        if (buf == NULL)
                return NULL;

        Py_BEGIN_ALLOW_THREADS
        memset(addrbuf, 0, addrlen);
        timeout = internal_select(s, 0);
        if (!timeout)
                n = recvfrom(s->sock_fd, PyBytes_AS_STRING(buf), len, flags,
                             (void *)addrbuf, &addrlen
                        );
        Py_END_ALLOW_THREADS

        if (timeout) {
                Py_DECREF(buf);
                PyErr_SetString(socket_timeout, "timed out");
                return NULL;
        }
        if (n < 0) {
                Py_DECREF(buf);
                return s->errorhandler();
        }

        if (n != len && _PyBytes_Resize(&buf, n) < 0)
                return NULL;

        if (!(addr = makesockaddr(s, (struct sockaddr *)addrbuf, addrlen)))
                goto finally;

        ret = Py_BuildValue("OO", buf, addr);

finally:
        Py_XDECREF(addr);
        Py_XDECREF(buf);
        return ret;
}

PyDoc_STRVAR(recvfrom_doc,
"recvfrom(buffersize[, flags]) -> (data, address info)\n\
\n\
Like recv(buffersize, flags) but also return the sender's address info.");

/* s.send(data [,flags]) method */

static PyObject *
sock_send(PySocketSockObject *s, PyObject *args)
{
        Py_buffer buf;
        int n = 0, flags = 0, timeout;

        if (!PyArg_ParseTuple(args, "s*|i:send", &buf, &flags))
                return NULL;

        Py_BEGIN_ALLOW_THREADS
        timeout = internal_select(s, 1);
        if (!timeout)
                n = send(s->sock_fd, buf.buf, buf.len, flags);
        Py_END_ALLOW_THREADS
        PyBuffer_Release(&buf);

        if (timeout) {
                PyErr_SetString(socket_timeout, "timed out");
                return NULL;
        }
        if (n < 0)
                return s->errorhandler();
        return PyLong_FromLong((long)n);
}

PyDoc_STRVAR(send_doc,
"send(data[, flags]) -> count\n\
\n\
Send a data string to the socket.  For the optional flags\n\
argument, see the Unix manual.  Return the number of bytes\n\
sent; this may be less than len(data) if the network is busy.");


/* s.sendall(data [,flags]) method */

static PyObject *
sock_sendall(PySocketSockObject *s, PyObject *args)
{
        Py_buffer buf;
        char *raw_buf;
        int len, n = 0, flags = 0, timeout;

        if (!PyArg_ParseTuple(args, "s*|i:sendall", &buf, &flags))
                return NULL;

        Py_BEGIN_ALLOW_THREADS
        raw_buf = buf.buf;
        len = buf.len;
        do {
                timeout = internal_select(s, 1);
                if (timeout)
                        break;
                n = send(s->sock_fd, raw_buf, len, flags);
                if (n < 0)
                        break;
                raw_buf += n;
                len -= n;
        } while (len > 0);
        raw_buf = NULL;
        Py_END_ALLOW_THREADS
        PyBuffer_Release(&buf);

        if (timeout) {
                PyErr_SetString(socket_timeout, "timed out");
                return NULL;
        }
        if (n < 0)
                return s->errorhandler();

        Py_RETURN_NONE;
}

PyDoc_STRVAR(sendall_doc,
"sendall(data[, flags])\n\
\n\
Send a data string to the socket.  For the optional flags\n\
argument, see the Unix manual.  This calls send() repeatedly\n\
until all data is sent.  If an error occurs, it's impossible\n\
to tell how much data has been sent.");


/* s.sendto(data, [flags,] sockaddr) method */

static PyObject *
sock_sendto(PySocketSockObject *s, PyObject *args)
{
        PyObject *addro;
        Py_buffer buf;
        struct sockaddr addr = { 0 };
        int addrlen, n = 0, flags, timeout;

        flags = 0;
        if (!PyArg_ParseTuple(args, "s*O:sendto", &buf, &addro)) {
                PyErr_Clear();
                if (!PyArg_ParseTuple(args, "s*iO:sendto", &buf, &flags, &addro))
                        return NULL;
        }

        if (!getsockaddrarg(s, addro, &addr, &addrlen))
                return NULL;

        Py_BEGIN_ALLOW_THREADS
        timeout = internal_select(s, 1);
        if (!timeout)
                n = sendto(s->sock_fd, buf.buf, buf.len, flags, &addr, addrlen);
        Py_END_ALLOW_THREADS
        PyBuffer_Release(&buf);

        if (timeout) {
                PyErr_SetString(socket_timeout, "timed out");
                return NULL;
        }
        if (n < 0)
                return s->errorhandler();
        return PyLong_FromLong((long)n);
}

PyDoc_STRVAR(sendto_doc,
"sendto(data[, flags], address) -> count\n\
\n\
Like send(data, flags) but allows specifying the destination address.\n\
For IP sockets, the address is a pair (hostaddr, port).");


/* s.shutdown(how) method */

static PyObject *
sock_shutdown(PySocketSockObject *s, PyObject *arg)
{
        int how;
        int res;

        how = PyLong_AsLong(arg);
        if (how == -1 && PyErr_Occurred())
                return NULL;
        Py_BEGIN_ALLOW_THREADS
        res = shutdown(s->sock_fd, how);
        Py_END_ALLOW_THREADS
        if (res < 0)
                return s->errorhandler();
        Py_RETURN_NONE;
}

PyDoc_STRVAR(shutdown_doc,
"shutdown(flag)\n\
\n\
Shut down the reading side of the socket (flag == 0), the writing side\n\
of the socket (flag == 1), or both ends (flag == 2).");

/* s.getsockid() method */

static PyObject *
sock_getsockid(PySocketSockObject *s, PyObject *arg)
{
    int dd;
    dd = s->sock_fd;
    return Py_BuildValue("i", dd);
}


/* List of methods for socket objects */

static PyMethodDef sock_methods[] = {
        {"accept",        (PyCFunction)sock_accept, METH_NOARGS,
                        accept_doc},
        {"bind",        (PyCFunction)sock_bind, METH_O,
                        bind_doc},
        {"close",        (PyCFunction)sock_close, METH_NOARGS,
                        close_doc},
        {"connect",        (PyCFunction)sock_connect, METH_O,
                        connect_doc},
        {"connect_ex",        (PyCFunction)sock_connect_ex, METH_O,
                        connect_ex_doc},
#ifndef NO_DUP
        {"dup",                (PyCFunction)sock_dup, METH_NOARGS,
                        dup_doc},
#endif
        {"fileno",        (PyCFunction)sock_fileno, METH_NOARGS,
                        fileno_doc},
        {"getpeername",        (PyCFunction)sock_getpeername,
                        METH_NOARGS, getpeername_doc},
    {"getsockid", (PyCFunction)sock_getsockid,
            METH_NOARGS, "Gets socket id."},
        {"getsockname",        (PyCFunction)sock_getsockname,
                        METH_NOARGS, getsockname_doc},
        {"getsockopt",        (PyCFunction)sock_getsockopt, METH_VARARGS,
                        getsockopt_doc},
        {"listen",        (PyCFunction)sock_listen, METH_O,
                        listen_doc},
#ifndef NO_DUP
        {"makefile",        (PyCFunction)sock_makefile, METH_VARARGS,
                        makefile_doc},
#endif
        {"recv",        (PyCFunction)sock_recv, METH_VARARGS,
                        recv_doc},
        {"recvfrom",        (PyCFunction)sock_recvfrom, METH_VARARGS,
                        recvfrom_doc},
        {"send",        (PyCFunction)sock_send, METH_VARARGS,
                        send_doc},
        {"sendall",        (PyCFunction)sock_sendall, METH_VARARGS,
                        sendall_doc},
        {"sendto",        (PyCFunction)sock_sendto, METH_VARARGS,
                        sendto_doc},
        {"setblocking",        (PyCFunction)sock_setblocking, METH_O,
                        setblocking_doc},
        {"settimeout", (PyCFunction)sock_settimeout, METH_O,
                        settimeout_doc},
        {"gettimeout", (PyCFunction)sock_gettimeout, METH_NOARGS,
                        gettimeout_doc},
        {"setsockopt",        (PyCFunction)sock_setsockopt, METH_VARARGS,
                        setsockopt_doc},
        {"setl2capsecurity",        (PyCFunction)sock_setl2capsecurity, METH_VARARGS,
                        setl2capsecurity_doc},
        {"shutdown",        (PyCFunction)sock_shutdown, METH_O,
                        shutdown_doc},
        {NULL,                        NULL}                /* sentinel */
};

/* SockObject members */
static PyMemberDef sock_memberlist[] = {
    {"family", T_INT, offsetof(PySocketSockObject, sock_family), READONLY, "the socket family"},
    {"type", T_INT, offsetof(PySocketSockObject, sock_type), READONLY, "the socket type"},
    {"proto", T_INT, offsetof(PySocketSockObject, sock_proto), READONLY, "the socket protocol"},
    {NULL} /* sentinel */
};

static PyGetSetDef sock_getsetlist[] = {
    {"timeout", (getter)sock_gettimeout, NULL, PyDoc_STR("the socket timeout")},
    {NULL} /* sentinel */
};

/* Deallocate a socket object in response to the last Py_DECREF().
   First close the file description. */

static void
sock_dealloc(PySocketSockObject *s)
{
    // close the OS file descriptor
        if (s->sock_fd != -1) {
        Py_BEGIN_ALLOW_THREADS
                close(s->sock_fd);
        Py_END_ALLOW_THREADS
    }

    if( s->sdp_session ) {
        sdp_close( s->sdp_session );
        s->sdp_record_handle = 0;
        s->sdp_session = NULL;
    }

        Py_TYPE(s)->tp_free((PyObject *)s);
}


static PyObject *
sock_repr(PySocketSockObject *s)
{
        char buf[512];
#if SIZEOF_SOCKET_T > SIZEOF_LONG
        if (s->sock_fd > LONG_MAX) {
                /* this can occur on Win64, and actually there is a special
                   ugly printf formatter for decimal pointer length integer
                   printing, only bother if necessary*/
                PyErr_SetString(PyExc_OverflowError,
                                "no printf formatter to display "
                                "the socket descriptor in decimal");
                return NULL;
        }
#endif
        PyOS_snprintf(
                buf, sizeof(buf),
                "<socket object, fd=%ld, family=%d, type=%d, protocol=%d>",
                (long)s->sock_fd, s->sock_family,
                s->sock_type,
                s->sock_proto);
        return PyUnicode_FromString(buf);
}


/* Create a new, uninitialized socket object. */

static PyObject *
sock_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
        PyObject *new;

        new = type->tp_alloc(type, 0);
        if (new != NULL) {
                ((PySocketSockObject *)new)->sock_fd = -1;
                ((PySocketSockObject *)new)->sock_timeout = -1.0;
                ((PySocketSockObject *)new)->errorhandler = &set_error;
        }
        return new;
}


/* Initialize a new socket object. */

/*ARGSUSED*/
static int
sock_initobj(PyObject *self, PyObject *args, PyObject *kwds)
{
        PySocketSockObject *s = (PySocketSockObject *)self;
        int fd;
        int family = AF_BLUETOOTH, type = SOCK_STREAM, proto = BTPROTO_RFCOMM;
        static char *keywords[] = {"proto", 0};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i:socket", keywords, &proto))
        return -1;

    switch(proto) {
        case BTPROTO_HCI:
            type = SOCK_RAW;
            break;
        case BTPROTO_L2CAP:
            type = SOCK_SEQPACKET;
            break;
        case BTPROTO_RFCOMM:
            type = SOCK_STREAM;
            break;
        case BTPROTO_SCO:
            type = SOCK_SEQPACKET;
            break;
    }

        Py_BEGIN_ALLOW_THREADS
        fd = socket(family, type, proto);
        Py_END_ALLOW_THREADS

        if (fd < 0)
        {
                set_error();
                return -1;
        }
        init_sockobject(s, fd, family, type, proto);
        /* From now on, ignore SIGPIPE and let the error checking
           do the work. */
#ifdef SIGPIPE
        (void) signal(SIGPIPE, SIG_IGN);
#endif

        return 0;
}


/* Type object for socket objects. */

PyTypeObject sock_type = {
    PyVarObject_HEAD_INIT(NULL, 0)   /* Must fill in type value later */
        "_bluetooth.btsocket",                        /* tp_name */
        sizeof(PySocketSockObject),                /* tp_basicsize */
        0,                                        /* tp_itemsize */
        (destructor)sock_dealloc,                /* tp_dealloc */
        0,                                        /* tp_print */
        0,                                        /* tp_getattr */
        0,                                        /* tp_setattr */
        0,                                        /* tp_compare */
        (reprfunc)sock_repr,/* tp_repr */
        0,                                        /* tp_as_number */
        0,                                        /* tp_as_sequence */
        0,                                        /* tp_as_mapping */
        0,                                        /* tp_hash */
        0,                                        /* tp_call */
        0,                                        /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                        /* tp_setattro */
        0,                                        /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
        sock_doc,                                /* tp_doc */
        0,                                        /* tp_traverse */
        0,                                        /* tp_clear */
        0,                                        /* tp_richcompare */
        0,                                        /* tp_weaklistoffset */
        0,                                        /* tp_iter */
        0,                                        /* tp_iternext */
        sock_methods,                                /* tp_methods */
        sock_memberlist,                        /* tp_members */
        sock_getsetlist,                        /* tp_getset */
        0,                                        /* tp_base */
        0,                                        /* tp_dict */
        0,                                        /* tp_descr_get */
        0,                                        /* tp_descr_set */
        0,                                        /* tp_dictoffset */
        sock_initobj,                /* tp_init */
        PyType_GenericAlloc,/* tp_alloc */
        sock_new,                        /* tp_new */
        PyObject_Del,                /* tp_free */
};


#ifndef NO_DUP
/* Create a socket object from a numeric file description.
   Useful e.g. if stdin is a socket.
   Additional arguments as for socket(). */

/*ARGSUSED*/
static PyObject *
bt_fromfd(PyObject *self, PyObject *args)
{
        PySocketSockObject *s;
        int fd;
        int family, type, proto = 0;
        if (!PyArg_ParseTuple(args, "iii|i:fromfd", &fd, &family, &type, &proto))
                return NULL;
        /* Dup the fd so it and the socket can be closed independently */
        fd = dup(fd);
        if (fd < 0)
                return set_error();
        s = new_sockobject(fd, family, type, proto);
        /* From now on, ignore SIGPIPE and let the error checking do the work. */
#ifdef SIGPIPE
        (void) signal(SIGPIPE, SIG_IGN);
#endif
        return (PyObject *) s;
}

PyDoc_STRVAR(bt_fromfd_doc,
"fromfd(fd, family, type[, proto]) -> socket object\n\
\n\
Create a socket object from the given file descriptor.\n\
The remaining arguments are the same as for socket().");

#endif /* NO_DUP */


static PyObject *
bt_btohs(PyObject *self, PyObject *args)
{
        int x1, x2;

        if (!PyArg_ParseTuple(args, "i:btohs", &x1)) {
                return NULL;
        }
        x2 = (int)btohs((short)x1);
        return PyLong_FromLong(x2);
}

PyDoc_STRVAR(bt_btohs_doc,
"btohs(integer) -> integer\n\
\n\
Convert a 16-bit integer from bluetooth to host byte order.");


static PyObject *
bt_btohl(PyObject *self, PyObject *args)
{
        unsigned long x;
        PyObject *arg;

        if (!PyArg_ParseTuple(args, "O:btohl", &arg)) {
                return NULL;
        }

        if (PyLong_Check(arg)) {
                x = PyLong_AS_LONG(arg);
                if (x == (unsigned long) -1 && PyErr_Occurred())
                        return NULL;
        }
        else if (PyLong_Check(arg)) {
                x = PyLong_AsUnsignedLong(arg);
                if (x == (unsigned long) -1 && PyErr_Occurred())
                        return NULL;
#if SIZEOF_LONG > 4
                {
                        unsigned long y;
                        /* only want the trailing 32 bits */
                        y = x & 0xFFFFFFFFUL;
                        if (y ^ x)
                                return PyErr_Format(PyExc_OverflowError,
                                            "long int larger than 32 bits");
                        x = y;
                }
#endif
        }
        else
                return PyErr_Format(PyExc_TypeError,
                                    "expected int/long, %s found",
                                    Py_TYPE(arg)->tp_name);
        if (x == (unsigned long) -1 && PyErr_Occurred())
                return NULL;
        return PyLong_FromLong(btohl(x));
}

PyDoc_STRVAR(bt_btohl_doc,
"btohl(integer) -> integer\n\
\n\
Convert a 32-bit integer from bluetooth to host byte order.");


static PyObject *
bt_htobs(PyObject *self, PyObject *args)
{
        unsigned long x1, x2;

        if (!PyArg_ParseTuple(args, "i:htobs", &x1)) {
                return NULL;
        }
        x2 = (int)htobs((short)x1);
        return PyLong_FromLong(x2);
}

PyDoc_STRVAR(bt_htobs_doc,
"htobs(integer) -> integer\n\
\n\
Convert a 16-bit integer from host to bluetooth byte order.");


static PyObject *
bt_htobl(PyObject *self, PyObject *args)
{
        unsigned long x;
        PyObject *arg;

        if (!PyArg_ParseTuple(args, "O:htobl", &arg)) {
                return NULL;
        }

        if (PyLong_Check(arg)) {
                x = PyLong_AS_LONG(arg);
                if (x == (unsigned long) -1 && PyErr_Occurred())
                        return NULL;
        }
        else if (PyLong_Check(arg)) {
                x = PyLong_AsUnsignedLong(arg);
                if (x == (unsigned long) -1 && PyErr_Occurred())
                        return NULL;
#if SIZEOF_LONG > 4
                {
                        unsigned long y;
                        /* only want the trailing 32 bits */
                        y = x & 0xFFFFFFFFUL;
                        if (y ^ x)
                                return PyErr_Format(PyExc_OverflowError,
                                            "long int larger than 32 bits");
                        x = y;
                }
#endif
        }
        else
                return PyErr_Format(PyExc_TypeError,
                                    "expected int/long, %s found",
                                    Py_TYPE(arg)->tp_name);
        return PyLong_FromLong(htobl(x));
}

//static PyObject *
//bt_get_available_port_number( PyObject *self, PyObject *arg )
//{
//        int protocol = -1;
//    int s;
//
//        protocol = PyLong_AsLong(arg);
//
//        if (protocol == -1 && PyErr_Occurred())
//                return NULL;
//
//    switch(protocol) {
//        case BTPROTO_RFCOMM:
//            {
//                struct sockaddr_rc sockaddr = { 0 };
//                int s, psm;
//                s = socket( AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM );
//
//                sockaddr.rc_family = AF_BLUETOOTH;
//                bacppy( &sockaddr.rc_bdaddr, BDADDR_ANY
//            }
//            break;
//        case BTPROTO_L2CAP:
//            {
//    loc_addr.l2_family = AF_BLUETOOTH;
//    bacpy( &loc_addr.l2_bdaddr, BDADDR_ANY );
//    loc_addr.l2_psm = htobs(0x1001);
//
//            }
//            break;
//        default:
//            {
//                PyErr_SetString( PyExc_ValueError,
//                        "protocol must be either RFCOMM or L2CAP" );
//                return 0;
//            }
//            break;
//    }
//    Py_RETURN_NONE;
//}

PyDoc_STRVAR(bt_htobl_doc,
"htobl(integer) -> integer\n\
\n\
Convert a 32-bit integer from host to bluetooth byte order.");

/* Python API to getting and setting the default timeout value. */

static PyObject *
bt_getdefaulttimeout(PyObject *self)
{
    if (defaulttimeout < 0.0)
        Py_RETURN_NONE;
    else
        return PyFloat_FromDouble(defaulttimeout);
}

PyDoc_STRVAR(bt_getdefaulttimeout_doc,
"getdefaulttimeout() -> timeout\n\
\n\
Returns the default timeout in floating seconds for new socket objects.\n\
A value of None indicates that new socket objects have no timeout.\n\
When the socket module is first imported, the default is None.");

static PyObject *
bt_setdefaulttimeout(PyObject *self, PyObject *arg)
{
        double timeout;

        if (Py_IsNone(arg))
                timeout = -1.0;
        else {
                timeout = PyFloat_AsDouble(arg);
                if (timeout < 0.0) {
                        if (!PyErr_Occurred())
                                PyErr_SetString(PyExc_ValueError, "Timeout value out of range");
                        return NULL;
                }
        }

        defaulttimeout = timeout;

        Py_RETURN_NONE;
}

PyDoc_STRVAR(bt_setdefaulttimeout_doc,
"setdefaulttimeout(timeout)\n\
\n\
Set the default timeout in floating seconds for new socket objects.\n\
A value of None indicates that new socket objects have no timeout.\n\
When the socket module is first imported, the default is None.");

/*
 * ----------------------------------------------------------------------
 *  HCI Section   (Calvin)
 *
 *  This section provides the socket methods for calling HCI commands.
 *  These commands may be called statically, and implementation is
 *  independent from the rest of the module (except for bt_methods[]).
 *
 * ----------------------------------------------------------------------
 *
 */

/*
 * params:  (int) device number
 * effect: opens and binds a new HCI socket
 * return: a PySocketSockObject, or NULL on failure
 */
static PyObject *
bt_hci_open_dev(PyObject *self, PyObject *args)
{
    int dev = -1, fd;
        PySocketSockObject *s = NULL;

    if ( !PyArg_ParseTuple(args, "|i", &dev) )
    {
        return NULL;
    }

    // if the device was not specified, just use the first available bt device
    if (dev < 0) {
        dev = hci_get_route(NULL);
    }

    if (dev < 0) {
        PyErr_SetString(bluetooth_error, "no available bluetoot devices");
        return 0;
    }

    Py_BEGIN_ALLOW_THREADS
    fd = hci_open_dev(dev);
    Py_END_ALLOW_THREADS

        s = (PySocketSockObject *)PyType_GenericNew(&sock_type, NULL, NULL);
        if (s != NULL) init_sockobject(s, fd, AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);

        return (PyObject*)s;
}

PyDoc_STRVAR(bt_hci_open_dev_doc, "hci_open_dev");

/*
 * params: (int) device number
 * effect: closes an HCI socket
 */
static PyObject *
bt_hci_close_dev(PyObject *self, PyObject *args)
{
    int dev, err;

    if ( !PyArg_ParseTuple(args, "i", &dev) )
    {
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS
    err = hci_close_dev(dev);
    Py_END_ALLOW_THREADS

    if( err < 0 ) return set_error();

    Py_RETURN_NONE;
}

PyDoc_STRVAR(bt_hci_close_dev_doc,
"hci_close_dev(dev_id)\n\
\n\
Closes the specified device id.  Note:  device id is NOT a btoscket.\n\
You can also use btsocket.close() to close a specific socket.");

/*
 * params: (int) socket fd, (uint_16) ogf control bits
 *         (uint_16) ocf control bits, (struct) command params
 * effect: executes command described by the OGF and OCF bits
 *          (see bluetooth/hci.h)
 * return: (int) 0 on success, -1 on failure
 */
static PyObject *
bt_hci_send_cmd(PyObject *self, PyObject *args)
{
    PySocketSockObject *socko = NULL;
    int err;
    Py_ssize_t plen = 0;
    uint16_t ogf, ocf;
    char *param = NULL;
    int dd = 0;

    if ( !PyArg_ParseTuple(args, "OHH|s#", &socko, &ogf, &ocf, &param, &plen)) {
        return NULL;
    }

    dd = socko->sock_fd;

    Py_BEGIN_ALLOW_THREADS
    err = hci_send_cmd(dd, ogf, ocf, plen, (void*)param);
    Py_END_ALLOW_THREADS

    if( err ) return socko->errorhandler();

    return Py_BuildValue("i", err);
}

PyDoc_STRVAR(bt_hci_send_cmd_doc,
"hci_send_cmd(sock, ogf, ocf, params)\n\
\n\
Transmits the specified HCI command to the socket.\n\
    sock     - the btoscket object to use\n\
    ogf, pcf - see bluetooth specification\n\
    params   - packed command parameters (use the struct module to do this)");

static PyObject *
bt_hci_send_req(PyObject *self, PyObject *args, PyObject *kwds)
{
    PySocketSockObject *socko = NULL;
    int err;
    int to=0;
    char rparam[256];
    Py_ssize_t req_clen;
    struct hci_request req = { 0 };
    int dd = 0;

    static char *keywords[] = { "sock", "ogf", "ocf", "event", "rlen", "params",
                                "timeout", 0 };

    if( !PyArg_ParseTupleAndKeywords(args, kwds, "OHHii|s#i", keywords,
                &socko, &req.ogf, &req.ocf, &req.event, &req.rlen,
                &req.cparam, &req_clen, &to) )
        return 0;
    req.clen = req_clen;

    req.rparam = rparam;
    dd = socko->sock_fd;

    Py_BEGIN_ALLOW_THREADS
    err = hci_send_req( dd, &req, to );
    Py_END_ALLOW_THREADS

    if( err< 0 ) return socko->errorhandler();

    return PyUnicode_FromStringAndSize(rparam, req.rlen);
}
PyDoc_STRVAR(bt_hci_send_req_doc,
"hci_send_req(sock, ogf, ocf, event, rlen, params=None, timeout=0)\n\
\n\
Transmits a HCI cmomand to the socket and waits for the specified event.\n\
   sock      - the btsocket object\n\
   ogf, ocf  - see bluetooth specification\n\
   event     - the event to wait for.  Probably one of EVT_*\n\
   rlen      - the size of the returned packet to expect.  This must be\n\
               specified since bt won't know how much data to expect\n\
               otherwise\n\
    params   - the command parameters\n\
    timeout  - timeout, in milliseconds");


static PyObject*
bt_hci_inquiry(PyObject *self, PyObject *args, PyObject *kwds)
{
    int i, err;
    int dev_id = 0;
    int length = 8;
    int flush = 1;
    int flags = 0;
    int lookup_class = 0;
    int iac = 0x9e8b33;
    char ba_name[19];
    inquiry_info *info = NULL;
    PySocketSockObject *socko = NULL;
    struct hci_inquiry_req *ir;
    char buf[sizeof(*ir) + sizeof(inquiry_info) * 250];

    PyObject *rtn_list = (PyObject *)NULL;
    static char *keywords[] = {"sock", "duration", "flush_cache",
                                "lookup_class", "device_id", "iac", 0};

    if( !PyArg_ParseTupleAndKeywords(args, kwds, "O|iiiii", keywords,
                &socko, &length, &flush, &lookup_class, &dev_id, &iac) )
    {
        return 0;
    }

    flags |= (flush) ? IREQ_CACHE_FLUSH : 0;


    ir = (struct hci_inquiry_req*)buf;
    ir->dev_id  = dev_id;
    ir->num_rsp = 250;
    ir->length  = length;
    ir->flags   = flags;

    ir->lap[0] = iac & 0xff;
    ir->lap[1] = (iac >> 8) & 0xff;
    ir->lap[2] = (iac >> 16) & 0xff;

    Py_BEGIN_ALLOW_THREADS
    err = ioctl(socko->sock_fd, HCIINQUIRY, (unsigned long) buf);
    Py_END_ALLOW_THREADS

    if( err < 0 ) return socko->errorhandler();

    info = (inquiry_info*)(buf + sizeof(*ir));

    if( (rtn_list = PyList_New(0)) == NULL ) return 0;

    memset( ba_name, 0, sizeof(ba_name) );
    // fill in the list with the discovered bluetooth addresses
    for(i=0;i<ir->num_rsp;i++) {
        PyObject * addr_entry = (PyObject *)NULL;
        int err;

        ba2str( &(info+i)->bdaddr, ba_name );

        addr_entry = PyUnicode_FromString( ba_name );

        if (lookup_class) {
            PyObject *item_tuple = PyTuple_New(2);

            int dev_class = (info+i)->dev_class[2] << 16 |
                            (info+i)->dev_class[1] << 8 |
                            (info+i)->dev_class[0];
            PyObject *class_entry = PyLong_FromLong( dev_class );

            err = PyTuple_SetItem( item_tuple, 0, addr_entry );
            if (err) {
                Py_XDECREF( item_tuple );
                Py_XDECREF( rtn_list );
                return NULL;
            }

            err = PyTuple_SetItem( item_tuple, 1, class_entry );
            if (err) {
                Py_XDECREF( item_tuple );
                Py_XDECREF( rtn_list );
                return NULL;
            }

            err = PyList_Append( rtn_list, item_tuple );
            Py_DECREF( item_tuple );
            if (err) {
                Py_XDECREF( rtn_list );
                return NULL;
            }

        } else {
            err = PyList_Append( rtn_list, addr_entry );
            Py_DECREF( addr_entry );
            if (err) {
                Py_XDECREF( rtn_list );
                return NULL;
            }
        }
    }

    return rtn_list;
}

PyDoc_STRVAR(bt_hci_inquiry_doc,
"hci_inquiry(dev_id=0, duration=8, flush_cache=True\n\
\n\
Performs a device inquiry using the specified device (usually 0 or 1).\n\
The inquiry will last 1.28 * duration seconds.  If flush_cache is True, then\n\
previously discovered devices will not be returned in the inquiry.)");


static PyObject*
bt_hci_read_remote_name(PyObject *self, PyObject *args, PyObject *kwds)
{
    char *addr = NULL;
    bdaddr_t ba;
    int timeout = 5192;
    static char name[249];
    PySocketSockObject *socko = NULL;
    int err = 0;

        static char *keywords[] = {"dd", "bdaddr", "timeout", 0};

    if( !PyArg_ParseTupleAndKeywords(args, kwds, "Os|i", keywords,
                &socko, &addr, &timeout) )
    {
        return 0;
    }

    str2ba( addr, &ba );
    memset( name, 0, sizeof(name) );

    Py_BEGIN_ALLOW_THREADS
    err = hci_read_remote_name( socko->sock_fd, &ba, sizeof(name)-1,
                name, timeout );
    Py_END_ALLOW_THREADS

    if( err < 0)
        return PyErr_SetFromErrno(bluetooth_error);

    return PyUnicode_FromString( name );
}
PyDoc_STRVAR(bt_hci_read_remote_name_doc,
"hci_read_remote_name(sock, bdaddr, timeout=5192)\n\
\n\
Performs a remote name request to the specified bluetooth device.\n\
   sock - the HCI socket object to use\n\
   bdaddr - the bluetooth address of the remote device\n\
   timeout - maximum amount of time, in milliseconds, to wait\n\
\n\
Returns the name of the device, or raises an error on failure");

static char *opcode2str(uint16_t opcode);

static PyObject*
bt_hci_opcode_name(PyObject *self, PyObject *args)
{
        int opcode;
        PyArg_ParseTuple(args,"i", &opcode);
        // DPRINTF("opcode = %x\n", opcode);
        const char* cmd_name = opcode2str (opcode);

    return PyUnicode_FromString( cmd_name );
}
PyDoc_STRVAR(bt_hci_opcode_name_doc,
"hci_opcode_name(opcode)\n\
\n\
Performs a remote name request to the specified bluetooth device.\n\
   sock - the HCI socket object to use\n\
   bdaddr - the bluetooth address of the remote device\n\
   timeout - maximum amount of time, in milliseconds, to wait\n\
\n\
Returns the name of the device, or raises an error on failure");

#define EVENT_NUM 77
static char *event_str[];

static PyObject*
bt_hci_event_name(PyObject *self, PyObject *args)
{
   int eventNum;
   PyArg_ParseTuple(args,"i", &eventNum);
   if ((eventNum > EVENT_NUM) || (eventNum < 0)) {
           PyErr_SetString(bluetooth_error,
                           "hci_event_name: invalid event number");
           return 0;
   }
   const char* event_name = event_str[eventNum];
   return PyUnicode_FromString( event_name );
}
PyDoc_STRVAR(bt_hci_event_name_doc,
"hci_event_name(eventNum)\n\
\n\
Returns a string withe description of the HCI event.\n\
   sock - the HCI socket object to use\n\
   eventNum - valid number\n\
\n\
Returns the name of the device, or raises an error on failure");

// lot of repetitive code... yay macros!!
#define DECL_HCI_FILTER_OP_1(name, docstring) \
static PyObject * bt_hci_filter_ ## name (PyObject *self, PyObject *args )\
{ \
    char *param; \
    Py_ssize_t len; \
    int arg; \
    if( !PyArg_ParseTuple(args,"s#i", &param, &len, &arg) ) \
        return 0; \
    if( len != sizeof(struct hci_filter) ) { \
        PyErr_SetString(PyExc_ValueError, "bad filter"); \
        return 0; \
    } \
    hci_filter_ ## name ( arg, (struct hci_filter*)param ); \
    return PyUnicode_FromStringAndSize(param, len); \
} \
PyDoc_STRVAR(bt_hci_filter_ ## name ## _doc, docstring);

DECL_HCI_FILTER_OP_1(set_ptype, "set ptype!")
DECL_HCI_FILTER_OP_1(clear_ptype, "clear ptype!")
DECL_HCI_FILTER_OP_1(test_ptype, "test ptype!")

DECL_HCI_FILTER_OP_1(set_event, "set event!")
DECL_HCI_FILTER_OP_1(clear_event, "clear event!")
DECL_HCI_FILTER_OP_1(test_event, "test event!")

DECL_HCI_FILTER_OP_1(set_opcode, "set opcode!")
DECL_HCI_FILTER_OP_1(test_opcode, "test opcode!")

#undef DECL_HCI_FILTER_OP_1

#define DECL_HCI_FILTER_OP_2(name, docstring) \
static PyObject * bt_hci_filter_ ## name (PyObject *self, PyObject *args )\
{ \
    char *param; \
    Py_ssize_t len; \
    if( !PyArg_ParseTuple(args,"s#", &param, &len) ) \
        return 0; \
    if( len != sizeof(struct hci_filter) ) { \
       PyErr_SetString(PyExc_ValueError, "bad filter"); \
        return 0; \
    } \
    hci_filter_ ## name ( (struct hci_filter*)param ); \
    return PyUnicode_FromStringAndSize(param, len); \
} \
PyDoc_STRVAR(bt_hci_filter_ ## name ## _doc, docstring);

DECL_HCI_FILTER_OP_2(all_events, "all events!");
DECL_HCI_FILTER_OP_2(clear, "clear filter");
DECL_HCI_FILTER_OP_2(all_ptypes, "all packet types!");
DECL_HCI_FILTER_OP_2(clear_opcode, "clear opcode!")

#undef DECL_HCI_FILTER_OP_2

static PyObject *
bt_cmd_opcode_pack(PyObject *self, PyObject *args )
{
    uint16_t opcode, ogf, ocf;
    if (!PyArg_ParseTuple(args, "HH", &ogf, &ocf )) return 0;
    opcode = cmd_opcode_pack(ogf, ocf);
    return Py_BuildValue("H", opcode);
}
PyDoc_STRVAR(bt_cmd_opcode_pack_doc,
"cmd_opcode_pack(ogf, ocf)\n\
\n\
Packs an OCF and an OGF value together to form a opcode");

static PyObject *
bt_cmd_opcode_ogf(PyObject *self, PyObject *args )
{
    uint16_t opcode;
    if (!PyArg_ParseTuple(args, "H", &opcode)) return 0;
    return Py_BuildValue("H", cmd_opcode_ogf(opcode));
}
PyDoc_STRVAR(bt_cmd_opcode_ogf_doc,
"cmd_opcode_ogf(opcode)\n\
\n\
Convenience function to extract and return the OGF value from an opcode");

static PyObject *
bt_cmd_opcode_ocf(PyObject *self, PyObject *args )
{
    uint16_t opcode;
    if (!PyArg_ParseTuple(args, "H", &opcode)) return 0;
    return Py_BuildValue("H", cmd_opcode_ocf(opcode));
}
PyDoc_STRVAR(bt_cmd_opcode_ocf_doc,
"cmd_opcode_ocf(opcode)\n\
\n\
Convenience function to extract and return the OCF value from an opcode");


static PyObject *
bt_ba2str(PyObject *self, PyObject *args)
{
    char *data=NULL;
    Py_ssize_t len=0;
    char ba_str[19] = {0};
    if (!PyArg_ParseTuple(args, "s#", &data, &len)) return 0;
    ba2str((bdaddr_t*)data, ba_str);
    return PyUnicode_FromString( ba_str );
//    return Py_BuildValue("s#", ba_str, 18);
}
PyDoc_STRVAR(bt_ba2str_doc,
"ba2str(data)\n\
\n\
Converts a packed bluetooth address to a human readable string");

static PyObject *
bt_str2ba(PyObject *self, PyObject *args)
{
    char *ba_str=NULL;
    bdaddr_t ba;
    if (!PyArg_ParseTuple(args, "s", &ba_str)) return 0;
    str2ba( ba_str, &ba );
    return Py_BuildValue("y#", (char*)(&ba), sizeof(ba));
}
PyDoc_STRVAR(bt_str2ba_doc,
"str2ba(string)\n\
\n\
Converts a bluetooth address string into a packed bluetooth address.\n\
The string should be of the form \"XX:XX:XX:XX:XX:XX\"");

/*
 * params:  (string) device address
 * effect: -
 * return: Device id
 */
static PyObject *
bt_hci_devid(PyObject *self, PyObject *args)
{
    char *devaddr=NULL;
    int devid;

    if ( !PyArg_ParseTuple(args, "|s", &devaddr) )
    {
        return NULL;
    }

        if (devaddr)
                devid=hci_devid(devaddr);

        else
                devid=hci_get_route(NULL);

    return Py_BuildValue("i",devid);
}
PyDoc_STRVAR( bt_hci_devid_doc,
"hci_devid(address)\n\
\n\
Get the device id for the local device with specified address.");

/*
 * params:  (string) device address
 * effect: -
 * return: Device id
 */
static PyObject *
bt_hci_role(PyObject *self, PyObject *args)
{
    int devid;
    int fd;
    int role;

    if ( !PyArg_ParseTuple(args, "ii", &fd, &devid) )
        return NULL;

    struct hci_dev_info di = {dev_id: devid};

    if (ioctl(fd, HCIGETDEVINFO, (void *) &di))
           return NULL;

    role = di.link_mode == HCI_LM_MASTER;

    return Py_BuildValue("i", role);
}
PyDoc_STRVAR( bt_hci_role_doc,
"hci_role(hci_fd, dev_id)\n\
\n\
Get the role (master or slave) of the device id.");

/*
 * params:  (string) device address
 * effect: -
 * return: Device id
 */
static PyObject *
bt_hci_read_clock(PyObject *self, PyObject *args)
{
    int fd;
    int handle;
    int which;
    int timeout;
    uint32_t btclock;
    uint16_t accuracy;
    int res;

    if ( !PyArg_ParseTuple(args, "iiii", &fd, &handle, &which, &timeout) )
        return NULL;

    res = hci_read_clock(fd, handle, which, &btclock, &accuracy, timeout);
    if (res)
        Py_RETURN_NONE;

    return Py_BuildValue("(ii)", btclock, accuracy);
}
PyDoc_STRVAR( bt_hci_read_clock_doc,
"hci_read_clock(hci_fd, acl_handle, which_clock, timeout_ms)\n\
\n\
Get the Bluetooth Clock (native or piconet).");

/*
 * params:  (string) device address
 * effect: -
 * return: Device id
 */
static PyObject *
bt_hci_get_route(PyObject *self, PyObject *args)
{
    int dev_id = 0;
    char *addr = NULL;
    bdaddr_t ba;
    if ( !PyArg_ParseTuple(args, "|s", &addr) ) {
        return NULL;
    }
    if(addr && strlen(addr)) {
        str2ba( addr, &ba );
        dev_id = hci_get_route(&ba);
    } else {
        dev_id = hci_get_route(NULL);
    }
    if (dev_id < 0) {
        return PyErr_SetFromErrno(PyExc_OSError);
    }
    return PyLong_FromLong(dev_id);
}
PyDoc_STRVAR( bt_hci_get_route_doc,
"hci_get_route(address)\n\
\n\
Get the device id through which remote specified addr can be reached.");

/*
 * params:  (string) device address
 * effect: -
 * return: Device id
 */
static PyObject *
bt_hci_acl_conn_handle(PyObject *self, PyObject *args)
{
    int fd;
    char *devaddr=NULL;
    bdaddr_t binaddr;
    struct hci_conn_info_req *cr;
    char buf[sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info)];
    int handle = -1;

    if ( !PyArg_ParseTuple(args, "is", &fd, &devaddr) )
        return NULL;

    if (devaddr)
            str2ba(devaddr, &binaddr);
    else
        str2ba("00:00:00:00:00:00", &binaddr);

    cr = (struct hci_conn_info_req*) &buf;
    bacpy(&cr->bdaddr, &binaddr);
    cr->type = ACL_LINK;

    if (ioctl(fd, HCIGETCONNINFO, (unsigned long) cr) == 0)
        handle = htobs(cr->conn_info->handle);

    return Py_BuildValue("i", handle);
}
PyDoc_STRVAR( bt_hci_acl_conn_handle_doc,
"hci_acl_conn_handle(hci_fd, address)\n\
\n\
Get the ACL connection handle for the given remote device addr.");

static PyObject *
bt_hci_filter_new(PyObject *self, PyObject *args)
{
    struct hci_filter flt;
    int len = sizeof(flt);
    hci_filter_clear( &flt );
    return Py_BuildValue("s#", (char*)&flt, len);
}
PyDoc_STRVAR(bt_hci_filter_new_doc,
"hci_filter_new()\n\
\n\
Returns a new HCI filter suitable for operating on with the hci_filter_*\n\
methods, and for passing to getsockopt and setsockopt.  The filter is\n\
initially cleared");


static PyObject *
bt_hci_le_add_white_list(PyObject *self, PyObject *args)
{
    PySocketSockObject *socko = NULL;
    int err = 0;
    int to = 0;
    char *addr = NULL;
    bdaddr_t ba;
    int dd = 0;
    uint8_t type;

    if ( !PyArg_ParseTuple(args, "OsHi", &socko, &addr, &type, &to) ) {
        return NULL;
    }
    if ( addr && strlen(addr) ){
        str2ba( addr, &ba );
    }
    else {
        return NULL;
    }

    dd = socko->sock_fd;
    err = hci_le_add_white_list(dd, &ba, type, to);
    if ( err < 0 ) {
        return NULL;
    }

    return Py_BuildValue("i", err);
}

PyDoc_STRVAR(bt_hci_le_add_white_list_doc,
"hci_le_add_white_list( dd, btaddr, type, to )\n\
\n\
Add the given MAC to the LE scan white list");

static PyObject *
bt_hci_le_read_white_list_size(PyObject *self, PyObject *args)
{
    PySocketSockObject *socko = NULL;
    int err = 0;
    int to = 0;
    uint8_t ret;
    int  dd;
    if ( !PyArg_ParseTuple(args, "Oi", &socko, &to) ) {
        return NULL;
    }
    dd = socko->sock_fd;
    err = hci_le_read_white_list_size(dd, &ret, to);
    if ( err < 0 ) {
        return NULL;
    }
    return Py_BuildValue("i", ret);
}
PyDoc_STRVAR(bt_hci_le_read_white_list_size_doc,
"hci_le_read_white_list_size( dd, size, to )\n\
\n\
Read the total length of the LE scan white list. This is the length of the\n\
empty list, not the total entries in the list");

static PyObject *
bt_hci_le_clear_white_list(PyObject *self, PyObject *args)
{
    PySocketSockObject *socko = NULL;
    int err = 0;
    int to = 0;
    int dd;
    if ( !PyArg_ParseTuple(args, "Oi", &socko, &to) ) {
        return NULL;
    }
    dd = socko->sock_fd;
    err = hci_le_clear_white_list(dd, to);
    return Py_BuildValue("i", err);
}
PyDoc_STRVAR(bt_hci_le_clear_white_list_doc,
"hci_le_clear_white_list( dd, to )\n\
\n\
Clears the LE scan white list");

/*
 * -------------------
 *  End of HCI section
 * -------------------
 */


/* ========= SDP specific bluetooth module methods ========== */

PyObject *
bt_sdp_advertise_service( PyObject *self, PyObject *args )
{
    PySocketSockObject *socko = NULL;
    char *name = NULL,
         *service_id_str = NULL,
         *provider = NULL,
         *description = NULL;
    PyObject *service_classes, *profiles, *protocols;
    Py_ssize_t namelen = 0, provlen = 0, desclen = 0;
    uuid_t svc_uuid = { 0 };
    int i;
    char addrbuf[256] = { 0 };
    int res;
    socklen_t addrlen;
    struct sockaddr *sockaddr;
    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid;
    sdp_list_t *l2cap_list = 0,
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0,
               *profile_list = 0,
               *svc_class_list = 0,
               *access_proto_list = 0;
    sdp_data_t *channel = 0, *psm = 0;

    sdp_record_t record;
    sdp_session_t *session = 0;
    int err = 0;

    if (!PyArg_ParseTuple(args, "O!s#sOOs#s#O", &sock_type, &socko, &name,
                &namelen, &service_id_str, &service_classes,
                &profiles, &provider, &provlen, &description, &desclen,
                &protocols)) {
        return 0;
    }
    if( provlen == 0 ) provider = NULL;
    if( desclen == 0 ) description = NULL;

    if( socko->sdp_record_handle != 0 ) {
        PyErr_SetString(bluetooth_error,
                "SDP service record already registered with this socket!");
        return 0;
    }

    if( namelen == 0 ) {
        PyErr_SetString(bluetooth_error, "must specify name!");
        return 0;
    }

    // convert the service ID string into a uuid_t if it was specified
    if( strlen(service_id_str) && ! str2uuid( service_id_str, &svc_uuid ) ) {
        PyErr_SetString(PyExc_ValueError, "invalid service ID");
        return NULL;
    }

    // service_classes must be a list / sequence
    if (! PySequence_Check(service_classes)) {
        PyErr_SetString(PyExc_ValueError, "service_classes must be a sequence");
        return 0;
    }
    // make sure each item in the list is a valid UUID
    for(i = 0; i < PySequence_Length(service_classes); ++i) {
        PyObject *item = PySequence_GetItem(service_classes, i);
        if( ! pyunicode2uuid( item, NULL ) ) {
            PyErr_SetString(PyExc_ValueError,
                    "service_classes must be a list of "
                    "strings, each either of the form XXXX or "
                    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX");
            return 0;
        }
    }

    // profiles must be a list / sequence
    if (! PySequence_Check(profiles)) {
            PyErr_SetString(PyExc_ValueError, "profiles must be a sequence");
            return 0;
    }
    // make sure each item in the list is a valid ( uuid, version ) pair
    for(i = 0; i < PySequence_Length(profiles); ++i) {
        char *profile_uuid_str = NULL;
        uint16_t version;
        PyObject *tuple = PySequence_GetItem(profiles, i);
        if ( ( ! PySequence_Check(tuple) ) ||
             ( ! PyArg_ParseTuple(tuple, "sH",
                 &profile_uuid_str, &version)) ||
             ( ! str2uuid( profile_uuid_str, NULL ) )
           ) {
            PyErr_SetString(PyExc_ValueError,
                    "Each profile must be a ('uuid', version) tuple");
            return 0;
        }
    }

    // protocols must be a list / sequence
    if (! PySequence_Check(protocols)) {
        PyErr_SetString(PyExc_ValueError, "protocols must be a sequence");
        return 0;
    }
    // make sure each item in the list is a valid UUID
    for(i = 0; i < PySequence_Length(protocols); ++i) {
        PyObject *item = PySequence_GetItem(protocols, i);
        if( ! pyunicode2uuid( item, NULL ) ) {
            PyErr_SetString(PyExc_ValueError,
                    "protocols must be a list of "
                    "strings, each either of the form XXXX or "
                    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX");
            return 0;
        }
    }

    // verify that the socket is bound and listening
    if( ! socko->is_listening_socket ) {
        PyErr_SetString(bluetooth_error,
                "must have already called socket.listen()");
        return 0;
    }

    // verify whether device can be advertisable
    if(adv_available(socko) < 0) {
        PyErr_SetString(bluetooth_error, "no advertisable device");
        return 0;
    }

    // get the socket information
        if (!getsockaddrlen(socko, &addrlen)) {
        PyErr_SetString(bluetooth_error, "error getting socket information");
                return 0;
    }
        Py_BEGIN_ALLOW_THREADS
        res = getsockname(socko->sock_fd, (struct sockaddr *) addrbuf, &addrlen);
        Py_END_ALLOW_THREADS
        if (res < 0) {
        PyErr_SetString(bluetooth_error, "error getting socket information");
                return 0;
    }
    sockaddr = (struct sockaddr *)addrbuf;

    // can only deal with L2CAP and RFCOMM sockets
    if( socko->sock_proto != BTPROTO_L2CAP &&
            socko->sock_proto != BTPROTO_RFCOMM ) {
        PyErr_SetString(bluetooth_error,
                "Sorry, can only advertise L2CAP and RFCOMM sockets for now");
        return 0;
    }

    // abort if this socket is already advertising a service
    if( socko->sdp_record_handle != 0 && socko->sdp_session != NULL ) {
        PyErr_SetString(bluetooth_error,
                "This socket is already being used to advertise a service!\n"
                "Use stop_advertising first!\n");
        return 0;
    }

    // okay, now construct the SDP service record.
    memset( &record, 0, sizeof(sdp_record_t) );

    record.handle = 0xffffffff;

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( &record, root_list );

    // set l2cap information (this will always go in)
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );

    if( socko->sock_proto == BTPROTO_RFCOMM ) {
        // register the RFCOMM channel for RFCOMM sockets
        uint8_t rfcomm_channel = ((struct sockaddr_rc*)sockaddr)->rc_channel;

        sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
        channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
        rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
        sdp_list_append( rfcomm_list, channel );
        sdp_list_append( proto_list, rfcomm_list );

    } else {
        // register the PSM for L2CAP sockets
        unsigned short l2cap_psm = ((struct sockaddr_l2*)sockaddr)->l2_psm;

        psm = sdp_data_alloc(SDP_UINT16, &l2cap_psm);
        sdp_list_append(l2cap_list, psm);
    }

    // add additional protocols, if any
    sdp_list_t *extra_protos_array[PySequence_Length(protocols)];
    if (PySequence_Length(protocols) > 0) {
        for(i = 0; i < PySequence_Length(protocols); i++) {
            uuid_t *proto_uuid = (uuid_t*) malloc( sizeof( uuid_t ) );
            PyObject *item = PySequence_GetItem(protocols, i);
            pyunicode2uuid( item, proto_uuid );

            sdp_list_t *new_list;
            new_list = sdp_list_append( 0, proto_uuid );
            proto_list = sdp_list_append( proto_list, new_list );

            // keep track, to free the list later
            extra_protos_array[i] = new_list;
        }
    }

    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( &record, access_proto_list );

    // add service classes, if any
    for(i = 0; i < PySequence_Length(service_classes); i++) {
        uuid_t *svc_class_uuid = (uuid_t*) malloc( sizeof( uuid_t ) );
        PyObject *item = PySequence_GetItem(service_classes, i);
        pyunicode2uuid( item, svc_class_uuid );
        svc_class_list = sdp_list_append(svc_class_list, svc_class_uuid);
    }
    sdp_set_service_classes(&record, svc_class_list);

    // add profiles, if any
    for(i = 0; i < PySequence_Length(profiles); i++) {
        char *profile_uuid_str;
        sdp_profile_desc_t *profile_desc =
            (sdp_profile_desc_t*)malloc(sizeof(sdp_profile_desc_t));
        PyObject *tuple = PySequence_GetItem(profiles, i);
        PyArg_ParseTuple(tuple, "sH", &profile_uuid_str,
                &profile_desc->version);
        str2uuid( profile_uuid_str, &profile_desc->uuid );
        profile_list = sdp_list_append( profile_list, profile_desc );
    }
    sdp_set_profile_descs(&record, profile_list);

    // set the name, provider and description
    sdp_set_info_attr( &record, name, provider, description );

    // set the general service ID, if needed
    if( strlen(service_id_str) ) sdp_set_service_id( &record, svc_uuid );

    // connect to the local SDP server, register the service record, and
    // disconnect
    Py_BEGIN_ALLOW_THREADS
    session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, 0 );
    Py_END_ALLOW_THREADS
    if (!session) {
        PyErr_SetFromErrno (bluetooth_error);
        return 0;
    }
    socko->sdp_session = session;
    Py_BEGIN_ALLOW_THREADS
    err = sdp_record_register(session, &record, 0);
    Py_END_ALLOW_THREADS

    // cleanup
    if( psm ) sdp_data_free( psm );
    if( channel ) sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    for(i = 0; i < PySequence_Length(protocols); i++) {
        sdp_list_free( extra_protos_array[i], free );
    }
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );
    sdp_list_free( svc_class_list, free );
    sdp_list_free( profile_list, free );

    if( err ) {
        PyErr_SetFromErrno(bluetooth_error);
        return 0;
    }
    socko->sdp_record_handle = record.handle;

    Py_RETURN_NONE;
}
PyDoc_STRVAR( bt_sdp_advertise_service_doc,
"sdp_advertise_service( socket, name )\n\
\n\
Registers a service with the local SDP server.\n\
\n\
socket must be a bound, listening socket - you must have already\n\
called socket.listen().  Only L2CAP and RFCOMM sockets are supported.\n\
\n\
name is the name that you want to appear in the SDP record\n\
\n\
Registered services will be automatically unregistered when the socket is\n\
closed.");


PyObject *
bt_sdp_stop_advertising( PyObject *self, PyObject *args )
{
    PySocketSockObject *socko = NULL;

    if ( !PyArg_ParseTuple(args, "O!", &sock_type, &socko ) ) {
        return 0;
    }

    // verify that we got a real socket object
    if( ! socko || (Py_TYPE(socko) != &sock_type) ) {
        // TODO change this to a more accurate exception type
        PyErr_SetString(bluetooth_error,
                "must pass in _bluetooth.socket object");
        return 0;
    }

    if( socko->sdp_session != NULL ) {
        Py_BEGIN_ALLOW_THREADS
        sdp_close( socko->sdp_session );
        Py_END_ALLOW_THREADS
        socko->sdp_session = NULL;
        socko->sdp_record_handle = 0;
    } else {
        PyErr_SetString( bluetooth_error, "not currently advertising!");
    }

    Py_RETURN_NONE;
}
PyDoc_STRVAR( bt_sdp_stop_advertising_doc,
"sdp_stop_advertising( socket )\n\
\n\
Stop advertising services associated with this socket");

/* List of functions exported by this module. */

#define DECL_BT_METHOD(name, argtype){ #name, (PyCFunction)bt_ ##name, argtype, bt_ ## name ## _doc }

static PyMethodDef bt_methods[] = {
    DECL_BT_METHOD( hci_devid, METH_VARARGS ),
    DECL_BT_METHOD( hci_get_route, METH_VARARGS ),
    DECL_BT_METHOD( hci_role, METH_VARARGS ),
    DECL_BT_METHOD( hci_read_clock, METH_VARARGS ),
    DECL_BT_METHOD( hci_acl_conn_handle, METH_VARARGS ),
    DECL_BT_METHOD( hci_open_dev, METH_VARARGS ),
    DECL_BT_METHOD( hci_close_dev, METH_VARARGS ),
    DECL_BT_METHOD( hci_send_cmd, METH_VARARGS ),
    DECL_BT_METHOD( hci_send_req, METH_VARARGS | METH_KEYWORDS ),
    DECL_BT_METHOD( hci_inquiry, METH_VARARGS | METH_KEYWORDS ),
    DECL_BT_METHOD( hci_read_remote_name, METH_VARARGS | METH_KEYWORDS ),
    DECL_BT_METHOD( hci_filter_new, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_clear, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_all_events, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_all_ptypes, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_clear_opcode, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_set_ptype, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_clear_ptype, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_test_ptype, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_set_event, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_clear_event, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_test_event, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_set_opcode, METH_VARARGS ),
    DECL_BT_METHOD( hci_filter_test_opcode, METH_VARARGS ),
    DECL_BT_METHOD( cmd_opcode_pack, METH_VARARGS ),
    DECL_BT_METHOD( cmd_opcode_ogf, METH_VARARGS ),
    DECL_BT_METHOD( cmd_opcode_ocf, METH_VARARGS ),
    DECL_BT_METHOD( ba2str, METH_VARARGS ),
    DECL_BT_METHOD( str2ba, METH_VARARGS ),
    DECL_BT_METHOD( hci_opcode_name, METH_VARARGS),
    DECL_BT_METHOD( hci_event_name, METH_VARARGS),
#ifndef NO_DUP
    DECL_BT_METHOD( fromfd, METH_VARARGS ),
#endif
    DECL_BT_METHOD( btohs, METH_VARARGS ),
    DECL_BT_METHOD( btohl, METH_VARARGS ),
    DECL_BT_METHOD( htobs, METH_VARARGS ),
    DECL_BT_METHOD( htobl, METH_VARARGS ),
    DECL_BT_METHOD( getdefaulttimeout, METH_NOARGS ),
    DECL_BT_METHOD( setdefaulttimeout, METH_O ),
    DECL_BT_METHOD( sdp_advertise_service, METH_VARARGS ),
    DECL_BT_METHOD( sdp_stop_advertising, METH_VARARGS ),
    DECL_BT_METHOD( hci_le_add_white_list, METH_VARARGS ),
    DECL_BT_METHOD( hci_le_read_white_list_size, METH_VARARGS ),
    DECL_BT_METHOD( hci_le_clear_white_list, METH_VARARGS ),
//    DECL_BT_METHOD( advertise_service, METH_VARARGS | METH_KEYWORDS ),
        {NULL,                        NULL}                 /* Sentinel */
};

#undef DECL_BT_METHOD

/* Initialize the bt module.
*/

PyDoc_STRVAR(socket_doc,
"Implementation module for bluetooth operations.\n\
\n\
See the bluetooth module for documentation.");

#define INITERROR return NULL

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_bluetooth",
    socket_doc,
    -1,
    bt_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__bluetooth(void)
{
    Py_SET_TYPE(&sock_type, &PyType_Type);
    Py_SET_TYPE(&sdp_session_type, &PyType_Type);
    PyObject *m = PyModule_Create(&moduledef);
    if (m == NULL)
        INITERROR;

    bluetooth_error = PyErr_NewException("_bluetooth.error", NULL, NULL);
    if (bluetooth_error == NULL)
        INITERROR;
    Py_INCREF(bluetooth_error);
    PyModule_AddObject(m, "error", bluetooth_error);

    socket_timeout = PyErr_NewException("_bluetooth.timeout", bluetooth_error,
                                        NULL);
    if (socket_timeout == NULL)
        INITERROR;
    Py_INCREF(socket_timeout);
    PyModule_AddObject(m, "timeout", socket_timeout);

    Py_INCREF((PyObject *)&sock_type);
    if (PyModule_AddObject(m, "btsocket", (PyObject *)&sock_type) != 0)
        INITERROR;

    Py_INCREF((PyObject *)&sdp_session_type);
    if (PyModule_AddObject(m, "SDPSession", (PyObject *)&sdp_session_type) != 0)
        INITERROR;

    // Global variables that can be accessible from Python.
//    PyModule_AddIntMacro(m, PF_BLUETOOTH);
//    PyModule_AddIntMacro(m, AF_BLUETOOTH);
    PyModule_AddIntMacro(m, SOL_HCI);
    PyModule_AddIntMacro(m, HCI_DATA_DIR);
    PyModule_AddIntMacro(m, HCI_TIME_STAMP);
    PyModule_AddIntMacro(m, HCI_FILTER);
    PyModule_AddIntMacro(m, HCI_MAX_EVENT_SIZE);
    PyModule_AddIntMacro(m, HCI_EVENT_HDR_SIZE);

    PyModule_AddIntConstant(m, "HCI", BTPROTO_HCI);
    PyModule_AddIntConstant(m, "L2CAP", BTPROTO_L2CAP);
    PyModule_AddIntConstant(m, "RFCOMM", BTPROTO_RFCOMM);
    PyModule_AddIntConstant(m, "SCO", BTPROTO_SCO);

//        /* Socket types */
//    PyModule_AddIntMacro(m, SOCK_STREAM);
//    PyModule_AddIntMacro(m, SOCK_DGRAM);
//    PyModule_AddIntMacro(m, SOCK_RAW);
//    PyModule_AddIntMacro(m, SOCK_SEQPACKET);

/* HCI Constants */

    /* HCI OGF values */
#ifdef OGF_LINK_CTL
    PyModule_AddIntMacro(m, OGF_LINK_CTL);
#endif
#ifdef OGF_LINK_POLICY
    PyModule_AddIntMacro(m, OGF_LINK_POLICY);
#endif
#ifdef OGF_HOST_CTL
    PyModule_AddIntMacro(m, OGF_HOST_CTL);
#endif
#ifdef OGF_INFO_PARAM
    PyModule_AddIntMacro(m, OGF_INFO_PARAM);
#endif
#ifdef OGF_STATUS_PARAM
    PyModule_AddIntMacro(m, OGF_STATUS_PARAM);
#endif
#ifdef OGF_TESTING_CMD
    PyModule_AddIntMacro(m, OGF_TESTING_CMD);
#endif
#ifdef OGF_VENDOR_CMD
    PyModule_AddIntMacro(m, OGF_VENDOR_CMD);
#endif

    /* HCI OCF values */
#ifdef OCF_INQUIRY
    PyModule_AddIntMacro(m, OCF_INQUIRY);
#endif
#ifdef OCF_INQUIRY_CANCEL
    PyModule_AddIntMacro(m, OCF_INQUIRY_CANCEL);
#endif
#ifdef OCF_PERIODIC_INQUIRY
    PyModule_AddIntMacro(m, OCF_PERIODIC_INQUIRY);
#endif
#ifdef OCF_EXIT_PERIODIC_INQUIRY
    PyModule_AddIntMacro(m, OCF_EXIT_PERIODIC_INQUIRY);
#endif
#ifdef OCF_CREATE_CONN
    PyModule_AddIntMacro(m, OCF_CREATE_CONN);
#endif
#ifdef CREATE_CONN_CP_SIZE
    PyModule_AddIntMacro(m, CREATE_CONN_CP_SIZE);
#endif

#ifdef ACL_PTYPE_MASK
    PyModule_AddIntMacro(m, ACL_PTYPE_MASK);
#endif

#ifdef OCF_DISCONNECT
    PyModule_AddIntMacro(m, OCF_DISCONNECT);
#endif
#ifdef OCF_ADD_SCO
    PyModule_AddIntMacro(m, OCF_ADD_SCO);
#endif
#ifdef OCF_ACCEPT_CONN_REQ
    PyModule_AddIntMacro(m, OCF_ACCEPT_CONN_REQ);
#endif
#ifdef OCF_REJECT_CONN_REQ
    PyModule_AddIntMacro(m, OCF_REJECT_CONN_REQ);
#endif
#ifdef OCF_LINK_KEY_REPLY
    PyModule_AddIntMacro(m, OCF_LINK_KEY_REPLY);
#endif
#ifdef OCF_LINK_KEY_NEG_REPLY
    PyModule_AddIntMacro(m, OCF_LINK_KEY_NEG_REPLY);
#endif
#ifdef OCF_PIN_CODE_REPLY
    PyModule_AddIntMacro(m, OCF_PIN_CODE_REPLY);
#endif
#ifdef OCF_PIN_CODE_NEG_REPLY
    PyModule_AddIntMacro(m, OCF_PIN_CODE_NEG_REPLY);
#endif
#ifdef OCF_SET_CONN_PTYPE
    PyModule_AddIntMacro(m, OCF_SET_CONN_PTYPE);
#endif
#ifdef OCF_AUTH_REQUESTED
    PyModule_AddIntMacro(m, OCF_AUTH_REQUESTED);
#endif
#ifdef OCF_SET_CONN_ENCRYPT
    PyModule_AddIntMacro(m, OCF_SET_CONN_ENCRYPT);
#endif
#ifdef OCF_REMOTE_NAME_REQ
    PyModule_AddIntMacro(m, OCF_REMOTE_NAME_REQ);
#endif
#ifdef OCF_READ_REMOTE_FEATURES
    PyModule_AddIntMacro(m, OCF_READ_REMOTE_FEATURES);
#endif
#ifdef OCF_READ_REMOTE_EXT_FEATURES
    PyModule_AddIntMacro(m, OCF_READ_REMOTE_EXT_FEATURES);
#endif
#ifdef OCF_READ_REMOTE_VERSION
    PyModule_AddIntMacro(m, OCF_READ_REMOTE_VERSION);
#endif
#ifdef OCF_READ_CLOCK_OFFSET
    PyModule_AddIntMacro(m, OCF_READ_CLOCK_OFFSET);
#endif
#ifdef OCF_READ_CLOCK_OFFSET
    PyModule_AddIntMacro(m, OCF_READ_CLOCK);
#endif
#ifdef OCF_IO_CAPABILITY_REPLY
    PyModule_AddIntMacro(m, OCF_IO_CAPABILITY_REPLY);
#endif
#ifdef OCF_USER_CONFIRM_REPLY
    PyModule_AddIntMacro(m, OCF_USER_CONFIRM_REPLY);
#endif
#ifdef OCF_HOLD_MODE
    PyModule_AddIntMacro(m, OCF_HOLD_MODE);
#endif
#ifdef OCF_SNIFF_MODE
    PyModule_AddIntMacro(m, OCF_SNIFF_MODE);
#endif
#ifdef OCF_EXIT_SNIFF_MODE
    PyModule_AddIntMacro(m, OCF_EXIT_SNIFF_MODE);
#endif
#ifdef OCF_PARK_MODE
    PyModule_AddIntMacro(m, OCF_PARK_MODE);
#endif
#ifdef OCF_EXIT_PARK_MODE
    PyModule_AddIntMacro(m, OCF_EXIT_PARK_MODE);
#endif
#ifdef OCF_QOS_SETUP
    PyModule_AddIntMacro(m, OCF_QOS_SETUP);
#endif
#ifdef OCF_ROLE_DISCOVERY
    PyModule_AddIntMacro(m, OCF_ROLE_DISCOVERY);
#endif
#ifdef OCF_SWITCH_ROLE
    PyModule_AddIntMacro(m, OCF_SWITCH_ROLE);
#endif
#ifdef OCF_READ_LINK_POLICY
    PyModule_AddIntMacro(m, OCF_READ_LINK_POLICY);
#endif
#ifdef OCF_WRITE_LINK_POLICY
    PyModule_AddIntMacro(m, OCF_WRITE_LINK_POLICY);
#endif
#ifdef OCF_RESET
    PyModule_AddIntMacro(m, OCF_RESET);
#endif
#ifdef OCF_SET_EVENT_MASK
    PyModule_AddIntMacro(m, OCF_SET_EVENT_MASK);
#endif
#ifdef OCF_SET_EVENT_FLT
    PyModule_AddIntMacro(m, OCF_SET_EVENT_FLT);
#endif
#ifdef OCF_CHANGE_LOCAL_NAME
    PyModule_AddIntMacro(m, OCF_CHANGE_LOCAL_NAME);
#endif
#ifdef OCF_READ_LOCAL_NAME
    PyModule_AddIntMacro(m, OCF_READ_LOCAL_NAME);
#endif
#ifdef OCF_WRITE_CA_TIMEOUT
    PyModule_AddIntMacro(m, OCF_WRITE_CA_TIMEOUT);
#endif
#ifdef OCF_WRITE_PG_TIMEOUT
    PyModule_AddIntMacro(m, OCF_WRITE_PG_TIMEOUT);
#endif
#ifdef OCF_READ_PAGE_TIMEOUT
    PyModule_AddIntMacro(m, OCF_READ_PAGE_TIMEOUT);
#endif
#ifdef OCF_WRITE_PAGE_TIMEOUT
    PyModule_AddIntMacro(m, OCF_WRITE_PAGE_TIMEOUT);
#endif
#ifdef OCF_READ_SCAN_ENABLE
    PyModule_AddIntMacro(m, OCF_READ_SCAN_ENABLE);
#endif
#ifdef OCF_WRITE_SCAN_ENABLE
    PyModule_AddIntMacro(m, OCF_WRITE_SCAN_ENABLE);
#endif
#ifdef OCF_READ_PAGE_ACTIVITY
    PyModule_AddIntMacro(m, OCF_READ_PAGE_ACTIVITY);
#endif
#ifdef OCF_WRITE_PAGE_ACTIVITY
    PyModule_AddIntMacro(m, OCF_WRITE_PAGE_ACTIVITY);
#endif
#ifdef OCF_READ_INQ_ACTIVITY
    PyModule_AddIntMacro(m, OCF_READ_INQ_ACTIVITY);
#endif
#ifdef OCF_WRITE_INQ_ACTIVITY
    PyModule_AddIntMacro(m, OCF_WRITE_INQ_ACTIVITY);
#endif
#ifdef OCF_READ_AUTH_ENABLE
    PyModule_AddIntMacro(m, OCF_READ_AUTH_ENABLE);
#endif
#ifdef OCF_WRITE_AUTH_ENABLE
    PyModule_AddIntMacro(m, OCF_WRITE_AUTH_ENABLE);
#endif
#ifdef OCF_READ_ENCRYPT_MODE
    PyModule_AddIntMacro(m, OCF_READ_ENCRYPT_MODE);
#endif
#ifdef OCF_WRITE_ENCRYPT_MODE
    PyModule_AddIntMacro(m, OCF_WRITE_ENCRYPT_MODE);
#endif
#ifdef OCF_READ_CLASS_OF_DEV
    PyModule_AddIntMacro(m, OCF_READ_CLASS_OF_DEV);
#endif
#ifdef OCF_WRITE_CLASS_OF_DEV
    PyModule_AddIntMacro(m, OCF_WRITE_CLASS_OF_DEV);
#endif
#ifdef OCF_READ_VOICE_SETTING
    PyModule_AddIntMacro(m, OCF_READ_VOICE_SETTING);
#endif
#ifdef OCF_WRITE_VOICE_SETTING
    PyModule_AddIntMacro(m, OCF_WRITE_VOICE_SETTING);
#endif
#ifdef OCF_READ_TRANSMIT_POWER_LEVEL
    PyModule_AddIntMacro(m, OCF_READ_TRANSMIT_POWER_LEVEL);
#endif
#ifdef OCF_HOST_BUFFER_SIZE
    PyModule_AddIntMacro(m, OCF_HOST_BUFFER_SIZE);
#endif
#ifdef OCF_READ_LINK_SUPERVISION_TIMEOUT
    PyModule_AddIntMacro(m, OCF_READ_LINK_SUPERVISION_TIMEOUT);
#endif
#ifdef OCF_WRITE_LINK_SUPERVISION_TIMEOUT
    PyModule_AddIntMacro(m, OCF_WRITE_LINK_SUPERVISION_TIMEOUT);
#endif
#ifdef OCF_READ_CURRENT_IAC_LAP
    PyModule_AddIntMacro(m, OCF_READ_CURRENT_IAC_LAP);
#endif
#ifdef OCF_WRITE_CURRENT_IAC_LAP
    PyModule_AddIntMacro(m, OCF_WRITE_CURRENT_IAC_LAP);
#endif
#ifdef OCF_READ_INQUIRY_MODE
    PyModule_AddIntMacro(m, OCF_READ_INQUIRY_MODE);
#endif
#ifdef OCF_WRITE_INQUIRY_MODE
    PyModule_AddIntMacro(m, OCF_WRITE_INQUIRY_MODE);
#endif
#ifdef OCF_READ_AFH_MODE
    PyModule_AddIntMacro(m, OCF_READ_AFH_MODE);
#endif
#ifdef OCF_WRITE_AFH_MODE
    PyModule_AddIntMacro(m, OCF_WRITE_AFH_MODE);
#endif

#ifdef OCF_WRITE_EXT_INQUIRY_RESPONSE
    PyModule_AddIntMacro(m, OCF_WRITE_EXT_INQUIRY_RESPONSE);
#endif

#ifdef OCF_WRITE_SIMPLE_PAIRING_MODE
    PyModule_AddIntMacro(m, OCF_WRITE_SIMPLE_PAIRING_MODE);
#endif

#ifdef OCF_READ_LOCAL_VERSION
    PyModule_AddIntMacro(m, OCF_READ_LOCAL_VERSION);
#endif
#ifdef OCF_READ_LOCAL_FEATURES
    PyModule_AddIntMacro(m, OCF_READ_LOCAL_FEATURES);
#endif
#ifdef OCF_READ_BUFFER_SIZE
    PyModule_AddIntMacro(m, OCF_READ_BUFFER_SIZE);
#endif
#ifdef OCF_READ_BD_ADDR
    PyModule_AddIntMacro(m, OCF_READ_BD_ADDR);
#endif
#ifdef OCF_READ_FAILED_CONTACT_COUNTER
    PyModule_AddIntMacro(m, OCF_READ_FAILED_CONTACT_COUNTER);
#endif
#ifdef OCF_RESET_FAILED_CONTACT_COUNTER
    PyModule_AddIntMacro(m, OCF_RESET_FAILED_CONTACT_COUNTER);
#endif
#ifdef OCF_GET_LINK_QUALITY
    PyModule_AddIntMacro(m, OCF_GET_LINK_QUALITY);
#endif
#ifdef OCF_READ_RSSI
    PyModule_AddIntMacro(m, OCF_READ_RSSI);
#endif
#ifdef OCF_READ_AFH_MAP
    PyModule_AddIntMacro(m, OCF_READ_AFH_MAP);
#endif

    /* HCI events */
#ifdef EVT_INQUIRY_COMPLETE
    PyModule_AddIntMacro(m, EVT_INQUIRY_COMPLETE);
#endif
#ifdef EVT_INQUIRY_RESULT
    PyModule_AddIntMacro(m, EVT_INQUIRY_RESULT);
#endif
#ifdef EVT_CONN_COMPLETE
    PyModule_AddIntMacro(m, EVT_CONN_COMPLETE);
#endif
#ifdef EVT_CONN_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_CONN_COMPLETE_SIZE);
#endif
#ifdef EVT_CONN_REQUEST
    PyModule_AddIntMacro(m, EVT_CONN_REQUEST);
#endif
#ifdef EVT_CONN_REQUEST_SIZE
    PyModule_AddIntMacro(m, EVT_CONN_REQUEST_SIZE);
#endif
#ifdef EVT_DISCONN_COMPLETE
    PyModule_AddIntMacro(m, EVT_DISCONN_COMPLETE);
#endif
#ifdef EVT_DISCONN_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_DISCONN_COMPLETE_SIZE);
#endif
#ifdef EVT_AUTH_COMPLETE
    PyModule_AddIntMacro(m, EVT_AUTH_COMPLETE);
#endif
#ifdef EVT_AUTH_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_AUTH_COMPLETE_SIZE);
#endif
#ifdef EVT_REMOTE_NAME_REQ_COMPLETE
    PyModule_AddIntMacro(m, EVT_REMOTE_NAME_REQ_COMPLETE);
#endif
#ifdef EVT_REMOTE_NAME_REQ_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_REMOTE_NAME_REQ_COMPLETE_SIZE);
#endif
#ifdef EVT_ENCRYPT_CHANGE
    PyModule_AddIntMacro(m, EVT_ENCRYPT_CHANGE);
#endif
#ifdef EVT_ENCRYPT_CHANGE_SIZE
    PyModule_AddIntMacro(m, EVT_ENCRYPT_CHANGE_SIZE);
#endif
#ifdef EVT_READ_REMOTE_FEATURES_COMPLETE
    PyModule_AddIntMacro(m, EVT_READ_REMOTE_FEATURES_COMPLETE);
#endif
#ifdef EVT_READ_REMOTE_FEATURES_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_READ_REMOTE_FEATURES_COMPLETE_SIZE);
#endif
#ifdef EVT_READ_REMOTE_VERSION_COMPLETE
    PyModule_AddIntMacro(m, EVT_READ_REMOTE_VERSION_COMPLETE);
#endif
#ifdef EVT_READ_REMOTE_VERSION_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_READ_REMOTE_VERSION_COMPLETE_SIZE);
#endif
#ifdef EVT_QOS_SETUP_COMPLETE
    PyModule_AddIntMacro(m, EVT_QOS_SETUP_COMPLETE);
#endif
#ifdef EVT_QOS_SETUP_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_QOS_SETUP_COMPLETE_SIZE);
#endif
#ifdef EVT_CMD_COMPLETE
    PyModule_AddIntMacro(m, EVT_CMD_COMPLETE);
#endif
#ifdef EVT_CMD_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_CMD_COMPLETE_SIZE);
#endif
#ifdef EVT_CMD_STATUS
    PyModule_AddIntMacro(m, EVT_CMD_STATUS);
#endif
#ifdef EVT_CMD_STATUS_SIZE
    PyModule_AddIntMacro(m, EVT_CMD_STATUS_SIZE);
#endif
#ifdef EVT_ROLE_CHANGE
    PyModule_AddIntMacro(m, EVT_ROLE_CHANGE);
#endif
#ifdef EVT_ROLE_CHANGE_SIZE
    PyModule_AddIntMacro(m, EVT_ROLE_CHANGE_SIZE);
#endif
#ifdef EVT_NUM_COMP_PKTS
    PyModule_AddIntMacro(m, EVT_NUM_COMP_PKTS);
#endif
#ifdef EVT_NUM_COMP_PKTS_SIZE
    PyModule_AddIntMacro(m, EVT_NUM_COMP_PKTS_SIZE);
#endif
#ifdef EVT_MODE_CHANGE
    PyModule_AddIntMacro(m, EVT_MODE_CHANGE);
#endif
#ifdef EVT_MODE_CHANGE_SIZE
    PyModule_AddIntMacro(m, EVT_MODE_CHANGE_SIZE);
#endif
#ifdef EVT_PIN_CODE_REQ
    PyModule_AddIntMacro(m, EVT_PIN_CODE_REQ);
#endif
#ifdef EVT_PIN_CODE_REQ_SIZE
    PyModule_AddIntMacro(m, EVT_PIN_CODE_REQ_SIZE);
#endif
#ifdef EVT_LINK_KEY_REQ
    PyModule_AddIntMacro(m, EVT_LINK_KEY_REQ);
#endif
#ifdef EVT_LINK_KEY_REQ_SIZE
    PyModule_AddIntMacro(m, EVT_LINK_KEY_REQ_SIZE);
#endif
#ifdef EVT_LINK_KEY_NOTIFY
    PyModule_AddIntMacro(m, EVT_LINK_KEY_NOTIFY);
#endif
#ifdef EVT_LINK_KEY_NOTIFY_SIZE
    PyModule_AddIntMacro(m, EVT_LINK_KEY_NOTIFY_SIZE);
#endif
#ifdef EVT_MAX_SLOTS_CHANGE
    PyModule_AddIntMacro(m, EVT_MAX_SLOTS_CHANGE);
#endif
#ifdef EVT_READ_CLOCK_OFFSET_COMPLETE
    PyModule_AddIntMacro(m, EVT_READ_CLOCK_OFFSET_COMPLETE);
#endif
#ifdef EVT_READ_CLOCK_OFFSET_COMPLETE_SIZE
    PyModule_AddIntMacro(m, EVT_READ_CLOCK_OFFSET_COMPLETE_SIZE);
#endif
#ifdef EVT_CONN_PTYPE_CHANGED
    PyModule_AddIntMacro(m, EVT_CONN_PTYPE_CHANGED);
#endif
#ifdef EVT_CONN_PTYPE_CHANGED_SIZE
    PyModule_AddIntMacro(m, EVT_CONN_PTYPE_CHANGED_SIZE);
#endif
#ifdef EVT_QOS_VIOLATION
    PyModule_AddIntMacro(m, EVT_QOS_VIOLATION);
#endif
#ifdef EVT_QOS_VIOLATION_SIZE
    PyModule_AddIntMacro(m, EVT_QOS_VIOLATION_SIZE);
#endif
#ifdef EVT_PSCAN_REP_MODE_CHANGE
    PyModule_AddIntMacro(m,EVT_PSCAN_REP_MODE_CHANGE);
#endif
#ifdef EVT_FLOW_SPEC_COMPLETE
    PyModule_AddIntMacro(m,EVT_FLOW_SPEC_COMPLETE);
#endif
#ifdef EVT_FLOW_SPEC_MODIFY_COMPLETE
    PyModule_AddIntMacro(m,EVT_FLOW_SPEC_MODIFY_COMPLETE);
#endif
#ifdef EVT_INQUIRY_RESULT_WITH_RSSI
    PyModule_AddIntMacro(m, EVT_INQUIRY_RESULT_WITH_RSSI);
#endif
#ifdef EVT_READ_REMOTE_EXT_FEATURES_COMPLETE
    PyModule_AddIntMacro(m, EVT_READ_REMOTE_EXT_FEATURES_COMPLETE);
#endif
#ifdef EVT_EXTENDED_INQUIRY_RESULT
    PyModule_AddIntMacro(m, EVT_EXTENDED_INQUIRY_RESULT);
    PyModule_AddIntConstant(m, "HAVE_EVT_EXTENDED_INQUIRY_RESULT", 1);
#else
    PyModule_AddIntConstant(m, "HAVE_EVT_EXTENDED_INQUIRY_RESULT", 0);
#endif
#ifdef EVT_DISCONNECT_LOGICAL_LINK_COMPLETE
    PyModule_AddIntMacro(m, EVT_DISCONNECT_LOGICAL_LINK_COMPLETE);
#endif
#ifdef EVT_IO_CAPABILITY_REQUEST
    PyModule_AddIntMacro(m, EVT_IO_CAPABILITY_REQUEST);
#endif

#ifdef EVT_IO_CAPABILITY_RESPONSE
    PyModule_AddIntMacro(m, EVT_IO_CAPABILITY_RESPONSE);
#endif

#ifdef EVT_USER_CONFIRM_REQUEST
    PyModule_AddIntMacro(m, EVT_USER_CONFIRM_REQUEST);
#endif

#ifdef EVT_SIMPLE_PAIRING_COMPLETE
    PyModule_AddIntMacro(m, EVT_SIMPLE_PAIRING_COMPLETE);
#endif
#ifdef EVT_TESTING
    PyModule_AddIntMacro(m, EVT_TESTING);
#endif
#ifdef EVT_VENDOR
    PyModule_AddIntMacro(m, EVT_VENDOR);
#endif
#ifdef EVT_STACK_INTERNAL
    PyModule_AddIntMacro(m, EVT_STACK_INTERNAL);
#endif
#ifdef EVT_STACK_INTERNAL_SIZE
    PyModule_AddIntMacro(m, EVT_STACK_INTERNAL_SIZE);
#endif
#ifdef EVT_SI_DEVICE
    PyModule_AddIntMacro(m, EVT_SI_DEVICE);
#endif
#ifdef EVT_SI_DEVICE_SIZE
    PyModule_AddIntMacro(m, EVT_SI_DEVICE_SIZE);
#endif
#ifdef EVT_SI_SECURITY
    PyModule_AddIntMacro(m, EVT_SI_SECURITY);
#endif
#ifdef EVT_NUMBER_COMPLETED_BLOCKS
    PyModule_AddIntMacro(m, EVT_NUMBER_COMPLETED_BLOCKS);
#endif
    /* HCI packet types */
#ifdef HCI_COMMAND_PKT
    PyModule_AddIntMacro(m, HCI_COMMAND_PKT);
#endif
#ifdef HCI_ACLDATA_PKT
    PyModule_AddIntMacro(m, HCI_ACLDATA_PKT);
#endif
#ifdef HCI_SCODATA_PKT
    PyModule_AddIntMacro(m, HCI_SCODATA_PKT);
#endif
#ifdef HCI_EVENT_PKT
    PyModule_AddIntMacro(m, HCI_EVENT_PKT);
#endif
#ifdef HCI_UNKNOWN_PKT
    PyModule_AddIntMacro(m, HCI_UNKNOWN_PKT);
#endif

    /* socket options */
#ifdef        SO_DEBUG
    PyModule_AddIntMacro(m, SO_DEBUG);
#endif
#ifdef        SO_ACCEPTCONN
    PyModule_AddIntMacro(m, SO_ACCEPTCONN);
#endif
#ifdef        SO_REUSEADDR
    PyModule_AddIntMacro(m, SO_REUSEADDR);
#endif
#ifdef        SO_KEEPALIVE
    PyModule_AddIntMacro(m, SO_KEEPALIVE);
#endif
#ifdef        SO_DONTROUTE
    PyModule_AddIntMacro(m, SO_DONTROUTE);
#endif
#ifdef        SO_BROADCAST
    PyModule_AddIntMacro(m, SO_BROADCAST);
#endif
#ifdef        SO_USELOOPBACK
    PyModule_AddIntMacro(m, SO_USELOOPBACK);
#endif
#ifdef        SO_LINGER
    PyModule_AddIntMacro(m, SO_LINGER);
#endif
#ifdef        SO_OOBINLINE
    PyModule_AddIntMacro(m, SO_OOBINLINE);
#endif
#ifdef        SO_REUSEPORT
    PyModule_AddIntMacro(m, SO_REUSEPORT);
#endif
#ifdef        SO_SNDBUF
    PyModule_AddIntMacro(m, SO_SNDBUF);
#endif
#ifdef        SO_RCVBUF
    PyModule_AddIntMacro(m, SO_RCVBUF);
#endif
#ifdef        SO_SNDLOWAT
    PyModule_AddIntMacro(m, SO_SNDLOWAT);
#endif
#ifdef        SO_RCVLOWAT
    PyModule_AddIntMacro(m, SO_RCVLOWAT);
#endif
#ifdef        SO_SNDTIMEO
    PyModule_AddIntMacro(m, SO_SNDTIMEO);
#endif
#ifdef        SO_RCVTIMEO
    PyModule_AddIntMacro(m, SO_RCVTIMEO);
#endif
#ifdef        SO_ERROR
    PyModule_AddIntMacro(m, SO_ERROR);
#endif
#ifdef        SO_TYPE
    PyModule_AddIntMacro(m, SO_TYPE);
#endif

        /* Maximum number of connections for "listen" */
#ifdef        SOMAXCONN
    PyModule_AddIntMacro(m, SOMAXCONN);
#else
    PyModule_AddIntMacro(m, SOMAXCONN);
#endif

        /* Flags for send, recv */
#ifdef        MSG_OOB
    PyModule_AddIntMacro(m, MSG_OOB);
#endif
#ifdef        MSG_PEEK
    PyModule_AddIntMacro(m, MSG_PEEK);
#endif
#ifdef        MSG_DONTROUTE
    PyModule_AddIntMacro(m, MSG_DONTROUTE);
#endif
#ifdef        MSG_DONTWAIT
    PyModule_AddIntMacro(m, MSG_DONTWAIT);
#endif
#ifdef        MSG_EOR
    PyModule_AddIntMacro(m, MSG_EOR);
#endif
#ifdef        MSG_TRUNC
    PyModule_AddIntMacro(m, MSG_TRUNC);
#endif
#ifdef        MSG_CTRUNC
    PyModule_AddIntMacro(m, MSG_CTRUNC);
#endif
#ifdef        MSG_WAITALL
    PyModule_AddIntMacro(m, MSG_WAITALL);
#endif
#ifdef        MSG_BTAG
    PyModule_AddIntMacro(m, MSG_BTAG);
#endif
#ifdef        MSG_ETAG
    PyModule_AddIntMacro(m, MSG_ETAG);
#endif

        /* Size of inquiry info */
#ifdef  INQUIRY_INFO_WITH_RSSI_SIZE
    PyModule_AddIntMacro(m, INQUIRY_INFO_WITH_RSSI_SIZE);
#endif
#ifdef  EXTENDED_INQUIRY_INFO_SIZE
    PyModule_AddIntMacro(m, EXTENDED_INQUIRY_INFO_SIZE);
#endif

        /* Protocol level and numbers, usable for [gs]etsockopt */
    PyModule_AddIntMacro(m, SOL_SOCKET);
    PyModule_AddIntMacro(m, SOL_L2CAP);
    PyModule_AddIntMacro(m, SOL_RFCOMM);
    PyModule_AddIntMacro(m, SOL_SCO);
    PyModule_AddIntMacro(m, SCO_OPTIONS);
    PyModule_AddIntMacro(m, L2CAP_OPTIONS);

    /* special channels to bind() */
    PyModule_AddIntMacro(m, HCI_CHANNEL_CONTROL);
    PyModule_AddIntMacro(m, HCI_CHANNEL_USER);
    PyModule_AddIntMacro(m, HCI_DEV_NONE);

    /* ioctl */
    PyModule_AddIntMacro(m, HCIDEVUP);
    PyModule_AddIntMacro(m, HCIDEVDOWN);
    PyModule_AddIntMacro(m, HCIDEVRESET);
    PyModule_AddIntMacro(m, HCIDEVRESTAT);
    PyModule_AddIntMacro(m, HCIGETDEVLIST);
    PyModule_AddIntMacro(m, HCIGETDEVINFO);
    PyModule_AddIntMacro(m, HCIGETCONNLIST);
    PyModule_AddIntMacro(m, HCIGETCONNINFO);
    PyModule_AddIntMacro(m, HCISETRAW);
    PyModule_AddIntMacro(m, HCISETSCAN);
    PyModule_AddIntMacro(m, HCISETAUTH);
    PyModule_AddIntMacro(m, HCISETENCRYPT);
    PyModule_AddIntMacro(m, HCISETPTYPE);
    PyModule_AddIntMacro(m, HCISETLINKPOL);
    PyModule_AddIntMacro(m, HCISETLINKMODE);
    PyModule_AddIntMacro(m, HCISETACLMTU);
    PyModule_AddIntMacro(m, HCISETSCOMTU);
    PyModule_AddIntMacro(m, HCIINQUIRY);

    PyModule_AddIntMacro(m, ACL_LINK);
    PyModule_AddIntMacro(m, SCO_LINK);

    /* RFCOMM */
    PyModule_AddIntMacro(m, RFCOMM_LM);
    PyModule_AddIntMacro(m, RFCOMM_LM_MASTER);
    PyModule_AddIntMacro(m, RFCOMM_LM_AUTH        );
    PyModule_AddIntMacro(m, RFCOMM_LM_ENCRYPT);
    PyModule_AddIntMacro(m, RFCOMM_LM_TRUSTED);
    PyModule_AddIntMacro(m, RFCOMM_LM_RELIABLE);
    PyModule_AddIntMacro(m, RFCOMM_LM_SECURE);

    /* L2CAP */
    PyModule_AddIntMacro(m, L2CAP_LM);
    PyModule_AddIntMacro(m, L2CAP_LM_MASTER);
    PyModule_AddIntMacro(m, L2CAP_LM_AUTH);
    PyModule_AddIntMacro(m, L2CAP_LM_ENCRYPT);
    PyModule_AddIntMacro(m, L2CAP_LM_TRUSTED);
    PyModule_AddIntMacro(m, L2CAP_LM_RELIABLE);
    PyModule_AddIntMacro(m, L2CAP_LM_SECURE);

    PyModule_AddIntMacro(m, L2CAP_COMMAND_REJ);
    PyModule_AddIntMacro(m, L2CAP_CONN_REQ        );
    PyModule_AddIntMacro(m, L2CAP_CONN_RSP        );
    PyModule_AddIntMacro(m, L2CAP_CONF_REQ        );
    PyModule_AddIntMacro(m, L2CAP_CONF_RSP        );
    PyModule_AddIntMacro(m, L2CAP_DISCONN_REQ);
    PyModule_AddIntMacro(m, L2CAP_DISCONN_RSP);
    PyModule_AddIntMacro(m, L2CAP_ECHO_REQ        );
    PyModule_AddIntMacro(m, L2CAP_ECHO_RSP        );
    PyModule_AddIntMacro(m, L2CAP_INFO_REQ        );
    PyModule_AddIntMacro(m, L2CAP_INFO_RSP        );

    PyModule_AddIntMacro(m, L2CAP_MODE_BASIC);
    PyModule_AddIntMacro(m, L2CAP_MODE_RETRANS);
    PyModule_AddIntMacro(m, L2CAP_MODE_FLOWCTL);
    PyModule_AddIntMacro(m, L2CAP_MODE_ERTM);
    PyModule_AddIntMacro(m, L2CAP_MODE_STREAMING);

    PyModule_AddIntMacro(m, BT_SECURITY);
    PyModule_AddIntMacro(m, BT_SECURITY_SDP);
    PyModule_AddIntMacro(m, BT_SECURITY_LOW);
    PyModule_AddIntMacro(m, BT_SECURITY_MEDIUM);
    PyModule_AddIntMacro(m, BT_SECURITY_HIGH);

#ifdef BT_DEFER_SETUP
    PyModule_AddIntMacro(m, BT_DEFER_SETUP);
#endif
    PyModule_AddIntMacro(m, SOL_BLUETOOTH);

    return m;
}

/*
 * Affix socket module
 * Socket module for python based in the original socket module for python
 * This code is a copy from socket.c source code from python2.2 with
 * updates/modifications to support affix socket interface *
 *   AAA     FFFFFFF FFFFFFF IIIIIII X     X
 * A     A   F       F          I     X   X
 * A     A   F       F      I      X X
 * AAAAAAA   FFFF    FFFF       I      X X
 * A     A   F       F      I     X   X
 * A     A   F       F       IIIIIII X     X
 *
 * Any modifications of this sourcecode must keep this information !!!!!
 *
 * by Carlos Chinea
 * (C) Nokia Research Center, 2004
*/
static char *event_str[EVENT_NUM + 1] = {
        "Unknown",
        "Inquiry Complete",
        "Inquiry Result",
        "Connect Complete",
        "Connect Request",
        "Disconn Complete",
        "Auth Complete",
        "Remote Name Req Complete",
        "Encrypt Change",
        "Change Connection Link Key Complete",
        "Master Link Key Complete",
        "Read Remote Supported Features",
        "Read Remote Ver Info Complete",
        "QoS Setup Complete",
        "Command Complete",
        "Command Status",
        "Hardware Error",
        "Flush Occurred",
        "Role Change",
        "Number of Completed Packets",
        "Mode Change",
        "Return Link Keys",
        "PIN Code Request",
        "Link Key Request",
        "Link Key Notification",
        "Loopback Command",
        "Data Buffer Overflow",
        "Max Slots Change",
        "Read Clock Offset Complete",
        "Connection Packet Type Changed",
        "QoS Violation",
        "Page Scan Mode Change",
        "Page Scan Repetition Mode Change",
        "Flow Specification Complete",
        "Inquiry Result with RSSI",
        "Read Remote Extended Features",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Synchronous Connect Complete",
        "Synchronous Connect Changed",
        "Sniff Subrate",
        "Extended Inquiry Result",
        "Encryption Key Refresh Complete",
        "IO Capability Request",
        "IO Capability Response",
        "User Confirmation Request",
        "User Passkey Request",
        "Remote OOB Data Request",
        "Simple Pairing Complete",
        "Unknown",
        "Link Supervision Timeout Change",
        "Enhanced Flush Complete",
        "Unknown",
        "User Passkey Notification",
        "Keypress Notification",
        "Remote Host Supported Features Notification",
        "LE Meta Event",
        "Unknown",
        "Physical Link Complete",
        "Channel Selected",
        "Disconnection Physical Link Complete",
        "Physical Link Loss Early Warning",
        "Physical Link Recovery",
        "Logical Link Complete",
        "Disconnection Logical Link Complete",
        "Flow Spec Modify Complete",
        "Number Of Completed Data Blocks",
        "AMP Start Test",
        "AMP Test End",
        "AMP Receiver Report",
        "Short Range Mode Change Complete",
        "AMP Status Change",
};

#define CMD_LINKCTL_NUM 60
static char *cmd_linkctl_str[CMD_LINKCTL_NUM + 1] = {
        "Unknown",
        "Inquiry",
        "Inquiry Cancel",
        "Periodic Inquiry Mode",
        "Exit Periodic Inquiry Mode",
        "Create Connection",
        "Disconnect",
        "Add SCO Connection",
        "Create Connection Cancel",
        "Accept Connection Request",
        "Reject Connection Request",
        "Link Key Request Reply",
        "Link Key Request Negative Reply",
        "PIN Code Request Reply",
        "PIN Code Request Negative Reply",
        "Change Connection Packet Type",
        "Unknown",
        "Authentication Requested",
        "Unknown",
        "Set Connection Encryption",
        "Unknown",
        "Change Connection Link Key",
        "Unknown",
        "Master Link Key",
        "Unknown",
        "Remote Name Request",
        "Remote Name Request Cancel",
        "Read Remote Supported Features",
        "Read Remote Extended Features",
        "Read Remote Version Information",
        "Unknown",
        "Read Clock Offset",
        "Read LMP Handle",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Setup Synchronous Connection",
        "Accept Synchronous Connection",
        "Reject Synchronous Connection",
        "IO Capability Request Reply",
        "User Confirmation Request Reply",
        "User Confirmation Request Negative Reply",
        "User Passkey Request Reply",
        "User Passkey Request Negative Reply",
        "Remote OOB Data Request Reply",
        "Unknown",
        "Unknown",
        "Remote OOB Data Request Negative Reply",
        "IO Capability Request Negative Reply",
        "Create Physical Link",
        "Accept Physical Link",
        "Disconnect Physical Link",
        "Create Logical Link",
        "Accept Logical Link",
        "Disconnect Logical Link",
        "Logical Link Cancel",
        "Flow Spec Modify",
};

#define CMD_LINKPOL_NUM 17
static char *cmd_linkpol_str[CMD_LINKPOL_NUM + 1] = {
        "Unknown",
        "Hold Mode",
        "Unknown",
        "Sniff Mode",
        "Exit Sniff Mode",
        "Park State",
        "Exit Park State",
        "QoS Setup",
        "Unknown",
        "Role Discovery",
        "Unknown",
        "Switch Role",
        "Read Link Policy Settings",
        "Write Link Policy Settings",
        "Read Default Link Policy Settings",
        "Write Default Link Policy Settings",
        "Flow Specification",
        "Sniff Subrating",
};

#define CMD_HOSTCTL_NUM 109
static char *cmd_hostctl_str[CMD_HOSTCTL_NUM + 1] = {
        "Unknown",
        "Set Event Mask",
        "Unknown",
        "Reset",
        "Unknown",
        "Set Event Filter",
        "Unknown",
        "Unknown",
        "Flush",
        "Read PIN Type ",
        "Write PIN Type",
        "Create New Unit Key",
        "Unknown",
        "Read Stored Link Key",
        "Unknown",
        "Unknown",
        "Unknown",
        "Write Stored Link Key",
        "Delete Stored Link Key",
        "Write Local Name",
        "Read Local Name",
        "Read Connection Accept Timeout",
        "Write Connection Accept Timeout",
        "Read Page Timeout",
        "Write Page Timeout",
        "Read Scan Enable",
        "Write Scan Enable",
        "Read Page Scan Activity",
        "Write Page Scan Activity",
        "Read Inquiry Scan Activity",
        "Write Inquiry Scan Activity",
        "Read Authentication Enable",
        "Write Authentication Enable",
        "Read Encryption Mode",
        "Write Encryption Mode",
        "Read Class of Device",
        "Write Class of Device",
        "Read Voice Setting",
        "Write Voice Setting",
        "Read Automatic Flush Timeout",
        "Write Automatic Flush Timeout",
        "Read Num Broadcast Retransmissions",
        "Write Num Broadcast Retransmissions",
        "Read Hold Mode Activity ",
        "Write Hold Mode Activity",
        "Read Transmit Power Level",
        "Read Synchronous Flow Control Enable",
        "Write Synchronous Flow Control Enable",
        "Unknown",
        "Set Host Controller To Host Flow Control",
        "Unknown",
        "Host Buffer Size",
        "Unknown",
        "Host Number of Completed Packets",
        "Read Link Supervision Timeout",
        "Write Link Supervision Timeout",
        "Read Number of Supported IAC",
        "Read Current IAC LAP",
        "Write Current IAC LAP",
        "Read Page Scan Period Mode",
        "Write Page Scan Period Mode",
        "Read Page Scan Mode",
        "Write Page Scan Mode",
        "Set AFH Host Channel Classification",
        "Unknown",
        "Unknown",
        "Read Inquiry Scan Type",
        "Write Inquiry Scan Type",
        "Read Inquiry Mode",
        "Write Inquiry Mode",
        "Read Page Scan Type",
        "Write Page Scan Type",
        "Read AFH Channel Assessment Mode",
        "Write AFH Channel Assessment Mode",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Unknown",
        "Read Extended Inquiry Response",
        "Write Extended Inquiry Response",
        "Refresh Encryption Key",
        "Unknown",
        "Read Simple Pairing Mode",
        "Write Simple Pairing Mode",
        "Read Local OOB Data",
        "Read Inquiry Response Transmit Power Level",
        "Write Inquiry Transmit Power Level",
        "Read Default Erroneous Data Reporting",
        "Write Default Erroneous Data Reporting",
        "Unknown",
        "Unknown",
        "Unknown",
        "Enhanced Flush",
        "Unknown",
        "Read Logical Link Accept Timeout",
        "Write Logical Link Accept Timeout",
        "Set Event Mask Page 2",
        "Read Location Data",
        "Write Location Data",
        "Read Flow Control Mode",
        "Write Flow Control Mode",
        "Read Enhanced Transmit Power Level",
        "Read Best Effort Flush Timeout",
        "Write Best Effort Flush Timeout",
        "Short Range Mode",
        "Read LE Host Supported",
        "Write LE Host Supported",
};

#define CMD_INFO_NUM 10
static char *cmd_info_str[CMD_INFO_NUM + 1] = {
        "Unknown",
        "Read Local Version Information",
        "Read Local Supported Commands",
        "Read Local Supported Features",
        "Read Local Extended Features",
        "Read Buffer Size",
        "Unknown",
        "Read Country Code",
        "Unknown",
        "Read BD ADDR",
        "Read Data Block Size",
};

#define CMD_STATUS_NUM 11
static char *cmd_status_str[CMD_STATUS_NUM + 1] = {
        "Unknown",
        "Read Failed Contact Counter",
        "Reset Failed Contact Counter",
        "Read Link Quality",
        "Unknown",
        "Read RSSI",
        "Read AFH Channel Map",
        "Read Clock",
        "Read Encryption Key Size",
        "Read Local AMP Info",
        "Read Local AMP ASSOC",
        "Write Remote AMP ASSOC"
};

#define CMD_TESTING_NUM 4
static char *cmd_testing_str[CMD_TESTING_NUM + 1] = {
        "Unknown",
        "Read Loopback Mode",
        "Write Loopback Mode",
        "Enable Device Under Test mode",
        "Unknown",
};

#define CMD_LE_NUM 31
static char *cmd_le_str[CMD_LE_NUM + 1] = {
        "Unknown",
        "LE Set Event Mask",
        "LE Read Buffer Size",
        "LE Read Local Supported Features",
        "Unknown",
        "LE Set Random Address",
        "LE Set Advertising Parameters",
        "LE Read Advertising Channel Tx Power",
        "LE Set Advertising Data",
        "LE Set Scan Response Data",
        "LE Set Advertise Enable",
        "LE Set Scan Parameters",
        "LE Set Scan Enable",
        "LE Create Connection",
        "LE Create Connection Cancel",
        "LE Read White List Size",
        "LE Clear White List",
        "LE Add Device To White List",
        "LE Remove Device From White List",
        "LE Connection Update",
        "LE Set Host Channel Classification",
        "LE Read Channel Map",
        "LE Read Remote Used Features",
        "LE Encrypt",
        "LE Rand",
        "LE Start Encryption",
        "LE Long Term Key Request Reply",
        "LE Long Term Key Request Negative Reply",
        "LE Read Supported States",
        "LE Receiver Test",
        "LE Transmitter Test",
        "LE Test End",
};

static char *opcode2str(uint16_t opcode)
{
        uint16_t ogf = cmd_opcode_ogf(opcode);
        uint16_t ocf = cmd_opcode_ocf(opcode);
        char *cmd;

        switch (ogf) {
        case OGF_INFO_PARAM:
                if (ocf <= CMD_INFO_NUM)
                        cmd = cmd_info_str[ocf];
                else
                        cmd = "Unknown";
                break;

        case OGF_HOST_CTL:
                if (ocf <= CMD_HOSTCTL_NUM)
                        cmd = cmd_hostctl_str[ocf];
                else
                        cmd = "Unknown";
                break;

        case OGF_LINK_CTL:
                if (ocf <= CMD_LINKCTL_NUM)
                        cmd = cmd_linkctl_str[ocf];
                else
                        cmd = "Unknown";
                break;

        case OGF_LINK_POLICY:
                if (ocf <= CMD_LINKPOL_NUM)
                        cmd = cmd_linkpol_str[ocf];
                else
                        cmd = "Unknown";
                break;

        case OGF_STATUS_PARAM:
                if (ocf <= CMD_STATUS_NUM)
                        cmd = cmd_status_str[ocf];
                else
                        cmd = "Unknown";
                break;

        case OGF_TESTING_CMD:
                if (ocf <= CMD_TESTING_NUM)
                        cmd = cmd_testing_str[ocf];
                else
                        cmd = "Unknown";
                break;

        case OGF_LE_CTL:
                if (ocf <= CMD_LE_NUM)
                        cmd = cmd_le_str[ocf];
                else
                        cmd = "Unknown";
                break;

        case OGF_VENDOR_CMD:
                cmd = "Vendor";
                break;

        default:
                cmd = "Unknown";
                break;
        }

        return cmd;
}
        python-version: ["3.9", "3.10", "3.11"]

    steps: #ifndef __btmodule_h__
#define __btmodule_h__

#include "Python.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The object holding a socket.  It holds some extra information,
   like the address family, which is used to decode socket address
   arguments properly. */

typedef struct {
        PyObject_HEAD
        int sock_fd;        /* Socket file descriptor */
        int sock_family;        /* Address family, always AF_BLUETOOTH */
        int sock_type;                /* Socket type, e.g., SOCK_STREAM */
        int sock_proto;                /* Protocol type, e.g., BTPROTO_L2CAP */
        PyObject *(*errorhandler)(void); /* Error handler; checks
                                            errno, returns NULL and
                                            sets a Python exception */
        double sock_timeout;                 /* Operation timeout in seconds;
                                            0.0 means non-blocking */

    int is_listening_socket;    // XXX this is a hack to make 
                                // sdp_advertise_service easier

    uint32_t sdp_record_handle; // if it's a listening socket and advertised 
                                // via SDP, this is the SDP handle
    sdp_session_t *sdp_session;
} PySocketSockObject;


#ifdef __cplusplus
}
#endif

extern PyObject *bluetooth_error;

#endif // __btmodule_h__
    - uses: actions/checkout@v4
    - name: Set up Python ${{ #include "Python.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <errno.h>
#include <netdb.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include "btmodule.h"
#include "btsdp.h"

extern PyTypeObject sock_type;
extern int getsockaddrlen(PySocketSockObject *s, socklen_t *len_ret);
extern PyObject *set_error(void);
extern int str2uuid( const char *uuid_str, uuid_t *uuid );
extern void uuid2str( const uuid_t *uuid, char *dest );

// =================== utility functions =====================

static void 
dict_set_strings(PyObject *dict, const char *key, const char *val)
{
    PyObject *valobj;
    valobj = PyUnicode_FromString( val );
    PyDict_SetItemString( dict, key, valobj );
    Py_DECREF( valobj );
}

static void 
dict_set_str_long(PyObject *dict, const char *key, long val)
{
    PyObject *valobj;
    valobj = PyLong_FromLong(val);
    PyDict_SetItemString( dict, key, valobj );
    Py_DECREF( valobj );
}

PyDoc_STRVAR(sess_doc, 
"SDPSession()\n\
\n\
TODO\n\
");

/* 
 * utility function to perform an SDP search on a connected session.  Builds 
 * and returns a python list of dictionaries.  Each dictionary represents a 
 * service record match
 */
static PyObject *
do_search( sdp_session_t *sess, uuid_t *uuid )
{
    sdp_list_t *response_list = NULL, *attrid_list, *search_list, *r;
    uint32_t range = 0x0000ffff;
    char buf[1024] = { 0 };
    int err = 0;
    PyObject *result = 0;

        PyObject *rtn_list = PyList_New(0);
    if( ! rtn_list ) return 0;
    search_list = sdp_list_append( 0, uuid );
    attrid_list = sdp_list_append( 0, &range );

    // perform the search
    Py_BEGIN_ALLOW_THREADS
    err = sdp_service_search_attr_req( sess, search_list, \
            SDP_ATTR_REQ_RANGE, attrid_list, &response_list);
    Py_END_ALLOW_THREADS
    if( err ) {
        PyErr_SetFromErrno( bluetooth_error );
        result = 0;
        goto cleanup;
    }

    // parse the results (ewww....)

    // go through each of the service records
    for (r = response_list; r; r = r->next ) {
        PyObject *dict = PyDict_New();
        sdp_record_t *rec = (sdp_record_t*) r->data;
        sdp_list_t *proto_list = NULL,
                   *svc_class_list = NULL,
                   *profile_list = NULL;
        PyObject *py_class_list = NULL, *py_profile_list = NULL; 
        uuid_t service_id = { 0 };

        if( ! dict ) return 0;

        // initialize service class list
        py_class_list = PyList_New(0);
        if( ! py_class_list ) return 0;
        PyDict_SetItemString( dict, "service-classes", py_class_list );
        Py_DECREF( py_class_list );

        // initialize profile list
        py_profile_list = PyList_New(0);
        if( ! py_profile_list ) return 0;
        PyDict_SetItemString( dict, "profiles", py_profile_list );
        Py_DECREF( py_profile_list );

        // set service name
        if( ! sdp_get_service_name( rec, buf, sizeof(buf) ) ) {
            dict_set_strings( dict, "name", buf );
            memset(buf, 0, sizeof( buf ) );
        } else {
            PyDict_SetItemString( dict, "name", Py_None );
        }

        // set service description
        if( ! sdp_get_service_desc( rec, buf, sizeof(buf) ) ) {
            dict_set_strings( dict, "description", buf );
            memset(buf, 0, sizeof( buf ) );
        } else {
            PyDict_SetItemString( dict, "description", Py_None );
        }

        // set service provider name
        if( ! sdp_get_provider_name( rec, buf, sizeof(buf) ) ) {
            dict_set_strings( dict, "provider", buf );
            memset(buf, 0, sizeof( buf ) );
        } else {
            PyDict_SetItemString( dict, "provider", Py_None );
        }

        // set service id
        if( ! sdp_get_service_id( rec, &service_id ) ) {
            uuid2str( &service_id, buf );
            dict_set_strings( dict, "service-id", buf );
            memset(buf, 0, sizeof( buf ) );
        } else {
            PyDict_SetItemString( dict, "service-id", Py_None );
        }

        // get a list of the protocol sequences
        if( sdp_get_access_protos( rec, &proto_list ) == 0 ) {
            sdp_list_t *p = proto_list;
            int port;

            if( ( port = sdp_get_proto_port( p, RFCOMM_UUID ) ) != 0 ) {
                dict_set_strings( dict, "protocol", "RFCOMM" );
                dict_set_str_long( dict, "port", port );
            } else if ( (port = sdp_get_proto_port( p, L2CAP_UUID ) ) != 0 ) {
                dict_set_strings( dict, "protocol", "L2CAP" );
                dict_set_str_long( dict, "port", port );
            } else {
                dict_set_strings( dict, "protocol", "UNKNOWN" );
                PyDict_SetItemString( dict, "port", Py_None );
            }

            // sdp_get_access_protos allocates data on the heap for the
            // protocol list, so we need to free the results...
            for( ; p ; p = p->next ) {
                sdp_list_free( (sdp_list_t*)p->data, 0 );
            }
            sdp_list_free( proto_list, 0 );
        } else {
            PyDict_SetItemString( dict, "protocol", Py_None );
            PyDict_SetItemString( dict, "port", Py_None );
        }

        // get a list of the service classes
        if( sdp_get_service_classes( rec, &svc_class_list ) == 0 ) {
            sdp_list_t *iter;
            for( iter = svc_class_list; iter != NULL; iter = iter->next ) {
                PyObject *pystr;
                char uuid_str[40] = { 0 };

                uuid2str( (uuid_t*)iter->data, uuid_str );
                pystr = PyUnicode_FromString( uuid_str );
                PyList_Append( py_class_list, pystr );
                Py_DECREF( pystr );
            }

            sdp_list_free( svc_class_list, free );
        }

        // get a list of the profiles
        if( sdp_get_profile_descs( rec, &profile_list ) == 0 ) {
            sdp_list_t *iter;
            for( iter = profile_list; iter != NULL; iter = iter->next ) {
                PyObject *tuple, *py_uuid, *py_version;
                sdp_profile_desc_t *desc = (sdp_profile_desc_t*)iter->data;
                char uuid_str[40] = { 0 };

                uuid2str( &desc->uuid, uuid_str );
                py_uuid = PyUnicode_FromString( uuid_str );
                py_version = PyLong_FromLong( desc->version );

                tuple = PyTuple_New( 2 );
                PyList_Append( py_profile_list, tuple );
                Py_DECREF( tuple );

                PyTuple_SetItem( tuple, 0, py_uuid );
                PyTuple_SetItem( tuple, 1, py_version );
//                Py_DECREF( py_uuid );
//                Py_DECREF( py_version );
            }
            sdp_list_free( profile_list, free );
        }

        PyList_Append( rtn_list, dict );
        Py_DECREF( dict );

        sdp_record_free( rec );
    }

    result = rtn_list;

cleanup:
    sdp_list_free( response_list, 0 );
    sdp_list_free( search_list, 0 );
    sdp_list_free( attrid_list, 0 );
    return result;
}

// ==================== SDPSession methods ===========================

// connect
static PyObject *
sess_connect(PySDPSessionObject *s, PyObject *args, PyObject *kwds)
{
    bdaddr_t src; 
    bdaddr_t dst; 
    char *dst_buf = "localhost";
    uint32_t flags = SDP_RETRY_IF_BUSY;

        static char *keywords[] = {"target", 0};

    bacpy( &src, BDADDR_ANY );
    bacpy( &dst, BDADDR_LOCAL );

    if( s->session != NULL ) {
        sdp_close( s->session );
    }

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", keywords, &dst_buf))
        return NULL;

    if( strncmp( dst_buf, "localhost", 18 ) != 0 ) {
        str2ba( dst_buf, &dst );
    } else {
        // XXX
    }

        Py_BEGIN_ALLOW_THREADS
    s->session = sdp_connect( &src, &dst, flags );
        Py_END_ALLOW_THREADS
    if( s->session == NULL ) 
        return PyErr_SetFromErrno( bluetooth_error );

    Py_RETURN_NONE;
}
PyDoc_STRVAR(sess_connect_doc,
"connect( dest = \"localhost\" )\n\
\n\
Connects the SDP session to the SDP server specified by dest.  If the\n\
session was already connected, it's closed first.\n\
\n\
dest specifies the bluetooth address of the server to connect to.  Special\n\
case is \"localhost\"\n\
\n\
Raises _bluetooth.error if something goes wrong");

// close
static PyObject *
sess_close(PySDPSessionObject *s)
{
    if( s->session != NULL ) {
        Py_BEGIN_ALLOW_THREADS
        sdp_close( s->session );
        Py_END_ALLOW_THREADS
        s->session = NULL;
    }
    Py_RETURN_NONE;
}
PyDoc_STRVAR(sess_close_doc,
"close()\n\
\n\
Closes the connection with the SDP server.  No effect if a session is not open.");

// fileno
static PyObject *
sess_fileno(PySDPSessionObject *s)
{
        return PyLong_FromLong((long) s->session->sock);
}
PyDoc_STRVAR(sess_fileno_doc,
"fileno() -> integer\n\
\n\
Return the integer file descriptor of the socket.\n\
You can use this for direct communication with the SDP server.");

// search
static PyObject *
sess_search(PySDPSessionObject *s, PyObject *args, PyObject *kwds)
{
    char *uuid_str = 0;
    uuid_t uuid = { 0 };
    PyObject *result = 0;

    if (!PyArg_ParseTuple(args, "s", &uuid_str)) return NULL;

    // convert the UUID string into a uuid_t
    if( ! str2uuid( uuid_str, &uuid ) ) {
        PyErr_SetString(PyExc_ValueError, "invalid UUID!");
        return NULL;
    }

    // make sure the SDP session is open
    if( ! s->session ) {
        PyErr_SetString( bluetooth_error, "SDP session is not active!" );
        return 0;
     }

    // perform the search
    result = do_search( s->session, &uuid );

    return result;
}
PyDoc_STRVAR(sess_search_doc,
"search( UUID )\n\
\n\
Searches for a service record with the specified UUID.  If no match is found,\n\
returns None.  Otherwise, returns a dictionary\n\
\n\
UUID must be in the form \"XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX\", \n\
where each X is a hexadecimal digit.");

// browse
static PyObject *
sess_browse(PySDPSessionObject *s)
{
    uuid_t uuid = { 0 };
    PyObject *result = 0;

    // convert the UUID string into a uuid_t
    sdp_uuid16_create(&uuid, PUBLIC_BROWSE_GROUP);

    // make sure the SDP session is open
    if( ! s->session ) {
        PyErr_SetString( bluetooth_error, "SDP session is not active!" );
        return 0;
     }

    // perform the search
    result = do_search( s->session, &uuid );

    return result;
}
PyDoc_STRVAR(sess_browse_doc,
"browse()\n\
\n\
Browses all services advertised by connected SDP session");

static PyMethodDef sess_methods[] = {
    { "search", (PyCFunction) sess_search, METH_VARARGS, 
        sess_search_doc },
    { "browse", (PyCFunction) sess_browse, METH_NOARGS, 
        sess_browse_doc },
    { "fileno", (PyCFunction)sess_fileno, METH_NOARGS, 
        sess_fileno_doc },
    { "connect", (PyCFunction) sess_connect, METH_VARARGS | METH_KEYWORDS, 
        sess_connect_doc },
    { "close", (PyCFunction)sess_close, METH_NOARGS, 
        sess_close_doc },
    {NULL, NULL}
};


/* =============== object maintenance =============== */

/* Deallocate a socket object in response to the last Py_DECREF().
   First close the file description. */

static void
sess_dealloc(PySDPSessionObject *s)
{
    if(s->session != NULL) {
        sdp_close( s->session );
        s->session = NULL;
    }
    Py_TYPE(s)->tp_free((PyObject *)s);
}

static PyObject *
sess_repr(PySDPSessionObject *s)
{
    char buf[512];
    if (s->session != NULL) {
        PyOS_snprintf( buf, sizeof(buf), "<SDP Session object - connected>");
    } else { 
        PyOS_snprintf( buf, sizeof(buf), "<SDP Session object - unconnected>");
    }
    return PyUnicode_FromString(buf);
}


/* Create a new, uninitialized socket object. */

static PyObject *
sess_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
        PyObject *newsess;

        newsess = type->tp_alloc(type, 0);
        if (newsess != NULL) {
        ((PySDPSessionObject *)newsess)->session = NULL;
        }
        return newsess;
}


/* Initialize a new socket object. */

/*ARGSUSED*/
static int
sess_initobj(PyObject *self, PyObject *args, PyObject *kwds)
{
    PySDPSessionObject *s = (PySDPSessionObject *)self;
    s->errorhandler = &set_error;

        /* From now on, ignore SIGPIPE and let the error checking
           do the work. */
#ifdef SIGPIPE
        (void) signal(SIGPIPE, SIG_IGN);
#endif

        return 0;
}


/* Type object for socket objects. */

PyTypeObject sdp_session_type = {
    PyVarObject_HEAD_INIT(NULL, 0)   /* Must fill in type value later */
        "_bluetooth.SDPSession",                        /* tp_name */
        sizeof(PySDPSessionObject),                /* tp_basicsize */
        0,                                        /* tp_itemsize */
        (destructor)sess_dealloc,                /* tp_dealloc */
        0,                                        /* tp_print */
        0,                                        /* tp_getattr */
        0,                                        /* tp_setattr */
        0,                                        /* tp_compare */
        (reprfunc)sess_repr,                        /* tp_repr */
        0,                                        /* tp_as_number */
        0,                                        /* tp_as_sequence */
        0,                                        /* tp_as_mapping */
        0,                                        /* tp_hash */
        0,                                        /* tp_call */
        0,                                        /* tp_str */
        PyObject_GenericGetAttr,                /* tp_getattro */
        0,                                        /* tp_setattro */
        0,                                        /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
        sess_doc,                                /* tp_doc */
        0,                                        /* tp_traverse */
        0,                                        /* tp_clear */
        0,                                        /* tp_richcompare */
        0,                                        /* tp_weaklistoffset */
        0,                                        /* tp_iter */
        0,                                        /* tp_iternext */
        sess_methods,                                /* tp_methods */
        0,                                        /* tp_members */
        0,                                        /* tp_getset */
        0,                                        /* tp_base */
        0,                                        /* tp_dict */
        0,                                        /* tp_descr_get */
        0,                                        /* tp_descr_set */
        0,                                        /* tp_dictoffset */
        sess_initobj,                                /* tp_init */
        PyType_GenericAlloc,                        /* tp_alloc */
        sess_new,                                /* tp_new */
        PyObject_Del,                                /* tp_free */
}; 
      uses: actions/setup-python@v3
      with: #ifndef __pybluez_sdp_h__
#define __pybluez_sdp_h__

#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

typedef struct {
    PyObject_HEAD
    sdp_session_t *session;

        PyObject *(*errorhandler)(void); /* Error handler; checks
                                            errno, returns NULL and
                                            sets a Python exception */
} PySDPSessionObject;

extern PyTypeObject sdp_session_type;

#endif
        python-version: ${{ // Header file providing new functions of the Python C API to old Python
// versions.
//
// File distributed under the MIT license.
// Copyright Contributors to the pythoncapi_compat project.
//
// Homepage:
// https://github.com/pythoncapi/pythoncapi_compat
//
// Latest version:
// https://raw.githubusercontent.com/pythoncapi/pythoncapi_compat/master/pythoncapi_compat.h
//
// SPDX-License-Identifier: MIT

#ifndef PYTHONCAPI_COMPAT
#define PYTHONCAPI_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "frameobject.h"          // PyFrameObject, PyFrame_GetBack()


// Compatibility with Visual Studio 2013 and older which don't support
// the inline keyword in C (only in C++): use __inline instead.
#if (defined(_MSC_VER) && _MSC_VER < 1900 \
     && !defined(__cplusplus) && !defined(inline))
#  define inline __inline
#  define PYTHONCAPI_COMPAT_MSC_INLINE
   // These two macros are undefined at the end of this file
#endif


// Cast argument to PyObject* type.
#ifndef _PyObject_CAST
#  define _PyObject_CAST(op) ((PyObject*)(op))
#endif
#ifndef _PyObject_CAST_CONST
#  define _PyObject_CAST_CONST(op) ((const PyObject*)(op))
#endif


// bpo-42262 added Py_NewRef() to Python 3.10.0a3
#if PY_VERSION_HEX < 0x030A00A3 && !defined(Py_NewRef)
static inline PyObject* _Py_NewRef(PyObject *obj)
{
    Py_INCREF(obj);
    return obj;
}
#define Py_NewRef(obj) _Py_NewRef(_PyObject_CAST(obj))
#endif


// bpo-42262 added Py_XNewRef() to Python 3.10.0a3
#if PY_VERSION_HEX < 0x030A00A3 && !defined(Py_XNewRef)
static inline PyObject* _Py_XNewRef(PyObject *obj)
{
    Py_XINCREF(obj);
    return obj;
}
#define Py_XNewRef(obj) _Py_XNewRef(_PyObject_CAST(obj))
#endif


// See https://bugs.python.org/issue42522
#if !defined(_Py_StealRef)
static inline PyObject* __Py_StealRef(PyObject *obj)
{
    Py_DECREF(obj);
    return obj;
}
#define _Py_StealRef(obj) __Py_StealRef(_PyObject_CAST(obj))
#endif


// See https://bugs.python.org/issue42522
#if !defined(_Py_XStealRef)
static inline PyObject* __Py_XStealRef(PyObject *obj)
{
    Py_XDECREF(obj);
    return obj;
}
#define _Py_XStealRef(obj) __Py_XStealRef(_PyObject_CAST(obj))
#endif


// bpo-39573 added Py_SET_REFCNT() to Python 3.9.0a4
#if PY_VERSION_HEX < 0x030900A4 && !defined(Py_SET_REFCNT)
static inline void _Py_SET_REFCNT(PyObject *ob, Py_ssize_t refcnt)
{
    ob->ob_refcnt = refcnt;
}
#define Py_SET_REFCNT(ob, refcnt) _Py_SET_REFCNT(_PyObject_CAST(ob), refcnt)
#endif


// Py_SETREF() and Py_XSETREF() were added to Python 3.5.2.
// It is excluded from the limited C API.
#if (PY_VERSION_HEX < 0x03050200 && !defined(Py_SETREF)) && !defined(Py_LIMITED_API)
#define Py_SETREF(op, op2)                      \
    do {                                        \
        PyObject *_py_tmp = _PyObject_CAST(op); \
        (op) = (op2);                           \
        Py_DECREF(_py_tmp);                     \
    } while (0)

#define Py_XSETREF(op, op2)                     \
    do {                                        \
        PyObject *_py_tmp = _PyObject_CAST(op); \
        (op) = (op2);                           \
        Py_XDECREF(_py_tmp);                    \
    } while (0)
#endif


// bpo-43753 added Py_Is(), Py_IsNone(), Py_IsTrue() and Py_IsFalse()
// to Python 3.10.0b1.
#if PY_VERSION_HEX < 0x030A00B1 && !defined(Py_Is)
#  define Py_Is(x, y) ((x) == (y))
#endif
#if PY_VERSION_HEX < 0x030A00B1 && !defined(Py_IsNone)
#  define Py_IsNone(x) Py_Is(x, Py_None)
#endif
#if PY_VERSION_HEX < 0x030A00B1 && !defined(Py_IsTrue)
#  define Py_IsTrue(x) Py_Is(x, Py_True)
#endif
#if PY_VERSION_HEX < 0x030A00B1 && !defined(Py_IsFalse)
#  define Py_IsFalse(x) Py_Is(x, Py_False)
#endif


// bpo-39573 added Py_SET_TYPE() to Python 3.9.0a4
#if PY_VERSION_HEX < 0x030900A4 && !defined(Py_SET_TYPE)
static inline void
_Py_SET_TYPE(PyObject *ob, PyTypeObject *type)
{
    ob->ob_type = type;
}
#define Py_SET_TYPE(ob, type) _Py_SET_TYPE(_PyObject_CAST(ob), type)
#endif


// bpo-39573 added Py_SET_SIZE() to Python 3.9.0a4
#if PY_VERSION_HEX < 0x030900A4 && !defined(Py_SET_SIZE)
static inline void
_Py_SET_SIZE(PyVarObject *ob, Py_ssize_t size)
{
    ob->ob_size = size;
}
#define Py_SET_SIZE(ob, size) _Py_SET_SIZE((PyVarObject*)(ob), size)
#endif


// bpo-40421 added PyFrame_GetCode() to Python 3.9.0b1
#if PY_VERSION_HEX < 0x030900B1
static inline PyCodeObject*
PyFrame_GetCode(PyFrameObject *frame)
{
    assert(frame != NULL);
    assert(frame->f_code != NULL);
    return (PyCodeObject*)Py_NewRef(frame->f_code);
}
#endif

static inline PyCodeObject*
_PyFrame_GetCodeBorrow(PyFrameObject *frame)
{
    return (PyCodeObject *)_Py_StealRef(PyFrame_GetCode(frame));
}


// bpo-40421 added PyFrame_GetCode() to Python 3.9.0b1
#if PY_VERSION_HEX < 0x030900B1 && !defined(PYPY_VERSION)
static inline PyFrameObject*
PyFrame_GetBack(PyFrameObject *frame)
{
    assert(frame != NULL);
    return (PyFrameObject*)Py_XNewRef(frame->f_back);
}
#endif

#if !defined(PYPY_VERSION)
static inline PyFrameObject*
_PyFrame_GetBackBorrow(PyFrameObject *frame)
{
    return (PyFrameObject *)_Py_XStealRef(PyFrame_GetBack(frame));
}
#endif


// bpo-39947 added PyThreadState_GetInterpreter() to Python 3.9.0a5
#if PY_VERSION_HEX < 0x030900A5
static inline PyInterpreterState *
PyThreadState_GetInterpreter(PyThreadState *tstate)
{
    assert(tstate != NULL);
    return tstate->interp;
}
#endif


// bpo-40429 added PyThreadState_GetFrame() to Python 3.9.0b1
#if PY_VERSION_HEX < 0x030900B1 && !defined(PYPY_VERSION)
static inline PyFrameObject*
PyThreadState_GetFrame(PyThreadState *tstate)
{
    assert(tstate != NULL);
    return (PyFrameObject *)Py_XNewRef(tstate->frame);
}
#endif

#if !defined(PYPY_VERSION)
static inline PyFrameObject*
_PyThreadState_GetFrameBorrow(PyThreadState *tstate)
{
    return (PyFrameObject *)_Py_XStealRef(PyThreadState_GetFrame(tstate));
}
#endif


// bpo-39947 added PyInterpreterState_Get() to Python 3.9.0a5
#if PY_VERSION_HEX < 0x030900A5
static inline PyInterpreterState *
PyInterpreterState_Get(void)
{
    PyThreadState *tstate;
    PyInterpreterState *interp;

    tstate = PyThreadState_GET();
    if (tstate == NULL) {
        Py_FatalError("GIL released (tstate is NULL)");
    }
    interp = tstate->interp;
    if (interp == NULL) {
        Py_FatalError("no current interpreter");
    }
    return interp;
}
#endif


// bpo-39947 added PyInterpreterState_Get() to Python 3.9.0a6
#if 0x030700A1 <= PY_VERSION_HEX && PY_VERSION_HEX < 0x030900A6 && !defined(PYPY_VERSION)
static inline uint64_t
PyThreadState_GetID(PyThreadState *tstate)
{
    assert(tstate != NULL);
    return tstate->id;
}
#endif


// bpo-37194 added PyObject_CallNoArgs() to Python 3.9.0a1
#if PY_VERSION_HEX < 0x030900A1
static inline PyObject*
PyObject_CallNoArgs(PyObject *func)
{
    return PyObject_CallFunctionObjArgs(func, NULL);
}
#endif


// bpo-39245 made PyObject_CallOneArg() public (previously called
// _PyObject_CallOneArg) in Python 3.9.0a4
#if PY_VERSION_HEX < 0x030900A4
static inline PyObject*
PyObject_CallOneArg(PyObject *func, PyObject *arg)
{
    return PyObject_CallFunctionObjArgs(func, arg, NULL);
}
#endif


// bpo-1635741 added PyModule_AddObjectRef() to Python 3.10.0a3
#if PY_VERSION_HEX < 0x030A00A3
static inline int
PyModule_AddObjectRef(PyObject *module, const char *name, PyObject *value)
{
    Py_XINCREF(value);
    int res = PyModule_AddObject(module, name, value);
    if (res < 0) {
        Py_XDECREF(value);
    }
    return res;
}
#endif


// bpo-40024 added PyModule_AddType() to Python 3.9.0a5
#if PY_VERSION_HEX < 0x030900A5
static inline int
PyModule_AddType(PyObject *module, PyTypeObject *type)
{
    const char *name, *dot;

    if (PyType_Ready(type) < 0) {
        return -1;
    }

    // inline _PyType_Name()
    name = type->tp_name;
    assert(name != NULL);
    dot = strrchr(name, '.');
    if (dot != NULL) {
        name = dot + 1;
    }

    return PyModule_AddObjectRef(module, name, (PyObject *)type);
}
#endif


// bpo-40241 added PyObject_GC_IsTracked() to Python 3.9.0a6.
// bpo-4688 added _PyObject_GC_IS_TRACKED() to Python 2.7.0a2.
#if PY_VERSION_HEX < 0x030900A6 && !defined(PYPY_VERSION)
static inline int
PyObject_GC_IsTracked(PyObject* obj)
{
    return (PyObject_IS_GC(obj) && _PyObject_GC_IS_TRACKED(obj));
}
#endif

// bpo-40241 added PyObject_GC_IsFinalized() to Python 3.9.0a6.
// bpo-18112 added _PyGCHead_FINALIZED() to Python 3.4.0 final.
#if PY_VERSION_HEX < 0x030900A6 && PY_VERSION_HEX >= 0x030400F0 && !defined(PYPY_VERSION)
static inline int
PyObject_GC_IsFinalized(PyObject *obj)
{
    return (PyObject_IS_GC(obj) && _PyGCHead_FINALIZED((PyGC_Head *)(obj)-1));
}
#endif


// bpo-39573 added Py_IS_TYPE() to Python 3.9.0a4
#if PY_VERSION_HEX < 0x030900A4 && !defined(Py_IS_TYPE)
static inline int
_Py_IS_TYPE(const PyObject *ob, const PyTypeObject *type) {
    return ob->ob_type == type;
}
#define Py_IS_TYPE(ob, type) _Py_IS_TYPE(_PyObject_CAST_CONST(ob), type)
#endif


// Py_UNUSED() was added to Python 3.4.0b2.
#if PY_VERSION_HEX < 0x030400B2 && !defined(Py_UNUSED)
#  if defined(__GNUC__) || defined(__clang__)
#    define Py_UNUSED(name) _unused_ ## name __attribute__((unused))
#  else
#    define Py_UNUSED(name) _unused_ ## name
#  endif
#endif


#ifdef PYTHONCAPI_COMPAT_MSC_INLINE
#  undef inline
#  undef PYTHONCAPI_COMPAT_MSC_INLINE
#endif

#ifdef __cplusplus
}
#endif
#endif  // PYTHONCAPI_COMPAT }}
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        python -m pip install flake8 pytest
        if [ -f requirements.txt ]; then pip install -r requirements.txt; fi
    - name: Lint with flake8
      run: |
        # stop the build if there are Python syntax errors or undefined names
        flake8 . --count --select=E9,F63,F7,F82 --show-source --statistics
        # exit-zero treats all errors as warnings. The GitHub editor is 127 chars wide
        flake8 . --count --exit-zero --max-complexity=10 --max-line-length=127 --statistics
    - name: Test with pytest
      run: |
        pytest
