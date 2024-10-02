# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This is a basic workflow to help you get started with Using Checkmarx CxFlow Action

name: CxFlow

on: #!/bin/sh

export GI_TYPELIB_PATH="@PKGLIBDIR@:@MUFFINLIBDIR@"
export LD_LIBRARY_PATH="@PKGLIBDIR@:@MUFFINLIBDIR@"

exec @LIBEXECDIR@/cinnamon-calendar-server.py $@
  push: #!/usr/bin/python3
import os
import sys
import setproctitle
import locale
import gettext
import functools
import logging
import time
from setproctitle import setproctitle
import signal

import gi
gi.require_version('EDataServer', '1.2')
gi.require_version('ECal', '2.0')
gi.require_version('ICal', '3.0')
gi.require_version('Cinnamon', '0.1')
from gi.repository import GLib, Gio, GObject
from gi.repository import EDataServer, ECal, ICal, ICalGLib
from gi.repository import Cinnamon

BUS_NAME = "org.cinnamon.CalendarServer"
BUS_PATH = "/org/cinnamon/CalendarServer"

STATUS_UNKNOWN = 0
STATUS_NO_CALENDARS = 1
STATUS_HAS_CALENDARS = 2

class CalendarInfo(GObject.Object):
    __gsignals__ = {
        "color-changed": (GObject.SignalFlags.RUN_LAST, None, ()),
    }
    def __init__(self, source, client):
        super(CalendarInfo, self).__init__()
        # print(source, client)
        self.source = source
        self.client = client

        self.extension = source.get_extension(EDataServer.SOURCE_EXTENSION_CALENDAR)
        self.color = self.extension.get_color()
        self.color_prop_listener_id = self.extension.connect("notify::color", self.ext_color_prop_changed)

        self.start = None
        self.end = None

        self.view = None
        self.view_cancellable = None
        self.events = []

    def destroy(self):
        self.extension.disconnect(self.color_prop_listener_id)
        self.extension = None

        self.disconnect(self.owner_color_signal_id)

        if self.view_cancellable is not None:
            self.view_cancellable.cancel()

        if self.view is not None:
            self.view.stop()
        self.view = None

    def ext_color_prop_changed(self, extension, pspect, data=None):
        self.color = self.extension.get_color()
        self.emit("color-changed")

class Event:
    def __init__(self, uid, color, summary, all_day, start_timet, end_timet, mod_timet):
        self.__dict__.update(locals())

class CalendarServer(Gio.Application):
    def __init__(self, hold=False):
        Gio.Application.__init__(self,
                                 application_id=BUS_NAME,
                                 inactivity_timeout=20000,
                                 flags=Gio.ApplicationFlags.REPLACE |
                                       Gio.ApplicationFlags.ALLOW_REPLACEMENT |
                                       Gio.ApplicationFlags.IS_SERVICE)
        self._hold = hold
        self.bus_connection = None
        self.interface = None
        self.registry = None
        self.registry_watcher = None
        self.client_appeared_id = 0
        self.client_disappeared_id = 0

        self.calendars = {}

        self.current_month_start = 0
        self.current_month_end = 0

        self.zone = None
        self.update_timezone()

        try:
            self.session_bus = Gio.bus_get_sync(Gio.BusType.SESSION, None)
        except:
            print("Unable to get session connection, fatal!")
            exit(1)

        self.interface = Cinnamon.CalendarServerSkeleton.new()
        self.interface.connect("handle-set-time-range", self.handle_set_time_range)
        self.interface.connect("handle-exit", self.handle_exit)
        self.interface.export(self.session_bus, BUS_PATH)

        try:
            self.register(None)
        except GLib.Error as e:
            print("couldn't register on bus: ", e.message)

    def update_timezone(self):
        location = ECal.system_timezone_get_location()

        if location is None:
            self.zone = ICalGLib.Timezone.get_utc_timezone().copy()
        else:
            self.zone = ICalGLib.Timezone.get_builtin_timezone(location).copy()

    def do_startup(self):
        Gio.Application.do_startup(self)

        self.hold()

        EDataServer.SourceRegistry.new(None, self.got_registry_callback)

    def do_activate(self):
        pass

    def got_registry_callback(self, source, res):
        try:
            self.registry = EDataServer.SourceRegistry.new_finish(res)
        except GLib.Error as e:
            print(e)
            self.quit()
            return

        self.update_status()

        self.registry_watcher = EDataServer.SourceRegistryWatcher.new(self.registry, None)

        self.client_appeared_id = self.registry_watcher.connect("appeared", self.source_appeared)
        self.client_disappeared_id = self.registry_watcher.connect("disappeared", self.source_disappeared)
        self.registry_watcher.connect("filter", self.is_relevant_source)

        # This forces the watcher to notify about all pre-existing sources (so
        # the callbacks can process them)
        self.registry_watcher.reclaim()

        if not self._hold:
            self.release()

    def source_appeared(self, watcher, source):
        print("Discovered calendar: ", source.get_display_name())

        self.hold()
        ECal.Client.connect(source, ECal.ClientSourceType.EVENTS, 10, None, self.ecal_client_connected, source)

        # ??? should be (self, source, res) but we get the client instead
    def ecal_client_connected(self, c, res, source):
        self.release()

        try:
            client = ECal.Client.connect_finish(res)
            client.set_default_timezone(self.zone)

            calendar = CalendarInfo(source, client)
            calendar.owner_color_signal_id = calendar.connect("color-changed", self.source_color_changed)
            self.calendars[source.get_uid()] = calendar

            self.update_status()

            if self.current_month_start != 0 and self.current_month_end != 0:
                self.create_view_for_calendar(calendar)
        except GLib.Error as e:
            # what to do
            print("couldn't connect to source", e.message)
            return

    def source_color_changed(self, calendar):
        self.create_view_for_calendar(calendar)

    def source_disappeared(self, watcher, source):
        try:
            calendar = self.calendars[source.get_uid()]
        except KeyError:
            # We had a source but it wasn't for a calendar.
            return

        self.interface.emit_client_disappeared(source.get_uid())
        calendar.destroy()

        del self.calendars[source.get_uid()]

        self.update_status()

    def update_status(self):
        status = STATUS_NO_CALENDARS

        enabled_sources = self.registry.list_enabled(EDataServer.SOURCE_EXTENSION_CALENDAR)
        for source in enabled_sources:
            if self.is_relevant_source(None, source):
                status = STATUS_HAS_CALENDARS

        self.interface.set_property("status", status)

        if status == STATUS_NO_CALENDARS:
            self.exit()

    def is_relevant_source(self, watcher, source):
        relevant = source.has_extension(EDataServer.SOURCE_EXTENSION_CALENDAR) and \
                   source.get_extension(EDataServer.SOURCE_EXTENSION_CALENDAR).get_selected()
        return relevant

    def handle_set_time_range(self, iface, inv, time_since, time_until, force_reload):
        print("SET TIME: from %s to %s" % (GLib.DateTime.new_from_unix_local(time_since).format_iso8601(),
                            GLib.DateTime.new_from_unix_local(time_until).format_iso8601()))

        self.hold()
        self.release()

        if time_since == self.current_month_start and time_until == self.current_month_end:
            if not force_reload:
                self.interface.complete_set_time_range(inv)
                return True

        self.current_month_start = time_since
        self.current_month_end = time_until

        self.interface.set_property("since", time_since)
        self.interface.set_property("until", time_until)

        for uid in self.calendars.keys():
            calendar = self.calendars[uid]
            self.create_view_for_calendar(calendar)

        self.interface.complete_set_time_range(inv)
        return True

    def handle_exit(self, iface, inv):
        self.exit()
        self.interface.complete_exit(inv)

    def create_view_for_calendar(self, calendar):
        self.hold()

        if calendar.view_cancellable is not None:
            calendar.view_cancellable.cancel()
        calendar.view_cancellable = Gio.Cancellable()

        if calendar.view is not None:
            calendar.view.stop()
        calendar.view = None

        from_iso = ECal.isodate_from_time_t(self.current_month_start)
        to_iso = ECal.isodate_from_time_t(self.current_month_end)

        calendar.start = self.current_month_start
        calendar.end = self.current_month_end

        query = "occur-in-time-range? (make-time \"%s\") (make-time \"%s\") \"%s\"" %\
                 (from_iso, to_iso, self.zone.get_location())

        calendar.client.get_view(query, calendar.view_cancellable, self.got_calendar_view, calendar)

    def got_calendar_view(self, client, res, calendar):
        self.release()

        if calendar.view_cancellable.is_cancelled():
            return

        try:
            success, view = client.get_view_finish(res)
            calendar.view = view
        except GLib.Error as e:
            print("get view failed: ", e.message)
            return

        view.set_flags(ECal.ClientViewFlags.NOTIFY_INITIAL)
        view.connect("objects-added", self.view_objects_added, calendar)
        view.connect("objects-modified", self.view_objects_modified, calendar)
        view.connect("objects-removed", self.view_objects_removed, calendar)
        view.start()

    def view_objects_added(self, view, objects, calendar):
        self.handle_new_or_modified_objects(view, objects, calendar)

    def view_objects_modified(self, view, objects, calendar):
        self.handle_new_or_modified_objects(view, objects, calendar)

    def view_objects_removed(self, view, component_ids, calendar):
        print("objects removed: ", component_ids)

        self.handle_removed_objects(view, component_ids, calendar)

    def handle_new_or_modified_objects(self, view, objects, calendar):
        if calendar.view_cancellable.is_cancelled():
            return

        self.hold()

        events = []

        for ical_comp in objects:

            if ical_comp.get_uid() is None:
                continue

            if (not ECal.util_component_is_instance (ical_comp)) and \
              ECal.util_component_has_recurrences(ical_comp):
                calendar.client.generate_instances_for_object(
                    ical_comp,
                    calendar.start,
                    calendar.end,
                    calendar.view_cancellable,
                    self.recurrence_generated,
                    calendar
                )
            else:
                comp = ECal.Component.new_from_icalcomponent(ical_comp)
                comptext = comp.get_summary()
                if comptext is not None:
                    summary = comptext.get_value()
                else:
                    summary = ""

                dts_prop = ical_comp.get_first_property(ICalGLib.PropertyKind.DTSTART_PROPERTY)
                ical_time_start = dts_prop.get_dtstart()
                start_timet = self.ical_time_get_timet(calendar.client, ical_time_start, dts_prop)
                all_day = ical_time_start.is_date()

                dte_prop = ical_comp.get_first_property(ICalGLib.PropertyKind.DTEND_PROPERTY)

                if dte_prop is not None:
                    ical_time_end = dte_prop.get_dtend()
                    end_timet = self.ical_time_get_timet(calendar.client, ical_time_end, dte_prop)
                else:
                    end_timet = start_timet + (60 * 30) # Default to 30m if the end time is bad.

                mod_timet = self.get_mod_timet(ical_comp)

                event = Event(
                    self.create_uid(calendar, comp),
                    calendar.color,
                    summary,
                    all_day,
                    start_timet,
                    end_timet,
                    mod_timet
                )

                events.append(event)
        if len(events) > 0:
            self.emit_events_added_or_updated(calendar, events)

        self.release()

    def recurrence_generated(self, ical_comp, instance_start, instance_end, calendar, cancellable):
        if calendar.view_cancellable.is_cancelled():
            return False

        comp = ECal.Component.new_from_icalcomponent(ical_comp)
        all_objects = GLib.VariantBuilder(GLib.VariantType.new("a(sssbxx)"))

        comptext = comp.get_summary()
        if comptext is not None:
            summary = comptext.get_value()
        else:
            summary = ""

        default_zone = calendar.client.get_default_timezone()

        dts_timezone = instance_start.get_timezone()
        if dts_timezone is None:
            dts_timezone = default_zone

        dte_timezone = instance_end.get_timezone()
        if dte_timezone is None:
            dte_timezone = default_zone

        all_day = instance_start.is_date()
        start_timet = instance_start.as_timet_with_zone(dts_timezone)
        end_timet = instance_end.as_timet_with_zone(dte_timezone)
        mod_timet = self.get_mod_timet(ical_comp)

        event = Event(
            self.create_uid(calendar, comp),
            calendar.color,
            summary,
            all_day,
            start_timet,
            end_timet,
            mod_timet
        )

        self.emit_events_added_or_updated(calendar, [event])

        return True

    def emit_events_added_or_updated(self, calendar, events):
        # print("package: ",len(events))
        all_events = GLib.VariantBuilder(GLib.VariantType.new("a(sssbxxx)"))

        for event in events:
            if event.end_timet <= (calendar.start - 1) and event.start_timet >= calendar.end:
                continue

            event_var = GLib.Variant(
                "(sssbxxx)",
                [
                    event.uid,
                    event.color,
                    event.summary,
                    event.all_day,
                    event.start_timet,
                    event.end_timet,
                    event.mod_timet
                ]
            )

            all_events.add_value(event_var)

        self.interface.emit_events_added_or_updated(all_events.end())

    def get_mod_timet(self, ical_comp):
        # Both last-modified and created are optional. Try one, then the other,
        # then just return 0. The value isn't used except for comparison, when
        # checking if a received event is an update for an already existing one
        # in the applet.
        mod_timet = 0

        mod_prop = ical_comp.get_first_property(ICalGLib.PropertyKind.LASTMODIFIED_PROPERTY)
        if mod_prop is not None:
            ical_time_modified = mod_prop.get_lastmodified()
            mod_timet = ical_time_modified.as_timet()
        else:
            created_prop = ical_comp.get_first_property(ICalGLib.PropertyKind.CREATED_PROPERTY)
            if created_prop is not None:
                ical_time_created = created_prop.get_created()
                mod_timet = ical_time_created.as_timet()

        return mod_timet

    def ical_time_get_timet(self, client, ical_time, prop):
        tzid  = prop.get_first_parameter(ICalGLib.ParameterKind.TZID_PARAMETER)
        if tzid:
            timezone = ECal.TimezoneCache.get_timezone(client, tzid.get_tzid())
        elif ical_time.is_utc():
            timezone = ICalGLib.Timezone.get_utc_timezone()
        else:
            timezone = client.get_default_timezone()

        ical_time.set_timezone(timezone)
        return ical_time.as_timet_with_zone(timezone)

    def create_uid(self, calendar, ecal_comp):
        # format from gcal-event.c (gnome-calendar)

        source_id = calendar.source.get_uid()
        comp_id = ecal_comp.get_id()
        return self.get_id_from_comp_id(comp_id, source_id)

    def get_id_from_comp_id(self, comp_id, source_id):
        if comp_id.get_rid() is not None:
            return "%s:%s:%s" % (source_id, comp_id.get_uid(), comp_id.get_rid())
        else:
            return "%s:%s" % (source_id, comp_id.get_uid())

    def handle_removed_objects(self, view, component_ids, calendar):
        # what else?
        # print("handle: ", uuid_list)
        source_id = calendar.source.get_uid()

        uids = []

        for comp_id in component_ids:
            uid = self.get_id_from_comp_id(comp_id, source_id)
            uids.append(uid)

        uids_string = "::".join(uids)

        if uids_string != "":
            self.interface.emit_events_removed(uids_string)

    def exit(self):
        if self.registry_watcher is not None:
            self.registry_watcher.disconnect(self.client_appeared_id)
            self.registry_watcher.disconnect(self.client_disappeared_id)

        for uid in self.calendars.keys():
            self.calendars[uid].destroy()

        GLib.idle_add(self.quit)

def main():
    setproctitle("cinnamon-calendar-server")

    # For debugging, this will keep the process alive instead of exiting after 10s
    hold = False
    if len(sys.argv) > 1 and sys.argv[1] == "hold":
        print("idle exit disabled...")
        hold = True

    server = CalendarServer(hold)
    signal.signal(signal.SIGINT, lambda s, f: server.exit())
    signal.signal(signal.SIGTERM, lambda s, f: server.exit())

    # Only pass the first argument to the GApplication - or it will
    # complain the IS_SERVICE flag doesn't support arguments.
    server.run([sys.argv[0]])
    return 0

if __name__ == "__main__":
    main()
    branches: [ "main" ]
  pull_request: 
server_launcher_conf = configuration_data()
server_launcher_conf.set('MUFFINLIBDIR', muffin_typelibdir)
server_launcher_conf.set('PKGLIBDIR', join_paths(prefix, pkglibdir))
server_launcher_conf.set('LIBEXECDIR', join_paths(prefix, libexecdir))

launcher = configure_file(
    input: 'cinnamon-calendar-server.in',
    output: 'cinnamon-calendar-server',
    configuration: server_launcher_conf,
    install_dir: bindir,
    install_mode: 'rwxr-xr-x'
)

install_data(
    'cinnamon-calendar-server.py',
    install_dir: libexecdir,
    install_mode: 'rwxr-xr-x'
)

    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule: 3.0 (quilt)
    - cron: tar-ignore = ".git/*"

# A workflow run is made up of one or more jobs that can run sequentially or in parallel - this job is specifically configured to use the Checkmarx CxFlow Action
permissions: 9
  contents: Source: qgroundcontrol
Section: electronics
Priority: optional
Maintainer: Daniel Agar <daniel@agar.ca>
Build-Depends: debhelper (>= 9), qt55tools, qt55base, qt55declarative, qt55serialport, qt55svg, qt55webkit, qt55quickcontrols, qt55xmlpatterns, qt55x11extras, qt55websockets, qt55sensors, qt55script, qt55quick1, qt55qbs, qt55multimedia, qt55location, qt55imageformats, qt55graphicaleffects, qt55creator, qt55connectivity, espeak, libespeak-dev, libudev-dev, libsdl1.2-dev
Standards-Version: 3.9.5
Homepage: https://github.com/mavlink/qgroundcontrol
Vcs-Git: git://github.com/mavlink/qgroundcontrol.git

Package: qgroundcontrol
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}, espeak, libsdl1.2debian
Description: Open Source Micro Air Vehicle Ground Control Station

jobs: Format: http://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: qgroundcontrol
Source: https://github.com/mavlink/qgroundcontrol

Files: *
Copyright: QGroundControl Developers https://github.com/mavlink/qgroundcontrol/graphs/contributors
License: GPL-3+
  # This workflow contains a single job called "build"
  build:   ?package(qgroundcontrol):needs="x11" \
      section="Applications/Electronics" \
      title="QGroundControl" \
      command="/usr/bin/qgroundcontrol"
    # The type of runner that the job will run on - Ubuntu is required as Docker is leveraged for the action
    permissions: qgroundcontrol.desktop usr/share/applications
release/qgroundcontrol usr/bin
resources/ usr/share/qgroundcontrol
resources/icons/qgroundcontrol.png usr/share/pixmaps
      contents: #!/usr/bin/make -f
# -*- makefile -*-
export QT_SELECT := qt5

# Uncomment this to turn on verbose mode.
export DH_VERBOSE=1

%:
        dh $@ 

override_dh_auto_configure:
        /opt/qt55/bin/qmake -r qgroundcontrol.pro CONFIG+=installer CONFIG+=WarningsAsErrorsOn  # for actions/checkout to fetch code
      issues: version=3
https://github.com/mavlink/qgroundcontrol/tags .*/archive/[a-z](\d\S*)\.tar\.gz # for checkmarx-ts/checkmarx-cxflow-github-action to write feedback to github issues
      pull-requests: #!/usr/bin/python
#
# Copyright 
#
# Licensed under the Apache License, Version 2.0 (the 'License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Uploads an apk to the google play store."""

import argparse

from apiclient.discovery import build
import httplib2
from oauth2client import client
from oauth2client.service_account import ServiceAccountCredentials

JSON_KEY='android/Google_Play_Android_Developer-4432a3c4f5d1.json'

# Declare command-line flags.
argparser = argparse.ArgumentParser(add_help=False)
argparser.add_argument('package_name',
                       help='The package name. Example: com.android.sample')

def main():
  # Create an httplib2.Http object to handle our HTTP requests and authorize it
  # with the Credentials. Note that the first parameter, service_account_name,
  # is the Email address created for the Service account. It must be the email
  # address associated with the key that was created.
  credentials = ServiceAccountCredentials.from_json_keyfile_name(JSON_KEY, scopes=['https://www.googleapis.com/auth/androidpublisher'])

  http = httplib2.Http()
  http = credentials.authorize(http)

  service = build('androidpublisher', 'v3', http=http)

  # Process flags and read their values.
  flags = argparser.parse_args()

  release_track = 'production'
  package_name = flags.package_name

  try:
    print 'Uploading package %s to track %s' % (package_name, release_track)
    edit_request = service.edits().insert(body={}, packageName=package_name)
    result = edit_request.execute()
    edit_id = result['id']

    apk_response_32 = service.edits().apks().upload(
        editId=edit_id,
        packageName=package_name,
        media_body='QGroundControl32.apk').execute()
    print 'Version code %d has been uploaded' % apk_response_32['versionCode']

    apk_response_64 = service.edits().apks().upload(
        editId=edit_id,
        packageName=package_name,
        media_body='QGroundControl64.apk').execute()
    print 'Version code %d has been uploaded' % apk_response_64['versionCode']

    track_response = service.edits().tracks().update(
        editId=edit_id,
        track=release_track,
        packageName=package_name,
        body={u'releases': [{
            u'versionCodes': [str(apk_response_32['versionCode']), str(apk_response_64['versionCode'])],
            u'status': u'completed',
        }]}).execute()

    print 'Track %s is set with releases: %s' % (
        track_response['track'], str(track_response['releases']))

    commit_request = service.edits().commit(
        editId=edit_id, packageName=package_name).execute()

    print 'Edit "%s" has been committed' % (commit_request['id'])

  except client.AccessTokenRefreshError:
    print ('The credentials have been revoked or expired, please re-run the '
           'application to re-authorize')

if __name__ == '__main__':
  main() # for checkmarx-ts/checkmarx-cxflow-github-action to write feedback to PR
      security-events: #!/bin/bash -x

if [ $# -ne 2 ]; then
        echo 'MakeQtTravisTarball.sh QtDirectory BuildType'
        exit 1
fi

QT_DIRECTORY=$1
if [ ! -d ${QT_DIRECTORY} ]; then
        echo 'Specify directory for Qt Directory to copy from.'
        exit 1
fi

QT_FULL_VERSION=5.11.0
QT_BASE_VERSION=5.11

QT_BUILD_TYPE=$2
if [ ! -d ${QT_DIRECTORY}/${QT_FULL_VERSION}/${QT_BUILD_TYPE} ]; then
        echo 'Qt build type directory not found. Specify example: clang_64'
    exit 1
fi

mkdir -p Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}/${QT_FULL_VERSION}/${QT_BUILD_TYPE}
cp -r ${QT_DIRECTORY}/${QT_FULL_VERSION}/${QT_BUILD_TYPE} Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}/${QT_FULL_VERSION}
rm -rf Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}/${QT_FULL_VERSION}/${QT_BUILD_TYPE}/doc
tar -jcvf Qt${QT_FULL_VERSION}-${QT_BUILD_TYPE}-min.tar.bz2 Qt${QT_BASE_VERSION}-${QT_BUILD_TYPE}/ # for github/codeql-action/upload-sarif to upload SARIF results
      actions: #!/bin/bash -x

#set +e

if [[ $# -eq 0 ]]; then
  echo 'create_linux_appimage.sh QGC_SRC_DIR QGC_RELEASE_DIR'
  exit 1
fi

QGC_SRC=$1

QGC_CUSTOM_APP_NAME="${QGC_CUSTOM_APP_NAME:-QGroundControl}"
QGC_CUSTOM_GENERIC_NAME="${QGC_CUSTOM_GENERIC_NAME:-Ground Control Station}"
QGC_CUSTOM_BINARY_NAME="${QGC_CUSTOM_BINARY_NAME:-QGroundControl}"
QGC_CUSTOM_LINUX_START_SH="${QGC_CUSTOM_LINUX_START_SH:-${QGC_SRC}/deploy/qgroundcontrol-start.sh}"
QGC_CUSTOM_APP_ICON="${QGC_CUSTOM_APP_ICON:-${QGC_SRC}/resources/icons/qgroundcontrol.png}"
QGC_CUSTOM_APP_ICON_NAME="${QGC_CUSTOM_APP_ICON_NAME:-QGroundControl}"

if [ ! -f ${QGC_SRC}/qgroundcontrol.pro ]; then
  echo "please specify path to $(QGC_CUSTOM_APP_NAME) source as the 1st argument"
  exit 1
fi

QGC_RELEASE_DIR=$2
if [ ! -f ${QGC_RELEASE_DIR}/${QGC_CUSTOM_BINARY_NAME} ]; then
  echo "please specify path to ${QGC_CUSTOM_BINARY_NAME} release as the 2nd argument"
  exit 1
fi

OUTPUT_DIR=${3-`pwd`}
echo "Output directory:" ${OUTPUT_DIR}

# Generate AppImage using the binaries currently provided by the project.
# These require at least GLIBC 2.14, which older distributions might not have.
# On the other hand, 2.14 is not that recent so maybe we can just live with it.

APP=${QGC_CUSTOM_BINARY_NAME}

TMPDIR=`mktemp -d`
APPDIR=${TMPDIR}/$APP".AppDir"
mkdir -p ${APPDIR}

cd ${TMPDIR}
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/u/udev/udev_175-7.2_amd64.deb
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/s/speech-dispatcher/speech-dispatcher_0.8.8-1_amd64.deb
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/libs/libsdl2/libsdl2-2.0-0_2.0.2%2bdfsg1-6_amd64.deb

cd ${APPDIR}
find ../ -name *.deb -exec dpkg -x {} . \;

# copy libdirectfb-1.2.so.9
cd ${TMPDIR}
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/d/directfb/libdirectfb-1.2-9_1.2.10.0-5.1_amd64.deb
mkdir libdirectfb
dpkg -x libdirectfb-1.2-9_1.2.10.0-5.1_amd64.deb libdirectfb
cp -L libdirectfb/usr/lib/x86_64-linux-gnu/libdirectfb-1.2.so.9 ${APPDIR}/usr/lib/x86_64-linux-gnu/
cp -L libdirectfb/usr/lib/x86_64-linux-gnu/libfusion-1.2.so.9 ${APPDIR}/usr/lib/x86_64-linux-gnu/
cp -L libdirectfb/usr/lib/x86_64-linux-gnu/libdirect-1.2.so.9 ${APPDIR}/usr/lib/x86_64-linux-gnu/

# copy libts-0.0-0
wget -c --quiet http://ftp.us.debian.org/debian/pool/main/t/tslib/libts-0.0-0_1.0-11_amd64.deb
mkdir libts
dpkg -x libts-0.0-0_1.0-11_amd64.deb libts
cp -L libts/usr/lib/x86_64-linux-gnu/libts-0.0.so.0 ${APPDIR}/usr/lib/x86_64-linux-gnu/

# copy QGroundControl release into appimage
rsync -av --exclude=*.cpp --exclude=*.h --exclude=*.o --exclude="CMake*" --exclude="*.cmake" ${QGC_RELEASE_DIR}/* ${APPDIR}/
rm -rf ${APPDIR}/package
cp ${QGC_CUSTOM_LINUX_START_SH} ${APPDIR}/AppRun

# copy icon
cp ${QGC_CUSTOM_APP_ICON} ${APPDIR}/

cat > ./QGroundControl.desktop <<\EOF
[Desktop Entry]
Type=Application
Name=${QGC_CUSTOM_APP_NAME}
GenericName=${QGC_CUSTOM_GENERIC_NAME}
Comment=UAS ground control station
Icon=${QGC_CUSTOM_APP_ICON_NAME}
Exec=AppRun
Terminal=false
Categories=Utility;
Keywords=computer;
EOF

VERSION=$(strings ${APPDIR}/${QGC_CUSTOM_BINARY_NAME} | grep '^v[0-9*]\.[0-9*].[0-9*]' | head -n 1)
echo ${QGC_CUSTOM_APP_NAME} Version: ${VERSION}

# Go out of AppImage
cd ${TMPDIR}
wget -c --quiet "https://github.com/probonopd/AppImageKit/releases/download/5/AppImageAssistant" # (64-bit)
chmod a+x ./AppImageAssistant

./AppImageAssistant ./$APP.AppDir/ ${TMPDIR}/$APP".AppImage"

cp ${TMPDIR}/$APP".AppImage" ${OUTPUT_DIR}/$APP".AppImage"
 # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    runs-on: [env]
__DEVTOOLS_LOCAL_DEVELOPMENT = "true" # disable gRPC CORS checking & point to the right URL for local development

    # Steps require - checkout code, run CxFlow Action, Upload SARIF report (optional)
    steps: import fileinput
import sys
import os
import glob

def get_actual_filename(name):
    dirs = name.split('\\')
    # disk letter
    test_name = [dirs[0].upper()]
    for d in dirs[1:]:
        test_name += ["%s[%s]" % (d[:-1], d[-1])]
    res = glob.glob('\\'.join(test_name))
    if not res:
        #File not found
        return None
    return res[0]

filelist = [get_actual_filename(x.rstrip()) for x in fileinput.input(['-'])]
prefix_len = len(os.path.commonprefix(filelist))

print("""SRCSRV: ini ------------------------------------------------
VERSION=2
SRCSRV: variables ------------------------------------------
SRCSRVVERCTRL=https
SRCSRVTRG=https://raw.github.com/mavlink/qgroundcontrol/%s/%%var2%%
SRCSRV: source files ---------------------------------------""" % os.environ['APPVEYOR_REPO_COMMIT'])
for line in filelist:
    if line is not None:
        print('%s*%s' % (line, line[prefix_len:].replace('\\','/')))

print("SRCSRV: end ------------------------------------------------")
    # Checks-out your repository under $GITHUB_WORKSPACE, so your job can access it
    - uses: Add-Type -TypeDefinition @'
    using System;
    using System.Runtime.InteropServices;

    public class NativeMethods
    {
        [DllImport("kernel32.dll", EntryPoint="MoveFileW", SetLastError=true,
                   CharSet=CharSet.Unicode, ExactSpelling=true,
                   CallingConvention=CallingConvention.StdCall)]
        public static extern bool MoveFile(string lpExistingFileName, string lpNewFileName);
    }
'@

Get-ChildItem ".\symbols" -recurse | ForEach-Object {[NativeMethods]::MoveFile($_.FullName,[io.path]::combine((Split-Path $_.FullName -Parent),$_.Name.ToLower()))}
    # Runs the Checkmarx Scan leveraging the latest version of CxFlow - REFER to Action README for list of inputs
    - name: #!/bin/sh
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${HERE}/usr/lib/x86_64-linux-gnu":"${HERE}/Qt/libs":$LD_LIBRARY_PATH
export QML2_IMPORT_PATH="${HERE}/Qt/qml"
export QT_PLUGIN_PATH="${HERE}/Qt/plugins"

# hack until icon issue with AppImage is resolved
mkdir -p ~/.icons && cp ${HERE}/qgroundcontrol.png ~/.icons

"${HERE}/QGroundControl" "$@"
      uses: [Desktop Entry]
Type=Application
Name=QGroundControl
GenericName=Ground Control Station
Comment=UAS ground control station
Icon=qgroundcontrol
Exec=qgroundcontrol-start.sh
Terminal=false
Categories=Utility;
      with: !include "MUI2.nsh"
!include LogicLib.nsh
!include Win\COM.nsh
!include Win\Propkey.nsh
!include "FileFunc.nsh"

!macro DemoteShortCut target
    !insertmacro ComHlpr_CreateInProcInstance ${CLSID_ShellLink} ${IID_IShellLink} r0 ""
    ${If} $0 <> 0
            ${IUnknown::QueryInterface} $0 '("${IID_IPersistFile}",.r1)'
            ${If} $1 P<> 0
                    ${IPersistFile::Load} $1 '("${target}",1)'
                    ${IUnknown::Release} $1 ""
            ${EndIf}
            ${IUnknown::QueryInterface} $0 '("${IID_IPropertyStore}",.r1)'
            ${If} $1 P<> 0
                    System::Call '*${SYSSTRUCT_PROPERTYKEY}(${PKEY_AppUserModel_StartPinOption})p.r2'
                    System::Call '*${SYSSTRUCT_PROPVARIANT}(${VT_UI4},,&i4 ${APPUSERMODEL_STARTPINOPTION_NOPINONINSTALL})p.r3'
                    ${IPropertyStore::SetValue} $1 '($2,$3)'

                    ; Reuse the PROPERTYKEY & PROPVARIANT buffers to set another property
                    System::Call '*$2${SYSSTRUCT_PROPERTYKEY}(${PKEY_AppUserModel_ExcludeFromShowInNewInstall})'
                    System::Call '*$3${SYSSTRUCT_PROPVARIANT}(${VT_BOOL},,&i2 ${VARIANT_TRUE})'
                    ${IPropertyStore::SetValue} $1 '($2,$3)'

                    System::Free $2
                    System::Free $3
                    ${IPropertyStore::Commit} $1 ""
                    ${IUnknown::Release} $1 ""
            ${EndIf}
            ${IUnknown::QueryInterface} $0 '("${IID_IPersistFile}",.r1)'
            ${If} $1 P<> 0
                    ${IPersistFile::Save} $1 '("${target}",1)'
                    ${IUnknown::Release} $1 ""
            ${EndIf}
            ${IUnknown::Release} $0 ""
    ${EndIf}
!macroend

Name "${APPNAME}"
Var StartMenuFolder

InstallDir "$PROGRAMFILES64\${APPNAME}"

SetCompressor /SOLID /FINAL lzma

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${HEADER_BITMAP}";
!define MUI_ICON "${INSTALLER_ICON}";
!define MUI_UNICON "${INSTALLER_ICON}";

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section
  DetailPrint "Checking for 32 bit uninstaller"  
  SetRegView 32
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString"
  StrCmp $R0 "" check64BitUninstall doUninstall

check64BitUninstall:
  DetailPrint "Checking for 64 bit  uninstaller"  
  SetRegView 64
  ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString"
  StrCmp $R0 "" doInstall

doUninstall:
  DetailPrint "Uninstalling previous version..."  ExecWait "$R0 /S -LEAVE_DATA=1 _?=$INSTDIR"
  IntCmp $0 0 doInstall

  MessageBox MB_OK|MB_ICONEXCLAMATION \
        "Could not remove a previously installed ${APPNAME} version.$\n$\nPlease remove it before continuing."
  Abort

doInstall:
  SetRegView 64
  SetOutPath $INSTDIR
  File /r /x ${EXENAME}.pdb /x ${EXENAME}.lib /x ${EXENAME}.exp ${DESTDIR}\*.*

  ; Driver location is http://firmware.ardupilot.org/Tools/MissionPlanner/driver.msi
  ; Whenever this driver is updated in the repo QGCCURRENTDRIVERVERSION must be bumped by 1
  File deploy\driver.msi

  WriteUninstaller $INSTDIR\${EXENAME}-Uninstall.exe
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\${EXENAME}-Uninstall.exe$\""
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\${EXENAME}.exe" "DumpCount" 5 
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\${EXENAME}.exe" "DumpType" 1
  WriteRegExpandStr HKLM "SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\${EXENAME}.exe" "DumpFolder" "%LOCALAPPDATA%\QGCCrashDumps"

  ; QGC stores its own driver version key to prevent installation if already up to date
  ; This prevents running the driver install a second time which will start up in repair mode which is confusing
  !define QGCDRIVERVERSIONKEY "SOFTWARE\QGroundControlUAVDrivers"
  !define QGCCURRENTDRIVERVERSION 2

  ; If the drivers are already installed the key "HKCU/SOFTWARE\MichaelOborne\driver\installed" will be present and set to 1
  SetRegView 64
  !define DRIVERKEY "SOFTWARE\MichaelOborne\driver"
  ReadRegDWORD $0 HKCU "${DRIVERKEY}" "installed"
  IntCmp $0 1 driversInstalled driversNotInstalled driversNotInstalled

driversInstalled:
  DetailPrint "UAV Drivers already installed. Checking version..."
  ; Check if the installed drivers are out of date. 
  ; Missing key also indicates out of date driver install.
  ReadRegDWORD $0 HKCU "${QGCDRIVERVERSIONKEY}" "version"
  IntCmp $0 ${QGCCURRENTDRIVERVERSION} done driversOutOfDate done

driversOutOfDate:
  DetailPrint "UAV Drivers out of date."
  goto installDrivers

driversNotInstalled:
  DetailPrint "UAV Drivers not installed."
  ; Delete abandoned possibly out of date version key
  DeleteRegKey HKCU "SOFTWARE\QGroundControlUAVDrivers"

installDrivers:
  DetailPrint "Installing UAV Drivers..."
  ExecWait '"msiexec" /i "driver.msi"'
  ; Set current driver version value
  WriteRegDWORD HKCU "${QGCDRIVERVERSIONKEY}" "version" ${QGCCURRENTDRIVERVERSION}
  goto done

done:
  SetRegView lastused
SectionEnd 

Section "Uninstall"
  SetRegView 64
  ${GetParameters} $R0
  ${GetOptions} $R0 "-LEAVE_DATA=" $R1
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  SetShellVarContext all
  RMDir /r /REBOOTOK $INSTDIR
  RMDir /r /REBOOTOK "$SMPROGRAMS\$StartMenuFolder\"
  SetShellVarContext current
  ${If} $R1 != 1
    RMDir /r /REBOOTOK "$APPDATA\${ORGNAME}\"
  ${Endif}
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\${EXENAME}.exe"
  ; NOTE: We specifically do not delete the driver version key since we need it to persist around uninstalls
SectionEnd

Section "create Start Menu Shortcuts"
  SetRegView 64
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME}.lnk" "$INSTDIR\${EXENAME}.exe" "" "$INSTDIR\${EXENAME}.exe" 0
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME} (GPU Compatibility Mode).lnk" "$INSTDIR\${EXENAME}.exe" "-angle" "$INSTDIR\${EXENAME}.exe" 0
  !insertmacro DemoteShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME} (GPU Compatibility Mode).lnk"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME} (GPU Safe Mode).lnk" "$INSTDIR\${EXENAME}.exe" "-swrast" "$INSTDIR\${EXENAME}.exe" 0
  !insertmacro DemoteShortCut "$SMPROGRAMS\$StartMenuFolder\${APPNAME} (GPU Safe Mode).lnk"
SectionEnd

        project: #! /bin/bash

tmpdir=`mktemp -d`
long_version=`git describe --always --tags`
short_version=`git describe --always --tags | cut -d "-" -f 1,2`

version=${long_version:1}
dir_version=${short_version:1}
echo $version

debchange -v ${version} --distribution trusty --package qgroundcontrol --create --empty

rsync -a --delete --exclude={build-release,release,Makefile\*} . ${tmpdir}/qgroundcontrol-${dir_version}/

cd ${tmpdir}
tar pczf qgroundcontrol_${dir_version}.orig.tar.gz qgroundcontrol-${dir_version}

cd ${tmpdir}/qgroundcontrol-${dir_version}/
debuild -S -uc -us

# build locally
debuild --prepend-path=/usr/lib/ccache -uc -us -sa -B -i -I -j4

# upload to launchpad
#dput -f ppa:qgroundcontrol/ppa ${tmpdir}/qgroundcontrol_${version}_source.changes
        team: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511181141 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $table = $schema->createTable('user');
        $table->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $table->addColumn('name', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('salt', 'string', [
            'length' => 40,
            'notnull' => false,
            'fixed' => true,
        ]);
        $table->addColumn('hash', 'string', [
            'length' => 40,
            'notnull' => false,
            'fixed' => true,
        ]);
        $table->addColumn('email', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('login', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('accessToken', 'string');

        $table->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('user');
    }
}
        checkmarx_url: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511202416 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $userTable = $schema->getTable('user');

        $testTable = $schema->createTable('test');
        $testTable->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $testTable->addColumn('user_id', 'integer');
        $testTable->addForeignKeyConstraint($userTable, ['user_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $testTable->addColumn('title', 'string');
        $testTable->addColumn('description', 'text', [
            'notnull' => false,
        ]);
        $testTable->addColumn('time_limit', 'integer', [
            'default' => 0,
        ]);
        $testTable->addColumn('added', 'datetime');

        $testTable->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('test');
    }
}
        checkmarx_username: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511212105 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $test = $schema->getTable('test');

        $tag = $schema->createTable('tag');
        $tag->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $tag->addColumn('name', 'string');
        $tag->addUniqueIndex(['name']);
        $tag->setPrimaryKey(['id']);

        $test_tag = $schema->createTable('test_tag');
        $test_tag->addColumn('test_id', 'integer');
        $test_tag->addColumn('tag_id', 'integer');
        $test_tag->addForeignKeyConstraint($tag, ['tag_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $test_tag->addForeignKeyConstraint($test, ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $test_tag->setPrimaryKey(['test_id', 'tag_id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('test_tag');
        $schema->dropTable('tag');
    }
}
        checkmarx_password: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512025516 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $testTable = $schema->getTable('test');

        $table = $schema->createTable('question');
        $table->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $table->addColumn('description', 'text');
        $table->addColumn('points', 'integer');
        $table->addColumn('sequence_number', 'integer');
        $table->addColumn('test_id', 'integer');
        $table->addForeignKeyConstraint($testTable, ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $table->addColumn('answer_text', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('answer_decimal', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('precision', 'decimal', [
            'notnull' => false,
        ]);
        $table->addColumn('type', 'string');

        $table->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('question');
    }
}
        checkmarx_client_secret: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512133552 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $question = $schema->getTable('question');

        $variant = $schema->createTable('variant');
        $variant->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $variant->addColumn('value', 'string');
        $variant->addColumn('is_right', 'string');
        $variant->addColumn('question_id', 'integer');
        $variant->addForeignKeyConstraint($question, ['question_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);

        $variant->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('variant');
    }
}
        scanners: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512135536 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $user = $schema->getTable('user');

        $attempt = $schema->createTable('attempt');
        $attempt->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $attempt->addColumn('started', 'datetime');
        $attempt->addColumn('user_id', 'integer');
        $attempt->addForeignKeyConstraint($user, ['user_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);

        $attempt->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('attempt');
    }
}
        params: --namespace=${{ <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512145224 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $answer = $schema->createTable('answer');
        $answer->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $answer->addColumn('question_id', 'integer');
        $answer->addColumn('attempt_id', 'integer');
        $answer->addColumn('variant_id', 'integer', [
            'notnull' => false
        ]);
        $answer->addForeignKeyConstraint('question', ['question_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addForeignKeyConstraint('attempt', ['attempt_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addForeignKeyConstraint('variant', ['variant_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addColumn('decimal_answer', 'decimal', [
            'notnull' => false,
        ]);
        $answer->addColumn('text_answer', 'string', [
            'notnull' => false,
        ]);
        $answer->addColumn('type', 'string');

        $answer->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('answer');
    }
} }} --repo-name=${{ <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160513005635 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->addColumn('test_id', 'integer');
        $attempt->addForeignKeyConstraint('test', ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->dropColumn('test_id');
    }
} }} --branch=${{ <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160518161400 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->addColumn('status', 'string', [
            'default' => 'active',
        ]);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->dropColumn('status');
    }
} }} --cx-flow.filter-severity --cx-flow.filter-category --checkmarx.disable-clubbing=true --repo-url=${{ {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %} }}
    # Upload the Report for CodeQL/Security Alerts
    - name: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      У вас нет прав на данное действие.
    {% endblock message %}
  </p>
{% endblock body %}
      uses: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Запрашиваемый ресурс не найден.
    {% endblock message %}
  </p>
{% endblock body %}
      with: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}
        sarif_file: <IfModule mod_authz_core.c>
    Require all denied
</IfModule>
<IfModule !mod_authz_core.c>
    Order deny,allow
    Deny from all
</IfModule>
