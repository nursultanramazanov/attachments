name: Python Package using Conda

on: [push]

jobs: import logging
import platform
import socket
import subprocess
import sys
from datetime import datetime as date
from logging.handlers import RotatingFileHandler
from os import path, remove, system
from time import sleep

import colorama
import getmac

import passwd
import scanmodule
import utils

colorama.init()
#Setting up log
log = logging.getLogger()
log.setLevel(logging.DEBUG)
formatt = logging.Formatter("|%(asctime)s:  %(levelname)s - %(message)s")
fileh = RotatingFileHandler('fingy.log', 'w+', 1000000, 5)
fileh.setLevel(logging.DEBUG)
fileh.setFormatter(formatt)
log.addHandler(fileh)
stream_handler = logging.StreamHandler()
stream_handler.setLevel(logging.WARN)
log.addHandler(stream_handler)
log.info("NEW EXECUTION")
#Starting...
print(colorama.Fore.BLUE+"""
        ███████╗██╗███╗   ██╗ ██████╗██╗   ██╗
        ██╔════╝██║████╗  ██║██╔════╝╚██╗ ██╔╝
        █████╗  ██║██╔██╗ ██║██║  ███╗╚████╔╝ 
        ██╔══╝  ██║██║╚██╗██║██║   ██║ ╚██╔╝  
        ██║     ██║██║ ╚████║╚██████╔╝  ██║   
        ╚═╝     ╚═╝╚═╝  ╚═══╝ ╚═════╝   ╚═╝    
                {0}
    """.format(colorama.Fore.WHITE+colorama.Style.BRIGHT+"Version:  Not really stable"+colorama.Style.RESET_ALL))
if not utils.Main().mycostatus:
    log.critical("You're not connected to Internet")
    sys.exit()
else:
    utils.Main().MyPcDetails()
if len(sys.argv) >= 2:
    if len(sys.argv) == 3:
        if sys.argv[2] == "--entire" or sys.argv[2] == "-e":
            scanentirenetwork = True
        else:
            scanentirenetwork = False
    else:
        scanentirenetwork = False
    try:
        x = int(sys.argv[1].split("-")[0])
        y = int(sys.argv[1].split("-")[1])
    except:
        log.warning("Usage: 'reload 1-256 (-e: scan the entire network)'")
        x = 1
        y = 2
else:
    scanentirenetwork = False
    x = 1
    y = 26
try:
    remove("__pycache__")
except (PermissionError,FileNotFoundError) as e:
    log.info(e)
try:
    from webbrowser import open_new_tab
    webbrowser = True
except ModuleNotFoundError:
    log.warn("Webbrowser module not found, but anyway ;)")
    webbrowser = False

vendors = {}
def retreive_vendors():
    try:
        a=open("mac_vendors.txt",encoding="utf-8").read()
    except FileNotFoundError:
        return False
    for i in a.split("\n"):
        identifier = i[0:6]
        vendor = i.split(identifier)[1].strip()
        if "#" in vendor:
            if vendor.startswith("W-Ie-"):
                vendor = vendor.split("#")[0].strip()
            elif "-" in vendor:
                vendor = vendor.split("-")[0]
            elif "+" in vendor:
                vendor =vendor.split("+")[0]
            elif "&" in vendor:
                vendor =vendor.split("&")[0]
            else:
                vendor = vendor.split("#")[0].strip()
        if vendor[-1] == "/":
            vendor = vendor[:-1]
        vendors[identifier] = vendor
retreive_vendors()
def SortIp(ips:list):
    last_bits=[]
    three_first_bits = ".".join(ips[0].split(".")[0:-1])+"."
    sorted_ips = []
    for i in ips:
        last_bit=i.split(".")[-1]
        last_bits.append(last_bit)
    sorted_ips.sort(key=int)
    for i in last_bits:
        sorted_ips.append(three_first_bits+i)
    return sorted_ips
log.info(dir())
utils.Addpid()
ipndhstname = {}
ipndports = {}
ups = []
socket.setdefaulttimeout(0.05)
local_ip = passwd.GetLocalIp()
log.info("Hosts discovery processus started")
if scanentirenetwork:
    scan_choice = scanmodule.Pingy().EntireNetwork(x,y)
else:
    sfff = scanmodule.Pingy().improvedscan()
    scan_duration = sfff[1]
    scan_choice = sfff[0]
for fghm in scan_choice:
    ups.append(fghm)
log.info(ups)
if len(ups) <= 2:
    log.warning(scanmodule.Pingy().warning)
    log.info("Typing 'up' is advisable")
for xa in ups:
    try:
        hst,tm,pt = socket.gethostbyaddr(str(xa))
    except (socket.herror, socket.gaierror):
        try:
            hst,tm,pt = socket.gethostbyaddr(str(xa))
        except (socket.herror, socket.gaierror):
            hst = 'Unknown'
    ipndhstname[xa] = hst
if utils.Main().mycostatus == False:
    log.warning("No Internet")

myplatform = platform.system().lower()
if myplatform == 'windows':
    clear = "cls"
else:
    clear = "clear"
print("+..............................+")
print(colorama.Style.BRIGHT+"{0} {1} {2}".format(colorama.Fore.GREEN+"[+]",colorama.Fore.WHITE+str(len(ups)),colorama.Fore.WHITE+"device(s) found"))
print("Finished at",str(date.now())[11:-7])
print("+..............................+")
print(colorama.Style.RESET_ALL)
while True:
    try:
        menu = input("$> ")
    except KeyboardInterrupt:
        print(colorama.Back.LIGHTMAGENTA_EX+"Use exit to quit")
        print(colorama.Style.RESET_ALL)
    except EOFError:
        log.warning("Stop messing around with it")
        continue
    log.info("INPUT-> "+menu)
    if menu == 'help' or menu == 'h':
        print(colorama.Style.RESET_ALL+"|------------HELP------------|")
        print("")

        print(colorama.Fore.WHITE+colorama.Style.BRIGHT+"a- Scan results")
        print(colorama.Style.RESET_ALL+"1- Get ip details")
        print(" - reload [+args]")
        print(" - {}".format(colorama.Fore.WHITE+colorama.Style.BRIGHT+"update"))
        print(colorama.Style.RESET_ALL+" - ls")
        print(" - exit")
        print("|----------------------------|")
        continue
    if menu == "up -h" or menu == "update -h" or menu == "up --help" or menu == "update --help":
        print("Use 'update' or 'up' to refresh the devices found list")
        print("It DOES NOT reload the script unlike 'reload'")
    if menu == "up" or menu == "update":
        if utils.Main().mycostatus == False:
            log.warning("No Internet")
        del ups
        del ipndhstname
        ups = []
        ipndhstname = {}
        ups.clear()
        for fghm in scanmodule.Pingy().improvedscan()[0]:
            ups.append(fghm)
        ups = SortIp(list(set(ups)))
        for xl in ups:
            try:
                hst,tm,pt = socket.gethostbyaddr(str(xl))
            except (socket.herror, socket.gaierror):
                try:
                    hst,tm,pt = socket.gethostbyaddr(str(xl))
                except (socket.herror, socket.gaierror):
                    hst = 'Unknown'
            ipndhstname[xl] = hst
        deb = colorama.Fore.WHITE+"{} device(s) found".format(str(len(ups)))
        print(colorama.Style.BRIGHT+"{0} ".format(colorama.Fore.GREEN+"[+]")+deb)
        log.info(ups)
    if menu == "h(r)" or menu == "help(r)" or menu == "help(reload)" or menu == "reload -h" or menu == "reload --help":
        print("|-----------------------------HELP(RELOAD)-----------------------------|")
        print("Type 'reload' to reload the script")
        print("This command supports a few arguments:")
        print("     r(eload) -h (or --help)")
        print("     reload 1-25 : restart by scanning your network from 192.168.1.1 to 25")
        print("     reload 1-25 --sudo : try to run as root")
        print("     reload (+args) -e : restart by scanning the entire network")
    if menu == "reload" or menu == "r":
        if utils.Main().mycostatus == False:
            log.error("No Internet->Unable to reload")
            continue
        try:
            subprocess.Popen([sys.executable, path.realpath(__file__)])
        except KeyboardInterrupt:
            pass
        except Exception as e:
            log.error(exc_info=1)
            continue

    if "reload " in menu and not "-h" in menu and not "--help" in menu:
        if utils.Main().mycostatus == False:
            log.error("No Internet->Unable to reload")
            continue
        try:
            arggs = menu.split()[1]
        except IndexError:
            print("J'ai pas tout suivi, recommence stp")
            continue
        try:
            argz = menu.split()[2]
        except IndexError:
            continue
        try:
            if menu.split()[2] == "--sudo":
                try:
                    if myplatform != "windows":
                        subprocess.Popen(["sudo",sys.executable, path.realpath(__file__),arggs])
                except:
                    log.critical("Unable to reload the script as admin")
                    continue
            elif "-e" in menu.split() or "--entire" in menu.split():
                subprocess.Popen([sys.executable,path.realpath(__file__),arggs,"-e"])
        except IndexError:
            try:
                subprocess.Popen([sys.executable, path.realpath(__file__),arggs])
            except:
                log.critical("Failed to reload",exc_info=1)
                continue


    if menu == 'exit':

        try:
            puids = open("puids.txt","r")
            for puidss in puids.readlines():
                if myplatform == "windows":
                    taskkill = subprocess.Popen(['taskkill','/F','/PID','{}'.format(int(puidss[:-1]))], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                else:
                    taskkill = subprocess.Popen(['kill','{}'.format(int(puidss[:-1]))], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            puids.close()
        except Exception as e:
            log.error(e)
        remtmp = remove("puids.txt")
        remtmp = open("puids.txt","a").close()
        sys.exit(0)

    if menu == '1':
        try:
            instance_testmyco_module = utils.Main()
            entree_pour_details = input("Ip=> ")
            if scanmodule.IsValidIp(entree_pour_details) == True or scanmodule.IsValidIp(entree_pour_details) == False:
                instance_testmyco_module.IpDetails(entree_pour_details)
            else:
                print("Unable to reach {0}".format(entree_pour_details))
        except Exception as e:
            log.error("Getting details for "+entree_pour_details+" failed",exc_info=1)
            continue

    if menu == 'a':
        if len(ups) == 0:
            print("No host found")
            continue
        ups=SortIp(ups)
        log.info(ups)
        print(colorama.Fore.YELLOW+"+----------------------------------------------------------------------------------------------------------------------------------------------+"+colorama.Style.RESET_ALL)
        port = ['20','21','22','23','25','37','43','53','69','80','111','135','137','138','139','443','445','548','631','989','993','995','990','2049','4444','4443','8080','8081','49152','62078','49674','49664','49665','49666','49666','49861','49862']
        for g in range(len(ups)):
            print(colorama.Style.RESET_ALL+colorama.Fore.GREEN+"(+)"+" {}".format(colorama.Fore.WHITE+ups[g]))
            ports_found = []
            for d in port:
                soc = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                result = soc.connect_ex((str(ups[g]),int(d)))
                soc.close()
                try:
                    utilisationduport = socket.getservbyport(int(d))
                    if result == 0:
                        ports_found.append("{0}:{1}/tcp".format(str(d),utilisationduport))
                except OSError:
                    pass
                ipndports[str(ups[g])] = ports_found
        def get_vendor(mac):
            if mac == None:
                return "Unknown"
            id = [i.upper() for i in "".join(mac.split(":")[:3]).split()][0]
            if id in vendors.keys():
                vendor = vendors[id]
            else:
                vendor = "Unknown"
            return vendor
        print(colorama.Fore.YELLOW+"|-------IP------|--------Hostname--------|--------------Ports---------------|---------MAC---------|--------------VENDOR------------")
        for pres in ups:
            print("|",colorama.Style.BRIGHT+"{}".format(colorama.Fore.YELLOW+pres))
            print(colorama.Style.NORMAL+"|                ",colorama.Fore.YELLOW+ipndhstname[pres])
            for mmlkj in ipndports[pres]:
                print("|                                          ",mmlkj)
            mac = getmac.get_mac_address(ip=pres)
            print("|                                                                            ",mac)
            print("|                                                                                                   ",get_vendor(mac))
        print("+----------------------------------------------------------------------------------------------------------------------------------------------+"+colorama.Style.RESET_ALL)
        continue
  build-linux: # encoding: utf-8
import subprocess
from socket import socket,AF_INET, SOCK_STREAM, setdefaulttimeout, SOCK_DGRAM
from platform import system
from getpass import getuser
from sys import exit
import re

def GetLocalIp():
        # Credits: Openclassroom?
    s = socket(AF_INET, SOCK_DGRAM)
    try:
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except:
        IP = False
    finally:
        s.close()
    return IP
def GetCurrentSSID():
        if system() == "Windows":
                try:
                        ssid=subprocess.check_output(['netsh', 'wlan', 'show', 'networks']).replace("\r\n".encode(),"".encode()).split("SSID".encode())[1].split(": ".encode())[1].split("T".encode())[0].strip().decode()
                        return ssid
                except Exception as e:
                        print(e)
                        print("An error occured while getting your ssid, check your connection and try again")
                        return False
        elif system() == "Linux":
                 if getuser() == "root":
                         pass
#cat /etc/NetworkManager/system-connections/{ssid} | grep psk=
def GetPass():
        ssid = GetCurrentSSID()
        if ssid != None and ssid != False:
                if system() == "Windows":
                        try:
                                passwd = subprocess.check_output(['netsh', 'wlan', 'show', 'profile', f'"{ssid}"', 'key=clear']).decode('utf-8', errors="ignore").split('\n')
                                passwd = [b.split(":")[1][1:-1] for b in passwd if "Key Content" in b or "Contenu de la cl" in b]
                        except subprocess.CalledProcessError:
                                return False
                        return passwd[0]
                elif system() == "Linux":
                        passwd = subprocess.check_output(['cat', '/etc/NetworkManager/system-connections/{0}'.format(ssid)]).decode('utf-8').split('\n')
        else:
                return False
def GetFirstDigits():
        passwd = GetPass()
        if passwd != None and passwd != False:
                firstdigits = passwd[:8]
                return firstdigits
        else:
                return False
def Getdefaultgateaway():
        gateway = str(subprocess.check_output(['ipconfig'])).split("\\r\\n")
        for x in gateway:
                search = re.search(r"fau",str(x))
                if search:
                        gateway = gateway[search.start():search.end()][0].split(" ")[-1]
                        break
        return gateway
def GetDefaultGateway():
        try:
                gateway = str(subprocess.check_output(['ipconfig'])).encode('utf-8').decode('utf-8').split('\n')
                for x in gateway:
                        if "Passerelle par d" in x:
                                ipv6dfois = x.split("Passerelle par d")[1]
                                ipv6dfois  = ipv6dfois.split("\\r\\n")[0]
                                ipv6dfois = ipv6dfois.split(": ")[1]
                                gateway = x.split()[-1][:-5]
                                break
                if type(gateway) == str and gateway != '':
                        return gateway
                elif type(ipv6dfois) == str and ipv6dfois != '':
                        return ipv6dfois
                else:
                        return False#Disconnected or Error(language,...)
        except:
                return False
    runs-on: import socket
import subprocess
import threading
from ipaddress import IPv4Address, IPv6Address
from time import sleep
import datetime

import passwd

main_thread = threading.currentThread()
streds=[]
class Pingy():
    ups = []
    def __init__(self):
        self.warning = "The number of threads might not be supported by some devices so the number of devices found can be LOWER"
        self.iplocal = passwd.GetLocalIp()
        self.racine = ".".join(self.iplocal.split('.')[0:-1]) + '.'
        self.deuxpremiers = ".".join(self.iplocal.split('.')[0:2]) + '.'
        self.troisiemeoctet = int(self.iplocal.split(".")[2])
    def Pingg(self, target:str):
        _ping_ = subprocess.Popen(['ping','-4','-n','1','{}'.format(target)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        sortie, erreurs = _ping_.communicate()
        if "TTL" in str(sortie) or "ttl" in str(sortie):
            return True
        else:
            return False
    def Ping(self, target:str):
        _ping_ = subprocess.Popen(['ping','-n','1','{}'.format(target)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        sortie, erreur = _ping_.communicate()
        if "TTL" in str(sortie) or "ttl" in str(sortie):
            self.ups.append(target)
    def ScanAll(self, x=1,y=25,sauf=[]):
        if x > y:
            return False
        for number in range(x,y):
            if number in sauf:
                continue
            else:
                r = self.racine+str(number)
                stred = threading.Thread(target=self.Ping, args=[r,])
                stred.start()
                streds.append(stred)
                if len(threading.enumerate()) > 200:
                    for i in threading.enumerate():
                        if main_thread is i or streds[-1] is i or streds[-2] is i:
                            continue
                        i.join()
        self.ups = list(dict.fromkeys(self.ups))
        return self.ups
    def EntireNetwork(self, x_third_oct=1, y_third_oct=2558, x=1,y=26):
        streds = []
        if y_third_oct == 2558:
            y_third_oct = self.troisiemeoctet
        for lsd in range(x_third_oct,y_third_oct):
            for m in range(x,y):
                rimk = self.deuxpremiers+str(lsd)+"."+str(m)
                stred = threading.Thread(target=self.Ping, args=[rimk, ])
                stred.start()
                streds.append(stred)
                if len(threading.enumerate()) > 200:
                    for i in threading.enumerate():
                        if main_thread is i or streds[-1] is i or streds[-2] is i:
                            continue
                        i.join()
            print(self.deuxpremiers+str(lsd)+".1/{}".format(str(y-1)))
        self.ups = list(dict.fromkeys(self.ups))
        return self.ups
    def improvedscan(self):
        a=[]
        debug = {"StartingTime":datetime.datetime.now()}
        for i in range(2):
            list(set(a))
            debug["Round "+str(i)] = {"(var)a":len(a)}
            if i != 0:
                for x in self.ScanAll(x=1,y=256,sauf=[int(x.split(".")[3]) for x in a]):
                    a.append(x)
                debug["Round "+str(i)]["FinishedTime"] = datetime.datetime.now()
            else:
                for x in self.ScanAll(x=1,y=256):
                    a.append(x)
                debug["Round "+str(i)]["FinishedTime"] = datetime.datetime.now()
        debug["EndingTime"] = datetime.datetime.now()
        debug["EndminusStart"] = debug["EndingTime"] - debug["StartingTime"]
        return (list(set(a)),debug)

def IsHttpPortOpen(target:str):
    try:
        res = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        socket.setdefaulttimeout(1)
        re = res.connect_ex((target,80))
    except Exception as e:
        print(e)
        return False
    finally:
        res.close()
    if re == 0:
        return True
    else:
        return False
def IsValidIp(ip):
    #return False for IPv4, True for v6, None if not valid
    ip = str(ip)
    try:
        IPv6Address(ip)
    except:
        try:
            IPv4Address(ip)
        except:
            return None
        else:
            return False
    else:
        return True
common_used_port_list = ['20','21','22','23','25','37','43','53','69','80','111','135','137','138','139','443','445','548','631','989','993','995','990','2049','4444','4443','49152','62078']
class Scan():
    hote = "_"
    open_tcp_ports = []
    closed_tcp_ports = []
    def __init__(self, hote):
        self.hote = str(hote)
        if IsValidIp(self.hote) == None:
            self.hote = '127.0.0.1'
    def TcpOpen(self, xx=False, yy=False,usage_common_used_port_list=False):
        self.open_tcp_ports = []
        self.closed_tcp_ports = []
        if self.hote != False:
            print("Target: ",self.hote)
            print("Tcp scan running...")
            if usage_common_used_port_list == True:
                for ix in common_used_port_list:
                    ix = int(ix)
                    sb = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    socket.setdefaulttimeout(0.0000005)
                    ab  = sb.connect_ex((self.hote, ix))
                    sleep(0.0000005)
                    sb.close()
                    if int(ab) == 0:
                        self.open_tcp_ports.append(ix)
                self.closed_tcp_ports.append(len(common_used_port_list) - len(self.open_tcp_ports))
                return self.open_tcp_ports, self.closed_tcp_ports
            if usage_common_used_port_list == False and xx == False or yy == False:
                #With no paramaters
                xx = 80
                yy = 81
                for ix in range(xx, yy):
                    sb = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    socket.setdefaulttimeout(0.0005)
                    ab  = sb.connect_ex((self.hote, ix))
                    sleep(0.0000001)
                    sb.close()
                    if int(ab) == 0:
                        self.open_tcp_ports.append(ix)
                self.closed_tcp_ports.append((max(yy, xx)-min(yy,xx))-len(self.open_tcp_ports))
                return self.open_tcp_ports, self.closed_tcp_ports
            if xx != False and yy != False:
                xx = int(xx)
                yy = int(yy)
                usage_common_used_port_list = False
                for ix in range(xx, yy):
                    sb = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    socket.setdefaulttimeout(0.0000005)
                    ab = sb.connect_ex((self.hote, ix))
                    sleep(0.0000005)
                    sb.close()
                    if int(ab) == 0:
                        self.open_tcp_ports.append(ix)
                self.closed_tcp_ports.append((max(yy, xx)-min(yy,xx))-len(self.open_tcp_ports))
                return self.open_tcp_ports, self.closed_tcp_ports
    strategy: import os
import platform
import socket
from getpass import getuser
from subprocess import check_output

from lxml import etree
from requests import get


def Addpid(file="puids.txt"):
        file = str(file)
        if not file.endswith(".txt"):
                return False
        try:
                pid        = os.getpid()
                puids = open(file,"a")
                puids.write(str(pid)+"\n")
                puids.close()
        except PermissionError:
                print("[Error] Access denied for",file)
                if platform.system() == "Linux":
                        print("Reload with '--sudo'")
                else:
                        print("Run it as admin next time")
        except Exception as e:
                print(e)
                print("|Session unsaved|")
class Main():
        mycostatus = False
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        ip = ""
        try:
                s.connect(('10.255.255.255', 1))
                local_ip = s.getsockname()[0]
        except:
                local_ip = ''
        finally:
                s.close()
        def __init__(self):
                ip = ""
                try:
                        moninstance = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                        moninstance.connect(("8.8.8.8", 80))
                except:
                        self.mycostatus = False
                else:
                        self.mycostatus = True
                finally:
                        moninstance.close()
        def IpDetails(self, target=ip, export=""):
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
                        print("   ---------------------{}---------------------".format(cible))
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
                        print("   ---------------------------------------------------------")
                        os.remove("resultatip.xml")#FileNotFoundError
                        if export == "json":
                                pass
                        elif export == "txt":
                                qsdf = open("ip_details.txt","w")
                                qsdf.write(f"""
   ---------------------{cible}---------------------
01| Country > {country}
02| Country code > {countrycode}
03| Region > {region}
04| Region name > {regionName}
05| City > {city}
06| Zip code > {zipcode}
07| Latitude > {latitude}
08| Longitude > {longitude}
09| Timezone > {timezone}
10| Isp name > {ispname}
11| Organization > {organization}
12| As > {As}
   ---------------------------------------------------------
                                """)
                                qsdf.close()
        def PublicIpAddress(self):
                if self.mycostatus == True:
                        self.ip = get('https://api.ipify.org').text
                        return self.ip
        def MyPcDetails(self):
                self.ip = get('https://api.ipify.org').text
                pc_details = platform.uname()
                print("|________________________________________________________________|")
                print("")
                print("User:",getuser())
                if self.mycostatus == True:
                        print("Internet access: OK")
                        if self.local_ip != False:
                                print("Local ip: ", self.local_ip)
                        print("External ip: ", self.ip)
                if pc_details[1] != "":
                        print("Name: ", pc_details[1])
                if pc_details[0] != "" and pc_details[2] != "":
                        print("OS: ", pc_details[0], pc_details[2])
                if pc_details[3] != "":
                        print("Version: ", pc_details[3])
                if pc_details[4] !="":
                        print("Machine: ", pc_details[4])
                if pc_details[5] != '':
                        print("Processor: ", pc_details[5])
                if platform.system() == 'Linux':
                        distribu = platform.linux_distribution()
                        if distribu[0] != "" and distribu[1] != "":
                                print("Distrib: ", distribu[0], distribu[1])
                        if distribu[2] != "":
                                print("Id: ", distribu[2])
                print("")
                print("|________________________________________________________________|")
      max-parallel: 5

    steps:
    - uses: actions/checkout@v4
    - name: Set up Python 3.10
      uses: actions/setup-python@v3
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
