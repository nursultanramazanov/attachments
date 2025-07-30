name: Python Package using Conda

on: [push]

jobs: import xml.etree.ElementTree as ET
import binascii
import time
import os
from Library.utils import *

class xmlparser():
    def getresponse(self,input):
        lines=input.split(b"<?xml")
        content = {}
        for line in lines:
            if line==b'':
                continue
            line=b"<?xml"+line
            parser = ET.XMLParser(encoding="utf-8")
            tree = ET.fromstring(line, parser=parser)
            e = ET.ElementTree(tree).getroot()
            for atype in e.findall('response'):
                for field in atype.attrib:
                    content[field]=atype.attrib[field]
        return content

    def getlog(self,input):
        lines=input.split(b"<?xml")
        data = ''
        for line in lines:
            if line==b'':
                continue
            line=b"<?xml"+line
            parser = ET.XMLParser(encoding="utf-8")
            tree = ET.fromstring(line, parser=parser)
            e = ET.ElementTree(tree).getroot()
            for atype in e.findall('log'): 
  build-linux: import binascii
from Library.utils import *

class gpt:
    from enum import Enum
    gpt_header = [
        ('signature', '8s'),
        ('revision', '>I'),
        ('header_size', 'I'),
        ('crc32', 'I'),
        ('reserved', 'I'),
        ('current_lba', 'Q'),
        ('backup_lba', 'Q'),
        ('first_usable_lba', 'Q'),
        ('last_usable_lba', 'Q'),
        ('disk_guid', '16s'),
        ('part_entry_start_lba', 'Q')
    ]

    gpt_partition = [
        ('type', '16s'),
        ('unique', '16s'),
        ('first_lba', 'Q'),
        ('last_lba', 'Q'),
        ('flags', '>Q'),
        ('name', '72s')]

    class efi_type(Enum):
        EFI_UNUSED              = 0x00000000
        EFI_MBR                 = 0x024DEE41
        EFI_SYSTEM              = 0xC12A7328
        EFI_BIOS_BOOT           = 0x21686148
        EFI_IFFS                = 0xD3BFE2DE
        EFI_SONY_BOOT           = 0xF4019732
        EFI_LENOVO_BOOT         = 0xBFBFAFE7
        EFI_MSR                 = 0xE3C9E316
        EFI_BASIC_DATA          = 0xEBD0A0A2
        EFI_LDM_META            = 0x5808C8AA
        EFI_LDM                 = 0xAF9B60A0
        EFI_RECOVERY            = 0xDE94BBA4
        EFI_GPFS                = 0x37AFFC90
        EFI_STORAGE_SPACES      = 0xE75CAF8F
        EFI_HPUX_DATA           = 0x75894C1E
        EFI_HPUX_SERVICE        = 0xE2A1E728
        EFI_LINUX_DAYA          = 0x0FC63DAF
        EFI_LINUX_RAID          = 0xA19D880F
        EFI_LINUX_ROOT32        = 0x44479540
        EFI_LINUX_ROOT64        = 0x4F68BCE3
        EFI_LINUX_ROOT_ARM32    = 0x69DAD710
        EFI_LINUX_ROOT_ARM64    = 0xB921B045
        EFI_LINUX_SWAP          = 0x0657FD6D
        EFI_LINUX_LVM           = 0xE6D6D379
        EFI_LINUX_HOME          = 0x933AC7E1
        EFI_LINUX_SRV           = 0x3B8F8425
        EFI_LINUX_DM_CRYPT      = 0x7FFEC5C9
        EFI_LINUX_LUKS          = 0xCA7D7CCB
        EFI_LINUX_RESERVED      = 0x8DA63339
        EFI_FREEBSD_BOOT        = 0x83BD6B9D
        EFI_FREEBSD_DATA        = 0x516E7CB4
        EFI_FREEBSD_SWAP        = 0x516E7CB5
        EFI_FREEBSD_UFS         = 0x516E7CB6
        EFI_FREEBSD_VINUM       = 0x516E7CB8
        EFI_FREEBSD_ZFS         = 0x516E7CBA
        EFI_OSX_HFS             = 0x48465300
        EFI_OSX_UFS             = 0x55465300
        EFI_OSX_ZFS             = 0x6A898CC3
        EFI_OSX_RAID            = 0x52414944
        EFI_OSX_RAID_OFFLINE    = 0x52414944
        EFI_OSX_RECOVERY        = 0x426F6F74
        EFI_OSX_LABEL           = 0x4C616265
        EFI_OSX_TV_RECOVERY     = 0x5265636F
        EFI_OSX_CORE_STORAGE    = 0x53746F72
        EFI_SOLARIS_BOOT        = 0x6A82CB45
        EFI_SOLARIS_ROOT        = 0x6A85CF4D
        EFI_SOLARIS_SWAP        = 0x6A87C46F
        EFI_SOLARIS_BACKUP      = 0x6A8B642B
        EFI_SOLARIS_USR         = 0x6A898CC3
        EFI_SOLARIS_VAR         = 0x6A8EF2E9
        EFI_SOLARIS_HOME        = 0x6A90BA39
        EFI_SOLARIS_ALTERNATE   = 0x6A9283A5
        EFI_SOLARIS_RESERVED1   = 0x6A945A3B
        EFI_SOLARIS_RESERVED2   = 0x6A9630D1
        EFI_SOLARIS_RESERVED3   = 0x6A980767
        EFI_SOLARIS_RESERVED4   = 0x6A96237F
        EFI_SOLARIS_RESERVED5   = 0x6A8D2AC7
        EFI_NETBSD_SWAP         = 0x49F48D32
        EFI_NETBSD_FFS          = 0x49F48D5A
        EFI_NETBSD_LFS          = 0x49F48D82
        EFI_NETBSD_RAID         = 0x49F48DAA
        EFI_NETBSD_CONCAT       = 0x2DB519C4
        EFI_NETBSD_ENCRYPT      = 0x2DB519EC
        EFI_CHROMEOS_KERNEL     = 0xFE3A2A5D
        EFI_CHROMEOS_ROOTFS     = 0x3CB8E202
        EFI_CHROMEOS_FUTURE     = 0x2E0A753D
        EFI_HAIKU               = 0x42465331
        EFI_MIDNIGHTBSD_BOOT    = 0x85D5E45E
        EFI_MIDNIGHTBSD_DATA    = 0x85D5E45A
        EFI_MIDNIGHTBSD_SWAP    = 0x85D5E45B
        EFI_MIDNIGHTBSD_UFS     = 0x0394EF8B
        EFI_MIDNIGHTBSD_VINUM   = 0x85D5E45C
        EFI_MIDNIGHTBSD_ZFS     = 0x85D5E45D
        EFI_CEPH_JOURNAL        = 0x45B0969E
        EFI_CEPH_ENCRYPT        = 0x45B0969E
        EFI_CEPH_OSD            = 0x4FBD7E29
        EFI_CEPH_ENCRYPT_OSD    = 0x4FBD7E29
        EFI_CEPH_CREATE         = 0x89C57F98
        EFI_CEPH_ENCRYPT_CREATE = 0x89C57F98
        EFI_OPENBSD             = 0x824CC7A0
        EFI_QNX                 = 0xCEF5A9AD
        EFI_PLAN9               = 0xC91818F9
        EFI_VMWARE_VMKCORE      = 0x9D275380
        EFI_VMWARE_VMFS         = 0xAA31E02A
        EFI_VMWARE_RESERVED     = 0x9198EFFC

    def __init__(self, num_part_entries=None, part_entry_size=None, part_entry_start_lba=None, *args, **kwargs):
        self.num_part_entries = num_part_entries
        self.part_entry_size = part_entry_size
        self.part_entry_start_lba = part_entry_start_lba
        if num_part_entries is None:
            self.gpt_header += [('num_part_entries', 'I'),]
            if part_entry_size is None:
                self.gpt_header += [('part_entry_size', 'I'),]


    def parse(self, gptdata, sectorsize=512):
        self.header = read_object(gptdata[sectorsize:sectorsize+0x5C], self.gpt_header)
        self.sectorsize=sectorsize
        if self.header["signature"]!=b"EFI PART":
            print("Invalid or unknown GPT magic.")
            return False
        if self.header["revision"]!=0x100:
            print("Unknown GPT revision.")
            return False
        if self.part_entry_start_lba is not None:
            start = self.part_entry_start_lba
        else:
            start=self.header["part_entry_start_lba"]*sectorsize
        if "part_entry_size" in self.header:
            entrysize=self.header["part_entry_size"]
        else:
            entrysize=self.part_entry_size
        self.partentries=[]

        class partf:
            unique=b""
            first_lba=0
            last_lba=0
            flags=0
            type=b""
            name=b""

        if "num_part_entries" in self.header:
            num_part_entries=self.header["num_part_entries"]
        else:
            num_part_entries=self.num_part_entries

        for idx in range(0,num_part_entries):
            data=gptdata[start+(idx*entrysize):start+(idx*entrysize)+entrysize]
            partentry=read_object(data,self.gpt_partition)
            pa=partf()
            guid1=struct.unpack("<I",partentry["unique"][0:0x4])[0]
            guid2=struct.unpack("<H",partentry["unique"][0x4:0x6])[0]
            guid3=struct.unpack("<H",partentry["unique"][0x6:0x8])[0]
            guid4=struct.unpack("<H",partentry["unique"][0x8:0xA])[0]
            guid5=binascii.hexlify(partentry["unique"][0xA:0x10]).decode('utf-8')
            pa.unique = "{:08x}-{:04x}-{:04x}-{:04x}-{}".format(guid1,guid2,guid3,guid4,guid5)
            pa.sector = partentry["first_lba"]
            pa.sectors = partentry["last_lba"]-partentry["first_lba"]+1
            pa.flags = partentry["flags"]
            type=int(struct.unpack("<I",partentry["type"][0:0x4])[0])
            try:
                pa.type = self.efi_type(type).name
            except:
                pa.type = hex(type)
            pa.name = partentry["name"].replace(b"\x00\x00",b"").decode('utf-16')
            if pa.type=="EFI_UNUSED":
                break
            self.partentries.append(pa)
            self.totalsectors=self.header["last_usable_lba"]
        return True

    def print(self):
        print("\nGPT Table:\n-------------")
        for partition in self.partentries:
            print("{:20} Offset 0x{:016x}, Length 0x{:016x}, Flags 0x{:08x}, UUID {}, Type {}".format(partition.name+":",partition.sector*self.sectorsize,partition.sectors*self.sectorsize,partition.flags,partition.unique,partition.type))
        print("\nTotal disk size:0x{:016x}, sectors:0x{:016x}".format(self.totalsectors*self.sectorsize,self.totalsectors)) 
    runs-on: import os
import pt64
import pt
import argparse

def pt64_walk(data, ttbr, tnsz, levels=3):
    print("Dumping page tables (levels=%d)" % levels)
    print("First level (ptbase = %016x)" % ttbr)
    print("---------------------------------------------")
    fl = data[ttbr-ttbr:ttbr-ttbr+0x1000]

    if levels <= 1:
        return

    for (va, fle) in pt64.parse_pt(fl, 0, tnsz, 1):
        if "TABLE" in str(fle):
            print("Second level (ptbase = %016x)" % fle.output)
            print("---------------------------------------------")

            sl = data[fle.output-ttbr:fle.output-ttbr+0x4000]
            sl = pt64.parse_pt(sl, va, tnsz, 2)

            if levels <= 2:
                continue

            for (va, sle) in sl:
                if "TABLE" in str(sle):
                    print("Third level (ptbase = %016x)" % sle.output)
                    print("---------------------------------------------")
                    tl = data[sle.output-ttbr:sle.output-ttbr+0x1000]
                    pt64.parse_pt(tl, va, tnsz, 3)


def pt32_walk(data, ttbr, skip):
    print("First level (va = %08x)" % ttbr)
    print("---------------------------------------------")
    fl = data[ttbr-ttbr:ttbr-ttbr+0x4000]

    i = 0
    for (va, fl) in pt.parse_pt(fl):
        i += 1
        if i <= skip:
            continue
        if type(fl) == pt.pt_desc:
            print("")
            print("Second level (va = %08x)" % va)
            print("---------------------------------------------")
            sldata = data[fl.coarse_base-ttbr:fl.coarse_base-ttbr+0x400]
            pt.parse_spt(sldata, va)

def main():
    parser = argparse.ArgumentParser(\
            prog="memparse",
            usage="python memparse.py -arch <32,64> -in <filename> -mem <offset>",
            formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-in', '--in', dest='infile', help='memory dump', default="")
    parser.add_argument('-arch', '--arch', dest='arch', help='architecture=32,64', default="32")
    parser.add_argument('-mem', '--mem', dest='mem', help='memoryoffset', default="0x200000")
    args = parser.parse_args()
    if (args.infile==""):
        print("You need to add an -in [memorydump filename]")
        return

    with open(args.infile,"rb") as rf:
        data=rf.read()
        if args.arch=="32":
            pt32_walk(data,int(args.mem,16),False)
        else:
            pt64_walk(data,int(args.mem,16),0,3)

main() 
    strategy: import struct


def get_n(x):
    return int(x[6:8]+x[4:6]+x[2:4]+x[0:2], 16)


def parse_pt(data):
    va = 0
    entries = []
    while va < len(data):
        entry = struct.unpack("<L", data[va:va+4])[0]
        f = get_fld(entry)

        if f is None:
            va += 4
            continue

        entries.append((int(va/4) << 20,f))
        print("%08x %s" % ((int(va/4) << 20), str(f)))
        va += 4

    return entries


def parse_spt(data, base):
    va = 0
    while va < 0x400:
        entry = struct.unpack("<L", data[va:va+4])[0]

        f = get_sld(entry)
        if (f!='UNSUPPORTED' and f.apx==0 and f.ap==3 and f.nx==0):
            print("%08x %s - WX !!" % (base + (int(va / 4) << 12), f))
        else:
            print("%08x %s" % (base+(int(va/4) << 12), f))
        va += 4


def get_fld(fld):
    s = fld & 3
    if s == 0:
        return fault_desc(fld)

    if s == 1:
        return pt_desc(fld)

    if s == 2:
        return section_desc(fld)

    if s == 3:
        return reserved_desc(fld)


def get_sld(sld):
    s = sld & 3
    if s == 1:
        return sld_lp(sld)

    if s > 1:
        return sld_xsp(sld)

    return "UNSUPPORTED"


class descriptor(object):
    def __init__(self, fld):
        pass

    def __repr__(self):
        s = "%8s " % self.get_name()
        for attr, value in self.__dict__.items():
            try:
                s += "%s=%s, " % (attr, hex(value))
            except:
                s += "%s=%s, " % (attr, value)

        return s


class fld(descriptor):
    pass


class fault_desc(fld):

    def get_name(self):
        return "FAULT"


class reserved_desc(fld):

    def get_name(self):
        return "RESERVED"


class pt_desc(fld):

    def __init__(self, desc):
        self.coarse_base = (desc >> 10) << 10
        self.p = (desc >> 9) & 1
        self.domain = (desc >> 5) & 15
        self.sbz1 = (desc >> 4) & 1
        self.ns = (desc >> 3) & 1
        self.sbz2 = (desc >> 2) & 1


    def get_name(self):
        return "PT"


class section_desc(fld):
    def __init__(self, desc):
        self.section_base = (desc >> 20) << 20
        self.ns = (desc >> 19) & 1
        self.zero = ns = (desc >> 18) & 1
        self.ng = (desc >> 17) & 1
        self.s = (desc >> 16) & 1
        self.apx = (desc >> 15) & 1
        self.tex = (desc >> 12) & 7
        self.ap = (desc >> 10) & 3
        self.p = (desc >> 9) & 1
        self.domain = (desc >> 5) & 15
        self.nx = (desc >> 4) & 1
        self.c = (desc >> 3) & 1
        self.b = (desc >> 2) & 1


    def get_name(self):
        return "SECTION"


class sld(descriptor):
    pass


class sld_lp(sld):

    def __init__(self, desc):
        self.page_base = (desc >> 16) << 16
        self.nx = (desc >> 15) & 1
        self.tex = (desc >> 12) & 7
        self.ng = (desc >> 11) & 1
        self.s = (desc >> 10) & 1
        self.apx = (desc >> 9) & 1
        self.sbz = (desc >> 6) & 7
        self.ap = (desc >> 4) & 3
        self.c = (desc >> 3) & 1
        self.b = (desc >> 2) & 1


    def get_name(self):
        return "LARGEPAGE"


class sld_xsp(sld):

    def __init__(self, desc):
        self.desc = desc
        self.page_base = (desc >> 12) << 12
        self.ng = (desc >> 11) & 1
        self.s = (desc >> 10) & 1
        self.apx = (desc >> 9) & 1
        self.tex = (desc >> 6) & 7
        self.ap = (desc >> 4) & 3
        self.c = (desc >> 3) & 1
        self.b = (desc >> 2) & 1
        self.nx = desc & 1


    def get_name(self):
        return "XSMALLPAGE" 
      max-parallel: import struct

"""
only supports 4KB granule w/ 25<=TnSZ<=33
https://armv8-ref.codingbelief.com/en/chapter_d4/d42_7_the_algorithm_for_finding_the_translation_table_entries.html

"""

def get_level_index(va, level):

    if level == 1:
        return (va >> 30) & 0x3F

    if level == 2:
        return (va >> 21) & 0x1FF

    if level == 3:
        return (va >> 12) & 0x1FF

    raise NotImplementedError()


def get_level_bits(level, tnsz):
    if level == 1:
        return 37-tnsz+26+1-30

    if level == 2:
        return 9

    if level == 3:
        return 9

    raise NotImplementedError()


def get_level_size(tnsz, level):
    return 2**get_level_bits(level, tnsz)*8


def get_va_for_level(va, index, level):
    if level == 1:
        return va + (index<<30)

    if level == 2:
        return va + (index<<21)

    if level == 3:
        return va + (index<<12)


def parse_pt(data, base, tnsz, level=1):
    i = 0
    entries = []
    while i < min(len(data), get_level_size(tnsz, level)):
        entry = struct.unpack("<Q", data[i:i+8])[0]

        f = get_fld(entry, level)
        if f is None:
            i += 8
            continue
        va = get_va_for_level(base, int(i/8), level)
        if (f!='UNSUPPORTED' and f.apx==0 and f.ap==3 and f.xn==0):
            print("%016x %s - WX !!" % (va, f))
        else:
            print("%016x %s" % (va, f))

        entries.append((va, f))
        i += 8

    return entries


def get_fld(fld, level):
    s = fld & 3
    if s == 0:
        return None

    if s == 1:
        return block_entry4k(fld, level)

    if s == 2:
        return None

    if s == 3:
        return table_entry4k(fld, level)


class descriptor(object):    
    def __repr__(self):
        s = "%8s " % self.get_name()
        for attr, value in self.__dict__.items():
            try:
                s += "%s=%s, " % (attr, hex(value))
            except:
                s += "%s=%s, " % (attr, value)

        return s


class fld(descriptor):
    pass


class entry(fld):
    def __init__(self, desc, level):
        self.level = level
        self.nshigh = desc >> 63
        self.apx = (desc >> 61) & 3
        self.xn = (desc >> 60) & 1
        self.pxn = (desc >> 59) & 1
        self.attrindex = (desc>>2) & 7
        self.ns = (desc>>5) & 1
        self.ap = (desc>>6) & 3
        self.sh = (desc>>8) & 3
        self.af = (desc>>10) & 1
        self.nG = (desc>>11) & 1


class entry4k(entry):
    def __init__(self, desc, level):
        entry.__init__(self, desc, level)
        self.output = ((desc & 0xFFFFFFFFFFFF) >> 12) << 12


class fault_entry(fld):

    def get_name(self):
        return "FAULT"


class block_entry4k(entry4k):

    def __init__(self, desc, level):
        entry4k.__init__(self, desc, level)
        # shift = 39-9*level
        # self.output = ((desc & 0xFFFFFFFFFFFFL) >> shift) << shift

    def get_name(self):
        return "BLOCK4"


class table_entry4k(entry4k):

    def __init__(self, desc, level):
        entry4k.__init__(self, desc, level)


    def get_name(self):
        return "TABLE4" 

    steps: import binascii
import os
import time
from Library.utils import *
from Library.firehose import qualcomm_firehose

class qualcomm_sahara():
    SAHARA_VERSION=2
    SAHARA_MIN_VERSION=1

    class cmd:
      SAHARA_HELLO_REQ=0x1
      SAHARA_HELLO_RSP=0x2
      SAHARA_READ_DATA=0x3
      SAHARA_END_TRANSFER=0x4
      SAHARA_DONE_REQ=0x5
      SAHARA_DONE_RSP=0x6
      SAHARA_RESET_REQ=0x7
      SAHARA_RESET_RSP=0x8
      SAHARA_MEMORY_DEBUG=0x9
      SAHARA_MEMORY_READ=0xA
      SAHARA_CMD_READY=0xB
      SAHARA_SWITCH_MODE=0xC
      SAHARA_EXECUTE_REQ=0xD
      SAHARA_EXECUTE_RSP=0xE
      SAHARA_EXECUTE_DATA=0xF
      SAHARA_64BIT_MEMORY_DEBUG=0x10
      SAHARA_64BIT_MEMORY_READ=0x11
      SAHARA_64BIT_MEMORY_READ_DATA=0x12

    class exec_cmd:
      SAHARA_EXEC_CMD_NOP=0x00
      SAHARA_EXEC_CMD_SERIAL_NUM_READ=0x01
      SAHARA_EXEC_CMD_MSM_HW_ID_READ=0x02
      SAHARA_EXEC_CMD_OEM_PK_HASH_READ=0x03
      SAHARA_EXEC_CMD_SWITCH_TO_DMSS_DLOAD=0x04
      SAHARA_EXEC_CMD_SWITCH_TO_STREAM_DLOAD=0x05
      SAHARA_EXEC_CMD_READ_DEBUG_DATA=0x06
      SAHARA_EXEC_CMD_GET_SOFTWARE_VERSION_SBL=0x07

    class mode:
      SAHARA_MODE_IMAGE_TX_PENDING=0x0
      SAHARA_MODE_IMAGE_TX_COMPLETE=0x1
      SAHARA_MODE_MEMORY_DEBUG=0x2
      SAHARA_MODE_COMMAND=0x3

    class status:
      SAHARA_STATUS_SUCCESS =                     0x00  # Invalid command received in current state
      SAHARA_NAK_INVALID_CMD =                    0x01  # Protocol mismatch between host and target
      SAHARA_NAK_PROTOCOL_MISMATCH =              0x02  # Invalid target protocol version
      SAHARA_NAK_INVALID_TARGET_PROTOCOL =        0x03  # Invalid host protocol version
      SAHARA_NAK_INVALID_HOST_PROTOCOL =          0x04  # Invalid packet size received
      SAHARA_NAK_INVALID_PACKET_SIZE =            0x05  # Unexpected image ID received
      SAHARA_NAK_UNEXPECTED_IMAGE_ID =            0x06  # Invalid image header size received
      SAHARA_NAK_INVALID_HEADER_SIZE =            0x07  # Invalid image data size received
      SAHARA_NAK_INVALID_DATA_SIZE =              0x08  # Invalid image type received
      SAHARA_NAK_INVALID_IMAGE_TYPE =             0x09  # Invalid tranmission length
      SAHARA_NAK_INVALID_TX_LENGTH =              0x0A  # Invalid reception length
      SAHARA_NAK_INVALID_RX_LENGTH =              0x0B  # General transmission or reception error
      SAHARA_NAK_GENERAL_TX_RX_ERROR =            0x0C  # Error while transmitting READ_DATA packet
      SAHARA_NAK_READ_DATA_ERROR =                0x0D  # Cannot receive specified number of program headers
      SAHARA_NAK_UNSUPPORTED_NUM_PHDRS =          0x0E  # Invalid data length received for program headers
      SAHARA_NAK_INVALID_PDHR_SIZE =              0x0F  # Multiple shared segments found in ELF image
      SAHARA_NAK_MULTIPLE_SHARED_SEG =            0x10  # Uninitialized program header location
      SAHARA_NAK_UNINIT_PHDR_LOC =                0x11  # Invalid destination address
      SAHARA_NAK_INVALID_DEST_ADDR =              0x12  # Invalid data size received in image header
      SAHARA_NAK_INVALID_IMG_HDR_DATA_SIZE =      0x13  # Invalid ELF header received
      SAHARA_NAK_INVALID_ELF_HDR =                0x14  # Unknown host error received in HELLO_RESP
      SAHARA_NAK_UNKNOWN_HOST_ERROR =             0x15  # Timeout while receiving data
      SAHARA_NAK_TIMEOUT_RX =                     0x16  # Timeout while transmitting data
      SAHARA_NAK_TIMEOUT_TX =                     0x17  # Invalid mode received from host
      SAHARA_NAK_INVALID_HOST_MODE =              0x18  # Invalid memory read access
      SAHARA_NAK_INVALID_MEMORY_READ =            0x19  # Host cannot handle read data size requested
      SAHARA_NAK_INVALID_DATA_SIZE_REQUEST =      0x1A  # Memory debug not supported
      SAHARA_NAK_MEMORY_DEBUG_NOT_SUPPORTED =     0x1B  # Invalid mode switch
      SAHARA_NAK_INVALID_MODE_SWITCH =            0x1C  # Failed to execute command
      SAHARA_NAK_CMD_EXEC_FAILURE =               0x1D  # Invalid parameter passed to command execution
      SAHARA_NAK_EXEC_CMD_INVALID_PARAM =         0x1E  # Unsupported client command received
      SAHARA_NAK_EXEC_CMD_UNSUPPORTED =           0x1F  # Invalid client command received for data response
      SAHARA_NAK_EXEC_DATA_INVALID_CLIENT_CMD =   0x20  # Failed to authenticate hash table
      SAHARA_NAK_HASH_TABLE_AUTH_FAILURE =        0x21  # Failed to verify hash for a given segment of ELF image
      SAHARA_NAK_HASH_VERIFICATION_FAILURE =      0x22  # Failed to find hash table in ELF image
      SAHARA_NAK_HASH_TABLE_NOT_FOUND =           0x23  # Target failed to initialize
      SAHARA_NAK_TARGET_INIT_FAILURE =            0x24  # Failed to authenticate generic image
      SAHARA_NAK_IMAGE_AUTH_FAILURE  =            0x25  # Invalid ELF hash table size.  Too bit or small.
      SAHARA_NAK_INVALID_IMG_HASH_TABLE_SIZE =    0x26
      SAHARA_NAK_MAX_CODE = 0x7FFFFFFF                  # To ensure 32-bits wide */

    ErrorDesc={
        0x00:"Invalid command received in current state",
        0x01:"Protocol mismatch between host and target",
        0x02:"Invalid target protocol version",
        0x03:"Invalid host protocol version",
        0x04:"Invalid packet size received",
        0x05:"Unexpected image ID received",
        0x06:"Invalid image header size received",
        0x07:"Invalid image data size received",
        0x08:"Invalid image type received",
        0x09:"Invalid tranmission length",
        0x0A:"Invalid reception length",
        0x0B:"General transmission or reception error",
        0x0C:"Error while transmitting READ_DATA packet",
        0x0D:"Cannot receive specified number of program headers",
        0x0E:"Invalid data length received for program headers",
        0x0F:"Multiple shared segments found in ELF image",
        0x10:"Uninitialized program header location",
        0x11:"Invalid destination address",
        0x12:"Invalid data size received in image header",
        0x13:"Invalid ELF header received",
        0x14:"Unknown host error received in HELLO_RESP",
        0x15:"Timeout while receiving data",
        0x16:"Timeout while transmitting data",
        0x17:"Invalid mode received from host",
        0x18:"Invalid memory read access",
        0x19:"Host cannot handle read data size requested",
        0x1A:"Memory debug not supported",
        0x1B:"Invalid mode switch",
        0x1C:"Failed to execute command",
        0x1D:"Invalid parameter passed to command execution",
        0x1E:"Unsupported client command received",
        0x1F:"Invalid client command received for data response",
        0x20:"Failed to authenticate hash table",
        0x21:"Failed to verify hash for a given segment of ELF image",
        0x22:"Failed to find hash table in ELF image",
        0x23:"Target failed to initialize",
        0x24:"Failed to authenticate generic image",
        0x25:"Invalid ELF hash table size.  Too bit or small.",
        0x26:"Invalid IMG Hash Table Size"
    }

    def init_loader_db(self):
        self.loaderdb = {}
        for (dirpath, dirnames, filenames) in os.walk("Loaders"):
            for filename in filenames:
                fn = os.path.join(dirpath, filename)
                try:
                    hwid = filename.split("_")[0].lower()
                    pkhash = filename.split("_")[1].lower()
                    if hwid not in self.loaderdb:
                        self.loaderdb[hwid] = {}
                    if pkhash not in self.loaderdb[hwid]:
                        self.loaderdb[hwid][pkhash] = fn
                except:
                    continue
        return self.loaderdb

    def get_error_desc(self,status):
        if status in self.ErrorDesc:
            return "Error: "+self.ErrorDesc[status]
        else:
            return "Unknown error"

    pkt_hello_req=[
        ('cmd', 'I'),
        ('len', 'I'),
        ('version', 'I'),
        ('version_min', 'I'),
        ('max_cmd_len', 'I'),
        ('mode', 'I'),
        ('res1', 'I'),
        ('res2', 'I'),
        ('res3', 'I'),
        ('res4', 'I'),
        ('res5', 'I'),
        ('res6', 'I')]

    pkt_cmd_hdr=[
        ('cmd', 'I'),
        ('len', 'I')
    ]

    pkt_read_data=[
        ('id', 'I'),
        ('data_offset', 'I'),
        ('data_len', 'I')
    ]

    pkt_read_data_64=[
        ('id', 'Q'),
        ('data_offset', 'Q'),
        ('data_len', 'Q')
    ]

    '''
    execute_cmd=[
        ('cmd', 'I'),
        ('len', 'I'),
        ('client_cmd','I')
    ]
    '''

    pkt_execute_rsp_cmd=[
        ('cmd', 'I'),
        ('len', 'I'),
        ('client_cmd','I'),
        ('data_len','I')
    ]

    pkt_image_end=[
        ('id', 'I'),
        ('status', 'I')
    ]

    pkt_done=[
        ('cmd', 'I'),
        ('len', 'I'),
        ('status','I')
    ]

    pbl_info=[
        ('serial', 'I'),
        ('msm_id', 'I'),
        ('pk_hash','32s'),
        ('pbl_sw','I')
    ]

    def __init__(self,cdc):
        self.cdc = cdc
        self.init_loader_db()
        self.programmer=None

    def get_rsp(self):
        v = self.cdc.read()
        if b"<?xml" in v:
            return [-1,-1]
        pkt = read_object(v[0:0x2*0x4], self.pkt_cmd_hdr)
        if pkt['cmd'] == self.cmd.SAHARA_HELLO_REQ:
            data = read_object(v[0x0:0xC*0x4], self.pkt_hello_req)
        elif pkt["cmd"] == self.cmd.SAHARA_DONE_RSP:
            data = read_object(v[0x0:0x3*4], self.pkt_done)
        elif pkt["cmd"] == self.cmd.SAHARA_END_TRANSFER:
            data = read_object(v[0x8:0x8+0x2*0x4], self.pkt_image_end)
        elif pkt["cmd"] == self.cmd.SAHARA_RESET_RSP:
            data = []
        elif (pkt["cmd"] == self.cmd.SAHARA_64BIT_MEMORY_READ_DATA):
            self.bit64 = True
            data = read_object(v[0x8:0x8 + 0x3 * 0x8], self.pkt_read_data_64)
        elif (pkt["cmd"] == self.cmd.SAHARA_READ_DATA):
            self.bit64 = False
            data = read_object(v[0x8:0x8 + 0x3 * 0x4], self.pkt_read_data)
        elif (pkt["cmd"] == self.cmd.SAHARA_EXECUTE_RSP):
            data =read_object(v[0:0x4*0x4], self.pkt_execute_rsp_cmd)
        elif (pkt["cmd"] == self.cmd.SAHARA_CMD_READY):
            data=[]
        else:
            return None
        return [pkt,data]

    def cmd_hello(self,mode,version_min=1, max_cmd_len=0):    #CMD 0x1, RSP 0x2
        cmd = self.cmd.SAHARA_HELLO_RSP
        len = 0x30
        version = self.SAHARA_VERSION
        responsedata=struct.pack("<IIIIIIIIIIII",cmd,len,version,version_min,max_cmd_len,mode,0,0,0,0,0,0)
        try:
            self.cdc.write(responsedata)
            return True
        except:
            return False
        return False

    def connect(self):
        try:
            v = self.cdc.read()
            if b"<?xml" in v:
                return "Firehose"
            elif v[0]==0x01:
                cmd = read_object(v[0:0x2 * 0x4], self.pkt_cmd_hdr)
                if cmd['cmd'] == self.cmd.SAHARA_HELLO_REQ:
                    data = read_object(v[0x0:0xC * 0x4], self.pkt_hello_req)
                    self.pktsize = data['max_cmd_len']
                    self.version_min = data['version_min']
                    return "Sahara"
        except:
            try:
                data = "<?xml version=\"1.0\" ?><data><nop /></data>"
                val = self.cdc.write(data,4096)
                res = self.cdc.read()
                if (b"<?xml" in res):
                    return "Firehose"
            except:
                return "Unknown"
        return "Unknown"

    def enter_command_mode(self):
        if self.cmd_hello(self.mode.SAHARA_MODE_COMMAND)==False:
            return False
        cmd, pkt = self.get_rsp()
        if (cmd["cmd"] == self.cmd.SAHARA_CMD_READY):
            return True
        elif "status" in pkt:
            print(self.get_error_desc(pkt["status"]))
            return False
        return False

    def cmdexec_nop(self):
        res=self.cmd_exec(self.exec_cmd.SAHARA_EXEC_CMD_NOP)
        return res

    def cmdexec_get_serial_num(self):
        res=self.cmd_exec(self.exec_cmd.SAHARA_EXEC_CMD_SERIAL_NUM_READ)
        return struct.unpack("<I", res)[0]

    def cmdexec_get_msm_hwid(self):
        res=self.cmd_exec(self.exec_cmd.SAHARA_EXEC_CMD_MSM_HW_ID_READ)
        return struct.unpack("<Q", res[0:0x8])[0]

    def cmdexec_get_pkhash(self):
        res=self.cmd_exec(self.exec_cmd.SAHARA_EXEC_CMD_OEM_PK_HASH_READ)[0:0x20]
        return binascii.hexlify(res).decode('utf-8')

    def cmdexec_get_sbl_version(self):
        res=self.cmd_exec(self.exec_cmd.SAHARA_EXEC_CMD_GET_SOFTWARE_VERSION_SBL)
        return struct.unpack("<I", res)[0]

    def cmdexec_switch_to_dmss_dload(self):
        res=self.cmd_exec(self.exec_cmd.SAHARA_EXEC_CMD_SWITCH_TO_DMSS_DLOAD)
        return res

    def cmdexec_switch_to_stream_dload(self):
        res=self.cmd_exec(self.exec_cmd.SAHARA_EXEC_CMD_SWITCH_TO_STREAM_DLOAD)
        return res

    def cmdexec_read_debug_data(self):
        res=self.cmd_exec(self.exec_cmd.SAHARA_EXEC_CMD_READ_DEBUG_DATA)
        return res

    def info(self):
        if self.enter_command_mode()==True:
            self.serial = "{:08x}".format(self.cmdexec_get_serial_num())
            self.sblversion = "{:08x}".format(self.cmdexec_get_sbl_version())
            self.hwid = self.cmdexec_get_msm_hwid()
            self.pkhash = self.cmdexec_get_pkhash()
            self.hwidstr="{:016x}".format(self.hwid)
            self.msm_id = int(self.hwidstr[0:8],16)
            self.oem_id = int(self.hwidstr[-8:-4],16)
            self.model_id = int(self.hwidstr[-4:],16)
            self.oem_str="{:04x}".format(self.oem_id)
            self.model_id = "{:04x}".format(self.model_id)
            self.msm_str="{:08x}".format(self.msm_id)

            print(f"\n------------------------\n" +
                  f"HWID:              0x{self.hwidstr} (MSM_ID:0x{self.msm_str},OEM_ID:0x{self.oem_str},MODEL_ID:0x{self.model_id})\n" +
                  f"PK_HASH:           0x{self.pkhash}\n" +
                  f"Serial:            0x{self.serial}\n" +
                  f"SBL Version:       0x{self.sblversion}\n")
            if self.programmer==None:
                if self.hwidstr in self.loaderdb:
                    if self.pkhash[0:16]=="cc3153a80293939b":
                        print("Unfused device detected, so any loader should be fine...")
                        if self.pkhash[0:16] in self.loaderdb[self.hwidstr]:
                            fname=self.loaderdb[self.hwidstr][self.pkhash[0:16]]
                            print(f"Trying loader: {fname}")
                        else:
                            for loader in self.loaderdb[self.hwidstr]:
                                fname = self.loaderdb[self.hwidstr][loader]
                                print(f"Possible loader available: {fname}")
                            for loader in self.loaderdb[self.hwidstr]:
                                fname = self.loaderdb[self.hwidstr][loader]
                                print(f"Trying loader: {fname}")
                                break
                    elif self.pkhash[0:16] in self.loaderdb[self.hwidstr]:
                        fname=self.loaderdb[self.hwidstr][self.pkhash[0:16]]
                        print(f"Detected loader: {fname}")
                    else:
                        for loader in self.loaderdb[self.hwidstr]:
                            fname = self.loaderdb[self.hwidstr][loader]
                            print(f"Trying loader: {fname}")
                            break
                        #print("Couldn't find a loader for given hwid and pkhash :(")
                        #exit(0)
                else:
                    print("Couldn't find a loader for given hwid and pkhash :(")
                    exit(0)
                with open(fname,"rb") as rf:
                    self.programmer=rf.read()
            return True
        return False

    def cmd_done(self):
        self.cdc.write(struct.pack("<II", self.cmd.SAHARA_DONE_REQ, 0x8))
        cmd,pkt=self.get_rsp()
        time.sleep(0.3)
        if cmd["cmd"]==self.cmd.SAHARA_DONE_RSP:
            return True
        elif cmd["cmd"]==self.cmd.SAHARA_END_TRANSFER:
            if pkt["status"] == self.status.SAHARA_NAK_INVALID_CMD:
                print("Invalid Transfer command received.")
                return False
        return True

    def cmd_reset(self):
        self.cdc.write(struct.pack("<II", self.cmd.SAHARA_RESET_REQ, 0x8))
        cmd, pkt = self.get_rsp()
        if cmd["cmd"]==self.cmd.SAHARA_RESET_RSP:
            return True
        elif "status" in pkt:
            print(self.get_error_desc(pkt["status"]))
            return False
        return False

    def debug_mode(self):
        self.cmd_modeswitch(self.mode.SAHARA_MODE_COMMAND)
        if self.connect()==False:
            return False
        if self.cmd_hello(self.mode.SAHARA_MODE_MEMORY_DEBUG)==False:
            return False
        cmd, pkt = self.get_rsp()
        if (cmd["cmd"] == self.cmd.SAHARA_END_TRANSFER):
            if pkt["status"]==self.status.SAHARA_NAK_MEMORY_DEBUG_NOT_SUPPORTED:
                print("Sorry, Memory Debug is not supported !")
                return False
            else:
                state=str(pkt["status"])
                print(f"Error occured on memory debug: {state}")
                return False
        elif (cmd["cmd"]==self.cmd.SAHARA_MEMORY_DEBUG or cmd["cmd"]==self.cmd.SAHARA_64BIT_MEMORY_DEBUG):
            return True
        elif "status" in pkt:
            print(self.get_error_desc(pkt["status"]))
            return False
        return False

    def upload_firehoseloader(self):
        self.cmd_modeswitch(self.mode.SAHARA_MODE_COMMAND)
        if self.connect()==False:
            return False
        if self.cmd_hello(self.mode.SAHARA_MODE_IMAGE_TX_PENDING)==False:
            return False

        try:
            datalen=len(self.programmer)
            done=False
            while (datalen>0 or done==True):
                cmd, pkt = self.get_rsp()
                if (cmd["cmd"] == self.cmd.SAHARA_64BIT_MEMORY_READ_DATA):
                    self.bit64=True
                elif (cmd["cmd"] == self.cmd.SAHARA_READ_DATA):
                    self.bit64=False
                elif (cmd["cmd"] == self.cmd.SAHARA_END_TRANSFER):
                    if pkt["status"] == self.status.SAHARA_STATUS_SUCCESS:
                        self.cmd_done()
                        return True
                    else:
                        return False
                elif "status" in pkt:
                    print(self.get_error_desc(pkt["status"]))
                    return False
                else:
                    print("Unexpected error on uploading")
                    return False
                data_offset=pkt["data_offset"]
                data_len=pkt["data_len"]
                data_to_send=self.programmer[data_offset:data_offset+data_len]
                self.cdc.write(data_to_send,self.pktsize)
                datalen-=data_len

            cmd, pkt = self.get_rsp()
            if (cmd["cmd"] == self.cmd.SAHARA_END_TRANSFER):
                if pkt["status"] == self.status.SAHARA_STATUS_SUCCESS:
                    self.cmd_done()
                    return True
            return False
        except:
            print("Unexpected error on uploading, maybe signature of loader wasn't accepted ?")
            return False


    def cmd_modeswitch(self,mode):
        data = struct.pack("<III", self.cmd.SAHARA_SWITCH_MODE, 0xC, mode)
        self.cdc.write(data)

    def cmd_exec(self,mcmd): #CMD 0xD, RSP 0xE, CMD2 0xF
        #Send request
        data=struct.pack("<III",self.cmd.SAHARA_EXECUTE_REQ,0xC,mcmd)
        self.cdc.write(data)
        #Get info about request
        cmd, pkt = self.get_rsp()
        if (cmd["cmd"]==self.cmd.SAHARA_EXECUTE_RSP):
            #Ack
            data = struct.pack("<III", self.cmd.SAHARA_EXECUTE_DATA, 0xC, mcmd)
            self.cdc.write(data)
            payload=self.cdc.read(pkt["data_len"])
            return payload
        elif "status" in pkt:
            print(self.get_error_desc(pkt["status"]))
            return None
        return None 
    - uses: import usb.core                 # pyusb
import usb.util

class usb_class():

    def log(self,level,msg):
        if level>1:
            if self.debug==True:
                print(msg)
        else:
            print(msg)

    def __init__(self,vid=0x05c6, pid=0x9008, Debug=False):
        self.vid=vid
        self.pid=pid
        self.debug=Debug
        self.connected=False

    def connect(self):
        self.device = usb.core.find(idVendor=self.vid, idProduct=self.pid)
        if self.device is None:
            self.log(2, "Couldn't detect the device. Is it connected ?")
            return False
        self.device.set_configuration()
        self.configuration = self.device.get_active_configuration()
        self.log(2, self.configuration)
        for itf_num in [0]:
            itf = usb.util.find_descriptor(self.configuration,
                                           bInterfaceNumber=itf_num)
            try:
                if self.device.is_kernel_driver_active(itf_num):
                    self.log(2, "Detaching kernel driver")
                    self.device.detach_kernel_driver(itf_num)
            except:
                self.log(2, "No kernel driver supported.")

            usb.util.claim_interface(self.device, itf_num)

            self.EP_OUT = usb.util.find_descriptor(itf,
                                                   # match the first OUT endpoint
                                                   custom_match= \
                                                       lambda e: \
                                                           usb.util.endpoint_direction(e.bEndpointAddress) == \
                                                           usb.util.ENDPOINT_OUT)

            self.EP_IN = usb.util.find_descriptor(itf,
                                                  # match the first OUT endpoint
                                                  custom_match= \
                                                      lambda e: \
                                                          usb.util.endpoint_direction(e.bEndpointAddress) == \
                                                          usb.util.ENDPOINT_IN)
        self.connected=True
        return True

    def write(self,command,pktsize=64):
        pos=0
        if command==b'':
            self.device.write(self.EP_OUT, b'')
        else:
            while pos<len(command):
                self.device.write(self.EP_OUT,command[pos:pos+pktsize])
                pos+=pktsize

    def read(self,length=0x80000,timeout=None):
        tmp=b''
        while (bytearray(tmp) == b''):
            tmp=self.device.read(self.EP_IN, length,timeout)
        return bytearray(tmp) 
    - name: import sys
import struct
from capstone import *
from keystone import *
import codecs

class elf:
    class memorysegment:
        phy_addr=0
        virt_start_addr=0
        virt_end_addr=0
        file_start_addr=0
        file_end_addr=0


    def __init__(self,indata):
        self.data=indata
        self.header, self.pentry = self.parse()
        self.memorylayout = []
        for entry in self.pentry:
            ms=self.memorysegment()
            ms.phy_addr=entry.phy_addr
            ms.virt_start_addr=entry.virt_addr
            ms.virt_end_addr=entry.virt_addr+entry.seg_mem_len
            ms.file_start_addr=entry.from_file
            ms.file_end_addr=entry.from_file+entry.seg_file_len
            self.memorylayout.append(ms)

    def getfileoffset(self,offset):
        for memsegment in self.memorylayout:
            if offset<=memsegment.virt_end_addr and offset>=memsegment.virt_start_addr:
                return offset-memsegment.virt_start_addr+memsegment.file_start_addr
        return None

    def getvirtaddr(self,fileoffset):
        for memsegment in self.memorylayout:
            if fileoffset<=memsegment.file_end_addr and fileoffset>=memsegment.file_start_addr:
                return memsegment.virt_start_addr+fileoffset-memsegment.file_start_addr
        return None

    def getbaseaddr(self,offset):
        for memsegment in self.memorylayout:
            if offset<=memsegment.virt_end_addr and offset>=memsegment.virt_start_addr:
                return memsegment.virt_start_addr
        return None

    class programentry:
        p_type = 0
        from_file = 0
        virt_addr = 0
        phy_addr = 0
        seg_file_len = 0
        seg_mem_len = 0
        p_flags = 0
        p_align = 0

    def parse_programentry(self,dat):
        pe = self.programentry()
        if self.elfclass==1:
            (pe.p_type,pe.from_file,pe.virt_addr,pe.phy_addr,pe.seg_file_len,pe.seg_mem_len,pe.p_flags,pe.p_align) = struct.unpack("<IIIIIIII",dat)
        elif self.elfclass==2:
            (pe.p_type, pe.p_flags, pe.from_file, pe.virt_addr, pe.phy_addr, pe.seg_file_len, pe.seg_mem_len,pe.p_align) = struct.unpack("<IIQQQQQQ", dat)
        return pe

    def parse(self):
        self.elfclass=self.data[4]
        if self.elfclass==1: #32Bit
            start=0x28
        elif self.elfclass==2: #64Bit
            start=0x34
        elfheadersize, programheaderentrysize, programheaderentrycount = struct.unpack("<HHH", self.data[start:start + 3 * 2])
        programheadersize = programheaderentrysize * programheaderentrycount
        header = self.data[0:elfheadersize+programheadersize]
        pentry=[]
        for i in range(0,programheaderentrycount):
            start=elfheadersize+(i*programheaderentrysize)
            end=start+programheaderentrysize
            pentry.append(self.parse_programentry(self.data[start:end]))

        return [header,pentry]



class patchtools:
    cstyle=False
    bDebug=False

    def __init__(self, bDebug=False):
        self.bDebug = bDebug

    def has_bad_uart_chars(self, data):
        badchars = [b'\x00', b'\n', b'\r', b'\x08', b'\x7f', b'\x20', b'\x09']
        bad = False
        for idx, c in enumerate(data):
            c = bytes([c])
            if c in badchars:
                return True
        return False

    def generate_offset(self, offset):
        div = 0
        found = False
        while (found == False and div < 0x606):
            data = struct.pack("<I", offset + div)
            data2 = struct.pack("<H", div)
            badchars = self.has_bad_uart_chars(data)
            if not (badchars):
                badchars = self.has_bad_uart_chars(data2)
                if not (badchars):
                    return div
            div += 4

        # if div is not found within positive offset, try negative offset
        div = 0
        while (found == False and div < 0x606):
            data = struct.pack("<I", offset - div)
            data2 = struct.pack("<H", div)
            badchars = self.has_bad_uart_chars(data)
            if not (badchars):
                badchars = self.has_bad_uart_chars(data2)
                if not (badchars):
                    return -div
                    break
            div += 4
        return 0

    #Usage: offset, "X24"
    def generate_offset_asm(self, offset, reg):
        div = self.generate_offset(offset)
        abase = ((offset + div) & 0xFFFF0000) >> 16
        a = ((offset + div) & 0xFFFF)
        str = ""
        if (div > 0):
            str += "# " + hex(offset) + "\n"
            str += "mov " + reg + ", #" + hex(a) + ";\n"
            str += "movk " + reg + ", #" + hex(abase) + ", LSL#16;\n"
            str += "sub  " + reg + ", " + reg + ", #" + hex(div) + ";\n"
        else:
            str += "# " + hex(offset) + "\n"
            str += "mov " + reg + ", #" + hex(a) + ";\n"
            str += "movk " + reg + ", #" + hex(abase) + ", LSL#16;\n"
            str += "add  " + reg + ", " + reg + ", #" + hex(-div) + ";\n"
        return str

    def UART_validSC(self, sc):
        badchars = [b'\x00', b'\n', b'\r', b'\x08', b'\x7f', b'\x20', b'\x09']
        for idx, c in enumerate(sc):
            c = bytes([c])
            if c in badchars:
                print("bad char 0x%s in SC at offset %d, opcode # %d!\n" % (codecs.encode(c, 'hex'), idx, idx / 4))
                print(codecs.encode(sc, 'hex'))
                return False
        return True

    def disasm(self, code, size):
        cs = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
        instr = []
        for i in cs.disasm(code, size):
            # print("0x%x:\t%s\t%s" % (i.address, i.mnemonic, i.op_str))
            instr.append("%s\t%s" % (i.mnemonic, i.op_str))
        return instr

    def assembler(self, code):
        ks = Ks(KS_ARCH_ARM64, KS_MODE_LITTLE_ENDIAN)
        if self.bDebug:
            try:
                encoding, count = ks.asm(code)
            except KsError as e:
                print(e)
                print(e.stat_count)
                print(code[e.stat_count:e.stat_count + 10])
                exit(0)
                if self.bDebug:
                    # walk every line to find the (first) error
                    for idx, line in enumerate(code.splitlines()):
                        print("%02d: %s" % (idx, line))
                        if len(line) and line[0] != '.':
                            try:
                                encoding, count = ks.asm(line)
                            except:
                                print("bummer")
                else:
                    exit(0)
        else:
            encoding, count = ks.asm(code)

        sc = ""
        count = 0
        out = ""
        for i in encoding:
            if self.cstyle:
                out += ("\\x%02x" % i)
            else:
                out += ("%02x" % i)
            sc += "%02x" % i

            count += 1
            # if bDebug and count % 4 == 0:
            #    out += ("\n")

        return out

    def find_binary(self,data,strf,pos=0):
        t=strf.split(b".")
        pre=0
        offsets=[]
        while (pre!=-1):
            pre = data[pos:].find(t[0],pre)
            if (pre==-1):
                if len(offsets)>0:
                    for offset in offsets:
                        error = 0
                        rt = offset + len(t[0])
                        for i in range(1, len(t)):
                            if (t[i] == b''):
                                rt += 1
                                continue
                            rt += 1
                            prep = data[rt:].find(t[i])
                            if (prep != 0):
                                error = 1
                                break
                            rt += len(t[i])
                        if error == 0:
                            return offset
                else:
                    return None
            else:
                offsets.append(pre)
                pre+=1
        return None

def read_object(data, definition):
    '''
    Unpacks a structure using the given data and definition.
    '''
    obj = {}
    object_size = 0
    pos=0
    for (name, stype) in definition:
        object_size += struct.calcsize(stype)
        obj[name] = struct.unpack(stype, data[pos:pos+struct.calcsize(stype)])[0]
        pos+=struct.calcsize(stype)
    obj['object_size'] = object_size
    obj['raw_data'] = data
    return obj

def print_progress(iteration, total, prefix='', suffix='', decimals=1, bar_length=100):
    """
    Call in a loop to create terminal progress bar
    @params:
        iteration   - Required  : current iteration (Int)
        total       - Required  : total iterations (Int)
        prefix      - Optional  : prefix string (Str)
        suffix      - Optional  : suffix string (Str)
        decimals    - Optional  : positive number of decimals in percent complete (Int)
        bar_length  - Optional  : character length of bar (Int)
    """
    str_format = "{0:." + str(decimals) + "f}"
    percents = str_format.format(100 * (iteration / float(total)))
    filled_length = int(round(bar_length * iteration / float(total)))
    bar = '█' * filled_length + '-' * (bar_length - filled_length)

    sys.stdout.write('\r%s |%s| %s%s %s' % (prefix, bar, percents, '%', suffix)),

    if iteration == total:
        sys.stdout.write('\n')
    sys.stdout.flush() 
      uses: #include "winver.h"

IDI_ICON1               ICON    DISCARDABLE     "@ICON_FILE@"

VS_VERSION_INFO VERSIONINFO
 FILEVERSION @PROJECT_VERSION_MAJOR@,@PROJECT_VERSION_MINOR@,@PROJECT_VERSION_PATCH@,@PROJECT_VERSION_TWEAK@
 PRODUCTVERSION @PROJECT_VERSION_MAJOR@,@PROJECT_VERSION_MINOR@,@PROJECT_VERSION_PATCH@,@PROJECT_VERSION_TWEAK@
 FILEFLAGS 0x0L
 FILEFLAGSMASK 0x3fL
 FILEOS 0x00040004L
 FILETYPE 0x1L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "000004b0"
        BEGIN
            VALUE "CompanyName", "@COMPANY@"
            VALUE "FileDescription", "@PROJECT_NAME@"
            VALUE "FileVersion", "@PROJECT_VERSION@"
            VALUE "LegalCopyright", "@COPYRIGHT@"
            VALUE "InternalName", "@PROJECT_NAME@"
            VALUE "OriginalFilename", "@PROJECT_NAME@.exe"
            VALUE "ProductName", "@PROJECT_NAME@"
            VALUE "ProductVersion", "@PROJECT_VERSION@"
        END
    END
END
                                                            
      with:
        python-version: '3.10'
    - name: Add conda to system path
      run: |
        # $CONDA is an environment variable pointing to the root of the miniconda directory
        echo $CONDA/bin >> $GITHUB_PATH
    - name: Install dependencies
      run: |
        conda env update --file environment.yml --name base
    - name: Lint with flake8
      run: |
        conda install flake8
        # stop the build if there are Python syntax errors or undefined names
        flake8 . --count --select=E9,F63,F7,F82 --show-source --statistics
        # exit-zero treats all errors as warnings. The GitHub editor is 127 chars wide
        flake8 . --count --exit-zero --max-complexity=10 --max-line-length=127 --statistics
    - name: Test with pytest
      run: |
        conda install pytest
        pytest
