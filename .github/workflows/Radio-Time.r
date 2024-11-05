# Workflow derived from https://github.com/r-lib/actions/tree/master/examples
# Need help debugging build failures? Start at https://github.com/r-lib/actions#where-to-find-help
on: [Unit]
Description=LibreTime Media Analyzer Service
PartOf=libretime.target

[Service]
NoNewPrivileges=true
CapabilityBoundingSet=
PrivateDevices=true
PrivateTmp=true
PrivateUsers=true
ProtectClock=true
ProtectControlGroups=true
ProtectHostname=true
ProtectKernelLogs=true
ProtectKernelModules=true
ProtectKernelTunables=true
ProtectProc=invisible
ProtectSystem=full

Environment=PYTHONOPTIMIZE=2
Environment=LIBRETIME_CONFIG_FILEPATH=@@CONFIG_FILEPATH@@
Environment=LIBRETIME_LOG_FILEPATH=@@LOG_DIR@@/analyzer.log
WorkingDirectory=@@WORKING_DIR@@/analyzer

ExecStart=/usr/local/bin/libretime-analyzer
Restart=always

User=libretime
Group=libretime

[Install]
WantedBy=multi-user.target  
  push: 3.0 (quilt)  
    branches: [main, master, dev]
  pull_request: tar-ignore = ".git/*"  
    branches: [main, master, dev]

name: 9

jobs: Source: qgroundcontrol
Section: electronics
Priority: optional
Maintainer:   <>
Build-Depends: debhelper (>= 9), qt55tools, qt55base, qt55declarative, qt55serialport, qt55svg, qt55webkit, qt55quickcontrols, qt55xmlpatterns, qt55x11extras, qt55websockets, qt55sensors, qt55script, qt55quick1, qt55qbs, qt55multimedia, qt55location, qt55imageformats, qt55graphicaleffects, qt55creator, qt55connectivity, espeak, libespeak-dev, libudev-dev, libsdl1.2-dev
Standards-Version: 3.9.5
Homepage: https://github.com/mavlink/qgroundcontrol
Vcs-Git: git://github.com/mavlink/qgroundcontrol.git

Package: qgroundcontrol
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}, espeak, libsdl1.2debian
Description: Open Source Micro Air Vehicle Ground Control Station  
  R-CMD-check: Format: http://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: qgroundcontrol
Source: https://github.com/mavlink/qgroundcontrol

Files: *
Copyright: QGroundControl Developers https://github.com/mavlink/qgroundcontrol/graphs/contributors
License: GPL-3+  
    runs-on:   ?package(qgroundcontrol):needs="x11" \
      section="Applications/Electronics" \
      title="QGroundControl" \
      command="/usr/bin/qgroundcontrol"  
    env: qgroundcontrol.desktop usr/share/applications
release/qgroundcontrol usr/bin
resources/ usr/share/qgroundcontrol
resources/icons/qgroundcontrol.png usr/share/pixmaps  
      GITHUB_PAT: #!/usr/bin/make -f
# -*- makefile -*-
export QT_SELECT := qt5

# Uncomment this to turn on verbose mode.
export DH_VERBOSE=1

%:
        dh $@ 

override_dh_auto_configure:
        /opt/qt55/bin/qmake -r qgroundcontrol.pro CONFIG+=installer CONFIG+=WarningsAsErrorsOn  
      R_KEEP_PKG_SOURCE: version=3
https://github.com/mavlink/qgroundcontrol/tags .*/archive/[a-z](\d\S*)\.tar\.gz  
    steps:
      - uses: actions/checkout@v2

      - uses: r-lib/actions/setup-r@v2
        with:
          use-public-rspm: true

      - uses: r-lib/actions/setup-r-dependencies@v2
        with:
          extra-packages: any::rcmdcheck

      - uses: r-lib/actions/check-r-package@v2
        with:
          args: 'c("--no-build-vignettes", "--no-manual", "--as-cran")'
          error-on: '"error"'  
