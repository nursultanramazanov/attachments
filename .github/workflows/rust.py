name: Rust

on: # Doxyfile 1.8.17

#---------------------------------------------------------------------------
# Project related configuration options
#---------------------------------------------------------------------------
DOXYFILE_ENCODING      = UTF-8
PROJECT_NAME           = QGroundControl
PROJECT_NUMBER         =
PROJECT_BRIEF          =
PROJECT_LOGO           = ./qgroundcontrol.png
OUTPUT_DIRECTORY       = .
CREATE_SUBDIRS         = YES
ALLOW_UNICODE_NAMES    = NO
OUTPUT_LANGUAGE        = English
OUTPUT_TEXT_DIRECTION  = None
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       = "The $name class" \
                         "The $name widget" \
                         "The $name file" \
                         is \
                         provides \
                         specifies \
                         contains \
                         represents \
                         a \
                         an \
                         the
ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = YES
STRIP_FROM_PATH        =  \
                         ..
STRIP_FROM_INC_PATH    =
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = NO
JAVADOC_BANNER         = NO
QT_AUTOBRIEF           = NO
MULTILINE_CPP_IS_BRIEF = NO
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
TAB_SIZE               = 4
ALIASES                = deprecated=@attention
TCL_SUBST              =
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
OPTIMIZE_FOR_FORTRAN   = NO
OPTIMIZE_OUTPUT_VHDL   = NO
OPTIMIZE_OUTPUT_SLICE  = NO
EXTENSION_MAPPING      = qml=C++
MARKDOWN_SUPPORT       = YES
TOC_INCLUDE_HEADINGS   = 5
AUTOLINK_SUPPORT       = YES
BUILTIN_STL_SUPPORT    = YES
CPP_CLI_SUPPORT        = NO
SIP_SUPPORT            = NO
IDL_PROPERTY_SUPPORT   = YES
DISTRIBUTE_GROUP_DOC   = NO
GROUP_NESTED_COMPOUNDS = NO
SUBGROUPING            = YES
INLINE_GROUPED_CLASSES = NO
INLINE_SIMPLE_STRUCTS  = NO
TYPEDEF_HIDES_STRUCT   = NO
LOOKUP_CACHE_SIZE      = 0
#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
EXTRACT_PRIV_VIRTUAL   = NO
EXTRACT_PACKAGE        = NO
EXTRACT_STATIC         = NO
EXTRACT_LOCAL_CLASSES  = YES
EXTRACT_LOCAL_METHODS  = NO
EXTRACT_ANON_NSPACES   = NO
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = NO
HIDE_SCOPE_NAMES       = NO
HIDE_COMPOUND_REFERENCE= NO
SHOW_INCLUDE_FILES     = YES
SHOW_GROUPED_MEMB_INC  = NO
FORCE_LOCAL_INCLUDES   = NO
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_BRIEF_DOCS        = NO
SORT_MEMBERS_CTORS_1ST = NO
SORT_GROUP_NAMES       = NO
SORT_BY_SCOPE_NAME     = NO
STRICT_PROTO_MATCHING  = NO
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       =
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
SHOW_FILES             = YES
SHOW_NAMESPACES        = NO
FILE_VERSION_FILTER    =
LAYOUT_FILE            =
CITE_BIB_FILES         =
#---------------------------------------------------------------------------
# Configuration options related to warning and progress messages
#---------------------------------------------------------------------------
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_NO_PARAMDOC       = NO
WARN_AS_ERROR          = NO
WARN_FORMAT            = "$file:$line: $text"
WARN_LOGFILE           =
#---------------------------------------------------------------------------
# Configuration options related to the input files
#---------------------------------------------------------------------------
INPUT                  =  \
                         ../src \
                         .
INPUT_ENCODING         = UTF-8
FILE_PATTERNS          = *.c \
                         *.cc \
                         *.cxx \
                         *.cpp \
                         *.c++ \
                         *.java \
                         *.h \
                         *.hh \
                         *.hxx \
                         *.hpp \
                         *.h++ \
                         *.m \
                         *.markdown \
                         *.md \
                         *.mm \
                         *.dox \
                         *.doc \
                         *.txt \
                         *.py \
                         *.qml
RECURSIVE              = YES
EXCLUDE                = ../src/Airmap/QJsonWebToken \
                         ../src/PairingManager/NfcLibrary \
                         ../src/PairingManager/NfcTask \
                         ../src/PairingManager/TML \
                         ../src/GPS/Drivers \
                         ../src/GPS/RTCM \
                         ../src/Airmap/QJsonWebToken \
                         ../src/Airmap/README.md \
                         ../src/QtLocationPlugin/qtlocation/README.md \
                         ../src/VideoStreaming/README.md
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       = *Test.*
EXCLUDE_SYMBOLS        = "UnitTest" "_*"
EXAMPLE_PATH           =
EXAMPLE_PATTERNS       = *
EXAMPLE_RECURSIVE      = NO
IMAGE_PATH             =
INPUT_FILTER           =
FILTER_PATTERNS        = *.qml=doxyqml
FILTER_SOURCE_FILES    = NO
FILTER_SOURCE_PATTERNS =
USE_MDFILE_AS_MAINPAGE =
#---------------------------------------------------------------------------
# Configuration options related to source browsing
#---------------------------------------------------------------------------
SOURCE_BROWSER         = NO
INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES
REFERENCED_BY_RELATION = NO
REFERENCES_RELATION    = NO
REFERENCES_LINK_SOURCE = YES
SOURCE_TOOLTIPS        = YES
USE_HTAGS              = NO
VERBATIM_HEADERS       = YES
#CLANG_ASSISTED_PARSING = NO
#CLANG_OPTIONS          =
#CLANG_DATABASE_PATH    =
#---------------------------------------------------------------------------
# Configuration options related to the alphabetical class index
#---------------------------------------------------------------------------
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          =
#---------------------------------------------------------------------------
# Configuration options related to the HTML output
#---------------------------------------------------------------------------
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            = ./header.html
HTML_FOOTER            =
HTML_STYLESHEET        =
HTML_EXTRA_STYLESHEET  =
HTML_EXTRA_FILES       =
HTML_COLORSTYLE_HUE    = 269
HTML_COLORSTYLE_SAT    = 60
HTML_COLORSTYLE_GAMMA  = 100
HTML_TIMESTAMP         = NO
HTML_DYNAMIC_MENUS     = YES
HTML_DYNAMIC_SECTIONS  = NO
HTML_INDEX_NUM_ENTRIES = 100
GENERATE_DOCSET        = NO
DOCSET_FEEDNAME        = "Doxygen generated docs"
DOCSET_BUNDLE_ID       = org.doxygen.Project
DOCSET_PUBLISHER_ID    = org.doxygen.Publisher
DOCSET_PUBLISHER_NAME  = Publisher
GENERATE_HTMLHELP      = NO
CHM_FILE               =
HHC_LOCATION           =
GENERATE_CHI           = NO
CHM_INDEX_ENCODING     =
BINARY_TOC             = NO
TOC_EXPAND             = NO
GENERATE_QHP           = NO
QCH_FILE               =
QHP_NAMESPACE          = org.doxygen.Project
QHP_VIRTUAL_FOLDER     = doc
QHP_CUST_FILTER_NAME   =
QHP_CUST_FILTER_ATTRS  =
QHP_SECT_FILTER_ATTRS  =
QHG_LOCATION           =
GENERATE_ECLIPSEHELP   = NO
ECLIPSE_DOC_ID         = org.doxygen.Project
DISABLE_INDEX          = NO
GENERATE_TREEVIEW      = NO
ENUM_VALUES_PER_LINE   = 4
TREEVIEW_WIDTH         = 250
EXT_LINKS_IN_WINDOW    = NO
FORMULA_FONTSIZE       = 10
FORMULA_TRANSPARENT    = YES
#FORMULA_MACROFILE      =
USE_MATHJAX            = NO
MATHJAX_FORMAT         = HTML-CSS
MATHJAX_RELPATH        = https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/
MATHJAX_EXTENSIONS     =
MATHJAX_CODEFILE       =
SEARCHENGINE           = YES
SERVER_BASED_SEARCH    = NO
EXTERNAL_SEARCH        = NO
SEARCHENGINE_URL       =
SEARCHDATA_FILE        = searchdata.xml
EXTERNAL_SEARCH_ID     =
EXTRA_SEARCH_MAPPINGS  =
#---------------------------------------------------------------------------
# Configuration options related to the LaTeX output
#---------------------------------------------------------------------------
GENERATE_LATEX         = NO
LATEX_OUTPUT           = latex
LATEX_CMD_NAME         =
MAKEINDEX_CMD_NAME     = makeindex
LATEX_MAKEINDEX_CMD    = makeindex
COMPACT_LATEX          = NO
PAPER_TYPE             = a4
EXTRA_PACKAGES         =
LATEX_HEADER           =
LATEX_FOOTER           =
LATEX_EXTRA_STYLESHEET =
LATEX_EXTRA_FILES      =
PDF_HYPERLINKS         = YES
USE_PDFLATEX           = YES
LATEX_BATCHMODE        = NO
LATEX_HIDE_INDICES     = NO
LATEX_SOURCE_CODE      = NO
LATEX_BIB_STYLE        = plain
LATEX_TIMESTAMP        = NO
LATEX_EMOJI_DIRECTORY  =
#---------------------------------------------------------------------------
# Configuration options related to the RTF output
#---------------------------------------------------------------------------
GENERATE_RTF           = NO
RTF_OUTPUT             = rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO
RTF_STYLESHEET_FILE    =
RTF_EXTENSIONS_FILE    =
RTF_SOURCE_CODE        = NO
#---------------------------------------------------------------------------
# Configuration options related to the man page output
#---------------------------------------------------------------------------
GENERATE_MAN           = NO
MAN_OUTPUT             = man
MAN_EXTENSION          = .3
MAN_SUBDIR             =
MAN_LINKS              = NO
#---------------------------------------------------------------------------
# Configuration options related to the XML output
#---------------------------------------------------------------------------
GENERATE_XML           = NO
XML_OUTPUT             = xml
XML_PROGRAMLISTING     = YES
XML_NS_MEMB_FILE_SCOPE = NO
#---------------------------------------------------------------------------
# Configuration options related to the DOCBOOK output
#---------------------------------------------------------------------------
GENERATE_DOCBOOK       = NO
DOCBOOK_OUTPUT         = docbook
DOCBOOK_PROGRAMLISTING = NO
#---------------------------------------------------------------------------
# Configuration options for the AutoGen Definitions output
#---------------------------------------------------------------------------
GENERATE_AUTOGEN_DEF   = NO
#---------------------------------------------------------------------------
# Configuration options related to the Perl module output
#---------------------------------------------------------------------------
GENERATE_PERLMOD       = NO
PERLMOD_LATEX          = NO
PERLMOD_PRETTY         = YES
PERLMOD_MAKEVAR_PREFIX =
#---------------------------------------------------------------------------
# Configuration options related to the preprocessor
#---------------------------------------------------------------------------
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = YES
EXPAND_ONLY_PREDEF     = NO
SEARCH_INCLUDES        = YES
INCLUDE_PATH           =
INCLUDE_FILE_PATTERNS  =
PREDEFINED             =
EXPAND_AS_DEFINED      =
SKIP_FUNCTION_MACROS   = YES
#---------------------------------------------------------------------------
# Configuration options related to external references
#---------------------------------------------------------------------------
TAGFILES               =
GENERATE_TAGFILE       =
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
EXTERNAL_PAGES         = YES
#---------------------------------------------------------------------------
# Configuration options related to the dot tool
#---------------------------------------------------------------------------
CLASS_DIAGRAMS         = YES
DIA_PATH               =
HIDE_UNDOC_RELATIONS   = YES
HAVE_DOT               = YES
DOT_NUM_THREADS        = 0
DOT_FONTNAME           = Helvetica
DOT_FONTSIZE           = 8
DOT_FONTPATH           =
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = YES
GROUP_GRAPHS           = YES
UML_LOOK               = YES
UML_LIMIT_NUM_FIELDS   = 50
TEMPLATE_RELATIONS     = YES
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = YES
CALLER_GRAPH           = YES
GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = YES
DOT_IMAGE_FORMAT       = svg
INTERACTIVE_SVG        = YES
DOT_PATH               = .
DOTFILE_DIRS           =
MSCFILE_DIRS           =
DIAFILE_DIRS           =
PLANTUML_JAR_PATH      =
PLANTUML_CFG_FILE      =
PLANTUML_INCLUDE_PATH  =
DOT_GRAPH_MAX_NODES    = 200
MAX_DOT_GRAPH_DEPTH    = 0
DOT_TRANSPARENT        = YES
DOT_MULTI_TARGETS      = YES
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
  push: Mac OS X
========
To install doxygen and dot: 
brew install doxygen dot

doxyqml:
https://github.com/agateau/doxyqml
$sudo pip3 install doxyqml

Then to generate the html, run the following code while you are in the qgroundcontrol/doc directory:
$doxygen Doxyfile

The html file index.html should be in doc/html. 

Debian
=====================
#To build documentation first install dependencies (doxygen, doxyqml and dot):
sudo apt install doxygen graphviz doxyqml

#Then build documentation in the doc/ directory
doxygen ./Doxyfile
    branches: [ "main" ]
  pull_request: #! /bin/bash

# This hack is a wrapper to GraphViz dot that removes any nodes that
# are contained in the following list.

LABELS_TO_FILTER="QObject Item"

ARGS=$@

for ARG in ${ARGS}
do
  if [ -e ${ARG} ]
  then
    FILENAME=$(basename "${ARG}")
    EXT="${FILENAME##*.}"

    if [ ${EXT} == "dot" ]
    then
      DOT_FILE=${ARG}

      for LABEL_TO_FILTER in ${LABELS_TO_FILTER}
      do
        NODE_NAME=$(grep "label=\"${LABEL_TO_FILTER}\"" ${DOT_FILE} | awk '{print $1}')

        if [[ ! -z "${NODE_NAME}" ]]
        then
          echo "${NODE_NAME} is labelled ${LABEL_TO_FILTER}, filtering..."
          sed -i -e "/${NODE_NAME}/d" ${DOT_FILE}
        fi
      done

      break
    fi
  fi
done

if [[ -x "/usr/local/bin/dot" ]]  
then
  /usr/local/bin/dot ${ARGS}
elif [[ -x "/usr/bin/dot" ]]
then 
  /usr/bin/dot ${ARGS}
else
  echo "Program dot not found: See file doc/dot"
  exit 1
fi
    branches: [ "main" ]

env: <!-- HTML header for doxygen 1.8.16-->
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "https://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/xhtml;charset=UTF-8"/>
<meta http-equiv="X-UA-Compatible" content="IE=9"/>
<meta name="generator" content="Doxygen $doxygenversion"/>
<meta name="viewport" content="width=device-width, initial-scale=1"/>
<!--BEGIN PROJECT_NAME--><title>$projectname: $title</title><!--END PROJECT_NAME-->
<!--BEGIN !PROJECT_NAME--><title>$title</title><!--END !PROJECT_NAME-->
<link href="$relpath^tabs.css" rel="stylesheet" type="text/css"/>
<script type="text/javascript" src="$relpath^jquery.js"></script>
<script type="text/javascript" src="$relpath^dynsections.js"></script>
$treeview
$search
$mathjax
<link href="$relpath^$stylesheet" rel="stylesheet" type="text/css" />
$extrastylesheet
</head>
<body>
<div id="top"><!-- do not remove this div, it is closed by doxygen! -->

<!--BEGIN TITLEAREA-->
<div id="titlearea">
<table cellspacing="0" cellpadding="0">
 <tbody>
 <tr style="height: 56px;">
  <!--BEGIN PROJECT_LOGO-->
  <td id="projectlogo"><img alt="Logo" src="$relpath^$projectlogo" style="width:64px;height:64px;"/></td>
  <!--END PROJECT_LOGO-->
  <!--BEGIN PROJECT_NAME-->
  <td id="projectalign" style="padding-left: 0.5em;">
   <div style="font: 200% Tahoma, Arial,sans-serif; margin: 0px; padding: 2px 0px;">$projectname
   <!--BEGIN PROJECT_NUMBER-->&#160;<span id="projectnumber">$projectnumber</span><!--END PROJECT_NUMBER-->
   </div>
   <!--BEGIN PROJECT_BRIEF--><div id="projectbrief">$projectbrief</div><!--END PROJECT_BRIEF-->
  </td>
  <!--END PROJECT_NAME-->
  <!--BEGIN !PROJECT_NAME-->
   <!--BEGIN PROJECT_BRIEF-->
    <td style="padding-left: 0.5em;">
    <div id="projectbrief">$projectbrief</div>
    </td>
   <!--END PROJECT_BRIEF-->
  <!--END !PROJECT_NAME-->
  <!--BEGIN DISABLE_INDEX-->
   <!--BEGIN SEARCHENGINE-->
   <td>$searchbox</td>
   <!--END SEARCHENGINE-->
  <!--END DISABLE_INDEX-->
 </tr>
 </tbody>
</table>
</div>
<!--END TITLEAREA-->
<!-- end header part -->
  CARGO_TERM_COLOR: # Link Management

The LinkManager creates, configures and maintains communication links. Links are created either through the user
interface or programmatically. The LinkConfiguration base classs defines the means to configure a given link
while the LinkInterface exposes the link itself.

Link specializations such as UDPLink, TCPLink, SerialLink, etc. are implemented in their own derived classes as well
as their equivalent configuration derivations such as UDPConfiguration, TCPConfiguration, SerialConfiguration, etc.

Links are primarily responsible to send and receive (MAVLink) data to and from a vehicle. When data arrives, the link will emit a
LinkInterface::bytesReceived signal and when data needs to be sent back to a vehicle, the code uses its
LinkInterface::writeBytesSafe method.

<div align="center">
<img src="../links.svg" style="width:80%; height=auto;">
</div>

jobs:
  build:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
    - name: Build
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose
