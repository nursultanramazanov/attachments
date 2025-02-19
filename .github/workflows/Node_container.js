# This workflow will do a clean installation of node dependencies, cache/restore them, build the source code and run tests across different versions of node
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-nodejs

name: Node.js CI

on: nanddump_SOURCES = nand-utils/nanddump.c
nanddump_LDADD = libmtd.a

nandwrite_SOURCES = nand-utils/nandwrite.c
nandwrite_LDADD = libmtd.a

nandtest_SOURCES = nand-utils/nandtest.c
nandtest_LDADD = libmtd.a

nftldump_SOURCES = nand-utils/nftldump.c include/mtd_swab.h
nftldump_SOURCES += include/mtd/nftl-user.h include/mtd/ftl-user.h
nftldump_LDADD = libmtd.a

nftl_format_SOURCES = nand-utils/nftl_format.c include/mtd_swab.h
nftl_format_SOURCES += include/mtd/nftl-user.h include/mtd/ftl-user.h
nftl_format_LDADD = libmtd.a

nandflipbits_SOURCES = nand-utils/nandflipbits.c
nandflipbits_LDADD = libmtd.a

NAND_SH = \
        nand-utils/load_nandsim.sh

EXTRA_DIST += $(NAND_SH)

sbin_PROGRAMS += nanddump nandwrite nandtest nftldump nftl_format nandflipbits

if BUILD_TESTS
test_SCRIPTS += $(NAND_SH)
endif 
  push: #!/bin/sh -euf

#
# This script inserts NAND simulator module to emulate NAND flash of specified
# size.
#
# Author: RamazanovNursultan
#

fatal()
{
        echo "Error: $1" 1>&2
        exit 1
}

usage()
{
        cat 1>&2 <<EOF
Load NAND simulator to simulate flash of a specified size.

Usage: ${0##*/} <size in MiB> <eraseblock size in KiB> \\
       <page size (512 or 2048)>

Only the first parameter is mandatory. Default eraseblock size
is 16KiB, default NAND page size is 512 bytes.

Only the following combinations are supported:
--------------------------------------------------
| size (MiB) | EB size (KiB) | Page size (bytes) |
--------------------------------------------------
| 16         | 16            | 512               |
| 32         | 16            | 512               |
| 64         | 16            | 512               |
| 128        | 16            | 512               |
| 256        | 16            | 512               |
| 64         | 64            | 2048              |
| 64         | 128           | 2048              |
| 64         | 256           | 2048              |
| 64         | 512           | 2048              |
| 128        | 64            | 2048              |
| 128        | 128           | 2048              |
| 128        | 256           | 2048              |
| 128        | 512           | 2048              |
| 256        | 64            | 2048              |
| 256        | 128           | 2048              |
| 256        | 256           | 2048              |
| 256        | 512           | 2048              |
| 512        | 64            | 2048              |
| 512        | 128           | 2048              |
| 512        | 256           | 2048              |
| 512        | 512           | 2048              |
| 1024       | 64            | 2048              |
| 1024       | 128           | 2048              |
| 1024       | 256           | 2048              |
| 1024       | 512           | 2048              |
--------------------------------------------------
EOF
}

if grep -q "NAND simulator" /proc/mtd; then
        fatal "nandsim is already loaded"
fi

if [ "$#" -lt "1" ]; then
        usage
        exit 1
fi

size="$1"
eb_size="$2"
page_size="$3"
if [ "$#" = "1" ]; then
        eb_size="16"
        page_size="512"
elif [ "$#" = "2" ]; then
        page_size="512"
fi

if [ "$page_size" -eq 512 ] && [ "$eb_size" -ne "16" ]; then
        fatal "only 16KiB eraseblocks are possible in case of 512 bytes page"
fi

first=
second=
third=
fourth=

if [ "$page_size" -eq "512" ]; then
        first="0x20"
        case "$size" in
        16)  second=0x33 ;;
        32)  second=0x35 ;;
        64)  second=0x36 ;;
        128) second=0x78 ;;
        256) second=0x71 ;;
        *) fatal "flash size ${size}MiB is not supported, try 16, 32, 64 or 256"
        esac
elif [ "$page_size" -eq "2048" ]; then
        case "$eb_size" in
        64)  fourth="0x05" ;;
        128) fourth="0x15" ;;
        256) fourth="0x25" ;;
        512) fourth="0x35" ;;
        *)   fatal "eraseblock ${eb_size}KiB is not supported"
        esac


        case "$size" in
        64)   first="0x20"; second="0xa2"; third="0x00 ";;
        128)  first="0xec"; second="0xa1"; third="0x00 ";;
        256)  first="0x20"; second="0xaa"; third="0x00 ";;
        512)  first="0x20"; second="0xac"; third="0x00 ";;
        1024) first="0xec"; second="0xd3"; third="0x51 ";;
        *) fatal "unable to emulate ${size}MiB flash with ${eb_size}KiB eraseblock"
        esac
else
        fatal "bad NAND page size ${page_size}KiB, it has to be either 512 or 2048"
fi

first="first_id_byte=$first"
second="second_id_byte=$second"
[ -z "$third" ]  || third="third_id_byte=$third"
[ -z "$fourth" ] || fourth="fourth_id_byte=$fourth"

modprobe nandsim "$first" "$second" $third $fourth

echo "Loaded NAND simulator (${size}MiB, ${eb_size}KiB eraseblock, $page_size bytes NAND page)" 
    branches: [ "main" ]
  pull_request: /*
 *  nanddump.c
 *
 *  Copyright (C) 2000 
 *                     
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Overview:
 *   This utility dumps the contents of raw NAND chips or NAND
 *   chips contained in DoC devices.
 */

#define PROGRAM_NAME "nanddump"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <asm/types.h>
#include <mtd/mtd-user.h>
#include "common.h"
#include <libmtd.h>

static void display_help(int status)
{
        fprintf(status == EXIT_SUCCESS ? stdout : stderr,
"Usage: %s [OPTIONS] MTD-device\n"
"Dumps the contents of a nand mtd partition.\n"
"\n"
"-h         --help               Display this help and exit\n"
"           --version            Output version information and exit\n"
"           --bb=METHOD          Choose bad block handling method (see below).\n"
"-a         --forcebinary        Force printing of binary data to tty\n"
"-c         --canonicalprint     Print canonical Hex+ASCII dump\n"
"-f file    --file=file          Dump to file\n"
"-l length  --length=length      Length\n"
"-n         --noecc              Read without error correction\n"
"           --omitoob            Omit OOB data (default)\n"
"-o         --oob                Dump OOB data\n"
"-p         --prettyprint        Print nice (hexdump)\n"
"-q         --quiet              Don't display progress and status messages\n"
"-s addr    --startaddress=addr  Start address\n"
"           --skip-bad-blocks-to-start\n"
"                                Skip bad blocks when seeking to the start address\n"
"-C         --continuous         Continuous read up to a block of data at a time\n"
"\n"
"--bb=METHOD, where METHOD can be `padbad', `dumpbad', or `skipbad':\n"
"    padbad:  dump flash data, substituting 0xFF for any bad blocks\n"
"    dumpbad: dump flash data, including any bad blocks\n"
"    skipbad: dump good data, completely skipping any bad blocks (default)\n",
        PROGRAM_NAME);
        exit(status);
}

static void display_version(void)
{
        common_print_version();
        printf("%1$s comes with NO WARRANTY\n"
                        "to the extent permitted by law.\n"
                        "\n"
                        "You may redistribute copies of %1$s\n"
                        "under the terms of the GNU General Public Licence.\n"
                        "See the file `COPYING' for more information.\n",
                        PROGRAM_NAME);
        exit(EXIT_SUCCESS);
}

// Option variables

static bool                        pretty_print = false;        // print nice
static bool                        noecc = false;                // don't error correct
static bool                        omitoob = true;                // omit oob data
static long long                start_addr;                // start address
static long long                length;                        // dump length
static const char                *mtddev;                // mtd device name
static const char                *dumpfile;                // dump file name
static bool                        quiet = false;                // suppress diagnostic output
static bool                        canonical = false;        // print nice + ascii
static bool                        forcebinary = false;        // force printing binary to tty
static bool                        skip_bad_blocks_to_start = false;
static bool                        continuous = false;        // leverage continuous reads

static enum {
        padbad,   // dump flash data, substituting 0xFF for any bad blocks
        dumpbad,  // dump flash data, including any bad blocks
        skipbad,  // dump good data, completely skipping any bad blocks
} bb_method = skipbad;

static void process_options(int argc, char * const argv[])
{
        int error = 0;
        bool oob_default = true;
        char *dumpfile_tmp = NULL;

        for (;;) {
                int option_index = 0;
                static const char short_options[] = "hs:f:l:opqncaVC";
                static const struct option long_options[] = {
                        {"version", no_argument, 0, 'V'},
                        {"bb", required_argument, 0, 0},
                        {"omitoob", no_argument, 0, 0},
                        {"skip-bad-blocks-to-start", no_argument, 0, 0 },
                        {"help", no_argument, 0, 'h'},
                        {"forcebinary", no_argument, 0, 'a'},
                        {"canonicalprint", no_argument, 0, 'c'},
                        {"file", required_argument, 0, 'f'},
                        {"oob", no_argument, 0, 'o'},
                        {"prettyprint", no_argument, 0, 'p'},
                        {"startaddress", required_argument, 0, 's'},
                        {"length", required_argument, 0, 'l'},
                        {"noecc", no_argument, 0, 'n'},
                        {"quiet", no_argument, 0, 'q'},
                        {"continuous", no_argument, 0, 'C'},
                        {0, 0, 0, 0},
                };

                int c = getopt_long(argc, argv, short_options,
                                long_options, &option_index);
                if (c == EOF) {
                        break;
                }

                switch (c) {
                        case 0:
                                switch (option_index) {
                                        case 1:
                                                /* Handle --bb=METHOD */
                                                if (!strcmp(optarg, "padbad"))
                                                        bb_method = padbad;
                                                else if (!strcmp(optarg, "dumpbad"))
                                                        bb_method = dumpbad;
                                                else if (!strcmp(optarg, "skipbad"))
                                                        bb_method = skipbad;
                                                else
                                                        error++;
                                                break;
                                        case 2: /* --omitoob */
                                                if (oob_default) {
                                                        oob_default = false;
                                                        omitoob = true;
                                                } else {
                                                        errmsg_die("--oob and --oomitoob are mutually exclusive");
                                                }
                                                break;
                                        case 3: /* --skip-bad-blocks-to-start */
                                                skip_bad_blocks_to_start = true;
                                                break;
                                }
                                break;
                        case 'V':
                                display_version();
                                break;
                        case 's':
                                start_addr = simple_strtoll(optarg, &error);
                                break;
                        case 'f':
                                free(dumpfile_tmp);
                                dumpfile = dumpfile_tmp = xstrdup(optarg);
                                break;
                        case 'l':
                                length = simple_strtoll(optarg, &error);
                                break;
                        case 'o':
                                if (oob_default) {
                                        oob_default = false;
                                        omitoob = false;
                                } else {
                                        errmsg_die("--oob and --oomitoob are mutually exclusive");
                                }
                                break;
                        case 'a':
                                forcebinary = true;
                                break;
                        case 'c':
                                canonical = true;
                                /* fall-through */
                        case 'p':
                                pretty_print = true;
                                break;
                        case 'q':
                                quiet = true;
                                break;
                        case 'n':
                                noecc = true;
                                break;
                        case 'C':
                                continuous = true;
                                break;
                        case 'h':
                                display_help(EXIT_SUCCESS);
                                break;
                        case '?':
                                error++;
                                break;
                }
        }

        if (start_addr < 0)
                errmsg_die("Can't specify negative offset with option -s: %lld",
                                start_addr);

        if (length < 0)
                errmsg_die("Can't specify negative length with option -l: %lld", length);

        if (quiet && pretty_print) {
                fprintf(stderr, "The quiet and pretty print options are mutually-\n"
                                "exclusive. Choose one or the other.\n");
                exit(EXIT_FAILURE);
        }

        if (forcebinary && pretty_print) {
                fprintf(stderr, "The forcebinary and pretty print options are\n"
                                "mutually-exclusive. Choose one or the "
                                "other.\n");
                exit(EXIT_FAILURE);
        }

        if (continuous && !omitoob) {
                fprintf(stderr, "Sequential/continuous reads (when available) will\n"
                                "always skip OOB data, so it is not possible to \n"
                                "request both at the same time.\n");
                exit(EXIT_FAILURE);
        }

        if ((argc - optind) != 1 || error)
                display_help(EXIT_FAILURE);

        mtddev = argv[optind];
}

#define PRETTY_ROW_SIZE 16
#define PRETTY_BUF_LEN 80

/**
 * pretty_dump_to_buffer - formats a blob of data to "hex ASCII" in memory
 * @buf: data blob to dump
 * @len: number of bytes in the @buf
 * @linebuf: where to put the converted data
 * @linebuflen: total size of @linebuf, including space for terminating NULL
 * @pagedump: true - dumping as page format; false - dumping as OOB format
 * @ascii: dump ascii formatted data next to hexdump
 * @prefix: address to print before line in a page dump, ignored if !pagedump
 *
 * pretty_dump_to_buffer() works on one "line" of output at a time, i.e.,
 * PRETTY_ROW_SIZE bytes of input data converted to hex + ASCII output.
 *
 * Given a buffer of unsigned char data, pretty_dump_to_buffer() converts the
 * input data to a hex/ASCII dump at the supplied memory location. A prefix
 * is included based on whether we are dumping page or OOB data. The converted
 * output is always NULL-terminated.
 *
 * e.g.
 *   pretty_dump_to_buffer(data, data_len, prettybuf, linelen, true,
 *                         false, 256);
 * produces:
 *   0x00000100: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f
 * NOTE: This function was adapted from linux kernel, "lib/hexdump.c"
 */
static void pretty_dump_to_buffer(const unsigned char *buf, size_t len,
                char *linebuf, size_t linebuflen, bool pagedump, bool ascii,
                unsigned long long prefix)
{
        static const char hex_asc[] = "0123456789abcdef";
        unsigned char ch;
        unsigned int j, lx = 0, ascii_column;

        if (pagedump)
                lx += sprintf(linebuf, "0x%.8llx: ", prefix);
        else
                lx += sprintf(linebuf, "  OOB Data: ");

        if (!len)
                goto nil;
        if (len > PRETTY_ROW_SIZE)        /* limit to one line at a time */
                len = PRETTY_ROW_SIZE;

        for (j = 0; (j < len) && (lx + 3) <= linebuflen; j++) {
                ch = buf[j];
                linebuf[lx++] = hex_asc[(ch & 0xf0) >> 4];
                linebuf[lx++] = hex_asc[ch & 0x0f];
                linebuf[lx++] = ' ';
        }
        if (j)
                lx--;

        ascii_column = 3 * PRETTY_ROW_SIZE + 14;

        if (!ascii)
                goto nil;

        /* Spacing between hex and ASCII - ensure at least one space */
        lx += sprintf(linebuf + lx, "%*s",
                        MAX((int)MIN(linebuflen, ascii_column) - 1 - lx, 1),
                        " ");

        linebuf[lx++] = '|';
        for (j = 0; (j < len) && (lx + 2) < linebuflen; j++)
                linebuf[lx++] = (isascii(buf[j]) && isprint(buf[j])) ? buf[j]
                        : '.';
        linebuf[lx++] = '|';
nil:
        linebuf[lx++] = '\n';
        linebuf[lx++] = '\0';
}

/**
 * ofd_write - writes whole buffer to the file associated with a descriptor
 *
 * On failure an error (negative number) is returned. Otherwise 0 is returned.
 */
static int ofd_write(int ofd, const void *buf, size_t nbyte)
{
        const unsigned char *data = buf;
        ssize_t bytes;

        while (nbyte) {
                bytes = write(ofd, data, nbyte);
                if (bytes < 0) {
                        int err = -errno;

                        sys_errmsg("Unable to write to output");

                        return err;
                }
                data += bytes;
                nbyte -= bytes;
        }

        return 0;
}

/*
 * Main program
 */
int main(int argc, char * const argv[])
{
        long long ofs, end_addr = 0, readbuf_sz;
        long long blockstart = 1;
        int i, fd, ofd = 0, bs, badblock = 0;
        struct mtd_dev_info mtd;
        char pretty_buf[PRETTY_BUF_LEN];
        int firstblock = 1;
        struct mtd_ecc_stats stat1, stat2;
        bool eccstats = false;
        unsigned char *readbuf = NULL, *oobbuf = NULL;
        libmtd_t mtd_desc;
        int err;

        process_options(argc, argv);

        /* Initialize libmtd */
        mtd_desc = libmtd_open();
        if (!mtd_desc)
                return errmsg("can't initialize libmtd");

        /* Open MTD device */
        if ((fd = open(mtddev, O_RDONLY)) == -1) {
                perror(mtddev);
                exit(EXIT_FAILURE);
        }

        /* Fill in MTD device capability structure */
        if (mtd_get_dev_info(mtd_desc, mtddev, &mtd) < 0)
                return errmsg("mtd_get_dev_info failed");

        /* Allocate buffers */
        readbuf_sz = mtd.eb_size;
        oobbuf = xmalloc(mtd.oob_size);
        readbuf = xmalloc(readbuf_sz);

        if (noecc)  {
                if (ioctl(fd, MTDFILEMODE, MTD_FILE_MODE_RAW) != 0) {
                                perror("MTDFILEMODE");
                                goto closeall;
                }
        } else {
                /* check if we can read ecc stats */
                if (!ioctl(fd, ECCGETSTATS, &stat1)) {
                        eccstats = true;
                        if (!quiet) {
                                fprintf(stderr, "ECC failed: %d\n", stat1.failed);
                                fprintf(stderr, "ECC corrected: %d\n", stat1.corrected);
                                fprintf(stderr, "Number of bad blocks: %d\n", stat1.badblocks);
                                fprintf(stderr, "Number of bbt blocks: %d\n", stat1.bbtblocks);
                        }
                } else
                        perror("No ECC status information available");
        }

        /* Open output file for writing. If file name is "-", write to standard
         * output. */
        if (!dumpfile) {
                ofd = STDOUT_FILENO;
        } else if ((ofd = open(dumpfile, O_WRONLY | O_TRUNC | O_CREAT, 0644))== -1) {
                perror(dumpfile);
                goto closeall;
        }

        if (!pretty_print && !forcebinary && isatty(ofd)) {
                fprintf(stderr, "Not printing binary garbage to tty. Use '-a'\n"
                                "or '--forcebinary' to override.\n");
                goto closeall;
        }

        /* Initialize start/end addresses and block size */
        if (start_addr & (mtd.min_io_size - 1)) {
                fprintf(stderr, "the start address (-s parameter) is not page-aligned!\n"
                                "The pagesize of this NAND Flash is 0x%x.\n",
                                mtd.min_io_size);
                goto closeall;
        }
        if (skip_bad_blocks_to_start) {
                long long bbs_offset = 0;
                while (bbs_offset < start_addr) {
                        err = mtd_is_bad(&mtd, fd, bbs_offset / mtd.eb_size);
                        if (err < 0) {
                                sys_errmsg("%s: MTD get bad block failed", mtddev);
                                goto closeall;
                        } else if (err == 1) {
                                if (!quiet)
                                        fprintf(stderr, "Bad block at %llx\n", bbs_offset);
                                start_addr += mtd.eb_size;
                        }
                        bbs_offset += mtd.eb_size;
                }
        }

        if (length)
                end_addr = start_addr + length;
        if (!length || end_addr > mtd.size)
                end_addr = mtd.size;

        /* Print informative message */
        if (!quiet) {
                fprintf(stderr, "Block size %d, page size %d, OOB size %d\n",
                                mtd.eb_size, mtd.min_io_size, mtd.oob_size);
                fprintf(stderr,
                                "Dumping data starting at 0x%08llx and ending at 0x%08llx...\n",
                                start_addr, end_addr);
        }

        /* Dump the flash contents */
        for (ofs = start_addr; ofs < end_addr; ofs += bs) {
                long long size_left = end_addr - ofs;

                /* Check for bad block */
                if (bb_method == dumpbad) {
                        badblock = 0;
                } else if (blockstart != (ofs & (~mtd.eb_size + 1)) ||
                                firstblock) {
                        blockstart = ofs & (~mtd.eb_size + 1);
                        firstblock = 0;
                        if ((badblock = mtd_is_bad(&mtd, fd, ofs / mtd.eb_size)) < 0) {
                                errmsg("libmtd: mtd_is_bad");
                                goto closeall;
                        }
                }

                if (continuous)
                        bs = MIN(size_left, mtd.eb_size);
                else
                        bs = mtd.min_io_size;

                if (badblock) {
                        /* skip bad block, increase end_addr */
                        if (bb_method == skipbad) {
                                end_addr += mtd.eb_size;
                                ofs += mtd.eb_size - mtd.min_io_size;
                                if (end_addr > mtd.size)
                                        end_addr = mtd.size;
                                continue;
                        }
                        memset(readbuf, 0xff, readbuf_sz);
                } else {
                        /* Read page data and exit on failure */
                        if (mtd_read(&mtd, fd, ofs / mtd.eb_size, ofs % mtd.eb_size, readbuf, bs)) {
                                errmsg("mtd_read");
                                goto closeall;
                        }
                }

                /* ECC stats available ? */
                if (eccstats) {
                        if (ioctl(fd, ECCGETSTATS, &stat2)) {
                                perror("ioctl(ECCGETSTATS)");
                                goto closeall;
                        }
                        if (stat1.failed != stat2.failed)
                                fprintf(stderr, "ECC: %d uncorrectable bitflip(s)"
                                                " at offset 0x%08llx\n",
                                                stat2.failed - stat1.failed, ofs);
                        if (stat1.corrected != stat2.corrected)
                                fprintf(stderr, "ECC: %d corrected bitflip(s) at"
                                                " offset 0x%08llx\n",
                                                stat2.corrected - stat1.corrected, ofs);
                        stat1 = stat2;
                }

                /* Write out page data */
                if (pretty_print) {
                        for (i = 0; i < bs; i += PRETTY_ROW_SIZE) {
                                pretty_dump_to_buffer(readbuf + i, PRETTY_ROW_SIZE,
                                                pretty_buf, PRETTY_BUF_LEN, true, canonical, ofs + i);
                                err = ofd_write(ofd, pretty_buf, strlen(pretty_buf));
                                if (err)
                                        goto closeall;
                        }
                } else {
                        /* Write requested length if oob is omitted */
                        if (omitoob && (size_left < bs))
                                err = ofd_write(ofd, readbuf, size_left);
                        else
                                err = ofd_write(ofd, readbuf, bs);

                        if (err)
                                goto closeall;
                }

                if (omitoob)
                        continue;

                if (badblock) {
                        memset(oobbuf, 0xff, mtd.oob_size);
                } else {
                        /* Read OOB data and exit on failure */
                        if (mtd_read_oob(mtd_desc, &mtd, fd, ofs, mtd.oob_size, oobbuf)) {
                                errmsg("libmtd: mtd_read_oob");
                                goto closeall;
                        }
                }

                /* Write out OOB data */
                if (pretty_print) {
                        for (i = 0; i < mtd.oob_size; i += PRETTY_ROW_SIZE) {
                                pretty_dump_to_buffer(oobbuf + i, mtd.oob_size - i,
                                                pretty_buf, PRETTY_BUF_LEN, false, canonical, 0);
                                err = ofd_write(ofd, pretty_buf, strlen(pretty_buf));
                                if (err)
                                        goto closeall;
                        }
                } else {
                        err = ofd_write(ofd, oobbuf, mtd.oob_size);
                        if (err)
                                goto closeall;
                }
        }

        /* Close the output file and MTD device, free memory */
        close(fd);
        close(ofd);
        free(oobbuf);
        free(readbuf);

        /* Exit happy */
        return EXIT_SUCCESS;

closeall:
        close(fd);
        if (ofd > 0 && ofd != STDOUT_FILENO)
                close(ofd);
        free(oobbuf);
        free(readbuf);
        exit(EXIT_FAILURE);
} 
    branches: [ "main" ]

jobs: // SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2014 Bootlin
 *
 * Authors: 
 *          
 *
 * Overview:
 *   This utility manually flips specified bits in a NAND flash.
 */

#define PROGRAM_NAME "nandflipbits"

#include <mtd/mtd-user.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libmtd.h>
#include <getopt.h>
#include <stdio.h>
#include <fcntl.h>

#include "common.h"

struct bit_flip {
        uint32_t block;
        uint64_t offset;
        int bit;
        bool done;
};

static void usage(int status)
{
        fprintf(status == EXIT_SUCCESS ? stdout : stderr,
"Usage: "PROGRAM_NAME" [OPTIONS] <device> <bit>@<address> [<bit>@<address>...]\n"
"\n"
"       Test ECC engines, see if they match the specified correction strength:\n"
"         * Reads in raw mode the data from an MTD device\n"
"         * Flips the indicated bit(s)\n"
"         * Write it back in raw mode.\n"
"\n"
"       -h, --help              Display this help and exit\n"
"       -o, --oob               Provided addresses take OOB area into account\n"
"       -q, --quiet             Don't display progress messages\n"
"\n"
        );
        exit(status);
}

static const char *mtd_device;
static bool quiet = false;
static bool oob_mode = false;
static struct bit_flip *bits_to_flip;
static int nbits_to_flip = 0;

static void process_options(int argc, char * const argv[])
{
        int error = 0;
        int i;

        for (;;) {
                int option_index = 0;
                static const char short_options[] = "hoq";
                static const struct option long_options[] = {
                        {"help", no_argument, 0, 'h'},
                        {"oob", no_argument, 0, 'o'},
                        {"quiet", no_argument, 0, 'q'},
                        {0, 0, 0, 0},
                };

                int c = getopt_long(argc, argv, short_options,
                                    long_options, &option_index);
                if (c == EOF)
                        break;

                switch (c) {
                case 'q':
                        quiet = true;
                        break;
                case 'o':
                        oob_mode = true;
                        break;
                case 'h':
                        usage(EXIT_SUCCESS);
                        break;
                case '?':
                default:
                        error++;
                        break;
                }
        }

        argc -= optind;
        argv += optind;

        /*
         * There must be at least the MTD device node path argument remaining
         * and a list of minimum one 'bits-to-flip'.
         */

        if (argc < 2 || error)
                usage(EXIT_FAILURE);

        /* MTD device */
        mtd_device = argv[0];
        argc--;
        argv++;

        /* Parse the bits to flip */
        nbits_to_flip = argc;
        bits_to_flip = malloc(sizeof(*bits_to_flip) * nbits_to_flip);
        if (!bits_to_flip)
                exit(EXIT_FAILURE);

        for (i = 0; i < nbits_to_flip; i++) {
                struct bit_flip        *bit_to_flip = &bits_to_flip[i];
                char *desc = argv[i];

                bit_to_flip->bit = strtol(desc, &desc, 0);
                if (errno || bit_to_flip->bit > 7)
                        goto free_bits;

                if (!desc || *desc++ != '@')
                        goto free_bits;

                bit_to_flip->offset = strtol(desc, &desc, 0);
                if (errno)
                        goto free_bits;
        }

        return;

free_bits:
        free(bits_to_flip);

        fprintf(stderr, "Invalid bit description\n");

        exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
        struct mtd_dev_info mtd;
        libmtd_t mtd_desc;
        uint64_t mtdlen;
        uint32_t pagelen, pages_per_blk, blklen;
        uint8_t *buffer;
        int fd, ret, i;

        process_options(argc, argv);

        /* Open the libmtd */
        mtd_desc = libmtd_open();
        if (!mtd_desc) {
                fprintf(stderr, "Cannot initialize libmtd\n");
                ret = EXIT_FAILURE;
                goto free_bits;
        }

        /* Fill in MTD device capability structure */
        ret = mtd_get_dev_info(mtd_desc, mtd_device, &mtd);
        if (ret < 0) {
                fprintf(stderr, "Cannot retrieve MTD device information\n");
                ret = EXIT_FAILURE;
                goto close_lib;
        }

        /* Verify we are using a NAND device */
        if (mtd.type != MTD_NANDFLASH && mtd.type != MTD_MLCNANDFLASH) {
                fprintf(stderr, "%s is not a NAND flash\n", mtd_device);
                ret = EXIT_FAILURE;
                goto close_lib;
        }

        /* Open the MTD device */
        fd = open(mtd_device, O_RDWR);
        if (fd < 0) {
                fprintf(stderr, "Cannot open %s\n", mtd_device);
                ret = EXIT_FAILURE;
                goto close_lib;
        }

        /* Select raw mode */
        ret = ioctl(fd, MTDFILEMODE, MTD_FILE_MODE_RAW);
        if (ret) {
                fprintf(stderr, "Unavailable raw mode ioctl\n");
                ret = EXIT_FAILURE;
                goto close_fd;
        }

        pagelen = mtd.min_io_size + (oob_mode ? mtd.oob_size : 0);
        pages_per_blk = mtd.eb_size / mtd.min_io_size;
        blklen = pages_per_blk * pagelen;
        mtdlen = (uint64_t)blklen * (uint64_t)mtd.eb_cnt;
        buffer = malloc((mtd.min_io_size + mtd.oob_size) * pages_per_blk);
        if (!buffer) {
                ret = EXIT_FAILURE;
                goto close_fd;
        }

        for (i = 0; i < nbits_to_flip; i++) {
                int page;

                if (bits_to_flip[i].offset >= mtdlen) {
                        fprintf(stderr, "Invalid byte offset %" PRId64
                                " (max %" PRId64 ")\n",
                                bits_to_flip[i].offset, mtdlen);
                        ret = EXIT_FAILURE;
                        goto free_buf;
                }

                bits_to_flip[i].block = bits_to_flip[i].offset / blklen;
                bits_to_flip[i].offset %= blklen;
                page = bits_to_flip[i].offset / pagelen;
                bits_to_flip[i].offset = (page *
                                          (mtd.min_io_size + mtd.oob_size)) +
                                         (bits_to_flip[i].offset % pagelen);
        }

        while (1) {
                struct bit_flip        *bit_to_flip = NULL;
                int blkoffs;
                int bufoffs;

                /* Look for the next bitflip to insert */
                for (i = 0; i < nbits_to_flip; i++) {
                        if (bits_to_flip[i].done == false) {
                                bit_to_flip = &bits_to_flip[i];
                                break;
                        }
                }

                if (!bit_to_flip) {
                        ret = EXIT_SUCCESS;
                        break;
                }

                /* Read the content of all the pages of a block */
                blkoffs = 0;
                bufoffs = 0;
                for (i = 0; i < pages_per_blk; i++) {
                        ret = mtd_read(&mtd, fd, bit_to_flip->block, blkoffs,
                                       buffer + bufoffs, mtd.min_io_size);
                        if (ret) {
                                fprintf(stderr, "MTD read failure\n");
                                ret = EXIT_FAILURE;
                                goto free_buf;
                        }

                        bufoffs += mtd.min_io_size;

                        ret = mtd_read_oob(mtd_desc, &mtd, fd,
                                           (unsigned long long)bit_to_flip->block * mtd.eb_size +
                                           blkoffs,
                                           mtd.oob_size, buffer + bufoffs);
                        if (ret) {
                                fprintf(stderr, "MTD OOB read failure\n");
                                ret = EXIT_FAILURE;
                                goto free_buf;
                        }

                        bufoffs += mtd.oob_size;
                        blkoffs += mtd.min_io_size;
                }

                /* Flip all bits that are located in this particular block */
                for (i = 0; i < nbits_to_flip; i++) {
                        unsigned char val, mask;

                        if (bits_to_flip[i].block != bit_to_flip->block)
                                continue;

                        mask = 1 << bits_to_flip[i].bit;
                        val = buffer[bits_to_flip[i].offset] & mask;
                        if (val)
                                buffer[bits_to_flip[i].offset] &= ~mask;
                        else
                                buffer[bits_to_flip[i].offset] |= mask;
                }

                /* Erase the block */
                ret = mtd_erase(mtd_desc, &mtd, fd, bit_to_flip->block);
                if (ret) {
                        fprintf(stderr, "MTD erase failure\n");
                        ret = EXIT_FAILURE;
                        goto free_buf;
                }

                /* Rewrite the pages, still in raw mode, with the bitflips */
                blkoffs = 0;
                bufoffs = 0;
                for (i = 0; i < pages_per_blk; i++) {
                        ret = mtd_write(mtd_desc, &mtd, fd, bit_to_flip->block,
                                        blkoffs, buffer + bufoffs, mtd.min_io_size,
                                        buffer + bufoffs + mtd.min_io_size,
                                        mtd.oob_size,
                                        MTD_OPS_RAW);
                        if (ret) {
                                fprintf(stderr, "MTD write failure\n");
                                ret = EXIT_FAILURE;
                                goto free_buf;
                        }

                        blkoffs += mtd.min_io_size;
                        bufoffs += mtd.min_io_size + mtd.oob_size;
                }

                /* Mark the added bitflips as done */
                for (i = 0; i < nbits_to_flip; i++) {
                        if (bits_to_flip[i].block == bit_to_flip->block)
                                bits_to_flip[i].done = true;
                }
        }

free_buf:
        free(buffer);
close_fd:
        close(fd);
close_lib:
        libmtd_close(mtd_desc);
free_bits:
        free(bits_to_flip);

        exit(ret);
} 
  build: #define PROGRAM_NAME "nandtest"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <getopt.h>

#include <asm/types.h>
#include "mtd/mtd-user.h"
#include "common.h"

static NORETURN void usage(int status)
{
        fprintf(status ? stderr : stdout,
                "usage: %s [OPTIONS] <device>\n\n"
                "  -h, --help           Display this help output\n"
                "  -V, --version        Display version information and exit\n"
                "  -m, --markbad        Mark blocks bad if they appear so\n"
                "  -s, --seed           Supply random seed\n"
                "  -p, --passes         Number of passes\n"
                "  -r <n>, --reads=<n>  Read & check <n> times per pass\n"
                "  -o, --offset         Start offset on flash\n"
                "  -l, --length         Length of flash to test\n"
                "  -k, --keep           Restore existing contents after test\n",
                PROGRAM_NAME);
        exit(status);
}

struct mtd_info_user meminfo;
struct mtd_ecc_stats oldstats, newstats;
int fd;
int markbad=0;
int seed;

static int read_and_compare(loff_t ofs, unsigned char *data,
                            unsigned char *rbuf)
{
        ssize_t len;
        int i;

        len = pread(fd, rbuf, meminfo.erasesize, ofs);
        if (len < meminfo.erasesize) {
                printf("\n");
                if (len)
                        fprintf(stderr, "Short read (%zd bytes)\n", len);
                else
                        perror("read");
                exit(1);
        }

        if (ioctl(fd, ECCGETSTATS, &newstats)) {
                printf("\n");
                perror("ECCGETSTATS");
                close(fd);
                exit(1);
        }

        if (newstats.corrected > oldstats.corrected) {
                printf("\n %d bit(s) ECC corrected at %08x\n",
                                newstats.corrected - oldstats.corrected,
                                (unsigned) ofs);
                oldstats.corrected = newstats.corrected;
        }
        if (newstats.failed > oldstats.failed) {
                printf("\nECC failed at %08x\n", (unsigned) ofs);
                oldstats.failed = newstats.failed;
        }

        printf("\r%08x: checking...", (unsigned)ofs);
        fflush(stdout);

        if (memcmp(data, rbuf, meminfo.erasesize)) {
                printf("\n");
                fprintf(stderr, "compare failed. seed %d\n", seed);
                for (i=0; i<meminfo.erasesize; i++) {
                        if (data[i] != rbuf[i])
                                printf("Byte 0x%x is %02x should be %02x\n",
                                       i, rbuf[i], data[i]);
                }
                return 1;
        }
        return 0;
}

static int erase_and_write(loff_t ofs, unsigned char *data, unsigned char *rbuf,
                           int nr_reads)
{
        struct erase_info_user er;
        ssize_t len;
        int i, read_errs = 0;

        printf("\r%08x: erasing... ", (unsigned)ofs);
        fflush(stdout);

        er.start = ofs;
        er.length = meminfo.erasesize;

        if (ioctl(fd, MEMERASE, &er)) {
                perror("MEMERASE");
                if (markbad) {
                        printf("Mark block bad at %08lx\n", (long)ofs);
                        ioctl(fd, MEMSETBADBLOCK, &ofs);
                }
                return 1;
        }

        printf("\r%08x: writing...", (unsigned)ofs);
        fflush(stdout);

        len = pwrite(fd, data, meminfo.erasesize, ofs);
        if (len < 0) {
                printf("\n");
                perror("write");
                if (markbad) {
                        printf("Mark block bad at %08lx\n", (long)ofs);
                        ioctl(fd, MEMSETBADBLOCK, &ofs);
                }
                return 1;
        }
        if (len < meminfo.erasesize) {
                printf("\n");
                fprintf(stderr, "Short write (%zd bytes)\n", len);
                exit(1);
        }

        for (i=1; i<=nr_reads; i++) {
                printf("\r%08x: reading (%d of %d)...", (unsigned)ofs, i, nr_reads);
                fflush(stdout);
                if (read_and_compare(ofs, data, rbuf))
                        read_errs++;
        }
        if (read_errs) {
                fprintf(stderr, "read/check %d of %d failed. seed %d\n", read_errs, nr_reads, seed);
                return 1;
        }
        return 0;
}

static uint64_t get_mem_size(const char* device)
{
        const char* p = strrchr(device, '/');
        char path[PATH_MAX];
        int fd;

        snprintf(path, sizeof(path), "/sys/class/mtd/%s/size", p);
        fd = open(path, O_RDONLY);
        if (fd >= 0) {
                char buffer[32];
                ssize_t n = read(fd, buffer, sizeof(buffer));
                close(fd);
                if (n > 0) {
                        return strtoull(buffer, NULL, 0);
                }
        }

        fprintf(stderr, "Can't read size from %s\n", path);
        exit(1);
}

/*
 * Main program
 */
int main(int argc, char **argv)
{
        int i;
        unsigned char *wbuf, *rbuf, *kbuf;
        int pass;
        int nr_passes = 1;
        int nr_reads = 4;
        int keep_contents = 0;
        uint64_t offset = 0;
        uint64_t length = -1;
        uint64_t mem_size = 0;
        int error = 0;

        seed = time(NULL);

        for (;;) {
                static const char short_options[] = "hkl:mo:p:r:s:V";
                static const struct option long_options[] = {
                        { "help", no_argument, 0, 'h' },
                        { "version", no_argument, 0, 'V' },
                        { "markbad", no_argument, 0, 'm' },
                        { "seed", required_argument, 0, 's' },
                        { "passes", required_argument, 0, 'p' },
                        { "offset", required_argument, 0, 'o' },
                        { "length", required_argument, 0, 'l' },
                        { "reads", required_argument, 0, 'r' },
                        { "keep", no_argument, 0, 'k' },
                        {0, 0, 0, 0},
                };
                int option_index = 0;
                int c = getopt_long(argc, argv, short_options, long_options, &option_index);
                if (c == EOF)
                        break;

                switch (c) {
                case 'h':
                        usage(EXIT_SUCCESS);
                case 'V':
                        common_print_version();
                        exit(EXIT_SUCCESS);
                        break;
                case '?':
                        usage(EXIT_FAILURE);

                case 'm':
                        markbad = 1;
                        break;

                case 'k':
                        keep_contents = 1;
                        break;

                case 's':
                        seed = atol(optarg);
                        break;

                case 'p':
                        nr_passes = atol(optarg);
                        break;

                case 'r':
                        nr_reads = atol(optarg);
                        break;

                case 'o':
                        offset = simple_strtoull(optarg, &error);
                        break;

                case 'l':
                        length = simple_strtoull(optarg, &error);
                        break;

                }
        }
        if (argc - optind != 1)
                usage(EXIT_FAILURE);
        if (error)
                errmsg_die("Try --help for more information");

        fd = open(argv[optind], O_RDWR);
        if (fd < 0) {
                perror("open");
                exit(1);
        }

        if (ioctl(fd, MEMGETINFO, &meminfo)) {
                perror("MEMGETINFO");
                close(fd);
                exit(1);
        }

        mem_size = get_mem_size(argv[optind]);

        if (length == -1)
                length = mem_size;

        if (offset % meminfo.erasesize) {
                fprintf(stderr, "Offset %" PRIx64
                        " not multiple of erase size %x\n",
                        offset, meminfo.erasesize);
                exit(1);
        }
        if (length % meminfo.erasesize) {
                fprintf(stderr, "Length %" PRIx64
                        " not multiple of erase size %x\n",
                        length, meminfo.erasesize);
                exit(1);
        }
        if (length + offset > mem_size) {
                fprintf(stderr, "Length %" PRIx64 " + offset %" PRIx64
                        " exceeds device size %" PRIx64 "\n",
                        length, offset, mem_size);
                exit(1);
        }

        wbuf = malloc(meminfo.erasesize * 3);
        if (!wbuf) {
                fprintf(stderr, "Could not allocate %d bytes for buffer\n",
                        meminfo.erasesize * 3);
                exit(1);
        }
        rbuf = wbuf + meminfo.erasesize;
        kbuf = rbuf + meminfo.erasesize;

        if (ioctl(fd, ECCGETSTATS, &oldstats)) {
                perror("ECCGETSTATS");
                close(fd);
                exit(1);
        }

        printf("ECC corrections: %d\n", oldstats.corrected);
        printf("ECC failures   : %d\n", oldstats.failed);
        printf("Bad blocks     : %d\n", oldstats.badblocks);
        printf("BBT blocks     : %d\n", oldstats.bbtblocks);

        srand(seed);

        for (pass = 0; pass < nr_passes; pass++) {
                loff_t test_ofs;

                for (test_ofs = offset; test_ofs < offset+length; test_ofs += meminfo.erasesize) {
                        ssize_t len;

                        seed = rand();
                        srand(seed);

                        if (ioctl(fd, MEMGETBADBLOCK, &test_ofs)) {
                                printf("\rBad block at 0x%08x\n", (unsigned)test_ofs);
                                continue;
                        }

                        for (i=0; i<meminfo.erasesize; i++)
                                wbuf[i] = rand();

                        if (keep_contents) {
                                printf("\r%08x: reading... ", (unsigned)test_ofs);
                                fflush(stdout);

                                len = pread(fd, kbuf, meminfo.erasesize, test_ofs);
                                if (len < meminfo.erasesize) {
                                        printf("\n");
                                        if (len)
                                                fprintf(stderr, "Short read (%zd bytes)\n", len);
                                        else
                                                perror("read");
                                        exit(1);
                                }
                        }
                        if (erase_and_write(test_ofs, wbuf, rbuf, nr_reads))
                                continue;
                        if (keep_contents)
                                erase_and_write(test_ofs, kbuf, rbuf, 1);
                }
                printf("\nFinished pass %d successfully\n", pass+1);
        }
        /* Return happy */
        return 0;
} 

    runs-on: /*
 *  nandwrite.c
 *
 *  Copyright (C) 
 *                 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Overview:
 *   This utility writes a binary image directly to a NAND flash
 *   chip or NAND chips contained in DoC devices. This is the
 *   "inverse operation" of nanddump.
 *
 * tglx: Major rewrite to handle bad blocks, write data with or without ECC
 *         write oob data only on request
 *
 * Bug/ToDo:
 */

#define PROGRAM_NAME "nandwrite"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <getopt.h>

#include <asm/types.h>
#include "mtd/mtd-user.h"
#include "common.h"
#include <libmtd.h>

static void display_help(int status)
{
        fprintf(status == EXIT_SUCCESS ? stdout : stderr,
"Usage: nandwrite [OPTION] MTD_DEVICE [INPUTFILE|-]\n"
"Writes to the specified MTD device.\n"
"\n"
"  -a, --autoplace         Use auto OOB layout\n"
"  -k, --skip-all-ffs      Skip pages that contain only 0xff bytes\n"
"  -m, --markbad           Mark blocks bad if write fails\n"
"  -n, --noecc             Write without ecc\n"
"  -N, --noskipbad         Write without bad block skipping\n"
"  -o, --oob               Input contains oob data\n"
"  -O, --onlyoob           Input contains oob data and only write the oob part\n"
"  -s addr, --start=addr   Set output start address (default is 0)\n"
"  --skip-bad-blocks-to-start"
"                          Skip bad blocks when seeking to the start address\n"
"  -p, --pad               Pad writes to page size\n"
"  -b, --blockalign=1|2|4  Set multiple of eraseblocks to align to\n"
"      --input-skip=length Skip |length| bytes of the input file\n"
"      --input-size=length Only read |length| bytes of the input file\n"
"  -q, --quiet             Don't display progress messages\n"
"  -h, --help              Display this help and exit\n"
"  -V, --version           Output version information and exit\n"
        );
        exit(status);
}

static void display_version(void)
{
        common_print_version();
        printf("Copyright (C) 2003 \n"
                        "\n"
                        "%1$s comes with NO WARRANTY\n"
                        "to the extent permitted by law.\n"
                        "\n"
                        "You may redistribute copies of %1$s\n"
                        "under the terms of the GNU General Public Licence.\n"
                        "See the file `COPYING' for more information.\n",
                        PROGRAM_NAME);
        exit(EXIT_SUCCESS);
}

static const char        *standard_input = "-";
static const char        *mtd_device, *img;
static long long        mtdoffset = 0;
static long long        inputskip = 0;
static long long        inputsize = 0;
static bool                quiet = false;
static bool                writeoob = false;
static bool                onlyoob = false;
static bool                markbad = false;
static bool                noecc = false;
static bool                autoplace = false;
static bool                skipallffs = false;
static bool                noskipbad = false;
static bool                pad = false;
static bool                skip_bad_blocks_to_start = false;
static int                blockalign = 1; /* default to using actual block size */

static void process_options(int argc, char * const argv[])
{
        int error = 0;

        for (;;) {
                int option_index = 0;
                static const char short_options[] = "hb:mnNoOpqs:akV";
                static const struct option long_options[] = {
                        /* Order of these args with val==0 matters; see option_index. */
                        {"version", no_argument, 0, 'V'},
                        {"input-skip", required_argument, 0, 0},
                        {"input-size", required_argument, 0, 0},
                        {"skip-bad-blocks-to-start", no_argument, 0, 0},
                        {"help", no_argument, 0, 'h'},
                        {"blockalign", required_argument, 0, 'b'},
                        {"markbad", no_argument, 0, 'm'},
                        {"noecc", no_argument, 0, 'n'},
                        {"noskipbad", no_argument, 0, 'N'},
                        {"oob", no_argument, 0, 'o'},
                        {"onlyoob", no_argument, 0, 'O'},
                        {"pad", no_argument, 0, 'p'},
                        {"quiet", no_argument, 0, 'q'},
                        {"start", required_argument, 0, 's'},
                        {"autoplace", no_argument, 0, 'a'},
                        {"skip-all-ffs", no_argument, 0, 'k'},
                        {0, 0, 0, 0},
                };

                int c = getopt_long(argc, argv, short_options,
                                long_options, &option_index);
                if (c == EOF)
                        break;

                switch (c) {
                case 0:
                        switch (option_index) {
                        case 1: /* --input-skip */
                                inputskip = simple_strtoll(optarg, &error);
                                break;
                        case 2: /* --input-size */
                                inputsize = simple_strtoll(optarg, &error);
                                break;
                        case 3: /* --skip-bad-blocks-to-start */
                                skip_bad_blocks_to_start = true;
                                break;
                        }
                        break;
                case 'V':
                        display_version();
                        break;
                case 'q':
                        quiet = true;
                        break;
                case 'n':
                        noecc = true;
                        break;
                case 'N':
                        noskipbad = true;
                        break;
                case 'm':
                        markbad = true;
                        break;
                case 'o':
                        writeoob = true;
                        break;
                case 'O':
                        writeoob = true;
                        onlyoob = true;
                        break;
                case 'p':
                        pad = true;
                        break;
                case 's':
                        mtdoffset = simple_strtoll(optarg, &error);
                        break;
                case 'b':
                        blockalign = atoi(optarg);
                        break;
                case 'a':
                        autoplace = true;
                        break;
                case 'k':
                        skipallffs = true;
                        break;
                case 'h':
                        display_help(EXIT_SUCCESS);
                        break;
                case '?':
                        error++;
                        break;
                }
        }

        if (mtdoffset < 0)
                errmsg_die("Can't specify negative device offset with option"
                                " -s: %lld", mtdoffset);

        if (blockalign <= 0)
                errmsg_die("Can't specify negative or zero blockalign with "
                                "option -b: %d", blockalign);

        if (!is_power_of_2(blockalign))
                errmsg_die("Can't specify a non-power-of-two blockalign with "
                                "option -b: %d", blockalign);

        if (autoplace && noecc)
                errmsg_die("Autoplacement and no-ECC are mutually exclusive");

        if (!onlyoob && (pad && writeoob))
                errmsg_die("Can't pad when oob data is present");

        argc -= optind;
        argv += optind;

        /*
         * There must be at least the MTD device node positional
         * argument remaining and, optionally, the input file.
         */

        if (argc < 1 || argc > 2 || error)
                display_help(EXIT_FAILURE);

        mtd_device = argv[0];

        /*
         * Standard input may be specified either explictly as "-" or
         * implicity by simply omitting the second of the two
         * positional arguments.
         */

        img = ((argc == 2) ? argv[1] : standard_input);
}

static void erase_buffer(void *buffer, size_t size)
{
        const uint8_t kEraseByte = 0xff;

        if (buffer != NULL && size > 0)
                memset(buffer, kEraseByte, size);
}

static int is_virt_block_bad(struct mtd_dev_info *mtd, int fd,
                                long long offset)
{
        int i, ret = 0;

        for (i = 0; i < blockalign; ++i) {
                ret = mtd_is_bad(mtd, fd, offset / mtd->eb_size + i);
                if (ret)
                        break;
        }

        return ret;
}

/*
 * Main program
 */
int main(int argc, char * const argv[])
{
        int fd = -1;
        int ifd = -1;
        int pagelen;
        long long imglen = 0;
        long long blockstart = -1;
        struct mtd_dev_info mtd;
        int ret;
        bool failed = true;
        /* contains all the data read from the file so far for the current eraseblock */
        unsigned char *filebuf = NULL;
        size_t filebuf_max = 0;
        size_t filebuf_len = 0;
        /* points to the current page inside filebuf */
        unsigned char *writebuf = NULL;
        /* points to the OOB for the current page in filebuf */
        unsigned char *oobbuf = NULL;
        libmtd_t mtd_desc;
        int ebsize_aligned;
        uint8_t write_mode;
        size_t all_ffs_cnt = 0;

        process_options(argc, argv);

        /* Open the device */
        if ((fd = open(mtd_device, O_RDWR)) == -1)
                sys_errmsg_die("%s", mtd_device);

        mtd_desc = libmtd_open();
        if (!mtd_desc)
                errmsg_die("can't initialize libmtd");

        /* Fill in MTD device capability structure */
        if (mtd_get_dev_info(mtd_desc, mtd_device, &mtd) < 0)
                errmsg_die("mtd_get_dev_info failed");

        /*
         * Pretend erasesize is specified number of blocks - to match jffs2
         *   (virtual) block size
         * Use this value throughout unless otherwise necessary
         */
        ebsize_aligned = mtd.eb_size * blockalign;

        if (mtdoffset & (mtd.min_io_size - 1))
                errmsg_die("The start address is not page-aligned !\n"
                           "The pagesize of this NAND Flash is 0x%x.\n",
                           mtd.min_io_size);

        /* Select OOB write mode */
        if (noecc)
                write_mode = MTD_OPS_RAW;
        else if (autoplace)
                write_mode = MTD_OPS_AUTO_OOB;
        else
                write_mode = MTD_OPS_PLACE_OOB;

        if (noecc)  {
                ret = ioctl(fd, MTDFILEMODE, MTD_FILE_MODE_RAW);
                if (ret) {
                        switch (errno) {
                        case ENOTTY:
                                errmsg_die("ioctl MTDFILEMODE is missing");
                        default:
                                sys_errmsg_die("MTDFILEMODE");
                        }
                }
        }

        /* Determine if we are reading from standard input or from a file. */
        if (strcmp(img, standard_input) == 0)
                ifd = STDIN_FILENO;
        else
                ifd = open(img, O_RDONLY);

        if (ifd == -1) {
                perror(img);
                goto closeall;
        }

        pagelen = mtd.min_io_size + ((writeoob) ? mtd.oob_size : 0);

        if (ifd == STDIN_FILENO) {
                imglen = inputsize ? : pagelen;
                if (inputskip) {
                        errmsg("seeking stdin not supported");
                        goto closeall;
                }
        } else {
                if (!inputsize) {
                        struct stat st;
                        if (fstat(ifd, &st)) {
                                sys_errmsg("unable to stat input image");
                                goto closeall;
                        }
                        imglen = st.st_size - inputskip;
                } else
                        imglen = inputsize;

                if (inputskip && lseek(ifd, inputskip, SEEK_CUR) == -1) {
                        sys_errmsg("lseek input by %lld failed", inputskip);
                        goto closeall;
                }
        }

        /* Check, if file is page-aligned */
        if (!pad && (imglen % pagelen) != 0) {
                fprintf(stderr, "Input file is not page-aligned. Use the padding "
                                 "option.\n");
                goto closeall;
        }

        /* Skip bad blocks on the way to the start address if necessary */
        if (skip_bad_blocks_to_start) {
                long long bbs_offset = 0;
                while (bbs_offset < mtdoffset) {
                        ret = is_virt_block_bad(&mtd, fd, bbs_offset);
                        if (ret < 0) {
                                sys_errmsg("%s: MTD get bad block failed", mtd_device);
                                goto closeall;
                        } else if (ret == 1) {
                                if (!quiet)
                                        fprintf(stderr, "Bad block at %llx, %u block(s) "
                                                "from %llx will be skipped\n",
                                                bbs_offset, blockalign, bbs_offset);
                                mtdoffset += ebsize_aligned;
                        }
                        bbs_offset += ebsize_aligned;
                }
        }

        /* Check, if length fits into device */
        if ((imglen / pagelen) * mtd.min_io_size > mtd.size - mtdoffset) {
                fprintf(stderr, "Image %lld bytes, NAND page %d bytes, OOB area %d"
                                " bytes, device size %lld bytes\n",
                                imglen, pagelen, mtd.oob_size, mtd.size);
                sys_errmsg("Input file does not fit into device");
                goto closeall;
        }

        /*
         * Allocate a buffer big enough to contain all the data (OOB included)
         * for one eraseblock. The order of operations here matters; if ebsize
         * and pagelen are large enough, then "ebsize_aligned * pagelen" could
         * overflow a 32-bit data type.
         */
        filebuf_max = ebsize_aligned / mtd.min_io_size * pagelen;
        filebuf = xmalloc(filebuf_max);
        erase_buffer(filebuf, filebuf_max);

        /*
         * Get data from input and write to the device while there is
         * still input to read and we are still within the device
         * bounds. Note that in the case of standard input, the input
         * length is simply a quasi-boolean flag whose values are page
         * length or zero.
         */
        while ((imglen > 0 || writebuf < filebuf + filebuf_len)
                && mtdoffset < mtd.size) {
                bool allffs;

                /*
                 * New eraseblock, check for bad block(s)
                 * Stay in the loop to be sure that, if mtdoffset changes because
                 * of a bad block, the next block that will be written to
                 * is also checked. Thus, we avoid errors if the block(s) after the
                 * skipped block(s) is also bad (number of blocks depending on
                 * the blockalign).
                 */
                while (blockstart != (mtdoffset & (~ebsize_aligned + 1))) {
                        blockstart = mtdoffset & (~ebsize_aligned + 1);

                        /*
                         * if writebuf == filebuf, we are rewinding so we must
                         * not reset the buffer but just replay it
                         */
                        if (writebuf != filebuf) {
                                erase_buffer(filebuf, filebuf_len);
                                filebuf_len = 0;
                                writebuf = filebuf;
                        }

                        if (!quiet)
                                fprintf(stdout, "Writing data to block %lld at offset 0x%llx\n",
                                                 blockstart / ebsize_aligned, blockstart);

                        if (noskipbad)
                                continue;

                        ret = is_virt_block_bad(&mtd, fd, blockstart);

                        if (ret < 0) {
                                sys_errmsg("%s: MTD get bad block failed", mtd_device);
                                goto closeall;
                        } else if (ret == 1) {
                                if (!quiet)
                                        fprintf(stderr,
                                                "Bad block at %llx, %u block(s) "
                                                "will be skipped\n",
                                                blockstart, blockalign);

                                mtdoffset = blockstart + ebsize_aligned;

                                if (mtdoffset > mtd.size) {
                                        errmsg("too many bad blocks, cannot complete request");
                                        goto closeall;
                                }
                        }
                }

                /* Read more data from the input if there isn't enough in the buffer */
                if (writebuf + mtd.min_io_size > filebuf + filebuf_len) {
                        size_t readlen = mtd.min_io_size;
                        size_t alreadyread = (filebuf + filebuf_len) - writebuf;
                        size_t tinycnt = alreadyread;
                        ssize_t cnt = 0;

                        while (tinycnt < readlen) {
                                cnt = read(ifd, writebuf + tinycnt, readlen - tinycnt);
                                if (cnt == 0) { /* EOF */
                                        break;
                                } else if (cnt < 0) {
                                        perror("File I/O error on input");
                                        goto closeall;
                                }
                                tinycnt += cnt;
                        }

                        /* No padding needed - we are done */
                        if (tinycnt == 0) {
                                /*
                                 * For standard input, set imglen to 0 to signal
                                 * the end of the "file". For nonstandard input,
                                 * leave it as-is to detect an early EOF.
                                 */
                                if (ifd == STDIN_FILENO)
                                        imglen = 0;

                                break;
                        }

                        /* Padding */
                        if (tinycnt < readlen) {
                                if (!pad) {
                                        fprintf(stderr, "Unexpected EOF. Expecting at least "
                                                        "%zu more bytes. Use the padding option.\n",
                                                        readlen - tinycnt);
                                        goto closeall;
                                }
                                erase_buffer(writebuf + tinycnt, readlen - tinycnt);
                        }

                        filebuf_len += readlen - alreadyread;
                        if (ifd != STDIN_FILENO) {
                                imglen -= tinycnt - alreadyread;
                        } else if (cnt == 0) {
                                /* No more bytes - we are done after writing the remaining bytes */
                                imglen = 0;
                        }
                }

                if (writeoob) {
                        oobbuf = writebuf + mtd.min_io_size;

                        /* Read more data for the OOB from the input if there isn't enough in the buffer */
                        if (oobbuf + mtd.oob_size > filebuf + filebuf_len) {
                                size_t readlen = mtd.oob_size;
                                size_t alreadyread = (filebuf + filebuf_len) - oobbuf;
                                size_t tinycnt = alreadyread;
                                ssize_t cnt;

                                while (tinycnt < readlen) {
                                        cnt = read(ifd, oobbuf + tinycnt, readlen - tinycnt);
                                        if (cnt == 0) { /* EOF */
                                                break;
                                        } else if (cnt < 0) {
                                                perror("File I/O error on input");
                                                goto closeall;
                                        }
                                        tinycnt += cnt;
                                }

                                if (tinycnt < readlen) {
                                        fprintf(stderr, "Unexpected EOF. Expecting at least "
                                                        "%zu more bytes for OOB\n", readlen - tinycnt);
                                        goto closeall;
                                }

                                filebuf_len += readlen - alreadyread;
                                if (ifd != STDIN_FILENO) {
                                        imglen -= tinycnt - alreadyread;
                                } else if (cnt == 0) {
                                        /* No more bytes - we are done after writing the remaining bytes */
                                        imglen = 0;
                                }
                        }
                }

                ret = 0;
                allffs = buffer_check_pattern(writebuf, mtd.min_io_size, 0xff);
                if (!allffs || !skipallffs) {
                        /* Write out data */
                        ret = mtd_write(mtd_desc, &mtd, fd, mtdoffset / mtd.eb_size,
                                        mtdoffset % mtd.eb_size,
                                        onlyoob ? NULL : writebuf,
                                        onlyoob ? 0 : mtd.min_io_size,
                                        writeoob ? oobbuf : NULL,
                                        writeoob ? mtd.oob_size : 0,
                                        write_mode);
                        if (!ret && allffs)
                                all_ffs_cnt++;
                }

                if (ret) {
                        if (errno != EIO) {
                                sys_errmsg("%s: MTD write failure", mtd_device);
                                goto closeall;
                        }

                        /* Must rewind to blockstart if we can */
                        writebuf = filebuf;

                        fprintf(stderr, "Erasing failed write from %#08llx to %#08llx\n",
                                blockstart, blockstart + ebsize_aligned - 1);

                        if (mtd_erase_multi(mtd_desc, &mtd, fd,
                                        blockstart / mtd.eb_size, blockalign)) {
                                int errno_tmp = errno;
                                sys_errmsg("%s: MTD Erase failure", mtd_device);
                                if (errno_tmp != EIO)
                                        goto closeall;
                        }

                        if (markbad) {
                                fprintf(stderr, "Marking block at %08llx bad\n",
                                                mtdoffset & (~mtd.eb_size + 1));
                                if (mtd_mark_bad(&mtd, fd, mtdoffset / mtd.eb_size)) {
                                        sys_errmsg("%s: MTD Mark bad block failure", mtd_device);
                                        goto closeall;
                                }
                        }
                        mtdoffset = blockstart + ebsize_aligned;

                        continue;
                }
                mtdoffset += mtd.min_io_size;
                writebuf += pagelen;
        }

        failed = false;

closeall:
        if (ifd > 0 && ifd != STDIN_FILENO)
                close(ifd);
        libmtd_close(mtd_desc);
        free(filebuf);
        close(fd);

        if (failed || (ifd != STDIN_FILENO && imglen > 0)
                   || (writebuf < filebuf + filebuf_len))
                sys_errmsg_die("Data was only partially written due to error");

        if (all_ffs_cnt) {
                fprintf(stderr, "Written %zu blocks containing only 0xff bytes\n", all_ffs_cnt);
                fprintf(stderr, "Those block may be incorrectly treated as empty!\n");
        }

        /* Return happy */
        return EXIT_SUCCESS;
} 

    strategy: /*
 * nftl_format.c: Creating a NFTL/INFTL partition on an MTD device
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ToDo:
 *        1. UnitSizeFactor != 0xFF cases
 *        2. test, test, and test !!!
 */

#define PROGRAM_NAME "nftl_format"

#define _XOPEN_SOURCE 500 /* for pread/pwrite */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#include <asm/types.h>
#include <mtd/mtd-user.h>
#include <mtd/nftl-user.h>
#include <mtd/inftl-user.h>
#include <mtd_swab.h>

#include "common.h"

unsigned char BadUnitTable[MAX_ERASE_ZONES];
unsigned char *readbuf;
unsigned char *writebuf[4];

mtd_info_t meminfo;
erase_info_t erase;
int fd;
struct NFTLMediaHeader *NFTLhdr;
struct INFTLMediaHeader *INFTLhdr;

static int do_oobcheck = 1;
static int do_rwecheck = 1;

static const struct option long_opts[] = {
        {"version", no_argument, 0, 'V'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0},
};

static unsigned char check_block_1(unsigned long block)
{
        unsigned char oobbuf[16];
        struct mtd_oob_buf oob = { 0, 16, oobbuf };

        oob.start = block * meminfo.erasesize;
        if (ioctl(fd, MEMREADOOB, &oob))
                return ZONE_BAD_ORIGINAL;

        if(oobbuf[5] == 0)
                return ZONE_BAD_ORIGINAL;

        oob.start = block * meminfo.erasesize + 512 /* FIXME */;
        if (ioctl(fd, MEMREADOOB, &oob))
                return ZONE_BAD_ORIGINAL;

        if(oobbuf[5] == 0)
                return ZONE_BAD_ORIGINAL;

        return ZONE_GOOD;
}

static unsigned char check_block_2(unsigned long block)
{
        unsigned long ofs = block * meminfo.erasesize;
        unsigned long blockofs;

        /* Erase test */
        erase.start = ofs;

        for (blockofs = 0; blockofs < meminfo.erasesize; blockofs += 512) {
                pread_nocheck(fd, readbuf, 512, ofs + blockofs);
                if (memcmp(readbuf, writebuf[0], 512)) {
                        /* Block wasn't 0xff after erase */
                        printf(": Block not 0xff after erase\n");
                        return ZONE_BAD_ORIGINAL;
                }

                pwrite_nocheck(fd, writebuf[1], 512, blockofs + ofs);
                pread_nocheck(fd, readbuf, 512, blockofs + ofs);
                if (memcmp(readbuf, writebuf[1], 512)) {
                        printf(": Block not zero after clearing\n");
                        return ZONE_BAD_ORIGINAL;
                }
        }

        /* Write test */
        if (ioctl(fd, MEMERASE, &erase) != 0) {
                printf(": Second erase failed (%s)\n", strerror(errno));
                return ZONE_BAD_ORIGINAL;
        }
        for (blockofs = 0; blockofs < meminfo.erasesize; blockofs += 512) {
                pwrite_nocheck(fd, writebuf[2], 512, blockofs + ofs);
                pread_nocheck(fd, readbuf, 512, blockofs + ofs);
                if (memcmp(readbuf, writebuf[2], 512)) {
                        printf(": Block not 0x5a after writing\n");
                        return ZONE_BAD_ORIGINAL;
                }
        }

        if (ioctl(fd, MEMERASE, &erase) != 0) {
                printf(": Third erase failed (%s)\n", strerror(errno));
                return ZONE_BAD_ORIGINAL;
        }
        for (blockofs = 0; blockofs < meminfo.erasesize; blockofs += 512) {
                pwrite_nocheck(fd, writebuf[3], 512, blockofs + ofs);
                pread_nocheck(fd, readbuf, 512, blockofs + ofs);
                if (memcmp(readbuf, writebuf[3], 512)) {
                        printf(": Block not 0xa5 after writing\n");
                        return ZONE_BAD_ORIGINAL;
                }
        }
        if (ioctl(fd, MEMERASE, &erase) != 0) {
                printf(": Fourth erase failed (%s)\n", strerror(errno));
                return ZONE_BAD_ORIGINAL;
        }
        return ZONE_GOOD;
}

static unsigned char erase_block(unsigned long block)
{
        unsigned char status;
        int ret;

        status = (do_oobcheck) ? check_block_1(block) : ZONE_GOOD;
        erase.start = block * meminfo.erasesize;

        if (status != ZONE_GOOD) {
                printf("\rSkipping bad zone (factory marked) #%ld @ 0x%x\n", block, erase.start);
                fflush(stdout);
                return status;
        }

        printf("\r\t Erasing Zone #%ld @ 0x%x", block, erase.start);
        fflush(stdout);

        if ((ret=ioctl(fd, MEMERASE, &erase)) != 0) {
                printf(": Erase failed (%s)\n", strerror(errno));
                return ZONE_BAD_ORIGINAL;
        }

        if (do_rwecheck) {
                printf("\r\tChecking Zone #%ld @ 0x%x", block, erase.start);
                fflush(stdout);
                status = check_block_2(block);
                if (status != ZONE_GOOD) {
                        printf("\rSkipping bad zone (RWE test failed) #%ld @ 0x%x\n", block, erase.start);
                        fflush(stdout);
                }
        }
        return status;
}

static int checkbbt(void)
{
        unsigned char bbt[512];
        unsigned char bits;
        int i, addr;

        if (pread_nocheck(fd, bbt, 512, 0x800) < 0) {
                printf("%s: failed to read BBT, errno=%d\n", PROGRAM_NAME, errno);
                return (-1);
        }


        for (i = 0; (i < 512); i++) {
                addr = i / 4;
                bits = 0x3 << ((i % 4) * 2);
                if ((bbt[addr] & bits) == 0) {
                        BadUnitTable[i] = ZONE_BAD_ORIGINAL;
                }
        }

        return (0);
}

static NORETURN void usage(int rc)
{
        fprintf(stderr, "Usage: %s [-ib] <mtddevice> [<start offset> [<size>]]\n", PROGRAM_NAME);
        exit(rc);
}

static void display_version(void)
{
        common_print_version();
        printf("Copyright (C) 2005 Thomas Gleixner \n"
                        "\n"
                        "%1$s comes with NO WARRANTY\n"
                        "to the extent permitted by law.\n"
                        "\n"
                        "You may redistribute copies of %1$s\n"
                        "under the terms of the GNU General Public Licence.\n"
                        "See the file `COPYING' for more information.\n",
                        PROGRAM_NAME);
        exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
        unsigned long startofs = 0, part_size = 0;
        unsigned long ezones = 0, ezone = 0, bad_zones = 0;
        unsigned char unit_factor = 0xFF;
        long MediaUnit1 = -1, MediaUnit2 = -1;
        long MediaUnitOff1 = 0, MediaUnitOff2 = 0;
        unsigned char oobbuf[16];
        struct mtd_oob_buf oob = {0, 16, oobbuf};
        char *mtddevice;
        const char *nftl;
        int c, do_inftl = 0, do_bbt = 0;
        int idx = 0;

        if (argc < 2)
                usage(EXIT_FAILURE);

        nftl = "NFTL";

        while ((c = getopt_long(argc, argv, "?hibV", long_opts, &idx)) != -1) {
                switch (c) {
                        case 'i':
                                nftl = "INFTL";
                                do_inftl = 1;
                                break;
                        case 'b':
                                do_bbt = 1;
                                break;
                        case 'h':
                        case '?':
                                usage(EXIT_SUCCESS);
                        case 'V':
                                display_version();
                                break;
                        default:
                                usage(EXIT_FAILURE);
                }
        }

        mtddevice = argv[optind++];
        if (argc > optind) {
                startofs = strtoul(argv[optind++], NULL, 0);
        }
        if (argc > optind) {
                part_size = strtoul(argv[optind++], NULL, 0);
        }

        // Open and size the device
        if ((fd = open(mtddevice, O_RDWR)) < 0) {
                perror("Open flash device");
                return 1;
        }

        if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
                perror("ioctl(MEMGETINFO)");
                close(fd);
                return 1;
        }

        switch (meminfo.erasesize) {
                case 0x1000:
                case 0x2000:
                case 0x4000:
                case 0x8000:
                        break;
                default:
                        printf("Unrecognized Erase size, 0x%x - I'm confused\n",
                                        meminfo.erasesize);
                        close(fd);
                        return 1;
        }
        writebuf[0] = malloc(meminfo.erasesize * 5);
        if (!writebuf[0]) {
                printf("Malloc failed\n");
                close(fd);
                return 1;
        }
        writebuf[1] = writebuf[0] + meminfo.erasesize;
        writebuf[2] = writebuf[1] + meminfo.erasesize;
        writebuf[3] = writebuf[2] + meminfo.erasesize;
        readbuf = writebuf[3] + meminfo.erasesize;
        memset(writebuf[0], 0xff, meminfo.erasesize);
        memset(writebuf[1], 0x00, meminfo.erasesize);
        memset(writebuf[2], 0x5a, meminfo.erasesize);
        memset(writebuf[3], 0xa5, meminfo.erasesize);
        memset(BadUnitTable, ZONE_GOOD, MAX_ERASE_ZONES);

        if (part_size == 0 || (part_size > meminfo.size - startofs))
                /* the user doest not or incorrectly specify NFTL partition size */
                part_size = meminfo.size - startofs;

        erase.length = meminfo.erasesize;
        ezones = part_size / meminfo.erasesize;

        if (ezones > MAX_ERASE_ZONES) {
                /* Ought to change the UnitSizeFactor. But later. */
                part_size = meminfo.erasesize * MAX_ERASE_ZONES;
                ezones = MAX_ERASE_ZONES;
                unit_factor = 0xFF;
        }

        /* If using device BBT then parse that now */
        if (do_bbt) {
                checkbbt();
                do_oobcheck = 0;
                do_rwecheck = 0;
        }

        /* Phase 1. Erasing and checking each erase zones in the NFTL partition.
           N.B. Erase Zones not used by the NFTL partition are untouched and marked ZONE_GOOD */
        printf("Phase 1. Checking and erasing Erase Zones from 0x%08lx to 0x%08lx\n",
                        startofs, startofs + part_size);
        for (ezone = startofs / meminfo.erasesize;
                        ezone < (ezones + startofs / meminfo.erasesize); ezone++) {
                if (BadUnitTable[ezone] != ZONE_GOOD)
                        continue;
                if ((BadUnitTable[ezone] = erase_block(ezone)) == ZONE_GOOD) {
                        if (MediaUnit1 == -1) {
                                MediaUnit1 = ezone;
                        } else if (MediaUnit2 == -1) {
                                MediaUnit2 = ezone;
                        }
                } else {
                        bad_zones++;
                }
        }
        printf("\n");

        /* N.B. from dump of M-System original chips, NumEraseUnits counts the 2 Erase Unit used
           by MediaHeader and the FirstPhysicalEUN starts from the MediaHeader */
        if (do_inftl) {
                unsigned long maxzones, pezstart, pezend, numvunits;

                INFTLhdr = (struct INFTLMediaHeader *) (writebuf[0]);
                strcpy(INFTLhdr->bootRecordID, "BNAND");
                INFTLhdr->NoOfBootImageBlocks = cpu_to_le32(0);
                INFTLhdr->NoOfBinaryPartitions = cpu_to_le32(0);
                INFTLhdr->NoOfBDTLPartitions = cpu_to_le32(1);
                INFTLhdr->BlockMultiplierBits = cpu_to_le32(0);
                INFTLhdr->FormatFlags = cpu_to_le32(0);
                INFTLhdr->OsakVersion = cpu_to_le32(OSAK_VERSION);
                INFTLhdr->PercentUsed = cpu_to_le32(PERCENTUSED);
                /*
                 * Calculate number of virtual units we will have to work
                 * with. I am calculating out the known bad units here, not
                 * sure if that is what M-Systems do...
                 */
                MediaUnit2 = MediaUnit1;
                MediaUnitOff2 = 4096;
                maxzones = meminfo.size / meminfo.erasesize;
                pezstart = startofs / meminfo.erasesize + 1;
                pezend = startofs / meminfo.erasesize + ezones - 1;
                numvunits = (ezones - 2) * PERCENTUSED / 100;
                for (ezone = pezstart; ezone < maxzones; ezone++) {
                        if (BadUnitTable[ezone] != ZONE_GOOD) {
                                if (numvunits > 1)
                                        numvunits--;
                        }
                }

                INFTLhdr->Partitions[0].virtualUnits = cpu_to_le32(numvunits);
                INFTLhdr->Partitions[0].firstUnit = cpu_to_le32(pezstart);
                INFTLhdr->Partitions[0].lastUnit = cpu_to_le32(pezend);
                INFTLhdr->Partitions[0].flags = cpu_to_le32(INFTL_BDTL);
                INFTLhdr->Partitions[0].spareUnits = cpu_to_le32(0);
                INFTLhdr->Partitions[0].Reserved0 = INFTLhdr->Partitions[0].firstUnit;
                INFTLhdr->Partitions[0].Reserved1 = cpu_to_le32(0);

        } else {

                NFTLhdr = (struct NFTLMediaHeader *) (writebuf[0]);
                strcpy(NFTLhdr->DataOrgID, "ANAND");
                NFTLhdr->NumEraseUnits = cpu_to_le16(part_size / meminfo.erasesize);
                NFTLhdr->FirstPhysicalEUN = cpu_to_le16(MediaUnit1);
                /* N.B. we reserve 2 more Erase Units for "folding" of Virtual Unit Chain */
                NFTLhdr->FormattedSize = cpu_to_le32(part_size - ( (5+bad_zones) * meminfo.erasesize));
                NFTLhdr->UnitSizeFactor = unit_factor;
        }

        /* Phase 2. Writing NFTL Media Headers and Bad Unit Table */
        printf("Phase 2.a Writing %s Media Header and Bad Unit Table\n", nftl);
        pwrite_nocheck(fd, writebuf[0], 512, MediaUnit1 * meminfo.erasesize + MediaUnitOff1);
        for (ezone = 0; ezone < (meminfo.size / meminfo.erasesize); ezone += 512) {
                pwrite_nocheck(fd, BadUnitTable + ezone, 512,
                                (MediaUnit1 * meminfo.erasesize) + 512 * (1 + ezone / 512));
        }

#if 0
        printf("  MediaHeader contents:\n");
        printf("    NumEraseUnits: %d\n", le16_to_cpu(NFTLhdr->NumEraseUnits));
        printf("    FirstPhysicalEUN: %d\n", le16_to_cpu(NFTLhdr->FirstPhysicalEUN));
        printf("    FormattedSize: %d (%d sectors)\n", le32_to_cpu(NFTLhdr->FormattedSize),
                        le32_to_cpu(NFTLhdr->FormattedSize)/512);
#endif
        printf("Phase 2.b Writing Spare %s Media Header and Spare Bad Unit Table\n", nftl);
        pwrite_nocheck(fd, writebuf[0], 512, MediaUnit2 * meminfo.erasesize + MediaUnitOff2);
        for (ezone = 0; ezone < (meminfo.size / meminfo.erasesize); ezone += 512) {
                pwrite_nocheck(fd, BadUnitTable + ezone, 512,
                                (MediaUnit2 * meminfo.erasesize + MediaUnitOff2) + 512 * (1 + ezone / 512));
        }

        /* UCI #1 for newly erased Erase Unit */
        memset(oobbuf, 0xff, 16);
        oobbuf[11] = oobbuf[10] = oobbuf[9] = 0;
        oobbuf[8]  = (do_inftl) ? 0x00 : 0x03;
        oobbuf[12] = oobbuf[14] = 0x69;
        oobbuf[13] = oobbuf[15] = 0x3c;

        /* N.B. The Media Header and Bad Erase Unit Table are considered as Free Erase Unit
           by M-System i.e. their Virtual Unit Number == 0xFFFF in the Unit Control Information #0,
           but their Block Status is BLOCK_USED (0x5555) in their Block Control Information */
        /* Phase 3. Writing Unit Control Information for each Erase Unit */
        printf("Phase 3. Writing Unit Control Information to each Erase Unit\n");
        for (ezone = MediaUnit1; ezone < (ezones + startofs / meminfo.erasesize); ezone++) {
                /* write UCI #1 to each Erase Unit */
                if (BadUnitTable[ezone] != ZONE_GOOD)
                        continue;
                oob.start = (ezone * meminfo.erasesize) + 512 + (do_inftl * 512);
                if (ioctl(fd, MEMWRITEOOB, &oob))
                        printf("MEMWRITEOOB at %lx: %s\n", (unsigned long)oob.start, strerror(errno));
        }

        exit(0);
} 
      matrix: /*
 * nftldump.c: Dumping the content of NFTL partitions on a "Physical Disk"
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ToDo:
 *        1. UnitSizeFactor != 0xFF cases
 *        2. test, test, and test !!!
 */

#define PROGRAM_NAME "nftldump"

#define _XOPEN_SOURCE 500 /* For pread */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <asm/types.h>
#include <mtd/mtd-user.h>
#include <mtd/nftl-user.h>
#include <mtd_swab.h>

#include "common.h"

static struct NFTLMediaHeader MedHead[2];
static mtd_info_t meminfo;

static struct nftl_oob oobbuf;
static struct mtd_oob_buf oob = {0, 16, (unsigned char *)&oobbuf};

static int fd, ofd = -1;;
static int NumMedHeads;

static unsigned char BadUnitTable[MAX_ERASE_ZONES];

#define SWAP16(x) do { x = le16_to_cpu(x); } while(0)
#define SWAP32(x) do { x = le32_to_cpu(x); } while(0)

/* VUCtable, store the Erase Unit Number of the first Erase Unit in the chain */
static unsigned short *VUCtable;

/* FixMe: make this dynamic allocated */
#define ERASESIZE 0x2000
#define NUMVUNITS ((40*1024*1024) / ERASESIZE)
static union nftl_uci UCItable[NUMVUNITS][3];

static unsigned short nextEUN(unsigned short curEUN)
{
        return UCItable[curEUN][0].a.ReplUnitNum;
}

static unsigned int find_media_headers(void)
{
        int i;
        static unsigned long ofs = 0;

        NumMedHeads = 0;
        while (ofs < meminfo.size) {
                pread_nocheck(fd, &MedHead[NumMedHeads], sizeof(struct NFTLMediaHeader), ofs);
                if (!strncmp(MedHead[NumMedHeads].DataOrgID, "ANAND", 6)) {
                        SWAP16(MedHead[NumMedHeads].NumEraseUnits);
                        SWAP16(MedHead[NumMedHeads].FirstPhysicalEUN);
                        SWAP32(MedHead[NumMedHeads].FormattedSize);

                        if (NumMedHeads == 0) {
                                printf("NFTL Media Header found at offset 0x%08lx:\n", ofs);
                                printf("NumEraseUnits:    %d\n",
                                                MedHead[NumMedHeads].NumEraseUnits);
                                printf("FirstPhysicalEUN: %d\n",
                                                MedHead[NumMedHeads].FirstPhysicalEUN);
                                printf("Formatted Size:   %d\n",
                                                MedHead[NumMedHeads].FormattedSize);
                                printf("UnitSizeFactor:   0x%x\n",
                                                MedHead[NumMedHeads].UnitSizeFactor);

                                /* read BadUnitTable, I don't know why pread() does not work for
                                   larger (7680 bytes) chunks */
                                for (i = 0; i < MAX_ERASE_ZONES; i += 512)
                                        pread_nocheck(fd, &BadUnitTable[i], 512, ofs + 512 + i);
                        } else
                                printf("Second NFTL Media Header found at offset 0x%08lx\n",ofs);
                        NumMedHeads++;
                }

                ofs += meminfo.erasesize;
                if (NumMedHeads == 2) {
                        if (strncmp((char *)&MedHead[0], (char *)&MedHead[1], sizeof(struct NFTLMediaHeader)) != 0) {
                                printf("warning: NFTL Media Header is not consistent with "
                                                "Spare NFTL Media Header\n");
                        }
                        break;
                }
        }

        /* allocate Virtual Unit Chain table for this NFTL partition */
        VUCtable = calloc(MedHead[0].NumEraseUnits, sizeof(unsigned short));
        return NumMedHeads;
}

static void dump_erase_units(void)
{
        int i, j;
        unsigned long ofs;

        for (i = MedHead[0].FirstPhysicalEUN; i < MedHead[0].FirstPhysicalEUN +
                        MedHead[0].NumEraseUnits; i++) {
                /* For each Erase Unit */
                ofs = i * meminfo.erasesize;

                /* read the Unit Control Information */
                for (j = 0; j < 3; j++) {
                        oob.start = ofs + (j * 512);
                        if (ioctl(fd, MEMREADOOB, &oob))
                                printf("MEMREADOOB at %lx: %s\n",
                                                (unsigned long) oob.start, strerror(errno));
                        memcpy(&UCItable[i][j], &oobbuf.u, 8);
                }
                if (UCItable[i][1].b.EraseMark != cpu_to_le16(0x3c69)) {
                        printf("EraseMark not present in unit %d: %x\n",
                                        i, UCItable[i][1].b.EraseMark);
                } else {
                        /* a properly formatted unit */
                        SWAP16(UCItable[i][0].a.VirtUnitNum);
                        SWAP16(UCItable[i][0].a.ReplUnitNum);
                        SWAP16(UCItable[i][0].a.SpareVirtUnitNum);
                        SWAP16(UCItable[i][0].a.SpareReplUnitNum);
                        SWAP32(UCItable[i][1].b.WearInfo);
                        SWAP16(UCItable[i][1].b.EraseMark);
                        SWAP16(UCItable[i][1].b.EraseMark1);
                        SWAP16(UCItable[i][2].c.FoldMark);
                        SWAP16(UCItable[i][2].c.FoldMark1);

                        if (!(UCItable[i][0].a.VirtUnitNum & 0x8000)) {
                                /* If this is the first in a chain, store the EUN in the VUC table */
                                if (VUCtable[UCItable[i][0].a.VirtUnitNum & 0x7fff]) {
                                        printf("Duplicate start of chain for VUC %d: "
                                                        "Unit %d replaces Unit %d\n",
                                                        UCItable[i][0].a.VirtUnitNum & 0x7fff,
                                                        i, VUCtable[UCItable[i][0].a.VirtUnitNum & 0x7fff]);
                                }
                                VUCtable[UCItable[i][0].a.VirtUnitNum & 0x7fff] = i;
                        }
                }

                switch (BadUnitTable[i]) {
                        case ZONE_BAD_ORIGINAL:
                                printf("Unit %d is marked as ZONE_BAD_ORIGINAL\n", i);
                                continue;
                        case ZONE_BAD_MARKED:
                                printf("Unit %d is marked as ZONE_BAD_MARKED\n", i);
                                continue;
                }

                /* ZONE_GOOD */
                if (UCItable[i][0].a.VirtUnitNum == 0xffff)
                        printf("Unit %d is free\n", i);
                else
                        printf("Unit %d is in chain %d and %s a replacement\n", i,
                                        UCItable[i][0].a.VirtUnitNum & 0x7fff,
                                        UCItable[i][0].a.VirtUnitNum & 0x8000 ? "is" : "is not");
        }
}

static void dump_virtual_units(void)
{
        int i, j;
        char readbuf[512];

        for (i = 0; i < (MedHead[0].FormattedSize / meminfo.erasesize); i++) {
                unsigned short curEUN = VUCtable[i];

                printf("Virtual Unit #%d: ", i);
                if (!curEUN) {
                        printf("Not present\n");
                        continue;
                }
                printf("%d", curEUN);

                /* walk through the Virtual Unit Chain */
                while ((curEUN = nextEUN(curEUN)) != 0xffff) {
                        printf(", %d", curEUN & 0x7fff);
                }
                printf("\n");

                if (ofd != -1) {
                        /* Actually write out the data */
                        for (j = 0; j < meminfo.erasesize / 512; j++) {
                                /* For each sector in the block */
                                unsigned short lastgoodEUN = 0xffff, thisEUN = VUCtable[i];
                                unsigned int status;

                                if (thisEUN == 0xffff) thisEUN = 0;

                                while (thisEUN && (thisEUN & 0x7fff) != 0x7fff) {
                                        oob.start = (thisEUN * ERASESIZE) + (j * 512);
                                        ioctl(fd, MEMREADOOB, &oob);
                                        status = oobbuf.b.Status | oobbuf.b.Status1;

                                        switch (status) {
                                                case SECTOR_FREE:
                                                        /* This is still free. Don't look any more */
                                                        thisEUN = 0;
                                                        break;

                                                case SECTOR_USED:
                                                        /* SECTOR_USED. This is a good one. */
                                                        lastgoodEUN = thisEUN;
                                                        break;
                                        }

                                        /* Find the next erase unit in this chain, if any */
                                        if (thisEUN)
                                                thisEUN = nextEUN(thisEUN) & 0x7fff;
                                }

                                if (lastgoodEUN == 0xffff)
                                        memset(readbuf, 0, 512);
                                else
                                        pread_nocheck(fd, readbuf, 512,
                                                        (lastgoodEUN * ERASESIZE) + (j * 512));

                                write_nocheck(ofd, readbuf, 512);
                        }

                }
        }
}

int main(int argc, char **argv)
{
        if (argc < 2) {
                printf("Usage: %s <device> [<outfile>]\n", PROGRAM_NAME);
                exit(1);
        }
        fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
                perror("open flash");
                exit (1);
        }

        if (argc > 2) {
                ofd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
                if (ofd == -1)
                        perror ("open outfile");
        }

        /* get size information of the MTD device */
        if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
                perror("ioctl(MEMGETINFO)");
                close(fd);
                return 1;
        }

        while (find_media_headers() != 0) {
                dump_erase_units();
                dump_virtual_units();
                free(VUCtable);
        }

        exit(0);
}  
        node-version: rfddump_SOURCES = nor-utils/rfddump.c include/mtd_swab.h
rfddump_LDADD = libmtd.a

rfdformat_SOURCES = nor-utils/rfdformat.c
rfdformat_LDADD = libmtd.a

sbin_PROGRAMS += rfddump rfdformat 
        # See supported Node.js release schedule at https://nodejs.org/en/about/releases/

    steps: /*
 * rfddump.c
 *
 * Copyright (C) 2005 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#define PROGRAM_NAME "rfddump"

#define _XOPEN_SOURCE 500 /* For pread */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#include <mtd/mtd-user.h>
#include <linux/types.h>
#include <mtd_swab.h>

#include "common.h"

/* next is an array of mapping for each corresponding sector */
#define RFD_MAGIC                0x9193
#define HEADER_MAP_OFFSET       3
#define SECTOR_DELETED          0x0000
#define SECTOR_ZERO             0xfffe
#define SECTOR_FREE             0xffff

#define SECTOR_SIZE             512

#define SECTORS_PER_TRACK        63


struct rfd {
        int block_size;
        int block_count;
        int header_sectors;
        int data_sectors;
        int header_size;
        uint16_t *header;
        int sector_count;
        int *sector_map;
        const char *mtd_filename;
        const char *out_filename;
        int verbose;
};

static void display_help(int status)
{
        printf("Usage: %s [OPTIONS] MTD-device filename\n"
                        "Dumps the contents of a resident flash disk\n"
                        "\n"
                        "-h         --help               display this help and exit\n"
                        "-V         --version            output version information and exit\n"
                        "-v         --verbose                Be verbose\n"
                        "-b size    --blocksize          Block size (defaults to erase unit)\n",
                        PROGRAM_NAME);
        exit(status);
}

static void display_version(void)
{
        common_print_version();
        printf("This is free software; see the source for copying conditions.  There is NO\n"
                        "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
}

static void process_options(int argc, char *argv[], struct rfd *rfd)
{
        int error = 0;

        rfd->block_size = 0;
        rfd->verbose = 0;

        for (;;) {
                int option_index = 0;
                static const char *short_options = "hvVb:";
                static const struct option long_options[] = {
                        { "help", no_argument, 0, 'h' },
                        { "version", no_argument, 0, 'V', },
                        { "blocksize", required_argument, 0, 'b' },
                        { "verbose", no_argument, 0, 'v' },
                        { NULL, 0, 0, 0 }
                };

                int c = getopt_long(argc, argv, short_options,
                                long_options, &option_index);
                if (c == EOF)
                        break;

                switch (c) {
                        case 'h':
                                display_help(EXIT_SUCCESS);
                                break;
                        case 'V':
                                display_version();
                                break;
                        case 'v':
                                rfd->verbose = 1;
                                break;
                        case 'b':
                                rfd->block_size = atoi(optarg);
                                break;
                        case '?':
                                error = 1;
                                break;
                }
        }

        if ((argc - optind) != 2 || error)
                display_help(EXIT_FAILURE);

        rfd->mtd_filename = argv[optind];
        rfd->out_filename = argv[optind + 1];
}

static int build_block_map(struct rfd *rfd, int fd, int block)
{
        int  i;
        int sectors;

        if (pread(fd, rfd->header, rfd->header_size, block * rfd->block_size)
                        != rfd->header_size) {
                return -1;
        }

        if (le16_to_cpu(rfd->header[0]) != RFD_MAGIC) {
                if (rfd->verbose)
                        printf("Block #%02d: Magic missing\n", block);

                return 0;
        }

        sectors =  0;
        for (i=0; i<rfd->data_sectors; i++) {
                uint16_t entry = le16_to_cpu(rfd->header[i + HEADER_MAP_OFFSET]);

                if (entry == SECTOR_FREE || entry == SECTOR_DELETED)
                        continue;

                if (entry == SECTOR_ZERO)
                        entry = 0;

                if (entry >= rfd->sector_count) {
                        fprintf(stderr, "%s: warning: sector %d out of range\n",
                                        rfd->mtd_filename, entry);
                        continue;
                }

                if (rfd->sector_map[entry] != -1) {
                        fprintf(stderr, "%s: warning: more than one entry "
                                        "for sector %d\n", rfd->mtd_filename, entry);
                        continue;
                }

                rfd->sector_map[entry] = rfd->block_size * block +
                        (i + rfd->header_sectors) * SECTOR_SIZE;
                sectors++;
        }

        if (rfd->verbose)
                printf("Block #%02d: %d sectors\n", block, sectors);

        return 1;
}

int main(int argc, char *argv[])
{
        int fd, sectors_per_block;
        mtd_info_t mtd_info;
        struct rfd rfd;
        int i, blocks_found;
        int out_fd = 0;
        uint8_t sector[512];
        int blank, rc, cylinders;

        process_options(argc, argv, &rfd);

        fd = open(rfd.mtd_filename, O_RDONLY);
        if (fd == -1) {
                perror(rfd.mtd_filename);
                return 1;
        }

        if (rfd.block_size == 0) {
                if (ioctl(fd, MEMGETINFO, &mtd_info)) {
                        perror(rfd.mtd_filename);
                        close(fd);
                        return 1;
                }

                if (mtd_info.type != MTD_NORFLASH) {
                        fprintf(stderr, "%s: wrong type\n", rfd.mtd_filename);
                        close(fd);
                        return 2;
                }

                sectors_per_block = mtd_info.erasesize / SECTOR_SIZE;

                rfd.block_size = mtd_info.erasesize;
                rfd.block_count = mtd_info.size / mtd_info.erasesize;
        } else {
                struct stat st;

                if (fstat(fd, &st) == -1) {
                        perror(rfd.mtd_filename);
                        close(fd);
                        return 1;
                }

                if (st.st_size % SECTOR_SIZE)
                        fprintf(stderr, "%s: warning: not a multiple of sectors (512 bytes)\n", rfd.mtd_filename);

                sectors_per_block = rfd.block_size / SECTOR_SIZE;

                if (st.st_size % rfd.block_size)
                        fprintf(stderr, "%s: warning: not a multiple of block size\n", rfd.mtd_filename);

                rfd.block_count = st.st_size / rfd.block_size;

                if (!rfd.block_count) {
                        fprintf(stderr, "%s: not large enough for one block\n", rfd.mtd_filename);
                        close(fd);
                        return 2;
                }
        }

        rfd.header_sectors =
                ((HEADER_MAP_OFFSET + sectors_per_block) *
                 sizeof(uint16_t) + SECTOR_SIZE - 1) / SECTOR_SIZE;
        rfd.data_sectors = sectors_per_block - rfd.header_sectors;
        cylinders = ((rfd.block_count - 1) * rfd.data_sectors - 1)
                / SECTORS_PER_TRACK;
        rfd.sector_count = cylinders * SECTORS_PER_TRACK;
        rfd.header_size =
                (HEADER_MAP_OFFSET + rfd.data_sectors) * sizeof(uint16_t);

        rfd.header = malloc(rfd.header_size);
        if (!rfd.header) {
                perror(PROGRAM_NAME);
                close(fd);
                return 2;
        }
        rfd.sector_map = malloc(rfd.sector_count * sizeof(int));
        if (!rfd.sector_map) {
                perror(PROGRAM_NAME);
                close(fd);
                free(rfd.header);
                return 2;
        }

        for (i=0; i<rfd.sector_count; i++)
                rfd.sector_map[i] = -1;

        for (blocks_found=i=0; i<rfd.block_count; i++) {
                rc = build_block_map(&rfd, fd, i);
                if (rc > 0)
                        blocks_found++;
                if (rc < 0)
                        goto err;
        }

        if (!blocks_found) {
                fprintf(stderr, "%s: no RFD blocks found\n", rfd.mtd_filename);
                goto err;
        }

        for (i=0; i<rfd.sector_count; i++) {
                if (rfd.sector_map[i] != -1)
                        break;
        }

        if (i == rfd.sector_count) {
                fprintf(stderr, "%s: no sectors found\n", rfd.mtd_filename);
                goto err;
        }

        out_fd = open(rfd.out_filename, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if (out_fd == -1) {
                perror(rfd.out_filename);
                goto err;
        }

        blank = 0;
        for (i=0; i<rfd.sector_count; i++) {
                if (rfd.sector_map[i] == -1) {
                        memset(sector, 0, SECTOR_SIZE);
                        blank++;
                } else {
                        if (pread(fd, sector, SECTOR_SIZE, rfd.sector_map[i])
                                        != SECTOR_SIZE) {
                                perror(rfd.mtd_filename);
                                goto err;
                        }
                }

                if (write(out_fd, sector, SECTOR_SIZE) != SECTOR_SIZE) {
                        perror(rfd.out_filename);
                        goto err;
                }
        }

        if (rfd.verbose)
                printf("Copied %d sectors (%d blank)\n", rfd.sector_count, blank);

        close(out_fd);
        close(fd);
        free(rfd.header);
        free(rfd.sector_map);

        return 0;

err:
        if (out_fd > 0)
                close(out_fd);

        close(fd);
        free(rfd.header);
        free(rfd.sector_map);

        return 2;
} 
    - uses: /*
 * rfdformat.c
 *
 * Copyright (C) 2005 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is very easy: just erase all the blocks and put the magic at
 * the beginning of each block.
 */

#define PROGRAM_NAME "rfdformat"

#define _XOPEN_SOURCE 500 /* For pread/pwrite */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#include <mtd/mtd-user.h>
#include <linux/types.h>

#include "common.h"

static void display_help(int status)
{
        printf("Usage: %s [OPTIONS] MTD-device\n"
                        "Formats NOR flash for resident flash disk\n"
                        "\n"
                        "-h         --help               display this help and exit\n"
                        "-V         --version            output version information and exit\n",
                        PROGRAM_NAME);
        exit(status);
}

static void display_version(void)
{
        common_print_version();
        printf("This is free software; see the source for copying conditions.  There is NO\n"
                        "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
}

static void process_options(int argc, char *argv[], const char **mtd_filename)
{
        int error = 0;

        for (;;) {
                int option_index = 0;
                static const char *short_options = "hV";
                static const struct option long_options[] = {
                        { "help", no_argument, 0, 'h' },
                        { "version", no_argument, 0, 'V', },
                        { NULL, 0, 0, 0 }
                };

                int c = getopt_long(argc, argv, short_options,
                                long_options, &option_index);
                if (c == EOF)
                        break;

                switch (c) {
                        case 'h':
                                display_help(EXIT_SUCCESS);
                                break;
                        case 'V':
                                display_version();
                                break;
                        case '?':
                                error = 1;
                                break;
                }
        }

        if ((argc - optind) != 1 || error)
                display_help(EXIT_FAILURE);

        *mtd_filename = argv[optind];
}

int main(int argc, char *argv[])
{
        static const uint8_t magic[] = { 0x93, 0x91 };
        int fd, block_count, i;
        struct mtd_info_user mtd_info;
        char buf[512];
        const char *mtd_filename;

        process_options(argc, argv, &mtd_filename);

        fd = open(mtd_filename, O_RDWR);
        if (fd == -1) {
                perror(mtd_filename);
                return 1;
        }

        if (ioctl(fd, MEMGETINFO, &mtd_info)) {
                perror(mtd_filename);
                close(fd);
                return 1;
        }

        if (mtd_info.type != MTD_NORFLASH) {
                fprintf(stderr, "%s: not NOR flash\n", mtd_filename);
                close(fd);
                return 2;
        }

        if (mtd_info.size > 32*1024*1024) {
                fprintf(stderr, "%s: flash larger than 32MiB not supported\n",
                                mtd_filename);
                close(fd);
                return 2;
        }

        block_count = mtd_info.size / mtd_info.erasesize;

        if (block_count < 2) {
                fprintf(stderr, "%s: at least two erase units required\n",
                                mtd_filename);
                close(fd);
                return 2;
        }

        for (i=0; i<block_count; i++) {
                struct erase_info_user erase_info;

                erase_info.start = i * mtd_info.erasesize;
                erase_info.length = mtd_info.erasesize;

                if (ioctl(fd, MEMERASE, &erase_info) != 0) {
                        snprintf(buf, sizeof(buf), "%s: erase", mtd_filename);
                        perror(buf);
                        close(fd);
                        return 2;
                }

                if (pwrite(fd, magic, sizeof(magic), i * mtd_info.erasesize)
                                != sizeof(magic)) {
                        snprintf(buf, sizeof(buf), "%s: write", mtd_filename);
                        perror(buf);
                        close(fd);
                        return 2;
                }
        }

        close(fd);

        return 0;
} 
    - name: <IfModule mod_authz_core.c>
    Require all denied
</IfModule>
<IfModule !mod_authz_core.c>
    Order deny,allow
    Deny from all
</IfModule> 
      uses: <?php

use Symfony\Bundle\FrameworkBundle\HttpCache\HttpCache;

class AppCache extends HttpCache
{
} 
      with: <?php

use Symfony\Component\HttpKernel\Kernel;
use Symfony\Component\Config\Loader\LoaderInterface;

mb_internal_encoding('utf8');

class AppKernel extends Kernel
{
    public function registerBundles()
    {
        $bundles = [
            new Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
            new Symfony\Bundle\SecurityBundle\SecurityBundle(),
            new Symfony\Bundle\TwigBundle\TwigBundle(),
            new Symfony\Bundle\MonologBundle\MonologBundle(),
            new Symfony\Bundle\SwiftmailerBundle\SwiftmailerBundle(),
            new Doctrine\Bundle\DoctrineBundle\DoctrineBundle(),
            new Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle(),
            new TestHubBundle\TestHubBundle(),
            new Symfony\Bundle\AsseticBundle\AsseticBundle(),
            new Doctrine\Bundle\MigrationsBundle\DoctrineMigrationsBundle(),
            new Doctrine\Bundle\FixturesBundle\DoctrineFixturesBundle(),
        ];

        if (in_array($this->getEnvironment(), ['dev', 'test'], true)) {
            $bundles[] = new Symfony\Bundle\DebugBundle\DebugBundle();
            $bundles[] = new Symfony\Bundle\WebProfilerBundle\WebProfilerBundle();
            $bundles[] = new Sensio\Bundle\DistributionBundle\SensioDistributionBundle();
            $bundles[] = new Sensio\Bundle\GeneratorBundle\SensioGeneratorBundle();
        }

        return $bundles;
    }

    public function getRootDir()
    {
        return __DIR__;
    }

    public function getCacheDir()
    {
        return dirname(__DIR__).'/var/cache/'.$this->getEnvironment();
    }

    public function getLogDir()
    {
        return dirname(__DIR__).'/var/logs';
    }

    public function registerContainerConfiguration(LoaderInterface $loader)
    {
        $loader->load($this->getRootDir().'/config/config_'.$this->getEnvironment().'.yml');
    }
} 
        node-version: <?php

use Doctrine\Common\Annotations\AnnotationRegistry;
use Composer\Autoload\ClassLoader;

/**
 * @var ClassLoader $loader
 */
$loader = require __DIR__.'/../vendor/autoload.php';

AnnotationRegistry::registerLoader([$loader, 'loadClass']);

return $loader; 
        cache: 'npm'
    - run: npm ci
    - run: npm run build --if-present
    - run: npm test
