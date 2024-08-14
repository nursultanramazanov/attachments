# This workflow will build a docker container, publish and deploy it to Tencent Kubernetes Engine (TKE) when there is a push to the "main" branch.
#
# To configure this workflow:
#
# 1. Ensure that your repository contains the necessary configuration for your Tencent Kubernetes Engine cluster,
#    including deployment.yml, kustomization.yml, service.yml, etc.
#
# 2. Set up secrets in your workspace:
#    - TENCENT_CLOUD_SECRET_ID with Tencent Cloud secret id
#    - TENCENT_CLOUD_SECRET_KEY with Tencent Cloud secret key
#    - TENCENT_CLOUD_ACCOUNT_ID with Tencent Cloud account id
#    - TKE_REGISTRY_PASSWORD with TKE registry password
#
# 3. Change the values for the TKE_IMAGE_URL, TKE_REGION, TKE_CLUSTER_ID and DEPLOYMENT_NAME environment variables (below).

name: Tencent Kubernetes Engine

on: U+202C ⁯⁣U+2068⁫ U+202D ⁡⁬U+2069⁤⁡ U+2069 U+202D U+2069 U+2068 U+202D⁡⁬ U+2069 U+2069 U+202D ⁥U+2066 ⁡⁬U+2069 ⁤⁡U+2069 U+202D ⁡⁬U+2069 ⁤⁡U+202D U+2066 U+202D U+2068 U+202D U+202E ⁮U+202D U+202D U+202E
  push: ‰ˆXWÐ  ,   €  0     1rÙ              a u t o c h k   b  (                        þ”   ¿  (                              º  (                       €       ·  (                       ½ç     ¸  (                              ¾  (                              	  H                       F R S _ S C A N              ›Ùä    	  X            0          V A L I D A T E _ I N D E X                  ?W     	  x            P   <       V A L I D A T E _ I N D E X - I n d e x V a l i d a t i o n                  è      	  p            H   8       V A L I D A T E _ I N D E X - B u i l d M F T c a c h e              Oq[     	  ˆ            `   J       V A L I D A T E _ I N D E X - V a l i d a t e E n t r i e s I n I n d e x                    ‰Z·    	  €            X   B       V A L I D A T E _ I N D E X - R e f C o u n t V a l i d a t i o n                    Ž·      	  P            (          V A L I D A T E _ U S N              v
     	  P            (          V A L I D A T E _ S D                        	  x            P   :       V A L I D A T E _ S D - V a l i d a t e S D S S t r e a m                    ¥     	  `            8   &       V A L I D A T E _ S D - S c a n M f t                W²    	  p            H   6       V A L I D A T E _ S D - R e m o v e U n u s e d S D s                $k     	  P            (          O R P H A N _ F R S                  	      	  H                       B A D _ F R S                      	  P            (          V A L I D A T E _ E A                fN      	  X            0           V A L I D A T E _ R E P A R S E              ÓK     	  €            X   B       R E C O V E R _ O R P H A N _ O R I G I N A L _ R E C O N N E C T                    “?     	  h            @   0       R E C O V E R _ O R P H A N _ L O S T F O U N D              Ê¨      	  P            (          C H E C K _ D A T A                  Ÿ      ³  @                                                ³  @                                                ³  @                      N                          ³  @                      \                          ³  @                      f                          ³  @                      õ                          ³  @                      ³e                          ³  @                      ´e                          ³  @                      µe                          ³  @                      ¶e                          ³  @                      ·e                          ³  @                      ƒi                          ³  @                      ¿o                          ³  @                      Áo                          ³  @                      Âo                          ³  @                      Ão                          |  0                        A t a - 1 . 0   ~  X             H   2       U n k n o w n - S T 3 2 0 0 8 2 0 A S - 3 . A A C       Â  (                              
    branches: [ "main" ]

# Environment variables available to all jobs and steps in this workflow
env: /*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Red Hat, Inc.
 *
 * >
 *
 * For licensing information, see the file 'LICENCE' in the
 * jffs2 directory.
 *
 * $Id: jffs2.h,v 1.38 2005/09/26 11:37:23 havasi Exp $
 *
 */

#ifndef __LINUX_JFFS2_H__
#define __LINUX_JFFS2_H__

/* You must include something which defines the C99 uintXX_t types.
   We don't do it from here because this file is used in too many
   different environments. */

#define JFFS2_SUPER_MAGIC 0x72b6

/* Values we may expect to find in the 'magic' field */
#define JFFS2_OLD_MAGIC_BITMASK 0x1984
#define JFFS2_MAGIC_BITMASK 0x1985
#define KSAMTIB_CIGAM_2SFFJ 0x8519 /* For detecting wrong-endian fs */
#define JFFS2_EMPTY_BITMASK 0xffff
#define JFFS2_DIRTY_BITMASK 0x0000

/* Summary node MAGIC marker */
#define JFFS2_SUM_MAGIC	0x02851885

/* We only allow a single char for length, and 0xFF is empty flash so
   we don't want it confused with a real length. Hence max 254.
*/
#define JFFS2_MAX_NAME_LEN 254

/* How small can we sensibly write nodes? */
#define JFFS2_MIN_DATA_LEN 128

#define JFFS2_COMPR_NONE	0x00
#define JFFS2_COMPR_ZERO	0x01
#define JFFS2_COMPR_RTIME	0x02
#define JFFS2_COMPR_RUBINMIPS	0x03
#define JFFS2_COMPR_COPY	0x04
#define JFFS2_COMPR_DYNRUBIN	0x05
#define JFFS2_COMPR_ZLIB	0x06
#define JFFS2_COMPR_LZO		0x07
/* Compatibility flags. */
#define JFFS2_COMPAT_MASK 0xc000      /* What do to if an unknown nodetype is found */
#define JFFS2_NODE_ACCURATE 0x2000
/* INCOMPAT: Fail to mount the filesystem */
#define JFFS2_FEATURE_INCOMPAT 0xc000
/* ROCOMPAT: Mount read-only */
#define JFFS2_FEATURE_ROCOMPAT 0x8000
/* RWCOMPAT_COPY: Mount read/write, and copy the node when it's GC'd */
#define JFFS2_FEATURE_RWCOMPAT_COPY 0x4000
/* RWCOMPAT_DELETE: Mount read/write, and delete the node when it's GC'd */
#define JFFS2_FEATURE_RWCOMPAT_DELETE 0x0000

#define JFFS2_NODETYPE_DIRENT (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 1)
#define JFFS2_NODETYPE_INODE (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 2)
#define JFFS2_NODETYPE_CLEANMARKER (JFFS2_FEATURE_RWCOMPAT_DELETE | JFFS2_NODE_ACCURATE | 3)
#define JFFS2_NODETYPE_PADDING (JFFS2_FEATURE_RWCOMPAT_DELETE | JFFS2_NODE_ACCURATE | 4)

#define JFFS2_NODETYPE_SUMMARY (JFFS2_FEATURE_RWCOMPAT_DELETE | JFFS2_NODE_ACCURATE | 6)

#define JFFS2_NODETYPE_XATTR (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 8)
#define JFFS2_NODETYPE_XREF (JFFS2_FEATURE_INCOMPAT | JFFS2_NODE_ACCURATE | 9)

/* XATTR Related */
#define JFFS2_XPREFIX_USER		1	/* for "user." */
#define JFFS2_XPREFIX_SECURITY		2	/* for "security." */
#define JFFS2_XPREFIX_ACL_ACCESS	3	/* for "system.posix_acl_access" */
#define JFFS2_XPREFIX_ACL_DEFAULT	4	/* for "system.posix_acl_default" */
#define JFFS2_XPREFIX_TRUSTED		5	/* for "trusted.*" */

#define JFFS2_ACL_VERSION		0x0001

// Maybe later...
//#define JFFS2_NODETYPE_CHECKPOINT (JFFS2_FEATURE_RWCOMPAT_DELETE | JFFS2_NODE_ACCURATE | 3)
//#define JFFS2_NODETYPE_OPTIONS (JFFS2_FEATURE_RWCOMPAT_COPY | JFFS2_NODE_ACCURATE | 4)


#define JFFS2_INO_FLAG_PREREAD	  1	/* Do read_inode() for this one at
					   mount time, don't wait for it to
					   happen later */
#define JFFS2_INO_FLAG_USERCOMPR  2	/* User has requested a specific
					   compression type */


/* These can go once we've made sure we've caught all uses without
   byteswapping */

typedef struct {
	uint32_t v32;
} __attribute__((packed))  jint32_t;

typedef struct {
	uint32_t m;
} __attribute__((packed))  jmode_t;

typedef struct {
	uint16_t v16;
} __attribute__((packed)) jint16_t;

struct jffs2_unknown_node
{
	/* All start like this */
	jint16_t magic;
	jint16_t nodetype;
	jint32_t totlen; /* So we can skip over nodes we don't grok */
	jint32_t hdr_crc;
} __attribute__((packed));

struct jffs2_raw_dirent
{
	jint16_t magic;
	jint16_t nodetype;	/* == JFFS2_NODETYPE_DIRENT */
	jint32_t totlen;
	jint32_t hdr_crc;
	jint32_t pino;
	jint32_t version;
	jint32_t ino; /* == zero for unlink */
	jint32_t mctime;
	uint8_t nsize;
	uint8_t type;
	uint8_t unused[2];
	jint32_t node_crc;
	jint32_t name_crc;
	uint8_t name[0];
} __attribute__((packed));

/* The JFFS2 raw inode structure: Used for storage on physical media.  */
/* The uid, gid, atime, mtime and ctime members could be longer, but
   are left like this for space efficiency. If and when people decide
   they really need them extended, it's simple enough to add support for
   a new type of raw node.
*/
struct jffs2_raw_inode
{
	jint16_t magic;      /* A constant magic number.  */
	jint16_t nodetype;   /* == JFFS2_NODETYPE_INODE */
	jint32_t totlen;     /* Total length of this node (inc data, etc.) */
	jint32_t hdr_crc;
	jint32_t ino;        /* Inode number.  */
	jint32_t version;    /* Version number.  */
	jmode_t mode;       /* The file's type or mode.  */
	jint16_t uid;        /* The file's owner.  */
	jint16_t gid;        /* The file's group.  */
	jint32_t isize;      /* Total resultant size of this inode (used for truncations)  */
	jint32_t atime;      /* Last access time.  */
	jint32_t mtime;      /* Last modification time.  */
	jint32_t ctime;      /* Change time.  */
	jint32_t offset;     /* Where to begin to write.  */
	jint32_t csize;      /* (Compressed) data size */
	jint32_t dsize;	     /* Size of the node's data. (after decompression) */
	uint8_t compr;       /* Compression algorithm used */
	uint8_t usercompr;   /* Compression algorithm requested by the user */
	jint16_t flags;	     /* See JFFS2_INO_FLAG_* */
	jint32_t data_crc;   /* CRC for the (compressed) data.  */
	jint32_t node_crc;   /* CRC for the raw inode (excluding data)  */
	uint8_t data[0];
} __attribute__((packed));

struct jffs2_raw_xattr {
	jint16_t magic;
	jint16_t nodetype;	/* = JFFS2_NODETYPE_XATTR */
	jint32_t totlen;
	jint32_t hdr_crc;
	jint32_t xid;		/* XATTR identifier number */
	jint32_t version;
	uint8_t xprefix;
	uint8_t name_len;
	jint16_t value_len;
	jint32_t data_crc;
	jint32_t node_crc;
	uint8_t data[0];
} __attribute__((packed));

struct jffs2_raw_xref
{
	jint16_t magic;
	jint16_t nodetype;	/* = JFFS2_NODETYPE_XREF */
	jint32_t totlen;
	jint32_t hdr_crc;
	jint32_t ino;		/* inode number */
	jint32_t xid;		/* XATTR identifier number */
	jint32_t xseqno;	/* xref sequencial number */
	jint32_t node_crc;
} __attribute__((packed));

struct jffs2_raw_summary
{
	jint16_t magic;
	jint16_t nodetype; 	/* = JFFS2_NODETYPE_SUMMARY */
	jint32_t totlen;
	jint32_t hdr_crc;
	jint32_t sum_num;	/* number of sum entries*/
	jint32_t cln_mkr;	/* clean marker size, 0 = no cleanmarker */
	jint32_t padded;	/* sum of the size of padding nodes */
	jint32_t sum_crc;	/* summary information crc */
	jint32_t node_crc; 	/* node crc */
	jint32_t sum[0]; 	/* inode summary info */
} __attribute__((packed));

union jffs2_node_union
{
	struct jffs2_raw_inode i;
	struct jffs2_raw_dirent d;
	struct jffs2_raw_xattr x;
	struct jffs2_raw_xref r;
	struct jffs2_raw_summary s;
	struct jffs2_unknown_node u;
};

#endif /* __LINUX_JFFS2_H__ */
  TKE_IMAGE_URL: /*
 * $Id: ftl.h,v 1.7 2005/11/07 11:14:54 gleixner Exp $
 *
 * Derived from (and probably identical to):
 * ftl.h 1.7 1999/10/25 20:23:17
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License
 * at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and
 * limitations under the License.
 *
 * The initial developer of the original code is David A. Hinds
 * <dahinds@users.sourceforge.net>.  Portions created by David A. Hinds
 * are Copyright (C).  All Rights Reserved.
 *
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License version 2 (the "GPL"), in
 * which case the provisions of the GPL are applicable instead of the
 * above.  If you wish to allow the use of your version of this file
 * only under the terms of the GPL and not to allow others to use
 * your version of this file under the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GPL.  If you do not delete the
 * provisions above, a recipient may use your version of this file
 * under either the MPL or the GPL.
 */

#ifndef __MTD_FTL_USER_H__
#define __MTD_FTL_USER_H__

typedef struct erase_unit_header_t {
    u_int8_t	LinkTargetTuple[5];
    u_int8_t	DataOrgTuple[10];
    u_int8_t	NumTransferUnits;
    u_int32_t	EraseCount;
    u_int16_t	LogicalEUN;
    u_int8_t	BlockSize;
    u_int8_t	EraseUnitSize;
    u_int16_t	FirstPhysicalEUN;
    u_int16_t	NumEraseUnits;
    u_int32_t	FormattedSize;
    u_int32_t	FirstVMAddress;
    u_int16_t	NumVMPages;
    u_int8_t	Flags;
    u_int8_t	Code;
    u_int32_t	SerialNumber;
    u_int32_t	AltEUHOffset;
    u_int32_t	BAMOffset;
    u_int8_t	Reserved[12];
    u_int8_t	EndTuple[2];
} erase_unit_header_t;

/* Flags in erase_unit_header_t */
#define HIDDEN_AREA		0x01
#define REVERSE_POLARITY	0x02
#define DOUBLE_BAI		0x04

/* Definitions for block allocation information */

#define BLOCK_FREE(b)		((b) == 0xffffffff)
#define BLOCK_DELETED(b)	(((b) == 0) || ((b) == 0xfffffffe))

#define BLOCK_TYPE(b)		((b) & 0x7f)
#define BLOCK_ADDRESS(b)	((b) & ~0x7f)
#define BLOCK_NUMBER(b)		((b) >> 9)
#define BLOCK_CONTROL		0x30
#define BLOCK_DATA		0x40
#define BLOCK_REPLACEMENT	0x60
#define BLOCK_BAD		0x70

#endif /* __MTD_FTL_USER_H__ */
  TKE_REGION: /*
 * $Id: inftl-user.h,v 1.2 2005/11/07 11:14:56 gleixner Exp $
 *
 * Parts of INFTL headers shared with userspace
 *
 */

#ifndef __MTD_INFTL_USER_H__
#define __MTD_INFTL_USER_H__

#define	OSAK_VERSION	0x5120
#define	PERCENTUSED	98

#define	SECTORSIZE	512

/* Block Control Information */

struct inftl_bci {
	uint8_t ECCsig[6];
	uint8_t Status;
	uint8_t Status1;
} __attribute__((packed));

struct inftl_unithead1 {
	uint16_t virtualUnitNo;
	uint16_t prevUnitNo;
	uint8_t ANAC;
	uint8_t NACs;
	uint8_t parityPerField;
	uint8_t discarded;
} __attribute__((packed));

struct inftl_unithead2 {
	uint8_t parityPerField;
	uint8_t ANAC;
	uint16_t prevUnitNo;
	uint16_t virtualUnitNo;
	uint8_t NACs;
	uint8_t discarded;
} __attribute__((packed));

struct inftl_unittail {
	uint8_t Reserved[4];
	uint16_t EraseMark;
	uint16_t EraseMark1;
} __attribute__((packed));

union inftl_uci {
	struct inftl_unithead1 a;
	struct inftl_unithead2 b;
	struct inftl_unittail c;
};

struct inftl_oob {
	struct inftl_bci b;
	union inftl_uci u;
};


/* INFTL Media Header */

struct INFTLPartition {
	__u32 virtualUnits;
	__u32 firstUnit;
	__u32 lastUnit;
	__u32 flags;
	__u32 spareUnits;
	__u32 Reserved0;
	__u32 Reserved1;
} __attribute__((packed));

struct INFTLMediaHeader {
	char bootRecordID[8];
	__u32 NoOfBootImageBlocks;
	__u32 NoOfBinaryPartitions;
	__u32 NoOfBDTLPartitions;
	__u32 BlockMultiplierBits;
	__u32 FormatFlags;
	__u32 OsakVersion;
	__u32 PercentUsed;
	struct INFTLPartition Partitions[4];
} __attribute__((packed));

/* Partition flag types */
#define	INFTL_BINARY	0x20000000
#define	INFTL_BDTL	0x40000000
#define	INFTL_LAST	0x80000000

#endif /* __MTD_INFTL_USER_H__ */
  TKE_CLUSTER_ID: /*
 * $Id: jffs2-user.h,v 1.1 2004/05/05 11:57:54 dwmw2 Exp $
 *
 * JFFS2 definitions for use in user space only
 */

#ifndef __JFFS2_USER_H__
#define __JFFS2_USER_H__

/* This file is blessed for inclusion by userspace */
#include <linux/jffs2.h>
#include <endian.h>
#include <byteswap.h>

#undef cpu_to_je16
#undef cpu_to_je32
#undef cpu_to_jemode
#undef je16_to_cpu
#undef je32_to_cpu
#undef jemode_to_cpu

extern int target_endian;

#define t16(x) ({ uint16_t __b = (x); (target_endian==__BYTE_ORDER)?__b:bswap_16(__b); })
#define t32(x) ({ uint32_t __b = (x); (target_endian==__BYTE_ORDER)?__b:bswap_32(__b); })

#define cpu_to_je16(x) ((jint16_t){t16(x)})
#define cpu_to_je32(x) ((jint32_t){t32(x)})
#define cpu_to_jemode(x) ((jmode_t){t32(x)})

#define je16_to_cpu(x) (t16((x).v16))
#define je32_to_cpu(x) (t32((x).v32))
#define jemode_to_cpu(x) (t32((x).m))

#define le16_to_cpu(x)	(__BYTE_ORDER==__LITTLE_ENDIAN ? (x) : bswap_16(x))
#define le32_to_cpu(x)	(__BYTE_ORDER==__LITTLE_ENDIAN ? (x) : bswap_32(x))
#define cpu_to_le16(x)	(__BYTE_ORDER==__LITTLE_ENDIAN ? (x) : bswap_16(x))
#define cpu_to_le32(x)	(__BYTE_ORDER==__LITTLE_ENDIAN ? (x) : bswap_32(x))

/* XATTR/POSIX-ACL related definition */
/* Namespaces copied from xattr.h and posix_acl_xattr.h */
#define XATTR_USER_PREFIX		"user."
#define XATTR_USER_PREFIX_LEN		(sizeof (XATTR_USER_PREFIX) - 1)
#define XATTR_SECURITY_PREFIX		"security."
#define XATTR_SECURITY_PREFIX_LEN	(sizeof (XATTR_SECURITY_PREFIX) - 1)
#define POSIX_ACL_XATTR_ACCESS		"system.posix_acl_access"
#define POSIX_ACL_XATTR_ACCESS_LEN	(sizeof (POSIX_ACL_XATTR_ACCESS) - 1)
#define POSIX_ACL_XATTR_DEFAULT		"system.posix_acl_default"
#define POSIX_ACL_XATTR_DEFAULT_LEN	(sizeof (POSIX_ACL_XATTR_DEFAULT) - 1)
#define XATTR_TRUSTED_PREFIX		"trusted."
#define XATTR_TRUSTED_PREFIX_LEN	(sizeof (XATTR_TRUSTED_PREFIX) - 1)

struct jffs2_acl_entry {
	jint16_t	e_tag;
	jint16_t	e_perm;
	jint32_t	e_id;
};

struct jffs2_acl_entry_short {
	jint16_t	e_tag;
	jint16_t	e_perm;
};

struct jffs2_acl_header {
	jint32_t	a_version;
};

/* copied from include/linux/posix_acl_xattr.h */
#define POSIX_ACL_XATTR_VERSION 0x0002

struct posix_acl_xattr_entry {
	uint16_t		e_tag;
	uint16_t		e_perm;
	uint32_t		e_id;
};

struct posix_acl_xattr_header {
	uint32_t			a_version;
	struct posix_acl_xattr_entry	a_entries[0];
};

#endif /* __JFFS2_USER_H__ */
  DEPLOYMENT_NAME: /*
 * Copyright © 1999-2010 et al.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __MTD_ABI_H__
#define __MTD_ABI_H__

#include <linux/types.h>

struct erase_info_user {
	__u32 start;
	__u32 length;
};

struct erase_info_user64 {
	__u64 start;
	__u64 length;
};

struct mtd_oob_buf {
	__u32 start;
	__u32 length;
	unsigned char *ptr;
};

struct mtd_oob_buf64 {
	__u64 start;
	__u32 pad;
	__u32 length;
	__u64 usr_ptr;
};

/**
 * MTD operation modes
 *
 * @MTD_OPS_PLACE_OOB:	OOB data are placed at the given offset (default)
 * @MTD_OPS_AUTO_OOB:	OOB data are automatically placed at the free areas
 *			which are defined by the internal ecclayout
 * @MTD_OPS_RAW:	data are transferred as-is, with no error correction;
 *			this mode implies %MTD_OPS_PLACE_OOB
 *
 * These modes can be passed to ioctl(MEMWRITE) and are also used internally.
 * See notes on "MTD file modes" for discussion on %MTD_OPS_RAW vs.
 * %MTD_FILE_MODE_RAW.
 */
enum {
	MTD_OPS_PLACE_OOB = 0,
	MTD_OPS_AUTO_OOB = 1,
	MTD_OPS_RAW = 2,
};

/**
 * struct mtd_write_req - data structure for requesting a write operation
 *
 * @start:	start address
 * @len:	length of data buffer
 * @ooblen:	length of OOB buffer
 * @usr_data:	user-provided data buffer
 * @usr_oob:	user-provided OOB buffer
 * @mode:	MTD mode (see "MTD operation modes")
 * @padding:	reserved, must be set to 0
 *
 * This structure supports ioctl(MEMWRITE) operations, allowing data and/or OOB
 * writes in various modes. To write to OOB-only, set @usr_data == NULL, and to
 * write data-only, set @usr_oob == NULL. However, setting both @usr_data and
 * @usr_oob to NULL is not allowed.
 */
struct mtd_write_req {
	__u64 start;
	__u64 len;
	__u64 ooblen;
	__u64 usr_data;
	__u64 usr_oob;
	__u8 mode;
	__u8 padding[7];
};

#define MTD_ABSENT		0
#define MTD_RAM			1
#define MTD_ROM			2
#define MTD_NORFLASH		3
#define MTD_NANDFLASH		4
#define MTD_DATAFLASH		6
#define MTD_UBIVOLUME		7
#define MTD_MLCNANDFLASH	8

#define MTD_WRITEABLE		0x400	/* Device is writeable */
#define MTD_BIT_WRITEABLE	0x800	/* Single bits can be flipped */
#define MTD_NO_ERASE		0x1000	/* No erase necessary */
#define MTD_POWERUP_LOCK	0x2000	/* Always locked after reset */

/* Some common devices / combinations of capabilities */
#define MTD_CAP_ROM		0
#define MTD_CAP_RAM		(MTD_WRITEABLE | MTD_BIT_WRITEABLE | MTD_NO_ERASE)
#define MTD_CAP_NORFLASH	(MTD_WRITEABLE | MTD_BIT_WRITEABLE)
#define MTD_CAP_NANDFLASH	(MTD_WRITEABLE)

/* Obsolete ECC byte placement modes (used with obsolete MEMGETOOBSEL) */
#define MTD_NANDECC_OFF		0	// Switch off ECC (Not recommended)
#define MTD_NANDECC_PLACE	1	// Use the given placement in the structure (YAFFS1 legacy mode)
#define MTD_NANDECC_AUTOPLACE	2	// Use the default placement scheme
#define MTD_NANDECC_PLACEONLY	3	// Use the given placement in the structure (Do not store ecc result on read)
#define MTD_NANDECC_AUTOPL_USR 	4	// Use the given autoplacement scheme rather than using the default

/* OTP mode selection */
#define MTD_OTP_OFF		0
#define MTD_OTP_FACTORY		1
#define MTD_OTP_USER		2

struct mtd_info_user {
	__u8 type;
	__u32 flags;
	__u32 size;	/* Total size of the MTD */
	__u32 erasesize;
	__u32 writesize;
	__u32 oobsize;	/* Amount of OOB data per block (e.g. 16) */
	__u64 padding;	/* Old obsolete field; do not use */
};

struct region_info_user {
	__u32 offset;		/* At which this region starts,
				 * from the beginning of the MTD */
	__u32 erasesize;	/* For this region */
	__u32 numblocks;	/* Number of blocks in this region */
	__u32 regionindex;
};

struct otp_info {
	__u32 start;
	__u32 length;
	__u32 locked;
};

/*
 * Note, the following ioctl existed in the past and was removed:
 * #define MEMSETOOBSEL           _IOW('M', 9, struct nand_oobinfo)
 * Try to avoid adding a new ioctl with the same ioctl number.
 */

/* Get basic MTD characteristics info (better to use sysfs) */
#define MEMGETINFO		_IOR('M', 1, struct mtd_info_user)
/* Erase segment of MTD */
#define MEMERASE		_IOW('M', 2, struct erase_info_user)
/* Write out-of-band data from MTD */
#define MEMWRITEOOB		_IOWR('M', 3, struct mtd_oob_buf)
/* Read out-of-band data from MTD */
#define MEMREADOOB		_IOWR('M', 4, struct mtd_oob_buf)
/* Lock a chip (for MTD that supports it) */
#define MEMLOCK			_IOW('M', 5, struct erase_info_user)
/* Unlock a chip (for MTD that supports it) */
#define MEMUNLOCK		_IOW('M', 6, struct erase_info_user)
/* Get the number of different erase regions */
#define MEMGETREGIONCOUNT	_IOR('M', 7, int)
/* Get information about the erase region for a specific index */
#define MEMGETREGIONINFO	_IOWR('M', 8, struct region_info_user)
/* Get info about OOB modes (e.g., RAW, PLACE, AUTO) - legacy interface */
#define MEMGETOOBSEL		_IOR('M', 10, struct nand_oobinfo)
/* Check if an eraseblock is bad */
#define MEMGETBADBLOCK		_IOW('M', 11, __kernel_loff_t)
/* Mark an eraseblock as bad */
#define MEMSETBADBLOCK		_IOW('M', 12, __kernel_loff_t)
/* Set OTP (One-Time Programmable) mode (factory vs. user) */
#define OTPSELECT		_IOR('M', 13, int)
/* Get number of OTP (One-Time Programmable) regions */
#define OTPGETREGIONCOUNT	_IOW('M', 14, int)
/* Get all OTP (One-Time Programmable) info about MTD */
#define OTPGETREGIONINFO	_IOW('M', 15, struct otp_info)
/* Lock a given range of user data (must be in mode %MTD_FILE_MODE_OTP_USER) */
#define OTPLOCK			_IOR('M', 16, struct otp_info)
/* Get ECC layout (deprecated) */
#define ECCGETLAYOUT		_IOR('M', 17, struct nand_ecclayout_user)
/* Get statistics about corrected/uncorrected errors */
#define ECCGETSTATS		_IOR('M', 18, struct mtd_ecc_stats)
/* Set MTD mode on a per-file-descriptor basis (see "MTD file modes") */
#define MTDFILEMODE		_IO('M', 19)
/* Erase segment of MTD (supports 64-bit address) */
#define MEMERASE64		_IOW('M', 20, struct erase_info_user64)
/* Write data to OOB (64-bit version) */
#define MEMWRITEOOB64		_IOWR('M', 21, struct mtd_oob_buf64)
/* Read data from OOB (64-bit version) */
#define MEMREADOOB64		_IOWR('M', 22, struct mtd_oob_buf64)
/* Check if chip is locked (for MTD that supports it) */
#define MEMISLOCKED		_IOR('M', 23, struct erase_info_user)
/*
 * Most generic write interface; can write in-band and/or out-of-band in various
 * modes (see "struct mtd_write_req")
 */
#define MEMWRITE		_IOWR('M', 24, struct mtd_write_req)
/* Erase a given range of user data (must be in mode %MTD_FILE_MODE_OTP_USER) */
#define OTPERASE		_IOW('M', 25, struct otp_info)

/*
 * Obsolete legacy interface. Keep it in order not to break userspace
 * interfaces
 */
struct nand_oobinfo {
	__u32 useecc;
	__u32 eccbytes;
	__u32 oobfree[8][2];
	__u32 eccpos[32];
};

struct nand_oobfree {
	__u32 offset;
	__u32 length;
};

#define MTD_MAX_OOBFREE_ENTRIES	8
#define MTD_MAX_ECCPOS_ENTRIES	64
/*
 * OBSOLETE: ECC layout control structure. Exported to user-space via ioctl
 * ECCGETLAYOUT for backwards compatbility and should not be mistaken as a
 * complete set of ECC information. The ioctl truncates the larger internal
 * structure to retain binary compatibility with the static declaration of the
 * ioctl. Note that the "MTD_MAX_..._ENTRIES" macros represent the max size of
 * the user struct, not the MAX size of the internal struct nand_ecclayout.
 */
struct nand_ecclayout_user {
	__u32 eccbytes;
	__u32 eccpos[MTD_MAX_ECCPOS_ENTRIES];
	__u32 oobavail;
	struct nand_oobfree oobfree[MTD_MAX_OOBFREE_ENTRIES];
};

/**
 * struct mtd_ecc_stats - error correction stats
 *
 * @corrected:	number of corrected bits
 * @failed:	number of uncorrectable errors
 * @badblocks:	number of bad blocks in this partition
 * @bbtblocks:	number of blocks reserved for bad block tables
 */
struct mtd_ecc_stats {
	__u32 corrected;
	__u32 failed;
	__u32 badblocks;
	__u32 bbtblocks;
};

/*
 * MTD file modes - for read/write access to MTD
 *
 * @MTD_FILE_MODE_NORMAL:	OTP disabled, ECC enabled
 * @MTD_FILE_MODE_OTP_FACTORY:	OTP enabled in factory mode
 * @MTD_FILE_MODE_OTP_USER:	OTP enabled in user mode
 * @MTD_FILE_MODE_RAW:		OTP disabled, ECC disabled
 *
 * These modes can be set via ioctl(MTDFILEMODE). The mode mode will be retained
 * separately for each open file descriptor.
 *
 * Note: %MTD_FILE_MODE_RAW provides the same functionality as %MTD_OPS_RAW -
 * raw access to the flash, without error correction or autoplacement schemes.
 * Wherever possible, the MTD_OPS_* mode will override the MTD_FILE_MODE_* mode
 * (e.g., when using ioctl(MEMWRITE)), but in some cases, the MTD_FILE_MODE is
 * used out of necessity (e.g., `write()', ioctl(MEMWRITEOOB64)).
 */
enum mtd_file_modes {
	MTD_FILE_MODE_NORMAL = MTD_OTP_OFF,
	MTD_FILE_MODE_OTP_FACTORY = MTD_OTP_FACTORY,
	MTD_FILE_MODE_OTP_USER = MTD_OTP_USER,
	MTD_FILE_MODE_RAW,
};

static inline int mtd_type_is_nand_user(const struct mtd_info_user *mtd)
{
	return mtd->type == MTD_NANDFLASH || mtd->type == MTD_MLCNANDFLASH;
}

#endif /* __MTD_ABI_H__ */

permissions: /*
 * Copyright © 1999-2010 >
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __MTD_USER_H__
#define __MTD_USER_H__

#include <stdint.h>

/* This file is blessed for inclusion by userspace */
#include <mtd/mtd-abi.h>

typedef struct mtd_info_user mtd_info_t;
typedef struct erase_info_user erase_info_t;
typedef struct region_info_user region_info_t;
typedef struct nand_oobinfo nand_oobinfo_t;
typedef struct nand_ecclayout_user nand_ecclayout_t;

#endif /* __MTD_USER_H__ */
  contents: /*
 * $Id: nftl-user.h,v 1.2 2005/11/07 11:14:56 gleixner Exp $
 *
 * Parts of NFTL headers shared with userspace
 *
 */

#ifndef __MTD_NFTL_USER_H__
#define __MTD_NFTL_USER_H__

/* Block Control Information */

struct nftl_bci {
	unsigned char ECCSig[6];
	uint8_t Status;
	uint8_t Status1;
}__attribute__((packed));

/* Unit Control Information */

struct nftl_uci0 {
	uint16_t VirtUnitNum;
	uint16_t ReplUnitNum;
	uint16_t SpareVirtUnitNum;
	uint16_t SpareReplUnitNum;
} __attribute__((packed));

struct nftl_uci1 {
	uint32_t WearInfo;
	uint16_t EraseMark;
	uint16_t EraseMark1;
} __attribute__((packed));

struct nftl_uci2 {
        uint16_t FoldMark;
        uint16_t FoldMark1;
	uint32_t unused;
} __attribute__((packed));

union nftl_uci {
	struct nftl_uci0 a;
	struct nftl_uci1 b;
	struct nftl_uci2 c;
};

struct nftl_oob {
	struct nftl_bci b;
	union nftl_uci u;
};

/* NFTL Media Header */

struct NFTLMediaHeader {
	char DataOrgID[6];
	uint16_t NumEraseUnits;
	uint16_t FirstPhysicalEUN;
	uint32_t FormattedSize;
	unsigned char UnitSizeFactor;
} __attribute__((packed));

#define MAX_ERASE_ZONES (8192 - 512)

#define ERASE_MARK 0x3c69
#define SECTOR_FREE 0xff
#define SECTOR_USED 0x55
#define SECTOR_IGNORE 0x11
#define SECTOR_DELETED 0x00

#define FOLD_MARK_IN_PROGRESS 0x5555

#define ZONE_GOOD 0xff
#define ZONE_BAD_ORIGINAL 0
#define ZONE_BAD_MARKED 7


#endif /* __MTD_NFTL_USER_H__ */

jobs: /*
 * Copyright (c) International Business Machines Corp., 2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * 
 *          
 *          
 *          
 *          
 */

/*
 * This file defines the layout of UBI headers and all the other UBI on-flash
 * data structures.
 */

#ifndef __UBI_MEDIA_H__
#define __UBI_MEDIA_H__

#include <asm/byteorder.h>

/* The version of UBI images supported by this implementation */
#define UBI_VERSION 1

/* The highest erase counter value supported by this implementation */
#define UBI_MAX_ERASECOUNTER 0x7FFFFFFF

/* The initial CRC32 value used when calculating CRC checksums */
#define UBI_CRC32_INIT 0xFFFFFFFFU

/* Erase counter header magic number (ASCII "UBI#") */
#define UBI_EC_HDR_MAGIC  0x55424923
/* Volume identifier header magic number (ASCII "UBI!") */
#define UBI_VID_HDR_MAGIC 0x55424921

/*
 * Volume type constants used in the volume identifier header.
 *
 * @UBI_VID_DYNAMIC: dynamic volume
 * @UBI_VID_STATIC: static volume
 */
enum {
	UBI_VID_DYNAMIC = 1,
	UBI_VID_STATIC  = 2
};

/*
 * Volume flags used in the volume table record.
 *
 * @UBI_VTBL_AUTORESIZE_FLG: auto-resize this volume
 * @UBI_VTBL_SKIP_CRC_CHECK_FLG: skip the CRC check done on a static volume at
 *				 open time. Should only be set on volumes that
 *				 are used by upper layers doing this kind of
 *				 check. Main use-case for this flag is
 *				 boot-time reduction
 *
 * %UBI_VTBL_AUTORESIZE_FLG flag can be set only for one volume in the volume
 * table. UBI automatically re-sizes the volume which has this flag and makes
 * the volume to be of largest possible size. This means that if after the
 * initialization UBI finds out that there are available physical eraseblocks
 * present on the device, it automatically appends all of them to the volume
 * (the physical eraseblocks reserved for bad eraseblocks handling and other
 * reserved physical eraseblocks are not taken). So, if there is a volume with
 * the %UBI_VTBL_AUTORESIZE_FLG flag set, the amount of available logical
 * eraseblocks will be zero after UBI is loaded, because all of them will be
 * reserved for this volume. Note, the %UBI_VTBL_AUTORESIZE_FLG bit is cleared
 * after the volume had been initialized.
 *
 * The auto-resize feature is useful for device production purposes. For
 * example, different NAND flash chips may have different amount of initial bad
 * eraseblocks, depending of particular chip instance. Manufacturers of NAND
 * chips usually guarantee that the amount of initial bad eraseblocks does not
 * exceed certain percent, e.g. 2%. When one creates an UBI image which will be
 * flashed to the end devices in production, he does not know the exact amount
 * of good physical eraseblocks the NAND chip on the device will have, but this
 * number is required to calculate the volume sized and put them to the volume
 * table of the UBI image. In this case, one of the volumes (e.g., the one
 * which will store the root file system) is marked as "auto-resizable", and
 * UBI will adjust its size on the first boot if needed.
 *
 * Note, first UBI reserves some amount of physical eraseblocks for bad
 * eraseblock handling, and then re-sizes the volume, not vice-versa. This
 * means that the pool of reserved physical eraseblocks will always be present.
 */
enum {
	UBI_VTBL_AUTORESIZE_FLG = 0x01,
	UBI_VTBL_SKIP_CRC_CHECK_FLG = 0x02,
};

/*
 * Compatibility constants used by internal volumes.
 *
 * @UBI_COMPAT_DELETE: delete this internal volume before anything is written
 *                     to the flash
 * @UBI_COMPAT_RO: attach this device in read-only mode
 * @UBI_COMPAT_PRESERVE: preserve this internal volume - do not touch its
 *                       physical eraseblocks, don't allow the wear-leveling
 *                       sub-system to move them
 * @UBI_COMPAT_REJECT: reject this UBI image
 */
enum {
	UBI_COMPAT_DELETE   = 1,
	UBI_COMPAT_RO       = 2,
	UBI_COMPAT_PRESERVE = 4,
	UBI_COMPAT_REJECT   = 5
};

/* Sizes of UBI headers */
#define UBI_EC_HDR_SIZE  sizeof(struct ubi_ec_hdr)
#define UBI_VID_HDR_SIZE sizeof(struct ubi_vid_hdr)

/* Sizes of UBI headers without the ending CRC */
#define UBI_EC_HDR_SIZE_CRC  (UBI_EC_HDR_SIZE  - sizeof(__be32))
#define UBI_VID_HDR_SIZE_CRC (UBI_VID_HDR_SIZE - sizeof(__be32))

/**
 * struct ubi_ec_hdr - UBI erase counter header.
 * @magic: erase counter header magic number (%UBI_EC_HDR_MAGIC)
 * @version: version of UBI implementation which is supposed to accept this
 *           UBI image
 * @padding1: reserved for future, zeroes
 * @ec: the erase counter
 * @vid_hdr_offset: where the VID header starts
 * @data_offset: where the user data start
 * @image_seq: image sequence number
 * @padding2: reserved for future, zeroes
 * @hdr_crc: erase counter header CRC checksum
 *
 * The erase counter header takes 64 bytes and has a plenty of unused space for
 * future usage. The unused fields are zeroed. The @version field is used to
 * indicate the version of UBI implementation which is supposed to be able to
 * work with this UBI image. If @version is greater than the current UBI
 * version, the image is rejected. This may be useful in future if something
 * is changed radically. This field is duplicated in the volume identifier
 * header.
 *
 * The @vid_hdr_offset and @data_offset fields contain the offset of the the
 * volume identifier header and user data, relative to the beginning of the
 * physical eraseblock. These values have to be the same for all physical
 * eraseblocks.
 *
 * The @image_seq field is used to validate a UBI image that has been prepared
 * for a UBI device. The @image_seq value can be any value, but it must be the
 * same on all eraseblocks. UBI will ensure that all new erase counter headers
 * also contain this value, and will check the value when scanning at start-up.
 * One way to make use of @image_seq is to increase its value by one every time
 * an image is flashed over an existing image, then, if the flashing does not
 * complete, UBI will detect the error when scanning.
 */
struct ubi_ec_hdr {
	__be32  magic;
	__u8    version;
	__u8    padding1[3];
	__be64  ec; /* Warning: the current limit is 31-bit anyway! */
	__be32  vid_hdr_offset;
	__be32  data_offset;
	__be32  image_seq;
	__u8    padding2[32];
	__be32  hdr_crc;
} __attribute__ ((packed));

/**
 * struct ubi_vid_hdr - on-flash UBI volume identifier header.
 * @magic: volume identifier header magic number (%UBI_VID_HDR_MAGIC)
 * @version: UBI implementation version which is supposed to accept this UBI
 *           image (%UBI_VERSION)
 * @vol_type: volume type (%UBI_VID_DYNAMIC or %UBI_VID_STATIC)
 * @copy_flag: if this logical eraseblock was copied from another physical
 *             eraseblock (for wear-leveling reasons)
 * @compat: compatibility of this volume (%0, %UBI_COMPAT_DELETE,
 *          %UBI_COMPAT_IGNORE, %UBI_COMPAT_PRESERVE, or %UBI_COMPAT_REJECT)
 * @vol_id: ID of this volume
 * @lnum: logical eraseblock number
 * @padding1: reserved for future, zeroes
 * @data_size: how many bytes of data this logical eraseblock contains
 * @used_ebs: total number of used logical eraseblocks in this volume
 * @data_pad: how many bytes at the end of this physical eraseblock are not
 *            used
 * @data_crc: CRC checksum of the data stored in this logical eraseblock
 * @padding2: reserved for future, zeroes
 * @sqnum: sequence number
 * @padding3: reserved for future, zeroes
 * @hdr_crc: volume identifier header CRC checksum
 *
 * The @sqnum is the value of the global sequence counter at the time when this
 * VID header was created. The global sequence counter is incremented each time
 * UBI writes a new VID header to the flash, i.e. when it maps a logical
 * eraseblock to a new physical eraseblock. The global sequence counter is an
 * unsigned 64-bit integer and we assume it never overflows. The @sqnum
 * (sequence number) is used to distinguish between older and newer versions of
 * logical eraseblocks.
 *
 * There are 2 situations when there may be more than one physical eraseblock
 * corresponding to the same logical eraseblock, i.e., having the same @vol_id
 * and @lnum values in the volume identifier header. Suppose we have a logical
 * eraseblock L and it is mapped to the physical eraseblock P.
 *
 * 1. Because UBI may erase physical eraseblocks asynchronously, the following
 * situation is possible: L is asynchronously erased, so P is scheduled for
 * erasure, then L is written to,i.e. mapped to another physical eraseblock P1,
 * so P1 is written to, then an unclean reboot happens. Result - there are 2
 * physical eraseblocks P and P1 corresponding to the same logical eraseblock
 * L. But P1 has greater sequence number, so UBI picks P1 when it attaches the
 * flash.
 *
 * 2. From time to time UBI moves logical eraseblocks to other physical
 * eraseblocks for wear-leveling reasons. If, for example, UBI moves L from P
 * to P1, and an unclean reboot happens before P is physically erased, there
 * are two physical eraseblocks P and P1 corresponding to L and UBI has to
 * select one of them when the flash is attached. The @sqnum field says which
 * PEB is the original (obviously P will have lower @sqnum) and the copy. But
 * it is not enough to select the physical eraseblock with the higher sequence
 * number, because the unclean reboot could have happen in the middle of the
 * copying process, so the data in P is corrupted. It is also not enough to
 * just select the physical eraseblock with lower sequence number, because the
 * data there may be old (consider a case if more data was added to P1 after
 * the copying). Moreover, the unclean reboot may happen when the erasure of P
 * was just started, so it result in unstable P, which is "mostly" OK, but
 * still has unstable bits.
 *
 * UBI uses the @copy_flag field to indicate that this logical eraseblock is a
 * copy. UBI also calculates data CRC when the data is moved and stores it at
 * the @data_crc field of the copy (P1). So when UBI needs to pick one physical
 * eraseblock of two (P or P1), the @copy_flag of the newer one (P1) is
 * examined. If it is cleared, the situation* is simple and the newer one is
 * picked. If it is set, the data CRC of the copy (P1) is examined. If the CRC
 * checksum is correct, this physical eraseblock is selected (P1). Otherwise
 * the older one (P) is selected.
 *
 * There are 2 sorts of volumes in UBI: user volumes and internal volumes.
 * Internal volumes are not seen from outside and are used for various internal
 * UBI purposes. In this implementation there is only one internal volume - the
 * layout volume. Internal volumes are the main mechanism of UBI extensions.
 * For example, in future one may introduce a journal internal volume. Internal
 * volumes have their own reserved range of IDs.
 *
 * The @compat field is only used for internal volumes and contains the "degree
 * of their compatibility". It is always zero for user volumes. This field
 * provides a mechanism to introduce UBI extensions and to be still compatible
 * with older UBI binaries. For example, if someone introduced a journal in
 * future, he would probably use %UBI_COMPAT_DELETE compatibility for the
 * journal volume.  And in this case, older UBI binaries, which know nothing
 * about the journal volume, would just delete this volume and work perfectly
 * fine. This is similar to what Ext2fs does when it is fed by an Ext3fs image
 * - it just ignores the Ext3fs journal.
 *
 * The @data_crc field contains the CRC checksum of the contents of the logical
 * eraseblock if this is a static volume. In case of dynamic volumes, it does
 * not contain the CRC checksum as a rule. The only exception is when the
 * data of the physical eraseblock was moved by the wear-leveling sub-system,
 * then the wear-leveling sub-system calculates the data CRC and stores it in
 * the @data_crc field. And of course, the @copy_flag is %in this case.
 *
 * The @data_size field is used only for static volumes because UBI has to know
 * how many bytes of data are stored in this eraseblock. For dynamic volumes,
 * this field usually contains zero. The only exception is when the data of the
 * physical eraseblock was moved to another physical eraseblock for
 * wear-leveling reasons. In this case, UBI calculates CRC checksum of the
 * contents and uses both @data_crc and @data_size fields. In this case, the
 * @data_size field contains data size.
 *
 * The @used_ebs field is used only for static volumes and indicates how many
 * eraseblocks the data of the volume takes. For dynamic volumes this field is
 * not used and always contains zero.
 *
 * The @data_pad is calculated when volumes are created using the alignment
 * parameter. So, effectively, the @data_pad field reduces the size of logical
 * eraseblocks of this volume. This is very handy when one uses block-oriented
 * software (say, cramfs) on top of the UBI volume.
 */
struct ubi_vid_hdr {
	__be32  magic;
	__u8    version;
	__u8    vol_type;
	__u8    copy_flag;
	__u8    compat;
	__be32  vol_id;
	__be32  lnum;
	__be32  leb_ver;
	__be32  data_size;
	__be32  used_ebs;
	__be32  data_pad;
	__be32  data_crc;
	__u8    padding2[4];
	__be64  sqnum;
	__u8    padding3[12];
	__be32  hdr_crc;
} __attribute__ ((packed));

/* Internal UBI volumes count */
#define UBI_INT_VOL_COUNT 1

/*
 * Starting ID of internal volumes. There is reserved room for 4096 internal
 * volumes.
 */
#define UBI_INTERNAL_VOL_START (0x7FFFFFFF - 4096)

/* The layout volume contains the volume table */

#define UBI_LAYOUT_VOLUME_ID     UBI_INTERNAL_VOL_START
#define UBI_LAYOUT_VOLUME_TYPE   UBI_VID_DYNAMIC
#define UBI_LAYOUT_VOLUME_ALIGN  1
#define UBI_LAYOUT_VOLUME_EBS    2
#define UBI_LAYOUT_VOLUME_NAME   "layout volume"
#define UBI_LAYOUT_VOLUME_COMPAT UBI_COMPAT_REJECT

/* The maximum number of volumes per one UBI device */
#define UBI_MAX_VOLUMES 128

/* The maximum volume name length */
#define UBI_VOL_NAME_MAX 127

/* Size of the volume table record */
#define UBI_VTBL_RECORD_SIZE sizeof(struct ubi_vtbl_record)

/* Size of the volume table record without the ending CRC */
#define UBI_VTBL_RECORD_SIZE_CRC (UBI_VTBL_RECORD_SIZE - sizeof(__be32))

/**
 * struct ubi_vtbl_record - a record in the volume table.
 * @reserved_pebs: how many physical eraseblocks are reserved for this volume
 * @alignment: volume alignment
 * @data_pad: how many bytes are unused at the end of the each physical
 * eraseblock to satisfy the requested alignment
 * @vol_type: volume type (%UBI_DYNAMIC_VOLUME or %UBI_STATIC_VOLUME)
 * @upd_marker: if volume update was started but not finished
 * @name_len: volume name length
 * @name: the volume name
 * @flags: volume flags (%UBI_VTBL_AUTORESIZE_FLG)
 * @padding: reserved, zeroes
 * @crc: a CRC32 checksum of the record
 *
 * The volume table records are stored in the volume table, which is stored in
 * the layout volume. The layout volume consists of 2 logical eraseblock, each
 * of which contains a copy of the volume table (i.e., the volume table is
 * duplicated). The volume table is an array of &struct ubi_vtbl_record
 * objects indexed by the volume ID.
 *
 * If the size of the logical eraseblock is large enough to fit
 * %UBI_MAX_VOLUMES records, the volume table contains %UBI_MAX_VOLUMES
 * records. Otherwise, it contains as many records as it can fit (i.e., size of
 * logical eraseblock divided by sizeof(struct ubi_vtbl_record)).
 *
 * The @upd_marker flag is used to implement volume update. It is set to %1
 * before update and set to %0 after the update. So if the update operation was
 * interrupted, UBI knows that the volume is corrupted.
 *
 * The @alignment field is specified when the volume is created and cannot be
 * later changed. It may be useful, for example, when a block-oriented file
 * system works on top of UBI. The @data_pad field is calculated using the
 * logical eraseblock size and @alignment. The alignment must be multiple to the
 * minimal flash I/O unit. If @alignment is 1, all the available space of
 * the physical eraseblocks is used.
 *
 * Empty records contain all zeroes and the CRC checksum of those zeroes.
 */
struct ubi_vtbl_record {
	__be32  reserved_pebs;
	__be32  alignment;
	__be32  data_pad;
	__u8    vol_type;
	__u8    upd_marker;
	__be16  name_len;
	__u8    name[UBI_VOL_NAME_MAX+1];
	__u8    flags;
	__u8    padding[23];
	__be32  crc;
} __attribute__ ((packed));

#endif /* !__UBI_MEDIA_H__ */
  setup-build-publish-deploy: /*
 * Copyright © International Business Machines Corp., 2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * 
 */

#ifndef __UBI_USER_H__
#define __UBI_USER_H__

/*
 * UBI device creation (the same as MTD device attachment)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * MTD devices may be attached using %UBI_IOCATT ioctl command of the UBI
 * control device. The caller has to properly fill and pass
 * &struct ubi_attach_req object - UBI will attach the MTD device specified in
 * the request and return the newly created UBI device number as the ioctl
 * return value.
 *
 * UBI device deletion (the same as MTD device detachment)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * An UBI device maybe deleted with %UBI_IOCDET ioctl command of the UBI
 * control device.
 *
 * UBI volume creation
 * ~~~~~~~~~~~~~~~~~~~
 *
 * UBI volumes are created via the %UBI_IOCMKVOL ioctl command of UBI character
 * device. A &struct ubi_mkvol_req object has to be properly filled and a
 * pointer to it has to be passed to the ioctl.
 *
 * UBI volume deletion
 * ~~~~~~~~~~~~~~~~~~~
 *
 * To delete a volume, the %UBI_IOCRMVOL ioctl command of the UBI character
 * device should be used. A pointer to the 32-bit volume ID hast to be passed
 * to the ioctl.
 *
 * UBI volume re-size
 * ~~~~~~~~~~~~~~~~~~
 *
 * To re-size a volume, the %UBI_IOCRSVOL ioctl command of the UBI character
 * device should be used. A &struct ubi_rsvol_req object has to be properly
 * filled and a pointer to it has to be passed to the ioctl.
 *
 * UBI volumes re-name
 * ~~~~~~~~~~~~~~~~~~~
 *
 * To re-name several volumes atomically at one go, the %UBI_IOCRNVOL command
 * of the UBI character device should be used. A &struct ubi_rnvol_req object
 * has to be properly filled and a pointer to it has to be passed to the ioctl.
 *
 * UBI volume update
 * ~~~~~~~~~~~~~~~~~
 *
 * Volume update should be done via the %UBI_IOCVOLUP ioctl command of the
 * corresponding UBI volume character device. A pointer to a 64-bit update
 * size should be passed to the ioctl. After this, UBI expects user to write
 * this number of bytes to the volume character device. The update is finished
 * when the claimed number of bytes is passed. So, the volume update sequence
 * is something like:
 *
 * fd = open("/dev/my_volume");
 * ioctl(fd, UBI_IOCVOLUP, &image_size);
 * write(fd, buf, image_size);
 * close(fd);
 *
 * Logical eraseblock erase
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To erase a logical eraseblock, the %UBI_IOCEBER ioctl command of the
 * corresponding UBI volume character device should be used. This command
 * unmaps the requested logical eraseblock, makes sure the corresponding
 * physical eraseblock is successfully erased, and returns.
 *
 * Atomic logical eraseblock change
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Atomic logical eraseblock change operation is called using the %UBI_IOCEBCH
 * ioctl command of the corresponding UBI volume character device. A pointer to
 * a &struct ubi_leb_change_req object has to be passed to the ioctl. Then the
 * user is expected to write the requested amount of bytes (similarly to what
 * should be done in case of the "volume update" ioctl).
 *
 * Logical eraseblock map
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * To map a logical eraseblock to a physical eraseblock, the %UBI_IOCEBMAP
 * ioctl command should be used. A pointer to a &struct ubi_map_req object is
 * expected to be passed. The ioctl maps the requested logical eraseblock to
 * a physical eraseblock and returns.  Only non-mapped logical eraseblocks can
 * be mapped. If the logical eraseblock specified in the request is already
 * mapped to a physical eraseblock, the ioctl fails and returns error.
 *
 * Logical eraseblock unmap
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To unmap a logical eraseblock to a physical eraseblock, the %UBI_IOCEBUNMAP
 * ioctl command should be used. The ioctl unmaps the logical eraseblocks,
 * schedules corresponding physical eraseblock for erasure, and returns. Unlike
 * the "LEB erase" command, it does not wait for the physical eraseblock being
 * erased. Note, the side effect of this is that if an unclean reboot happens
 * after the unmap ioctl returns, you may find the LEB mapped again to the same
 * physical eraseblock after the UBI is run again.
 *
 * Check if logical eraseblock is mapped
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To check if a logical eraseblock is mapped to a physical eraseblock, the
 * %UBI_IOCEBISMAP ioctl command should be used. It returns %0 if the LEB is
 * not mapped, and %1 if it is mapped.
 *
 * Set an UBI volume property
 * ~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To set an UBI volume property the %UBI_IOCSETPROP ioctl command should be
 * used. A pointer to a &struct ubi_set_vol_prop_req object is expected to be
 * passed. The object describes which property should be set, and to which value
 * it should be set.
 *
 * Block devices on UBI volumes
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To create a R/O block device on top of an UBI volume the %UBI_IOCVOLCRBLK
 * should be used. A pointer to a &struct ubi_blkcreate_req object is expected
 * to be passed, which is not used and reserved for future usage.
 *
 * Conversely, to remove a block device the %UBI_IOCVOLRMBLK should be used,
 * which takes no arguments.
 */

/*
 * When a new UBI volume or UBI device is created, users may either specify the
 * volume/device number they want to create or to let UBI automatically assign
 * the number using these constants.
 */
#define UBI_VOL_NUM_AUTO (-1)
#define UBI_DEV_NUM_AUTO (-1)

/* Maximum volume name length */
#define UBI_MAX_VOLUME_NAME 127

/* ioctl commands of UBI character devices */

#define UBI_IOC_MAGIC 'o'

/* Create an UBI volume */
#define UBI_IOCMKVOL _IOW(UBI_IOC_MAGIC, 0, struct ubi_mkvol_req)
/* Remove an UBI volume */
#define UBI_IOCRMVOL _IOW(UBI_IOC_MAGIC, 1, int32_t)
/* Re-size an UBI volume */
#define UBI_IOCRSVOL _IOW(UBI_IOC_MAGIC, 2, struct ubi_rsvol_req)
/* Re-name volumes */
#define UBI_IOCRNVOL _IOW(UBI_IOC_MAGIC, 3, struct ubi_rnvol_req)

/* ioctl commands of the UBI control character device */

#define UBI_CTRL_IOC_MAGIC 'o'

/* Attach an MTD device */
#define UBI_IOCATT _IOW(UBI_CTRL_IOC_MAGIC, 64, struct ubi_attach_req)
/* Detach an MTD device */
#define UBI_IOCDET _IOW(UBI_CTRL_IOC_MAGIC, 65, int32_t)

/* ioctl commands of UBI volume character devices */

#define UBI_VOL_IOC_MAGIC 'O'

/* Start UBI volume update */
#define UBI_IOCVOLUP _IOW(UBI_VOL_IOC_MAGIC, 0, int64_t)
/* LEB erasure command, used for debugging, disabled by default */
#define UBI_IOCEBER _IOW(UBI_VOL_IOC_MAGIC, 1, int32_t)
/* Atomic LEB change command */
#define UBI_IOCEBCH _IOW(UBI_VOL_IOC_MAGIC, 2, int32_t)
/* Map LEB command */
#define UBI_IOCEBMAP _IOW(UBI_VOL_IOC_MAGIC, 3, struct ubi_map_req)
/* Unmap LEB command */
#define UBI_IOCEBUNMAP _IOW(UBI_VOL_IOC_MAGIC, 4, int32_t)
/* Check if LEB is mapped command */
#define UBI_IOCEBISMAP _IOR(UBI_VOL_IOC_MAGIC, 5, int32_t)
/* Set an UBI volume property */
#define UBI_IOCSETVOLPROP _IOW(UBI_VOL_IOC_MAGIC, 6, \
			       struct ubi_set_vol_prop_req)
/* Create a R/O block device on top of an UBI volume */
#define UBI_IOCVOLCRBLK _IOW(UBI_VOL_IOC_MAGIC, 7, struct ubi_blkcreate_req)
/* Remove the R/O block device */
#define UBI_IOCVOLRMBLK _IO(UBI_VOL_IOC_MAGIC, 8)

/* Maximum MTD device name length supported by UBI */
#define MAX_UBI_MTD_NAME_LEN 127

/* Maximum amount of UBI volumes that can be re-named at one go */
#define UBI_MAX_RNVOL 32

/*
 * UBI volume type constants.
 *
 * @UBI_DYNAMIC_VOLUME: dynamic volume
 * @UBI_STATIC_VOLUME:  static volume
 */
enum {
	UBI_DYNAMIC_VOLUME = 3,
	UBI_STATIC_VOLUME  = 4,
};

/*
 * UBI set volume property ioctl constants.
 *
 * @UBI_VOL_PROP_DIRECT_WRITE: allow (any non-zero value) or disallow (value 0)
 *                             user to directly write and erase individual
 *                             eraseblocks on dynamic volumes
 */
enum {
	UBI_VOL_PROP_DIRECT_WRITE = 1,
};

/**
 * struct ubi_attach_req - attach MTD device request.
 * @ubi_num: UBI device number to create
 * @mtd_num: MTD device number to attach
 * @vid_hdr_offset: VID header offset (use defaults if %0)
 * @max_beb_per1024: maximum expected number of bad PEB per 1024 PEBs
 * @disable_fm: whether disable fastmap
 * @need_resv_pool: whether reserve free pebs for filling pool/wl_pool
 * @padding: reserved for future, not used, has to be zeroed
 *
 * This data structure is used to specify MTD device UBI has to attach and the
 * parameters it has to use. The number which should be assigned to the new UBI
 * device is passed in @ubi_num. UBI may automatically assign the number if
 * @UBI_DEV_NUM_AUTO is passed. In this case, the device number is returned in
 * @ubi_num.
 *
 * Most applications should pass %0 in @vid_hdr_offset to make UBI use default
 * offset of the VID header within physical eraseblocks. The default offset is
 * the next min. I/O unit after the EC header. For example, it will be offset
 * 512 in case of a 512 bytes page NAND flash with no sub-page support. Or
 * it will be 512 in case of a 2KiB page NAND flash with 4 512-byte sub-pages.
 *
 * But in rare cases, if this optimizes things, the VID header may be placed to
 * a different offset. For example, the boot-loader might do things faster if
 * the VID header sits at the end of the first 2KiB NAND page with 4 sub-pages.
 * As the boot-loader would not normally need to read EC headers (unless it
 * needs UBI in RW mode), it might be faster to calculate ECC. This is weird
 * example, but it real-life example. So, in this example, @vid_hdr_offer would
 * be 2KiB-64 bytes = 1984. Note, that this position is not even 512-bytes
 * aligned, which is OK, as UBI is clever enough to realize this is 4th
 * sub-page of the first page and add needed padding.
 *
 * The @max_beb_per1024 is the maximum amount of bad PEBs UBI expects on the
 * UBI device per 1024 eraseblocks.  This value is often given in an other form
 * in the NAND datasheet (min NVB i.e. minimal number of valid blocks). The
 * maximum expected bad eraseblocks per 1024 is then:
 *    1024 * (1 - MinNVB / MaxNVB)
 * Which gives 20 for most NAND devices.  This limit is used in order to derive
 * amount of eraseblock UBI reserves for handling new bad blocks. If the device
 * has more bad eraseblocks than this limit, UBI does not reserve any physical
 * eraseblocks for new bad eraseblocks, but attempts to use available
 * eraseblocks (if any). The accepted range is 0-768. If 0 is given, the
 * default kernel value of %CONFIG_MTD_UBI_BEB_LIMIT will be used.
 *
 * If @disable_fm is not zero, ubi doesn't create new fastmap even the module
 * param 'fm_autoconvert' is set, and existed old fastmap will be destroyed
 * after doing full scanning.
 */
struct ubi_attach_req {
	int32_t ubi_num;
	int32_t mtd_num;
	int32_t vid_hdr_offset;
	int16_t max_beb_per1024;
	int8_t	disable_fm;
	int8_t  need_resv_pool;
	int8_t  padding[8];
};

/*
 * UBI volume flags.
 *
 * @UBI_VOL_SKIP_CRC_CHECK_FLG: skip the CRC check done on a static volume at
 *				open time. Only valid for static volumes and
 *				should only be used if the volume user has a
 *				way to verify data integrity
 */
enum {
	UBI_VOL_SKIP_CRC_CHECK_FLG = 0x1,
};

/**
 * struct ubi_mkvol_req - volume description data structure used in
 *                        volume creation requests.
 * @vol_id: volume number
 * @alignment: volume alignment
 * @bytes: volume size in bytes
 * @vol_type: volume type (%UBI_DYNAMIC_VOLUME or %UBI_STATIC_VOLUME)
 * @flags: volume flags (%UBI_VOL_SKIP_CRC_CHECK_FLG)
 * @name_len: volume name length
 * @padding2: reserved for future, not used, has to be zeroed
 * @name: volume name
 *
 * This structure is used by user-space programs when creating new volumes. The
 * @used_bytes field is only necessary when creating static volumes.
 *
 * The @alignment field specifies the required alignment of the volume logical
 * eraseblock. This means, that the size of logical eraseblocks will be aligned
 * to this number, i.e.,
 *	(UBI device logical eraseblock size) mod (@alignment) = 0.
 *
 * To put it differently, the logical eraseblock of this volume may be slightly
 * shortened in order to make it properly aligned. The alignment has to be
 * multiple of the flash minimal input/output unit, or %1 to utilize the entire
 * available space of logical eraseblocks.
 *
 * The @alignment field may be useful, for example, when one wants to maintain
 * a block device on top of an UBI volume. In this case, it is desirable to fit
 * an integer number of blocks in logical eraseblocks of this UBI volume. With
 * alignment it is possible to update this volume using plane UBI volume image
 * BLOBs, without caring about how to properly align them.
 */
struct ubi_mkvol_req {
	int32_t vol_id;
	int32_t alignment;
	int64_t bytes;
	int8_t vol_type;
	uint8_t flags;
	int16_t name_len;
	int8_t padding2[4];
	char name[UBI_MAX_VOLUME_NAME + 1];
} __attribute__((packed));

/**
 * struct ubi_rsvol_req - a data structure used in volume re-size requests.
 * @vol_id: ID of the volume to re-size
 * @bytes: new size of the volume in bytes
 *
 * Re-sizing is possible for both dynamic and static volumes. But while dynamic
 * volumes may be re-sized arbitrarily, static volumes cannot be made to be
 * smaller than the number of bytes they bear. To arbitrarily shrink a static
 * volume, it must be wiped out first (by means of volume update operation with
 * zero number of bytes).
 */
struct ubi_rsvol_req {
	int64_t bytes;
	int32_t vol_id;
} __attribute__((packed));

/**
 * struct ubi_rnvol_req - volumes re-name request.
 * @count: count of volumes to re-name
 * @padding1:  reserved for future, not used, has to be zeroed
 * @vol_id: ID of the volume to re-name
 * @name_len: name length
 * @padding2:  reserved for future, not used, has to be zeroed
 * @name: new volume name
 *
 * UBI allows to re-name up to %32 volumes at one go. The count of volumes to
 * re-name is specified in the @count field. The ID of the volumes to re-name
 * and the new names are specified in the @vol_id and @name fields.
 *
 * The UBI volume re-name operation is atomic, which means that should power cut
 * happen, the volumes will have either old name or new name. So the possible
 * use-cases of this command is atomic upgrade. Indeed, to upgrade, say, volumes
 * A and B one may create temporary volumes %A1 and %B1 with the new contents,
 * then atomically re-name A1->A and B1->B, in which case old %A and %B will
 * be removed.
 *
 * If it is not desirable to remove old A and B, the re-name request has to
 * contain 4 entries: A1->A, A->A1, B1->B, B->B1, in which case old A1 and B1
 * become A and B, and old A and B will become A1 and B1.
 *
 * It is also OK to request: A1->A, A1->X, B1->B, B->Y, in which case old A1
 * and B1 become A and B, and old A and B become X and Y.
 *
 * In other words, in case of re-naming into an existing volume name, the
 * existing volume is removed, unless it is re-named as well at the same
 * re-name request.
 */
struct ubi_rnvol_req {
	int32_t count;
	int8_t padding1[12];
	struct {
		int32_t vol_id;
		int16_t name_len;
		int8_t  padding2[2];
		char    name[UBI_MAX_VOLUME_NAME + 1];
	} ents[UBI_MAX_RNVOL];
} __attribute__((packed));

/**
 * struct ubi_leb_change_req - a data structure used in atomic LEB change
 *                             requests.
 * @lnum: logical eraseblock number to change
 * @bytes: how many bytes will be written to the logical eraseblock
 * @dtype: pass "3" for better compatibility with old kernels
 * @padding: reserved for future, not used, has to be zeroed
 *
 * The @dtype field used to inform UBI about what kind of data will be written
 * to the LEB: long term (value 1), short term (value 2), unknown (value 3).
 * UBI tried to pick a PEB with lower erase counter for short term data and a
 * PEB with higher erase counter for long term data. But this was not really
 * used because users usually do not know this and could easily mislead UBI. We
 * removed this feature in May 2012. UBI currently just ignores the @dtype
 * field. But for better compatibility with older kernels it is recommended to
 * set @dtype to 3 (unknown).
 */
struct ubi_leb_change_req {
	int32_t lnum;
	int32_t bytes;
	int8_t  dtype; /* obsolete, do not use! */
	int8_t  padding[7];
} __attribute__((packed));

/**
 * struct ubi_map_req - a data structure used in map LEB requests.
 * @dtype: pass "3" for better compatibility with old kernels
 * @lnum: logical eraseblock number to unmap
 * @padding: reserved for future, not used, has to be zeroed
 */
struct ubi_map_req {
	int32_t lnum;
	int8_t  dtype; /* obsolete, do not use! */
	int8_t  padding[3];
} __attribute__((packed));


/**
 * struct ubi_set_vol_prop_req - a data structure used to set an UBI volume
 *                               property.
 * @property: property to set (%UBI_VOL_PROP_DIRECT_WRITE)
 * @padding: reserved for future, not used, has to be zeroed
 * @value: value to set
 */
struct ubi_set_vol_prop_req {
	uint8_t  property;
	uint8_t  padding[7];
	uint64_t value;
}  __attribute__((packed));

/**
 * struct ubi_blkcreate_req - a data structure used in block creation requests.
 * @padding: reserved for future, not used, has to be zeroed
 */
struct ubi_blkcreate_req {
	int8_t  padding[128];
}  __attribute__((packed));

#endif /* __UBI_USER_H__ */
    name: /*
 * This file is part of UBIFS.
 *
 * Copyright (C) 2006-2008 Nokia Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * 
 *          
 */

/*
 * This file describes UBIFS on-flash format and contains definitions of all the
 * relevant data structures and constants.
 *
 * All UBIFS on-flash objects are stored in the form of nodes. All nodes start
 * with the UBIFS node magic number and have the same common header. Nodes
 * always sit at 8-byte aligned positions on the media and node header sizes are
 * also 8-byte aligned (except for the indexing node and the padding node).
 */

#ifndef __UBIFS_MEDIA_H__
#define __UBIFS_MEDIA_H__

#include <asm/byteorder.h>

/* UBIFS node magic number (must not have the padding byte first or last) */
#define UBIFS_NODE_MAGIC  0x06101831

/*
 * UBIFS on-flash format version. This version is increased when the on-flash
 * format is changing. If this happens, UBIFS is will support older versions as
 * well. But older UBIFS code will not support newer formats. Format changes
 * will be rare and only when absolutely necessary, e.g. to fix a bug or to add
 * a new feature.
 *
 * UBIFS went into mainline kernel with format version 4. The older formats
 * were development formats.
 */
#define UBIFS_FORMAT_VERSION 5

/*
 * Read-only compatibility version. If the UBIFS format is changed, older UBIFS
 * implementations will not be able to mount newer formats in read-write mode.
 * However, depending on the change, it may be possible to mount newer formats
 * in R/O mode. This is indicated by the R/O compatibility version which is
 * stored in the super-block.
 *
 * This is needed to support boot-loaders which only need R/O mounting. With
 * this flag it is possible to do UBIFS format changes without a need to update
 * boot-loaders.
 */
#define UBIFS_RO_COMPAT_VERSION 0

/* Minimum logical eraseblock size in bytes */
#define UBIFS_MIN_LEB_SZ (15*1024)

/* Initial CRC32 value used when calculating CRC checksums */
#define UBIFS_CRC32_INIT 0xFFFFFFFFU

/*
 * UBIFS does not try to compress data if its length is less than the below
 * constant.
 */
#define UBIFS_MIN_COMPR_LEN 128

/*
 * If compressed data length is less than %UBIFS_MIN_COMPRESS_DIFF bytes
 * shorter than uncompressed data length, UBIFS prefers to leave this data
 * node uncompress, because it'll be read faster.
 */
#define UBIFS_MIN_COMPRESS_DIFF 64

/* Root inode number */
#define UBIFS_ROOT_INO 1

/* Lowest inode number used for regular inodes (not UBIFS-only internal ones) */
#define UBIFS_FIRST_INO 64

/*
 * Maximum file name and extended attribute length (must be a multiple of 8,
 * minus 1).
 */
#define UBIFS_MAX_NLEN 255

/* Maximum number of data journal heads */
#define UBIFS_MAX_JHEADS 1

/*
 * Size of UBIFS data block. Note, UBIFS is not a block oriented file-system,
 * which means that it does not treat the underlying media as consisting of
 * blocks like in case of hard drives. Do not be confused. UBIFS block is just
 * the maximum amount of data which one data node can have or which can be
 * attached to an inode node.
 */
#define UBIFS_BLOCK_SIZE  4096
#define UBIFS_BLOCK_SHIFT 12

/* UBIFS padding byte pattern (must not be first or last byte of node magic) */
#define UBIFS_PADDING_BYTE 0xCE

/* Maximum possible key length */
#define UBIFS_MAX_KEY_LEN 16

/* Key length ("simple" format) */
#define UBIFS_SK_LEN 8

/* Minimum index tree fanout */
#define UBIFS_MIN_FANOUT 3

/* Maximum number of levels in UBIFS indexing B-tree */
#define UBIFS_MAX_LEVELS 512

/* Maximum amount of data attached to an inode in bytes */
#define UBIFS_MAX_INO_DATA UBIFS_BLOCK_SIZE

/* LEB Properties Tree fanout (must be power of 2) and fanout shift */
#define UBIFS_LPT_FANOUT 4
#define UBIFS_LPT_FANOUT_SHIFT 2

/* LEB Properties Tree bit field sizes */
#define UBIFS_LPT_CRC_BITS 16
#define UBIFS_LPT_CRC_BYTES 2
#define UBIFS_LPT_TYPE_BITS 4

/* The key is always at the same position in all keyed nodes */
#define UBIFS_KEY_OFFSET offsetof(struct ubifs_ino_node, key)

/* Garbage collector journal head number */
#define UBIFS_GC_HEAD   0
/* Base journal head number */
#define UBIFS_BASE_HEAD 1
/* Data journal head number */
#define UBIFS_DATA_HEAD 2

/*
 * LEB Properties Tree node types.
 *
 * UBIFS_LPT_PNODE: LPT leaf node (contains LEB properties)
 * UBIFS_LPT_NNODE: LPT internal node
 * UBIFS_LPT_LTAB: LPT's own lprops table
 * UBIFS_LPT_LSAVE: LPT's save table (big model only)
 * UBIFS_LPT_NODE_CNT: count of LPT node types
 * UBIFS_LPT_NOT_A_NODE: all ones (15 for 4 bits) is never a valid node type
 */
enum {
	UBIFS_LPT_PNODE,
	UBIFS_LPT_NNODE,
	UBIFS_LPT_LTAB,
	UBIFS_LPT_LSAVE,
	UBIFS_LPT_NODE_CNT,
	UBIFS_LPT_NOT_A_NODE = (1 << UBIFS_LPT_TYPE_BITS) - 1,
};

/*
 * UBIFS inode types.
 *
 * UBIFS_ITYPE_REG: regular file
 * UBIFS_ITYPE_DIR: directory
 * UBIFS_ITYPE_LNK: soft link
 * UBIFS_ITYPE_BLK: block device node
 * UBIFS_ITYPE_CHR: character device node
 * UBIFS_ITYPE_FIFO: fifo
 * UBIFS_ITYPE_SOCK: socket
 * UBIFS_ITYPES_CNT: count of supported file types
 */
enum {
	UBIFS_ITYPE_REG,
	UBIFS_ITYPE_DIR,
	UBIFS_ITYPE_LNK,
	UBIFS_ITYPE_BLK,
	UBIFS_ITYPE_CHR,
	UBIFS_ITYPE_FIFO,
	UBIFS_ITYPE_SOCK,
	UBIFS_ITYPES_CNT,
};

/*
 * Supported key hash functions.
 *
 * UBIFS_KEY_HASH_R5: R5 hash
 * UBIFS_KEY_HASH_TEST: test hash which just returns first 4 bytes of the name
 */
enum {
	UBIFS_KEY_HASH_R5,
	UBIFS_KEY_HASH_TEST,
};

/*
 * Supported key formats.
 *
 * UBIFS_SIMPLE_KEY_FMT: simple key format
 */
enum {
	UBIFS_SIMPLE_KEY_FMT,
};

/*
 * The simple key format uses 29 bits for storing UBIFS block number and hash
 * value.
 */
#define UBIFS_S_KEY_BLOCK_BITS 29
#define UBIFS_S_KEY_BLOCK_MASK 0x1FFFFFFF
#define UBIFS_S_KEY_HASH_BITS  UBIFS_S_KEY_BLOCK_BITS
#define UBIFS_S_KEY_HASH_MASK  UBIFS_S_KEY_BLOCK_MASK

/*
 * Key types.
 *
 * UBIFS_INO_KEY: inode node key
 * UBIFS_DATA_KEY: data node key
 * UBIFS_DENT_KEY: directory entry node key
 * UBIFS_XENT_KEY: extended attribute entry key
 * UBIFS_KEY_TYPES_CNT: number of supported key types
 */
enum {
	UBIFS_INO_KEY,
	UBIFS_DATA_KEY,
	UBIFS_DENT_KEY,
	UBIFS_XENT_KEY,
	UBIFS_KEY_TYPES_CNT,
};

/* Count of LEBs reserved for the superblock area */
#define UBIFS_SB_LEBS 1
/* Count of LEBs reserved for the master area */
#define UBIFS_MST_LEBS 2

/* First LEB of the superblock area */
#define UBIFS_SB_LNUM 0
/* First LEB of the master area */
#define UBIFS_MST_LNUM (UBIFS_SB_LNUM + UBIFS_SB_LEBS)
/* First LEB of the log area */
#define UBIFS_LOG_LNUM (UBIFS_MST_LNUM + UBIFS_MST_LEBS)

/*
 * The below constants define the absolute minimum values for various UBIFS
 * media areas. Many of them actually depend of flash geometry and the FS
 * configuration (number of journal heads, orphan LEBs, etc). This means that
 * the smallest volume size which can be used for UBIFS cannot be pre-defined
 * by these constants. The file-system that meets the below limitation will not
 * necessarily mount. UBIFS does run-time calculations and validates the FS
 * size.
 */

/* Minimum number of logical eraseblocks in the log */
#define UBIFS_MIN_LOG_LEBS 2
/* Minimum number of bud logical eraseblocks (one for each head) */
#define UBIFS_MIN_BUD_LEBS 3
/* Minimum number of journal logical eraseblocks */
#define UBIFS_MIN_JNL_LEBS (UBIFS_MIN_LOG_LEBS + UBIFS_MIN_BUD_LEBS)
/* Minimum number of LPT area logical eraseblocks */
#define UBIFS_MIN_LPT_LEBS 2
/* Minimum number of orphan area logical eraseblocks */
#define UBIFS_MIN_ORPH_LEBS 1
/*
 * Minimum number of main area logical eraseblocks (buds, 3 for the index, 1
 * for GC, 1 for deletions, and at least 1 for committed data).
 */
#define UBIFS_MIN_MAIN_LEBS (UBIFS_MIN_BUD_LEBS + 6)

/* Minimum number of logical eraseblocks */
#define UBIFS_MIN_LEB_CNT (UBIFS_SB_LEBS + UBIFS_MST_LEBS + \
			   UBIFS_MIN_LOG_LEBS + UBIFS_MIN_LPT_LEBS + \
			   UBIFS_MIN_ORPH_LEBS + UBIFS_MIN_MAIN_LEBS)

/* Node sizes (N.B. these are guaranteed to be multiples of 8) */
#define UBIFS_CH_SZ        sizeof(struct ubifs_ch)
#define UBIFS_INO_NODE_SZ  sizeof(struct ubifs_ino_node)
#define UBIFS_DATA_NODE_SZ sizeof(struct ubifs_data_node)
#define UBIFS_DENT_NODE_SZ sizeof(struct ubifs_dent_node)
#define UBIFS_TRUN_NODE_SZ sizeof(struct ubifs_trun_node)
#define UBIFS_PAD_NODE_SZ  sizeof(struct ubifs_pad_node)
#define UBIFS_SB_NODE_SZ   sizeof(struct ubifs_sb_node)
#define UBIFS_MST_NODE_SZ  sizeof(struct ubifs_mst_node)
#define UBIFS_REF_NODE_SZ  sizeof(struct ubifs_ref_node)
#define UBIFS_IDX_NODE_SZ  sizeof(struct ubifs_idx_node)
#define UBIFS_CS_NODE_SZ   sizeof(struct ubifs_cs_node)
#define UBIFS_ORPH_NODE_SZ sizeof(struct ubifs_orph_node)
#define UBIFS_AUTH_NODE_SZ sizeof(struct ubifs_auth_node)
#define UBIFS_SIG_NODE_SZ  sizeof(struct ubifs_sig_node)

/* Extended attribute entry nodes are identical to directory entry nodes */
#define UBIFS_XENT_NODE_SZ UBIFS_DENT_NODE_SZ
/* Only this does not have to be multiple of 8 bytes */
#define UBIFS_BRANCH_SZ    sizeof(struct ubifs_branch)

/* Maximum node sizes (N.B. these are guaranteed to be multiples of 8) */
#define UBIFS_MAX_DATA_NODE_SZ  (UBIFS_DATA_NODE_SZ + UBIFS_BLOCK_SIZE)
#define UBIFS_MAX_INO_NODE_SZ   (UBIFS_INO_NODE_SZ + UBIFS_MAX_INO_DATA)
#define UBIFS_MAX_DENT_NODE_SZ  (UBIFS_DENT_NODE_SZ + UBIFS_MAX_NLEN + 1)
#define UBIFS_MAX_XENT_NODE_SZ  UBIFS_MAX_DENT_NODE_SZ

/* The largest UBIFS node */
#define UBIFS_MAX_NODE_SZ UBIFS_MAX_INO_NODE_SZ

/* The maxmimum size of a hash, enough for sha512 */
#define UBIFS_MAX_HASH_LEN 64

/* The maxmimum size of a hmac, enough for hmac(sha512) */
#define UBIFS_MAX_HMAC_LEN 64

/*
 * xattr name of UBIFS encryption context, we don't use a prefix
 * nor a long name to not waste space on the flash.
 */
#define UBIFS_XATTR_NAME_ENCRYPTION_CONTEXT "c"

/* Type field in ubifs_sig_node */
#define UBIFS_SIGNATURE_TYPE_PKCS7	1

/*
 * On-flash inode flags.
 *
 * UBIFS_COMPR_FL: use compression for this inode
 * UBIFS_SYNC_FL:  I/O on this inode has to be synchronous
 * UBIFS_IMMUTABLE_FL: inode is immutable
 * UBIFS_APPEND_FL: writes to the inode may only append data
 * UBIFS_DIRSYNC_FL: I/O on this directory inode has to be synchronous
 * UBIFS_XATTR_FL: this inode is the inode for an extended attribute value
 * UBIFS_CRYPT_FL: use encryption for this inode
 *
 * Note, these are on-flash flags which correspond to ioctl flags
 * (@FS_COMPR_FL, etc). They have the same values now, but generally, do not
 * have to be the same.
 */
enum {
	UBIFS_COMPR_FL     = 0x01,
	UBIFS_SYNC_FL      = 0x02,
	UBIFS_IMMUTABLE_FL = 0x04,
	UBIFS_APPEND_FL    = 0x08,
	UBIFS_DIRSYNC_FL   = 0x10,
	UBIFS_XATTR_FL     = 0x20,
	UBIFS_CRYPT_FL     = 0x40,
};

/* Inode flag bits used by UBIFS */
#define UBIFS_FL_MASK 0x0000001F

/*
 * UBIFS compression algorithms.
 *
 * UBIFS_COMPR_NONE: no compression
 * UBIFS_COMPR_LZO: LZO compression
 * UBIFS_COMPR_ZLIB: ZLIB compression
 * UBIFS_COMPR_ZSTD: ZSTD compression
 * UBIFS_COMPR_TYPES_CNT: count of supported compression types
 */
enum {
	UBIFS_COMPR_NONE,
	UBIFS_COMPR_LZO,
	UBIFS_COMPR_ZLIB,
	UBIFS_COMPR_ZSTD,
	UBIFS_COMPR_TYPES_CNT,
};

/*
 * UBIFS node types.
 *
 * UBIFS_INO_NODE: inode node
 * UBIFS_DATA_NODE: data node
 * UBIFS_DENT_NODE: directory entry node
 * UBIFS_XENT_NODE: extended attribute node
 * UBIFS_TRUN_NODE: truncation node
 * UBIFS_PAD_NODE: padding node
 * UBIFS_SB_NODE: superblock node
 * UBIFS_MST_NODE: master node
 * UBIFS_REF_NODE: LEB reference node
 * UBIFS_IDX_NODE: index node
 * UBIFS_CS_NODE: commit start node
 * UBIFS_ORPH_NODE: orphan node
 * UBIFS_AUTH_NODE: authentication node
 * UBIFS_SIG_NODE: signature node
 * UBIFS_NODE_TYPES_CNT: count of supported node types
 *
 * Note, we index arrays by these numbers, so keep them low and contiguous.
 * Node type constants for inodes, direntries and so on have to be the same as
 * corresponding key type constants.
 */
enum {
	UBIFS_INO_NODE,
	UBIFS_DATA_NODE,
	UBIFS_DENT_NODE,
	UBIFS_XENT_NODE,
	UBIFS_TRUN_NODE,
	UBIFS_PAD_NODE,
	UBIFS_SB_NODE,
	UBIFS_MST_NODE,
	UBIFS_REF_NODE,
	UBIFS_IDX_NODE,
	UBIFS_CS_NODE,
	UBIFS_ORPH_NODE,
	UBIFS_AUTH_NODE,
	UBIFS_SIG_NODE,
	UBIFS_NODE_TYPES_CNT,
};

/*
 * Master node flags.
 *
 * UBIFS_MST_DIRTY: rebooted uncleanly - master node is dirty
 * UBIFS_MST_NO_ORPHS: no orphan inodes present
 * UBIFS_MST_RCVRY: written by recovery
 */
enum {
	UBIFS_MST_DIRTY = 1,
	UBIFS_MST_NO_ORPHS = 2,
	UBIFS_MST_RCVRY = 4,
};

/*
 * Node group type (used by recovery to recover whole group or none).
 *
 * UBIFS_NO_NODE_GROUP: this node is not part of a group
 * UBIFS_IN_NODE_GROUP: this node is a part of a group
 * UBIFS_LAST_OF_NODE_GROUP: this node is the last in a group
 */
enum {
	UBIFS_NO_NODE_GROUP = 0,
	UBIFS_IN_NODE_GROUP,
	UBIFS_LAST_OF_NODE_GROUP,
};

/*
 * Superblock flags.
 *
 * UBIFS_FLG_BIGLPT: if "big" LPT model is used if set
 * UBIFS_FLG_SPACE_FIXUP: first-mount "fixup" of free space within LEBs needed
 * UBIFS_FLG_DOUBLE_HASH: store a 32bit cookie in directory entry nodes to
 *			  support 64bit cookies for lookups by hash
 * UBIFS_FLG_ENCRYPTION: this filesystem contains encrypted files
 * UBIFS_FLG_AUTHENTICATION: this filesystem contains hashes for authentication
 */
enum {
	UBIFS_FLG_BIGLPT = 0x02,
	UBIFS_FLG_SPACE_FIXUP = 0x04,
	UBIFS_FLG_DOUBLE_HASH = 0x08,
	UBIFS_FLG_ENCRYPTION = 0x10,
	UBIFS_FLG_AUTHENTICATION = 0x20,
};

#define UBIFS_FLG_MASK (UBIFS_FLG_BIGLPT | UBIFS_FLG_SPACE_FIXUP | \
		UBIFS_FLG_DOUBLE_HASH | UBIFS_FLG_ENCRYPTION | \
		UBIFS_FLG_AUTHENTICATION)

/**
 * struct ubifs_ch - common header node.
 * @magic: UBIFS node magic number (%UBIFS_NODE_MAGIC)
 * @crc: CRC-32 checksum of the node header
 * @sqnum: sequence number
 * @len: full node length
 * @node_type: node type
 * @group_type: node group type
 * @padding: reserved for future, zeroes
 *
 * Every UBIFS node starts with this common part. If the node has a key, the
 * key always goes next.
 */
struct ubifs_ch {
	__le32 magic;
	__le32 crc;
	__le64 sqnum;
	__le32 len;
	__u8 node_type;
	__u8 group_type;
	__u8 padding[2];
} __attribute__ ((packed));

/**
 * union ubifs_dev_desc - device node descriptor.
 * @new: new type device descriptor
 * @huge: huge type device descriptor
 *
 * This data structure describes major/minor numbers of a device node. In an
 * inode is a device node then its data contains an object of this type. UBIFS
 * uses standard Linux "new" and "huge" device node encodings.
 */
union ubifs_dev_desc {
	__le32 new;
	__le64 huge;
} __attribute__ ((packed));

/**
 * struct ubifs_ino_node - inode node.
 * @ch: common header
 * @key: node key
 * @creat_sqnum: sequence number at time of creation
 * @size: inode size in bytes (amount of uncompressed data)
 * @atime_sec: access time seconds
 * @ctime_sec: creation time seconds
 * @mtime_sec: modification time seconds
 * @atime_nsec: access time nanoseconds
 * @ctime_nsec: creation time nanoseconds
 * @mtime_nsec: modification time nanoseconds
 * @nlink: number of hard links
 * @uid: owner ID
 * @gid: group ID
 * @mode: access flags
 * @flags: per-inode flags (%UBIFS_COMPR_FL, %UBIFS_SYNC_FL, etc)
 * @data_len: inode data length
 * @xattr_cnt: count of extended attributes this inode has
 * @xattr_size: summarized size of all extended attributes in bytes
 * @padding1: reserved for future, zeroes
 * @xattr_names: sum of lengths of all extended attribute names belonging to
 *               this inode
 * @compr_type: compression type used for this inode
 * @padding2: reserved for future, zeroes
 * @data: data attached to the inode
 *
 * Note, even though inode compression type is defined by @compr_type, some
 * nodes of this inode may be compressed with different compressor - this
 * happens if compression type is changed while the inode already has data
 * nodes. But @compr_type will be use for further writes to the inode.
 *
 * Note, do not forget to amend 'zero_ino_node_unused()' function when changing
 * the padding fields.
 */
struct ubifs_ino_node {
	struct ubifs_ch ch;
	__u8 key[UBIFS_MAX_KEY_LEN];
	__le64 creat_sqnum;
	__le64 size;
	__le64 atime_sec;
	__le64 ctime_sec;
	__le64 mtime_sec;
	__le32 atime_nsec;
	__le32 ctime_nsec;
	__le32 mtime_nsec;
	__le32 nlink;
	__le32 uid;
	__le32 gid;
	__le32 mode;
	__le32 flags;
	__le32 data_len;
	__le32 xattr_cnt;
	__le32 xattr_size;
	__u8 padding1[4]; /* Watch 'zero_ino_node_unused()' if changing! */
	__le32 xattr_names;
	__le16 compr_type;
	__u8 padding2[26]; /* Watch 'zero_ino_node_unused()' if changing! */
	__u8 data[];
} __attribute__ ((packed));

/**
 * struct ubifs_dent_node - directory entry node.
 * @ch: common header
 * @key: node key
 * @inum: target inode number
 * @padding1: reserved for future, zeroes
 * @type: type of the target inode (%UBIFS_ITYPE_REG, %UBIFS_ITYPE_DIR, etc)
 * @nlen: name length
 * @cookie: A 32bits random number, used to construct a 64bits
 *          identifier.
 * @name: zero-terminated name
 *
 * Note, do not forget to amend 'zero_dent_node_unused()' function when
 * changing the padding fields.
 */
struct ubifs_dent_node {
	struct ubifs_ch ch;
	__u8 key[UBIFS_MAX_KEY_LEN];
	__le64 inum;
	__u8 padding1;
	__u8 type;
	__le16 nlen;
	__le32 cookie;
	__u8 name[];
} __attribute__ ((packed));

/**
 * struct ubifs_data_node - data node.
 * @ch: common header
 * @key: node key
 * @size: uncompressed data size in bytes
 * @compr_type: compression type (%UBIFS_COMPR_NONE, %UBIFS_COMPR_LZO, etc)
 * @compr_size: compressed data size in bytes, only valid when data is encrypted
 * @data: data
 *
 */
struct ubifs_data_node {
	struct ubifs_ch ch;
	__u8 key[UBIFS_MAX_KEY_LEN];
	__le32 size;
	__le16 compr_type;
	__le16 compr_size;
	__u8 data[];
} __attribute__ ((packed));

/**
 * struct ubifs_trun_node - truncation node.
 * @ch: common header
 * @inum: truncated inode number
 * @padding: reserved for future, zeroes
 * @old_size: size before truncation
 * @new_size: size after truncation
 *
 * This node exists only in the journal and never goes to the main area. Note,
 * do not forget to amend 'zero_trun_node_unused()' function when changing the
 * padding fields.
 */
struct ubifs_trun_node {
	struct ubifs_ch ch;
	__le32 inum;
	__u8 padding[12]; /* Watch 'zero_trun_node_unused()' if changing! */
	__le64 old_size;
	__le64 new_size;
} __attribute__ ((packed));

/**
 * struct ubifs_pad_node - padding node.
 * @ch: common header
 * @pad_len: how many bytes after this node are unused (because padded)
 * @padding: reserved for future, zeroes
 */
struct ubifs_pad_node {
	struct ubifs_ch ch;
	__le32 pad_len;
} __attribute__ ((packed));

/**
 * struct ubifs_sb_node - superblock node.
 * @ch: common header
 * @padding: reserved for future, zeroes
 * @key_hash: type of hash function used in keys
 * @key_fmt: format of the key
 * @flags: file-system flags (%UBIFS_FLG_BIGLPT, etc)
 * @min_io_size: minimal input/output unit size
 * @leb_size: logical eraseblock size in bytes
 * @leb_cnt: count of LEBs used by file-system
 * @max_leb_cnt: maximum count of LEBs used by file-system
 * @max_bud_bytes: maximum amount of data stored in buds
 * @log_lebs: log size in logical eraseblocks
 * @lpt_lebs: number of LEBs used for lprops table
 * @orph_lebs: number of LEBs used for recording orphans
 * @jhead_cnt: count of journal heads
 * @fanout: tree fanout (max. number of links per indexing node)
 * @lsave_cnt: number of LEB numbers in LPT's save table
 * @fmt_version: UBIFS on-flash format version
 * @default_compr: default compression algorithm (%UBIFS_COMPR_LZO, etc)
 * @padding1: reserved for future, zeroes
 * @rp_uid: reserve pool UID
 * @rp_gid: reserve pool GID
 * @rp_size: size of the reserved pool in bytes
 * @padding2: reserved for future, zeroes
 * @time_gran: time granularity in nanoseconds
 * @uuid: UUID generated when the file system image was created
 * @ro_compat_version: UBIFS R/O compatibility version
 * @hmac: HMAC to authenticate the superblock node
 * @hmac_wkm: HMAC of a well known message (the string "UBIFS") as a convenience
 *            to the user to check if the correct key is passed.
 * @hash_algo: The hash algo used for this filesystem (one of enum hash_algo)
 * @hash_mst: hash of the master node, only valid for signed images in which the
 *            master node does not contain a hmac
 */
struct ubifs_sb_node {
	struct ubifs_ch ch;
	__u8 padding[2];
	__u8 key_hash;
	__u8 key_fmt;
	__le32 flags;
	__le32 min_io_size;
	__le32 leb_size;
	__le32 leb_cnt;
	__le32 max_leb_cnt;
	__le64 max_bud_bytes;
	__le32 log_lebs;
	__le32 lpt_lebs;
	__le32 orph_lebs;
	__le32 jhead_cnt;
	__le32 fanout;
	__le32 lsave_cnt;
	__le32 fmt_version;
	__le16 default_compr;
	__u8 padding1[2];
	__le32 rp_uid;
	__le32 rp_gid;
	__le64 rp_size;
	__le32 time_gran;
	__u8 uuid[16];
	__le32 ro_compat_version;
	__u8 hmac[UBIFS_MAX_HMAC_LEN];
	__u8 hmac_wkm[UBIFS_MAX_HMAC_LEN];
	__le16 hash_algo;
	__u8 hash_mst[UBIFS_MAX_HASH_LEN];
	__u8 padding2[3774];
} __attribute__ ((packed));

/**
 * struct ubifs_mst_node - master node.
 * @ch: common header
 * @highest_inum: highest inode number in the committed index
 * @cmt_no: commit number
 * @flags: various flags (%UBIFS_MST_DIRTY, etc)
 * @log_lnum: start of the log
 * @root_lnum: LEB number of the root indexing node
 * @root_offs: offset within @root_lnum
 * @root_len: root indexing node length
 * @gc_lnum: LEB reserved for garbage collection (%-1 value means the LEB was
 * not reserved and should be reserved on mount)
 * @ihead_lnum: LEB number of index head
 * @ihead_offs: offset of index head
 * @index_size: size of index on flash
 * @total_free: total free space in bytes
 * @total_dirty: total dirty space in bytes
 * @total_used: total used space in bytes (includes only data LEBs)
 * @total_dead: total dead space in bytes (includes only data LEBs)
 * @total_dark: total dark space in bytes (includes only data LEBs)
 * @lpt_lnum: LEB number of LPT root nnode
 * @lpt_offs: offset of LPT root nnode
 * @nhead_lnum: LEB number of LPT head
 * @nhead_offs: offset of LPT head
 * @ltab_lnum: LEB number of LPT's own lprops table
 * @ltab_offs: offset of LPT's own lprops table
 * @lsave_lnum: LEB number of LPT's save table (big model only)
 * @lsave_offs: offset of LPT's save table (big model only)
 * @lscan_lnum: LEB number of last LPT scan
 * @empty_lebs: number of empty logical eraseblocks
 * @idx_lebs: number of indexing logical eraseblocks
 * @leb_cnt: count of LEBs used by file-system
 * @hash_root_idx: the hash of the root index node
 * @hash_lpt: the hash of the LPT
 * @hmac: HMAC to authenticate the master node
 * @padding: reserved for future, zeroes
 */
struct ubifs_mst_node {
	struct ubifs_ch ch;
	__le64 highest_inum;
	__le64 cmt_no;
	__le32 flags;
	__le32 log_lnum;
	__le32 root_lnum;
	__le32 root_offs;
	__le32 root_len;
	__le32 gc_lnum;
	__le32 ihead_lnum;
	__le32 ihead_offs;
	__le64 index_size;
	__le64 total_free;
	__le64 total_dirty;
	__le64 total_used;
	__le64 total_dead;
	__le64 total_dark;
	__le32 lpt_lnum;
	__le32 lpt_offs;
	__le32 nhead_lnum;
	__le32 nhead_offs;
	__le32 ltab_lnum;
	__le32 ltab_offs;
	__le32 lsave_lnum;
	__le32 lsave_offs;
	__le32 lscan_lnum;
	__le32 empty_lebs;
	__le32 idx_lebs;
	__le32 leb_cnt;
	__u8 hash_root_idx[UBIFS_MAX_HASH_LEN];
	__u8 hash_lpt[UBIFS_MAX_HASH_LEN];
	__u8 hmac[UBIFS_MAX_HMAC_LEN];
	__u8 padding[152];
} __attribute__ ((packed));

/**
 * struct ubifs_ref_node - logical eraseblock reference node.
 * @ch: common header
 * @lnum: the referred logical eraseblock number
 * @offs: start offset in the referred LEB
 * @jhead: journal head number
 * @padding: reserved for future, zeroes
 */
struct ubifs_ref_node {
	struct ubifs_ch ch;
	__le32 lnum;
	__le32 offs;
	__le32 jhead;
	__u8 padding[28];
} __attribute__ ((packed));

/**
 * struct ubifs_auth_node - node for authenticating other nodes
 * @ch: common header
 * @hmac: The HMAC
 */
struct ubifs_auth_node {
	struct ubifs_ch ch;
	__u8 hmac[];
} __attribute__ ((packed));

/**
 * struct ubifs_sig_node - node for signing other nodes
 * @ch: common header
 * @type: type of the signature, currently only UBIFS_SIGNATURE_TYPE_PKCS7
 * supported
 * @len: The length of the signature data
 * @padding: reserved for future, zeroes
 * @sig: The signature data
 */
struct ubifs_sig_node {
	struct ubifs_ch ch;
	__le32 type;
	__le32 len;
	__u8 padding[32];
	__u8 sig[];
} __attribute__ ((packed));

/**
 * struct ubifs_branch - key/reference/length branch
 * @lnum: LEB number of the target node
 * @offs: offset within @lnum
 * @len: target node length
 * @key: key
 *
 * In an authenticated UBIFS we have the hash of the referenced node after @key.
 * This can't be added to the struct type definition because @key is a
 * dynamically sized element already.
 */
struct ubifs_branch {
	__le32 lnum;
	__le32 offs;
	__le32 len;
	__u8 key[];
} __attribute__ ((packed));

/**
 * struct ubifs_idx_node - indexing node.
 * @ch: common header
 * @child_cnt: number of child index nodes
 * @level: tree level
 * @branches: LEB number / offset / length / key branches
 */
struct ubifs_idx_node {
	struct ubifs_ch ch;
	__le16 child_cnt;
	__le16 level;
	__u8 branches[];
} __attribute__ ((packed));

/**
 * struct ubifs_cs_node - commit start node.
 * @ch: common header
 * @cmt_no: commit number
 */
struct ubifs_cs_node {
	struct ubifs_ch ch;
	__le64 cmt_no;
} __attribute__ ((packed));

/**
 * struct ubifs_orph_node - orphan node.
 * @ch: common header
 * @cmt_no: commit number (also top bit is set on the last node of the commit)
 * @inos: inode numbers of orphans
 */
struct ubifs_orph_node {
	struct ubifs_ch ch;
	__le64 cmt_no;
	__le64 inos[];
} __attribute__ ((packed));

#endif /* __UBIFS_MEDIA_H__ */
    runs-on: /*
 * Copyright (c), 2007, 2008
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __MTD_UTILS_COMMON_H__
#define __MTD_UTILS_COMMON_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <features.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/sysmacros.h>

#ifndef PROGRAM_NAME
# error "You must define PROGRAM_NAME before including this header"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIN	/* some C lib headers define this for us */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#define min(a, b) MIN(a, b) /* glue for linux kernel source */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define ALIGN(x,a) __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask) (((x)+(mask))&~(mask))

#define min_t(t,x,y) ({ \
	typeof((x)) _x = (x); \
	typeof((y)) _y = (y); \
	(_x < _y) ? _x : _y; \
})

#define max_t(t,x,y) ({ \
	typeof((x)) _x = (x); \
	typeof((y)) _y = (y); \
	(_x > _y) ? _x : _y; \
})

/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif

/* Verbose messages */
#define bareverbose(verbose, fmt, ...) do {                        \
	if (verbose)                                               \
		printf(fmt, ##__VA_ARGS__);                        \
} while(0)
#define verbose(verbose, fmt, ...) \
	bareverbose(verbose, "%s: " fmt "\n", PROGRAM_NAME, ##__VA_ARGS__)

/* Normal messages */
#define normsg_cont(fmt, ...) do {                                 \
	printf("%s: " fmt, PROGRAM_NAME, ##__VA_ARGS__);           \
} while(0)
#define normsg(fmt, ...) do {                                      \
	normsg_cont(fmt "\n", ##__VA_ARGS__);                      \
} while(0)

/* Error messages */
#define errmsg(fmt, ...)  ({                                                \
	fprintf(stderr, "%s: error!: " fmt "\n", PROGRAM_NAME, ##__VA_ARGS__); \
	-1;                                                                 \
})
#define errmsg_die(fmt, ...) do {                                           \
	exit(errmsg(fmt, ##__VA_ARGS__));                                   \
} while(0)

/* System error messages */
#define sys_errmsg(fmt, ...)  ({                                            \
	int _err = errno;                                                   \
	errmsg(fmt, ##__VA_ARGS__);                                         \
	fprintf(stderr, "%*serror %d (%s)\n", (int)sizeof(PROGRAM_NAME) + 1,\
		"", _err, strerror(_err));                                  \
	-1;                                                                 \
})
#define sys_errmsg_die(fmt, ...) do {                                       \
	exit(sys_errmsg(fmt, ##__VA_ARGS__));                               \
} while(0)

/* Warnings */
#define warnmsg(fmt, ...) do {                                                \
	fprintf(stderr, "%s: warning!: " fmt "\n", PROGRAM_NAME, ##__VA_ARGS__); \
} while(0)

/* for tagging functions that always exit */
#if defined(__GNUC__) || defined(__clang__)
	#define NORETURN __attribute__((noreturn))
#else
	#define NORETURN
#endif

/**
 * prompt the user for confirmation
 */
static inline bool prompt(const char *msg, bool def)
{
	bool ret = def;
	char line[64];

	do {
		normsg_cont("%s (%c/%c) ", msg, def ? 'Y' : 'y', def ? 'n' : 'N');
		fflush(stdout);

		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("failed to read prompt; assuming '%s'\n",
				def ? "yes" : "no");
			break;
		}

		if (strcmp("\n", line) != 0) {
			switch (line[0]) {
			case 'N':
			case 'n': ret = false; break;
			case 'Y':
			case 'y': ret = true; break;
			default:
				puts("unknown response; please try again");
				continue;
			}
		}
		break;
	} while (1);

	return ret;
}

static inline int is_power_of_2(unsigned long long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

/* Check whether buffer is filled with character 'pattern' */
static inline int buffer_check_pattern(unsigned char *buffer, size_t size,
				       unsigned char pattern)
{
	/* Invalid input */
	if (!buffer || (size == 0))
		return 0;

	/* No match on first byte */
	if (*buffer != pattern)
		return 0;

	/* First byte matched and buffer is 1 byte long, OK. */
	if (size == 1)
		return 1;

	/*
	 * Check buffer longer than 1 byte. We already know that buffer[0]
	 * matches the pattern, so the test below only checks whether the
	 * buffer[0...size-2] == buffer[1...size-1] , which is a test for
	 * whether the buffer is filled with constant value.
	 */
	return !memcmp(buffer, buffer + 1, size - 1);
}

/**
 * simple_strtoX - convert a hex/dec/oct string into a number
 * @snum: buffer to convert
 * @error: set to 1 when buffer isn't fully consumed
 *
 * These functions are similar to the standard strtoX() functions, but they are
 * a little bit easier to use if you want to convert full string of digits into
 * the binary form. The typical usage:
 *
 * int error = 0;
 * unsigned long num;
 *
 * num = simple_strtoul(str, &error);
 * if (error || ... if needed, your check that num is not out of range ...)
 * 	error_happened();
 */
#define simple_strtoX(func, type) \
static inline type simple_##func(const char *snum, int *error) \
{ \
	char *endptr; \
	type ret = func(snum, &endptr, 0); \
 \
	if (error && (!*snum || *endptr)) { \
		errmsg("%s: unable to parse the number '%s'", #func, snum); \
		*error = 1; \
	} \
 \
	return ret; \
}
simple_strtoX(strtol, long int)
simple_strtoX(strtoll, long long int)
simple_strtoX(strtoul, unsigned long int)
simple_strtoX(strtoull, unsigned long long int)

/* Simple version-printing for utils */
#define common_print_version() \
do { \
	printf("%s (mtd-utils) %s\n", PROGRAM_NAME, VERSION); \
} while (0)

#include "xalloc.h"

long long util_get_bytes(const char *str);
void util_print_bytes(long long bytes, int bracket);
int util_srand(void);
char *mtd_find_dev_node(const char *id);

/*
 * The following helpers are here to avoid compiler complaints about unchecked
 * return code.
 * FIXME: The proper fix would be to check the return code in all those places,
 * but it's usually placed in old code which have no proper exit path and
 * handling  errors requires rewriting a lot of code.
 *
 * WARNING: Please do not use these helpers in new code. Instead, make sure
 * you check the function return code and provide coherent error handling in
 * case of error.
 */
static inline ssize_t read_nocheck(int fd, void *buf, size_t count)
{
	return read(fd, buf, count);
}

static inline ssize_t write_nocheck(int fd, void *buf, size_t count)
{
	return write(fd, buf, count);
}

static inline ssize_t pread_nocheck(int fd, void *buf, size_t count,
				    off_t offset)
{
	return pread(fd, buf, count, offset);
}

static inline ssize_t pwrite_nocheck(int fd, void *buf, size_t count,
				     off_t offset)
{
	return pwrite(fd, buf, count, offset);
}
#ifdef __cplusplus
}
#endif

#endif /* !__MTD_UTILS_COMMON_H__ */
    environment: /*
 * This code was taken from the linux kernel. The license is GPL Version 2.
 */

#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>

/* Return a 32-bit CRC of the contents of the buffer */
extern uint32_t mtd_crc32(uint32_t val, const void *ss, int len);

#endif /* __CRC32_H__ */
    steps: 
/*-------------------------------------------------------------------------*/
/**
   @file    dictionary.h
   
   @date    Sep 2007
   @version $Revision: 1.12 $
   @brief   Implements a dictionary for string variables.

   This module implements a simple dictionary object, i.e. a list
   of string/string associations. This object is useful to store e.g.
   informations retrieved from a configuration file (ini files).
*/
/*--------------------------------------------------------------------------*/

/*
	$Id: dictionary.h,v 1.12 2007-11-23 21:37:00 ndevilla Exp $
	$Author: ndevilla $
	$Date: 2007-11-23 21:37:00 $
	$Revision: 1.12 $
*/

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*---------------------------------------------------------------------------
   								New types
 ---------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/**
  @brief	Dictionary object

  This object contains a list of string/string associations. Each
  association is identified by a unique string key. Looking up values
  in the dictionary is speeded up by the use of a (hopefully collision-free)
  hash function.
 */
/*-------------------------------------------------------------------------*/
typedef struct _dictionary_ {
	int				n ;		/** Number of entries in dictionary */
	int				size ;	/** Storage size */
	char 		**	val ;	/** List of string values */
	char 		**  key ;	/** List of string keys */
	unsigned	 *	hash ;	/** List of hash values for keys */
} dictionary ;


/*---------------------------------------------------------------------------
  							Function prototypes
 ---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/**
  @brief    Compute the hash key for a string.
  @param    key     Character string to use for key.
  @return   1 unsigned int on at least 32 bits.

  This hash function has been taken from an Article in Dr Dobbs Journal.
  This is normally a collision-free function, distributing keys evenly.
  The key is stored anyway in the struct so that collision can be avoided
  by comparing the key itself in last resort.
 */
/*--------------------------------------------------------------------------*/
unsigned dictionary_hash(char * key);

/*-------------------------------------------------------------------------*/
/**
  @brief    Create a new dictionary object.
  @param    size    Optional initial size of the dictionary.
  @return   1 newly allocated dictionary objet.

  This function allocates a new dictionary object of given size and returns
  it. If you do not know in advance (roughly) the number of entries in the
  dictionary, give size=0.
 */
/*--------------------------------------------------------------------------*/
dictionary * dictionary_new(int size);

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete a dictionary object
  @param    d   dictionary object to deallocate.
  @return   void

  Deallocate a dictionary object and all memory associated to it.
 */
/*--------------------------------------------------------------------------*/
void dictionary_del(dictionary * vd);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get a value from a dictionary.
  @param    d       dictionary object to search.
  @param    key     Key to look for in the dictionary.
  @param    def     Default value to return if key not found.
  @return   1 pointer to internally allocated character string.

  This function locates a key in a dictionary and returns a pointer to its
  value, or the passed 'def' pointer if no such key can be found in
  dictionary. The returned character pointer points to data internal to the
  dictionary object, you should not try to free it or modify it.
 */
/*--------------------------------------------------------------------------*/
char * dictionary_get(dictionary * d, char * key, char * def);


/*-------------------------------------------------------------------------*/
/**
  @brief    Set a value in a dictionary.
  @param    d       dictionary object to modify.
  @param    key     Key to modify or add.
  @param    val     Value to add.
  @return   int     0 if Ok, anything else otherwise

  If the given key is found in the dictionary, the associated value is
  replaced by the provided one. If the key cannot be found in the
  dictionary, it is added to it.

  It is Ok to provide a NULL value for val, but NULL values for the dictionary
  or the key are considered as errors: the function will return immediately
  in such a case.

  Notice that if you dictionary_set a variable to NULL, a call to
  dictionary_get will return a NULL value: the variable will be found, and
  its value (NULL) is returned. In other words, setting the variable
  content to NULL is equivalent to deleting the variable from the
  dictionary. It is not possible (in this implementation) to have a key in
  the dictionary without value.

  This function returns non-zero in case of failure.
 */
/*--------------------------------------------------------------------------*/
int dictionary_set(dictionary * vd, char * key, char * val);

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete a key in a dictionary
  @param    d       dictionary object to modify.
  @param    key     Key to remove.
  @return   void

  This function deletes a key in a dictionary. Nothing is done if the
  key cannot be found.
 */
/*--------------------------------------------------------------------------*/
void dictionary_unset(dictionary * d, char * key);


/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump
  @param    f   Opened file pointer.
  @return   void

  Dumps a dictionary onto an opened file pointer. Key pairs are printed out
  as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
  output file pointers.
 */
/*--------------------------------------------------------------------------*/
void dictionary_dump(dictionary * d, FILE * out);

#endif

    - name: #ifndef LIBFEC_H
#define LIBFEC_H

struct fec_parms;

/* k - number of actual data packets
 * n - total number of packets including data and redundant packets
 *   (actual packet size isn't relevant here) */
struct fec_parms *fec_new(int k, int n);
void fec_free(struct fec_parms *p);

/* src   - array of (n) pointers to data packets
 * fec   - buffer for packet to be generated
 * index - index of packet to be generated (0 <= index < n)
 * sz    - data packet size
 *
 * _linear version just takes a pointer to the raw data; no
 * mucking about with packet pointers.
 */
void fec_encode(struct fec_parms *code, unsigned char *src[],
		unsigned char *fec, int index, int sz);
void fec_encode_linear(struct fec_parms *code, unsigned char *src,
		       unsigned char *fec, int index, int sz);

/* data  - array of (k) pointers to data packets, in arbitrary order (see i)
 * i     - indices of (data) packets
 * sz    - data packet size
 *
 * Will never fail as long as you give it (k) individual data packets.
 * Will re-order the (data) pointers but not the indices -- data packets
 * are ordered on return.
 */
int fec_decode(struct fec_parms *code, unsigned char *data[],
	       int i[], int sz);

#endif /* LIBFEC_H */

      uses: 
/*-------------------------------------------------------------------------*/
/**
   @file    iniparser.h
   @
   @date    Sep 2007
   @version 3.0
   @brief   Parser for ini files.
*/
/*--------------------------------------------------------------------------*/

/*
	$Id: iniparser.h,v 1.24 2007-11-23 21:38:19 ndevilla Exp $
	$Revision: 1.24 $
*/

#ifndef _INIPARSER_H_
#define _INIPARSER_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * The following #include is necessary on many Unixes but not Linux.
 * It is not needed for Windows platforms.
 * Uncomment it if needed.
 */
/* #include <unistd.h> */

#include "dictionary.h"

/*---------------------------------------------------------------------------
   								Macros
 ---------------------------------------------------------------------------*/
/** For backwards compatibility only */
#define iniparser_getstr(d, k)  iniparser_getstring(d, k, NULL)
#define iniparser_setstr        iniparser_setstring

/*-------------------------------------------------------------------------*/
/**
  @brief    Get number of sections in a dictionary
  @param    d   Dictionary to examine
  @return   int Number of sections found in dictionary

  This function returns the number of sections found in a dictionary.
  The test to recognize sections is done on the string stored in the
  dictionary: a section name is given as "section" whereas a key is
  stored as "section:key", thus the test looks for entries that do not
  contain a colon.

  This clearly fails in the case a section name contains a colon, but
  this should simply be avoided.

  This function returns -1 in case of error.
 */
/*--------------------------------------------------------------------------*/

int iniparser_getnsec(dictionary * d);


/*-------------------------------------------------------------------------*/
/**
  @brief    Get name for section n in a dictionary.
  @param    d   Dictionary to examine
  @param    n   Section number (from 0 to nsec-1).
  @return   Pointer to char string

  This function locates the n-th section in a dictionary and returns
  its name as a pointer to a string statically allocated inside the
  dictionary. Do not free or modify the returned string!

  This function returns NULL in case of error.
 */
/*--------------------------------------------------------------------------*/

char * iniparser_getsecname(dictionary * d, int n);


/*-------------------------------------------------------------------------*/
/**
  @brief    Save a dictionary to a loadable ini file
  @param    d   Dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given dictionary into a loadable ini file.
  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/

void iniparser_dump_ini(dictionary * d, FILE * f);

/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump.
  @param    f   Opened file pointer to dump to.
  @return   void

  This function prints out the contents of a dictionary, one element by
  line, onto the provided file pointer. It is OK to specify @c stderr
  or @c stdout as output files. This function is meant for debugging
  purposes mostly.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dump(dictionary * d, FILE * f);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key
  @param    d       Dictionary to search
  @param    key     Key string to look for
  @param    def     Default value to return if key not found.
  @return   pointer to statically allocated character string

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
char * iniparser_getstring(dictionary * d, const char * key, char * def);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to an int
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  Supported values for integers include the usual C notation
  so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
  are supported. Examples:

  - "42"      ->  42
  - "042"     ->  34 (octal -> decimal)
  - "0x42"    ->  66 (hexa  -> decimal)

  Warning: the conversion may overflow in various ways. Conversion is
  totally outsourced to strtol(), see the associated man page for overflow
  handling.

  Credits: Thanks to A. Becker for suggesting strtol()
 */
/*--------------------------------------------------------------------------*/
int iniparser_getint(dictionary * d, const char * key, int notfound);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a boolean
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  A true boolean is found if one of the following is matched:

  - A string starting with 'y'
  - A string starting with 'Y'
  - A string starting with 't'
  - A string starting with 'T'
  - A string starting with '1'

  A false boolean is found if one of the following is matched:

  - A string starting with 'n'
  - A string starting with 'N'
  - A string starting with 'f'
  - A string starting with 'F'
  - A string starting with '0'

  The notfound value returned if no boolean is identified, does not
  necessarily have to be 0 or 1.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getboolean(dictionary * d, const char * key, int notfound);


/*-------------------------------------------------------------------------*/
/**
  @brief    Set an entry in a dictionary.
  @param    ini     Dictionary to modify.
  @param    entry   Entry to modify (entry name)
  @param    val     New value to associate to the entry.
  @return   int 0 if Ok, -1 otherwise.

  If the given entry can be found in the dictionary, it is modified to
  contain the provided value. If it cannot be found, -1 is returned.
  It is Ok to set val to NULL.
 */
/*--------------------------------------------------------------------------*/
int iniparser_setstring(dictionary * ini, char * entry, char * val);


/*-------------------------------------------------------------------------*/
/**
  @brief    Delete an entry in a dictionary
  @param    ini     Dictionary to modify
  @param    entry   Entry to delete (entry name)
  @return   void

  If the given entry can be found, it is deleted from the dictionary.
 */
/*--------------------------------------------------------------------------*/
void iniparser_unset(dictionary * ini, char * entry);

/*-------------------------------------------------------------------------*/
/**
  @brief    Finds out if a given entry exists in a dictionary
  @param    ini     Dictionary to search
  @param    entry   Name of the entry to look for
  @return   integer 1 if entry exists, 0 otherwise

  Finds out if a given entry exists in the dictionary. Since sections
  are stored as keys with NULL associated values, this is the only way
  of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
int iniparser_find_entry(dictionary * ini, char * entry) ;

/*-------------------------------------------------------------------------*/
/**
  @brief    Parse an ini file and return an allocated dictionary object
  @param    ininame Name of the ini file to read.
  @return   Pointer to newly allocated dictionary

  This is the parser for ini files. This function is called, providing
  the name of the file to be read. It returns a dictionary object that
  should not be accessed directly, but through accessor functions
  instead.

  The returned dictionary must be freed using iniparser_freedict().
 */
/*--------------------------------------------------------------------------*/
dictionary * iniparser_load(const char * ininame);

/*-------------------------------------------------------------------------*/
/**
  @brief    Free all memory associated to an ini dictionary
  @param    d Dictionary to free
  @return   void

  Free all memory associated to an ini dictionary.
  It is mandatory to call this function before the dictionary object
  gets out of the current context.
 */
/*--------------------------------------------------------------------------*/
void iniparser_freedict(dictionary * d);

#endif

    # Build
    - name: #ifndef LIBMISSING_H
#define LIBMISSING_H

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifndef HAVE_EXECINFO_H
int backtrace(void **buffer, int size);
char **backtrace_symbols(void *const *buffer, int size);
void backtrace_symbols_fd(void *const *buffer, int size, int fd);
#endif

#endif /* LIBMISSING_H */

      run: |
        docker build -t ${ /*
 * Copyright (C) 2008, 2009 Nokia Corporation
 * Copyright 2021 NXP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * 
 *
 * MTD library.
 */

#ifndef __LIBMTD_H__
#define __LIBMTD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum MTD device name length */
#define MTD_NAME_MAX 127
/* Maximum MTD device type string length */
#define MTD_TYPE_MAX 64

/* MTD library descriptor */
typedef void * libmtd_t;

/* Forward decls */
struct region_info_user;

/**
 * @mtd_dev_cnt: count of MTD devices in system
 * @lowest_mtd_num: lowest MTD device number in system
 * @highest_mtd_num: highest MTD device number in system
 * @sysfs_supported: non-zero if sysfs is supported by MTD
 */
struct mtd_info
{
	int mtd_dev_cnt;
	int lowest_mtd_num;
	int highest_mtd_num;
	unsigned int sysfs_supported:1;
};

/**
 * struct mtd_dev_info - information about an MTD device.
 * @mtd_num: MTD device number
 * @major: major number of corresponding character device
 * @minor: minor number of corresponding character device
 * @type: flash type (constants like %MTD_NANDFLASH defined in mtd-abi.h)
 * @type_str: static R/O flash type string
 * @name: device name
 * @size: device size in bytes
 * @eb_cnt: count of eraseblocks
 * @eb_size: eraseblock size
 * @min_io_size: minimum input/output unit size
 * @subpage_size: sub-page size
 * @oob_size: OOB size (zero if the device does not have OOB area)
 * @oobavail: free OOB size
 * @region_cnt: count of additional erase regions
 * @writable: zero if the device is read-only
 * @bb_allowed: non-zero if the MTD device may have bad eraseblocks
 */
struct mtd_dev_info
{
	int mtd_num;
	int major;
	int minor;
	int type;
	const char type_str[MTD_TYPE_MAX + 1];
	const char name[MTD_NAME_MAX + 1];
	long long size;
	int eb_cnt;
	int eb_size;
	int min_io_size;
	int subpage_size;
	int oob_size;
	int oobavail;
	int region_cnt;
	unsigned int writable:1;
	unsigned int bb_allowed:1;
};

/**
 * libmtd_open - open MTD library.
 *
 * This function initializes and opens the MTD library and returns MTD library
 * descriptor in case of success and %NULL in case of failure. In case of
 * failure, errno contains zero if MTD is not present in the system, or
 * contains the error code if a real error happened.
 */
libmtd_t libmtd_open(void);

/**
 * libmtd_close - close MTD library.
 * @desc: MTD library descriptor
 */
void libmtd_close(libmtd_t desc);

/**
 * mtd_dev_present - check whether a MTD device is present.
 * @desc: MTD library descriptor
 * @mtd_num: MTD device number to check
 *
 * This function returns %1 if MTD device is present and %0 if not.
 */
int mtd_dev_present(libmtd_t desc, int mtd_num);

/**
 * mtd_get_info - get general MTD information.
 * @desc: MTD library descriptor
 * @info: the MTD device information is returned here
 *
 * This function fills the passed @info object with general MTD information and
 * returns %0 in case of success and %-1 in case of failure.
 */
int mtd_get_info(libmtd_t desc, struct mtd_info *info);

/**
 * mtd_get_dev_info - get information about an MTD device.
 * @desc: MTD library descriptor
 * @node: path of the MTD device node
 * @mtd: the MTD device information is returned here
 *
 * This function gets information about MTD device defined by the @node device
 * node file and saves this information in the @mtd object. Returns %0 in case
 * of success and %-1 in case of failure. If MTD subsystem is not present in the
 * system, or the MTD device does not exist, errno is set to @ENODEV.
 */
int mtd_get_dev_info(libmtd_t desc, const char *node, struct mtd_dev_info *mtd);

/**
 * mtd_get_dev_info1 - get information about an MTD device.
 * @desc: MTD library descriptor
 * @mtd_num: MTD device number to fetch information about
 * @mtd: the MTD device information is returned here
 *
 * This function is identical to 'mtd_get_dev_info()' except that it accepts
 * MTD device number, not MTD character device.
 */
int mtd_get_dev_info1(libmtd_t desc, int mtd_num, struct mtd_dev_info *mtd);

/**
 * mtd_get_dev_info2 - get information about an MTD device.
 * @desc: MTD library descriptor
 * @name: name of the MTD device
 * @mtd: the MTD device information is returned here
 *
 * This function is identical to 'mtd_get_dev_info()' except that it accepts
 * MTD device's name, not MTD character device.
 */
int mtd_get_dev_info2(libmtd_t desc, const char *name, struct mtd_dev_info *mtd);

/**
 * mtd_lock - lock eraseblocks.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to lock
 *
 * This function locks eraseblock @eb. Returns %0 in case of success and %-1
 * in case of failure.
 */
int mtd_lock(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_unlock - unlock eraseblocks.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to lock
 *
 * This function unlocks eraseblock @eb. Returns %0 in case of success and %-1
 * in case of failure.
 */
int mtd_unlock(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_unlock_multi - unlock eraseblocks.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: index of first eraseblock to unlock
 * @blocks: the number of eraseblocks to unlock
 *
 * This function unlocks @blocks starting at eraseblock @eb.
 * Returns %0 in case of success and %-1 in case of failure.
 */
int mtd_unlock_multi(const struct mtd_dev_info *mtd, int fd, int eb,
		     int blocks);

/**
 * mtd_erase - erase multiple eraseblocks.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: index of first eraseblock to erase
 * @blocks: the number of eraseblocks to erase
 *
 * This function erases @blocks starting at eraseblock @eb of MTD device
 * described by @fd. Returns %0 in case of success and %-1 in case of failure.
 */
int mtd_erase_multi(libmtd_t desc, const struct mtd_dev_info *mtd,
			int fd, int eb, int blocks);

/**
 * mtd_erase - erase an eraseblock.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to erase
 *
 * This function erases eraseblock @eb of MTD device described by @fd. Returns
 * %0 in case of success and %-1 in case of failure.
 */
int mtd_erase(libmtd_t desc, const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_regioninfo - get information about an erase region.
 * @fd: MTD device node file descriptor
 * @regidx: index of region to look up
 * @reginfo: the region information is returned here
 *
 * This function gets information about an erase region defined by the
 * @regidx index and saves this information in the @reginfo object.
 * Returns %0 in case of success and %-1 in case of failure. If the
 * @regidx is not valid or unavailable, errno is set to @ENODEV.
 */
int mtd_regioninfo(int fd, int regidx, struct region_info_user *reginfo);

/**
 * mtd_is_locked - see if the specified eraseblock is locked.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to check
 *
 * This function checks to see if eraseblock @eb of MTD device described
 * by @fd is locked. Returns %0 if it is unlocked, %1 if it is locked, and
 * %-1 in case of failure. If the ioctl is not supported (support was added in
 * Linux kernel 2.6.36) or this particular device does not support it, errno is
 * set to @ENOTSUPP.
 */
int mtd_is_locked(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_torture - torture an eraseblock.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to torture
 *
 * This function tortures eraseblock @eb. Returns %0 in case of success and %-1
 * in case of failure.
 */
int mtd_torture(libmtd_t desc, const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_is_bad - check if eraseblock is bad.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to check
 *
 * This function checks if eraseblock @eb is bad. Returns %0 if not, %1 if yes,
 * and %-1 in case of failure.
 */
int mtd_is_bad(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_mark_bad - mark an eraseblock as bad.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to mark as bad
 *
 * This function marks eraseblock @eb as bad. Returns %0 in case of success and
 * %-1 in case of failure.
 */
int mtd_mark_bad(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_read - read data from an MTD device.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to read from
 * @offs: offset withing the eraseblock to read from
 * @buf: buffer to read data to
 * @len: how many bytes to read
 *
 * This function reads @len bytes of data from eraseblock @eb and offset @offs
 * of the MTD device defined by @mtd and stores the read data at buffer @buf.
 * Returns %0 in case of success and %-1 in case of failure.
 */
int mtd_read(const struct mtd_dev_info *mtd, int fd, int eb, int offs,
	     void *buf, int len);

/**
 * mtd_write - write data to an MTD device.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to write to
 * @offs: offset withing the eraseblock to write to
 * @data: data buffer to write
 * @len: how many data bytes to write
 * @oob: OOB buffer to write
 * @ooblen: how many OOB bytes to write
 * @mode: write mode (e.g., %MTD_OOB_PLACE, %MTD_OOB_RAW)
 *
 * This function writes @len bytes of data to eraseblock @eb and offset @offs
 * of the MTD device defined by @mtd. Returns %0 in case of success and %-1 in
 * case of failure.
 *
 * Can only write to a single page at a time if writing to OOB.
 */
int mtd_write(libmtd_t desc, const struct mtd_dev_info *mtd, int fd, int eb,
	      int offs, void *data, int len, void *oob, int ooblen,
	      uint8_t mode);

/**
 * mtd_read_oob - read out-of-band area.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @start: page-aligned start address
 * @length: number of OOB bytes to read
 * @data: read buffer
 *
 * This function reads @length OOB bytes starting from address @start on
 * MTD device described by @fd. The address is specified as page byte offset
 * from the beginning of the MTD device. This function returns %0 in case of
 * success and %-1 in case of failure.
 */
int mtd_read_oob(libmtd_t desc, const struct mtd_dev_info *mtd, int fd,
		 uint64_t start, uint64_t length, void *data);

/**
 * mtd_write_oob - write out-of-band area.
 * @desc: MTD library descriptor
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @start: page-aligned start address
 * @length: number of OOB bytes to write
 * @data: write buffer
 *
 * This function writes @length OOB bytes starting from address @start on
 * MTD device described by @fd. The address is specified as page byte offset
 * from the beginning of the MTD device. Returns %0 in case of success and %-1
 * in case of failure.
 */
int mtd_write_oob(libmtd_t desc, const struct mtd_dev_info *mtd, int fd,
		  uint64_t start, uint64_t length, void *data);

/**
 * mtd_probe_node - test MTD node.
 * @desc: MTD library descriptor
 * @node: the node to test
 *
 * This function tests whether @node is an MTD device node and returns %1 if it
 * is, and %-1 if it is not (errno is %ENODEV in this case) or if an error
 * occurred.
 */
int mtd_probe_node(libmtd_t desc, const char *node);

#ifdef __cplusplus
}
#endif

#endif /* __LIBMTD_H__ */ }:${ /*
 * Copyright (C) 2008 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * 
 *
 * UBI scanning library.
 */

#ifndef __LIBSCAN_H__
#define __LIBSCAN_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * If an eraseblock does not contain an erase counter, this value is used
 * instead of the erase counter.
 */
#define NO_EC 0xFFFFFFFF

/*
 * If an eraseblock contains a corrupted erase counter, this value is used
 * instead of the erase counter.
 */
#define CORRUPT_EC 0xFFFFFFFE

/*
 * If an eraseblock does not contain an erase counter, one of these values is
 * used.
 *
 * @EB_EMPTY: the eraseblock appeared to be empty
 * @EB_CORRUPTED: the eraseblock contains corrupted erase counter header
 * @EB_ALIEN: the eraseblock contains some non-UBI data
 * @EC_MAX: maximum allowed erase counter value
 */
enum
{
	EB_EMPTY     = 0xFFFFFFFF,
	EB_CORRUPTED = 0xFFFFFFFE,
	EB_ALIEN     = 0xFFFFFFFD,
	EB_BAD       = 0xFFFFFFFC,
	EC_MAX       = UBI_MAX_ERASECOUNTER,
};

/**
 * struct ubi_scan_info - UBI scanning information.
 * @ec: erase counters or eraseblock status for all eraseblocks
 * @mean_ec: mean erase counter
 * @ok_cnt: count of eraseblock with correct erase counter header
 * @empty_cnt: count of supposedly eraseblocks
 * @corrupted_cnt: count of eraseblocks with corrupted erase counter header
 * @alien_cnt: count of eraseblock containing non-ubi data
 * @bad_cnt: count of bad eraseblocks
 * @good_cnt: count of non-bad eraseblocks
 * @vid_hdr_offs: volume ID header offset from the found EC headers (%-1 means
 *                undefined)
 * @data_offs: data offset from the found EC headers (%-1 means undefined)
 */
struct ubi_scan_info
{
	uint32_t *ec;
	long long mean_ec;
	int ok_cnt;
	int empty_cnt;
	int corrupted_cnt;
	int alien_cnt;
	int bad_cnt;
	int good_cnt;
	int vid_hdr_offs;
	int data_offs;
};

struct mtd_dev_info;

/**
 * ubi_scan - scan an MTD device.
 * @mtd: information about the MTD device to scan
 * @fd: MTD device node file descriptor
 * @info: the result of the scanning is returned here
 * @verbose: verbose mode: %0 - be silent, %1 - output progress information,
 *           2 - debugging output mode
 */
int ubi_scan(struct mtd_dev_info *mtd, int fd, struct ubi_scan_info **info,
	     int verbose);

/**
 * ubi_scan_free - free scanning information.
 * @si: scanning information to free
 */
void ubi_scan_free(struct ubi_scan_info *si);

#ifdef __cplusplus
}
#endif

#endif /* __LIBSCAN_H__ */ } .

    - name: /*
 * Copyright (c) International Business Machines Corp., 2006
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * 
 *
 * UBI (Unsorted Block Images) library.
 */

#ifndef __LIBUBI_H__
#define __LIBUBI_H__

#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <mtd/ubi-user.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UBI version libubi is made for */
#define LIBUBI_UBI_VERSION 1

/* Maximum physical eraseblock size in bytes */
#define UBI_MAX_PEB_SZ (2*1024*1024)

/* The maximum volume name length (from Linux's ubi-media.h) */
#define UBI_VOL_NAME_MAX 127

/* UBI library descriptor */
typedef void * libubi_t;

/**
 * struct ubi_attach_request - MTD device attachment request.
 * @dev_num: number to assign to the newly created UBI device
 *           (%UBI_DEV_NUM_AUTO should be used to automatically assign the
 *           number)
 * @mtd_num: MTD device number to attach (used if @mtd_dev_node is %NULL)
 * @mtd_dev_node: path to MTD device node to attach
 * @vid_hdr_offset: VID header offset (%0 means default offset and this is what
 *                  most of the users want)
 * @max_beb_per1024: Maximum expected bad eraseblocks per 1024 eraseblocks
 * @disable_fm: whether disable fastmap
 * @need_resv_pool: whether reserve free pebs for filling pool/wl_pool
 */
struct ubi_attach_request
{
	int dev_num;
	int mtd_num;
	const char *mtd_dev_node;
	int vid_hdr_offset;
	int max_beb_per1024;
	bool disable_fm;
	bool need_resv_pool;
};

/**
 * struct ubi_mkvol_request - volume creation request.
 * @vol_id: ID to assign to the new volume (%UBI_VOL_NUM_AUTO should be used to
 *          automatically assign ID)
 * @alignment: volume alignment
 * @bytes: volume size in bytes
 * @vol_type: volume type (%UBI_DYNAMIC_VOLUME or %UBI_STATIC_VOLUME)
 * @name: volume name
 * @flags: volume flags
 */
struct ubi_mkvol_request
{
	int vol_id;
	int alignment;
	long long bytes;
	int vol_type;
	const char *name;
	uint8_t flags;
};

/**
 * struct ubi_info - general UBI information.
 * @dev_count: count of UBI devices in system
 * @lowest_dev_num: lowest UBI device number
 * @highest_dev_num: highest UBI device number
 * @version: UBI version
 * @ctrl_major: major number of the UBI control device
 * @ctrl_minor: minor number of the UBI control device
 */
struct ubi_info
{
	int dev_count;
	int lowest_dev_num;
	int highest_dev_num;
	int version;
	int ctrl_major;
	int ctrl_minor;
};

/**
 * struct ubi_dev_info - UBI device information.
 * @dev_num: UBI device number
 * @mtd_num: MTD device number on top of which this UBI device is working
 * @vol_count: count of volumes on this UBI device
 * @lowest_vol_id: lowest volume ID
 * @highest_vol_id: highest volume ID
 * @major: major number of corresponding character device
 * @minor: minor number of corresponding character device
 * @total_lebs: total number of logical eraseblocks on this UBI device
 * @avail_lebs: how many logical eraseblocks are not used and available for new
 *             volumes
 * @total_bytes: @total_lebs * @leb_size
 * @avail_bytes: @avail_lebs * @leb_size
 * @bad_count: count of bad physical eraseblocks
 * @leb_size: logical eraseblock size
 * @max_ec: current highest erase counter value
 * @bad_rsvd: how many physical eraseblocks of the underlying flash device are
 *            reserved for bad eraseblocks handling
 * @max_vol_count: maximum possible number of volumes on this UBI device
 * @min_io_size: minimum input/output unit size of the UBI device
 */
struct ubi_dev_info
{
	int dev_num;
	int mtd_num;
	int vol_count;
	int lowest_vol_id;
	int highest_vol_id;
	int major;
	int minor;
	int total_lebs;
	int avail_lebs;
	long long total_bytes;
	long long avail_bytes;
	int bad_count;
	int leb_size;
	long long max_ec;
	int bad_rsvd;
	int max_vol_count;
	int min_io_size;
};

/**
 * struct ubi_vol_info - UBI volume information.
 * @dev_num: UBI device number the volume resides on
 * @vol_id: ID of this volume
 * @major: major number of corresponding volume character device
 * @minor: minor number of corresponding volume character device
 * @type: volume type (%UBI_DYNAMIC_VOLUME or %UBI_STATIC_VOLUME)
 * @alignment: alignment of this volume
 * @data_bytes: how many data bytes are stored on this volume (equivalent to
 *              @rsvd_bytes for dynamic volumes)
 * @rsvd_bytes: how many bytes are reserved for this volume
 * @rsvd_lebs: how many logical eraseblocks are reserved for this volume
 * @leb_size: logical eraseblock size of this volume (may be less then
 *            device's logical eraseblock size due to alignment)
 * @corrupted: non-zero if the volume is corrupted
 * @name: volume name (null-terminated)
 */
struct ubi_vol_info
{
	int dev_num;
	int vol_id;
	int major;
	int minor;
	int type;
	int alignment;
	long long data_bytes;
	long long rsvd_bytes;
	int rsvd_lebs;
	int leb_size;
	int corrupted;
	char name[UBI_VOL_NAME_MAX + 1];
};

/**
 * libubi_open - open UBI library.
 *
 * This function initializes and opens the UBI library and returns UBI library
 * descriptor in case of success and %NULL in case of failure. In case of
 * failure, errno contains the error code or zero if UBI is not present in the
 * system.
 */
libubi_t libubi_open(void);

/**
 * libubi_close - close UBI library.
 * @desc: UBI library descriptor
 */
void libubi_close(libubi_t desc);

/**
 * ubi_get_info - get general UBI information.
 * @desc: UBI library descriptor
 * @info: pointer to the &struct ubi_info object to fill
 *
 * This function fills the passed @info object with general UBI information and
 * returns %0 in case of success and %-1 in case of failure.
 */
int ubi_get_info(libubi_t desc, struct ubi_info *info);

/**
 * mtd_num2ubi_dev - find UBI device by attached MTD device.
 * @@desc: UBI library descriptor
 * @mtd_num: MTD device number
 * @dev_num: UBI device number is returned here
 *
 * This function finds UBI device to which MTD device @mtd_num is attached.
 * Returns %0 if the UBI device was found and %-1 if not.
 */
int mtd_num2ubi_dev(libubi_t desc, int mtd_num, int *dev_num);

/**
 * ubi_attach - attach an MTD device by its node path or bt MTD device number
 * @desc: UBI library descriptor
 * @node: name of the UBI control character device node
 * @req: MTD attach request
 *
 * This function creates new UBI device by attaching an MTD device described by
 * @req. If @req->mtd_dev_node is given it should contain path to the MTD
 * device node. Otherwise @req->mtd_num will be used.
 *
 * Returns %0 in case of success, %-1 in case of failure (errno is set) and %1
 * if parameter @req->max_beb_per1024 was ignored by kernel (because the kernel
 * is old and does not support this feature, which was added in 3.7). The newly
 * created UBI device number is returned in @req->dev_num. In the MTD device
 * was specified by its device node path, the MTD device number is returned in
 * @req->mtd_num.
 */
int ubi_attach(libubi_t desc, const char *node, struct ubi_attach_request *req);

/**
 * ubi_detach_mtd - detach an MTD device.
 * @desc: UBI library descriptor
 * @node: name of the UBI control character device node
 * @mtd_num: MTD device number to detach
 *
 * This function detaches MTD device number @mtd_num from UBI, which means the
 * corresponding UBI device is removed. Returns zero in case of success and %-1
 * in case of failure.
 */
int ubi_detach_mtd(libubi_t desc, const char *node, int mtd_num);

/**
 * ubi_detach - detach an MTD device by its node path.
 * @desc: UBI library descriptor
 * @node: name of the UBI control character device node
 * @mtd_dev_node: path to an MTD device node
 *
 * This function detaches an MTD device @mtd_dev_node from UBI. Returns zero in
 * case of success and %-1 in case of failure.
 */
int ubi_detach(libubi_t desc, const char *node, const char *mtd_dev_node);

/**
 * ubi_remove_dev - remove an UBI device.
 * @desc: UBI library descriptor
 * @node: name of the UBI control character device node
 * @ubi_dev: UBI device number to remove
 *
 * This function removes UBI device number @ubi_dev and returns zero in case of
 * success and %-1 in case of failure.
 */
int ubi_remove_dev(libubi_t desc, const char *node, int ubi_dev);

/**
 * ubi_mkvol - create an UBI volume.
 * @desc: UBI library descriptor
 * @node: name of the UBI character device to create a volume at
 * @req: UBI volume creation request
 *
 * This function creates a UBI volume as described at @req and returns %0 in
 * case of success and %-1 in case of failure. The assigned volume ID is
 * returned in @req->vol_id.
 */
int ubi_mkvol(libubi_t desc, const char *node, struct ubi_mkvol_request *req);

/**
 * ubi_rmvol - remove a UBI volume.
 * @desc: UBI library descriptor
 * @node: name of the UBI character device to remove a volume from
 * @vol_id: ID of the volume to remove
 *
 * This function removes volume @vol_id from UBI device @node and returns %0 in
 * case of success and %-1 in case of failure.
 */
int ubi_rmvol(libubi_t desc, const char *node, int vol_id);


/**
 * ubi_rnvols - rename UBI volumes.
 * @desc: UBI library descriptor
 * @node: name of the UBI character device to remove a volume from
 * @rnvol: description of volumes to rename
 *
 * This function removes volume @vol_id from UBI device @node and returns %0 in
 * case of success and %-1 in case of failure.
 */
int ubi_rnvols(libubi_t desc, const char *node, struct ubi_rnvol_req *rnvol);

/**
 * ubi_rsvol - re-size UBI volume.
 * @desc: UBI library descriptor
 * @node: name of the UBI character device owning the volume which should be
 *        re-sized
 * @vol_id: volume ID to re-size
 * @bytes: new volume size in bytes
 *
 * This function returns %0 in case of success and %-1 in case of error.
 */
int ubi_rsvol(libubi_t desc, const char *node, int vol_id, long long bytes);

/**
 * ubi_probe_node - test UBI node.
 * @desc: UBI library descriptor
 * @node: the node to test
 *
 * This function tests whether @node is a UBI device or volume node and returns
 * %1 if this is an UBI device node, %2 if this is a volume node, and %-1 if
 * this is not an UBI device or volume node (errno is ENODEV in this case) or
 * if an error occurred.
 */
int ubi_probe_node(libubi_t desc, const char *node);

/**
 * ubi_get_dev_info - get UBI device information.
 * @desc: UBI library descriptor
 * @node: name of the UBI character device to fetch information about
 * @info: pointer to the &struct ubi_dev_info object to fill
 *
 * This function fills the passed @info object with UBI device information and
 * returns %0 in case of success and %-1 in case of failure. If the UBI device
 * corresponding to @node does not exist, errno is set to @ENODEV.
 */
int ubi_get_dev_info(libubi_t desc, const char *node,
		     struct ubi_dev_info *info);

/**
 * ubi_dev_present - check whether an UBI device is present.
 * @desc: UBI library descriptor
 * @dev_num: UBI device number to check
 *
 * This function returns %1 if UBI device is present and %0 if not.
 */
int ubi_dev_present(libubi_t desc, int dev_num);

/**
 * ubi_get_dev_info1 - get UBI device information.
 * @desc: UBI library descriptor
 * @dev_num: UBI device number to fetch information about
 * @info: pointer to the &struct ubi_dev_info object to fill
 *
 * This function is identical to 'ubi_get_dev_info()' except that it accepts UBI
 * device number, not UBI character device. If the UBI device @dev_num does not
 * exist, errno is set to @ENODEV.
 */
int ubi_get_dev_info1(libubi_t desc, int dev_num, struct ubi_dev_info *info);

/**
 * ubi_get_vol_info - get UBI volume information.
 * @desc: UBI library descriptor
 * @node: name of the UBI volume character device to fetch information about
 * @info: pointer to the &struct ubi_vol_info object to fill
 *
 * This function fills the passed @info object with UBI volume information and
 * returns %0 in case of success and %-1 in case of failure. If the UBI volume
 * corresponding to @node does not exist, errno is set to @ENODEV.
 */
int ubi_get_vol_info(libubi_t desc, const char *node,
		     struct ubi_vol_info *info);

/**
 * ubi_get_vol_info1 - get UBI volume information.
 * @desc: UBI library descriptor
 * @dev_num: UBI device number
 * @vol_id: ID of the UBI volume to fetch information about
 * @info: pointer to the &struct ubi_vol_info object to fill
 *
 * This function is identical to 'ubi_get_vol_info()' except that it accepts UBI
 * volume ID, not UBI volume character device. If the UBI device @dev_num does
 * not exist, or if the UBI volume @vol_id does not exist, errno is set to
 * @ENODEV.
 */
int ubi_get_vol_info1(libubi_t desc, int dev_num, int vol_id,
		      struct ubi_vol_info *info);

/**
 * ubi_get_vol_info1_nm - get UBI volume information by volume name.
 * @desc: UBI library descriptor
 * @dev_num: UBI device number
 * @name: name of the UBI volume to fetch information about
 * @info: pointer to the &struct ubi_vol_info object to fill
 *
 * This function is identical to 'ubi_get_vol_info()' except that it accepts UBI
 * volume name, not UBI volume ID. If the UBI device @dev_num does not exist,
 * or if the UBI volume @name does not exist, errno is set to @ENODEV.
 */
int ubi_get_vol_info1_nm(libubi_t desc, int dev_num, const char *name,
			 struct ubi_vol_info *info);

/**
 * ubi_vol_block_create - create a block device on top of an UBI volume.
 * @fd: volume character device file descriptor
 *
 * Returns %0 in case of success and %-1 in case of failure.
 */
int ubi_vol_block_create(int fd);

/**
 * ubi_vol_block_remove - remove a block device from an UBI volume.
 * @fd: volume character device file descriptor
 *
 * Returns %0 in case of success and %-1 in case of failure.
 */
int ubi_vol_block_remove(int fd);

/**
 * ubi_update_start - start UBI volume update.
 * @desc: UBI library descriptor
 * @fd: volume character device file descriptor
 * @bytes: how many bytes will be written to the volume
 *
 * This function initiates UBI volume update and returns %0 in case of success
 * and %-1 in case of error. The caller is assumed to write @bytes data to the
 * volume @fd afterward.
 */
int ubi_update_start(libubi_t desc, int fd, long long bytes);

/**
 * ubi_leb_change_start - start atomic LEB change.
 * @desc: UBI library descriptor
 * @fd: volume character device file descriptor
 * @lnum: LEB number to change
 * @bytes: how many bytes of new data will be written to the LEB
 *
 * This function initiates atomic LEB change operation and returns %0 in case
 * of success and %-1 in case of error. he caller is assumed to write @bytes
 * data to the volume @fd afterward.
 */
int ubi_leb_change_start(libubi_t desc, int fd, int lnum, int bytes);

/**
 * ubi_set_property - set volume propety.
 * @fd: volume character device file descriptor
 * @property: the property to change (%UBI_VOL_PROP_DIRECT_WRITE, etc)
 * @value: new value of the changed property
 *
 * This function changes a property of a volume. Returns zero in case of
 * success and a negative error code in case of error.
 */
int ubi_set_property(int fd, uint8_t property, uint64_t value);

/**
 * ubi_leb_unmap - unmap a logical eraseblock.
 * @fd: volume character device file descriptor
 * @lnum: logical eraseblock to unmap
 *
 * This function unmaps LEB @lnum and returns zero in case of success and a
 * negative error code in case of error.
 */
int ubi_leb_unmap(int fd, int lnum);

/**
 * ubi_is_mapped - check if logical eraseblock is mapped.
 * @fd: volume character device file descriptor
 * @lnum: logical eraseblock number
 *
 * This function checks if logical eraseblock @lnum is mapped to a physical
 * eraseblock. If a logical eraseblock is un-mapped, this does not necessarily
 * mean it will still be un-mapped after the UBI device is re-attached. The
 * logical eraseblock may become mapped to the physical eraseblock it was last
 * mapped to.
 *
 * This function returns %1 if the LEB is mapped, %0 if not, and %-1 in case of
 * failure. If the volume is damaged because of an interrupted update errno
 * set with %EBADF error code.
 */
int ubi_is_mapped(int fd, int lnum);

#ifdef __cplusplus
}
#endif

#endif /* !__LIBUBI_H__ */
      run: |
        docker login -u ${{ /*
 * Copyright (c) International Business Machines Corp., 2006
 * Copyright (C) 2008 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * 
 *          
 */

#ifndef __LIBUBIGEN_H__
#define __LIBUBIGEN_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * struct ubigen_info - libubigen information.
 * @leb_size: logical eraseblock size
 * @peb_size: size of the physical eraseblock
 * @min_io_size: minimum input/output unit size
 * @vid_hdr_offs: offset of the VID header
 * @data_offs: data offset
 * @ubi_ver: UBI version
 * @vtbl_size: volume table size
 * @max_volumes: maximum amount of volumes
 * @image_seq: UBI image sequence number
 */
struct ubigen_info
{
	int leb_size;
	int peb_size;
	int min_io_size;
	int vid_hdr_offs;
	int data_offs;
	int ubi_ver;
	int vtbl_size;
	int max_volumes;
	uint32_t image_seq;
};

/**
 * struct ubigen_vol_info - information about a volume.
 * @id: volume id
 * @type: volume type (%UBI_VID_DYNAMIC or %UBI_VID_STATIC)
 * @alignment: volume alignment
 * @data_pad: how many bytes are unused at the end of the each physical
 *            eraseblock to satisfy the requested alignment
 * @usable_leb_size: LEB size accessible for volume users
 * @name: volume name
 * @name_len: volume name length
 * @compat: compatibility of this volume (%0, %UBI_COMPAT_DELETE,
 *          %UBI_COMPAT_IGNORE, %UBI_COMPAT_PRESERVE, or %UBI_COMPAT_REJECT)
 * @used_ebs: total number of used logical eraseblocks in this volume (relevant
 *            for static volumes only)
 * @bytes: size of the volume contents in bytes (relevant for static volumes
 *         only)
 * @flags: volume flags (%UBI_VTBL_AUTORESIZE_FLG)
 */
struct ubigen_vol_info
{
	int id;
	int type;
	int alignment;
	int data_pad;
	int usable_leb_size;
	const char *name;
	int name_len;
	int compat;
	int used_ebs;
	long long bytes;
	uint8_t flags;
};

/**
 * ubigen_info_init - initialize libubigen.
 * @ui: libubigen information
 * @peb_size: flash physical eraseblock size
 * @min_io_size: flash minimum input/output unit size
 * @subpage_size: flash sub-page, if present (has to be equivalent to
 *                @min_io_size if does not exist)
 * @vid_hdr_offs: offset of the VID header
 * @ubi_ver: UBI version
 * @image_seq: UBI image sequence number
 */
void ubigen_info_init(struct ubigen_info *ui, int peb_size, int min_io_size,
		      int subpage_size, int vid_hdr_offs, int ubi_ver,
		      uint32_t image_seq);

/**
 * ubigen_create_empty_vtbl - creates empty volume table.
 * @ui: libubigen information
 *
 * This function creates an empty volume table and returns a pointer to it in
 * case of success and %NULL in case of failure. The returned object has to be
 * freed with 'free()' call.
 */
struct ubi_vtbl_record *ubigen_create_empty_vtbl(const struct ubigen_info *ui);

/**
 * ubigen_init_ec_hdr - initialize EC header.
 * @ui: libubigen information
 * @hdr: the EC header to initialize
 * @ec: erase counter value
 */
void ubigen_init_ec_hdr(const struct ubigen_info *ui,
		        struct ubi_ec_hdr *hdr, long long ec);

/**
 * ubigen_init_vid_hdr - initialize VID header.
 * @ui: libubigen information
 * @vi: volume information
 * @hdr: the VID header to initialize
 * @lnum: logical eraseblock number
 * @data: the contents of the LEB (static volumes only)
 * @data_size: amount of data in this LEB (static volumes only)
 *
 * Note, @used_ebs, @data and @data_size are ignored in case of dynamic
 * volumes.
 */
void ubigen_init_vid_hdr(const struct ubigen_info *ui,
			 const struct ubigen_vol_info *vi,
			 struct ubi_vid_hdr *hdr, int lnum,
			 const void *data, int data_size);

/**
 * ubigen_add_volume - add a volume to the volume table.
 * @ui: libubigen information
 * @vi: volume information
 * @vtbl: volume table to add to
 *
 * This function adds volume described by input parameters to the volume table
 * @vtbl.
 */
int ubigen_add_volume(const struct ubigen_info *ui,
		      const struct ubigen_vol_info *vi,
		      struct ubi_vtbl_record *vtbl);

/**
 * ubigen_write_volume - write UBI volume.
 * @ui: libubigen information
 * @vi: volume information
 * @ec: erase counter value to put to EC headers
 * @bytes: volume size in bytes
 * @in: input file descriptor (has to be properly seeked)
 * @out: output file descriptor
 *
 * This function reads the contents of the volume from the input file @in and
 * writes the UBI volume to the output file @out. Returns zero on success and
 * %-1 on failure.
 */
int ubigen_write_volume(const struct ubigen_info *ui,
			const struct ubigen_vol_info *vi, long long ec,
			long long bytes, int in, int out);

/**
 * ubigen_write_layout_vol - write UBI layout volume
 * @ui: libubigen information
 * @peb1: physical eraseblock number to write the first volume table copy
 * @peb2: physical eraseblock number to write the second volume table copy
 * @ec1: erase counter value for @peb1
 * @ec2: erase counter value for @peb1
 * @vtbl: volume table
 * @fd: output file descriptor seeked to the proper position
 *
 * This function creates the UBI layout volume which contains 2 copies of the
 * volume table. Returns zero in case of success and %-1 in case of failure.
 */
int ubigen_write_layout_vol(const struct ubigen_info *ui, int peb1, int peb2,
			    long long ec1, long long ec2,
			    struct ubi_vtbl_record *vtbl, int fd);

#ifdef __cplusplus
}
#endif

#endif /* !__LIBUBIGEN_H__ */ }} -p '${{ #ifndef MTD_SWAB_H
#define MTD_SWAB_H

#include <endian.h>

#define swab16(x) \
        ((uint16_t)( \
                (((uint16_t)(x) & (uint16_t)0x00ffU) << 8) | \
                (((uint16_t)(x) & (uint16_t)0xff00U) >> 8) ))
#define swab32(x) \
        ((uint32_t)( \
                (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) | \
                (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) | \
                (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) | \
                (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24) ))

#define swab64(x) \
		((uint64_t)( \
				(((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) | \
				(((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
				(((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
				(((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) << 8) | \
				(((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >> 8) | \
				(((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
				(((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
				(((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56) ))


#if __BYTE_ORDER == __BIG_ENDIAN
#define cpu_to_le16(x) ({ uint16_t _x = x; swab16(_x); })
#define cpu_to_le32(x) ({ uint32_t _x = x; swab32(_x); })
#define cpu_to_le64(x) ({ uint64_t _x = x; swab64(_x); })
#define cpu_to_be16(x) (x)
#define cpu_to_be32(x) (x)
#define cpu_to_be64(x) (x)
#else
#define cpu_to_le16(x) (x)
#define cpu_to_le32(x) (x)
#define cpu_to_le64(x) (x)
#define cpu_to_be16(x) ({ uint16_t _x = x; swab16(_x); })
#define cpu_to_be32(x) ({ uint32_t _x = x; swab32(_x); })
#define cpu_to_be64(x) ({ uint64_t _x = x; swab64(_x); })
#endif
#define le16_to_cpu(x) cpu_to_le16(x)
#define be16_to_cpu(x) cpu_to_be16(x)
#define le32_to_cpu(x) cpu_to_le32(x)
#define be32_to_cpu(x) cpu_to_be32(x)
#define le64_to_cpu(x) cpu_to_le64(x)
#define be64_to_cpu(x) cpu_to_be64(x)

#endif }}' ${ /*
 * memory wrappers
 *
 * 
 * Copyright 2001, 2002 Red Hat, Inc.
 *           
 *           2002 Axis Communications AB
 *           
 *           2004 University of Szeged, Hungary
 *           
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __MTD_UTILS_XALLOC_H__
#define __MTD_UTILS_XALLOC_H__

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/*
 * Mark these functions as unused so that gcc does not emit warnings
 * when people include this header but don't use every function.
 */

__attribute__((unused))
static void *xmalloc(size_t size)
{
	void *ptr = malloc(size);

	if (ptr == NULL && size != 0)
		sys_errmsg_die("out of memory");
	return ptr;
}

__attribute__((unused))
static void *xcalloc(size_t nmemb, size_t size)
{
	void *ptr = calloc(nmemb, size);

	if (ptr == NULL && nmemb != 0 && size != 0)
		sys_errmsg_die("out of memory");
	return ptr;
}

__attribute__((unused))
static void *xzalloc(size_t size)
{
	return xcalloc(1, size);
}

__attribute__((unused))
static void *xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (ptr == NULL && size != 0)
		sys_errmsg_die("out of memory");
	return ptr;
}

__attribute__((unused))
static char *xstrdup(const char *s)
{
	char *t;

	if (s == NULL)
		return NULL;
	t = strdup(s);
	if (t == NULL)
		sys_errmsg_die("out of memory");
	return t;
}

#ifdef _GNU_SOURCE

__attribute__((unused))
static int xasprintf(char **strp, const char *fmt, ...)
{
	int cnt;
	va_list ap;

	va_start(ap, fmt);
	cnt = vasprintf(strp, fmt, ap);
	va_end(ap);

	if (cnt == -1)
		sys_errmsg_die("out of memory");

	return cnt;
}
#endif

#endif /* !__MTD_UTILS_XALLOC_H__ */ }

    # Push the Docker image to TKE Registry
    - name: mkfs_jffs2_SOURCES = \
	jffsX-utils/mkfs.jffs2.c \
	jffsX-utils/rbtree.h \
	jffsX-utils/compr.h \
	jffsX-utils/rbtree.c \
	jffsX-utils/compr.c \
	jffsX-utils/compr_rtime.c \
	jffsX-utils/compr.h \
	jffsX-utils/rbtree.h \
	jffsX-utils/summary.h \
	include/linux/jffs2.h \
	include/mtd/jffs2-user.h
mkfs_jffs2_LDADD = libmtd.a $(ZLIB_LIBS) $(LZO_LIBS)
mkfs_jffs2_CPPFLAGS = $(AM_CPPFLAGS) $(ZLIB_CFLAGS) $(LZO_CFLAGS)

jffs2reader_SOURCES = jffsX-utils/jffs2reader.c	include/mtd/jffs2-user.h
jffs2reader_LDADD = libmtd.a $(ZLIB_LIBS) $(LZO_LIBS)
jffs2reader_CPPFLAGS = $(AM_CPPFLAGS) $(ZLIB_CFLAGS) $(LZO_CFLAGS)

jffs2dump_SOURCES = jffsX-utils/jffs2dump.c include/mtd/jffs2-user.h
jffs2dump_SOURCES += jffsX-utils/summary.h
jffs2dump_LDADD = libmtd.a $(ZLIB_LIBS) $(LZO_LIBS)
jffs2dump_CPPFLAGS = $(AM_CPPFLAGS) $(ZLIB_CFLAGS) $(LZO_CFLAGS)

sumtool_SOURCES = jffsX-utils/sumtool.c jffsX-utils/summary.h
sumtool_LDADD = libmtd.a

if WITH_LZO
mkfs_jffs2_SOURCES += jffsX-utils/compr_lzo.c
endif

if WITH_ZLIB
mkfs_jffs2_SOURCES += jffsX-utils/compr_zlib.c
endif

EXTRA_DIST += jffsX-utils/device_table.txt jffsX-utils/mkfs.jffs2.1

dist_man1_MANS += jffsX-utils/mkfs.jffs2.1
sbin_PROGRAMS += mkfs.jffs2 jffs2dump jffs2reader sumtool
      run: /*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2004 
 *                    University of Szeged, Hungary
 *
 * For licensing information, see the file 'LICENCE' in this directory
 * in the jffs2 directory.
 */

#include "compr.h"
#include <string.h>
#include <stdlib.h>
#include <linux/jffs2.h>

#define FAVOUR_LZO_PERCENT 80

extern int page_size;

/* LIST IMPLEMENTATION (from linux/list.h) */

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void __list_add(struct list_head *new,
		struct list_head *prev,
		struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = (void *) 0;
	entry->prev = (void *) 0;
}

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define list_for_each_entry(pos, head, member)                          \
	for (pos = list_entry((head)->next, typeof(*pos), member);      \
			&pos->member != (head);                                    \
			pos = list_entry(pos->member.next, typeof(*pos), member))


/* Available compressors are on this list */
static LIST_HEAD(jffs2_compressor_list);

/* Actual compression mode */
static int jffs2_compression_mode = JFFS2_COMPR_MODE_PRIORITY;

void jffs2_set_compression_mode(int mode)
{
	jffs2_compression_mode = mode;
}

int jffs2_get_compression_mode(void)
{
	return jffs2_compression_mode;
}

/* Statistics for blocks stored without compression */
static uint32_t none_stat_compr_blocks=0,none_stat_decompr_blocks=0,none_stat_compr_size=0;

/* Compression test stuffs */

static int jffs2_compression_check = 0;

static unsigned char *jffs2_compression_check_buf = NULL;

void jffs2_compression_check_set(int yesno)
{
	jffs2_compression_check = yesno;
}

int jffs2_compression_check_get(void)
{
	return jffs2_compression_check;
}

static int jffs2_error_cnt = 0;

int jffs2_compression_check_errorcnt_get(void)
{
	return jffs2_error_cnt;
}

#define JFFS2_BUFFER_FILL 0x55

/* Called before compression (if compression_check is setted) to prepare
   the buffer for buffer overflow test */
static void jffs2_decompression_test_prepare(unsigned char *buf, int size)
{
	memset(buf,JFFS2_BUFFER_FILL,size+1);
}

/* Called after compression (if compression_check is setted) to test the result */
static void jffs2_decompression_test(struct jffs2_compressor *compr,
		unsigned char *data_in, unsigned char *output_buf,
		uint32_t cdatalen, uint32_t datalen, uint32_t buf_size)
{
	uint32_t i;

	/* buffer overflow test */
	for (i=buf_size;i>cdatalen;i--) {
		if (output_buf[i]!=JFFS2_BUFFER_FILL) {
			fprintf(stderr,"COMPR_ERROR: buffer overflow at %s. "
					"(bs=%d csize=%d b[%d]=%d)\n", compr->name,
					buf_size, cdatalen, i, (int)(output_buf[i]));
			jffs2_error_cnt++;
			return;
		}
	}
	/* allocing temporary buffer for decompression */
	if (!jffs2_compression_check_buf) {
		jffs2_compression_check_buf = malloc(page_size);
		if (!jffs2_compression_check_buf) {
			fprintf(stderr,"No memory for buffer allocation. Compression check disabled.\n");
			jffs2_compression_check = 0;
			return;
		}
	}
	/* decompressing */
	if (!compr->decompress) {
		fprintf(stderr,"JFFS2 compression check: there is no decompress function at %s.\n", compr->name);
		jffs2_error_cnt++;
		return;
	}
	if (compr->decompress(output_buf,jffs2_compression_check_buf,cdatalen,datalen)) {
		fprintf(stderr,"JFFS2 compression check: decompression failed at %s.\n", compr->name);
		jffs2_error_cnt++;
	}
	/* validate decompression */
	else {
		for (i=0;i<datalen;i++) {
			if (data_in[i]!=jffs2_compression_check_buf[i]) {
				fprintf(stderr,"JFFS2 compression check: data mismatch at %s (pos %d).\n", compr->name, i);
				jffs2_error_cnt++;
				break;
			}
		}
	}
}

/*
 * Return 1 to use this compression
 */
static int jffs2_is_best_compression(struct jffs2_compressor *this,
		struct jffs2_compressor *best, uint32_t size, uint32_t bestsize)
{
	switch (jffs2_compression_mode) {
	case JFFS2_COMPR_MODE_SIZE:
		if (bestsize > size)
			return 1;
		return 0;
	case JFFS2_COMPR_MODE_FAVOURLZO:
		if ((this->compr == JFFS2_COMPR_LZO) && (bestsize > size))
			return 1;
		if ((best->compr != JFFS2_COMPR_LZO) && (bestsize > size))
			return 1;
		if ((this->compr == JFFS2_COMPR_LZO) && (bestsize > (size * FAVOUR_LZO_PERCENT / 100)))
			return 1;
		if ((bestsize * FAVOUR_LZO_PERCENT / 100) > size)
			return 1;

		return 0;
	}
	/* Shouldn't happen */
	return 0;
}

/* jffs2_compress:
 * @data: Pointer to uncompressed data
 * @cdata: Pointer to returned pointer to buffer for compressed data
 * @datalen: On entry, holds the amount of data available for compression.
 *	On exit, expected to hold the amount of data actually compressed.
 * @cdatalen: On entry, holds the amount of space available for compressed
 *	data. On exit, expected to hold the actual size of the compressed
 *	data.
 *
 * Returns: Lower byte to be stored with data indicating compression type used.
 * Zero is used to show that the data could not be compressed - the
 * compressed version was actually larger than the original.
 * Upper byte will be used later. (soon)
 *
 * If the cdata buffer isn't large enough to hold all the uncompressed data,
 * jffs2_compress should compress as much as will fit, and should set
 * *datalen accordingly to show the amount of data which were compressed.
 */
uint16_t jffs2_compress( unsigned char *data_in, unsigned char **cpage_out,
		uint32_t *datalen, uint32_t *cdatalen)
{
	int ret = JFFS2_COMPR_NONE;
	int compr_ret;
	struct jffs2_compressor *this, *best=NULL;
	unsigned char *output_buf = NULL, *tmp_buf;
	uint32_t orig_slen, orig_dlen;
	uint32_t best_slen=0, best_dlen=0;

	switch (jffs2_compression_mode) {
		case JFFS2_COMPR_MODE_NONE:
			break;
		case JFFS2_COMPR_MODE_PRIORITY:
			orig_slen = *datalen;
			orig_dlen = *cdatalen;
			output_buf = malloc(orig_dlen+jffs2_compression_check);
			if (!output_buf) {
				fprintf(stderr,"mkfs.jffs2: No memory for compressor allocation. Compression failed.\n");
				goto out;
			}
			list_for_each_entry(this, &jffs2_compressor_list, list) {
				/* Skip decompress-only backwards-compatibility and disabled modules */
				if ((!this->compress)||(this->disabled))
					continue;

				this->usecount++;

				if (jffs2_compression_check) /*preparing output buffer for testing buffer overflow */
					jffs2_decompression_test_prepare(output_buf, orig_dlen);

				*datalen  = orig_slen;
				*cdatalen = orig_dlen;
				compr_ret = this->compress(data_in, output_buf, datalen, cdatalen);
				this->usecount--;
				if (!compr_ret) {
					ret = this->compr;
					this->stat_compr_blocks++;
					this->stat_compr_orig_size += *datalen;
					this->stat_compr_new_size  += *cdatalen;
					if (jffs2_compression_check)
						jffs2_decompression_test(this, data_in, output_buf, *cdatalen, *datalen, orig_dlen);
					break;
				}
			}
			if (ret == JFFS2_COMPR_NONE) free(output_buf);
			break;
		case JFFS2_COMPR_MODE_FAVOURLZO:
		case JFFS2_COMPR_MODE_SIZE:
			orig_slen = *datalen;
			orig_dlen = *cdatalen;
			list_for_each_entry(this, &jffs2_compressor_list, list) {
				uint32_t needed_buf_size;

				if (jffs2_compression_mode == JFFS2_COMPR_MODE_FAVOURLZO)
					needed_buf_size = orig_slen + jffs2_compression_check;
				else
					needed_buf_size = orig_dlen + jffs2_compression_check;

				/* Skip decompress-only backwards-compatibility and disabled modules */
				if ((!this->compress)||(this->disabled))
					continue;
				/* Allocating memory for output buffer if necessary */
				if ((this->compr_buf_size < needed_buf_size) && (this->compr_buf)) {
					free(this->compr_buf);
					this->compr_buf_size=0;
					this->compr_buf=NULL;
				}
				if (!this->compr_buf) {
					tmp_buf = malloc(needed_buf_size);
					if (!tmp_buf) {
						fprintf(stderr,"mkfs.jffs2: No memory for compressor allocation. (%d bytes)\n",orig_dlen);
						continue;
					}
					else {
						this->compr_buf = tmp_buf;
						this->compr_buf_size = orig_dlen;
					}
				}
				this->usecount++;
				if (jffs2_compression_check) /*preparing output buffer for testing buffer overflow */
					jffs2_decompression_test_prepare(this->compr_buf,this->compr_buf_size);
				*datalen  = orig_slen;
				*cdatalen = orig_dlen;
				compr_ret = this->compress(data_in, this->compr_buf, datalen, cdatalen);
				this->usecount--;
				if (!compr_ret) {
					if (jffs2_compression_check)
						jffs2_decompression_test(this, data_in, this->compr_buf, *cdatalen, *datalen, this->compr_buf_size);
					if (((!best_dlen) || jffs2_is_best_compression(this, best, *cdatalen, best_dlen))
								&& (*cdatalen < *datalen)) {
						best_dlen = *cdatalen;
						best_slen = *datalen;
						best = this;
					}
				}
			}
			if (best_dlen) {
				*cdatalen = best_dlen;
				*datalen  = best_slen;
				output_buf = best->compr_buf;
				best->compr_buf = NULL;
				best->compr_buf_size = 0;
				best->stat_compr_blocks++;
				best->stat_compr_orig_size += best_slen;
				best->stat_compr_new_size  += best_dlen;
				ret = best->compr;
			}
			break;
		default:
			fprintf(stderr,"mkfs.jffs2: unknown compression mode.\n");
	}
out:
	if (ret == JFFS2_COMPR_NONE) {
		*cpage_out = data_in;
		*datalen = *cdatalen;
		none_stat_compr_blocks++;
		none_stat_compr_size += *datalen;
	}
	else {
		*cpage_out = output_buf;
	}
	return ret;
}


int jffs2_register_compressor(struct jffs2_compressor *comp)
{
	struct jffs2_compressor *this;

	if (!comp->name) {
		fprintf(stderr,"NULL compressor name at registering JFFS2 compressor. Failed.\n");
		return -1;
	}
	comp->compr_buf_size=0;
	comp->compr_buf=NULL;
	comp->usecount=0;
	comp->stat_compr_orig_size=0;
	comp->stat_compr_new_size=0;
	comp->stat_compr_blocks=0;
	comp->stat_decompr_blocks=0;

	list_for_each_entry(this, &jffs2_compressor_list, list) {
		if (this->priority < comp->priority) {
			list_add(&comp->list, this->list.prev);
			goto out;
		}
	}
	list_add_tail(&comp->list, &jffs2_compressor_list);
out:
	return 0;
}

int jffs2_unregister_compressor(struct jffs2_compressor *comp)
{

	if (comp->usecount) {
		fprintf(stderr,"mkfs.jffs2: Compressor modul is in use. Unregister failed.\n");
		return -1;
	}
	list_del(&comp->list);

	return 0;
}

#define JFFS2_STAT_BUF_SIZE 16000

char *jffs2_list_compressors(void)
{
	struct jffs2_compressor *this;
	char *buf, *act_buf;

	act_buf = buf = malloc(JFFS2_STAT_BUF_SIZE);
	list_for_each_entry(this, &jffs2_compressor_list, list) {
		act_buf += sprintf(act_buf, "%10s priority:%d ", this->name, this->priority);
		if ((this->disabled)||(!this->compress))
			act_buf += sprintf(act_buf,"disabled");
		else
			act_buf += sprintf(act_buf,"enabled");
		act_buf += sprintf(act_buf,"\n");
	}
	return buf;
}

char *jffs2_stats(void)
{
	struct jffs2_compressor *this;
	char *buf, *act_buf;

	act_buf = buf = malloc(JFFS2_STAT_BUF_SIZE);

	act_buf += sprintf(act_buf,"Compression mode: ");
	switch (jffs2_compression_mode) {
		case JFFS2_COMPR_MODE_NONE:
			act_buf += sprintf(act_buf,"none");
			break;
		case JFFS2_COMPR_MODE_PRIORITY:
			act_buf += sprintf(act_buf,"priority");
			break;
		case JFFS2_COMPR_MODE_SIZE:
			act_buf += sprintf(act_buf,"size");
			break;
		case JFFS2_COMPR_MODE_FAVOURLZO:
			act_buf += sprintf(act_buf, "favourlzo");
			break;
		default:
			act_buf += sprintf(act_buf, "unknown");
			break;
	}
	act_buf += sprintf(act_buf,"\nCompressors:\n");
	act_buf += sprintf(act_buf,"%10s             ","none");
	act_buf += sprintf(act_buf,"compr: %d blocks (%d)  decompr: %d blocks\n", none_stat_compr_blocks,
			none_stat_compr_size, none_stat_decompr_blocks);
	list_for_each_entry(this, &jffs2_compressor_list, list) {
		act_buf += sprintf(act_buf,"%10s (prio:%d) ",this->name,this->priority);
		if ((this->disabled)||(!this->compress))
			act_buf += sprintf(act_buf,"- ");
		else
			act_buf += sprintf(act_buf,"+ ");
		act_buf += sprintf(act_buf,"compr: %d blocks (%d/%d)  decompr: %d blocks ", this->stat_compr_blocks,
				this->stat_compr_new_size, this->stat_compr_orig_size,
				this->stat_decompr_blocks);
		act_buf += sprintf(act_buf,"\n");
	}
	return buf;
}

int jffs2_set_compression_mode_name(const char *name)
{
	if (!strcmp("none",name)) {
		jffs2_compression_mode = JFFS2_COMPR_MODE_NONE;
		return 0;
	}
	if (!strcmp("priority",name)) {
		jffs2_compression_mode = JFFS2_COMPR_MODE_PRIORITY;
		return 0;
	}
	if (!strcmp("size",name)) {
		jffs2_compression_mode = JFFS2_COMPR_MODE_SIZE;
		return 0;
	}
	if (!strcmp("favourlzo", name)) {
		jffs2_compression_mode = JFFS2_COMPR_MODE_FAVOURLZO;
		return 0;
	}

	return 1;
}

static int jffs2_compressor_Xable(const char *name, int disabled)
{
	struct jffs2_compressor *this;
	list_for_each_entry(this, &jffs2_compressor_list, list) {
		if (!strcmp(this->name, name)) {
			this->disabled = disabled;
			return 0;
		}
	}
	return 1;
}

int jffs2_enable_compressor_name(const char *name)
{
	return jffs2_compressor_Xable(name, 0);
}

int jffs2_disable_compressor_name(const char *name)
{
	return jffs2_compressor_Xable(name, 1);
}

int jffs2_set_compressor_priority(const char *name, int priority)
{
	struct jffs2_compressor *this,*comp;
	list_for_each_entry(this, &jffs2_compressor_list, list) {
		if (!strcmp(this->name, name)) {
			this->priority = priority;
			comp = this;
			goto reinsert;
		}
	}
	fprintf(stderr,"mkfs.jffs2: compressor %s not found.\n",name);
	return 1;
reinsert:
	/* list is sorted in the order of priority, so if
	   we change it we have to reinsert it into the
	   good place */
	list_del(&comp->list);
	list_for_each_entry(this, &jffs2_compressor_list, list) {
		if (this->priority < comp->priority) {
			list_add(&comp->list, this->list.prev);
			return 0;
		}
	}
	list_add_tail(&comp->list, &jffs2_compressor_list);
	return 0;
}


int jffs2_compressors_init(void)
{
#ifdef WITH_ZLIB
	jffs2_zlib_init();
#endif
#ifdef CONFIG_JFFS2_RTIME
	jffs2_rtime_init();
#endif
#ifdef WITH_LZO
	jffs2_lzo_init();
#endif
	return 0;
}

int jffs2_compressors_exit(void)
{
#ifdef CONFIG_JFFS2_RTIME
	jffs2_rtime_exit();
#endif
#ifdef WITH_ZLIB
	jffs2_zlib_exit();
#endif
#ifdef WITH_LZO
	jffs2_lzo_exit();
#endif
	return 0;
}
        

    - name: /*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2004 >,
 *                    University of Szeged, Hungary
 *
 * For licensing information, see the file 'LICENCE' in the
 * jffs2 directory.
 */

#ifndef __JFFS2_COMPR_H__
#define __JFFS2_COMPR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "linux/jffs2.h"

#define CONFIG_JFFS2_RTIME

#define JFFS2_RUBINMIPS_PRIORITY 10
#define JFFS2_DYNRUBIN_PRIORITY  20
#define JFFS2_RTIME_PRIORITY     50
#define JFFS2_ZLIB_PRIORITY      60
#define JFFS2_LZO_PRIORITY       80

#define JFFS2_COMPR_MODE_NONE       0
#define JFFS2_COMPR_MODE_PRIORITY   1
#define JFFS2_COMPR_MODE_SIZE       2
#define JFFS2_COMPR_MODE_FAVOURLZO  3

#define kmalloc(a,b)                malloc(a)
#define kfree(a)                    free(a)
#ifndef GFP_KERNEL
#define GFP_KERNEL                  0
#endif

#define vmalloc(a)                  malloc(a)
#define vfree(a)                    free(a)

#define printk(...)                 fprintf(stderr,__VA_ARGS__)

#define KERN_EMERG
#define KERN_ALERT
#define KERN_CRIT
#define KERN_ERR
#define KERN_WARNING
#define KERN_NOTICE
#define KERN_INFO
#define KERN_DEBUG

struct list_head {
	struct list_head *next, *prev;
};

void jffs2_set_compression_mode(int mode);
int jffs2_get_compression_mode(void);
int jffs2_set_compression_mode_name(const char *mode_name);

int jffs2_enable_compressor_name(const char *name);
int jffs2_disable_compressor_name(const char *name);

int jffs2_set_compressor_priority(const char *name, int priority);

struct jffs2_compressor {
	struct list_head list;
	int priority;             /* used by prirority comr. mode */
	const char *name;
	char compr;               /* JFFS2_COMPR_XXX */
	int (*compress)(unsigned char *data_in, unsigned char *cpage_out,
			uint32_t *srclen, uint32_t *destlen);
	int (*decompress)(unsigned char *cdata_in, unsigned char *data_out,
			uint32_t cdatalen, uint32_t datalen);
	int usecount;
	int disabled;             /* if seted the compressor won't compress */
	unsigned char *compr_buf; /* used by size compr. mode */
	uint32_t compr_buf_size;  /* used by size compr. mode */
	uint32_t stat_compr_orig_size;
	uint32_t stat_compr_new_size;
	uint32_t stat_compr_blocks;
	uint32_t stat_decompr_blocks;
};

int jffs2_register_compressor(struct jffs2_compressor *comp);
int jffs2_unregister_compressor(struct jffs2_compressor *comp);

int jffs2_compressors_init(void);
int jffs2_compressors_exit(void);

uint16_t jffs2_compress(unsigned char *data_in, unsigned char **cpage_out,
		uint32_t *datalen, uint32_t *cdatalen);

/* If it is setted, a decompress will be called after every compress */
void jffs2_compression_check_set(int yesno);
int jffs2_compression_check_get(void);
int jffs2_compression_check_errorcnt_get(void);

char *jffs2_list_compressors(void);
char *jffs2_stats(void);

/* Compressor modules */

/* These functions will be called by jffs2_compressors_init/exit */
#ifdef WITH_ZLIB
int jffs2_zlib_init(void);
void jffs2_zlib_exit(void);
#endif
#ifdef CONFIG_JFFS2_RTIME
int jffs2_rtime_init(void);
void jffs2_rtime_exit(void);
#endif
#ifdef WITH_LZO
int jffs2_lzo_init(void);
void jffs2_lzo_exit(void);
#endif

#endif /* __JFFS2_COMPR_H__ */
      run: /*
 * JFFS2 LZO Compression Interface.
 *
 * Copyright (C) 2007 Nokia Corporation. All rights reserved.
 *
 * 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <asm/types.h>
#include <linux/jffs2.h>
#include <lzo/lzo1x.h>
#include "compr.h"

extern int page_size;

static void *lzo_mem;
static void *lzo_compress_buf;

/*
 * Note about LZO compression.
 *
 * We want to use the _999_ compression routine which gives better compression
 * rates at the expense of time. Decompression time is unaffected. We might as
 * well use the standard lzo library routines for this but they will overflow
 * the destination buffer since they don't check the destination size.
 *
 * We therefore compress to a temporary buffer and copy if it will fit.
 *
 */
static int jffs2_lzo_cmpr(unsigned char *data_in, unsigned char *cpage_out,
			  uint32_t *sourcelen, uint32_t *dstlen)
{
	lzo_uint compress_size;
	int ret;

	ret = lzo1x_999_compress(data_in, *sourcelen, lzo_compress_buf, &compress_size, lzo_mem);

	if (ret != LZO_E_OK)
		return -1;

	if (compress_size > *dstlen)
		return -1;

	memcpy(cpage_out, lzo_compress_buf, compress_size);
	*dstlen = compress_size;

	return 0;
}

static int jffs2_lzo_decompress(unsigned char *data_in, unsigned char *cpage_out,
				 uint32_t srclen, uint32_t destlen)
{
	int ret;
	lzo_uint dl;

	dl = destlen;
	ret = lzo1x_decompress_safe(data_in,srclen,cpage_out,&dl,NULL);

	if (ret != LZO_E_OK || dl != destlen)
		return -1;

	return 0;
}

static struct jffs2_compressor jffs2_lzo_comp = {
	.priority = JFFS2_LZO_PRIORITY,
	.name = "lzo",
	.compr = JFFS2_COMPR_LZO,
	.compress = &jffs2_lzo_cmpr,
	.decompress = &jffs2_lzo_decompress,
	.disabled = 1,
};

int jffs2_lzo_init(void)
{
	int ret;

	lzo_mem = malloc(LZO1X_999_MEM_COMPRESS);
	if (!lzo_mem)
		return -1;

	/* Worse case LZO compression size from their FAQ */
	lzo_compress_buf = malloc(page_size + (page_size / 16) + 64 + 3);
	if (!lzo_compress_buf) {
		free(lzo_mem);
		return -1;
	}

	ret = jffs2_register_compressor(&jffs2_lzo_comp);
	if (ret < 0) {
		free(lzo_compress_buf);
		free(lzo_mem);
	}

	return ret;
}

void jffs2_lzo_exit(void)
{
	jffs2_unregister_compressor(&jffs2_lzo_comp);
	free(lzo_compress_buf);
	free(lzo_mem);
}
         

    - name: /*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Red Hat, Inc.
 *
 * 
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * Very simple lz77-ish encoder.
 *
 * Theory of operation: Both encoder and decoder have a list of "last
 * occurrences" for every possible source-value; after sending the
 * first source-byte, the second byte indicated the "run" length of
 * matches
 *
 * The algorithm is intended to only send "whole bytes", no bit-messing.
 *
 */

#include <stdint.h>
#include <string.h>
#include "compr.h"

/* _compress returns the compressed size, -1 if bigger */
static int jffs2_rtime_compress(unsigned char *data_in, unsigned char *cpage_out,
		uint32_t *sourcelen, uint32_t *dstlen)
{
	short positions[256];
	int outpos = 0;
	int pos=0;

	memset(positions,0,sizeof(positions));

	while (pos < (*sourcelen) && outpos+2 <= (*dstlen)) {
		int backpos, runlen=0;
		unsigned char value;

		value = data_in[pos];

		cpage_out[outpos++] = data_in[pos++];

		backpos = positions[value];
		positions[value]=pos;

		while ((backpos < pos) && (pos < (*sourcelen)) &&
				(data_in[pos]==data_in[backpos++]) && (runlen<255)) {
			pos++;
			runlen++;
		}
		cpage_out[outpos++] = runlen;
	}

	if (outpos >= pos) {
		/* We failed */
		return -1;
	}

	/* Tell the caller how much we managed to compress, and how much space it took */
	*sourcelen = pos;
	*dstlen = outpos;
	return 0;
}


static int jffs2_rtime_decompress(unsigned char *data_in, unsigned char *cpage_out,
		__attribute__((unused)) uint32_t srclen, uint32_t destlen)
{
	short positions[256];
	int outpos = 0;
	int pos=0;

	memset(positions,0,sizeof(positions));

	while (outpos<destlen) {
		unsigned char value;
		int backoffs;
		int repeat;

		value = data_in[pos++];
		cpage_out[outpos++] = value; /* first the verbatim copied byte */
		repeat = data_in[pos++];
		backoffs = positions[value];

		positions[value]=outpos;
		if (repeat) {
			if (backoffs + repeat >= outpos) {
				while(repeat) {
					cpage_out[outpos++] = cpage_out[backoffs++];
					repeat--;
				}
			} else {
				memcpy(&cpage_out[outpos],&cpage_out[backoffs],repeat);
				outpos+=repeat;
			}
		}
	}
	return 0;
}


static struct jffs2_compressor jffs2_rtime_comp = {
	.priority = JFFS2_RTIME_PRIORITY,
	.name = "rtime",
	.disabled = 0,
	.compr = JFFS2_COMPR_RTIME,
	.compress = &jffs2_rtime_compress,
	.decompress = &jffs2_rtime_decompress,
};

int jffs2_rtime_init(void)
{
	return jffs2_register_compressor(&jffs2_rtime_comp);
}

void jffs2_rtime_exit(void)
{
	jffs2_unregister_compressor(&jffs2_rtime_comp);
}
      uses: /*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001 Red Hat, Inc.
 *
 *   
 * The original JFFS, from which the design for JFFS2 was derived,
 * was designed and implemented by Axis Communications AB.
 *
 * The contents of this file are subject to the Red Hat eCos Public
 * License Version 1.1 (the "Licence"); you may not use this file
 * except in compliance with the Licence.  You may obtain a copy of
 * the Licence at http://www.redhat.com/
 *
 * Software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.
 * See the Licence for the specific language governing rights and
 * limitations under the Licence.
 *
 * The Original Code is JFFS2 - Journalling Flash File System, version 2
 *
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License version 2 (the "GPL"), in
 * which case the provisions of the GPL are applicable instead of the
 * above.  If you wish to allow the use of your version of this file
 * only under the terms of the GPL and not to allow others to use your
 * version of this file under the RHEPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GPL.  If you do not delete the
 * provisions above, a recipient may use your version of this file
 * under either the RHEPL or the GPL.
 */

#define PROGRAM_NAME "compr_zlib"

#include <stdint.h>
#define crc32 __zlib_crc32
#include <zlib.h>
#undef crc32
#include <stdio.h>
#include <asm/types.h>
#include <linux/jffs2.h>
#include "common.h"
#include "compr.h"

/* Plan: call deflate() with avail_in == *sourcelen,
   avail_out = *dstlen - 12 and flush == Z_FINISH.
   If it doesn't manage to finish,	call it again with
   avail_in == 0 and avail_out set to the remaining 12
   bytes for it to clean up.
Q: Is 12 bytes sufficient?
 */
#define STREAM_END_SPACE 12

static int jffs2_zlib_compress(unsigned char *data_in, unsigned char *cpage_out,
		uint32_t *sourcelen, uint32_t *dstlen)
{
	z_stream strm;
	int ret;

	if (*dstlen <= STREAM_END_SPACE)
		return -1;

	strm.zalloc = (void *)0;
	strm.zfree = (void *)0;

	if (Z_OK != deflateInit(&strm, 3)) {
		return -1;
	}
	strm.next_in = data_in;
	strm.total_in = 0;

	strm.next_out = cpage_out;
	strm.total_out = 0;

	while (strm.total_out < *dstlen - STREAM_END_SPACE && strm.total_in < *sourcelen) {
		strm.avail_out = *dstlen - (strm.total_out + STREAM_END_SPACE);
		strm.avail_in = min((unsigned)(*sourcelen-strm.total_in), strm.avail_out);
		ret = deflate(&strm, Z_PARTIAL_FLUSH);
		if (ret != Z_OK) {
			deflateEnd(&strm);
			return -1;
		}
	}
	strm.avail_out += STREAM_END_SPACE;
	strm.avail_in = 0;
	ret = deflate(&strm, Z_FINISH);
	if (ret != Z_STREAM_END) {
		deflateEnd(&strm);
		return -1;
	}
	deflateEnd(&strm);

	if (strm.total_out >= strm.total_in)
		return -1;


	*dstlen = strm.total_out;
	*sourcelen = strm.total_in;
	return 0;
}

static int jffs2_zlib_decompress(unsigned char *data_in, unsigned char *cpage_out,
		uint32_t srclen, uint32_t destlen)
{
	z_stream strm;
	int ret;

	strm.zalloc = (void *)0;
	strm.zfree = (void *)0;

	if (Z_OK != inflateInit(&strm)) {
		return 1;
	}
	strm.next_in = data_in;
	strm.avail_in = srclen;
	strm.total_in = 0;

	strm.next_out = cpage_out;
	strm.avail_out = destlen;
	strm.total_out = 0;

	while((ret = inflate(&strm, Z_FINISH)) == Z_OK)
		;

	inflateEnd(&strm);
	return 0;
}

static struct jffs2_compressor jffs2_zlib_comp = {
	.priority = JFFS2_ZLIB_PRIORITY,
	.name = "zlib",
	.disabled = 0,
	.compr = JFFS2_COMPR_ZLIB,
	.compress = &jffs2_zlib_compress,
	.decompress = &jffs2_zlib_decompress,
};

int jffs2_zlib_init(void)
{
	return jffs2_register_compressor(&jffs2_zlib_comp);
}

void jffs2_zlib_exit(void)
{
	jffs2_unregister_compressor(&jffs2_zlib_comp);
}
      with: # This is a sample device table file for use with mkfs.jffs2.  You can
# do all sorts of interesting things with a device table file.  For
# example, if you want to adjust the permissions on a particular file
# you can just add an entry like:
#   /sbin/foobar	f	2755	0	0	-	-	-	-	-
# and (assuming the file /sbin/foobar exists) it will be made setuid
# root (regardless of what its permissions are on the host filesystem.
#
# Device table entries take the form of:
# <name>		<type>	<mode>	<uid>	<gid>	<major>	<minor>	<start>	<inc>	<count>
# where name is the file name,  type can be one of:
#	f	A regular file
#	d	Directory
#	c	Character special device file
#	b	Block special device file
#	p	Fifo (named pipe)
#	l	Link
# uid is the user id for the target file, gid is the group id for the
# target file.  The rest of the entried apply only to device special
# file.

# When building a target filesystem, it is desirable to not have to
# become root and then run 'mknod' a thousand times.  Using a device
# table you can create device nodes and directories "on the fly".
# Furthermore, you can use a single table entry to create a many device
# minors.  For example, if I wanted to create /dev/hda and /dev/hda[0-15]
# I could just use the following two table entries:
#   /dev/hda	b	640	0	0	3	0	0	0	-
#   /dev/hda	b	640	0	0	3	1	1	1	15
#
# Have fun
# -
#

#<name>		<type>	<mode>	<uid>	<gid>	<major>	<minor>	<start>	<inc>	<count>
/dev		d	755	0	0	-	-	-	-	-
/dev/mem	c	640	0	0	1	1	0	0	-
/dev/kmem	c	640	0	0	1	2	0	0	-
/dev/null	c	640	0	0	1	3	0	0	-
/dev/zero	c	640	0	0	1	5	0	0	-
/dev/random	c	640	0	0	1	8	0	0	-
/dev/urandom	c	640	0	0	1	9	0	0	-
/dev/tty	c	666	0	0	5	0	0	0	-
/dev/tty	c	666	0	0	4	0	0	1	6
/dev/console	c	640	0	0	5	1	0	0	-
/dev/ram	b	640	0	0	1	1	0	0	-
/dev/ram	b	640	0	0	1	0	0	1	4
/dev/loop	b	640	0	0	7	0	0	1	2
/dev/ptmx	c	666	0	0	5	2	0	0	-
#/dev/ttyS	c	640	0	0	4	64	0	1	4
#/dev/psaux	c	640	0	0	10	1	0	0	-
#/dev/rtc	c	640	0	0	10	135	0	0	-

# Adjust permissions on some normal files
#/etc/shadow	f	600	0	0	-	-	-	-	-
#/bin/tinylogin	f	4755	0	0	-	-	-	-	-

# User-mode Linux stuff
/dev/ubda	b	640	0	0	98	0	0	0	-
/dev/ubda	b	640	0	0	98	1	1	1	15

# IDE Devices
/dev/hda	b	640	0	0	3	0	0	0	-
/dev/hda	b	640	0	0	3	1	1	1	15
/dev/hdb	b	640	0	0	3	64	0	0	-
/dev/hdb	b	640	0	0	3	65	1	1	15
#/dev/hdc	b	640	0	0	22	0	0	0	-
#/dev/hdc	b	640	0	0	22	1	1	1	15
#/dev/hdd	b	640	0	0	22	64	0	0	-
#/dev/hdd	b	640	0	0	22	65	1	1	15
#/dev/hde	b	640	0	0	33	0	0	0	-
#/dev/hde	b	640	0	0	33	1	1	1	15
#/dev/hdf	b	640	0	0	33	64	0	0	-
#/dev/hdf	b	640	0	0	33	65	1	1	15
#/dev/hdg	b	640	0	0	34	0	0	0	-
#/dev/hdg	b	640	0	0	34	1	1	1	15
#/dev/hdh	b	640	0	0	34	64	0	0	-
#/dev/hdh	b	640	0	0	34	65	1	1	15

# SCSI Devices
#/dev/sda	b	640	0	0	8	0	0	0	-
#/dev/sda	b	640	0	0	8	1	1	1	15
#/dev/sdb	b	640	0	0	8	16	0	0	-
#/dev/sdb	b	640	0	0	8	17	1	1	15
#/dev/sdc	b	640	0	0	8	32	0	0	-
#/dev/sdc	b	640	0	0	8	33	1	1	15
#/dev/sdd	b	640	0	0	8	48	0	0	-
#/dev/sdd	b	640	0	0	8	49	1	1	15
#/dev/sde	b	640	0	0	8	64	0	0	-
#/dev/sde	b	640	0	0	8	65	1	1	15
#/dev/sdf	b	640	0	0	8	80	0	0	-
#/dev/sdf	b	640	0	0	8	81	1	1	15
#/dev/sdg	b	640	0	0	8	96	0	0	-
#/dev/sdg	b	640	0	0	8	97	1	1	15
#/dev/sdh	b	640	0	0	8	112	0	0	-
#/dev/sdh	b	640	0	0	8	113	1	1	15
#/dev/sg		c	640	0	0	21	0	0	1	15
#/dev/scd	b	640	0	0	11	0	0	1	15
#/dev/st		c	640	0	0	9	0	0	1	8
#/dev/nst	c	640	0	0	9	128	0	1	8
#/dev/st	c	640	0	0	9	32	1	1	4
#/dev/st	c	640	0	0	9	64	1	1	4
#/dev/st	c	640	0	0	9	96	1	1	4

# Floppy disk devices
#/dev/fd		b	640	0	0	2	0	0	1	2
#/dev/fd0d360	b	640	0	0	2	4	0	0	-
#/dev/fd1d360	b	640	0	0	2	5	0	0	-
#/dev/fd0h1200	b	640	0	0	2	8	0	0	-
#/dev/fd1h1200	b	640	0	0	2	9	0	0	-
#/dev/fd0u1440	b	640	0	0	2	28	0	0	-
#/dev/fd1u1440	b	640	0	0	2	29	0	0	-
#/dev/fd0u2880	b	640	0	0	2	32	0	0	-
#/dev/fd1u2880	b	640	0	0	2	33	0	0	-

# All the proprietary cdrom devices in the world
#/dev/aztcd	b	640	0	0	29	0	0	0	-
#/dev/bpcd	b	640	0	0	41	0	0	0	-
#/dev/capi20	c	640	0	0	68	0	0	1	2
#/dev/cdu31a	b	640	0	0	15	0	0	0	-
#/dev/cdu535	b	640	0	0	24	0	0	0	-
#/dev/cm206cd	b	640	0	0	32	0	0	0	-
#/dev/sjcd	b	640	0	0	18	0	0	0	-
#/dev/sonycd	b	640	0	0	15	0	0	0	-
#/dev/gscd	b	640	0	0	16	0	0	0	-
#/dev/sbpcd	b	640	0	0	25	0	0	0	-
#/dev/sbpcd	b	640	0	0	25	0	0	1	4
#/dev/mcd	b	640	0	0	23	0	0	0	-
#/dev/optcd	b	640	0	0	17	0	0	0	-
        secret_id: ${{ /*
 *  dumpjffs2.c
 *
 *  Copyright (C) 2003  
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Overview:
 *   This utility dumps the contents of a binary JFFS2 image
 *
 *
 * Bug/ToDo:
 */

#define PROGRAM_NAME "jffs2dump"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <asm/types.h>
#include <dirent.h>
#include <mtd/jffs2-user.h>
#include <endian.h>
#include <byteswap.h>
#include <getopt.h>
#include <crc32.h>
#include "summary.h"
#include "common.h"

#define PAD(x) (((x)+3)&~3)

/* For outputting a byte-swapped version of the input image. */
#define cnv_e32(x) ((jint32_t){bswap_32(x.v32)})
#define cnv_e16(x) ((jint16_t){bswap_16(x.v16)})

#define t32_backwards(x) ({ uint32_t __b = (x); (target_endian==__BYTE_ORDER)?bswap_32(__b):__b; })
#define cpu_to_e32(x) ((jint32_t){t32_backwards(x)})

// Global variables
long	imglen;		// length of image
char	*data;		// image data

static void display_help (int error)
{
	printf("Usage: %s [OPTION]... INPUTFILE\n"
	       "Dump the contents of a binary JFFS2 image.\n\n"
	       " -h, --help                   display this help and exit\n"
	       " -V, --version                display version information and exit\n"
	       " -b, --bigendian              image is big endian\n"
	       " -l, --littleendian           image is little endian\n"
	       " -c, --content                dump image contents\n"
	       " -e, --endianconvert=FNAME    convert image endianness, output to file fname\n"
	       " -r, --recalccrc              recalc name and data crc on endian conversion\n"
	       " -d, --datsize=LEN            size of data chunks, when oob data in binary image (NAND only)\n"
	       " -o, --oobsize=LEN            size of oob data chunk in binary image (NAND only)\n"
	       " -v, --verbose                verbose output\n",
	       PROGRAM_NAME);
	exit(error ? EXIT_FAILURE : EXIT_SUCCESS);
}

static void display_version (void)
{
	common_print_version();
	printf("Copyright (C) 2003 Thomas Gleixner \n"
			"\n"
			"%1$s comes with NO WARRANTY\n"
			"to the extent permitted by law.\n"
			"\n"
			"You may redistribute copies of %1$s\n"
			"under the terms of the GNU General Public Licence.\n"
			"See the file `COPYING' for more information.\n",
			PROGRAM_NAME);
	exit(0);
}

// Option variables

int 	verbose;		// verbose output
char 	*img;			// filename of image
int	dumpcontent;		// dump image content
int	target_endian = __BYTE_ORDER;	// image endianess
int	convertendian;		// convert endianness
int	recalccrc;		// recalc name and data crc's on endian conversion
char	cnvfile[256];		// filename for conversion output
int	datsize;		// Size of data chunks, when oob data is inside the binary image
int	oobsize;		// Size of oob chunks, when oob data is inside the binary image

static void process_options (int argc, char *argv[])
{
	int error = 0;

	for (;;) {
		int option_index = 0;
		static const char *short_options = "blce:rd:o:vVh";
		static const struct option long_options[] = {
			{"help", no_argument, 0, 'h'},
			{"version", no_argument, 0, 'V'},
			{"bigendian", no_argument, 0, 'b'},
			{"littleendian", no_argument, 0, 'l'},
			{"content", no_argument, 0, 'c'},
			{"endianconvert", required_argument, 0, 'e'},
			{"datsize", required_argument, 0, 'd'},
			{"oobsize", required_argument, 0, 'o'},
			{"recalccrc", required_argument, 0, 'r'},
			{"verbose", no_argument, 0, 'v'},
			{0, 0, 0, 0},
		};

		int c = getopt_long(argc, argv, short_options,
				long_options, &option_index);
		if (c == EOF) {
			break;
		}

		switch (c) {
			case 'h':
				display_help(0);
				break;
			case 'V':
				display_version();
				break;
			case 'v':
				verbose = 1;
				break;
			case 'b':
				target_endian = __BIG_ENDIAN;
				break;
			case 'l':
				target_endian = __LITTLE_ENDIAN;
				break;
			case 'c':
				dumpcontent = 1;
				break;
			case 'd':
				datsize = atoi(optarg);
				break;
			case 'o':
				oobsize = atoi(optarg);
				break;
			case 'e':
				convertendian = 1;
				strncpy (cnvfile, optarg, sizeof(cnvfile) - 1);
				cnvfile[sizeof(cnvfile) - 1] = '\0';
				break;
			case 'r':
				recalccrc = 1;
				break;
			case '?':
				error = 1;
				break;
		}
	}

	if ((argc - optind) != 1 || error)
		display_help (error);

	img = argv[optind];
}


/*
 *	Dump image contents
 */
static void do_dumpcontent (void)
{
	char			*p = data, *p_free_begin;
	union jffs2_node_union 	*node;
	int			empty = 0, dirty = 0;
	char			name[256];
	uint32_t		crc;
	uint16_t		type;
	int			bitchbitmask = 0;
	int			obsolete;

	p_free_begin = NULL;
	while ( p < (data + imglen)) {
		node = (union jffs2_node_union*) p;

		/* Skip empty space */
		if (!p_free_begin)
			p_free_begin = p;
		if (je16_to_cpu (node->u.magic) == 0xFFFF && je16_to_cpu (node->u.nodetype) == 0xFFFF) {
			p += 4;
			empty += 4;
			continue;
		}

		if (p != p_free_begin)
			printf("Empty space found from 0x%08zx to 0x%08zx\n", p_free_begin-data, p-data);
		p_free_begin = NULL;

		if (je16_to_cpu (node->u.magic) != JFFS2_MAGIC_BITMASK)	{
			if (!bitchbitmask++)
				printf ("Wrong bitmask  at  0x%08zx, 0x%04x\n", p - data, je16_to_cpu (node->u.magic));
			p += 4;
			dirty += 4;
			continue;
		}
		bitchbitmask = 0;

		type = je16_to_cpu(node->u.nodetype);
		if ((type & JFFS2_NODE_ACCURATE) != JFFS2_NODE_ACCURATE) {
			obsolete = 1;
			type |= JFFS2_NODE_ACCURATE;
		} else
			obsolete = 0;
		/* Set accurate for CRC check */
		node->u.nodetype = cpu_to_je16(type);

		crc = mtd_crc32 (0, node, sizeof (struct jffs2_unknown_node) - 4);
		if (crc != je32_to_cpu (node->u.hdr_crc)) {
			printf ("Wrong hdr_crc  at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->u.hdr_crc), crc);
			p += 4;
			dirty += 4;
			continue;
		}

		switch(je16_to_cpu(node->u.nodetype)) {

			case JFFS2_NODETYPE_INODE:
				printf ("%8s Inode      node at 0x%08zx, totlen 0x%08x, #ino  %5d, version %5d, isize %8d, csize %8d, dsize %8d, offset %8d\n",
						obsolete ? "Obsolete" : "",
						p - data, je32_to_cpu (node->i.totlen), je32_to_cpu (node->i.ino),
						je32_to_cpu ( node->i.version), je32_to_cpu (node->i.isize),
						je32_to_cpu (node->i.csize), je32_to_cpu (node->i.dsize), je32_to_cpu (node->i.offset));

				crc = mtd_crc32 (0, node, sizeof (struct jffs2_raw_inode) - 8);
				if (crc != je32_to_cpu (node->i.node_crc)) {
					printf ("Wrong node_crc at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->i.node_crc), crc);
					p += PAD(je32_to_cpu (node->i.totlen));
					dirty += PAD(je32_to_cpu (node->i.totlen));;
					continue;
				}

				crc = mtd_crc32(0, p + sizeof (struct jffs2_raw_inode), je32_to_cpu(node->i.csize));
				if (crc != je32_to_cpu(node->i.data_crc)) {
					printf ("Wrong data_crc at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->i.data_crc), crc);
					p += PAD(je32_to_cpu (node->i.totlen));
					dirty += PAD(je32_to_cpu (node->i.totlen));;
					continue;
				}

				p += PAD(je32_to_cpu (node->i.totlen));
				break;

			case JFFS2_NODETYPE_DIRENT:
				memcpy (name, node->d.name, node->d.nsize);
				name [node->d.nsize] = 0x0;
				printf ("%8s Dirent     node at 0x%08zx, totlen 0x%08x, #pino %5d, version %5d, #ino  %8d, nsize %8d, name %s\n",
						obsolete ? "Obsolete" : "",
						p - data, je32_to_cpu (node->d.totlen), je32_to_cpu (node->d.pino),
						je32_to_cpu ( node->d.version), je32_to_cpu (node->d.ino),
						node->d.nsize, name);

				crc = mtd_crc32 (0, node, sizeof (struct jffs2_raw_dirent) - 8);
				if (crc != je32_to_cpu (node->d.node_crc)) {
					printf ("Wrong node_crc at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->d.node_crc), crc);
					p += PAD(je32_to_cpu (node->d.totlen));
					dirty += PAD(je32_to_cpu (node->d.totlen));;
					continue;
				}

				crc = mtd_crc32(0, p + sizeof (struct jffs2_raw_dirent), node->d.nsize);
				if (crc != je32_to_cpu(node->d.name_crc)) {
					printf ("Wrong name_crc at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->d.name_crc), crc);
					p += PAD(je32_to_cpu (node->d.totlen));
					dirty += PAD(je32_to_cpu (node->d.totlen));;
					continue;
				}

				p += PAD(je32_to_cpu (node->d.totlen));
				break;

			case JFFS2_NODETYPE_XATTR:
				memcpy(name, node->x.data, node->x.name_len);
				name[node->x.name_len] = '\x00';
				printf ("%8s Xattr      node at 0x%08zx, totlen 0x%08x, xid   %5d, version %5d, name_len   %3d, name %s\n",
						obsolete ? "Obsolete" : "",
						p - data,
						je32_to_cpu (node->x.totlen),
						je32_to_cpu (node->x.xid),
						je32_to_cpu (node->x.version),
						node->x.name_len,
						name);

				crc = mtd_crc32 (0, node, sizeof (struct jffs2_raw_xattr) - sizeof (node->x.node_crc));
				if (crc != je32_to_cpu (node->x.node_crc)) {
					printf ("Wrong node_crc at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->x.node_crc), crc);
					p += PAD(je32_to_cpu (node->x.totlen));
					dirty += PAD(je32_to_cpu (node->x.totlen));
					continue;
				}

				crc = mtd_crc32 (0, p + sizeof (struct jffs2_raw_xattr), node->x.name_len + je16_to_cpu (node->x.value_len) + 1);
				if (crc != je32_to_cpu (node->x.data_crc)) {
					printf ("Wrong data_crc at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->x.data_crc), crc);
					p += PAD(je32_to_cpu (node->x.totlen));
					dirty += PAD(je32_to_cpu (node->x.totlen));
					continue;
				}
				p += PAD(je32_to_cpu (node->x.totlen));
				break;

			case JFFS2_NODETYPE_XREF:
				printf ("%8s Xref       node at 0x%08zx, totlen 0x%08x, xid   %5d, xseqno  %5d, #ino  %8d\n",
						obsolete ? "Obsolete" : "",
						p - data,
						je32_to_cpu (node->r.totlen),
						je32_to_cpu (node->r.xid),
						je32_to_cpu (node->r.xseqno),
						je32_to_cpu (node->r.ino));
				p += PAD(je32_to_cpu (node->r.totlen));
				break;

			case JFFS2_NODETYPE_SUMMARY: {

											 int i;
											 struct jffs2_sum_marker * sm;

											 printf("%8s Inode Sum  node at 0x%08zx, totlen 0x%08x, sum_num  %5d, cleanmarker size %5d\n",
													 obsolete ? "Obsolete" : "",
													 p - data,
													 je32_to_cpu (node->s.totlen),
													 je32_to_cpu (node->s.sum_num),
													 je32_to_cpu (node->s.cln_mkr));

											 crc = mtd_crc32 (0, node, sizeof (struct jffs2_raw_summary) - 8);
											 if (crc != je32_to_cpu (node->s.node_crc)) {
												 printf ("Wrong node_crc at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->s.node_crc), crc);
												 p += PAD(je32_to_cpu (node->s.totlen));
												 dirty += PAD(je32_to_cpu (node->s.totlen));;
												 continue;
											 }

											 crc = mtd_crc32(0, p + sizeof (struct jffs2_raw_summary),  je32_to_cpu (node->s.totlen) - sizeof(struct jffs2_raw_summary));
											 if (crc != je32_to_cpu(node->s.sum_crc)) {
												 printf ("Wrong data_crc at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->s.sum_crc), crc);
												 p += PAD(je32_to_cpu (node->s.totlen));
												 dirty += PAD(je32_to_cpu (node->s.totlen));;
												 continue;
											 }

											 if (verbose) {
												 void *sp;
												 sp = (p + sizeof(struct jffs2_raw_summary));

												 for(i=0; i<je32_to_cpu(node->s.sum_num); i++) {

													 switch(je16_to_cpu(((struct jffs2_sum_unknown_flash *)sp)->nodetype)) {
														 case JFFS2_NODETYPE_INODE : {

																						 struct jffs2_sum_inode_flash *spi;
																						 spi = sp;

																						 printf ("%14s #ino  %5d,  version %5d, offset 0x%08x, totlen 0x%08x\n",
																								 "",
																								 je32_to_cpu (spi->inode),
																								 je32_to_cpu (spi->version),
																								 je32_to_cpu (spi->offset),
																								 je32_to_cpu (spi->totlen));

																						 sp += JFFS2_SUMMARY_INODE_SIZE;
																						 break;
																					 }

														 case JFFS2_NODETYPE_DIRENT : {

																						  char name[255];
																						  struct jffs2_sum_dirent_flash *spd;
																						  spd = sp;

																						  memcpy(name,spd->name,spd->nsize);
																						  name [spd->nsize] = 0x0;

																						  printf ("%14s dirent offset 0x%08x, totlen 0x%08x, #pino  %5d,  version %5d, #ino  %8d, nsize %8d, name %s \n",
																								  "",
																								  je32_to_cpu (spd->offset),
																								  je32_to_cpu (spd->totlen),
																								  je32_to_cpu (spd->pino),
																								  je32_to_cpu (spd->version),
																								  je32_to_cpu (spd->ino),
																								  spd->nsize,
																								  name);

																						  sp += JFFS2_SUMMARY_DIRENT_SIZE(spd->nsize);
																						  break;
																					  }

														 case JFFS2_NODETYPE_XATTR : {
																						  struct jffs2_sum_xattr_flash *spx;
																						  spx = sp;
																						  printf ("%14s Xattr  offset 0x%08x, totlen 0x%08x, version %5d, #xid %8d\n",
																								  "",
																								  je32_to_cpu (spx->offset),
																								  je32_to_cpu (spx->totlen),
																								  je32_to_cpu (spx->version),
																								  je32_to_cpu (spx->xid));
																						  sp += JFFS2_SUMMARY_XATTR_SIZE;
																						  break;
																					  }

														 case JFFS2_NODETYPE_XREF : {
																						  struct jffs2_sum_xref_flash *spr;
																						  spr = sp;
																						  printf ("%14s Xref   offset 0x%08x\n",
																								  "",
																								  je32_to_cpu (spr->offset));
																						  sp += JFFS2_SUMMARY_XREF_SIZE;
																						  break;
																					  }

														 default :
																					  printf("Unknown summary node!\n");
																					  break;
													 }
												 }

												 sm = (struct jffs2_sum_marker *) ((char *)p + je32_to_cpu(node->s.totlen) - sizeof(struct jffs2_sum_marker));

												 printf("%14s Sum Node Offset  0x%08x, Magic 0x%08x, Padded size 0x%08x\n",
														 "",
														 je32_to_cpu(sm->offset),
														 je32_to_cpu(sm->magic),
														 je32_to_cpu(node->s.padded));
											 }

											 p += PAD(je32_to_cpu (node->s.totlen));
											 break;
										 }

			case JFFS2_NODETYPE_CLEANMARKER:
										 if (verbose) {
											 printf ("%8s Cleanmarker     at 0x%08zx, totlen 0x%08x\n",
													 obsolete ? "Obsolete" : "",
													 p - data, je32_to_cpu (node->u.totlen));
										 }
										 p += PAD(je32_to_cpu (node->u.totlen));
										 break;

			case JFFS2_NODETYPE_PADDING:
										 if (verbose) {
											 printf ("%8s Padding    node at 0x%08zx, totlen 0x%08x\n",
													 obsolete ? "Obsolete" : "",
													 p - data, je32_to_cpu (node->u.totlen));
										 }
										 p += PAD(je32_to_cpu (node->u.totlen));
										 break;

			case 0xffff:
										 p += 4;
										 empty += 4;
										 break;

			default:
										 if (verbose) {
											 printf ("%8s Unknown    node at 0x%08zx, totlen 0x%08x\n",
													 obsolete ? "Obsolete" : "",
													 p - data, je32_to_cpu (node->u.totlen));
										 }
										 p += PAD(je32_to_cpu (node->u.totlen));
										 dirty += PAD(je32_to_cpu (node->u.totlen));

		}
	}

	if (verbose)
		printf ("Empty space: %d, dirty space: %d\n", empty, dirty);
}

/*
 *	Convert endianess
 */
static void do_endianconvert (void)
{
	char			*p = data;
	union jffs2_node_union 	*node, newnode;
	int			fd, len;
	jint32_t		mode;
	uint32_t		crc;

	fd = open (cnvfile, O_WRONLY | O_CREAT, 0644);
	if (fd < 0) {
		fprintf (stderr, "Cannot open / create file: %s\n", cnvfile);
		return;
	}

	while ( p < (data + imglen)) {
		node = (union jffs2_node_union*) p;

		/* Skip empty space */
		if (je16_to_cpu (node->u.magic) == 0xFFFF && je16_to_cpu (node->u.nodetype) == 0xFFFF) {
			write_nocheck (fd, p, 4);
			p += 4;
			continue;
		}

		if (je16_to_cpu (node->u.magic) != JFFS2_MAGIC_BITMASK)	{
			printf ("Wrong bitmask  at  0x%08zx, 0x%04x\n", p - data, je16_to_cpu (node->u.magic));
			newnode.u.magic = cnv_e16 (node->u.magic);
			newnode.u.nodetype = cnv_e16 (node->u.nodetype);
			write_nocheck (fd, &newnode, 4);
			p += 4;
			continue;
		}

		crc = mtd_crc32 (0, node, sizeof (struct jffs2_unknown_node) - 4);
		if (crc != je32_to_cpu (node->u.hdr_crc)) {
			printf ("Wrong hdr_crc  at  0x%08zx, 0x%08x instead of 0x%08x\n", p - data, je32_to_cpu (node->u.hdr_crc), crc);
		}

		switch(je16_to_cpu(node->u.nodetype)) {

			case JFFS2_NODETYPE_INODE:

				newnode.i.magic = cnv_e16 (node->i.magic);
				newnode.i.nodetype = cnv_e16 (node->i.nodetype);
				newnode.i.totlen = cnv_e32 (node->i.totlen);
				newnode.i.hdr_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_unknown_node) - 4));
				newnode.i.ino = cnv_e32 (node->i.ino);
				newnode.i.version = cnv_e32 (node->i.version);
				mode.v32 = node->i.mode.m;
				mode = cnv_e32 (mode);
				newnode.i.mode.m = mode.v32;
				newnode.i.uid = cnv_e16 (node->i.uid);
				newnode.i.gid = cnv_e16 (node->i.gid);
				newnode.i.isize = cnv_e32 (node->i.isize);
				newnode.i.atime = cnv_e32 (node->i.atime);
				newnode.i.mtime = cnv_e32 (node->i.mtime);
				newnode.i.ctime = cnv_e32 (node->i.ctime);
				newnode.i.offset = cnv_e32 (node->i.offset);
				newnode.i.csize = cnv_e32 (node->i.csize);
				newnode.i.dsize = cnv_e32 (node->i.dsize);
				newnode.i.compr = node->i.compr;
				newnode.i.usercompr = node->i.usercompr;
				newnode.i.flags = cnv_e16 (node->i.flags);
				if (recalccrc) {
					len = je32_to_cpu(node->i.csize);
					newnode.i.data_crc = cpu_to_e32 ( mtd_crc32(0, p + sizeof (struct jffs2_raw_inode), len));
				} else
					newnode.i.data_crc = cnv_e32 (node->i.data_crc);

				newnode.i.node_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_raw_inode) - 8));

				write_nocheck (fd, &newnode, sizeof (struct jffs2_raw_inode));
				write_nocheck (fd, p + sizeof (struct jffs2_raw_inode), PAD (je32_to_cpu (node->i.totlen) -  sizeof (struct jffs2_raw_inode)));

				p += PAD(je32_to_cpu (node->i.totlen));
				break;

			case JFFS2_NODETYPE_DIRENT:
				newnode.d.magic = cnv_e16 (node->d.magic);
				newnode.d.nodetype = cnv_e16 (node->d.nodetype);
				newnode.d.totlen = cnv_e32 (node->d.totlen);
				newnode.d.hdr_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_unknown_node) - 4));
				newnode.d.pino = cnv_e32 (node->d.pino);
				newnode.d.version = cnv_e32 (node->d.version);
				newnode.d.ino = cnv_e32 (node->d.ino);
				newnode.d.mctime = cnv_e32 (node->d.mctime);
				newnode.d.nsize = node->d.nsize;
				newnode.d.type = node->d.type;
				newnode.d.unused[0] = node->d.unused[0];
				newnode.d.unused[1] = node->d.unused[1];
				newnode.d.node_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_raw_dirent) - 8));
				if (recalccrc)
					newnode.d.name_crc = cpu_to_e32 ( mtd_crc32(0, p + sizeof (struct jffs2_raw_dirent), node->d.nsize));
				else
					newnode.d.name_crc = cnv_e32 (node->d.name_crc);

				write_nocheck (fd, &newnode, sizeof (struct jffs2_raw_dirent));
				write_nocheck (fd, p + sizeof (struct jffs2_raw_dirent), PAD (je32_to_cpu (node->d.totlen) -  sizeof (struct jffs2_raw_dirent)));
				p += PAD(je32_to_cpu (node->d.totlen));
				break;

			case JFFS2_NODETYPE_XATTR:
				newnode.x.magic = cnv_e16 (node->x.magic);
				newnode.x.nodetype = cnv_e16 (node->x.nodetype);
				newnode.x.totlen = cnv_e32 (node->x.totlen);
				newnode.x.hdr_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_unknown_node) - 4));
				newnode.x.xid = cnv_e32 (node->x.xid);
				newnode.x.version = cnv_e32 (node->x.version);
				newnode.x.xprefix = node->x.xprefix;
				newnode.x.name_len = node->x.name_len;
				newnode.x.value_len = cnv_e16 (node->x.value_len);
				if (recalccrc)
					newnode.x.data_crc = cpu_to_e32 (mtd_crc32 (0, p + sizeof (struct jffs2_raw_xattr), node->x.name_len + je16_to_cpu (node->x.value_len) + 1));
				else
					newnode.x.data_crc = cnv_e32 (node->x.data_crc);
				newnode.x.node_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_raw_xattr) - sizeof (newnode.x.node_crc)));

				write_nocheck (fd, &newnode, sizeof (struct jffs2_raw_xattr));
				write_nocheck (fd, p + sizeof (struct jffs2_raw_xattr), PAD (je32_to_cpu (node->d.totlen) -  sizeof (struct jffs2_raw_xattr)));
				p += PAD(je32_to_cpu (node->x.totlen));
				break;

			case JFFS2_NODETYPE_XREF:
				newnode.r.magic = cnv_e16 (node->r.magic);
				newnode.r.nodetype = cnv_e16 (node->r.nodetype);
				newnode.r.totlen = cnv_e32 (node->r.totlen);
				newnode.r.hdr_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_unknown_node) - sizeof (newnode.r.hdr_crc)));
				newnode.r.ino = cnv_e32 (node->r.ino);
				newnode.r.xid = cnv_e32 (node->r.xid);
				newnode.r.xseqno = cnv_e32 (node->r.xseqno);
				newnode.r.node_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_raw_xref) - sizeof (newnode.r.node_crc)));
				p += PAD(je32_to_cpu (node->x.totlen));
				break;

			case JFFS2_NODETYPE_CLEANMARKER:
			case JFFS2_NODETYPE_PADDING:
				newnode.u.magic = cnv_e16 (node->u.magic);
				newnode.u.nodetype = cnv_e16 (node->u.nodetype);
				newnode.u.totlen = cnv_e32 (node->u.totlen);
				newnode.u.hdr_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_unknown_node) - 4));

				write_nocheck (fd, &newnode, sizeof (struct jffs2_unknown_node));
				len = PAD(je32_to_cpu (node->u.totlen) - sizeof (struct jffs2_unknown_node));
				if (len > 0)
					write_nocheck (fd, p + sizeof (struct jffs2_unknown_node), len);

				p += PAD(je32_to_cpu (node->u.totlen));
				break;

			case JFFS2_NODETYPE_SUMMARY : {
											  struct jffs2_sum_marker *sm_ptr;
											  int i,sum_len;
											  int counter = 0;

											  newnode.s.magic = cnv_e16 (node->s.magic);
											  newnode.s.nodetype = cnv_e16 (node->s.nodetype);
											  newnode.s.totlen = cnv_e32 (node->s.totlen);
											  newnode.s.hdr_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_unknown_node) - 4));
											  newnode.s.sum_num = cnv_e32 (node->s.sum_num);
											  newnode.s.cln_mkr = cnv_e32 (node->s.cln_mkr);
											  newnode.s.padded = cnv_e32 (node->s.padded);

											  newnode.s.node_crc = cpu_to_e32 (mtd_crc32 (0, &newnode, sizeof (struct jffs2_raw_summary) - 8));

											  // summary header
											  p += sizeof (struct jffs2_raw_summary);

											  // summary data
											  sum_len = je32_to_cpu (node->s.totlen) - sizeof (struct jffs2_raw_summary) - sizeof (struct jffs2_sum_marker);

											  for (i=0; i<je32_to_cpu (node->s.sum_num); i++) {
												  union jffs2_sum_flash *fl_ptr;

												  fl_ptr = (union jffs2_sum_flash *) p;

												  switch (je16_to_cpu (fl_ptr->u.nodetype)) {
													  case JFFS2_NODETYPE_INODE:

														  fl_ptr->i.nodetype = cnv_e16 (fl_ptr->i.nodetype);
														  fl_ptr->i.inode = cnv_e32 (fl_ptr->i.inode);
														  fl_ptr->i.version = cnv_e32 (fl_ptr->i.version);
														  fl_ptr->i.offset = cnv_e32 (fl_ptr->i.offset);
														  fl_ptr->i.totlen = cnv_e32 (fl_ptr->i.totlen);
														  p += sizeof (struct jffs2_sum_inode_flash);
														  counter += sizeof (struct jffs2_sum_inode_flash);
														  break;

													  case JFFS2_NODETYPE_DIRENT:
														  fl_ptr->d.nodetype = cnv_e16 (fl_ptr->d.nodetype);
														  fl_ptr->d.totlen = cnv_e32 (fl_ptr->d.totlen);
														  fl_ptr->d.offset = cnv_e32 (fl_ptr->d.offset);
														  fl_ptr->d.pino = cnv_e32 (fl_ptr->d.pino);
														  fl_ptr->d.version = cnv_e32 (fl_ptr->d.version);
														  fl_ptr->d.ino = cnv_e32 (fl_ptr->d.ino);
														  p += sizeof (struct jffs2_sum_dirent_flash) + fl_ptr->d.nsize;
														  counter += sizeof (struct jffs2_sum_dirent_flash) + fl_ptr->d.nsize;
														  break;

													  case JFFS2_NODETYPE_XATTR:
														  fl_ptr->x.nodetype = cnv_e16 (fl_ptr->x.nodetype);
														  fl_ptr->x.xid = cnv_e32 (fl_ptr->x.xid);
														  fl_ptr->x.version = cnv_e32 (fl_ptr->x.version);
														  fl_ptr->x.offset = cnv_e32 (fl_ptr->x.offset);
														  fl_ptr->x.totlen = cnv_e32 (fl_ptr->x.totlen);
														  p += sizeof (struct jffs2_sum_xattr_flash);
														  counter += sizeof (struct jffs2_sum_xattr_flash);
														  break;

													  case JFFS2_NODETYPE_XREF:
														  fl_ptr->r.nodetype = cnv_e16 (fl_ptr->r.nodetype);
														  fl_ptr->r.offset = cnv_e32 (fl_ptr->r.offset);
														  p += sizeof (struct jffs2_sum_xref_flash);
														  counter += sizeof (struct jffs2_sum_xref_flash);
														  break;

													  default :
														  printf("Unknown node in summary information!!! nodetype(%x)\n", je16_to_cpu (fl_ptr->u.nodetype));
														  exit(EXIT_FAILURE);
														  break;
												  }

											  }

											  //pad
											  p += sum_len - counter;

											  // summary marker
											  sm_ptr = (struct jffs2_sum_marker *) p;
											  sm_ptr->offset = cnv_e32 (sm_ptr->offset);
											  sm_ptr->magic = cnv_e32 (sm_ptr->magic);
											  p += sizeof (struct jffs2_sum_marker);

											  // generate new crc on sum data
											  newnode.s.sum_crc = cpu_to_e32 ( mtd_crc32(0, ((char *) node) + sizeof (struct jffs2_raw_summary),
														  je32_to_cpu (node->s.totlen) - sizeof (struct jffs2_raw_summary)));

											  // write out new node header
											  write_nocheck(fd, &newnode, sizeof (struct jffs2_raw_summary));
											  // write out new summary data
											  write_nocheck(fd, &node->s.sum, sum_len + sizeof (struct jffs2_sum_marker));

											  break;
										  }

			case 0xffff:
										  write_nocheck (fd, p, 4);
										  p += 4;
										  break;

			default:
										  printf ("Unknown node type: 0x%04x at 0x%08zx, totlen 0x%08x\n", je16_to_cpu (node->u.nodetype), p - data, je32_to_cpu (node->u.totlen));
										  p += PAD(je32_to_cpu (node->u.totlen));

		}
	}

	close (fd);

}

/*
 * Main program
 */
int main(int argc, char **argv)
{
	int fd;

	process_options(argc, argv);

	/* Open the input file */
	if ((fd = open(img, O_RDONLY)) == -1) {
		perror("open input file");
		exit(EXIT_FAILURE);
	}

	// get image length
	imglen = lseek(fd, 0, SEEK_END);
	if (imglen < 0) {
		perror(img);
		close(fd);
		exit(EXIT_FAILURE);
	}

	lseek (fd, 0, SEEK_SET);

	data = malloc (imglen);
	if (!data) {
		perror("out of memory");
		close (fd);
		exit(EXIT_FAILURE);
	}

	if (datsize && oobsize) {
		int  idx = 0;
		long len = imglen;
		uint8_t oob[oobsize];
		printf ("Peeling data out of combined data/oob image\n");
		while (len) {
			// read image data
			read_nocheck (fd, &data[idx], datsize);
			read_nocheck (fd, oob, oobsize);
			idx += datsize;
			imglen -= oobsize;
			len -= datsize + oobsize;
		}

	} else {
		// read image data
		read_nocheck (fd, data, imglen);
	}
	// Close the input file
	close(fd);

	if (dumpcontent)
		do_dumpcontent ();

	if (convertendian)
		do_endianconvert ();

	// free memory
	free (data);

	// Return happy
	exit (EXIT_SUCCESS);
} }}
        secret_key: ${{ /* vi: set sw=4 ts=4: */
/*
 * jffs2reader v0.0.18 A jffs2 image reader
 *
 * Copyright (c) 2001 
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the author be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 * not claim that you wrote the original software. If you use this
 * software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must
 * not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 *
 *********
 *  This code was altered September 2001
 *  Changes are Copyright (c) >
 *
 * In compliance with (2) above, this is hereby marked as an altered
 * version of this software.  It has been altered as follows:
 *      *) Listing a directory now mimics the behavior of 'ls -l'
 *      *) Support for recursive listing has been added
 *      *) Without options, does a recursive 'ls' on the whole filesystem
 *      *) option parsing now uses getopt()
 *      *) Now uses printf, and error messages go to stderr.
 *      *) The copyright notice has been cleaned up and reformatted
 *      *) The code has been reformatted
 *      *) Several twisty code paths have been fixed so I can understand them.
 *  -Erik, 1 September 2001
 *
 *      *) Made it show major/minor numbers for device nodes
 *      *) Made it show symlink targets
 *  -, 13 September 2001
 */


/*
TODO:

- Add CRC checking code to places marked with XXX.
- Add support for other node compression types.

- Test with real life images.
- Maybe port into bootloader.
 */

/*
BUGS:

- Doesn't check CRC checksums.
 */

#define PROGRAM_NAME "jffs2reader"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#ifdef WITH_ZLIB
#include <zlib.h>
#else
typedef unsigned long uLongf;
#endif

#include "mtd/jffs2-user.h"
#include "common.h"

static struct option long_opt[] = {
	{"help", 0, NULL, 'h'},
	{"version", 0, NULL, 'V'},
	{NULL, 0, NULL, 0},
};

static const char *short_opt = "rd:f:tVh";

#define SCRATCH_SIZE (5*1024*1024)

/* macro to avoid "lvalue required as left operand of assignment" error */
#define ADD_BYTES(p, n)		((p) = (typeof(p))((char *)(p) + (n)))

#define DIRENT_INO(dirent) ((dirent) !=NULL ? je32_to_cpu((dirent)->ino) : 0)
#define DIRENT_PINO(dirent) ((dirent) !=NULL ? je32_to_cpu((dirent)->pino) : 0)

struct dir {
	struct dir *next;
	uint8_t type;
	uint8_t nsize;
	uint32_t ino;
	char name[256];
};

int target_endian = __BYTE_ORDER;

static struct jffs2_raw_inode *find_raw_inode(char *, size_t, uint32_t);

static void lsdir(char *, size_t, const char *, int, int);

/* writes file node into buffer, to the proper position. */
/* reading all valid nodes in version order reconstructs the file. */

/*
   b       - buffer
   bsize   - buffer size
   rsize   - result size
   n       - node
 */

static void putblock(char *b, size_t bsize, size_t * rsize,
		struct jffs2_raw_inode *n)
{
	uLongf dlen = je32_to_cpu(n->dsize);

	if (je32_to_cpu(n->isize) > bsize || (je32_to_cpu(n->offset) + dlen) > bsize)
		errmsg_die("File does not fit into buffer!");

	if (*rsize < je32_to_cpu(n->isize))
		bzero(b + *rsize, je32_to_cpu(n->isize) - *rsize);

	switch (n->compr) {
#ifdef WITH_ZLIB
		case JFFS2_COMPR_ZLIB:
			uncompress((Bytef *) b + je32_to_cpu(n->offset), &dlen,
					(Bytef *) ((char *) n) + sizeof(struct jffs2_raw_inode),
					(uLongf) je32_to_cpu(n->csize));
			break;
#endif
		case JFFS2_COMPR_NONE:
			memcpy(b + je32_to_cpu(n->offset),
					((char *) n) + sizeof(struct jffs2_raw_inode), dlen);
			break;

		case JFFS2_COMPR_ZERO:
			bzero(b + je32_to_cpu(n->offset), dlen);
			break;

			/* [DYN]RUBIN support required! */

		default:
			errmsg_die("Unsupported compression method!");
	}

	*rsize = je32_to_cpu(n->isize);
}

/* adds/removes directory node into dir struct. */
/* reading all valid nodes in version order reconstructs the directory. */

/*
   dd      - directory struct being processed
   n       - node

   return value: directory struct value replacing dd
 */

static struct dir *putdir(struct dir *dd, struct jffs2_raw_dirent *n)
{
	struct dir *o, *d, *p;

	o = dd;

	if (je32_to_cpu(n->ino)) {
		if (dd == NULL) {
			d = xmalloc(sizeof(struct dir));
			d->type = n->type;
			memcpy(d->name, n->name, n->nsize);
			d->nsize = n->nsize;
			d->ino = je32_to_cpu(n->ino);
			d->next = NULL;

			return d;
		}

		while (1) {
			if (n->nsize == dd->nsize &&
					!memcmp(n->name, dd->name, n->nsize)) {
				dd->type = n->type;
				dd->ino = je32_to_cpu(n->ino);

				return o;
			}

			if (dd->next == NULL) {
				dd->next = xmalloc(sizeof(struct dir));
				dd->next->type = n->type;
				memcpy(dd->next->name, n->name, n->nsize);
				dd->next->nsize = n->nsize;
				dd->next->ino = je32_to_cpu(n->ino);
				dd->next->next = NULL;

				return o;
			}

			dd = dd->next;
		}
	} else {
		if (dd == NULL)
			return NULL;

		if (n->nsize == dd->nsize && !memcmp(n->name, dd->name, n->nsize)) {
			d = dd->next;
			free(dd);
			return d;
		}

		while (1) {
			p = dd;
			dd = dd->next;

			if (dd == NULL)
				return o;

			if (n->nsize == dd->nsize &&
					!memcmp(n->name, dd->name, n->nsize)) {
				p->next = dd->next;
				free(dd);

				return o;
			}
		}
	}
}


#define TYPEINDEX(mode) (((mode) >> 12) & 0x0f)
#define TYPECHAR(mode)  ("0pcCd?bB-?l?s???" [TYPEINDEX(mode)])

/* The special bits. If set, display SMODE0/1 instead of MODE0/1 */
static const mode_t SBIT[] = {
	0, 0, S_ISUID,
	0, 0, S_ISGID,
	0, 0, S_ISVTX
};

/* The 9 mode bits to test */
static const mode_t MBIT[] = {
	S_IRUSR, S_IWUSR, S_IXUSR,
	S_IRGRP, S_IWGRP, S_IXGRP,
	S_IROTH, S_IWOTH, S_IXOTH
};

static const char MODE1[] = "rwxrwxrwx";
static const char MODE0[] = "---------";
static const char SMODE1[] = "..s..s..t";
static const char SMODE0[] = "..S..S..T";

/*
 * Return the standard ls-like mode string from a file mode.
 * This is static and so is overwritten on each call.
 */
static const char *mode_string(int mode)
{
	static char buf[12];

	int i;

	buf[0] = TYPECHAR(mode);
	for (i = 0; i < 9; i++) {
		if (mode & SBIT[i])
			buf[i + 1] = (mode & MBIT[i]) ? SMODE1[i] : SMODE0[i];
		else
			buf[i + 1] = (mode & MBIT[i]) ? MODE1[i] : MODE0[i];
	}
	return buf;
}

/* prints contents of directory structure */

/*
   d       - dir struct
 */

static void printdir(char *o, size_t size, struct dir *d, const char *path,
					 int recurse, int want_ctime)
{
	char m;
	char *filetime;
	time_t age;
	struct jffs2_raw_inode *ri;
	jint32_t mode;

	if (!path)
		return;
	if (strlen(path) == 1 && *path == '/')
		path++;

	while (d != NULL) {
		switch (d->type) {
			case DT_REG:
				m = ' ';
				break;

			case DT_FIFO:
				m = '|';
				break;

			case DT_CHR:
				m = ' ';
				break;

			case DT_BLK:
				m = ' ';
				break;

			case DT_DIR:
				m = '/';
				break;

			case DT_LNK:
				m = ' ';
				break;

			case DT_SOCK:
				m = '=';
				break;

			default:
				m = '?';
		}
		ri = find_raw_inode(o, size, d->ino);
		if (!ri) {
			warnmsg("bug: raw_inode missing!");
			d = d->next;
			continue;
		}
		time_t _ctime;
		memcpy(&_ctime, &(ri->ctime), sizeof(time_t));
		filetime = ctime(&_ctime);
		age = time(NULL) - je32_to_cpu(ri->ctime);
		mode.v32 = ri->mode.m;
		printf("%s %-4d %-8d %-8d ", mode_string(je32_to_cpu(mode)),
				1, je16_to_cpu(ri->uid), je16_to_cpu(ri->gid));
		if ( d->type==DT_BLK || d->type==DT_CHR ) {
			dev_t rdev;
			size_t devsize;
			putblock((char*)&rdev, sizeof(rdev), &devsize, ri);
			printf("%4d, %3d ", major(rdev), minor(rdev));
		} else {
			printf("%9ld ", (long)je32_to_cpu(ri->dsize));
		}
		d->name[d->nsize]='\0';
		if (want_ctime) {
			if (age < 3600L * 24 * 365 / 2 && age > -15 * 60)
				/* hh:mm if less than 6 months old */
				printf("%6.6s %5.5s ", filetime + 4, filetime + 11);
			else
				printf("%6.6s %4.4s ", filetime + 4, filetime + 20);
		}
		printf("%s/%s%c", path, d->name, m);
		if (d->type == DT_LNK) {
			char symbuf[1024];
			size_t symsize;
			putblock(symbuf, sizeof(symbuf), &symsize, ri);
			symbuf[symsize] = 0;
			printf(" -> %s", symbuf);
		}
		printf("\n");

		if (d->type == DT_DIR && recurse) {
			char *tmp;
			tmp = xmalloc(BUFSIZ);
			sprintf(tmp, "%s/%s", path, d->name);
			lsdir(o, size, tmp, recurse, want_ctime);	/* Go recursive */
			free(tmp);
		}

		d = d->next;
	}
}

/* frees memory used by directory structure */

/*
   d       - dir struct
 */

static void freedir(struct dir *d)
{
	struct dir *t;

	while (d != NULL) {
		t = d->next;
		free(d);
		d = t;
	}
}

/* collects directory/file nodes in version order. */

/*
   f       - file flag.
   if zero, collect file, compare ino to inode
   otherwise, collect directory, compare ino to parent inode
   o       - filesystem image pointer
   size    - size of filesystem image
   ino     - inode to compare against. see f.

   return value: a jffs2_raw_inode that corresponds the the specified
   inode, or NULL
 */

static struct jffs2_raw_inode *find_raw_inode(char *o, size_t size,
											  uint32_t ino)
{
	/* aligned! */
	union jffs2_node_union *n;
	union jffs2_node_union *e = (union jffs2_node_union *) (o + size);
	union jffs2_node_union *lr;	/* last block position */
	union jffs2_node_union *mp = NULL;	/* minimum position */

	uint32_t vmin, vmint, vmaxt, vmax, vcur, v;

	vmin = 0;					/* next to read */
	vmax = ~((uint32_t) 0);		/* last to read */
	vmint = ~((uint32_t) 0);
	vmaxt = 0;					/* found maximum */
	vcur = 0;					/* XXX what is smallest version number used? */
	/* too low version number can easily result excess log rereading */

	n = (union jffs2_node_union *) o;
	lr = n;

	do {
		while (n < e && je16_to_cpu(n->u.magic) != JFFS2_MAGIC_BITMASK)
			ADD_BYTES(n, 4);

		if (n < e && je16_to_cpu(n->u.magic) == JFFS2_MAGIC_BITMASK) {
			if (je16_to_cpu(n->u.nodetype) == JFFS2_NODETYPE_INODE &&
				je32_to_cpu(n->i.ino) == ino && (v = je32_to_cpu(n->i.version)) > vcur) {
				/* XXX crc check */

				if (vmaxt < v)
					vmaxt = v;
				if (vmint > v) {
					vmint = v;
					mp = n;
				}

				if (v == (vcur + 1))
					return (&(n->i));
			}

			ADD_BYTES(n, ((je32_to_cpu(n->u.totlen) + 3) & ~3));
		} else
			n = (union jffs2_node_union *) o;	/* we're at the end, rewind to the beginning */

		if (lr == n) {			/* whole loop since last read */
			vmax = vmaxt;
			vmin = vmint;
			vmint = ~((uint32_t) 0);

			if (vcur < vmax && vcur < vmin)
				return (&(mp->i));
		}
	} while (vcur < vmax);

	return NULL;
}

/* collects dir struct for selected inode */

/*
   o       - filesystem image pointer
   size    - size of filesystem image
   pino    - inode of the specified directory
   d       - input directory structure

   return value: result directory structure, replaces d.
 */

static struct dir *collectdir(char *o, size_t size, uint32_t ino, struct dir *d)
{
	/* aligned! */
	union jffs2_node_union *n;
	union jffs2_node_union *e = (union jffs2_node_union *) (o + size);
	union jffs2_node_union *lr;	/* last block position */
	union jffs2_node_union *mp = NULL;	/* minimum position */

	uint32_t vmin, vmint, vmaxt, vmax, vcur, v;

	vmin = 0;					/* next to read */
	vmax = ~((uint32_t) 0);		/* last to read */
	vmint = ~((uint32_t) 0);
	vmaxt = 0;					/* found maximum */
	vcur = 0;					/* XXX what is smallest version number used? */
	/* too low version number can easily result excess log rereading */

	n = (union jffs2_node_union *) o;
	lr = n;

	do {
		while (n < e && je16_to_cpu(n->u.magic) != JFFS2_MAGIC_BITMASK)
			ADD_BYTES(n, 4);

		if (n < e && je16_to_cpu(n->u.magic) == JFFS2_MAGIC_BITMASK) {
			if (je16_to_cpu(n->u.nodetype) == JFFS2_NODETYPE_DIRENT &&
				je32_to_cpu(n->d.pino) == ino && (v = je32_to_cpu(n->d.version)) > vcur) {
				/* XXX crc check */

				if (vmaxt < v)
					vmaxt = v;
				if (vmint > v) {
					vmint = v;
					mp = n;
				}

				if (v == (vcur + 1)) {
					d = putdir(d, &(n->d));

					lr = n;
					vcur++;
					vmint = ~((uint32_t) 0);
				}
			}

			ADD_BYTES(n, ((je32_to_cpu(n->u.totlen) + 3) & ~3));
		} else
			n = (union jffs2_node_union *) o;	/* we're at the end, rewind to the beginning */

		if (lr == n) {			/* whole loop since last read */
			vmax = vmaxt;
			vmin = vmint;
			vmint = ~((uint32_t) 0);

			if (vcur < vmax && vcur < vmin) {
				d = putdir(d, &(mp->d));

				lr = n =
					(union jffs2_node_union *) (((char *) mp) +
							((je32_to_cpu(mp->u.totlen) + 3) & ~3));

				vcur = vmin;
			}
		}
	} while (vcur < vmax);

	return d;
}



/* resolve dirent based on criteria */

/*
   o       - filesystem image pointer
   size    - size of filesystem image
   ino     - if zero, ignore,
   otherwise compare against dirent inode
   pino    - if zero, ingore,
   otherwise compare against parent inode
   and use name and nsize as extra criteria
   name    - name of wanted dirent, used if pino!=0
   nsize   - length of name of wanted dirent, used if pino!=0

   return value: pointer to relevant dirent structure in
   filesystem image or NULL
 */

static struct jffs2_raw_dirent *resolvedirent(char *o, size_t size,
		uint32_t ino, uint32_t pino,
		char *name, uint8_t nsize)
{
	/* aligned! */
	union jffs2_node_union *n;
	union jffs2_node_union *e = (union jffs2_node_union *) (o + size);

	struct jffs2_raw_dirent *dd = NULL;

	uint32_t vmax, v;

	if (!pino && ino <= 1)
		return dd;

	vmax = 0;

	n = (union jffs2_node_union *) o;

	do {
		while (n < e && je16_to_cpu(n->u.magic) != JFFS2_MAGIC_BITMASK)
			ADD_BYTES(n, 4);

		if (n < e && je16_to_cpu(n->u.magic) == JFFS2_MAGIC_BITMASK) {
			if (je16_to_cpu(n->u.nodetype) == JFFS2_NODETYPE_DIRENT &&
					(!ino || je32_to_cpu(n->d.ino) == ino) &&
					(v = je32_to_cpu(n->d.version)) > vmax &&
					(!pino || (je32_to_cpu(n->d.pino) == pino &&
							   nsize == n->d.nsize &&
							   !memcmp(name, n->d.name, nsize)))) {
				/* XXX crc check */

				if (vmax < v) {
					vmax = v;
					dd = &(n->d);
				}
			}

			ADD_BYTES(n, ((je32_to_cpu(n->u.totlen) + 3) & ~3));
		} else
			return dd;
	} while (1);
}

/* resolve name under certain parent inode to dirent */

/*
   o       - filesystem image pointer
   size    - size of filesystem image
   pino    - requested parent inode
   name    - name of wanted dirent
   nsize   - length of name of wanted dirent

   return value: pointer to relevant dirent structure in
   filesystem image or NULL
 */

static struct jffs2_raw_dirent *resolvename(char *o, size_t size, uint32_t pino,
		char *name, uint8_t nsize)
{
	return resolvedirent(o, size, 0, pino, name, nsize);
}

/* resolve inode to dirent */

/*
   o       - filesystem image pointer
   size    - size of filesystem image
   ino     - compare against dirent inode

   return value: pointer to relevant dirent structure in
   filesystem image or NULL
 */

static struct jffs2_raw_dirent *resolveinode(char *o, size_t size, uint32_t ino)
{
	return resolvedirent(o, size, ino, 0, NULL, 0);
}

/* resolve slash-style path into dirent and inode.
   slash as first byte marks absolute path (root=inode 1).
   . and .. are resolved properly, and symlinks are followed.
 */

/*
   o       - filesystem image pointer
   size    - size of filesystem image
   ino     - root inode, used if path is relative
   p       - path to be resolved
   inos    - result inode, zero if failure
   recc    - recursion count, to detect symlink loops

   return value: pointer to dirent struct in file system image.
   note that root directory doesn't have dirent struct
   (return value is NULL), but it has inode (*inos=1)
 */

static struct jffs2_raw_dirent *resolvepath0(char *o, size_t size, uint32_t ino,
		const char *p, uint32_t * inos, int recc)
{
	struct jffs2_raw_dirent *dir = NULL;

	int d = 1;
	uint32_t tino;

	char *next;

	char *path, *pp;

	char symbuf[1024];
	size_t symsize;

	if (recc > 16) {
		/* probably symlink loop */
		*inos = 0;
		return NULL;
	}

	pp = path = xstrdup(p);

	if (*path == '/') {
		path++;
		ino = 1;
	}

	if (ino > 1) {
		dir = resolveinode(o, size, ino);

		ino = DIRENT_INO(dir);
	}

	next = path - 1;

	while (ino && next != NULL && next[1] != 0 && d) {
		path = next + 1;
		next = strchr(path, '/');

		if (next != NULL)
			*next = 0;

		if (*path == '.' && path[1] == 0)
			continue;
		if (*path == '.' && path[1] == '.' && path[2] == 0) {
			if (DIRENT_PINO(dir) == 1) {
				ino = 1;
				dir = NULL;
			} else {
				dir = resolveinode(o, size, DIRENT_PINO(dir));
				ino = DIRENT_INO(dir);
			}

			continue;
		}

		dir = resolvename(o, size, ino, path, (uint8_t) strlen(path));

		if (DIRENT_INO(dir) == 0 ||
				(next != NULL &&
				 !(dir->type == DT_DIR || dir->type == DT_LNK))) {
			free(pp);

			*inos = 0;

			return NULL;
		}

		if (dir->type == DT_LNK) {
			struct jffs2_raw_inode *ri;
			ri = find_raw_inode(o, size, DIRENT_INO(dir));
			putblock(symbuf, sizeof(symbuf), &symsize, ri);
			symbuf[symsize] = 0;

			tino = ino;
			ino = 0;

			dir = resolvepath0(o, size, tino, symbuf, &ino, ++recc);

			if (dir != NULL && next != NULL &&
					!(dir->type == DT_DIR || dir->type == DT_LNK)) {
				free(pp);

				*inos = 0;
				return NULL;
			}
		}
		if (dir != NULL)
			ino = DIRENT_INO(dir);
	}

	free(pp);

	*inos = ino;

	return dir;
}

/* resolve slash-style path into dirent and inode.
   slash as first byte marks absolute path (root=inode 1).
   . and .. are resolved properly, and symlinks are followed.
 */

/*
   o       - filesystem image pointer
   size    - size of filesystem image
   ino     - root inode, used if path is relative
   p       - path to be resolved
   inos    - result inode, zero if failure

   return value: pointer to dirent struct in file system image.
   note that root directory doesn't have dirent struct
   (return value is NULL), but it has inode (*inos=1)
 */

static struct jffs2_raw_dirent *resolvepath(char *o, size_t size, uint32_t ino,
		const char *p, uint32_t * inos)
{
	return resolvepath0(o, size, ino, p, inos, 0);
}

/* lists files on directory specified by path */

/*
   o       - filesystem image pointer
   size    - size of filesystem image
   p       - path to be resolved
 */

static void lsdir(char *o, size_t size, const char *path, int recurse,
				  int want_ctime)
{
	struct jffs2_raw_dirent *dd;
	struct dir *d = NULL;

	uint32_t ino;

	dd = resolvepath(o, size, 1, path, &ino);

	if (ino == 0 ||
			(dd == NULL && ino == 0) || (dd != NULL && dd->type != DT_DIR))
		errmsg_die("%s: No such file or directory", path);

	d = collectdir(o, size, ino, d);
	printdir(o, size, d, path, recurse, want_ctime);
	freedir(d);
}

/* writes file specified by path to the buffer */

/*
   o       - filesystem image pointer
   size    - size of filesystem image
   p       - path to be resolved
   b       - file buffer
   bsize   - file buffer size
   rsize   - file result size
 */

static void catfile(char *o, size_t size, char *path, char *b, size_t bsize,
					size_t * rsize)
{
	struct jffs2_raw_dirent *dd;
	struct jffs2_raw_inode *ri;
	uint32_t ino;

	dd = resolvepath(o, size, 1, path, &ino);

	if (ino == 0)
		errmsg_die("%s: No such file or directory", path);

	if (dd == NULL || dd->type != DT_REG)
		errmsg_die("%s: Not a regular file", path);

	ri = find_raw_inode(o, size, ino);
	putblock(b, bsize, rsize, ri);

	write_nocheck(1, b, *rsize);
}

/* usage example */

int main(int argc, char **argv)
{
	int fd, opt, c, recurse = 0, want_ctime = 0;
	struct stat st;

	char *scratch, *dir = NULL, *file = NULL;
	size_t ssize = 0;

	char *buf;

	while ((opt = getopt_long(argc, argv, short_opt, long_opt, &c)) > 0) {
		switch (opt) {
			case 'd':
				dir = optarg;
				break;
			case 'f':
				file = optarg;
				break;
			case 'r':
				recurse++;
				break;
			case 't':
				want_ctime++;
				break;
			case 'V':
				common_print_version();
				exit(EXIT_SUCCESS);
			default:
				fprintf(stderr,
						"Usage: %s <image> [-d|-f] < path >\n",
						PROGRAM_NAME);
				exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
		}
	}

	fd = open(argv[optind], O_RDONLY);
	if (fd == -1)
		sys_errmsg_die("%s", argv[optind]);

	if (fstat(fd, &st))
		sys_errmsg_die("%s", argv[optind]);

	buf = xmalloc((size_t) st.st_size);

	if (read(fd, buf, st.st_size) != (ssize_t) st.st_size)
		sys_errmsg_die("%s", argv[optind]);

	if (dir)
		lsdir(buf, st.st_size, dir, recurse, want_ctime);

	if (file) {
		scratch = xmalloc(SCRATCH_SIZE);

		catfile(buf, st.st_size, file, scratch, SCRATCH_SIZE, &ssize);
		free(scratch);
	}

	if (!dir && !file)
		lsdir(buf, st.st_size, "/", 1, want_ctime);


	free(buf);
	exit(EXIT_SUCCESS);
} }}
        tke_region: ${{ .TH MKFS.JFFS2 1
.SH NAME
mkfs.jffs2 \- Create a JFFS2 file system image from directory
.SH SYNOPSIS
.B mkfs.jffs2
[
.B -p,--pad[=SIZE]
]
[
.B -r,-d,--root
.I directory
]
[
.B -s,--pagesize=SIZE
]
[
.B -e,--eraseblock=SIZE
]
[
.B -c,--cleanmarker=SIZE
]
[
.B -n,--no-cleanmarkers
]
[
.B -o,--output
.I image.jffs2
]
[
.B -l,--little-endian
]
[
.B -b,--big-endian
]
[
.B -D,--devtable=FILE
]
[
.B -f,--faketime
]
[
.B -q,--squash
]
[
.B -U,--squash-uids
]
[
.B -P,--squash-perms
]
[
.B --with-xattr
]
[
.B --with-selinux
]
[
.B --with-posix-acl
]
[
.B -m,--compression-mode=MODE
]
[
.B -x,--disable-compressor=NAME
]
[
.B -X,--enable-compressor=NAME
]
[
.B -y,--compressor-priority=PRIORITY:NAME
]
[
.B -L,--list-compressors
]
[
.B -t,--test-compression
]
[
.B -h,--help
]
[
.B -v,--verbose
]
[
.B -V,--version
]
[
.B -i,--incremental
.I image.jffs2
]

.SH DESCRIPTION
The program
.B mkfs.jffs2
creates a JFFS2 (Second Journalling Flash File System) file system
image and writes the resulting image to the file specified by the
.B -o
option or by default to the standard output, unless the standard
output is a terminal device in which case mkfs.jffs2 will abort.

The file system image is created using the files and directories
contained in the directory specified by the option
.B -r
or the present directory, if the
.B -r
option is not specified.

Each block of the files to be placed into the file system image
are compressed using one of the available compressors depending
on the selected compression mode.

File systems are created with the same endianness as the host,
unless the
.B -b
or
.B -l
options are specified.  JFFS2 driver in the 2.4 Linux kernel only
supported images having the same endianness as the CPU. As of 2.5.48,
the kernel can be changed with a #define to accept images of the
non-native endianness. Full bi-endian support in the kernel is not
planned.

It is unlikely that JFFS2 images are useful except in conjunction
with the MTD (Memory Technology Device) drivers in the Linux
kernel, since the JFFS2 file system driver in the kernel requires
MTD devices.
.SH OPTIONS
Options that take SIZE arguments can be specified as either
decimal (e.g., 65536), octal (0200000), or hexadecimal (0x1000).
.TP
.B -p, --pad[=SIZE]
Pad output to SIZE bytes with 0xFF.  If SIZE is not specified,
the output is padded to the end of the final erase block.
.TP
.B -r, -d, --root=DIR
Build file system from directory DIR.  The default is the current
directory.
.TP
.B -s, --pagesize=SIZE
Use page size SIZE.  The default is 4 KiB.  This size is the
maximum size of a data node.  Set according to target system's memory
management page size (NOTE: this is NOT related to NAND page size).
.TP
.B -e, --eraseblock=SIZE
Use erase block size SIZE.  The default is 64 KiB.  If you use a erase
block size different than the erase block size of the target MTD
device, JFFS2 may not perform optimally. If the SIZE specified is
below 4096, the units are assumed to be KiB.
.TP
.B -c, --cleanmarker=SIZE
Write \'CLEANMARKER\' nodes with the size specified. It is not
normally appropriate to specify a size other than the default 12
bytes.
.TP
.B -n, --no-cleanmarkers
Do not write \'CLEANMARKER\' nodes to the beginning of each erase
block. This option can be useful for creating JFFS2 images for
use on NAND flash, and for creating images which are to be used
on a variety of hardware with differing eraseblock sizes.
.TP
.B -o, --output=FILE
Write JFFS2 image to file FILE.  Default is the standard output.
.TP
.B -l, --little-endian
Create a little-endian JFFS2 image.  Default is to make an image
with the same endianness as the host.
.TP
.B -b, --big-endian
Create a big-endian JFFS2 image.  Default is to make an image
with the same endianness as the host.
.TP
.B -D, --devtable=FILE
Use the named FILE as a device table file, for including devices and
changing permissions in the created image when the user does not have
appropriate permissions to create them on the file system used as
source.
.TP
.B -f, --faketime
Change all file timestamps to \'0\' for regression testing.
.TP
.B -q, --squash
Squash permissions and owners, making all files be owned by root and
removing write permission for \'group\' and \'other\'.
.TP
.B -U, --squash-uids
Squash owners making all files be owned by root.
.TP
.B -P, --squash-perms
Squash permissions, removing write permission for \'group\' and \'other\'.
.TP
.B --with-xattr
Enables xattr, stuff all xattr entries into jffs2 image file.
.TP
.B --with-selinux
Enables xattr, stuff only SELinux Labels into jffs2 image file.
.TP
.B --with-posix-acl
Enable xattr, stuff only POSIX ACL entries into jffs2 image file.
.TP
.B -m, --compression-mode=MODE
Set the default compression mode. The default mode is
.B priority
which tries the compressors in a predefinied order and chooses the first
successful one. The alternatives are:
.B none
(mkfs will not compress) and
.B size
(mkfs will try all compressor and chooses the one which have the smallest result).
.TP
.B -x, --disable-compressor=NAME
Disable a compressor. Use
.B -L
to see the list of the available compressors and their default states.
.TP
.B -X, --enable-compressor=NAME
Enable a compressor. Use
.B -L
to see the list of the available compressors and their default states.
.TP
.B -y, --compressor-priority=PRIORITY:NAME
Set the priority of a compressor. Use
.B -L
to see the list of the available compressors and their default priority.
Priorities are used by priority compression mode.
.TP
.B -L, --list-compressors
Show the list of the available compressors and their states.
.TP
.B -t, --test-compression
Call decompress after every compress - and compare the result with the original data -, and
some other check.
.TP
.B -h, --help
Display help text.
.TP
.B -v, --verbose
Verbose operation.
.TP
.B -V, --version
Display version information.
.TP
.B -i, --incremental=FILE
Generate an appendage image for FILE. If FILE is written to flash and flash
is appended with the output, then it seems as if it was one thing.

.SH LIMITATIONS
The format and grammar of the device table file does not allow it to
create symbolic links when the symbolic links are not already present
in the root working directory.

However, symbolic links may be specified in the device table file
using the \fIl\fR type for the purposes of setting their permissions
and ownership.
.SH BUGS
JFFS2 limits device major and minor numbers to 8 bits each.  Some
consider this a bug.

.B mkfs.jffs2
does not properly handle hard links in the input directory structure.
Currently, hard linked files will be expanded to multiple identical
files in the output image.
.SH AUTHORS
David Woodhouse
.br
Manual page written by David Schleef <ds@schleef.org>
.SH SEE ALSO
.BR mkfs (8),
.BR mkfs.jffs (1),
.BR fakeroot (1) }}
        cluster_id: ${{ /* vi: set sw=4 ts=4: */
/*
 * Build a JFFS2 image in a file, from a given directory tree.
 *
 * Copyright 2001, 2002 Red Hat, Inc.
 *           
 *           2002 Axis Communications AB
 *           2001>
 *           2004 University of Szeged, Hungary
 *           2006 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Cross-endian support added by David Schleef <ds@schleef.org>.
 *
 * Major architectural rewrite by Erik Andersen <andersen@codepoet.org>
 * to allow support for making hard links (though hard links support is
 * not yet implemented), and for munging file permissions and ownership
 * on the fly using --faketime, --squash, --devtable.   And I plugged a
 * few memory leaks, adjusted the error handling and fixed some little
 * nits here and there.
 *
 * I also added a sample device table file.  See device_table.txt
 *  -Erik, September 2001
 *
 * Cleanmarkers support added by Axis Communications AB
 *
 * Rewritten again.  Cleanly separated host and target filsystem
 * activities (mainly so I can reuse all the host handling stuff as I
 * rewrite other mkfs utils).  Added a verbose option to list types
 * and attributes as files are added to the file system.  Major cleanup
 * and scrubbing of the code so it can be read, understood, and
 * modified by mere mortals.
 *
 *  -Erik, November 2002
 */

#define PROGRAM_NAME "mkfs.jffs2"

#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <libgen.h>
#include <ctype.h>
#include <time.h>
#include <getopt.h>
#ifdef WITH_XATTR
#include <sys/xattr.h>
#include <sys/acl.h>
#endif
#include <byteswap.h>
#include <crc32.h>
#include <inttypes.h>
#include <limits.h>

#include "rbtree.h"
#include "common.h"

/* Do not use the weird XPG version of basename */
#undef basename

//#define DMALLOC
//#define mkfs_debug_msg    errmsg
#define mkfs_debug_msg(a...)	{ }

#define PAD(x) (((x)+3)&~3)

struct filesystem_entry {
	char *name;					/* Name of this directory (think basename) */
	char *path;					/* Path of this directory (think dirname) */
	char *fullname;				/* Full name of this directory (i.e. path+name) */
	char *hostname;				/* Full path to this file on the host filesystem */
	uint32_t ino;				/* Inode number of this file in JFFS2 */
	struct stat sb;				/* Stores directory permissions and whatnot */
	char *link;					/* Target a symlink points to. */
	struct filesystem_entry *parent;	/* Parent directory */
	struct filesystem_entry *prev;	/* Only relevant to non-directories */
	struct filesystem_entry *next;	/* Only relevant to non-directories */
	struct filesystem_entry *files;	/* Only relevant to directories */
	struct rb_node hardlink_rb;
};

struct rb_root hardlinks;
static int out_fd = -1;
static int in_fd = -1;
static char default_rootdir[] = ".";
static char *rootdir = default_rootdir;
static int verbose = 0;
static int squash_uids = 0;
static int squash_perms = 0;
static int fake_times = 0;
int target_endian = __BYTE_ORDER;

static uint32_t find_hardlink(struct filesystem_entry *e)
{
	struct filesystem_entry *f;
	struct rb_node **n = &hardlinks.rb_node;
	struct rb_node *parent = NULL;

	while (*n) {
		parent = *n;
		f = rb_entry(parent, struct filesystem_entry, hardlink_rb);

		if ((f->sb.st_dev < e->sb.st_dev) ||
		    (f->sb.st_dev == e->sb.st_dev &&
		     f->sb.st_ino < e->sb.st_ino))
			n = &parent->rb_left;
		else if ((f->sb.st_dev > e->sb.st_dev) ||
			 (f->sb.st_dev == e->sb.st_dev &&
			  f->sb.st_ino > e->sb.st_ino)) {
			n = &parent->rb_right;
		} else
			return f->ino;
	}

	rb_link_node(&e->hardlink_rb, parent, n);
	rb_insert_color(&e->hardlink_rb, &hardlinks);
	return 0;
}

static char *xreadlink(const char *path)
{
	static const int GROWBY = 80; /* how large we will grow strings by */

	char *buf = NULL;
	int bufsize = 0, readsize = 0;

	do {
		buf = xrealloc(buf, bufsize += GROWBY);
		readsize = readlink(path, buf, bufsize); /* 1st try */
		if (readsize == -1) {
			sys_errmsg("%s:%s", PROGRAM_NAME, path);
			free(buf);
			return NULL;
		}
	}
	while (bufsize < readsize + 1);

	buf[readsize] = '\0';

	return buf;
}
static FILE *xfopen(const char *path, const char *mode)
{
	FILE *fp;
	if ((fp = fopen(path, mode)) == NULL)
		sys_errmsg_die("%s", path);
	return fp;
}

static struct filesystem_entry *find_filesystem_entry(
		struct filesystem_entry *dir, char *fullname, uint32_t type)
{
	struct filesystem_entry *e = dir;

	if (S_ISDIR(dir->sb.st_mode)) {
		/* If this is the first call, and we actually want this
		 * directory, then return it now */
		if (strcmp(fullname, e->fullname) == 0)
			return e;

		e = dir->files;
	}
	while (e) {
		if (S_ISDIR(e->sb.st_mode)) {
			int len = strlen(e->fullname);

			/* Check if we are a parent of the correct path */
			if (strncmp(e->fullname, fullname, len) == 0) {
				/* Is this an _exact_ match? */
				if (strcmp(fullname, e->fullname) == 0) {
					return (e);
				}
				/* Looks like we found a parent of the correct path */
				if (fullname[len] == '/') {
					if (e->files) {
						return (find_filesystem_entry (e, fullname, type));
					} else {
						return NULL;
					}
				}
			}
		} else {
			if (strcmp(fullname, e->fullname) == 0) {
				return (e);
			}
		}
		e = e->next;
	}
	return (NULL);
}

static struct filesystem_entry *add_host_filesystem_entry(const char *name,
		const char *path, unsigned long uid, unsigned long gid,
		unsigned long mode, dev_t rdev, struct filesystem_entry *parent)
{
	int status;
	char *tmp;
	struct stat sb;
	time_t timestamp = time(NULL);
	struct filesystem_entry *entry;

	memset(&sb, 0, sizeof(struct stat));
	status = lstat(path, &sb);

	if (status >= 0) {
		/* It is ok for some types of files to not exit on disk (such as
		 * device nodes), but if they _do_ exist the specified mode had
		 * better match the actual file or strange things will happen.... */
		if ((mode & S_IFMT) != (sb.st_mode & S_IFMT)) {
			errmsg_die ("%s: file type does not match specified type!", path);
		}
		timestamp = sb.st_mtime;
	} else {
		/* If this is a regular file, it _must_ exist on disk */
		if ((mode & S_IFMT) == S_IFREG) {
			errmsg_die("%s: does not exist!", path);
		}
	}

	/* Squash all permissions so files are owned by root, all
	 * timestamps are _right now_, and file permissions
	 * have group and other write removed */
	if (squash_uids) {
		uid = gid = 0;
	}
	if (squash_perms) {
		if (!S_ISLNK(mode)) {
			mode &= ~(S_IWGRP | S_IWOTH);
			mode &= ~(S_ISUID | S_ISGID);
		}
	}
	if (fake_times) {
		timestamp = 0;
	}

	entry = xcalloc(1, sizeof(struct filesystem_entry));

	entry->hostname = xstrdup(path);
	entry->fullname = xstrdup(name);
	tmp = xstrdup(name);
	entry->name = xstrdup(basename(tmp));
	free(tmp);
	tmp = xstrdup(name);
	entry->path = xstrdup(dirname(tmp));
	free(tmp);

	entry->sb.st_ino = sb.st_ino;
	entry->sb.st_dev = sb.st_dev;
	entry->sb.st_nlink = sb.st_nlink;

	entry->sb.st_uid = uid;
	entry->sb.st_gid = gid;
	entry->sb.st_mode = mode;
	entry->sb.st_rdev = rdev;
	entry->sb.st_atime = entry->sb.st_ctime =
		entry->sb.st_mtime = timestamp;
	if (S_ISREG(mode)) {
		entry->sb.st_size = sb.st_size;
	}
	if (S_ISLNK(mode)) {
		entry->link = xreadlink(path);
		entry->sb.st_size = strlen(entry->link);
	}

	/* This happens only for root */
	if (!parent)
		return (entry);

	/* Hook the file into the parent directory */
	entry->parent = parent;
	if (!parent->files) {
		parent->files = entry;
	} else {
		struct filesystem_entry *prev;
		for (prev = parent->files; prev->next; prev = prev->next);
		prev->next = entry;
		entry->prev = prev;
	}

	return (entry);
}

static struct filesystem_entry *recursive_add_host_directory(
		struct filesystem_entry *parent, const char *targetpath,
		const char *hostpath)
{
	int i, n;
	struct stat sb;
	char *hpath, *tpath;
	struct dirent *dp, **namelist;
	struct filesystem_entry *entry;


	if (lstat(hostpath, &sb)) {
		sys_errmsg_die("%s", hostpath);
	}

	entry = add_host_filesystem_entry(targetpath, hostpath,
			sb.st_uid, sb.st_gid, sb.st_mode, 0, parent);

	n = scandir(hostpath, &namelist, 0, alphasort);
	if (n < 0) {
		sys_errmsg_die("opening directory %s", hostpath);
	}

	for (i=0; i<n; i++)
	{
		dp = namelist[i];
		if (dp->d_name[0] == '.' && (dp->d_name[1] == 0 ||
					(dp->d_name[1] == '.' &&  dp->d_name[2] == 0)))
		{
			free(dp);
			continue;
		}

		xasprintf(&hpath, "%s/%s", hostpath, dp->d_name);
		if (lstat(hpath, &sb)) {
			sys_errmsg_die("%s", hpath);
		}
		if (strcmp(targetpath, "/") == 0) {
			xasprintf(&tpath, "%s%s", targetpath, dp->d_name);
		} else {
			xasprintf(&tpath, "%s/%s", targetpath, dp->d_name);
		}

		switch (sb.st_mode & S_IFMT) {
			case S_IFDIR:
				recursive_add_host_directory(entry, tpath, hpath);
				break;

			case S_IFREG:
			case S_IFSOCK:
			case S_IFIFO:
			case S_IFLNK:
			case S_IFCHR:
			case S_IFBLK:
				add_host_filesystem_entry(tpath, hpath, sb.st_uid,
						sb.st_gid, sb.st_mode, sb.st_rdev, entry);
				break;

			default:
				errmsg("Unknown file type %o for %s", sb.st_mode, hpath);
				break;
		}
		free(dp);
		free(hpath);
		free(tpath);
	}
	free(namelist);
	return (entry);
}

/* the GNU C library has a wonderful scanf("%as", string) which will
   allocate the string with the right size, good to avoid buffer overruns.
   the following macros use it if available or use a hacky workaround...
 */

#ifdef __GNUC__
#if __STDC_VERSION__ >= 199901L
#define SCANF_PREFIX "m"
#else
#define SCANF_PREFIX "a"
#endif
#define SCANF_STRING(s) (&s)
#define GETCWD_SIZE 0
#else
#define SCANF_PREFIX "511"
#define SCANF_STRING(s) (s = xmalloc(512))
#define GETCWD_SIZE -1
inline int snprintf(char *str, size_t n, const char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vsprintf(str, fmt, ap);
	va_end(ap);
	return ret;
}
#endif

/*  device table entries take the form of:
	<path>	<type> <mode>	<uid>	<gid>	<major>	<minor>	<start>	<inc>	<count>
	/dev/mem     c    640       0       0         1       1       0     0         -

	type can be one of:
	f	A regular file
	d	Directory
	c	Character special device file
	b	Block special device file
	p	Fifo (named pipe)

	I don't bother with symlinks (permissions are irrelevant), hard
	links (special cases of regular files), or sockets (why bother).

	Regular files must exist in the target root directory.  If a char,
	block, fifo, or directory does not exist, it will be created.
 */
static int interpret_table_entry(struct filesystem_entry *root, char *line)
{
	char *hostpath;
	char type, *name = NULL, *tmp, *dir;
	unsigned long mode = 0755, uid = 0, gid = 0, major = 0, minor = 0;
	unsigned long start = 0, increment = 1, count = 0;
	struct filesystem_entry *parent, *entry;

	if (sscanf (line, "%" SCANF_PREFIX "s %c %lo %lu %lu %lu %lu %lu %lu %lu",
				SCANF_STRING(name), &type, &mode, &uid, &gid, &major, &minor,
				&start, &increment, &count) < 0)
	{
		return 1;
	}

	if (!strcmp(name, "/")) {
		errmsg_die("Device table entries require absolute paths");
	}

	xasprintf(&hostpath, "%s%s", rootdir, name);

	/* Check if this file already exists... */
	switch (type) {
		case 'd':
			mode |= S_IFDIR;
			break;
		case 'f':
			mode |= S_IFREG;
			break;
		case 'p':
			mode |= S_IFIFO;
			break;
		case 'c':
			mode |= S_IFCHR;
			break;
		case 'b':
			mode |= S_IFBLK;
			break;
		case 'l':
			mode |= S_IFLNK;
			break;
		default:
			errmsg_die("Unsupported file type '%c'", type);
	}
	entry = find_filesystem_entry(root, name, mode);
	if (entry && !(count > 0 && (type == 'c' || type == 'b'))) {
		/* Ok, we just need to fixup the existing entry
		 * and we will be all done... */
		entry->sb.st_uid = uid;
		entry->sb.st_gid = gid;
		entry->sb.st_mode = mode;
		if (major && minor) {
			entry->sb.st_rdev = makedev(major, minor);
		}
	} else {
		/* If parent is NULL (happens with device table entries),
		 * try and find our parent now) */
		tmp = xstrdup(name);
		dir = dirname(tmp);
		parent = find_filesystem_entry(root, dir, S_IFDIR);
		free(tmp);
		if (parent == NULL) {
			errmsg ("skipping device_table entry '%s': no parent directory!", name);
			free(name);
			free(hostpath);
			return 1;
		}

		switch (type) {
			case 'd':
				add_host_filesystem_entry(name, hostpath, uid, gid, mode, 0, parent);
				break;
			case 'f':
				add_host_filesystem_entry(name, hostpath, uid, gid, mode, 0, parent);
				break;
			case 'p':
				add_host_filesystem_entry(name, hostpath, uid, gid, mode, 0, parent);
				break;
			case 'c':
			case 'b':
				if (count > 0) {
					dev_t rdev;
					unsigned long i;
					char *dname, *hpath;

					for (i = start; i < (start + count); i++) {
						xasprintf(&dname, "%s%lu", name, i);
						xasprintf(&hpath, "%s/%s%lu", rootdir, name, i);
						rdev = makedev(major, minor + (i - start) * increment);
						add_host_filesystem_entry(dname, hpath, uid, gid,
								mode, rdev, parent);
						free(dname);
						free(hpath);
					}
				} else {
					dev_t rdev = makedev(major, minor);
					add_host_filesystem_entry(name, hostpath, uid, gid,
							mode, rdev, parent);
				}
				break;
			default:
				errmsg_die("Unsupported file type '%c'", type);
		}
	}
	free(name);
	free(hostpath);
	return 0;
}

static int parse_device_table(struct filesystem_entry *root, FILE * file)
{
	char *line;
	int status = 0;
	size_t length = 0;

	/* Turn off squash, since we must ensure that values
	 * entered via the device table are not squashed */
	squash_uids = 0;
	squash_perms = 0;

	/* Looks ok so far.  The general plan now is to read in one
	 * line at a time, check for leading comment delimiters ('#'),
	 * then try and parse the line as a device table.  If we fail
	 * to parse things, try and help the poor fool to fix their
	 * device table with a useful error msg... */
	line = NULL;
	while (getline(&line, &length, file) != -1) {
		/* First trim off any whitespace */
		int len = strlen(line);

		/* trim trailing whitespace */
		while (len > 0 && isspace(line[len - 1]))
			line[--len] = '\0';
		/* trim leading whitespace */
		memmove(line, &line[strspn(line, " \n\r\t\v")], len);

		/* How long are we after trimming? */
		len = strlen(line);

		/* If this is NOT a comment line, try to interpret it */
		if (len && *line != '#') {
			if (interpret_table_entry(root, line))
				status = 1;
		}

		free(line);
		line = NULL;
	}
	fclose(file);

	return status;
}

static void cleanup(struct filesystem_entry *dir)
{
	struct filesystem_entry *e, *prev;

	e = dir->files;
	while (e) {
		if (e->name)
			free(e->name);
		if (e->path)
			free(e->path);
		if (e->fullname)
			free(e->fullname);
		e->next = NULL;
		e->name = NULL;
		e->path = NULL;
		e->fullname = NULL;
		e->prev = NULL;
		prev = e;
		if (S_ISDIR(e->sb.st_mode)) {
			cleanup(e);
		}
		e = e->next;
		free(prev);
	}
}

/* Here is where we do the actual creation of the file system */
#include "mtd/jffs2-user.h"

#define JFFS2_MAX_FILE_SIZE 0xFFFFFFFF
#ifndef JFFS2_MAX_SYMLINK_LEN
#define JFFS2_MAX_SYMLINK_LEN 254
#endif

static uint32_t ino = 0;
static uint8_t *file_buffer = NULL;		/* file buffer contains the actual erase block*/
static int out_ofs = 0;
static int erase_block_size = 65536;
static int pad_fs_size = 0;
static int add_cleanmarkers = 1;
static struct jffs2_unknown_node cleanmarker;
static int cleanmarker_size = sizeof(cleanmarker);
static unsigned char ffbuf[16] =
{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff
};

/* We set this at start of main() using sysconf(), -1 means we don't know */
/* When building an fs for non-native systems, use --pagesize=SIZE option */
int page_size = -1;

#include "compr.h"

static void full_write(int fd, const void *buf, int len)
{
	int ret;

	while (len > 0) {
		ret = write(fd, buf, len);

		if (ret < 0)
			sys_errmsg_die("write");

		if (ret == 0)
			sys_errmsg_die("write returned zero");

		len -= ret;
		buf += ret;
		out_ofs += ret;
	}
}

static void padblock(void)
{
	while (out_ofs % erase_block_size) {
		full_write(out_fd, ffbuf, min(sizeof(ffbuf),
					erase_block_size - (out_ofs % erase_block_size)));
	}
}

static void pad(int req)
{
	while (req) {
		if (req > sizeof(ffbuf)) {
			full_write(out_fd, ffbuf, sizeof(ffbuf));
			req -= sizeof(ffbuf);
		} else {
			full_write(out_fd, ffbuf, req);
			req = 0;
		}
	}
}

static inline void padword(void)
{
	if (out_ofs % 4) {
		full_write(out_fd, ffbuf, 4 - (out_ofs % 4));
	}
}

static inline void pad_block_if_less_than(int req)
{
	if (add_cleanmarkers) {
		if ((out_ofs % erase_block_size) == 0) {
			full_write(out_fd, &cleanmarker, sizeof(cleanmarker));
			pad(cleanmarker_size - sizeof(cleanmarker));
			padword();
		}
	}
	if ((out_ofs % erase_block_size) + req > erase_block_size) {
		padblock();
	}
	if (add_cleanmarkers) {
		if ((out_ofs % erase_block_size) == 0) {
			full_write(out_fd, &cleanmarker, sizeof(cleanmarker));
			pad(cleanmarker_size - sizeof(cleanmarker));
			padword();
		}
	}
}

static void write_dirent(struct filesystem_entry *e)
{
	char *name = e->name;
	struct jffs2_raw_dirent rd;
	struct stat *statbuf = &(e->sb);
	static uint32_t version = 0;

	memset(&rd, 0, sizeof(rd));

	rd.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	rd.nodetype = cpu_to_je16(JFFS2_NODETYPE_DIRENT);
	rd.totlen = cpu_to_je32(sizeof(rd) + strlen(name));
	rd.hdr_crc = cpu_to_je32(mtd_crc32(0, &rd,
				sizeof(struct jffs2_unknown_node) - 4));
	rd.pino = cpu_to_je32((e->parent) ? e->parent->ino : 1);
	rd.version = cpu_to_je32(version++);
	rd.ino = cpu_to_je32(e->ino);
	rd.mctime = cpu_to_je32(statbuf->st_mtime);
	rd.nsize = strlen(name);
	rd.type = IFTODT(statbuf->st_mode);
	//rd.unused[0] = 0;
	//rd.unused[1] = 0;
	rd.node_crc = cpu_to_je32(mtd_crc32(0, &rd, sizeof(rd) - 8));
	rd.name_crc = cpu_to_je32(mtd_crc32(0, name, strlen(name)));

	pad_block_if_less_than(sizeof(rd) + rd.nsize);
	full_write(out_fd, &rd, sizeof(rd));
	full_write(out_fd, name, rd.nsize);
	padword();
}

static unsigned int write_regular_file(struct filesystem_entry *e)
{
	int fd, len;
	uint32_t ver;
	unsigned int offset;
	unsigned char *buf, *cbuf, *wbuf;
	struct jffs2_raw_inode ri;
	struct stat *statbuf;
	unsigned int totcomp = 0;

	statbuf = &(e->sb);
	if (statbuf->st_size >= JFFS2_MAX_FILE_SIZE) {
		errmsg("Skipping file \"%s\" too large.", e->path);
		return -1;
	}
	fd = open(e->hostname, O_RDONLY);
	if (fd == -1) {
		sys_errmsg_die("%s: open file", e->hostname);
	}

	e->ino = ++ino;
	mkfs_debug_msg("writing file '%s'  ino=%lu  parent_ino=%lu",
			e->name, (unsigned long) e->ino,
			(unsigned long) e->parent->ino);
	write_dirent(e);

	buf = xmalloc(page_size);
	cbuf = NULL;

	ver = 0;
	offset = 0;

	memset(&ri, 0, sizeof(ri));
	ri.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri.nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);

	ri.ino = cpu_to_je32(e->ino);
	ri.mode = cpu_to_jemode(statbuf->st_mode);
	ri.uid = cpu_to_je16(statbuf->st_uid);
	ri.gid = cpu_to_je16(statbuf->st_gid);
	ri.atime = cpu_to_je32(statbuf->st_atime);
	ri.ctime = cpu_to_je32(statbuf->st_ctime);
	ri.mtime = cpu_to_je32(statbuf->st_mtime);
	ri.isize = cpu_to_je32(statbuf->st_size);

	while ((len = read(fd, buf, page_size))) {
		unsigned char *tbuf = buf;

		if (len < 0) {
			sys_errmsg_die("read");
		}

		while (len) {
			uint32_t dsize, space;
			uint16_t compression;

			pad_block_if_less_than(sizeof(ri) + JFFS2_MIN_DATA_LEN);

			dsize = len;
			space =
				erase_block_size - (out_ofs % erase_block_size) -
				sizeof(ri);
			if (space > dsize)
				space = dsize;

			compression = jffs2_compress(tbuf, &cbuf, &dsize, &space);

			ri.compr = compression & 0xff;
			ri.usercompr = (compression >> 8) & 0xff;

			if (ri.compr) {
				wbuf = cbuf;
			} else {
				wbuf = tbuf;
				dsize = space;
			}

			ri.totlen = cpu_to_je32(sizeof(ri) + space);
			ri.hdr_crc = cpu_to_je32(mtd_crc32(0,
						&ri, sizeof(struct jffs2_unknown_node) - 4));

			ri.version = cpu_to_je32(++ver);
			ri.offset = cpu_to_je32(offset);
			ri.csize = cpu_to_je32(space);
			ri.dsize = cpu_to_je32(dsize);
			ri.node_crc = cpu_to_je32(mtd_crc32(0, &ri, sizeof(ri) - 8));
			ri.data_crc = cpu_to_je32(mtd_crc32(0, wbuf, space));

			full_write(out_fd, &ri, sizeof(ri));
			totcomp += sizeof(ri);
			full_write(out_fd, wbuf, space);
			totcomp += space;
			padword();

			if (tbuf != cbuf) {
				free(cbuf);
				cbuf = NULL;
			}

			tbuf += dsize;
			len -= dsize;
			offset += dsize;

		}
	}
	if (!je32_to_cpu(ri.version)) {
		/* Was empty file */
		pad_block_if_less_than(sizeof(ri));

		ri.version = cpu_to_je32(++ver);
		ri.totlen = cpu_to_je32(sizeof(ri));
		ri.hdr_crc = cpu_to_je32(mtd_crc32(0,
					&ri, sizeof(struct jffs2_unknown_node) - 4));
		ri.csize = cpu_to_je32(0);
		ri.dsize = cpu_to_je32(0);
		ri.node_crc = cpu_to_je32(mtd_crc32(0, &ri, sizeof(ri) - 8));

		full_write(out_fd, &ri, sizeof(ri));
		padword();
	}
	free(buf);
	close(fd);
	return totcomp;
}

static void write_symlink(struct filesystem_entry *e)
{
	int len;
	struct stat *statbuf;
	struct jffs2_raw_inode ri;

	statbuf = &(e->sb);
	e->ino = ++ino;
	mkfs_debug_msg("writing symlink '%s'  ino=%lu  parent_ino=%lu",
			e->name, (unsigned long) e->ino,
			(unsigned long) e->parent->ino);
	write_dirent(e);

	len = strlen(e->link);
	if (len > JFFS2_MAX_SYMLINK_LEN) {
		errmsg("symlink too large. Truncated to %d chars.",
				JFFS2_MAX_SYMLINK_LEN);
		len = JFFS2_MAX_SYMLINK_LEN;
	}

	memset(&ri, 0, sizeof(ri));

	ri.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri.nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri.totlen = cpu_to_je32(sizeof(ri) + len);
	ri.hdr_crc = cpu_to_je32(mtd_crc32(0,
				&ri, sizeof(struct jffs2_unknown_node) - 4));

	ri.ino = cpu_to_je32(e->ino);
	ri.mode = cpu_to_jemode(statbuf->st_mode);
	ri.uid = cpu_to_je16(statbuf->st_uid);
	ri.gid = cpu_to_je16(statbuf->st_gid);
	ri.atime = cpu_to_je32(statbuf->st_atime);
	ri.ctime = cpu_to_je32(statbuf->st_ctime);
	ri.mtime = cpu_to_je32(statbuf->st_mtime);
	ri.isize = cpu_to_je32(statbuf->st_size);
	ri.version = cpu_to_je32(1);
	ri.csize = cpu_to_je32(len);
	ri.dsize = cpu_to_je32(len);
	ri.node_crc = cpu_to_je32(mtd_crc32(0, &ri, sizeof(ri) - 8));
	ri.data_crc = cpu_to_je32(mtd_crc32(0, e->link, len));

	pad_block_if_less_than(sizeof(ri) + len);
	full_write(out_fd, &ri, sizeof(ri));
	full_write(out_fd, e->link, len);
	padword();
}

static void write_pipe(struct filesystem_entry *e)
{
	struct stat *statbuf;
	struct jffs2_raw_inode ri;

	statbuf = &(e->sb);
	e->ino = ++ino;
	if (S_ISDIR(statbuf->st_mode)) {
		mkfs_debug_msg("writing dir '%s'  ino=%lu  parent_ino=%lu",
				e->name, (unsigned long) e->ino,
				(unsigned long) (e->parent) ? e->parent->ino : 1);
	}
	write_dirent(e);

	memset(&ri, 0, sizeof(ri));

	ri.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri.nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri.totlen = cpu_to_je32(sizeof(ri));
	ri.hdr_crc = cpu_to_je32(mtd_crc32(0,
				&ri, sizeof(struct jffs2_unknown_node) - 4));

	ri.ino = cpu_to_je32(e->ino);
	ri.mode = cpu_to_jemode(statbuf->st_mode);
	ri.uid = cpu_to_je16(statbuf->st_uid);
	ri.gid = cpu_to_je16(statbuf->st_gid);
	ri.atime = cpu_to_je32(statbuf->st_atime);
	ri.ctime = cpu_to_je32(statbuf->st_ctime);
	ri.mtime = cpu_to_je32(statbuf->st_mtime);
	ri.isize = cpu_to_je32(0);
	ri.version = cpu_to_je32(1);
	ri.csize = cpu_to_je32(0);
	ri.dsize = cpu_to_je32(0);
	ri.node_crc = cpu_to_je32(mtd_crc32(0, &ri, sizeof(ri) - 8));
	ri.data_crc = cpu_to_je32(0);

	pad_block_if_less_than(sizeof(ri));
	full_write(out_fd, &ri, sizeof(ri));
	padword();
}

static void write_special_file(struct filesystem_entry *e)
{
	jint16_t kdev;
	struct stat *statbuf;
	struct jffs2_raw_inode ri;

	statbuf = &(e->sb);
	e->ino = ++ino;
	write_dirent(e);

	kdev = cpu_to_je16((major(statbuf->st_rdev) << 8) +
			minor(statbuf->st_rdev));

	memset(&ri, 0, sizeof(ri));

	ri.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri.nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri.totlen = cpu_to_je32(sizeof(ri) + sizeof(kdev));
	ri.hdr_crc = cpu_to_je32(mtd_crc32(0,
				&ri, sizeof(struct jffs2_unknown_node) - 4));

	ri.ino = cpu_to_je32(e->ino);
	ri.mode = cpu_to_jemode(statbuf->st_mode);
	ri.uid = cpu_to_je16(statbuf->st_uid);
	ri.gid = cpu_to_je16(statbuf->st_gid);
	ri.atime = cpu_to_je32(statbuf->st_atime);
	ri.ctime = cpu_to_je32(statbuf->st_ctime);
	ri.mtime = cpu_to_je32(statbuf->st_mtime);
	ri.isize = cpu_to_je32(statbuf->st_size);
	ri.version = cpu_to_je32(1);
	ri.csize = cpu_to_je32(sizeof(kdev));
	ri.dsize = cpu_to_je32(sizeof(kdev));
	ri.node_crc = cpu_to_je32(mtd_crc32(0, &ri, sizeof(ri) - 8));
	ri.data_crc = cpu_to_je32(mtd_crc32(0, &kdev, sizeof(kdev)));

	pad_block_if_less_than(sizeof(ri) + sizeof(kdev));
	full_write(out_fd, &ri, sizeof(ri));
	full_write(out_fd, &kdev, sizeof(kdev));
	padword();
}

#ifdef WITH_XATTR
typedef struct xattr_entry {
	struct xattr_entry *next;
	uint32_t xid;
	int xprefix;
	char *xname;
	char *xvalue;
	int name_len;
	int value_len;
} xattr_entry_t;

#define XATTR_BUFFER_SIZE		(64 * 1024)	/* 64KB */
static uint32_t enable_xattr = 0;
static uint32_t highest_xid = 0;
static uint32_t highest_xseqno = 0;

static struct {
	int xprefix;
	const char *string;
	int length;
} xprefix_tbl[] = {
	{ JFFS2_XPREFIX_USER, XATTR_USER_PREFIX, XATTR_USER_PREFIX_LEN },
	{ JFFS2_XPREFIX_SECURITY, XATTR_SECURITY_PREFIX, XATTR_SECURITY_PREFIX_LEN },
	{ JFFS2_XPREFIX_ACL_ACCESS, POSIX_ACL_XATTR_ACCESS, POSIX_ACL_XATTR_ACCESS_LEN },
	{ JFFS2_XPREFIX_ACL_DEFAULT, POSIX_ACL_XATTR_DEFAULT, POSIX_ACL_XATTR_DEFAULT_LEN },
	{ JFFS2_XPREFIX_TRUSTED, XATTR_TRUSTED_PREFIX, XATTR_TRUSTED_PREFIX_LEN },
	{ 0, NULL, 0 }
};

static void formalize_posix_acl(void *xvalue, int *value_len)
{
	struct posix_acl_xattr_header *pacl_header;
	struct posix_acl_xattr_entry *pent, *plim;
	struct jffs2_acl_header *jacl_header;
	struct jffs2_acl_entry *jent;
	struct jffs2_acl_entry_short *jent_s;
	char buffer[XATTR_BUFFER_SIZE];
	int offset = 0;

	pacl_header = xvalue;;
	pent = pacl_header->a_entries;
	plim = xvalue + *value_len;

	jacl_header = (struct jffs2_acl_header *)buffer;
	offset += sizeof(struct jffs2_acl_header);
	jacl_header->a_version = cpu_to_je32(JFFS2_ACL_VERSION);

	while (pent < plim) {
		switch(le16_to_cpu(pent->e_tag)) {
			case ACL_USER_OBJ:
			case ACL_GROUP_OBJ:
			case ACL_MASK:
			case ACL_OTHER:
				jent_s = (struct jffs2_acl_entry_short *)(buffer + offset);
				offset += sizeof(struct jffs2_acl_entry_short);
				jent_s->e_tag = cpu_to_je16(le16_to_cpu(pent->e_tag));
				jent_s->e_perm = cpu_to_je16(le16_to_cpu(pent->e_perm));
				break;
			case ACL_USER:
			case ACL_GROUP:
				jent = (struct jffs2_acl_entry *)(buffer + offset);
				offset += sizeof(struct jffs2_acl_entry);
				jent->e_tag = cpu_to_je16(le16_to_cpu(pent->e_tag));
				jent->e_perm = cpu_to_je16(le16_to_cpu(pent->e_perm));
				jent->e_id = cpu_to_je32(le32_to_cpu(pent->e_id));
				break;
			default:
				printf("%04x : Unknown XATTR entry tag.\n", le16_to_cpu(pent->e_tag));
				exit(1);
		}
		pent++;
	}
	if (offset > *value_len) {
		printf("Length of JFFS2 ACL expression(%u) is longer than general one(%u).\n",
				offset, *value_len);
		exit(EXIT_FAILURE);
	}
	memcpy(xvalue, buffer, offset);
	*value_len = offset;
}

static xattr_entry_t *create_xattr_entry(int xprefix, char *xname, char *xvalue, int value_len)
{
	xattr_entry_t *xe;
	struct jffs2_raw_xattr rx;
	int name_len;

	/* create xattr entry */
	name_len = strlen(xname);
	xe = xcalloc(1, sizeof(xattr_entry_t) + name_len + 1 + value_len);
	xe->next = NULL;
	xe->xid = ++highest_xid;
	xe->xprefix = xprefix;
	xe->xname = ((char *)xe) + sizeof(xattr_entry_t);
	xe->xvalue = xe->xname + name_len + 1;
	xe->name_len = name_len;
	xe->value_len = value_len;
	strcpy(xe->xname, xname);
	memcpy(xe->xvalue, xvalue, value_len);

	/* write xattr node */
	memset(&rx, 0, sizeof(rx));
	rx.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	rx.nodetype = cpu_to_je16(JFFS2_NODETYPE_XATTR);
	rx.totlen = cpu_to_je32(PAD(sizeof(rx) + xe->name_len + 1 + xe->value_len));
	rx.hdr_crc = cpu_to_je32(mtd_crc32(0, &rx, sizeof(struct jffs2_unknown_node) - 4));

	rx.xid = cpu_to_je32(xe->xid);
	rx.version = cpu_to_je32(1);	/* initial version */
	rx.xprefix = xprefix;
	rx.name_len = xe->name_len;
	rx.value_len = cpu_to_je16(xe->value_len);
	rx.data_crc = cpu_to_je32(mtd_crc32(0, xe->xname, xe->name_len + 1 + xe->value_len));
	rx.node_crc = cpu_to_je32(mtd_crc32(0, &rx, sizeof(rx) - 4));

	pad_block_if_less_than(sizeof(rx) + xe->name_len + 1 + xe->value_len);
	full_write(out_fd, &rx, sizeof(rx));
	full_write(out_fd, xe->xname, xe->name_len + 1 + xe->value_len);
	padword();

	return xe;
}

#define XATTRENTRY_HASHSIZE	57
static xattr_entry_t *find_xattr_entry(int xprefix, char *xname, char *xvalue, int value_len)
{
	static xattr_entry_t **xentry_hash = NULL;
	xattr_entry_t *xe;
	int index, name_len;

	/* create hash table */
	if (!xentry_hash)
		xentry_hash = xcalloc(1, sizeof(xe) * XATTRENTRY_HASHSIZE);

	if (xprefix == JFFS2_XPREFIX_ACL_ACCESS
			|| xprefix == JFFS2_XPREFIX_ACL_DEFAULT)
		formalize_posix_acl(xvalue, &value_len);

	name_len = strlen(xname);
	index = (mtd_crc32(0, xname, name_len) ^ mtd_crc32(0, xvalue, value_len)) % XATTRENTRY_HASHSIZE;
	for (xe = xentry_hash[index]; xe; xe = xe->next) {
		if (xe->xprefix == xprefix
				&& xe->value_len == value_len
				&& !strcmp(xe->xname, xname)
				&& !memcmp(xe->xvalue, xvalue, value_len))
			break;
	}
	if (!xe) {
		xe = create_xattr_entry(xprefix, xname, xvalue, value_len);
		xe->next = xentry_hash[index];
		xentry_hash[index] = xe;
	}
	return xe;
}

static void write_xattr_entry(struct filesystem_entry *e)
{
	struct jffs2_raw_xref ref;
	struct xattr_entry *xe;
	char xlist[XATTR_BUFFER_SIZE], xvalue[XATTR_BUFFER_SIZE];
	char *xname;
	const char *prefix_str;
	int i, xprefix, prefix_len;
	int list_sz, offset, name_len, value_len;

	if (!enable_xattr)
		return;

	list_sz = llistxattr(e->hostname, xlist, XATTR_BUFFER_SIZE);
	if (list_sz < 0) {
		if (verbose)
			printf("llistxattr('%s') = %d : %s\n",
					e->hostname, errno, strerror(errno));
		return;
	}

	for (offset = 0; offset < list_sz; offset += name_len) {
		xname = xlist + offset;
		name_len = strlen(xname) + 1;

		for (i = 0; (xprefix = xprefix_tbl[i].xprefix); i++) {
			prefix_str = xprefix_tbl[i].string;
			prefix_len = xprefix_tbl[i].length;
			if (prefix_str[prefix_len - 1] == '.') {
				if (!strncmp(xname, prefix_str, prefix_len - 1))
					break;
			} else {
				if (!strcmp(xname, prefix_str))
					break;
			}
		}
		if (!xprefix) {
			if (verbose)
				printf("%s: xattr '%s' is not supported.\n",
						e->hostname, xname);
			continue;
		}
		if ((enable_xattr & (1 << xprefix)) == 0)
			continue;

		value_len = lgetxattr(e->hostname, xname, xvalue, XATTR_BUFFER_SIZE);
		if (value_len < 0) {
			if (verbose)
				printf("lgetxattr('%s', '%s') = %d : %s\n",
						e->hostname, xname, errno, strerror(errno));
			continue;
		}
		xe = find_xattr_entry(xprefix, xname + prefix_len, xvalue, value_len);
		if (!xe) {
			if (verbose)
				printf("%s : xattr '%s' was ignored.\n",
						e->hostname, xname);
			continue;
		}

		memset(&ref, 0, sizeof(ref));
		ref.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
		ref.nodetype = cpu_to_je16(JFFS2_NODETYPE_XREF);
		ref.totlen = cpu_to_je32(sizeof(ref));
		ref.hdr_crc = cpu_to_je32(mtd_crc32(0, &ref, sizeof(struct jffs2_unknown_node) - 4));
		ref.ino = cpu_to_je32(e->ino);
		ref.xid = cpu_to_je32(xe->xid);
		ref.xseqno = cpu_to_je32(highest_xseqno += 2);
		ref.node_crc = cpu_to_je32(mtd_crc32(0, &ref, sizeof(ref) - 4));

		pad_block_if_less_than(sizeof(ref));
		full_write(out_fd, &ref, sizeof(ref));
		padword();
	}
}

#else /* WITH_XATTR */
#define write_xattr_entry(x)
#endif

static void recursive_populate_directory(struct filesystem_entry *dir)
{
	struct filesystem_entry *e;
	unsigned int wrote;

	if (verbose) {
		printf("%s\n", dir->fullname);
	}
	write_xattr_entry(dir);		/* for '/' */

	e = dir->files;
	while (e) {
		if (e->sb.st_nlink >= 1 &&
		    (e->ino = find_hardlink(e))) {

			write_dirent(e);
			if (verbose) {
				printf("\tL %04o %9lu             %5d:%-3d %s\n",
				       e->sb.st_mode & ~S_IFMT, (unsigned long) e->ino,
				       (int) (e->sb.st_uid), (int) (e->sb.st_gid),
				       e->name);
			}
		} else switch (e->sb.st_mode & S_IFMT) {
			case S_IFDIR:
				if (verbose) {
					printf("\td %04o %9lld             %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, (long long)e->sb.st_size,
							(int) (e->sb.st_uid), (int) (e->sb.st_gid),
							e->name);
				}
				write_pipe(e);
				write_xattr_entry(e);
				break;
			case S_IFSOCK:
				if (verbose) {
					printf("\ts %04o %9lld             %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, (long long)e->sb.st_size,
							(int) e->sb.st_uid, (int) e->sb.st_gid, e->name);
				}
				write_pipe(e);
				write_xattr_entry(e);
				break;
			case S_IFIFO:
				if (verbose) {
					printf("\tp %04o %9lld             %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, (long long)e->sb.st_size,
							(int) e->sb.st_uid, (int) e->sb.st_gid, e->name);
				}
				write_pipe(e);
				write_xattr_entry(e);
				break;
			case S_IFCHR:
				if (verbose) {
					printf("\tc %04o %4d,%4d             %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, major(e->sb.st_rdev),
							minor(e->sb.st_rdev), (int) e->sb.st_uid,
							(int) e->sb.st_gid, e->name);
				}
				write_special_file(e);
				write_xattr_entry(e);
				break;
			case S_IFBLK:
				if (verbose) {
					printf("\tb %04o %4d,%4d             %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, major(e->sb.st_rdev),
							minor(e->sb.st_rdev), (int) e->sb.st_uid,
							(int) e->sb.st_gid, e->name);
				}
				write_special_file(e);
				write_xattr_entry(e);
				break;
			case S_IFLNK:
				if (verbose) {
					printf("\tl %04o %9lld             %5d:%-3d %s -> %s\n",
							e->sb.st_mode & ~S_IFMT, (long long)e->sb.st_size,
							(int) e->sb.st_uid, (int) e->sb.st_gid, e->name,
							e->link);
				}
				write_symlink(e);
				write_xattr_entry(e);
				break;
			case S_IFREG:
				wrote = write_regular_file(e);
				write_xattr_entry(e);
				if (verbose) {
					printf("\tf %04o %9lld (%9u) %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, (long long)e->sb.st_size, wrote,
							(int) e->sb.st_uid, (int) e->sb.st_gid, e->name);
				}
				break;
			default:
				errmsg("Unknown mode %o for %s", e->sb.st_mode,
						e->fullname);
				break;
		}
		e = e->next;
	}

	e = dir->files;
	while (e) {
		if (S_ISDIR(e->sb.st_mode)) {
			if (e->files) {
				recursive_populate_directory(e);
			} else if (verbose) {
				printf("%s\n", e->fullname);
			}
		}
		e = e->next;
	}
}

static void create_target_filesystem(struct filesystem_entry *root)
{
	cleanmarker.magic    = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	cleanmarker.nodetype = cpu_to_je16(JFFS2_NODETYPE_CLEANMARKER);
	cleanmarker.totlen   = cpu_to_je32(cleanmarker_size);
	cleanmarker.hdr_crc  = cpu_to_je32(mtd_crc32(0, &cleanmarker, sizeof(struct jffs2_unknown_node)-4));

	if (ino == 0)
		ino = 1;

	root->ino = 1;
	recursive_populate_directory(root);

	if (pad_fs_size == -1) {
		padblock();
	} else {
		if (pad_fs_size && add_cleanmarkers){
			padblock();
			while (out_ofs < pad_fs_size) {
				full_write(out_fd, &cleanmarker, sizeof(cleanmarker));
				pad(cleanmarker_size - sizeof(cleanmarker));
				padblock();
			}
		} else {
			while (out_ofs < pad_fs_size) {
				full_write(out_fd, ffbuf, min(sizeof(ffbuf), pad_fs_size - out_ofs));
			}

		}
	}
}

static struct option long_options[] = {
	{"pad", 2, NULL, 'p'},
	{"root", 1, NULL, 'r'},
	{"pagesize", 1, NULL, 's'},
	{"eraseblock", 1, NULL, 'e'},
	{"output", 1, NULL, 'o'},
	{"help", 0, NULL, 'h'},
	{"verbose", 0, NULL, 'v'},
	{"version", 0, NULL, 'V'},
	{"big-endian", 0, NULL, 'b'},
	{"little-endian", 0, NULL, 'l'},
	{"no-cleanmarkers", 0, NULL, 'n'},
	{"cleanmarker", 1, NULL, 'c'},
	{"squash", 0, NULL, 'q'},
	{"squash-uids", 0, NULL, 'U'},
	{"squash-perms", 0, NULL, 'P'},
	{"faketime", 0, NULL, 'f'},
	{"devtable", 1, NULL, 'D'},
	{"compression-mode", 1, NULL, 'm'},
	{"disable-compressor", 1, NULL, 'x'},
	{"enable-compressor", 1, NULL, 'X'},
	{"test-compression", 0, NULL, 't'},
	{"compressor-priority", 1, NULL, 'y'},
	{"incremental", 1, NULL, 'i'},
#ifdef WITH_XATTR
	{"with-xattr", 0, NULL, 1000 },
	{"with-selinux", 0, NULL, 1001 },
	{"with-posix-acl", 0, NULL, 1002 },
#endif
	{NULL, 0, NULL, 0}
};

static const char helptext[] =
"Usage: mkfs.jffs2 [OPTIONS]\n"
"Make a JFFS2 file system image from an existing directory tree\n\n"
"Options:\n"
"  -p, --pad[=SIZE]        Pad output to SIZE bytes with 0xFF. If SIZE is\n"
"                          not specified, the output is padded to the end of\n"
"                          the final erase block\n"
"  -r, -d, --root=DIR      Build file system from directory DIR (default: cwd)\n"
"  -s, --pagesize=SIZE     Use page size (max data node size) SIZE.\n"
"                          Set according to target system's memory management\n"
"                          page size (default: 4KiB)\n"
"  -e, --eraseblock=SIZE   Use erase block size SIZE (default: 64KiB)\n"
"  -c, --cleanmarker=SIZE  Size of cleanmarker (default 12)\n"
"  -m, --compression-mode=MODE\n"
"                          Select compression mode (default: priority)\n"
"  -x, --disable-compressor=COMPRESSOR_NAME\n"
"                          Disable a compressor\n"
"  -X, --enable-compressor=COMPRESSOR_NAME\n"
"                          Enable a compressor\n"
"  -y, --compressor-priority=PRIORITY:COMPRESSOR_NAME\n"
"                          Set the priority of a compressor\n"
"  -L, --list-compressors  Show the list of the available compressors\n"
"  -t, --test-compression  Call decompress and compare with the original (for test)\n"
"  -n, --no-cleanmarkers   Don't add a cleanmarker to every eraseblock\n"
"  -o, --output=FILE       Output to FILE (default: stdout)\n"
"  -l, --little-endian     Create a little-endian filesystem\n"
"  -b, --big-endian        Create a big-endian filesystem\n"
"  -D, --devtable=FILE     Use the named FILE as a device table file\n"
"  -f, --faketime          Change all file times to '0' for regression testing\n"
"  -q, --squash            Squash permissions and owners making all files be owned by root\n"
"  -U, --squash-uids       Squash owners making all files be owned by root\n"
"  -P, --squash-perms      Squash permissions on all files\n"
#ifdef WITH_XATTR
"      --with-xattr        stuff all xattr entries into image\n"
"      --with-selinux      stuff only SELinux Labels into jffs2 image\n"
"      --with-posix-acl    stuff only POSIX ACL entries into jffs2 image\n"
#endif
"  -h, --help              Display this help text\n"
"  -v, --verbose           Verbose operation\n"
"  -V, --version           Display version information\n"
"  -i, --incremental=FILE  Parse FILE and generate appendage output for it\n\n";

static int load_next_block(void) {

	int ret;
	ret = read(in_fd, file_buffer, erase_block_size);

	if(verbose)
		printf("Load next block : %d bytes read\n",ret);

	return ret;
}

static void process_buffer(int inp_size) {
	uint8_t		*p = file_buffer;
	union jffs2_node_union 	*node;
	uint16_t	type;
	int		bitchbitmask = 0;
	int		obsolete;

	char	name[256];

	while ( p < (file_buffer + inp_size)) {

		node = (union jffs2_node_union *) p;

		/* Skip empty space */
		if (je16_to_cpu (node->u.magic) == 0xFFFF && je16_to_cpu (node->u.nodetype) == 0xFFFF) {
			p += 4;
			continue;
		}

		if (je16_to_cpu (node->u.magic) != JFFS2_MAGIC_BITMASK)	{
			if (!bitchbitmask++)
				printf ("Wrong bitmask  at  0x%08zx, 0x%04x\n", p - file_buffer, je16_to_cpu (node->u.magic));
			p += 4;
			continue;
		}

		bitchbitmask = 0;

		type = je16_to_cpu(node->u.nodetype);
		if ((type & JFFS2_NODE_ACCURATE) != JFFS2_NODE_ACCURATE) {
			obsolete = 1;
			type |= JFFS2_NODE_ACCURATE;
		} else
			obsolete = 0;

		node->u.nodetype = cpu_to_je16(type);

		switch(je16_to_cpu(node->u.nodetype)) {

			case JFFS2_NODETYPE_INODE:
				if(verbose)
					printf ("%8s Inode      node at 0x%08zx, totlen 0x%08x, #ino  %5d, version %5d, isize %8d, csize %8d, dsize %8d, offset %8d\n",
							obsolete ? "Obsolete" : "",
							p - file_buffer, je32_to_cpu (node->i.totlen), je32_to_cpu (node->i.ino),
							je32_to_cpu ( node->i.version), je32_to_cpu (node->i.isize),
							je32_to_cpu (node->i.csize), je32_to_cpu (node->i.dsize), je32_to_cpu (node->i.offset));

				if ( je32_to_cpu (node->i.ino) > ino )
					ino = je32_to_cpu (node->i.ino);

				p += PAD(je32_to_cpu (node->i.totlen));
				break;

			case JFFS2_NODETYPE_DIRENT:
				memcpy (name, node->d.name, node->d.nsize);
				name [node->d.nsize] = 0x0;

				if(verbose)
					printf ("%8s Dirent     node at 0x%08zx, totlen 0x%08x, #pino %5d, version %5d, #ino  %8d, nsize %8d, name %s\n",
							obsolete ? "Obsolete" : "",
							p - file_buffer, je32_to_cpu (node->d.totlen), je32_to_cpu (node->d.pino),
							je32_to_cpu ( node->d.version), je32_to_cpu (node->d.ino),
							node->d.nsize, name);

				p += PAD(je32_to_cpu (node->d.totlen));
				break;

			case JFFS2_NODETYPE_CLEANMARKER:
				if (verbose) {
					printf ("%8s Cleanmarker     at 0x%08zx, totlen 0x%08x\n",
							obsolete ? "Obsolete" : "",
							p - file_buffer, je32_to_cpu (node->u.totlen));
				}

				p += PAD(je32_to_cpu (node->u.totlen));
				break;

			case JFFS2_NODETYPE_PADDING:
				if (verbose) {
					printf ("%8s Padding    node at 0x%08zx, totlen 0x%08x\n",
							obsolete ? "Obsolete" : "",
							p - file_buffer, je32_to_cpu (node->u.totlen));
				}

				p += PAD(je32_to_cpu (node->u.totlen));
				break;

			case 0xffff:
				p += 4;
				break;

			default:
				if (verbose) {
					printf ("%8s Unknown    node at 0x%08zx, totlen 0x%08x\n",
							obsolete ? "Obsolete" : "",
							p - file_buffer, je32_to_cpu (node->u.totlen));
				}

				p += PAD(je32_to_cpu (node->u.totlen));
		}
	}
}

static void parse_image(void){
	int ret;

	file_buffer = xmalloc(erase_block_size);

	while ((ret = load_next_block())) {
		process_buffer(ret);
	}

	if (file_buffer)
		free(file_buffer);

	close(in_fd);
}

int main(int argc, char **argv)
{
	int c, opt;
	char *cwd;
	struct stat sb;
	FILE *devtable = NULL;
	struct filesystem_entry *root;
	char *compr_name = NULL;
	int compr_prior  = -1;
	int warn_page_size = 0;

	page_size = sysconf(_SC_PAGESIZE);
	if (page_size < 0) /* System doesn't know so ... */
		page_size = 4096; /* ... we make an educated guess */
	if (page_size != 4096)
		warn_page_size = 1; /* warn user if page size not 4096 */

	jffs2_compressors_init();

	while ((opt = getopt_long(argc, argv,
					"D:d:r:s:o:qUPfh?vVe:lbp::nc:m:x:X:Lty:i:", long_options, &c)) >= 0)
	{
		switch (opt) {
			case 'D':
				devtable = xfopen(optarg, "r");
				if (fstat(fileno(devtable), &sb) < 0)
					sys_errmsg_die("%s", optarg);
				if (sb.st_size < 10)
					errmsg_die("%s: not a proper device table file", optarg);
				break;

			case 'r':
			case 'd':	/* for compatibility with mkfs.jffs, genext2fs, etc... */
				if (rootdir != default_rootdir) {
					errmsg_die("root directory specified more than once");
				}
				rootdir = xstrdup(optarg);
				break;

			case 's':
				page_size = strtol(optarg, NULL, 0);
				warn_page_size = 0; /* set by user, so don't need to warn */
				break;

			case 'o':
				if (out_fd != -1) {
					errmsg_die("output filename specified more than once");
				}
				out_fd = open(optarg, O_CREAT | O_TRUNC | O_RDWR, 0644);
				if (out_fd == -1) {
					sys_errmsg_die("open output file");
				}
				break;

			case 'q':
				squash_uids = 1;
				squash_perms = 1;
				break;

			case 'U':
				squash_uids = 1;
				break;

			case 'P':
				squash_perms = 1;
				break;

			case 'f':
				fake_times = 1;
				break;

			case 'h':
				puts(helptext);
				exit(EXIT_SUCCESS);
			case '?':
				puts(helptext);
				exit(EXIT_FAILURE);

			case 'v':
				verbose = 1;
				break;

			case 'V':
				common_print_version();
				exit(EXIT_SUCCESS);

			case 'e': {
						  char *next;
						  unsigned units = 0;
						  erase_block_size = strtol(optarg, &next, 0);
						  if (!erase_block_size)
							  errmsg_die("Unrecognisable erase size\n");

						  if (*next) {
							  if (!strcmp(next, "KiB")) {
								  units = 1024;
							  } else if (!strcmp(next, "MiB")) {
								  units = 1024 * 1024;
							  } else {
								  errmsg_die("Unknown units in erasesize\n");
							  }
						  } else {
							  if (erase_block_size < 0x1000)
								  units = 1024;
							  else
								  units = 1;
						  }
						  erase_block_size *= units;

						  /* If it's less than 8KiB, they're not allowed */
						  if (erase_block_size < 0x2000) {
							  fprintf(stderr, "Erase size 0x%x too small. Increasing to 8KiB minimum\n",
									  erase_block_size);
							  erase_block_size = 0x2000;
						  }
						  break;
					  }

			case 'l':
					  target_endian = __LITTLE_ENDIAN;
					  break;

			case 'b':
					  target_endian = __BIG_ENDIAN;
					  break;

			case 'p':
					  if (optarg)
						  pad_fs_size = strtol(optarg, NULL, 0);
					  else
						  pad_fs_size = -1;
					  break;
			case 'n':
					  add_cleanmarkers = 0;
					  break;
			case 'c':
					  cleanmarker_size = strtol(optarg, NULL, 0);
					  if (cleanmarker_size < sizeof(cleanmarker)) {
						  errmsg_die("cleanmarker size must be >= 12");
					  }
					  if (cleanmarker_size >= erase_block_size) {
						  errmsg_die("cleanmarker size must be < eraseblock size");
					  }
					  break;
			case 'm':
					  if (jffs2_set_compression_mode_name(optarg)) {
						  errmsg_die("Unknown compression mode %s", optarg);
					  }
					  break;
			case 'x':
					  if (jffs2_disable_compressor_name(optarg)) {
						  errmsg_die("Unknown compressor name %s",optarg);
					  }
					  break;
			case 'X':
					  if (jffs2_enable_compressor_name(optarg)) {
						  errmsg_die("Unknown compressor name %s",optarg);
					  }
					  break;
			case 'L':
					  errmsg_die("\n%s",jffs2_list_compressors());
					  break;
			case 't':
					  jffs2_compression_check_set(1);
					  break;
			case 'y':
					  compr_name = xmalloc(strlen(optarg));
					  sscanf(optarg,"%d:%s",&compr_prior,compr_name);
					  if ((compr_prior>=0)&&(compr_name)) {
						  if (jffs2_set_compressor_priority(compr_name, compr_prior))
							  exit(EXIT_FAILURE);
					  }
					  else {
						  errmsg_die("Cannot parse %s",optarg);
					  }
					  free(compr_name);
					  break;
			case 'i':
					  if (in_fd != -1) {
						  errmsg_die("(incremental) filename specified more than once");
					  }
					  in_fd = open(optarg, O_RDONLY);
					  if (in_fd == -1) {
						  sys_errmsg_die("cannot open (incremental) file");
					  }
					  break;
#ifdef WITH_XATTR
			case 1000:	/* --with-xattr  */
					  enable_xattr |= (1 << JFFS2_XPREFIX_USER)
						  | (1 << JFFS2_XPREFIX_SECURITY)
						  | (1 << JFFS2_XPREFIX_ACL_ACCESS)
						  | (1 << JFFS2_XPREFIX_ACL_DEFAULT)
						  | (1 << JFFS2_XPREFIX_TRUSTED);
					  break;
			case 1001:	/*  --with-selinux  */
					  enable_xattr |= (1 << JFFS2_XPREFIX_SECURITY);
					  break;
			case 1002:	/*  --with-posix-acl  */
					  enable_xattr |= (1 << JFFS2_XPREFIX_ACL_ACCESS)
						  | (1 << JFFS2_XPREFIX_ACL_DEFAULT);
					  break;
#endif
		}
	}
	if (warn_page_size) {
		errmsg("Page size for this system is by default %d", page_size);
		errmsg("Use the --pagesize=SIZE option if this is not what you want");
	}
	if (out_fd == -1) {
		if (isatty(1)) {
			errmsg_die("%s", helptext);
		}
		out_fd = 1;
	}

	if (chdir(rootdir))
		sys_errmsg_die("%s", rootdir);

	if (!(cwd = getcwd(0, GETCWD_SIZE)))
		sys_errmsg_die("getcwd failed");

	if(in_fd != -1)
		parse_image();

	root = recursive_add_host_directory(NULL, "/", cwd);

	if (devtable)
		parse_device_table(root, devtable);

	create_target_filesystem(root);

	cleanup(root);

	if (rootdir != default_rootdir)
		free(rootdir);

	close(out_fd);

	if (verbose) {
		char *s = jffs2_stats();
		fprintf(stderr,"\n\n%s",s);
		free(s);
	}
	if ((verbose)||(jffs2_compression_check_get()&&(jffs2_compression_check_errorcnt_get()))) {
		fprintf(stderr,"Compression errors: %d\n",jffs2_compression_check_errorcnt_get());
	}

	jffs2_compressors_exit();

	return 0;
} }}

    - name: /*
  Red Black Trees
  (C) 
  (C) >

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  linux/lib/rbtree.c
*/

#include <stdlib.h>
#include "rbtree.h"

static void __rb_rotate_left(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *right = node->rb_right;
	struct rb_node *parent = rb_parent(node);

	if ((node->rb_right = right->rb_left))
		rb_set_parent(right->rb_left, node);
	right->rb_left = node;

	rb_set_parent(right, parent);

	if (parent)
	{
		if (node == parent->rb_left)
			parent->rb_left = right;
		else
			parent->rb_right = right;
	}
	else
		root->rb_node = right;
	rb_set_parent(node, right);
}

static void __rb_rotate_right(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *left = node->rb_left;
	struct rb_node *parent = rb_parent(node);

	if ((node->rb_left = left->rb_right))
		rb_set_parent(left->rb_right, node);
	left->rb_right = node;

	rb_set_parent(left, parent);

	if (parent)
	{
		if (node == parent->rb_right)
			parent->rb_right = left;
		else
			parent->rb_left = left;
	}
	else
		root->rb_node = left;
	rb_set_parent(node, left);
}

void rb_insert_color(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *parent, *gparent;

	while ((parent = rb_parent(node)) && rb_is_red(parent))
	{
		gparent = rb_parent(parent);

		if (parent == gparent->rb_left)
		{
			{
				register struct rb_node *uncle = gparent->rb_right;
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			if (parent->rb_right == node)
			{
				register struct rb_node *tmp;
				__rb_rotate_left(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			rb_set_black(parent);
			rb_set_red(gparent);
			__rb_rotate_right(gparent, root);
		} else {
			{
				register struct rb_node *uncle = gparent->rb_left;
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			if (parent->rb_left == node)
			{
				register struct rb_node *tmp;
				__rb_rotate_right(parent, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			rb_set_black(parent);
			rb_set_red(gparent);
			__rb_rotate_left(gparent, root);
		}
	}

	rb_set_black(root->rb_node);
}

static void __rb_erase_color(struct rb_node *node, struct rb_node *parent,
			     struct rb_root *root)
{
	struct rb_node *other;

	while ((!node || rb_is_black(node)) && node != root->rb_node)
	{
		if (parent->rb_left == node)
		{
			other = parent->rb_right;
			if (rb_is_red(other))
			{
				rb_set_black(other);
				rb_set_red(parent);
				__rb_rotate_left(parent, root);
				other = parent->rb_right;
			}
			if ((!other->rb_left || rb_is_black(other->rb_left)) &&
			    (!other->rb_right || rb_is_black(other->rb_right)))
			{
				rb_set_red(other);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				if (!other->rb_right || rb_is_black(other->rb_right))
				{
					struct rb_node *o_left;
					if ((o_left = other->rb_left))
						rb_set_black(o_left);
					rb_set_red(other);
					__rb_rotate_right(other, root);
					other = parent->rb_right;
				}
				rb_set_color(other, rb_color(parent));
				rb_set_black(parent);
				if (other->rb_right)
					rb_set_black(other->rb_right);
				__rb_rotate_left(parent, root);
				node = root->rb_node;
				break;
			}
		}
		else
		{
			other = parent->rb_left;
			if (rb_is_red(other))
			{
				rb_set_black(other);
				rb_set_red(parent);
				__rb_rotate_right(parent, root);
				other = parent->rb_left;
			}
			if ((!other->rb_left || rb_is_black(other->rb_left)) &&
			    (!other->rb_right || rb_is_black(other->rb_right)))
			{
				rb_set_red(other);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				if (!other->rb_left || rb_is_black(other->rb_left))
				{
					register struct rb_node *o_right;
					if ((o_right = other->rb_right))
						rb_set_black(o_right);
					rb_set_red(other);
					__rb_rotate_left(other, root);
					other = parent->rb_left;
				}
				rb_set_color(other, rb_color(parent));
				rb_set_black(parent);
				if (other->rb_left)
					rb_set_black(other->rb_left);
				__rb_rotate_right(parent, root);
				node = root->rb_node;
				break;
			}
		}
	}
	if (node)
		rb_set_black(node);
}

void rb_erase(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *child, *parent;
	int color;

	if (!node->rb_left)
		child = node->rb_right;
	else if (!node->rb_right)
		child = node->rb_left;
	else
	{
		struct rb_node *old = node, *left;

		node = node->rb_right;
		while ((left = node->rb_left) != NULL)
			node = left;
		child = node->rb_right;
		parent = rb_parent(node);
		color = rb_color(node);

		if (child)
			rb_set_parent(child, parent);
		if (parent == old) {
			parent->rb_right = child;
			parent = node;
		} else
			parent->rb_left = child;

		node->rb_parent_color = old->rb_parent_color;
		node->rb_right = old->rb_right;
		node->rb_left = old->rb_left;

		if (rb_parent(old))
		{
			if (rb_parent(old)->rb_left == old)
				rb_parent(old)->rb_left = node;
			else
				rb_parent(old)->rb_right = node;
		} else
			root->rb_node = node;

		rb_set_parent(old->rb_left, node);
		if (old->rb_right)
			rb_set_parent(old->rb_right, node);
		goto color;
	}

	parent = rb_parent(node);
	color = rb_color(node);

	if (child)
		rb_set_parent(child, parent);
	if (parent)
	{
		if (parent->rb_left == node)
			parent->rb_left = child;
		else
			parent->rb_right = child;
	}
	else
		root->rb_node = child;

 color:
	if (color == RB_BLACK)
		__rb_erase_color(child, parent, root);
}

/*
 * This function returns the first node (in sort order) of the tree.
 */
struct rb_node *rb_first(struct rb_root *root)
{
	struct rb_node	*n;

	n = root->rb_node;
	if (!n)
		return NULL;
	while (n->rb_left)
		n = n->rb_left;
	return n;
}

struct rb_node *rb_last(struct rb_root *root)
{
	struct rb_node	*n;

	n = root->rb_node;
	if (!n)
		return NULL;
	while (n->rb_right)
		n = n->rb_right;
	return n;
}

struct rb_node *rb_next(struct rb_node *node)
{
	struct rb_node *parent;

	if (rb_parent(node) == node)
		return NULL;

	/* If we have a right-hand child, go down and then left as far
	   as we can. */
	if (node->rb_right) {
		node = node->rb_right;
		while (node->rb_left)
			node=node->rb_left;
		return node;
	}

	/* No right-hand children.  Everything down and left is
	   smaller than us, so any 'next' node must be in the general
	   direction of our parent. Go up the tree; any time the
	   ancestor is a right-hand child of its parent, keep going
	   up. First time it's a left-hand child of its parent, said
	   parent is our 'next' node. */
	while ((parent = rb_parent(node)) && node == parent->rb_right)
		node = parent;

	return parent;
}

struct rb_node *rb_prev(struct rb_node *node)
{
	struct rb_node *parent;

	if (rb_parent(node) == node)
		return NULL;

	/* If we have a left-hand child, go down and then right as far
	   as we can. */
	if (node->rb_left) {
		node = node->rb_left;
		while (node->rb_right)
			node=node->rb_right;
		return node;
	}

	/* No left-hand children. Go up till we find an ancestor which
	   is a right-hand child of its parent */
	while ((parent = rb_parent(node)) && node == parent->rb_left)
		node = parent;

	return parent;
}

void rb_replace_node(struct rb_node *victim, struct rb_node *new,
		     struct rb_root *root)
{
	struct rb_node *parent = rb_parent(victim);

	/* Set the surrounding nodes to point to the replacement */
	if (parent) {
		if (victim == parent->rb_left)
			parent->rb_left = new;
		else
			parent->rb_right = new;
	} else {
		root->rb_node = new;
	}
	if (victim->rb_left)
		rb_set_parent(victim->rb_left, new);
	if (victim->rb_right)
		rb_set_parent(victim->rb_right, new);

	/* Copy the pointers/colour from the victim to the replacement */
	*new = *victim;
}
      run: /*
  Red Black Trees
  (C) 1999  

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  linux/include/linux/rbtree.h

  To use rbtrees you'll have to implement your own insert and search cores.
  This will avoid us to use callbacks and to drop drammatically performances.
  I know it's not the cleaner way,  but in C (not in C++) to get
  performances and genericity...

  Some example of insert and search follows here. The search is a plain
  normal search over an ordered tree. The insert instead must be implemented
  int two steps: as first thing the code must insert the element in
  order as a red leaf in the tree, then the support library function
  rb_insert_color() must be called. Such function will do the
  not trivial work to rebalance the rbtree if necessary.

-----------------------------------------------------------------------
static inline struct page * rb_search_page_cache(struct inode * inode,
						 unsigned long offset)
{
	struct rb_node * n = inode->i_rb_page_cache.rb_node;
	struct page * page;

	while (n)
	{
		page = rb_entry(n, struct page, rb_page_cache);

		if (offset < page->offset)
			n = n->rb_left;
		else if (offset > page->offset)
			n = n->rb_right;
		else
			return page;
	}
	return NULL;
}

static inline struct page * __rb_insert_page_cache(struct inode * inode,
						   unsigned long offset,
						   struct rb_node * node)
{
	struct rb_node ** p = &inode->i_rb_page_cache.rb_node;
	struct rb_node * parent = NULL;
	struct page * page;

	while (*p)
	{
		parent = *p;
		page = rb_entry(parent, struct page, rb_page_cache);

		if (offset < page->offset)
			p = &(*p)->rb_left;
		else if (offset > page->offset)
			p = &(*p)->rb_right;
		else
			return page;
	}

	rb_link_node(node, parent, p);

	return NULL;
}

static inline struct page * rb_insert_page_cache(struct inode * inode,
						 unsigned long offset,
						 struct rb_node * node)
{
	struct page * ret;
	if ((ret = __rb_insert_page_cache(inode, offset, node)))
		goto out;
	rb_insert_color(node, &inode->i_rb_page_cache);
 out:
	return ret;
}
-----------------------------------------------------------------------
*/

#ifndef	_LINUX_RBTREE_H
#define	_LINUX_RBTREE_H

#include <linux/kernel.h>
#include <linux/stddef.h>

struct rb_node
{
	unsigned long  rb_parent_color;
#define	RB_RED		0
#define	RB_BLACK	1
	struct rb_node *rb_right;
	struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));
    /* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_root
{
	struct rb_node *rb_node;
};


#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)   ((r)->rb_parent_color & 1)
#define rb_is_red(r)   (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
	rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}
static inline void rb_set_color(struct rb_node *rb, int color)
{
	rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

#define RB_ROOT	(struct rb_root) { NULL, }

/* Newer gcc versions take care of exporting this */
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root)	((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)	(rb_parent(node) == node)
#define RB_CLEAR_NODE(node)	(rb_set_parent(node, node))

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(struct rb_node *);
extern struct rb_node *rb_prev(struct rb_node *);
extern struct rb_node *rb_first(struct rb_root *);
extern struct rb_node *rb_last(struct rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(struct rb_node *victim, struct rb_node *new,
			    struct rb_root *root);

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent,
				struct rb_node ** rb_link)
{
	node->rb_parent_color = (unsigned long )parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

#endif	/* _LINUX_RBTREE_H */
        

    # Deploy the Docker image to the TKE cluster
    - name: /*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2004  
 *                     
 *                     
 *                     University of Szeged, Hungary
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 */

#ifndef JFFS2_SUMMARY_H
#define JFFS2_SUMMARY_H

#include <linux/jffs2.h>

#define DIRTY_SPACE(x) do { typeof(x) _x = (x); \
	c->free_size -= _x; c->dirty_size += _x; \
	jeb->free_size -= _x ; jeb->dirty_size += _x; \
}while(0)
#define USED_SPACE(x) do { typeof(x) _x = (x); \
	c->free_size -= _x; c->used_size += _x; \
	jeb->free_size -= _x ; jeb->used_size += _x; \
}while(0)
#define WASTED_SPACE(x) do { typeof(x) _x = (x); \
	c->free_size -= _x; c->wasted_size += _x; \
	jeb->free_size -= _x ; jeb->wasted_size += _x; \
}while(0)
#define UNCHECKED_SPACE(x) do { typeof(x) _x = (x); \
	c->free_size -= _x; c->unchecked_size += _x; \
	jeb->free_size -= _x ; jeb->unchecked_size += _x; \
}while(0)

#define BLK_STATE_ALLFF		0
#define BLK_STATE_CLEAN		1
#define BLK_STATE_PARTDIRTY	2
#define BLK_STATE_CLEANMARKER	3
#define BLK_STATE_ALLDIRTY	4
#define BLK_STATE_BADBLOCK	5

#define JFFS2_SUMMARY_NOSUM_SIZE 0xffffffff
#define JFFS2_SUMMARY_INODE_SIZE (sizeof(struct jffs2_sum_inode_flash))
#define JFFS2_SUMMARY_DIRENT_SIZE(x) (sizeof(struct jffs2_sum_dirent_flash) + (x))
#define JFFS2_SUMMARY_XATTR_SIZE (sizeof(struct jffs2_sum_xattr_flash))
#define JFFS2_SUMMARY_XREF_SIZE (sizeof(struct jffs2_sum_xref_flash))

/* Summary structures used on flash */

struct jffs2_sum_unknown_flash
{
	jint16_t nodetype;	/* node type */
} __attribute__((packed));

struct jffs2_sum_inode_flash
{
	jint16_t nodetype;	/* node type */
	jint32_t inode;		/* inode number */
	jint32_t version;	/* inode version */
	jint32_t offset;	/* offset on jeb */
	jint32_t totlen; 	/* record length */
} __attribute__((packed));

struct jffs2_sum_dirent_flash
{
	jint16_t nodetype;	/* == JFFS_NODETYPE_DIRENT */
	jint32_t totlen;	/* record length */
	jint32_t offset;	/* ofset on jeb */
	jint32_t pino;		/* parent inode */
	jint32_t version;	/* dirent version */
	jint32_t ino; 		/* == zero for unlink */
	uint8_t nsize;		/* dirent name size */
	uint8_t type;		/* dirent type */
	uint8_t name[0];	/* dirent name */
} __attribute__((packed));

struct jffs2_sum_xattr_flash
{
	jint16_t nodetype;	/* == JFFS2_NODETYPE_XATR */
	jint32_t xid;		/* xattr identifier */
	jint32_t version;	/* version number */
	jint32_t offset;	/* offset on jeb */
	jint32_t totlen;	/* node length */
} __attribute__((packed));

struct jffs2_sum_xref_flash
{
	jint16_t nodetype;	/* == JFFS2_NODETYPE_XREF */
	jint32_t offset;	/* offset on jeb */
} __attribute__((packed));

union jffs2_sum_flash
{
	struct jffs2_sum_unknown_flash u;
	struct jffs2_sum_inode_flash i;
	struct jffs2_sum_dirent_flash d;
	struct jffs2_sum_xattr_flash x;
	struct jffs2_sum_xref_flash r;
};

/* Summary structures used in the memory */

struct jffs2_sum_unknown_mem
{
	union jffs2_sum_mem *next;
	jint16_t nodetype;	/* node type */
} __attribute__((packed));

struct jffs2_sum_inode_mem
{
	union jffs2_sum_mem *next;
	jint16_t nodetype;	/* node type */
	jint32_t inode;		/* inode number */
	jint32_t version;	/* inode version */
	jint32_t offset;	/* offset on jeb */
	jint32_t totlen; 	/* record length */
} __attribute__((packed));

struct jffs2_sum_dirent_mem
{
	union jffs2_sum_mem *next;
	jint16_t nodetype;	/* == JFFS_NODETYPE_DIRENT */
	jint32_t totlen;	/* record length */
	jint32_t offset;	/* ofset on jeb */
	jint32_t pino;		/* parent inode */
	jint32_t version;	/* dirent version */
	jint32_t ino; 		/* == zero for unlink */
	uint8_t nsize;		/* dirent name size */
	uint8_t type;		/* dirent type */
	uint8_t name[0];	/* dirent name */
} __attribute__((packed));

struct jffs2_sum_xattr_mem
{
	union jffs2_sum_mem *next;
	jint16_t nodetype;
	jint32_t xid;
	jint32_t version;
	jint32_t offset;
	jint32_t totlen;
} __attribute__((packed));

struct jffs2_sum_xref_mem
{
	union jffs2_sum_mem *next;
	jint16_t nodetype;
	jint32_t offset;
} __attribute__((packed));

union jffs2_sum_mem
{
	struct jffs2_sum_unknown_mem u;
	struct jffs2_sum_inode_mem i;
	struct jffs2_sum_dirent_mem d;
	struct jffs2_sum_xattr_mem x;
	struct jffs2_sum_xref_mem r;
};

struct jffs2_summary
{
	uint32_t sum_size;
	uint32_t sum_num;
	uint32_t sum_padded;
	union jffs2_sum_mem *sum_list_head;
	union jffs2_sum_mem *sum_list_tail;
};

/* Summary marker is stored at the end of every sumarized erase block */

struct jffs2_sum_marker
{
	jint32_t offset;	/* offset of the summary node in the jeb */
	jint32_t magic; 	/* == JFFS2_SUM_MAGIC */
};

#define JFFS2_SUMMARY_FRAME_SIZE (sizeof(struct jffs2_raw_summary) + sizeof(struct jffs2_sum_marker))

#endif
      run: /*
 *  sumtool.c
 *
 *  Copyright (C) 2004 
 *                     
 *                     University of Szeged, Hungary
 *                2006
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Overview:
 *   This is a utility insert summary information into JFFS2 image for
 *   faster mount time
 *
 */

#define PROGRAM_NAME "sumtool"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <asm/types.h>
#include <dirent.h>
#include <mtd/jffs2-user.h>
#include <endian.h>
#include <byteswap.h>
#include <getopt.h>
#include <crc32.h>
#include "summary.h"
#include "common.h"

#define PAD(x) (((x)+3)&~3)

static struct jffs2_summary *sum_collected = NULL;

static int verbose = 0;
static int padto = 0;				/* pad the output with 0xFF to the end of the final eraseblock */
static int add_cleanmarkers = 1;		/* add cleanmarker to output */
static int use_input_cleanmarker_size = 1;	/* use input file's cleanmarker size (default) */
static int found_cleanmarkers = 0;		/* cleanmarker found in input file */
static struct jffs2_unknown_node cleanmarker;
static int cleanmarker_size = sizeof(cleanmarker);
static const char *short_options = "o:i:e:hvVblnc:p";
static int erase_block_size = 65536;
static int out_fd = -1;
static int in_fd = -1;

static uint8_t *data_buffer = NULL; 		/* buffer for inodes */
static unsigned int data_ofs = 0;	 	/* inode buffer offset */

static uint8_t *file_buffer = NULL;		/* file buffer contains the actual erase block*/
static unsigned int file_ofs = 0;		/* position in the buffer */

int target_endian = __BYTE_ORDER;

static struct option long_options[] = {
	{"output", 1, NULL, 'o'},
	{"input", 1, NULL, 'i'},
	{"eraseblock", 1, NULL, 'e'},
	{"help", 0, NULL, 'h'},
	{"verbose", 0, NULL, 'v'},
	{"version", 0, NULL, 'V'},
	{"bigendian", 0, NULL, 'b'},
	{"littleendian", 0, NULL, 'l'},
	{"no-cleanmarkers", 0, NULL, 'n'},
	{"cleanmarker", 1, NULL, 'c'},
	{"pad", 0, NULL, 'p'},
	{NULL, 0, NULL, 0}
};

static const char helptext[] =
"Usage: sumtool [OPTIONS] -i inputfile -o outputfile\n\n"
"Convert the input JFFS2 image to a summarized JFFS2 image\n"
"Summary makes mounting faster - if summary support enabled in your kernel\n\n"
"Options:\n"
"  -e, --eraseblock=SIZE     Use erase block size SIZE (default: 64KiB)\n"
"                            (usually 16KiB on NAND)\n"
"  -c, --cleanmarker=SIZE    Size of cleanmarker (default 12).\n"
"                            (usually 16 bytes on NAND, and will be set to\n"
"                            this value if left at the default 12). Will be\n"
"                            stored in OOB after each physical page composing\n"
"                            a physical eraseblock.\n"
"  -n, --no-cleanmarkers     Don't add a cleanmarker to every eraseblock\n"
"  -o, --output=FILE         Output to FILE \n"
"  -i, --input=FILE          Input from FILE \n"
"  -b, --bigendian           Image is big endian\n"
"  -l  --littleendian        Image is little endian\n"
"  -h, --help                Display this help text\n"
"  -v, --verbose             Verbose operation\n"
"  -V, --version             Display version information\n"
"  -p, --pad                 Pad the OUTPUT with 0xFF to the end of the final\n"
"                            eraseblock\n\n";


static unsigned char ffbuf[16] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static void full_write(void *target_buff, const void *buf, int len);

static void setup_cleanmarker(void)
{
	cleanmarker.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	cleanmarker.nodetype = cpu_to_je16(JFFS2_NODETYPE_CLEANMARKER);
	cleanmarker.totlen = cpu_to_je32(cleanmarker_size);
	cleanmarker.hdr_crc = cpu_to_je32(mtd_crc32(0, &cleanmarker, sizeof(struct jffs2_unknown_node)-4));
}

static void process_options (int argc, char **argv)
{
	int opt,c;

	while ((opt = getopt_long(argc, argv, short_options, long_options, &c)) >= 0) {
		switch (opt) {
			case 'o':
				if (out_fd != -1)
					errmsg_die("output filename specified more than once");
				out_fd = open(optarg, O_CREAT | O_TRUNC | O_RDWR, 0644);
				if (out_fd == -1)
					sys_errmsg_die("open output file");
				break;

			case 'i':
				if (in_fd != -1)
					errmsg_die("input filename specified more than once");
				in_fd = open(optarg, O_RDONLY);
				if (in_fd == -1)
					sys_errmsg_die("open input file");
				break;
			case 'b':
				target_endian = __BIG_ENDIAN;
				break;
			case 'l':
				target_endian = __LITTLE_ENDIAN;
				break;
			case 'h':
				puts(helptext);
				exit(EXIT_SUCCESS);
			case '?':
				puts(helptext);
				exit(EXIT_FAILURE);
			case 'v':
				verbose = 1;
				break;

			case 'V':
				common_print_version();
				exit(EXIT_SUCCESS);

			case 'e': {
						  char *next;
						  unsigned units = 0;
						  erase_block_size = strtol(optarg, &next, 0);
						  if (!erase_block_size)
							  errmsg_die("Unrecognisable erase size\n");

						  if (*next) {
							  if (!strcmp(next, "KiB")) {
								  units = 1024;
							  } else if (!strcmp(next, "MiB")) {
								  units = 1024 * 1024;
							  } else {
								  errmsg_die("Unknown units in erasesize\n");
							  }
						  } else {
							  if (erase_block_size < 0x1000)
								  units = 1024;
							  else
								  units = 1;
						  }
						  erase_block_size *= units;

						  /* If it's less than 8KiB, they're not allowed */
						  if (erase_block_size < 0x2000) {
							  warnmsg("Erase size 0x%x too small. Increasing to 8KiB minimum\n",
									erase_block_size);
							  erase_block_size = 0x2000;
						  }
						  break;
					  }

			case 'n':
					  add_cleanmarkers = 0;
					  break;
			case 'c':
					  cleanmarker_size = strtol(optarg, NULL, 0);

					  if (cleanmarker_size < sizeof(cleanmarker)) {
						  errmsg_die("cleanmarker size must be >= 12");
					  }
					  if (cleanmarker_size >= erase_block_size) {
						  errmsg_die("cleanmarker size must be < eraseblock size");
					  }

					  use_input_cleanmarker_size = 0;
					  found_cleanmarkers = 1;
					  setup_cleanmarker();

					  break;
			case 'p':
					  padto = 1;
					  break;
		}
	}
}


static void init_buffers(void)
{
	data_buffer = xmalloc(erase_block_size);
	file_buffer = xmalloc(erase_block_size);
}

static void init_sumlist(void)
{
	sum_collected = xzalloc(sizeof(*sum_collected));
}

static void clean_buffers(void)
{
	free(data_buffer);
	free(file_buffer);
}

static void clean_sumlist(void)
{
	union jffs2_sum_mem *temp;

	if (sum_collected) {

		while (sum_collected->sum_list_head) {
			temp = sum_collected->sum_list_head;
			sum_collected->sum_list_head = sum_collected->sum_list_head->u.next;
			free(temp);
			sum_collected->sum_num--;
		}

		if (sum_collected->sum_num != 0)
			warnmsg("Ooops, something wrong happened! sum_num != 0, but sum_list = null ???");

		free(sum_collected);
	}
}

static int load_next_block(void)
{
	int ret;
	ret = read(in_fd, file_buffer, erase_block_size);
	file_ofs = 0;

	bareverbose(verbose, "Load next block : %d bytes read\n", ret);

	return ret;
}

static void write_buff_to_file(void)
{
	int ret;
	int len = data_ofs;

	uint8_t *buf = NULL;

	buf = data_buffer;
	while (len > 0) {
		ret = write(out_fd, buf, len);

		if (ret < 0)
			sys_errmsg_die("write");

		if (ret == 0)
			sys_errmsg_die("write returned zero");

		len -= ret;
		buf += ret;
	}

	data_ofs = 0;
}

static void dump_sum_records(void)
{

	struct jffs2_raw_summary isum;
	struct jffs2_sum_marker *sm;
	union jffs2_sum_mem *temp;
	jint32_t offset;
	jint32_t *tpage;
	void *wpage;
	int datasize, infosize, padsize;
	jint32_t magic = cpu_to_je32(JFFS2_SUM_MAGIC);

	if (!sum_collected->sum_num || !sum_collected->sum_list_head)
		return;

	datasize = sum_collected->sum_size + sizeof(struct jffs2_sum_marker);
	infosize = sizeof(struct jffs2_raw_summary) + datasize;
	padsize = erase_block_size - data_ofs - infosize;
	infosize += padsize; datasize += padsize;
	offset = cpu_to_je32(data_ofs);

	tpage = xmalloc(datasize);

	memset(tpage, 0xff, datasize);
	memset(&isum, 0, sizeof(isum));

	isum.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	isum.nodetype = cpu_to_je16(JFFS2_NODETYPE_SUMMARY);
	isum.totlen = cpu_to_je32(infosize);
	isum.hdr_crc = cpu_to_je32(mtd_crc32(0, &isum, sizeof(struct jffs2_unknown_node) - 4));
	isum.padded = cpu_to_je32(0);

	if (add_cleanmarkers && found_cleanmarkers) {
		isum.cln_mkr = cpu_to_je32(cleanmarker_size);
	} else {
		isum.cln_mkr = cpu_to_je32(0);
	}

	isum.sum_num = cpu_to_je32(sum_collected->sum_num);
	wpage = tpage;

	while (sum_collected->sum_num) {
		switch(je16_to_cpu(sum_collected->sum_list_head->u.nodetype)) {

			case JFFS2_NODETYPE_INODE : {
											struct jffs2_sum_inode_flash *sino_ptr = wpage;

											sino_ptr->nodetype = sum_collected->sum_list_head->i.nodetype;
											sino_ptr->inode = sum_collected->sum_list_head->i.inode;
											sino_ptr->version = sum_collected->sum_list_head->i.version;
											sino_ptr->offset = sum_collected->sum_list_head->i.offset;
											sino_ptr->totlen = sum_collected->sum_list_head->i.totlen;

											wpage += JFFS2_SUMMARY_INODE_SIZE;
											break;
										}

			case JFFS2_NODETYPE_DIRENT : {
											 struct jffs2_sum_dirent_flash *sdrnt_ptr = wpage;

											 sdrnt_ptr->nodetype = sum_collected->sum_list_head->d.nodetype;
											 sdrnt_ptr->totlen = sum_collected->sum_list_head->d.totlen;
											 sdrnt_ptr->offset = sum_collected->sum_list_head->d.offset;
											 sdrnt_ptr->pino = sum_collected->sum_list_head->d.pino;
											 sdrnt_ptr->version = sum_collected->sum_list_head->d.version;
											 sdrnt_ptr->ino = sum_collected->sum_list_head->d.ino;
											 sdrnt_ptr->nsize = sum_collected->sum_list_head->d.nsize;
											 sdrnt_ptr->type = sum_collected->sum_list_head->d.type;

											 memcpy(sdrnt_ptr->name, sum_collected->sum_list_head->d.name,
													 sum_collected->sum_list_head->d.nsize);

											 wpage += JFFS2_SUMMARY_DIRENT_SIZE(sum_collected->sum_list_head->d.nsize);
											 break;
										 }

			case JFFS2_NODETYPE_XATTR: {
										   struct jffs2_sum_xattr_flash *sxattr_ptr = wpage;

										   sxattr_ptr->nodetype = sum_collected->sum_list_head->x.nodetype;
										   sxattr_ptr->xid = sum_collected->sum_list_head->x.xid;
										   sxattr_ptr->version = sum_collected->sum_list_head->x.version;
										   sxattr_ptr->offset = sum_collected->sum_list_head->x.offset;
										   sxattr_ptr->totlen = sum_collected->sum_list_head->x.totlen;

										   wpage += JFFS2_SUMMARY_XATTR_SIZE;
										   break;
									   }

			case JFFS2_NODETYPE_XREF: {
										  struct jffs2_sum_xref_flash *sxref_ptr = wpage;

										  sxref_ptr->nodetype = sum_collected->sum_list_head->r.nodetype;
										  sxref_ptr->offset = sum_collected->sum_list_head->r.offset;

										  wpage += JFFS2_SUMMARY_XREF_SIZE;
										  break;
									  }

			default : {
						  warnmsg("Unknown node type!\n");
					  }
		}

		temp = sum_collected->sum_list_head;
		sum_collected->sum_list_head = sum_collected->sum_list_head->u.next;
		free(temp);

		sum_collected->sum_num--;
	}

	sum_collected->sum_size = 0;
	sum_collected->sum_num = 0;
	sum_collected->sum_list_tail = NULL;

	wpage += padsize;

	sm = wpage;
	sm->offset = offset;
	sm->magic = magic;

	isum.sum_crc = cpu_to_je32(mtd_crc32(0, tpage, datasize));
	isum.node_crc = cpu_to_je32(mtd_crc32(0, &isum, sizeof(isum) - 8));

	full_write(data_buffer + data_ofs, &isum, sizeof(isum));
	full_write(data_buffer + data_ofs, tpage, datasize);

	free(tpage);
}

static void full_write(void *target_buff, const void *buf, int len)
{
	memcpy(target_buff, buf, len);
	data_ofs += len;
}

static void pad(int req)
{
	while (req) {
		if (req > sizeof(ffbuf)) {
			full_write(data_buffer + data_ofs, ffbuf, sizeof(ffbuf));
			req -= sizeof(ffbuf);
		} else {
			full_write(data_buffer + data_ofs, ffbuf, req);
			req = 0;
		}
	}
}

static inline void padword(void)
{
	if (data_ofs % 4)
		full_write(data_buffer + data_ofs, ffbuf, 4 - (data_ofs % 4));
}


static inline void pad_block_if_less_than(int req,int plus)
{

	int datasize = req + plus + sum_collected->sum_size + sizeof(struct jffs2_raw_summary) + 8;
	datasize += (4 - (datasize % 4)) % 4;

	if (data_ofs + req > erase_block_size - datasize) {
		dump_sum_records();
		write_buff_to_file();
	}

	if (add_cleanmarkers && found_cleanmarkers) {
		if (!data_ofs) {
			full_write(data_buffer, &cleanmarker, sizeof(cleanmarker));
			pad(cleanmarker_size - sizeof(cleanmarker));
			padword();
		}
	}
}

static void flush_buffers(void)
{

	if ((add_cleanmarkers == 1) && (found_cleanmarkers == 1)) { /* CLEANMARKER */
		if (data_ofs != cleanmarker_size) {	/* INODE BUFFER */

			int datasize = sum_collected->sum_size + sizeof(struct jffs2_raw_summary) + 8;
			datasize += (4 - (datasize % 4)) % 4;

			/* If we have a full inode buffer, then write out inode and summary data  */
			if (data_ofs + sizeof(struct jffs2_raw_inode) + 2*JFFS2_MIN_DATA_LEN > erase_block_size - datasize) {
				dump_sum_records();
				write_buff_to_file();
			} else {	/* else just write out inode data */
				if (padto)
					pad(erase_block_size - data_ofs);
				write_buff_to_file();
			}
		}
	} else { /* NO CLEANMARKER */
		if (data_ofs != 0) { /* INODE BUFFER */

			int datasize = sum_collected->sum_size + sizeof(struct jffs2_raw_summary) + 8;
			datasize += (4 - (datasize % 4)) % 4;

			/* If we have a full inode buffer, then write out inode and summary data */
			if (data_ofs + sizeof(struct jffs2_raw_inode) + 2*JFFS2_MIN_DATA_LEN > erase_block_size - datasize) {
				dump_sum_records();
				write_buff_to_file();
			} else {	/* Else just write out inode data */
				if(padto)
					pad(erase_block_size - data_ofs);
				write_buff_to_file();
			}
		}
	}
}

static int add_sum_mem(union jffs2_sum_mem *item)
{

	if (!sum_collected->sum_list_head)
		sum_collected->sum_list_head = (union jffs2_sum_mem *) item;
	if (sum_collected->sum_list_tail)
		sum_collected->sum_list_tail->u.next = (union jffs2_sum_mem *) item;
	sum_collected->sum_list_tail = (union jffs2_sum_mem *) item;

	switch (je16_to_cpu(item->u.nodetype)) {
		case JFFS2_NODETYPE_INODE:
			sum_collected->sum_size += JFFS2_SUMMARY_INODE_SIZE;
			sum_collected->sum_num++;
			break;

		case JFFS2_NODETYPE_DIRENT:
			sum_collected->sum_size += JFFS2_SUMMARY_DIRENT_SIZE(item->d.nsize);
			sum_collected->sum_num++;
			break;

		case JFFS2_NODETYPE_XATTR:
			sum_collected->sum_size += JFFS2_SUMMARY_XATTR_SIZE;
			sum_collected->sum_num++;
			break;

		case JFFS2_NODETYPE_XREF:
			sum_collected->sum_size += JFFS2_SUMMARY_XREF_SIZE;
			sum_collected->sum_num++;
			break;

		default:
			errmsg_die("__jffs2_add_sum_mem(): UNKNOWN node type %d\n", je16_to_cpu(item->u.nodetype));
	}
	return 0;
}

static void add_sum_inode_mem(union jffs2_node_union *node)
{
	struct jffs2_sum_inode_mem *temp = xmalloc(sizeof(*temp));

	temp->nodetype = node->i.nodetype;
	temp->inode = node->i.ino;
	temp->version = node->i.version;
	temp->offset = cpu_to_je32(data_ofs);
	temp->totlen = node->i.totlen;
	temp->next = NULL;

	add_sum_mem((union jffs2_sum_mem *) temp);
}

static void add_sum_dirent_mem(union jffs2_node_union *node)
{
	struct jffs2_sum_dirent_mem *temp = xmalloc(sizeof(*temp) + node->d.nsize);

	temp->nodetype = node->d.nodetype;
	temp->totlen = node->d.totlen;
	temp->offset = cpu_to_je32(data_ofs);
	temp->pino = node->d.pino;
	temp->version = node->d.version;
	temp->ino = node->d.ino;
	temp->nsize = node->d.nsize;
	temp->type = node->d.type;
	temp->next = NULL;

	memcpy(temp->name,node->d.name,node->d.nsize);
	add_sum_mem((union jffs2_sum_mem *) temp);
}

static void add_sum_xattr_mem(union jffs2_node_union *node)
{
	struct jffs2_sum_xattr_mem *temp = xmalloc(sizeof(*temp));

	temp->nodetype = node->x.nodetype;
	temp->xid = node->x.xid;
	temp->version = node->x.version;
	temp->offset = cpu_to_je32(data_ofs);
	temp->totlen = node->x.totlen;
	temp->next = NULL;

	add_sum_mem((union jffs2_sum_mem *) temp);
}

static void add_sum_xref_mem(union jffs2_node_union *node)
{
	struct jffs2_sum_xref_mem *temp = xmalloc(sizeof(*temp));

	temp->nodetype = node->r.nodetype;
	temp->offset = cpu_to_je32(data_ofs);
	temp->next = NULL;

	add_sum_mem((union jffs2_sum_mem *) temp);
}

static void write_dirent_to_buff(union jffs2_node_union *node)
{
	pad_block_if_less_than(je32_to_cpu (node->d.totlen),JFFS2_SUMMARY_DIRENT_SIZE(node->d.nsize));
	add_sum_dirent_mem(node);
	full_write(data_buffer + data_ofs, &(node->d), je32_to_cpu (node->d.totlen));
	padword();
}


static void write_inode_to_buff(union jffs2_node_union *node)
{
	pad_block_if_less_than(je32_to_cpu (node->i.totlen),JFFS2_SUMMARY_INODE_SIZE);
	add_sum_inode_mem(node);	/* Add inode summary mem to summary list */
	full_write(data_buffer + data_ofs, &(node->i), je32_to_cpu (node->i.totlen));	/* Write out the inode to inode_buffer */
	padword();
}

static void write_xattr_to_buff(union jffs2_node_union *node)
{
	pad_block_if_less_than(je32_to_cpu(node->x.totlen), JFFS2_SUMMARY_XATTR_SIZE);
	add_sum_xattr_mem(node);	/* Add xdatum summary mem to summary list */
	full_write(data_buffer + data_ofs, &(node->x), je32_to_cpu(node->x.totlen));
	padword();
}

static void write_xref_to_buff(union jffs2_node_union *node)
{
	pad_block_if_less_than(je32_to_cpu(node->r.totlen), JFFS2_SUMMARY_XREF_SIZE);
	add_sum_xref_mem(node);		/* Add xref summary mem to summary list */
	full_write(data_buffer + data_ofs, &(node->r), je32_to_cpu(node->r.totlen));
	padword();
}

static void create_summed_image(int inp_size)
{
	uint8_t *p = file_buffer;
	union jffs2_node_union *node;
	uint32_t crc, length;
	uint16_t type;
	int bitchbitmask = 0;
	int obsolete;
	char name[256];

	while ( p < (file_buffer + inp_size)) {

		node = (union jffs2_node_union *) p;

		/* Skip empty space */
		if (je16_to_cpu (node->u.magic) == 0xFFFF && je16_to_cpu (node->u.nodetype) == 0xFFFF) {
			p += 4;
			continue;
		}

		if (je16_to_cpu (node->u.magic) != JFFS2_MAGIC_BITMASK) {
			if (!bitchbitmask++)
				warnmsg("Wrong bitmask  at  0x%08zx, 0x%04x\n",
					p - file_buffer, je16_to_cpu (node->u.magic));
			p += 4;
			continue;
		}

		bitchbitmask = 0;

		type = je16_to_cpu(node->u.nodetype);
		if ((type & JFFS2_NODE_ACCURATE) != JFFS2_NODE_ACCURATE) {
			obsolete = 1;
			type |= JFFS2_NODE_ACCURATE;
		} else {
			obsolete = 0;
		}

		node->u.nodetype = cpu_to_je16(type);

		crc = mtd_crc32 (0, node, sizeof (struct jffs2_unknown_node) - 4);
		if (crc != je32_to_cpu (node->u.hdr_crc)) {
			warnmsg("Wrong hdr_crc  at  0x%08zx, 0x%08x instead of 0x%08x\n",
				p - file_buffer, je32_to_cpu (node->u.hdr_crc), crc);
			p += 4;
			continue;
		}

		switch(je16_to_cpu(node->u.nodetype)) {
			case JFFS2_NODETYPE_INODE:
				bareverbose(verbose,
					"%8s Inode      node at 0x%08zx, totlen 0x%08x, #ino  %5d, version %5d, isize %8d, csize %8d, dsize %8d, offset %8d\n",
					obsolete ? "Obsolete" : "",
					p - file_buffer, je32_to_cpu (node->i.totlen), je32_to_cpu (node->i.ino),
					je32_to_cpu (node->i.version), je32_to_cpu (node->i.isize),
					je32_to_cpu (node->i.csize), je32_to_cpu (node->i.dsize), je32_to_cpu (node->i.offset));

				crc = mtd_crc32 (0, node, sizeof (struct jffs2_raw_inode) - 8);
				if (crc != je32_to_cpu (node->i.node_crc)) {
					warnmsg("Wrong node_crc at  0x%08zx, 0x%08x instead of 0x%08x\n",
						p - file_buffer, je32_to_cpu (node->i.node_crc), crc);
					p += PAD(je32_to_cpu (node->i.totlen));
					continue;
				}

				crc = mtd_crc32(0, p + sizeof (struct jffs2_raw_inode), je32_to_cpu(node->i.csize));
				if (crc != je32_to_cpu(node->i.data_crc)) {
					warnmsg("Wrong data_crc at  0x%08zx, 0x%08x instead of 0x%08x\n",
						p - file_buffer, je32_to_cpu (node->i.data_crc), crc);
					p += PAD(je32_to_cpu (node->i.totlen));
					continue;
				}

				write_inode_to_buff(node);

				p += PAD(je32_to_cpu (node->i.totlen));
				break;

			case JFFS2_NODETYPE_DIRENT:
				memcpy (name, node->d.name, node->d.nsize);
				name [node->d.nsize] = 0x0;

				bareverbose(verbose,
					"%8s Dirent     node at 0x%08zx, totlen 0x%08x, #pino %5d, version %5d, #ino  %8d, nsize %8d, name %s\n",
					obsolete ? "Obsolete" : "",
					p - file_buffer, je32_to_cpu (node->d.totlen), je32_to_cpu (node->d.pino),
					je32_to_cpu (node->d.version), je32_to_cpu (node->d.ino),
					node->d.nsize, name);

				crc = mtd_crc32 (0, node, sizeof (struct jffs2_raw_dirent) - 8);
				if (crc != je32_to_cpu (node->d.node_crc)) {
					warnmsg("Wrong node_crc at  0x%08zx, 0x%08x instead of 0x%08x\n",
						p - file_buffer, je32_to_cpu (node->d.node_crc), crc);
					p += PAD(je32_to_cpu (node->d.totlen));
					continue;
				}

				crc = mtd_crc32(0, p + sizeof (struct jffs2_raw_dirent), node->d.nsize);
				if (crc != je32_to_cpu(node->d.name_crc)) {
					warnmsg("Wrong name_crc at  0x%08zx, 0x%08x instead of 0x%08x\n",
						p - file_buffer, je32_to_cpu (node->d.name_crc), crc);
					p += PAD(je32_to_cpu (node->d.totlen));
					continue;
				}

				write_dirent_to_buff(node);

				p += PAD(je32_to_cpu (node->d.totlen));
				break;

			case JFFS2_NODETYPE_XATTR:
				if (je32_to_cpu(node->x.node_crc) == 0xffffffff)
					obsolete = 1;
				bareverbose(verbose,
					"%8s Xdatum     node at 0x%08zx, totlen 0x%08x, #xid  %5u, version %5u\n",
					obsolete ? "Obsolete" : "",
					p - file_buffer, je32_to_cpu (node->x.totlen),
					je32_to_cpu(node->x.xid), je32_to_cpu(node->x.version));
				crc = mtd_crc32(0, node, sizeof (struct jffs2_raw_xattr) - 4);
				if (crc != je32_to_cpu(node->x.node_crc)) {
					warnmsg("Wrong node_crc at 0x%08zx, 0x%08x instead of 0x%08x\n",
							p - file_buffer, je32_to_cpu(node->x.node_crc), crc);
					p += PAD(je32_to_cpu (node->x.totlen));
					continue;
				}
				length = node->x.name_len + 1 + je16_to_cpu(node->x.value_len);
				crc = mtd_crc32(0, node->x.data, length);
				if (crc != je32_to_cpu(node->x.data_crc)) {
					warnmsg("Wrong data_crc at 0x%08zx, 0x%08x instead of 0x%08x\n",
							p - file_buffer, je32_to_cpu(node->x.data_crc), crc);
					p += PAD(je32_to_cpu (node->x.totlen));
					continue;
				}

				write_xattr_to_buff(node);
				p += PAD(je32_to_cpu (node->x.totlen));
				break;

			case JFFS2_NODETYPE_XREF:
				if (je32_to_cpu(node->r.node_crc) == 0xffffffff)
					obsolete = 1;
				bareverbose(verbose,
					"%8s Xref       node at 0x%08zx, totlen 0x%08x, #ino  %5u, xid     %5u\n",
					obsolete ? "Obsolete" : "",
					p - file_buffer, je32_to_cpu(node->r.totlen),
					je32_to_cpu(node->r.ino), je32_to_cpu(node->r.xid));
				crc = mtd_crc32(0, node, sizeof (struct jffs2_raw_xref) - 4);
				if (crc != je32_to_cpu(node->r.node_crc)) {
					warnmsg("Wrong node_crc at 0x%08zx, 0x%08x instead of 0x%08x\n",
							p - file_buffer, je32_to_cpu(node->r.node_crc), crc);
					p += PAD(je32_to_cpu (node->r.totlen));
					continue;
				}

				write_xref_to_buff(node);
				p += PAD(je32_to_cpu (node->r.totlen));
				break;

			case JFFS2_NODETYPE_CLEANMARKER:
				bareverbose(verbose,
					"%8s Cleanmarker     at 0x%08zx, totlen 0x%08x\n",
					obsolete ? "Obsolete" : "",
					p - file_buffer, je32_to_cpu (node->u.totlen));

				if (!found_cleanmarkers) {
					found_cleanmarkers = 1;

					if (add_cleanmarkers == 1 && use_input_cleanmarker_size == 1){
						cleanmarker_size = je32_to_cpu (node->u.totlen);
						setup_cleanmarker();
					}
				}

				p += PAD(je32_to_cpu (node->u.totlen));
				break;

			case JFFS2_NODETYPE_PADDING:
				bareverbose(verbose,
					"%8s Padding    node at 0x%08zx, totlen 0x%08x\n",
					obsolete ? "Obsolete" : "",
					p - file_buffer, je32_to_cpu (node->u.totlen));
				p += PAD(je32_to_cpu (node->u.totlen));
				break;

			case 0xffff:
				p += 4;
				break;

			default:
				bareverbose(verbose,
					"%8s Unknown    node at 0x%08zx, totlen 0x%08x\n",
					obsolete ? "Obsolete" : "",
					p - file_buffer, je32_to_cpu (node->u.totlen));

				p += PAD(je32_to_cpu (node->u.totlen));
		}
	}
}

int main(int argc, char **argv)
{
	int ret;

	process_options(argc,argv);

	if ((in_fd == -1) || (out_fd == -1)) {
		if(in_fd != -1)
			close(in_fd);
		if(out_fd != -1)
			close(out_fd);
		fprintf(stderr, "%s", helptext);
		errmsg_die("You must specify input and output files!\n");
	}

	init_buffers();
	init_sumlist();

	while ((ret = load_next_block())) {
		create_summed_image(ret);
	}

	flush_buffers();
	clean_buffers();
	clean_sumlist();

	if (in_fd != -1)
		close(in_fd);
	if (out_fd != -1)
		close(out_fd);

	return 0;
}
