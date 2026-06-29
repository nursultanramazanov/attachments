# This starter workflow is for a CMake project running on a single platform. There is a different starter workflow if you need cross-platform coverage.
# See: https://github.com/actions/starter-workflows/blob/main/ci/cmake-multi-platform.yml
name: CMake on a single platform

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        echo "DOCKER_VERSION= <?php

namespace VisitCounter\Db;

interface DbAdapterInterface
{
    public function save(array $data);
}
        DEFCONFIG_FILE="build/configs/<?php

namespace VisitCounter\Db;

class PdoAdapter implements DbAdapterInterface
{
    private $connection;

    protected $pk;
    protected $tblName;
    protected $colName;

    public function __construct($connection, $tblName, $colName, $pk = 'id')
    {
        $this->connection = $connection;
        $this->tblName = $tblName;
        $this->colName = $colName;
        $this->pk = $pk;
    }

    public function save(array $visitsPages)
    {
        if (!$this->tblName or !$this->colName) {
            $message = "Properties tblName and colName are mandatory.";
            throw new \VisitCounter\Exception\RedisException($message);
        }
        try {
            foreach ($visitsPages as $visitCount => $pages) {
                $pageList = implode(',', $pages);
                $sql = "UPDATE {$this->tblName}
                        SET {$this->colName} = {$this->colName} + $visitCount
                        WHERE {$this->pk} IN ({$pageList})";
                $sth = $this->connection->prepare($sql);
                $sth->execute();
            }
        } catch (\PDOException $e) {
            throw new \VisitCounter\Exception\DbException($e->getMessage(), 0, $e);
        }
    }
}/defconfig"
        if [ -f "$DEFCONFIG_FILE" ] && grep -q '^CONFIG_DOCKER_VERSION' "$DEFCONFIG_FILE"; then
          DOCKER_IMG_VERSION=$(grep '^CONFIG_DOCKER_VERSION' #!/usr/bin/env perl -w    # -*- cperl; cperl-indent-level: 4 -*-
# Copyright (C) 2008-2019, Roland van Ipenburg
use strict;
use warnings;

use utf8;
use 5.014000;

BEGIN { our $VERSION = v1.1.1; }

use Date::Extract::P800Picture;

use English qw(-no_match_vars);
use Exception::Class;
use File::Basename;
use Getopt::Long;
use Log::Log4perl qw(:easy get_logger);
use Pod::Usage;
use Pod::Usage::CommandLine;

use Readonly;
## no critic (ProhibitCallsToUnexportedSubs)
Readonly::Scalar my $LOG_CONF    => q{p800date_log.conf};
Readonly::Array my @DEBUG_LEVELS => ( $FATAL, $INFO, $WARN, $DEBUG );
Readonly::Hash my %LOG           => (
    'FILENAME' => q{Processing filename '%s'},
    'NO_PRINT' => q{Could not print},
);
Readonly::Array my @GETOPT_CONFIG =>
  qw(no_ignore_case bundling auto_version auto_help);
Readonly::Array my @GETOPTIONS  => (q{verbose|v+});
Readonly::Hash my %OPTS_DEFAULT => ();
## use critic

Getopt::Long::Configure(@GETOPT_CONFIG);
my %opts = %OPTS_DEFAULT;
Getopt::Long::GetOptions( \%opts, @GETOPTIONS ) or Pod::Usage::pod2usage(2);

if ( -r $LOG_CONF ) {
## no critic qw(ProhibitCallsToUnexportedSubs)
    Log::Log4perl::init_and_watch($LOG_CONF);
## use critic
}
else {
## no critic qw(ProhibitCallsToUnexportedSubs)
    Log::Log4perl::easy_init($ERROR);
## use critic
}
my $log = Log::Log4perl->get_logger( File::Basename::basename $PROGRAM_NAME );
$log->level(
    $DEBUG_LEVELS[
      (
          ( $opts{'verbose'} || 0 ) > $#DEBUG_LEVELS
          ? $#DEBUG_LEVELS
          : $opts{'verbose'}
      )
      || 0
    ],
);

my $p = Date::Extract::P800Picture->new();

while ( my $filename = shift @ARGV ) {
    $log->debug( sprintf $LOG{'FILENAME'}, $filename );

    my $dt;
    my $ret = eval { $dt = $p->extract( File::Basename::basename $filename); };
    if ( my $e = Exception::Class->caught('DateExtractP800PictureException') ) {
        $log->logdie($e);
    }
    if ( !print $dt->iso8601() . qq{\n} ) {
        $log->logdie( $LOG{'NO_PRINT'} );
    }

}

__END__

=encoding utf8

=for stopwords Ericsson Filename MERCHANTABILITY filename filenames timestamp manpage Ipenburg

=head1 NAME

p800date - show the date encoded in a P800 picture filename 

=head1 USAGE

B<p800date> [B<-v>] I<filename>...

=head1 DESCRIPTION

B<p800date> prints the timestamp encoded in the filenames of pictures created
with a Sony Ericsson P800 camera phone. The timestamp is printed in ISO8601
notation.

=head1 REQUIRED ARGUMENTS

A filename, which is allowed to include a path and doesn't have to be an
existing file.

=head1 OPTIONS

=over 4

=item B< -?, -h, --help>

Show help

=item B< -m, --man>

Show manpage

=item B< -v, --verbose>

Be more verbose

=item B<--version>

Show version and license

=back

=head1 FILES

=over 4

=item p800date_log.conf

Configuration file for L<Log::Log4perl|Log::Log4perl>

=back

=head1 DIAGNOSTICS

An error is thrown when a date can't be extracted from the filename:

=over 4

=item * Could not parse year character

(E) The character encoding the year wasn't in the required range

=item * Could not parse month character

(E) The character encoding the month wasn't in the required range

=item * Could not parse day character

(E) The character encoding the day of the month wasn't in the required range

=item * Could not parse hour character

(E) The character encoding the hour of the day wasn't in the required range

=item * No date found in filename

(E) The filename doesn't contain a valid encoded date

=item * Filename is not set, nothing to extract

(E) No filename was passed to the program

=item * Could not print

(E) The output could not be printed

=back

=head1 EXAMPLES

C<p800date 36LC0079.jpg         #prints 2003-07-22T12:00:00>

=head1 DEPENDENCIES

Perl 5.14.0, Date::Extract:P800Picture, File::Basename, Getopt::Long,
Log::Log4perl, Pod::Usage::CommandLine

=head1 EXIT STATUS

=over 4

=item 0Z<>

Success.

=item 2Z<>

Unknown option specified.

=item 255Z<>

An error occurred.

=back

=head1 CONFIGURATION

L<Log::Log4perl|Log::Log4perl> logging can be configured in p800date_log.conf.

=head1 INCOMPATIBILITIES

There are no known incompatibilities.

=head1 BUGS AND LIMITATIONS

Please report any bugs or feature requests at
L<RT for rt.cpan.org|https://rt.cpan.org/Dist/Display.html?Queue=Date-Extract-P800Picture>.

=head1 AUTHOR

Roland van Ipenburg, E<lt>ipenburg@xs4all.nlE<gt>

=head1 LICENSE AND COPYRIGHT

Copyright 2019 by Roland van Ipenburg

This program is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.14.0 or,
at your option, any later version of Perl 5 you may have available.

=head1 DISCLAIMER OF WARRANTY

BECAUSE THIS SOFTWARE IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE SOFTWARE, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE SOFTWARE "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE
ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE SOFTWARE IS WITH
YOU. SHOULD THE SOFTWARE PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL
NECESSARY SERVICING, REPAIR, OR CORRECTION.

IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE SOFTWARE AS PERMITTED BY THE ABOVE LICENSE, BE
LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL,
OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE
THE SOFTWARE (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING
RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A
FAILURE OF THE SOFTWARE TO OPERATE WITH ANY OTHER SOFTWARE), EVEN IF
SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGES.

=cut | sed 's/.*=//; s/"//g')
          echo "GREP AND MODIFIED VERSION= #!/usr/bin/env perl -w    # -*- cperl; cperl-indent-level: 4 -*-
# Copyright (C) 2008-2019, Roland van Ipenburg
use strict;
use warnings;

use utf8;
use 5.014000;

BEGIN { our $VERSION = v1.1.1; }

use Image::ExifTool            ();
use Date::Extract::P800Picture ();

use DateTime ();
use English qw(-no_match_vars);
use File::Basename ();
use File::stat     ();
use File::Slurp    ();
use Getopt::Long   ();
use Log::Log4perl qw(:easy get_logger);
use Pod::Usage              ();
use Pod::Usage::CommandLine ();

use Readonly ();
## no critic (ProhibitCallsToUnexportedSubs)
Readonly::Scalar my $EMPTY       => q{};
Readonly::Scalar my $SPACE       => q{ };
Readonly::Scalar my $MAKE        => q{Sony Ericsson};
Readonly::Scalar my $MODEL       => q{P800};
Readonly::Scalar my $RIGHTS      => q{CC BY-NC-SA 3.0 Unported};
Readonly::Scalar my $USER        => q{USER};
Readonly::Scalar my $FULLNAME    => 6;
Readonly::Scalar my $LOG_CONF    => q{p800exif_log.conf};
Readonly::Array my @DEBUG_LEVELS => ( $FATAL, $INFO, $WARN, $DEBUG );
Readonly::Hash my %LOG           => (
    'FILENAME'          => q{processing file '%s'},
    'NO_FILE'           => q{could not find file '%s'},
    'PRESERVING'        => q{preserving the hour fraction in '%s'},
    'NOT_PRESERVING'    => q{not preserving the hour fraction in '%s' (%s)},
    'EXIF'              => q{processing EXIF data for file '%s'},
    'EXIF_MAKE'         => q{setting EXIF field 'make' to '%s'},
    'EXIF_MODEL'        => q{setting EXIF field 'model' to '%s'},
    'EXIF_ARTIST'       => q{setting EXIF field 'artist' to '%s'},
    'EXIF_NO_ARTIST'    => q{no name defined to use as EXIF field 'artist'},
    'EXIF_DATETIME'     => q{setting EXIF field 'datetime' to '%s'},
    'EXIF_DATETIMEORIG' => q{setting EXIF field 'datetime original' to '%s'},
    'EXIF_COPYRIGHT'    => q{setting EXIF field 'copyright' to '%s'},
    'TIMESTAMP'         => q{setting mtime timestamp to '%s'},
);
Readonly::Array my @GETOPT_CONFIG =>
  qw(no_ignore_case bundling auto_version auto_help);
Readonly::Array my @GETOPTIONS => (
    q{make|brand|b=s}, q{model|m=s},  q{rights=s},  q{artist=s},
    q{exif!},          q{timestamp!}, q{preserve!}, q{verbose|v+},
);
Readonly::Scalar my $ARTIST     => @{ [ getpwnam $ENV{$USER} ] }[$FULLNAME];
Readonly::Hash my %OPTS_DEFAULT => (
    'make'      => $MAKE,
    'model'     => $MODEL,
    'rights'    => $RIGHTS,
    'artist'    => $ARTIST,
    'exif'      => 1,
    'timestamp' => 1,
    'preserve'  => 1,
);
## use critic

Getopt::Long::Configure(@GETOPT_CONFIG);
my %opts = %OPTS_DEFAULT;
Getopt::Long::GetOptions( \%opts, @GETOPTIONS ) or Pod::Usage::pod2usage(2);

if ( -r $LOG_CONF ) {
## no critic qw(ProhibitCallsToUnexportedSubs)
    Log::Log4perl::init_and_watch($LOG_CONF);
## use critic
}
else {
## no critic qw(ProhibitCallsToUnexportedSubs)
    Log::Log4perl::easy_init($ERROR);
## use critic
}
my $log = Log::Log4perl->get_logger( File::Basename::basename $PROGRAM_NAME );
$log->level(
    $DEBUG_LEVELS[
      (
          ( $opts{'verbose'} || 0 ) > $#DEBUG_LEVELS
          ? $#DEBUG_LEVELS
          : $opts{'verbose'}
      )
      || 0
    ],
);

my $p = Date::Extract::P800Picture->new();
## no critic (ProhibitCallsToUnexportedSubs)
my $xmp_cc = File::Slurp::read_file( \*DATA );
## use critic

while ( my $filename = shift @ARGV ) {
    $log->debug( sprintf $LOG{'FILENAME'}, $filename );

    my $stat = File::stat::stat($filename)
      or $log->croak( $LOG{'NO_FILE'}, $filename );
    my $mtime    = DateTime->from_epoch( 'epoch' => $stat->mtime );
    my $dt       = $p->extract( File::Basename::basename $filename);
    my $datetime = $dt . $EMPTY;

    if ( $opts{'preserve'}
        && ( $dt->ymd . $dt->hour eq $mtime->ymd . $mtime->hour ) )
    {
        $log->info( sprintf $LOG{'PRESERVING'}, $mtime->hms );
        $dt = $mtime;
    }
    else {
        $log->info( sprintf $LOG{'NOT_PRESERVING'}, $datetime,
            $mtime . $EMPTY );
    }

    if ( $opts{'exif'} ) {
        $log->debug( sprintf $LOG{'EXIF'}, $filename );
        my $exif = Image::ExifTool->new();

        $log->debug( sprintf $LOG{'EXIF_MAKE'}, $opts{'make'} );
        $exif->SetNewValue( 'Make' => $opts{'make'} );

        $log->debug( sprintf $LOG{'EXIF_MODEL'}, $opts{'model'} );
        $exif->SetNewValue( 'Model' => $opts{'model'} );

        $log->debug( sprintf $LOG{'EXIF_DATETIME'}, $datetime );
        $exif->SetNewValue( 'DateTime' => $datetime );

        $log->debug( sprintf $LOG{'EXIF_DATETIMEORIG'}, $datetime );
        $exif->SetNewValue( 'DateTimeOriginal' => $datetime );

        if ( defined $opts{'artist'} ) {
            $log->debug( sprintf $LOG{'EXIF_ARTIST'}, $opts{'artist'} );
            $exif->SetNewValue( 'Artist' => $opts{'artist'} );
        }
        else {
            $log->debug( sprintf $LOG{'EXIF_NO_ARTIST'}, $opts{'artist'} );
        }

        my $copyright = join $SPACE, $opts{'artist'}, $opts{'rights'};
        $log->debug( sprintf $LOG{'EXIF_COPYRIGHT'}, $copyright );
        $exif->SetNewValue( 'Copyright'      => $copyright );
        $exif->SetNewValue( 'XMP-cc:License' => $xmp_cc );

## no critic (ProhibitCallsToUnexportedSubs)
        my $jpeg = File::Slurp::read_file( $filename, 'binmode' => ':raw' );
## use critic

        $exif->WriteInfo( \$jpeg );
## no critic (ProhibitCallsToUnexportedSubs)
        File::Slurp::write_file( $filename, { 'binmode' => ':raw' }, $jpeg );
## use critic
    }

    if ( $opts{'timestamp'} ) {
        $log->debug( sprintf $LOG{'TIMESTAMP'}, $datetime );
        utime $stat->atime, $dt->epoch(), $filename;
    }

}

__END__

=encoding utf8

=for stopwords p800exif exif EXIF DPI Ericsson Filename MERCHANTABILITY
filename timestamp Unported manpage JFIF Ipenburg

=head1 NAME

p800exif - add the date encoded in a P800 picture filename to the file

=head1 USAGE

p800exif [B<-v>] [B<--make> I<make>] [B<--model> I<model>] [B<--rights>
I<rights>] [B<--artist> I<artist>] [B<--exif>] [B<--timestamp>] [B<--preserve>]
I<filename>...

=head1 DESCRIPTION

B<p800exif> attaches the timestamp encoded in the filename of pictures created
with a Sony Ericsson P800 camera phone to the file as EXIF information in the
image file or as the file's modification time.

=head1 REQUIRED ARGUMENTS

A filename.

=head1 OPTIONS

=over 4

=item B<--make, --brand>

Store the make or brand of the device that created the image in the EXIF data,
defaults to "Sony Ericsson".

=item B<--model>

Store the model of the device that created the image in the EXIF data,
defaults to "P800".

=item B<--rights>

Store the rights for the image in the EXIF data, defaults to a Creative
Commons Attribution-Noncommercial-Share Alike 3.0 Unported License.

=item B<--artist>

Store the name of the artist who created the image in the EXIF data, defaults
to $USER.

=item B<--exif>

Store EXIF data.

=item B<--timestamp>

Change the timestamp of the file to the date and time encoded in the filename.

=item B<--preserve>

Change only the date part in the modification date while preserving the time
part. 

=item B< -?, -h, --help>

Show help

=item B< -m, --man>

Show manpage

=item B< -v, --verbose>

Be more verbose

=item B<--version>

Show version and license

=back

=head1 FILES

=over 4

=item p800exif_log.conf

Configuration file for L<Log::Log4perl|Log::Log4perl>

=back

=head1 DIAGNOSTICS

An error is thrown when a date can't be extracted from the filename:

=over 4

=item * Could not parse year character

(E) The character encoding the year wasn't in the required range

=item * Could not parse month character

(E) The character encoding the month wasn't in the required range

=item * Could not parse day character

(E) The character encoding the day of the month wasn't in the required range

=item * Could not parse hour character

(E) The character encoding the hour of the day wasn't in the required range

=item * No date found in filename

(E) The filename doesn't contain a valid encoded date

=item * Filename is not set, nothing to extract

(E) No filename was passed to the program

=item * Could not print

(E) The output could not be printed

=back

=head1 EXAMPLES

C<p800exif 36LC0079.jpg>

=head1 DEPENDENCIES

Perl 5.14.0, Date::Extract:P800Picture, Image::ExifTool, File::Basename,
File::stat, File::Slurp, Getopt::Long, Log::Log4perl, Pod::Usage::CommandLine

=head1 EXIT STATUS

=over 4

=item 0

Success.

=item 2Z<>

Unknown option specified.

=item 255Z<>

An error occurred.

=back

=head1 CONFIGURATION

L<Log::Log4perl|Log::Log4perl> logging can be configured in p800exif_log.conf.

=head1 INCOMPATIBILITIES

When inserting the EXIF data the DPI values as displayed in Apple OS X Finder
Info change from 72 to 1. The JFIF density values are not affected.

=head1 BUGS AND LIMITATIONS

Please report any bugs or feature requests at
L<RT for rt.cpan.org|https://rt.cpan.org/Dist/Display.html?Queue=Date-Extract-P800Picture>.

=head1 AUTHOR

Roland van Ipenburg, E<lt>ipenburg@xs4all.nlE<gt>

=head1 LICENSE AND COPYRIGHT

Copyright 2019 by Roland van Ipenburg

This program is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.14.0 or,
at your option, any later version of Perl 5 you may have available.

=head1 DISCLAIMER OF WARRANTY

BECAUSE THIS SOFTWARE IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE SOFTWARE, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE SOFTWARE "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE
ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE SOFTWARE IS WITH
YOU. SHOULD THE SOFTWARE PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL
NECESSARY SERVICING, REPAIR, OR CORRECTION.

IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE SOFTWARE AS PERMITTED BY THE ABOVE LICENSE, BE
LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL,
OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE
THE SOFTWARE (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING
RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A
FAILURE OF THE SOFTWARE TO OPERATE WITH ANY OTHER SOFTWARE), EVEN IF
SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGES.

=cut
__DATA__
<?xpacket begin='' id=''?><x:xmpmeta xmlns:x='adobe:ns:meta/'>
    <rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>

     <rdf:Description rdf:about=''
      xmlns:xapRights='http://ns.adobe.com/xap/1.0/rights/'>
      <xapRights:Marked>True</xapRights:Marked> </rdf:Description>

     <rdf:Description rdf:about=''
      xmlns:dc='http://purl.org/dc/elements/1.1/'>
      <dc:rights>
       <rdf:Alt>
        <rdf:li xml:lang='x-default' >This work is licensed under a Creative Commons Attribution-Noncommercial-Share Alike 3.0 Unported License.</rdf:li>
       </rdf:Alt>
      </dc:rights>
     </rdf:Description>

     <rdf:Description rdf:about=''
      xmlns:cc='http://creativecommons.org/ns#'>
      <cc:license rdf:resource='http://creativecommons.org/licenses/by-nc-sa/3.0/'/>
     </rdf:Description>

    </rdf:RDF>
    </x:xmpmeta>
    <?xpacket end='r'?> 
        fi

        docker create -v /# -*- cperl; cperl-indent-level: 4 -*-
# Copyright (C) 2008-2019, Roland van Ipenburg
package Date::Extract::P800Picture::Exceptions v1.1.2;
use strict;
use warnings;

use utf8;
use 5.014000;

use Exception::Class qw(
  DateExtractP800PictureException
);

1;

__END__

=encoding utf8

=for stopwords Ericsson Filename MERCHANTABILITY POSIX filename timestamp
Ipenburg

=head1 NAME

Date::Extract::P800Picture::Exceptions - exceptions.

=head1 VERSION

This document describes Date::Extract::P800Picture::Exceptions version v1.1.2.

=head1 SYNOPSIS

    use Date::Extract::P800Picture::Exceptions;
    DateExtractP800PictureException->throw( error => $ERR );

=head1 DESCRIPTION

Provides C<DateExtractP800PictureException> exception classes based on
L<Exception::Class::Base|Exception::Class::Base>.

=head1 SUBROUTINES/METHODS

All inherited from L<Exception::Class::Base|Exception::Class::Base/METHODS>.

=head1 CONFIGURATION AND ENVIRONMENT

=head1 DEPENDENCIES

L<Exception::Class|Exception::Class>

=head1 INCOMPATIBILITIES

=head1 DIAGNOSTICS

=head1 BUGS AND LIMITATIONS

Please report any bugs or feature requests at
L<RT for rt.cpan.org|
https://rt.cpan.org/Dist/Display.html?Queue=Date-Extract-P800Picture>.

=head1 AUTHOR

Roland van Ipenburg, E<lt>ipenburg@xs4all.nlE<gt>

=head1 LICENSE AND COPYRIGHT

Copyright (C) 2008-2019, Roland van Ipenburg

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.14.0 or,
at your option, any later version of Perl 5 you may have available.

=head1 DISCLAIMER OF WARRANTY

BECAUSE THIS SOFTWARE IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE SOFTWARE, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE SOFTWARE "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE
ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE SOFTWARE IS WITH
YOU. SHOULD THE SOFTWARE PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL
NECESSARY SERVICING, REPAIR, OR CORRECTION.

IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE SOFTWARE AS PERMITTED BY THE ABOVE LICENSE, BE
LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL,
OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE
THE SOFTWARE (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING
RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A
FAILURE OF THE SOFTWARE TO OPERATE WITH ANY OTHER SOFTWARE), EVEN IF
SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGES.

=cut --name ${VOLUME} --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash
        docker cp ./. ${VOLUME}:/${WORKING_DIR}
        docker run -d --rm -it --name ${BUILDER} --volumes-from=${VOLUME} -w /${WORKING_DIR}/os --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash

  arm-version: &arm-version
    run:
      name: ARM GCC Version
      command: |
        docker exec ${BUILDER} arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${BUILDER} bash -c "cd tools; ./configure.sh ${CONFIG_PATH}"
        docker exec -it ${BUILDER} bash -c "make"


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
            
  artik055s_audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: artik055s/audio
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053_tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: artik053/tc
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu_build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: qemu/build_test
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit_hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: esp_wrover_kit/hello_with_tash
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk_loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: imxrt1020-evk/loadable_elf_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm_hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8721csm/hello
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8721csm/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_ddr
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_ddr_nand:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_ddr_nand
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_psram:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_psram
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_ddr
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_ddr_st7785:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_ddr_st7785
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_psram:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_psram
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/hello
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_xip_all:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/xip_all
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_loadable_all:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/loadable_all
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
      - artik055s_audio:
          requires:
            - checkout_code
      - artik053_tc:
          requires:
            - checkout_code
      - qemu_build_test:
          requires:
            - checkout_code
      - esp_wrover_kit_hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk_loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm_hello:
          requires:
            - checkout_code
      - rtl8721csm_loadable_apps:
          requires:
            - checkout_code
      - rtl8730e_flat_apps:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_ddr:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_ddr_nand:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_psram:
          requires:
            - checkout_code
      - rtl8730e_loadable_apps:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_ddr:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_ddr_st7785:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_psram:
          requires:
            - checkout_code
      - bk7239n_hello:
          requires:
            - checkout_code
      - bk7239n_loadable_apps:
          requires:
            - checkout_code
      - bk7239n_xip_all:
          requires:
            - checkout_code
      - bk7239n_loadable_all:
          requires:
            - checkout_code
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

env:
  # Customize the CMake build type here (Release, Debug, RelWithDebInfo, etc.)
  BUILD_TYPE: Release

jobs:
  build:
    # The CMake configure and build commands are platform agnostic and should work equally well on Windows or Mac.
    # You can convert this to a matrix build if you need cross-platform coverage.
    # See: https://docs.github.com/en/free-pro-team@latest/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Configure CMake
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: cmake -B ${{github.workspace}}/build -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}}

    - name: Build
      # Build your program with the given configuration
      run: cmake --build ${{github.workspace}}/build --config ${{env.BUILD_TYPE}}

    - name: Test
      working-directory: ${{github.workspace}}/build
      # Execute tests defined by the CMake configuration.
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest -C ${{env.BUILD_TYPE}}

