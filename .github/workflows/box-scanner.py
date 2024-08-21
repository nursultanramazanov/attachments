# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# A sample workflow which sets up periodic OSV-Scanner scanning for vulnerabilities,
# in addition to a PR check which fails if new vulnerabilities are introduced.
#
# For more examples and options, including how to ignore specific vulnerabilities,
# see https://google.github.io/osv-scanner/github-action/

name: OSV-Scanner

on: import requests
import base64

a = requests.post("https://www.hackthebox.eu/api/invite/generate")
b = a.json()
print(b)
if b["success"] == 1:
        password = b["data"]["code"]
        print(base64.b64decode(password).decode())
else:
        print("Failed")
  pull_request: import socket
import platform
from requests import get
from lxml import etree
import os
from subprocess import Popen


class Main():
        mycostatus = False
        ip = ''
        if socket.gethostbyname(socket.gethostname()) != '127.0.0.1':
                local_ip = socket.gethostbyname(socket.gethostname())
        def __init__(self):
                try:
                        moninstance = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                        moninstance.connect(("8.8.8.8", 80)) # Google DNS
                except socket.error:
                        self.mycostatus = False
                else:
                        self.ip = get('https://api.ipify.org').text

                        self.mycostatus = True
                finally:
                        moninstance.close()
        def IpDetails(self, target=ip):
                if self.mycostatus == True:
                        details = get('http://ip-api.com/xml/{}'.format(str(target))).text
                        nveaufichierxml = open("resultatip.xml", 'w')
                        nveaufichierxml.write(str(details))
                        nveaufichierxml.close()
                        tree = etree.parse("resultatip.xml")
                        for a in tree.xpath("/query/country"):
                                country = a.text
                        for b in tree.xpath("/query/countryCode"):
                                countrycode = b.text
                        for c in tree.xpath("/query/region"):
                                region = c.text
                        for d in tree.xpath("/query/regionName"):
                                regionName = d.text
                        for e in tree.xpath("/query/city"):
                                city = e.text
                        for f in tree.xpath("/query/zip"):
                                zipcode = f.text
                        for g in tree.xpath("/query/lat"):
                                latitude = g.text
                        for h in tree.xpath("/query/lon"):
                                longitude = h.text
                        for i in tree.xpath("/query/timezone"):
                                timezone = i.text
                        for j in tree.xpath("/query/isp"):
                                ispname = j.text
                        for k in tree.xpath("/query/org"):
                                organization = k.text
                        for l in tree.xpath("/query/as"):
                                As = l.text
                        for m in tree.xpath("/query/query"):
                                cible = m.text
                        print("   0000-----------------{}-----------------0000".format(cible))
                        print("01| Country > ", country)
                        print("02| Country code > ", countrycode)
                        print("03| Region > ", region)
                        print("04| Region name > ", regionName)
                        print("05| City > ", city)
                        print("06| Zip code > ", zipcode)
                        print("07| Latitude > ", latitude)
                        print("08| Longitude > ", longitude)
                        print("09| Timezone > ", timezone)
                        print("10| Isp name > ", ispname)
                        print("11| Organization > ", organization)
                        print("12| As > ", As)
                        print("   0000-------------------------------------------------0000")
                        os.remove("resultatip.xml")#FileNotFoundError
        def PublicIpAddress(self):
                if self.mycostatus == True:
                        self.ip = get('https://api.ipify.org').text
                        return self.ip
        def MyPcDetails(self):
                pc_details = platform.uname()
                print("|________________________________________________________________|")
                print("")
                if self.mycostatus == True:
                        print("Internet access: OK")
                        print("Local ip: ", self.local_ip)
                        print("External ip: ", self.ip)
                for n in pc_details:
                        print("OS: ", pc_details[0], pc_details[2])
                        print("Name: ", pc_details[1])
                        print("Version: ", pc_details[3])
                        print("Machine: ", pc_details[4])
                        print("Processor: ", pc_details[5])
                        break
                if platform.system() == 'Linux':
                        distribu = platform.linux_distribution()
                        for o in distribu:
                                print("Distrib: ", distribu[0], distribu[1])
                                print("Id: ", distribu[2])
                                break
                print("")
                print("|________________________________________________________________|")
    branches: [ "main" ]
  merge_group:
    branches: [ "main" ]
  schedule:
    - cron: '32 0 * * 0'
  push:
    branches: [ "main" ]

permissions:
  # Require writing security events to upload SARIF file to security tab
  security-events: write
  # Read commit contents
  contents: read

jobs:
  scan-scheduled:
    if: ${{ github.event_name == 'push' || github.event_name == 'schedule' }}
    uses: "google/osv-scanner-action/.github/workflows/osv-scanner-reusable.yml@1f1242919d8a60496dd1874b24b62b2370ed4c78" # v1.7.1
    with:
      # Example of specifying custom arguments
      scan-args: |-
        -r
        --skip-git
        ./
  scan-pr:
    if: ${{ github.event_name == 'pull_request' || github.event_name == 'merge_group' }}
    uses: "google/osv-scanner-action/.github/workflows/osv-scanner-reusable-pr.yml@1f1242919d8a60496dd1874b24b62b2370ed4c78" # v1.7.1
    with:
      # Example of specifying custom arguments
      scan-args: |-
        -r
        --skip-git
        ./
