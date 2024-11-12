# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow lets you generate SLSA provenance file for your project.
# The generation satisfies level 3 for the provenance requirements - see https://slsa.dev/spec/v0.1/requirements
# The project is an initiative of the OpenSSF (openssf.org) and is developed at
# https://github.com/slsa-framework/slsa-github-generator.
# The provenance file can be verified using https://github.com/slsa-framework/slsa-verifier.
# For more information about SLSA and how it improves the supply-chain, visit slsa.dev.

name: SLSA generic generator
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
  workflow_dispatch: Mac OS X
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
  release: #! /bin/bash

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
    types: <!-- HTML header for doxygen 1.8.16-->
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

jobs: <?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:xl="http://www.w3.org/1999/xlink" version="1.1" xmlns="http://www.w3.org/2000/svg" viewBox="-2.81 667.69 720.2501 425" width="720.2501" height="425">
  <defs>
    <filter id="Shadow" filterUnits="userSpaceOnUse" x="-2.81" y="667.69">
      <feGaussianBlur in="SourceAlpha" result="blur" stdDeviation="2.616"/>
      <feOffset in="blur" result="offset" dx="1.44" dy="1.44"/>
      <feFlood flood-color="black" flood-opacity=".5" result="flood"/>
      <feComposite in="flood" in2="offset" operator="in" result="color"/>
      <feMerge>
        <feMergeNode in="color"/>
        <feMergeNode in="SourceGraphic"/>
      </feMerge>
    </filter>
    <linearGradient x1="0" x2="1" id="Gradient" gradientUnits="userSpaceOnUse">
      <stop offset="0" stop-color="#eee"/>
      <stop offset="1" stop-color="#afafaf"/>
    </linearGradient>
    <linearGradient id="Obj_Gradient" xl:href="#Gradient" gradientTransform="translate(358.50003 672.75) rotate(90) scale(90)"/>
    <font-face font-family="Courier" font-size="12" units-per-em="1000" underline-position="-178.22266" underline-thickness="57.61719" slope="0" x-height="456.54297" cap-height="586.91406" ascent="753.90625" descent="-246.09375" font-weight="400">
      <font-face-src>
        <font-face-name name="Courier"/>
      </font-face-src>
    </font-face>
    <font-face font-family="Helvetica Neue" font-size="12" panose-1="2 0 5 3 0 0 0 2 0 4" units-per-em="1000" underline-position="-100" underline-thickness="50" slope="0" x-height="517" cap-height="714" ascent="951.9958" descent="-212.99744" font-weight="400">
      <font-face-src>
        <font-face-name name="HelveticaNeue"/>
      </font-face-src>
    </font-face>
    <linearGradient x1="0" x2="1" id="Gradient_2" gradientUnits="userSpaceOnUse">
      <stop offset="0" stop-color="#ff9691"/>
      <stop offset=".45238096" stop-color="#eb7e79"/>
      <stop offset="1" stop-color="#d56560"/>
    </linearGradient>
    <linearGradient id="Obj_Gradient_2" xl:href="#Gradient_2" gradientTransform="translate(225.75007 731.25) rotate(-90) scale(27)"/>
    <linearGradient x1="0" x2="1" id="Gradient_3" gradientUnits="userSpaceOnUse">
      <stop offset="0" stop-color="#54ba6b"/>
      <stop offset=".45238096" stop-color="#6dd98a"/>
      <stop offset="1" stop-color="#84f8a8"/>
    </linearGradient>
    <linearGradient id="Obj_Gradient_3" xl:href="#Gradient_3" gradientTransform="translate(489.00007 678.375) rotate(90) scale(27)"/>
    <linearGradient id="Obj_Gradient_4" xl:href="#Gradient" gradientTransform="translate(358.50003 831.375) rotate(90) scale(90)"/>
    <linearGradient id="Obj_Gradient_5" xl:href="#Gradient_3" gradientTransform="translate(489.00007 730.125) rotate(90) scale(27)"/>
    <linearGradient id="Obj_Gradient_6" xl:href="#Gradient_2" gradientTransform="translate(221.99993 862.875) rotate(90) scale(27)"/>
    <linearGradient id="Obj_Gradient_7" xl:href="#Gradient" gradientTransform="translate(358.50003 994.75) rotate(90) scale(90)"/>
    <linearGradient id="Obj_Gradient_8" xl:href="#Gradient_2" gradientTransform="translate(221.99993 1026.25) rotate(90) scale(27)"/>
    <linearGradient id="Obj_Gradient_9" xl:href="#Gradient_3" gradientTransform="translate(489.00007 1026.25) rotate(90) scale(27)"/>
    <marker orient="auto" overflow="visible" markerUnits="strokeWidth" id="Diamond_Marker" stroke-linejoin="miter" stroke-miterlimit="10" viewBox="-9 -4 10 8" markerWidth="10" markerHeight="8" color="black">
      <g>
        <path d="M -8 0 L -4 3 L 0 0 L -4 -3 Z" fill="none" stroke="currentColor" stroke-width="1"/>
      </g>
    </marker>
  </defs>
  <metadata> Produced by OmniGraffle 7.12.1 
    <dc:date>2020-02-02 20:02:31 +0000</dc:date>
  </metadata>
  <g id="Canvas_1" fill-opacity="1" fill="none" stroke="none" stroke-opacity="1" stroke-dasharray="none">
    <title>Canvas 1</title>
    <g id="Canvas_1: Layer 1">
      <title>Layer 1</title>
      <g id="Graphic_20" filter="url(#Shadow)">
        <path d="M 277.50003 672.75 L 439.50003 672.75 C 444.4706 672.75 448.50003 676.7794 448.50003 681.75 L 448.50003 753.75 C 448.50003 758.7206 444.4706 762.75 439.50003 762.75 L 277.50003 762.75 C 272.52947 762.75 268.50003 758.7206 268.50003 753.75 L 268.50003 681.75 C 268.50003 676.7794 272.52947 672.75 277.50003 672.75 Z" fill="url(#Obj_Gradient)"/>
        <path d="M 277.50003 672.75 L 439.50003 672.75 C 444.4706 672.75 448.50003 676.7794 448.50003 681.75 L 448.50003 753.75 C 448.50003 758.7206 444.4706 762.75 439.50003 762.75 L 277.50003 762.75 C 272.52947 762.75 268.50003 758.7206 268.50003 753.75 L 268.50003 681.75 C 268.50003 676.7794 272.52947 672.75 277.50003 672.75 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(273.50003 710.75)" fill="black">
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="30.99121" y="11">MAVLinkProtocol</tspan>
        </text>
      </g>
      <g id="Graphic_22" filter="url(#Shadow)">
        <path d="M 11.25 697.5 L 173.25 697.5 C 178.22056 697.5 182.25 701.5294 182.25 706.5 L 182.25 726.75 C 182.25 731.7206 178.22056 735.75 173.25 735.75 L 11.25 735.75 C 6.279437 735.75 2.25 731.7206 2.25 726.75 L 2.25 706.5 C 2.25 701.5294 6.279437 697.5 11.25 697.5 Z" fill="#ffc0c0"/>
        <path d="M 11.25 697.5 L 173.25 697.5 C 178.22056 697.5 182.25 701.5294 182.25 706.5 L 182.25 726.75 C 182.25 731.7206 178.22056 735.75 173.25 735.75 L 11.25 735.75 C 6.279437 735.75 2.25 731.7206 2.25 726.75 L 2.25 706.5 C 2.25 701.5294 6.279437 697.5 11.25 697.5 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(7.25 702.457)" fill="black">
          <tspan font-family="Helvetica Neue" font-size="12" font-weight="400" fill="black" x="74.446" y="11">Slot</tspan>
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="38.192383" y="25.336">bytesReceived</tspan>
        </text>
      </g>
      <g id="Graphic_21" filter="url(#Shadow)">
        <path d="M 198 711 L 198 724.5 L 233.50013 724.5 L 233.50013 731.25 L 253.50013 717.75 L 233.50013 704.25 L 233.50013 711 Z" fill="url(#Obj_Gradient_2)"/>
        <path d="M 198 711 L 198 724.5 L 233.50013 724.5 L 233.50013 731.25 L 253.50013 717.75 L 233.50013 704.25 L 233.50013 711 Z" stroke="#c01b00" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
      </g>
      <g id="Graphic_24" filter="url(#Shadow)">
        <path d="M 537.75 672.75 L 699.75 672.75 C 704.7206 672.75 708.75 676.7794 708.75 681.75 L 708.75 702 C 708.75 706.9706 704.7206 711 699.75 711 L 537.75 711 C 532.77944 711 528.75 706.9706 528.75 702 L 528.75 681.75 C 528.75 676.7794 532.77944 672.75 537.75 672.75 Z" fill="#c0ffc0"/>
        <path d="M 537.75 672.75 L 699.75 672.75 C 704.7206 672.75 708.75 676.7794 708.75 681.75 L 708.75 702 C 708.75 706.9706 704.7206 711 699.75 711 L 537.75 711 C 532.77944 711 528.75 706.9706 528.75 702 L 528.75 681.75 C 528.75 676.7794 532.77944 672.75 537.75 672.75 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(533.75 677.707)" fill="black">
          <tspan font-family="Helvetica Neue" font-size="12" font-weight="400" fill="black" x="68.446" y="11">Signal</tspan>
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="12.988281" y="25.336">vehicleHeartbeatInfo</tspan>
        </text>
      </g>
      <g id="Graphic_23" filter="url(#Shadow)">
        <path d="M 461.25 698.625 L 461.25 685.125 L 496.7501 685.125 L 496.7501 678.375 L 516.7501 691.875 L 496.7501 705.375 L 496.7501 698.625 Z" fill="url(#Obj_Gradient_3)"/>
        <path d="M 461.25 698.625 L 461.25 685.125 L 496.7501 685.125 L 496.7501 678.375 L 516.7501 691.875 L 496.7501 705.375 L 496.7501 698.625 Z" stroke="#c01b00" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
      </g>
      <g id="Graphic_28" filter="url(#Shadow)">
        <path d="M 277.50003 831.375 L 439.50003 831.375 C 444.4706 831.375 448.50003 835.4044 448.50003 840.375 L 448.50003 912.375 C 448.50003 917.3456 444.4706 921.375 439.50003 921.375 L 277.50003 921.375 C 272.52947 921.375 268.50003 917.3456 268.50003 912.375 L 268.50003 840.375 C 268.50003 835.4044 272.52947 831.375 277.50003 831.375 Z" fill="url(#Obj_Gradient_4)"/>
        <path d="M 277.50003 831.375 L 439.50003 831.375 C 444.4706 831.375 448.50003 835.4044 448.50003 840.375 L 448.50003 912.375 C 448.50003 917.3456 444.4706 921.375 439.50003 921.375 L 277.50003 921.375 C 272.52947 921.375 268.50003 917.3456 268.50003 912.375 L 268.50003 840.375 C 268.50003 835.4044 272.52947 831.375 277.50003 831.375 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(273.50003 869.375)" fill="black">
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="16.588867" y="11">MultiVehicleManager</tspan>
        </text>
      </g>
      <g id="Graphic_30" filter="url(#Shadow)">
        <path d="M 537.75 724.5 L 699.75 724.5 C 704.7206 724.5 708.75 728.5294 708.75 733.5 L 708.75 753.75 C 708.75 758.7206 704.7206 762.75 699.75 762.75 L 537.75 762.75 C 532.77944 762.75 528.75 758.7206 528.75 753.75 L 528.75 733.5 C 528.75 728.5294 532.77944 724.5 537.75 724.5 Z" fill="#c0ffc0"/>
        <path d="M 537.75 724.5 L 699.75 724.5 C 704.7206 724.5 708.75 728.5294 708.75 733.5 L 708.75 753.75 C 708.75 758.7206 704.7206 762.75 699.75 762.75 L 537.75 762.75 C 532.77944 762.75 528.75 758.7206 528.75 753.75 L 528.75 733.5 C 528.75 728.5294 532.77944 724.5 537.75 724.5 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(533.75 729.457)" fill="black">
          <tspan font-family="Helvetica Neue" font-size="12" font-weight="400" fill="black" x="68.446" y="11">Signal</tspan>
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="30.99121" y="25.336">messageReceived</tspan>
        </text>
      </g>
      <g id="Graphic_29" filter="url(#Shadow)">
        <path d="M 461.25 750.375 L 461.25 736.875 L 496.7501 736.875 L 496.7501 730.125 L 516.7501 743.625 L 496.7501 757.125 L 496.7501 750.375 Z" fill="url(#Obj_Gradient_5)"/>
        <path d="M 461.25 750.375 L 461.25 736.875 L 496.7501 736.875 L 496.7501 730.125 L 516.7501 743.625 L 496.7501 757.125 L 496.7501 750.375 Z" stroke="#c01b00" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
      </g>
      <g id="Graphic_32" filter="url(#Shadow)">
        <path d="M 11.25 857.25 L 173.25 857.25 C 178.22056 857.25 182.25 861.2794 182.25 866.25 L 182.25 886.5 C 182.25 891.4706 178.22056 895.5 173.25 895.5 L 11.25 895.5 C 6.279437 895.5 2.25 891.4706 2.25 886.5 L 2.25 866.25 C 2.25 861.2794 6.279437 857.25 11.25 857.25 Z" fill="#ffc0c0"/>
        <path d="M 11.25 857.25 L 173.25 857.25 C 178.22056 857.25 182.25 861.2794 182.25 866.25 L 182.25 886.5 C 182.25 891.4706 178.22056 895.5 173.25 895.5 L 11.25 895.5 C 6.279437 895.5 2.25 891.4706 2.25 886.5 L 2.25 866.25 C 2.25 861.2794 6.279437 857.25 11.25 857.25 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(7.25 862.207)" fill="black">
          <tspan font-family="Helvetica Neue" font-size="12" font-weight="400" fill="black" x="74.446" y="11">Slot</tspan>
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="12.988281" y="25.336">vehicleHeartbeatInfo</tspan>
        </text>
      </g>
      <g id="Graphic_31" filter="url(#Shadow)">
        <path d="M 194.24987 883.125 L 194.24987 869.625 L 229.75 869.625 L 229.75 862.875 L 249.75 876.375 L 229.75 889.875 L 229.75 883.125 Z" fill="url(#Obj_Gradient_6)"/>
        <path d="M 194.24987 883.125 L 194.24987 869.625 L 229.75 869.625 L 229.75 862.875 L 249.75 876.375 L 229.75 889.875 L 229.75 883.125 Z" stroke="#c01b00" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
      </g>
      <g id="Graphic_34" filter="url(#Shadow)">
        <path d="M 277.50003 994.75 L 439.50003 994.75 C 444.4706 994.75 448.50003 998.7794 448.50003 1003.75 L 448.50003 1075.75 C 448.50003 1080.7206 444.4706 1084.75 439.50003 1084.75 L 277.50003 1084.75 C 272.52947 1084.75 268.50003 1080.7206 268.50003 1075.75 L 268.50003 1003.75 C 268.50003 998.7794 272.52947 994.75 277.50003 994.75 Z" fill="url(#Obj_Gradient_7)"/>
        <path d="M 277.50003 994.75 L 439.50003 994.75 C 444.4706 994.75 448.50003 998.7794 448.50003 1003.75 L 448.50003 1075.75 C 448.50003 1080.7206 444.4706 1084.75 439.50003 1084.75 L 277.50003 1084.75 C 272.52947 1084.75 268.50003 1080.7206 268.50003 1075.75 L 268.50003 1003.75 C 268.50003 998.7794 272.52947 994.75 277.50003 994.75 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(273.50003 1032.75)" fill="black">
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="59.7959" y="11">Vehicle</tspan>
        </text>
      </g>
      <g id="Graphic_36" filter="url(#Shadow)">
        <path d="M 11.25 1020.625 L 173.25 1020.625 C 178.22056 1020.625 182.25 1024.6544 182.25 1029.625 L 182.25 1049.875 C 182.25 1054.8456 178.22056 1058.875 173.25 1058.875 L 11.25 1058.875 C 6.279437 1058.875 2.25 1054.8456 2.25 1049.875 L 2.25 1029.625 C 2.25 1024.6544 6.279437 1020.625 11.25 1020.625 Z" fill="#ffc0c0"/>
        <path d="M 11.25 1020.625 L 173.25 1020.625 C 178.22056 1020.625 182.25 1024.6544 182.25 1029.625 L 182.25 1049.875 C 182.25 1054.8456 178.22056 1058.875 173.25 1058.875 L 11.25 1058.875 C 6.279437 1058.875 2.25 1054.8456 2.25 1049.875 L 2.25 1029.625 C 2.25 1024.6544 6.279437 1020.625 11.25 1020.625 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(7.25 1025.582)" fill="black">
          <tspan font-family="Helvetica Neue" font-size="12" font-weight="400" fill="black" x="74.446" y="11">Slot</tspan>
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="30.99121" y="25.336">messageReceived</tspan>
        </text>
      </g>
      <g id="Graphic_35" filter="url(#Shadow)">
        <path d="M 194.24987 1046.5 L 194.24987 1033 L 229.75 1033 L 229.75 1026.25 L 249.75 1039.75 L 229.75 1053.25 L 229.75 1046.5 Z" fill="url(#Obj_Gradient_8)"/>
        <path d="M 194.24987 1046.5 L 194.24987 1033 L 229.75 1033 L 229.75 1026.25 L 249.75 1039.75 L 229.75 1053.25 L 229.75 1046.5 Z" stroke="#c01b00" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
      </g>
      <g id="Graphic_38" filter="url(#Shadow)">
        <path d="M 538.5001 1020.625 L 700.5001 1020.625 C 705.47064 1020.625 709.5001 1024.6544 709.5001 1029.625 L 709.5001 1049.875 C 709.5001 1054.8456 705.47064 1058.875 700.5001 1058.875 L 538.5001 1058.875 C 533.5295 1058.875 529.5001 1054.8456 529.5001 1049.875 L 529.5001 1029.625 C 529.5001 1024.6544 533.5295 1020.625 538.5001 1020.625 Z" fill="#c0ffc0"/>
        <path d="M 538.5001 1020.625 L 700.5001 1020.625 C 705.47064 1020.625 709.5001 1024.6544 709.5001 1029.625 L 709.5001 1049.875 C 709.5001 1054.8456 705.47064 1058.875 700.5001 1058.875 L 538.5001 1058.875 C 533.5295 1058.875 529.5001 1054.8456 529.5001 1049.875 L 529.5001 1029.625 C 529.5001 1024.6544 533.5295 1020.625 538.5001 1020.625 Z" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
        <text transform="translate(534.5001 1025.582)" fill="black">
          <tspan font-family="Helvetica Neue" font-size="12" font-weight="400" fill="black" x="74.782" y="11">Call</tspan>
          <tspan font-family="Courier" font-size="12" font-weight="400" fill="black" x="34.591797" y="25.336">writeBytesSafe</tspan>
        </text>
      </g>
      <g id="Graphic_39" filter="url(#Shadow)">
        <path d="M 461.25 1046.5 L 461.25 1033 L 496.7501 1033 L 496.7501 1026.25 L 516.7501 1039.75 L 496.7501 1053.25 L 496.7501 1046.5 Z" fill="url(#Obj_Gradient_9)"/>
        <path d="M 461.25 1046.5 L 461.25 1033 L 496.7501 1033 L 496.7501 1026.25 L 516.7501 1039.75 L 496.7501 1053.25 L 496.7501 1046.5 Z" stroke="#c01b00" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
      </g>
      <g id="Line_48">
        <line x1="358.50003" y1="930.625" x2="358.50003" y2="994.75" marker-start="url(#Diamond_Marker)" stroke="black" stroke-linecap="round" stroke-linejoin="round" stroke-width="1"/>
      </g>
    </g>
  </g>
</svg>         
  build: {
  "info" : {
    "version" : 1,
    "author" : "xcode"
  }
}
    runs-on: <?xml version="1.0" encoding="UTF-8" standalone="no"?>
<document type="com.apple.InterfaceBuilder3.CocoaTouch.XIB" version="3.0" toolsVersion="9059" systemVersion="14F1021" targetRuntime="iOS.CocoaTouch" propertyAccessControl="none" useAutolayout="YES" launchScreen="YES" useTraitCollections="YES">
    <dependencies>
        <deployment identifier="iOS"/>
        <plugIn identifier="com.apple.InterfaceBuilder.IBCocoaTouchPlugin" version="9049"/>
        <capability name="Constraints with non-1.0 multipliers" minToolsVersion="5.1"/>
    </dependencies>
    <objects>
        <placeholder placeholderIdentifier="IBFilesOwner" id="-1" userLabel="File's Owner"/>
        <placeholder placeholderIdentifier="IBFirstResponder" id="-2" customClass="UIResponder"/>
        <view contentMode="scaleToFill" id="iN0-l3-epB">
            <rect key="frame" x="0.0" y="0.0" width="480" height="480"/>
            <autoresizingMask key="autoresizingMask" widthSizable="YES" heightSizable="YES"/>
            <subviews>
                <label opaque="NO" clipsSubviews="YES" userInteractionEnabled="NO" contentMode="left" horizontalHuggingPriority="251" verticalHuggingPriority="251" misplaced="YES" text="" textAlignment="center" lineBreakMode="tailTruncation" baselineAdjustment="alignBaselines" minimumFontSize="9" translatesAutoresizingMaskIntoConstraints="NO" id="8ie-xW-0ye">
                    <rect key="frame" x="20" y="439" width="441" height="21"/>
                    <fontDescription key="fontDescription" type="system" pointSize="17"/>
                    <color key="textColor" cocoaTouchSystemColor="darkTextColor"/>
                    <nil key="highlightedColor"/>
                    <variation key="widthClass=compact">
                        <fontDescription key="fontDescription" type="system" pointSize="11"/>
                    </variation>
                </label>
                <label opaque="NO" clipsSubviews="YES" userInteractionEnabled="NO" contentMode="left" horizontalHuggingPriority="251" verticalHuggingPriority="251" text="QGroundControl" textAlignment="center" lineBreakMode="middleTruncation" baselineAdjustment="alignBaselines" minimumFontSize="18" translatesAutoresizingMaskIntoConstraints="NO" id="kId-c2-rCX">
                    <rect key="frame" x="20" y="140" width="441" height="43"/>
                    <fontDescription key="fontDescription" type="boldSystem" pointSize="36"/>
                    <color key="textColor" white="1" alpha="1" colorSpace="calibratedWhite"/>
                    <color key="highlightedColor" white="1" alpha="1" colorSpace="calibratedWhite"/>
                </label>
            </subviews>
            <color key="backgroundColor" white="0.0" alpha="1" colorSpace="calibratedWhite"/>
            <constraints>
                <constraint firstItem="kId-c2-rCX" firstAttribute="centerY" secondItem="iN0-l3-epB" secondAttribute="bottom" multiplier="1/3" constant="1" id="Kid-kn-2rF"/>
                <constraint firstAttribute="centerX" secondItem="kId-c2-rCX" secondAttribute="centerX" id="Koa-jz-hwk"/>
                <constraint firstAttribute="bottom" secondItem="8ie-xW-0ye" secondAttribute="bottom" constant="20" id="Kzo-t9-V3l"/>
                <constraint firstItem="8ie-xW-0ye" firstAttribute="leading" secondItem="iN0-l3-epB" secondAttribute="leading" constant="20" symbolic="YES" id="MfP-vx-nX0"/>
                <constraint firstAttribute="centerX" secondItem="8ie-xW-0ye" secondAttribute="centerX" id="ZEH-qu-HZ9"/>
                <constraint firstItem="kId-c2-rCX" firstAttribute="leading" secondItem="iN0-l3-epB" secondAttribute="leading" constant="20" symbolic="YES" id="fvb-Df-36g"/>
            </constraints>
            <nil key="simulatedStatusBarMetrics"/>
            <freeformSimulatedSizeMetrics key="simulatedDestinationMetrics"/>
            <point key="canvasLocation" x="404" y="445"/>
        </view>
    </objects>
</document>      
    outputs: <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>NSCameraUsageDescription</key>
        <string>QGC uses UVC devices for video streaming.</string>
        <key>CFBundleDisplayName</key>
        <string>QGroundControl</string>
        <key>CFBundleExecutable</key>
        <string>$(EXECUTABLE_NAME)</string>
        <key>NSHumanReadableCopyright</key>
        <string>Open Source Flight Systems GmbH - Internal Build</string>
        <key>CFBundleIconFile</key>
        <string></string>
        <key>CFBundleIdentifier</key>
        <string>org.QGroundControl.qgc</string>
        <key>CFBundleName</key>
        <string>$(PRODUCT_NAME)</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleShortVersionString</key>
        <string>0.0.0</string>
        <key>CFBundleSignature</key>
        <string>????</string>
        <key>CFBundleVersion</key>
        <string>1</string>
        <key>LSRequiresIPhoneOS</key>
        <true/>
        <key>UIRequiresFullScreen</key>
        <true/>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>ForAppStore</key>
        <string>No</string>
        <key>NSLocationUsageDescription</key>
        <string>Ground Station Location</string>
        <key>NSLocationWhenInUseUsageDescription</key>
        <string>Ground Station Location</string>
        <key>UILaunchStoryboardName</key>
        <string>QGCLaunchScreen</string>
        <key>UISupportedInterfaceOrientations</key>
        <array>
                <string>UIInterfaceOrientationLandscapeLeft</string>
                <string>UIInterfaceOrientationLandscapeRight</string>
        </array>
        <key>CFBundleIcons</key>
        <dict>
                <key>CFBundlePrimaryIcon</key>
                <dict>
                        <key>CFBundleIconFiles</key>
                        <array>
                                <string>AppIcon29x29.png</string>
                                <string>AppIcon29x29@2x.png</string>
                                <string>AppIcon40x40@2x.png</string>
                                <string>AppIcon57x57.png</string>
                                <string>AppIcon57x57@2x.png</string>
                                <string>AppIcon60x60@2x.png</string>
                        </array>
                </dict>
        </dict>
        <key>CFBundleIcons~ipad</key>
        <dict>
                <key>CFBundlePrimaryIcon</key>
                <dict>
                        <key>CFBundleIconFiles</key>
                        <array>
                                <string>AppIcon29x29.png</string>
                                <string>AppIcon29x29@2x.png</string>
                                <string>AppIcon40x40@2x.png</string>
                                <string>AppIcon57x57.png</string>
                                <string>AppIcon57x57@2x.png</string>
                                <string>AppIcon60x60@2x.png</string>
                                <string>AppIcon29x29~ipad.png</string>
                                <string>AppIcon29x29@2x~ipad.png</string>
                                <string>AppIcon40x40~ipad.png</string>
                                <string>AppIcon40x40@2x~ipad.png</string>
                                <string>AppIcon50x50~ipad.png</string>
                                <string>AppIcon50x50@2x~ipad.png</string>
                                <string>AppIcon72x72~ipad.png</string>
                                <string>AppIcon72x72@2x~ipad.png</string>
                                <string>AppIcon76x76~ipad.png</string>
                                <string>AppIcon76x76@2x~ipad.png</string>
                                <string>AppIcon83.5x83.5@2x~ipad.png</string>
                        </array>
                </dict>
        </dict>
        <key>UIFileSharingEnabled</key>
        <true/>
</dict>
</plist>       
      digests: <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDisplayName</key>
        <string>QGroundControl</string>
        <key>CFBundleDevelopmentRegion</key>
        <string>en</string>
        <key>CFBundleExecutable</key>
        <string>$(EXECUTABLE_NAME)</string>
        <key>NSHumanReadableCopyright</key>
        <string>Open Source Flight Systems GmbH</string>
        <key>CFBundleIconFile</key>
        <string></string>
        <key>NSCameraUsageDescription</key>
        <string>QGC uses UVC devices for video streaming.</string>
        <key>NSPhotoLibraryUsageDescription</key>
        <string>We do not access it. Apple thinks we do.</string>
        <key>UILaunchStoryboardName</key>
        <string>QGCLaunchScreen</string>
        <key>UIRequiredDeviceCapabilities</key>
        <array>
                <string>armv7</string>
        </array>
        <key>CFBundleIdentifier</key>
        <string>org.QGroundControl.qgc</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>CFBundleName</key>
        <string>$(PRODUCT_NAME)</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleShortVersionString</key>
        <string>###VERSION###</string>
        <key>CFBundleSignature</key>
        <string>????</string>
        <key>CFBundleVersion</key>
        <string>###BUILD###</string>
        <key>ForAppStore</key>
        <string>Yes</string>
        <key>LSRequiresIPhoneOS</key>
        <true/>
        <key>NSLocationUsageDescription</key>
        <string>Ground Station Location</string>
        <key>NSLocationWhenInUseUsageDescription</key>
        <string>Ground Station Location</string>
        <key>NSLocationAlwaysAndWhenInUseUsageDescription</key>
        <string>Ground Station Location</string>
        <key>NSLocationAlwaysUsageDescription</key>
        <string>Ground Station Location</string>
    <key>NSBluetoothPeripheralUsageDescription</key>
    <string>QGroundControl would like to use bluetooth.</string>    
        <key>UIRequiresFullScreen</key>
        <true/>
        <key>UISupportedInterfaceOrientations</key>
        <array>
                <string>UIInterfaceOrientationLandscapeLeft</string>
                <string>UIInterfaceOrientationLandscapeRight</string>
        </array>
        <key>UIFileSharingEnabled</key>
        <true/>
</dict>
</plist>        

    steps: CODE_SIGN_IDENTITY = "";
CODE_SIGNING_REQUIRED = NO;
CLANG_WARN_BOOL_CONVERSION = YES;
CLANG_WARN_CONSTANT_CONVERSION = YES;
CLANG_WARN_EMPTY_BODY = YES;
CLANG_WARN_ENUM_CONVERSION = YES;
CLANG_WARN_INT_CONVERSION = YES;
CLANG_WARN_UNREACHABLE_CODE = YES;
CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
ENABLE_STRICT_OBJC_MSGSEND = YES;
GCC_NO_COMMON_BLOCKS = YES;
GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
GCC_WARN_ABOUT_RETURN_TYPE = YES;
GCC_WARN_UNDECLARED_SELECTOR = YES;
GCC_WARN_UNINITIALIZED_AUTOS = YES;
GCC_WARN_UNUSED_FUNCTION = YES;
GCC_WARN_UNUSED_VARIABLE = YES;     
      - uses: CODE_SIGN_IDENTITY = "iPhone Distribution";
#PROVISIONING_PROFILE = f22bae36-10c2-4fd8-b6f1-c83e47765614;
CLANG_WARN_BOOL_CONVERSION = YES;
CLANG_WARN_CONSTANT_CONVERSION = YES;
CLANG_WARN_EMPTY_BODY = YES;
CLANG_WARN_ENUM_CONVERSION = YES;
CLANG_WARN_INT_CONVERSION = YES;
CLANG_WARN_UNREACHABLE_CODE = YES;
CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
ENABLE_STRICT_OBJC_MSGSEND = YES;
GCC_NO_COMMON_BLOCKS = YES;
GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
GCC_WARN_ABOUT_RETURN_TYPE = YES;
GCC_WARN_UNDECLARED_SELECTOR = YES;
GCC_WARN_UNINITIALIZED_AUTOS = YES;
GCC_WARN_UNUSED_FUNCTION = YES;
GCC_WARN_UNUSED_VARIABLE = YES;            

      # ========================================================
      #
      # Step 1: Build your artifacts.
      #
      # ========================================================
      - name: Build artifacts
        run: |
            # These are some amazing artifacts.
            echo "artifact1" > artifact1
            echo "artifact2" > artifact2

      # ========================================================
      #
      # Step 2: Add a step to generate the provenance subjects
      #         as shown below. Update the sha256 sum arguments
      #         to include all binaries that you generate
      #         provenance for.
      #
      # ========================================================
      - name: Generate subject for provenance
        id: hash
        run: |
          set -euo pipefail

          # List the artifacts the provenance will refer to.
          files=$(ls artifact*)
          # Generate the subjects (base64 encoded).
          echo "hashes=$(sha256sum $files | base64 -w0)" >> "${GITHUB_OUTPUT}"

  provenance:
    needs: [build]
    permissions:
      actions: read   # To read the workflow path.
      id-token: write # To sign the provenance.
      contents: write # To add assets to a release.
    uses: slsa-framework/slsa-github-generator/.github/workflows/generator_generic_slsa3.yml@v1.4.0
    with:
      base64-subjects: "${{ needs.build.outputs.digests }}"
      upload-assets: true # Optional: Upload to a new release
