name: Rust

on: #!/bin/sh
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${ <?php

namespace VisitCounter\Db;

interface DbAdapterInterface
{
    public function save(array $data);
} }/usr/lib/x86_64-linux-gnu":"${ <?php

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
} }/Qt/libs":$LD_LIBRARY_PATH
export QML2_IMPORT_PATH="${ <?php

namespace VisitCounter\Exception;

class DbException extends \Exception
{
} }/Qt/qml"
export QT_PLUGIN_PATH="${ <?php

namespace VisitCounter\Exception;

class RedisException extends \Exception
{
} }/Qt/plugins"

# hack until icon issue with AppImage is resolved
mkdir -p ~/.icons && \cp -f ${ <?php

namespace VisitCounter\Redis;

interface RedisAdapterInterface
{
    public function setnx($keyName, $expire, $value = '');
    public function rpush($listName, $value);
    public function llen($listName);
    public function lrange($listName, $start = 0, $end = -1);
    public function ltrim($listName, $start, $end = -1);
    public function hincrby($hashName, $field, $count = 1);
    public function hmget($hashName, array $fields);
} }/qgroundcontrol.png ~/.icons

"${ <?php

namespace VisitCounter\Redis;

class RediskaAdapter implements RedisAdapterInterface
{
    private $client;

    public function __construct(\Rediska $client)
    {
        $this->client = $client;
    }

    public function setnx($keyName, $expire, $value = '')
    {
        $command = new \Rediska_Command_Set(
            $this->client,
            'Set',
            array($keyName, $value, false)
        );
        try {
            if ( !$command->execute() ) return false;
            $key = new \Rediska_Key($keyName);
            $key->expire($expire);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function rpush($listName, $value)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->append($value);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function llen($listName)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $length = $key->getLength();
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $length;
    }

    public function lrange($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $result = $key->getValues($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $result;
    }

    public function ltrim($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->truncate($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hincrby($hashName, $field, $count = 1)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $key->increment($field, $count);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hmget($hashName, array $fields)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $result = $key->get($fields);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return array_combine($fields, $result);
    }
} }/CustomQGC" "$@"  
  push: #!/usr/bin/env perl -w    # -*- cperl; cperl-indent-level: 4 -*-
# Copyright (C) 2008-2019
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
=head1 LICENSE AND COPYRIGHT
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
    branches: [ "main" ]
  pull_request: #!/usr/bin/env perl -w    # -*- cperl; cperl-indent-level: 4 -*-
  # Copyright (C) 2008-2019
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
=head1 LICENSE AND COPYRIGHT
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
    branches: [ "main" ]

env: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick 2.11

Item {
    id: root
    property real   rollAngle :     0
    property real   pitchAngle:     0
    property color  skyColor1:      Qt.hsla(0.6, 1.0, 0.25)
    property color  skyColor2:      Qt.hsla(0.6, 0.5, 0.55)
    property color  groundColor1:   Qt.hsla(0.25,  0.5, 0.45)
    property color  groundColor2:   Qt.hsla(0.25, 0.75, 0.25)

    clip:           true
    anchors.fill:   parent

    property real angularScale: pitchAngle * root.height / 45

    Item {
        id:     artificialHorizon
        width:  root.width  * 4
        height: root.height * 8
        anchors.centerIn: parent
        Rectangle {
            id:             sky
            anchors.fill:   parent
            smooth:         true
            antialiasing:   true
            gradient: Gradient {
                GradientStop { position: 0.25; color: root.skyColor1 }
                GradientStop { position: 0.5;  color: root.skyColor2 }
            }
        }
        Rectangle {
            id:             ground
            height:         sky.height / 2
            anchors {
                left:       sky.left;
                right:      sky.right;
                bottom:     sky.bottom
            }
            smooth:         true
            antialiasing:   true
            gradient: Gradient {
                GradientStop { position: 0.0;  color: root.groundColor1 }
                GradientStop { position: 0.25; color: root.groundColor2 }
            }
        }
        transform: [
            Translate {
                y:  angularScale
            },
            Rotation {
                origin.x: artificialHorizon.width  / 2
                origin.y: artificialHorizon.height / 2
                angle:    -rollAngle
            }]
    }
} 
  CARGO_TERM_COLOR: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick              2.11
import QtGraphicalEffects   1.0

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0
import QGroundControl.FlightMap     1.0

Item {
    id: root

    property bool showPitch:    true
    property var  vehicle:      null
    property real size
    property bool showHeading:  false

    property real _rollAngle:   vehicle ? vehicle.roll.rawValue  : 0
    property real _pitchAngle:  vehicle ? vehicle.pitch.rawValue : 0

    width:  size
    height: size

    Item {
        id:             instrument
        anchors.fill:   parent
        visible:        false

        //----------------------------------------------------
        //-- Artificial Horizon
        CustomArtificialHorizon {
            rollAngle:          _rollAngle
            pitchAngle:         _pitchAngle
            skyColor1:          "#0a2e50"
            skyColor2:          "#2f85d4"
            groundColor1:       "#897459"
            groundColor2:       "#4b3820"
            anchors.fill:       parent
        }
        //----------------------------------------------------
        //-- Instrument Dial
        Image {
            id:                 instrumentDial
            source:             "/custom/img/attitude_dial.svg"
            mipmap:             true
            fillMode:           Image.PreserveAspectFit
            anchors.fill:       parent
            sourceSize.height:  parent.height
            transform: Rotation {
                origin.x:       root.width  / 2
                origin.y:       root.height / 2
                angle:          -_rollAngle
            }
        }
        //----------------------------------------------------
        //-- Pointer
        Image {
            id:                 pointer
            height:             size * 0.0625
            width:              height
            source:             "/custom/img/attitude_pointer.svg"
            antialiasing:       true
            fillMode:           Image.PreserveAspectFit
            sourceSize.height:  height
            anchors.top:        parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }
        //----------------------------------------------------
        //-- Pitch
        QGCPitchIndicator {
            id:                 pitchWidget
            visible:            root.showPitch
            size:               root.size * 0.5
            anchors.verticalCenter: parent.verticalCenter
            pitchAngle:         _pitchAngle
            rollAngle:          _rollAngle
            color:              Qt.rgba(0,0,0,0)
        }
        //----------------------------------------------------
        //-- Cross Hair
        Image {
            id:                 crossHair
            anchors.centerIn:   parent
            source:             "/custom/img/attitude_crosshair.svg"
            mipmap:             true
            width:              size * 0.75
            sourceSize.width:   width
            fillMode:           Image.PreserveAspectFit
        }
    }

    Rectangle {
        id:             mask
        anchors.fill:   instrument
        radius:         width / 2
        color:          "black"
        visible:        false
    }

    OpacityMask {
        anchors.fill:   instrument
        source:         instrument
        maskSource:     mask
    }

    Rectangle {
        id:             borderRect
        anchors.fill:   parent
        radius:         width / 2
        color:          Qt.rgba(0,0,0,0)
        border.color:   "#000"
        border.width:   1
    }

    QGCLabel {
        anchors.bottomMargin:       Math.round(ScreenTools.defaultFontPixelHeight * 0.5)
        anchors.bottom:             parent.bottom
        anchors.horizontalCenter:   parent.horizontalCenter
        text:                       _headingString3
        color:                      "white"
        visible:                    showHeading
        font.pointSize:             ScreenTools.smallFontPointSize
        property string _headingString: vehicle ? vehicle.heading.rawValue.toFixed(0) : "OFF"
        property string _headingString2: _headingString.length  === 1 ? "0" + _headingString  : _headingString
        property string _headingString3: _headingString2.length === 2 ? "0" + _headingString2 : _headingString2
    }
} 

jobs: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                      2.11
import QtQuick.Controls             2.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QtGraphicalEffects           1.0

Button {
    id:                             _rootButton
    width:                          parent.height * 1.25
    height:                         parent.height
    flat:                           true
    contentItem: Item {
        id:                         _content
        anchors.fill:               _rootButton
        Row {
            id:                     _edge
            spacing:                ScreenTools.defaultFontPixelWidth * 0.25
            anchors.left:           parent.left
            anchors.leftMargin:     ScreenTools.defaultFontPixelWidth
            anchors.verticalCenter: parent.verticalCenter
            Repeater {
                model: [1,2,3]
                Rectangle {
                    height:         ScreenTools.defaultFontPixelHeight
                    width:          ScreenTools.defaultFontPixelWidth * 0.25
                    color:          qgcPal.text
                    opacity:        0.75
                }
            }
        }
        Image {
            id:                     _icon
            height:                 _rootButton.height * 0.75
            width:                  height
            smooth:                 true
            mipmap:                 true
            antialiasing:           true
            fillMode:               Image.PreserveAspectFit
            source:                 qgcPal.globalTheme === QGCPalette.Light ? "/res/QGCLogoBlack" : "/res/QGCLogoWhite"
            sourceSize.height:      height
            anchors.left:           _edge.right
            anchors.leftMargin:     ScreenTools.defaultFontPixelWidth
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    background: Item {
        anchors.fill: parent
    }
} 
  build: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick 2.3

import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0

Rectangle {
    id:     _root
    height: Math.round(ScreenTools.defaultFontPixelHeight * 2)
    width:  ScreenTools.defaultFontPixelWidth  * 10
    color:  qgcPal.button
    border.color: qgcPal.windowShade
    border.width: 0

    property bool checked: true

    signal  clicked

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    Rectangle {
        width:      parent.width  * 0.5
        height:     parent.height
        color:      checked ? qgcPal.button : qgcPal.buttonHighlight
        border.color: qgcPal.text
        border.width: 0
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        QGCLabel {
            text: qsTr("Off")
            anchors.centerIn: parent
            color:      qgcPal.text
        }
    }
    Rectangle {
        width:      parent.width  * 0.5
        height:     parent.height
        color:      checked ? qgcPal.buttonHighlight : qgcPal.button
        border.color: qgcPal.text
        border.width: 0
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        QGCLabel {
            text:       qsTr("On")
            color:      qgcPal.buttonHighlightText
            anchors.centerIn: parent
        }
    }
    MouseArea {
        anchors.fill:   parent
        onClicked: {
            checked = !checked
            _root.clicked()
        }
    }
}  

    runs-on: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                      2.11
import QtQuick.Controls             2.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

Button {
    id:                         control
    autoExclusive:              true
    checkable:                  true

    property string iconSource:     ""
    property real   iconRatio:      0.5
    property real   buttonRadius:   ScreenTools.defaultFontPixelWidth * 0.5

    background: Rectangle {
        width:                  control.width
        height:                 width
        anchors.centerIn:       parent
        color:                  (mouseArea.pressed || control.checked) ? qgcPal.buttonHighlight : (qgcPal.globalTheme === QGCPalette.Light ? Qt.rgba(1,1,1,0.5) : Qt.rgba(0,0,0,0.5))
        radius:                 control.buttonRadius
    }
    contentItem: Item {
        anchors.fill:           control
        QGCColoredImage {
            source:             iconSource
            color:              (mouseArea.pressed || control.checked) ? qgcPal.buttonHighlightText : qgcPal.buttonText
            width:              control.width * iconRatio
            height:             width
            anchors.centerIn:   parent
            sourceSize.height:  height
        }
    }
    MouseArea {
        id:                     mouseArea
        anchors.fill:           parent
        onClicked: {
            if(checkable)
                checked = true
            control.clicked()
        }
    }
}  

    steps: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                              2.11
import QtQuick.Controls                     1.4

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

Item {
    width:  size
    height: size

    property real size:     50
    property real percent:  0

    QGCPalette { id: qgcPal }

    function getIcon() {
        if (percent < 20)
            return "/custom/img/menu_signal_0.svg"
        if (percent < 40)
            return "/custom/img/menu_signal_25.svg"
        if (percent < 60)
            return "/custom/img/menu_signal_50.svg"
        if (percent < 90)
            return "/custom/img/menu_signal_75.svg"
        return "/custom/img/menu_signal_100.svg"
    }

    QGCColoredImage {
        source:             getIcon()
        fillMode:           Image.PreserveAspectFit
        anchors.fill:       parent
        sourceSize.height:  size
        color:              qgcPal.text
    }
} 
    - uses: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                      2.11
import QtQuick.Controls             2.4

import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

Button {
    id:                             button
    autoExclusive:                  true

    background: Rectangle {
        anchors.fill:               parent
        color:                      qgcPal.buttonHighlight
        visible:                    (mouseArea.pressed || button.checked)
    }

    contentItem: Row {
        spacing:                    ScreenTools.defaultFontPixelWidth
        anchors.left:               button.left
        anchors.leftMargin:         ScreenTools.defaultFontPixelWidth
        anchors.verticalCenter:     button.verticalCenter
        Item {
            height:                 ScreenTools.defaultFontPixelHeight * 3
            width:                  1
        }
        QGCColoredImage {
            id:                     _icon
            height:                 ScreenTools.defaultFontPixelHeight
            width:                  height
            sourceSize.height:      parent.height
            fillMode:               Image.PreserveAspectFit
            color:                  qgcPal.buttonText
            source:                 button.icon.source
            anchors.verticalCenter: parent.verticalCenter
        }
        Label {
            id:                     _label
            visible:                text !== ""
            text:                   button.text
            color:                  qgcPal.buttonText
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    // Process hover events
    MouseArea {
        id:                         mouseArea
        anchors.fill:               parent
        onClicked:                  button.clicked()
    }
}  
    - name: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                      2.11
import QtQuick.Controls             2.4

import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

Button {
    id:                             button
    height:                         _infoCol.height * 1.25
    autoExclusive:                  true

    property var vehicle:           null

    function getBatteryColor() {
        if(vehicle) {
            if(vehicle.battery.percentRemaining.value > 75) {
                return qgcPal.colorGreen
            }
            if(vehicle.battery.percentRemaining.value > 50) {
                return qgcPal.colorOrange
            }
            if(vehicle.battery.percentRemaining.value > 0.1) {
                return qgcPal.colorRed
            }
        }
        return qgcPal.colorGrey
    }

    function getBatteryPercentage() {
        if(vehicle) {
            return vehicle.battery.percentRemaining.value / 100.0
        }
        return 1
    }

    background: Rectangle {
        anchors.fill:               parent
        color:                      button.checked ? qgcPal.buttonHighlight : qgcPal.button
        radius:                     ScreenTools.defaultFontPixelWidth * 0.5
    }

    contentItem: Row {
        spacing:                    ScreenTools.defaultFontPixelWidth
        anchors.margins:            ScreenTools.defaultFontPixelWidth
        anchors.verticalCenter:     button.verticalCenter
//        QGCColoredImage {
//            id:                     _icon
//            height:                 ScreenTools.defaultFontPixelHeight * 1.5
//            width:                  height
//            sourceSize.height:      parent.height
//            fillMode:               Image.PreserveAspectFit
//            color:                  button.checked ? qgcPal.buttonHighlightText : qgcPal.buttonText
//            source:                 "/qmlimages/PaperPlane.svg"
//            anchors.verticalCenter: parent.verticalCenter
//        }
        Column {
            id:                     _infoCol
            spacing:                ScreenTools.defaultFontPixelHeight * 0.25
            QGCLabel {
                text:               qsTr("Vehicle ") + (vehicle ? vehicle.id : qsTr("None"))
                font.family:        ScreenTools.demiboldFontFamily
                color:              button.checked ? qgcPal.buttonHighlightText : qgcPal.buttonText
            }
            Row {
                spacing:            ScreenTools.defaultFontPixelWidth
                QGCLabel {
                    text:           vehicle ? vehicle.flightMode : qsTr("None")
                    color:          button.checked ? qgcPal.buttonHighlightText : qgcPal.buttonText
                }
                Rectangle {
                    height:         ScreenTools.defaultFontPixelHeight * 0.5
                    width:          ScreenTools.defaultFontPixelWidth  * 3
                    color:          Qt.rgba(0,0,0,0)
                    anchors.verticalCenter: parent.verticalCenter
                    Rectangle {
                        height:     parent.height
                        width:      parent.width * getBatteryPercentage()
                        color:      getBatteryColor()
                        anchors.right: parent.right
                    }
                }
            }
        }
    }

}   
      run: Module Custom.Widgets

CustomArtificialHorizon 1.0     CustomArtificialHorizon.qml
CustomAttitudeWidget    1.0     CustomAttitudeWidget.qml
CustomIconButton        1.0     CustomIconButton.qml
CustomOnOffSwitch       1.0     CustomOnOffSwitch.qml
CustomQuickButton       1.0     CustomQuickButton.qml
CustomSignalStrength    1.0     CustomSignalStrength.qml
CustomToolBarButton     1.0     CustomToolBarButton.qml
CustomVehicleButton     1.0     CustomVehicleButton.qml 
    - name: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick          2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts  1.11

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

import Custom.Widgets 1.0

Item {
    property var parentToolInsets                       // These insets tell you what screen real estate is available for positioning the controls in your overlay
    property var totalToolInsets:   _totalToolInsets    // The insets updated for the custom overlay additions
    property var mapControl

    readonly property string noGPS:         qsTr("NO GPS")
    readonly property real   indicatorValueWidth:   ScreenTools.defaultFontPixelWidth * 7

    property var    _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property real   _indicatorDiameter:     ScreenTools.defaultFontPixelWidth * 18
    property real   _indicatorsHeight:      ScreenTools.defaultFontPixelHeight
    property var    _sepColor:              qgcPal.globalTheme === QGCPalette.Light ? Qt.rgba(0,0,0,0.5) : Qt.rgba(1,1,1,0.5)
    property color  _indicatorsColor:       qgcPal.text
    property bool   _isVehicleGps:          _activeVehicle ? _activeVehicle.gps.count.rawValue > 1 && _activeVehicle.gps.hdop.rawValue < 1.4 : false
    property string _altitude:              _activeVehicle ? (isNaN(_activeVehicle.altitudeRelative.value) ? "0.0" : _activeVehicle.altitudeRelative.value.toFixed(1)) + ' ' + _activeVehicle.altitudeRelative.units : "0.0"
    property string _distanceStr:           isNaN(_distance) ? "0" : _distance.toFixed(0) + ' ' + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString
    property real   _heading:               _activeVehicle   ? _activeVehicle.heading.rawValue : 0
    property real   _distance:              _activeVehicle ? _activeVehicle.distanceToHome.rawValue : 0
    property string _messageTitle:          ""
    property string _messageText:           ""
    property real   _toolsMargin:           ScreenTools.defaultFontPixelWidth * 0.75

    function secondsToHHMMSS(timeS) {
        var sec_num = parseInt(timeS, 10);
        var hours   = Math.floor(sec_num / 3600);
        var minutes = Math.floor((sec_num - (hours * 3600)) / 60);
        var seconds = sec_num - (hours * 3600) - (minutes * 60);
        if (hours   < 10) {hours   = "0"+hours;}
        if (minutes < 10) {minutes = "0"+minutes;}
        if (seconds < 10) {seconds = "0"+seconds;}
        return hours+':'+minutes+':'+seconds;
    }

    QGCToolInsets {
        id:                     _totalToolInsets
        topEdgeCenterInset:     compassArrowIndicator.y + compassArrowIndicator.height
        bottomEdgeRightInset:   parent.height - vehicleIndicator.y
        bottomEdgeCenterInset:  bottomEdgeRightInset
    }

    //-------------------------------------------------------------------------
    //-- Heading Indicator
    Rectangle {
        id:                         compassBar
        height:                     ScreenTools.defaultFontPixelHeight * 1.5
        width:                      ScreenTools.defaultFontPixelWidth  * 50
        color:                      "#DEDEDE"
        radius:                     2
        clip:                       true
        anchors.top:                headingIndicator.bottom
        anchors.topMargin:          -headingIndicator.height / 2
        anchors.horizontalCenter:   parent.horizontalCenter
        Repeater {
            model: 720
            QGCLabel {
                function _normalize(degrees) {
                    var a = degrees % 360
                    if (a < 0) a += 360
                    return a
                }
                property int _startAngle: modelData + 180 + _heading
                property int _angle: _normalize(_startAngle)
                anchors.verticalCenter: parent.verticalCenter
                x:              visible ? ((modelData * (compassBar.width / 360)) - (width * 0.5)) : 0
                visible:        _angle % 45 == 0
                color:          "#75505565"
                font.pointSize: ScreenTools.smallFontPointSize
                text: {
                    switch(_angle) {
                    case 0:     return "N"
                    case 45:    return "NE"
                    case 90:    return "E"
                    case 135:   return "SE"
                    case 180:   return "S"
                    case 225:   return "SW"
                    case 270:   return "W"
                    case 315:   return "NW"
                    }
                    return ""
                }
            }
        }
    }
    Rectangle {
        id:                         headingIndicator
        height:                     ScreenTools.defaultFontPixelHeight
        width:                      ScreenTools.defaultFontPixelWidth * 4
        color:                      qgcPal.windowShadeDark
        anchors.top:                parent.top
        anchors.topMargin:          _toolsMargin
        anchors.horizontalCenter:   parent.horizontalCenter
        QGCLabel {
            text:                   _heading
            color:                  qgcPal.text
            font.pointSize:         ScreenTools.smallFontPointSize
            anchors.centerIn:       parent
        }
    }
    Image {
        id:                         compassArrowIndicator
        height:                     _indicatorsHeight
        width:                      height
        source:                     "/custom/img/compass_pointer.svg"
        fillMode:                   Image.PreserveAspectFit
        sourceSize.height:          height
        anchors.top:                compassBar.bottom
        anchors.topMargin:          -height / 2
        anchors.horizontalCenter:   parent.horizontalCenter
    }
    //-------------------------------------------------------------------------
    //-- Vehicle Indicator
    Rectangle {
        id:                     vehicleIndicator
        color:                  qgcPal.window
        width:                  vehicleStatusGrid.width  + (ScreenTools.defaultFontPixelWidth  * 3)
        height:                 vehicleStatusGrid.height + (ScreenTools.defaultFontPixelHeight * 1.5)
        radius:                 2
        //anchors.bottomMargin:   parentToolInsets.bottomEdgeRightInset
        anchors.bottom:         parent.bottom
        anchors.bottomMargin:   _toolsMargin
        anchors.right:          attitudeIndicator.left
        anchors.rightMargin:    -ScreenTools.defaultFontPixelWidth

        GridLayout {
            id:                     vehicleStatusGrid
            columnSpacing:          ScreenTools.defaultFontPixelWidth  * 1.5
            rowSpacing:             ScreenTools.defaultFontPixelHeight * 0.5
            columns:                7
            anchors.centerIn:       parent

            //-- Compass
            Item {
                Layout.rowSpan:         3
                Layout.column:          6
                Layout.minimumWidth:    parent.height * 1.25
                Layout.fillHeight:      true
                Layout.fillWidth:       true
                //-- Large circle
                Rectangle {
                    height:             parent.height
                    width:              height
                    radius:             height * 0.5
                    border.color:       qgcPal.text
                    border.width:       1
                    color:              Qt.rgba(0,0,0,0)
                    anchors.centerIn:   parent
                }
                //-- North Label
                Rectangle {
                    height:             ScreenTools.defaultFontPixelHeight * 0.75
                    width:              ScreenTools.defaultFontPixelWidth  * 2
                    radius:             ScreenTools.defaultFontPixelWidth  * 0.25
                    color:              qgcPal.windowShade
                    anchors.top:        parent.top
                    anchors.topMargin:  ScreenTools.defaultFontPixelHeight * -0.25
                    anchors.horizontalCenter: parent.horizontalCenter
                    QGCLabel {
                        text:               "N"
                        color:              qgcPal.text
                        font.pointSize:     ScreenTools.smallFontPointSize
                        anchors.centerIn:   parent
                    }
                }
                //-- Needle
                Image {
                    id:                 compassNeedle
                    anchors.centerIn:   parent
                    height:             parent.height * 0.75
                    width:              height
                    source:             "/custom/img/compass_needle.svg"
                    fillMode:           Image.PreserveAspectFit
                    sourceSize.height:  height
                    transform: [
                        Rotation {
                            origin.x:   compassNeedle.width  / 2
                            origin.y:   compassNeedle.height / 2
                            angle:      _heading
                        }]
                }
                //-- Heading
                Rectangle {
                    height:             ScreenTools.defaultFontPixelHeight * 0.75
                    width:              ScreenTools.defaultFontPixelWidth  * 3.5
                    radius:             ScreenTools.defaultFontPixelWidth  * 0.25
                    color:              qgcPal.windowShade
                    anchors.bottom:         parent.bottom
                    anchors.bottomMargin:   ScreenTools.defaultFontPixelHeight * -0.25
                    anchors.horizontalCenter: parent.horizontalCenter
                    QGCLabel {
                        text:               _heading
                        color:              qgcPal.text
                        font.pointSize:     ScreenTools.smallFontPointSize
                        anchors.centerIn:   parent
                    }
                }
            }
            //-- Second Row
            //-- Chronometer
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/chronometer.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text
            }
            QGCLabel {
                id:                     firstLabel
                text: {
                    if(_activeVehicle)
                        return secondsToHHMMSS(_activeVehicle.getFact("flightTime").value)
                    return "00:00:00"
                }
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    Text.AlignLeft
            }
            //-- Ground Speed
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/horizontal_speed.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text
            }
            QGCLabel {
                text:                   _activeVehicle ? _activeVehicle.groundSpeed.value.toFixed(1) + ' ' + _activeVehicle.groundSpeed.units : "0.0"
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
            //-- Vertical Speed
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/vertical_speed.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text

            }
            QGCLabel {
                text:                   _activeVehicle ? _activeVehicle.climbRate.value.toFixed(1) + ' ' + _activeVehicle.climbRate.units : "0.0"
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
            //-- Third Row
            //-- Odometer
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/odometer.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text

            }
            QGCLabel {
                text:                   _activeVehicle ? ('00000' + _activeVehicle.flightDistance.value.toFixed(0)).slice(-5) + ' ' + _activeVehicle.flightDistance.units : "00000"
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
            //-- Altitude
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/altitude.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text

            }
            QGCLabel {
                text:                   _altitude
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
            //-- Distance
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/distance.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text

            }
            QGCLabel {
                text:                   _distance ? _distanceStr : noGPS
                color:                  _distance ? _indicatorsColor : qgcPal.colorOrange
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
        }
    }
    //-------------------------------------------------------------------------
    //-- Attitude Indicator
    Rectangle {
        color:                  qgcPal.window
        width:                  attitudeIndicator.width * 0.5
        height:                 vehicleIndicator.height
        anchors.top:            vehicleIndicator.top
        anchors.left:           vehicleIndicator.right
    }
    Rectangle {
        id:                     attitudeIndicator
        anchors.bottom:         vehicleIndicator.bottom
        anchors.bottomMargin:   ScreenTools.defaultFontPixelWidth * -0.5
        anchors.right:          parent.right
        anchors.rightMargin:    _toolsMargin
        height:                 ScreenTools.defaultFontPixelHeight * 6
        width:                  height
        radius:                 height * 0.5
        color:                  qgcPal.windowShade
        CustomAttitudeWidget {
            size:               parent.height * 0.95
            vehicle:            _activeVehicle
            showHeading:        false
            anchors.centerIn:   parent
        }
    }
} 
      run: cargo test --verbose
