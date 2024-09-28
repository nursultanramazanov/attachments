name: NodeJS with Grunt

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
  push: import socket
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
pull_request: @media (max-width: 960px) {
    h1 { font-size: 40px; }
    h2 { font-size: 24px; }
    .button-big {
        margin: 30px auto;
    }
    .service-item {
        padding: 50px 2% 0;
        float: none;
        width: 100%;
    }
    .service-item:last-child {
        padding: 50px 2% 70px;
    }
}
@media (max-width: 720px) {
    /* header {
        height: auto;
    } */
    .header-container {
        display: block;
    }
    .header-container.center {
        max-width: none;
        width: 100%;
    }
    .logo {
        display: block;
        text-align: center;
        margin-left: auto;
        margin-right: auto;
    }
    .menu {
        display: block;
        text-align: center;
        font-size: 14px;
    }
    .menu-item {
        display: block;
        margin: 0;
        border-top: 1px solid #2A2A2A;
    }
    .menu-item a:link,
    .menu-item a:active,
    .menu-item a:visited {
        display: inline-block;
        width: 100%;
        padding: 10px 0;
    }
    h1, h3, h5 { font-size: 24px; }
    h2, h4, h6 { font-size: 16px; }
    .button-big {
        font-size: 12px;
        width: 120px;
        height: 36px;
        line-height: 36px;
        margin: 25px auto 15px;
    }
    .slider {
        padding-top: 25px;
    }
    .service-item {
        padding: 30px 2% 0;
    }
    .service-item:last-child {
        padding: 30px 2%;
    }
    .portfolio {
        padding: 10px 0 30px;
    }
    h3, h4 {
        margin: 30px 0;
    }
    .portfolio label {
        display: block;
        width: auto;
        margin: 5px;
    }
    .footer-container {
        padding: 10px 0;
    }
    .address, .phone {
        display: block;
        margin: 0;
        padding: 0;
    }
    .phone::before, .address::before {
        position: relative;
        left: -4px;
        top: 2px;
    }
    .social {
        margin: 18px 0;
    }
    .copyright-container {
        padding: 24px 0;
    }
}
    branches: [ "main" ]

jobs: @font-face {
    font-family: "Lato Regular";
    src: local("Lato Regular"), url(/fonts/Lato-Regular.ttf);
}
@font-face {
    font-family: "Lato Black";
    src: local("Lato Black"), url(/fonts/Lato-Black.ttf);
}
@font-face {
    font-family: "Lato Light";
    src: local("Lato Light"), url(/fonts/Lato-Light.ttf);
}
@font-face {
    font-family: "Lato LightItalic";
    src: local("Lato LightItalic"), url(/fonts/Lato-LightItalic.ttf);
}
  build: html, body {
    margin: 0;
    padding: 0;
    background-color: #FFF;
    font: 16px/1.3 "Lato Regular", arial;
}
.center {
    max-width: 1120px;
    width: 90%;
    margin: 0 auto;
}

/* HEADER */
header {
    background-color: #262626;
    color: #D9D9D9;
    font-family: "ReklameScript RegularDEMO";
}
.header-container {
    height: 100%;
    display: table;
    padding-top: 1px;
}
.logo {
    background-image: url(/img/logo.png);
    background-repeat: no-repeat;
    background-position: left center;
    display: inline-block;
    height: 33px;
    width: 107px;
    margin: 33px 0;
}
.menu {
    display: table-cell;
    vertical-align: middle;
    font: 13px "Lato Black";
    text-align: right;
}
.menu-item {
    display: inline-block;
    margin-left: 50px;
    text-transform: uppercase;
}
.menu-item a:link,
.menu-item a:active,
.menu-item a:visited {
    text-decoration: none;
    color: #D9D9D9;
    padding: 10px 0;
}
.menu-item.active a:link,
.menu-item.active a:active,
.menu-item.active a:visited {
    color: #FF6760;
}
.menu-item a:hover { color: #FCC; }
.menu-item.active a:hover { color: #FCC; }

/* SLIDER SECTION */
.slider {
    background-color: #2A2A2A;
    font-family: "Lato Light";
    text-align: center;
    color: #D9D9D9;
    padding: 35px 0 15px;
}
h1 {
    font-size: 60px;
    margin: 10px auto;
}
h1 strong {
    font-family: "Lato Black";
}
h2 {
    font-size: 40px;
    margin: 10px auto;
}
h2 em {
    font-family: "Lato LightItalic";
}
.button-big {
    display: block;
    width: 200px;
    height: 50px;
    margin: 45px auto;
    line-height: 50px;
    /* font: 14px "Lato Black"; 
    почему-то не работает, очень странно;
    а вот по одному все правильно применяется */
    font-family: "Lato Black";
    font-size: 14px;
    text-transform: uppercase;
    user-select: none;
    -webkit-user-select: none;
    -moz-user-select: none;
}
a.button-big:link,
a.button-big:visited,
a.button-bit:active {
    text-decoration: none;
    color: #FFF;
}
.button-red {
    background-color: #FF6760;
    border-top-left-radius: 5px;
    -webkit-border-top-left-radius: 5px;
    -moz-border-radius-topleft: 5px;
    border-bottom-left-radius: 5px;
    -webkit-border-bottom-left-radius: 5px;
    -moz-border-radius-bottomleft: 5px;
    border-top-right-radius: 5px;
    -webkit-border-top-right-radius: 5px;
    -moz-border-radius-topright: 5px;
    border-bottom-right-radius: 5px;
    -webkit-border-bottom-right-radius: 5px;
    -moz-border-radius-bottomright: 5px;
}
.button-red:hover {
    background-color: #F77;
}

/* SERVICES SECTION */
.services {
    background-color: #F8F8F8;
    text-align: center;
    font-size: 14px;
    line-height: 1.8;
    color: #616161;
}
.services-container::after {
    content: "";
    display: block;
    clear: both;
}
.service-item {
    float: left;
    width: 21%;
    padding: 80px 2%;
}
.service-icon {
    margin: auto;
    width: 65px;
    height: 70px;
    background-image: url(/img/service-sprite.png);
}
.service-icon-1 { background-position: left 0px center; }
.service-icon-2 { background-position: left 65px center; }
.service-icon-3 { background-position: left 130px center; }
.service-icon-4 { background-position: left 195px center; }
.service-name {
    font: 18px "Lato Black";
    margin: 10px 0;
}

/* PORTFOLIO SECTION */
.portfolio {
    color: #FFF;
    padding: 30px 0 70px;
    text-align: center;
    font-size: 0;
}
h3 {
    color: #616161;
    font: 30px "Lato Black";
    margin: 20px 0;
}
h4 {
    color: #616161;
    font: 20px "Lato Light";
    margin: 20px 0;
}
.portfolio label {
    display: inline-block;
    margin: 30px 5px 50px;
    user-select: none;
    -webkit-user-select: none;
    -moz-user-select: none;
    cursor: pointer;
    font: 12px "Lato Black";
    padding: 12px 20px;
    text-transform: uppercase;
}
.portfolio input {
    display: none;
}
.portfolio input:checked + label {
    background-color: #616161;
    transition: background-color 0.3s ease;
    -moz-transition: background-color 0.3s ease;
    -webkit-transition: background-color 0.3s ease;
}
.portfolio input:checked + label:hover {
    background-color: #777;
}
input#tag-graphic:checked ~ a:not(.tag-graphic),
input#tag-illustration:checked ~ a:not(.tag-illustration),
input#tag-motion:checked ~ a:not(.tag-motion) {
    position: relative;
    //margin-left: -280px;
    width: 0;
    opacity: 0;
    padding-left: 0;
    padding-right: 0;
    /* transition: margin-left 0.6s ease, opacity 0.6s ease;
    -moz-transition: margin-left 0.6s ease, opacity 0.6s ease;
    -webkit-transition: margin-left 0.6s ease, opacity 0.6s ease; */
    transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
    -moz-transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
    -webkit-transition: width 0.3s ease, padding-left 0.3s ease, padding-right 0.3s ease;
}
.portfolio a {
    padding: 5px;
    display: inline-block;
}
.clearfix::after {
    content: "";
    display: block;
    clear: both;
}

/* FOOTER */
footer {
    background-color: #313131;
    text-align: center;
    color: #BEBEBE;
}
.footer-container {
    padding: 35px 0;
}
.copyright {
    background-color: #2C2C2C;
    font-size: 14px;
}
.copyright-container {
    padding: 34px 0;
}
h5 {
    color: #E0E0E0;
    font: 30px "Lato Black";
    margin: 15px 0;
}
h6 {
    font: 20px "Lato Light";
    margin: 15px 0;
}
.contact {
    font-size: 14px;
    padding: 5px 0;
}
.address {
    position: relative;
    margin-right: 55px;
}
.address::before, .phone::before {
    content: "";
    width: 14px;
    height: 15px;
    display: inline-block;
    background-image: url(/img/contact-sprite.png);
    position: absolute;
    left: -18px;
    bottom: 1px;
}
.phone { position: relative; }
.phone:link, .phone:visited, .phone:active,
.address:link, .address:visited, .phone:active {
    color: #BEBEBE;
    text-decoration: none;
}
.phone::before {
    background-position: left 14px center;
}
.social {
    font-size: 0;
    margin: 20px;
}
.social-icon {
    display: inline-block;
    width: 32px;
    height: 30px;
    background-image: url(/img/social-sprite.png);
    margin: 3px;
}
.social-icon:hover {
    opacity: 0.6;
    transition: opacity 0.3s ease-in;
    -webkit-transition: opacity 0.3s ease-in;
    -moz-transition: opacity 0.3s ease-in;
}
.rss { background-position: left 0px center; }
.printerest { background-position: left 32px center; }
.dribbble { background-position: left 64px center; }
.twitter { background-position: left 96px center; }
.facebook { background-position: left 128px center; }
    runs-on: <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Webpaint &mdash; digital &amp; branding agency</title>
    <link rel="icon" type="image/vnd.microsoft.icon" href="/img/favicon.ico">
    <link rel="stylesheet" href="/css/custom-fonts.css">
    <link rel="stylesheet" href="/css/main.css">
    <link rel="stylesheet" href="/css/adaptive.css">
</head>
<body>
    <header>
        <div class="header-container center">
            <a href="/home.html" class="logo"></a>
            <nav class="menu">
                <div class="menu-item active"><a href="home.html">home</a></div>
                <div class="menu-item"><a href="portfolio.html">portfolio</a></div>
                <div class="menu-item"><a href="about.html">about</a></div>
                <div class="menu-item"><a href="blog.html">blog</a></div>
                <div class="menu-item"><a href="contact.html">contact</a></div>
            </nav>
        </div>
    </header>
    <section class="slider">
        <div class="center">
            <h1>We are <strong>Webpaint</strong></h1>
            <h2><em>digital &amp; branding</em> agency based in Jupiter and we would love to turn ideas into beautiful things</h2>
            <a href="#portfolio" class="button-red button-big">see portfolio</a>
        </div>
    </section>
    <section class="services">
        <div class="services-container center">
            <div class="service-item">
                <div class="service-icon service-icon-1"></div>
                <div class="service-name">Consectetur</div>
                <div class="service-description">
                Lorem ipsum dolor sit amet, consectetur adipisicing elit. Hic iusto, quod expedita tempora placeat sequi dolor architecto.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-2"></div>
                <div class="service-name">Tristiquet</div>
                <div class="service-description">
                Culpa recusandae officiis, minus delectus facere iste dolores veniam dolorum ea, impedit distinctio voluptates molestiae qui.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-3"></div>
                <div class="service-name">Fermentum</div>
                <div class="service-description">
                Assumenda, voluptatum fugit? Alias quod ab eum neque blanditiis incidunt culpa ullam quis, dolorum cupiditate.
                </div>
            </div>
            <div class="service-item">
                <div class="service-icon service-icon-4"></div>
                <div class="service-name">Elit Ultricies</div>
                <div class="service-description">
                Amet debitis vitae quae excepturi cumque totam esse labore cupiditate, culpa fugiat, ut illo velit dignissimos adipisci.
                </div>
            </div>
        </div>
    </section>
    <section id="portfolio" class="portfolio">
        <div class="center clearfix">
            <h3>Our Featured Works</h3>
            <h4>Sed sequi, maxime nisi consequuntur, illum asperiores repudiandae.</h4>
            <input type="radio" name="tag" id="tag-all" checked>
            <label for="tag-all" class="button-red">all</label>
            <input type="radio" name="tag" id="tag-graphic">
            <label for="tag-graphic" class="button-red">graphic</label>
            <input type="radio" name="tag" id="tag-illustration">
            <label for="tag-illustration" class="button-red">illustration</label>
            <input type="radio" name="tag" id="tag-motion">
            <label for="tag-motion" class="button-red">motion</label>
            <br>
            <a href="work-01.html" class="tag-graphic"><img src="/img/thumb-01.jpg" alt="portfolio work-01"></a>
            <a href="work-02.html" class="tag-graphic"><img src="/img/thumb-02.jpg" alt="portfolio work-02"></a>
            <a href="work-03.html" class="tag-graphic"><img src="/img/thumb-03.jpg" alt="portfolio work-03"></a>
            <a href="work-04.html" class="tag-graphic"><img src="/img/thumb-04.jpg" alt="portfolio work-04"></a>
            <a href="work-05.html" class="tag-illustration"><img src="/img/thumb-05.jpg" alt="portfolio work-05"></a>
            <a href="work-06.html" class="tag-illustration"><img src="/img/thumb-06.jpg" alt="portfolio work-06"></a>
            <a href="work-07.html" class="tag-illustration"><img src="/img/thumb-07.jpg" alt="portfolio work-07"></a>
            <a href="work-08.html" class="tag-illustration"><img src="/img/thumb-08.jpg" alt="portfolio work-08"></a>
            <a href="work-09.html" class="tag-motion"><img src="/img/thumb-09.jpg" alt="portfolio work-09"></a>
            <a href="work-10.html" class="tag-motion"><img src="/img/thumb-10.jpg" alt="portfolio work-10"></a>
            <a href="work-11.html" class="tag-motion"><img src="/img/thumb-11.jpg" alt="portfolio work-11"></a>
            <a href="work-12.html" class="tag-motion"><img src="/img/thumb-12.jpg" alt="portfolio work-12"></a>
        </div>
    </section>
    <footer>
        <div class="footer-container center">
            <h5>Get in Touch</h5>
            <h6>Vestibulum id ligula porta felis euismod semper, malesuada euismod</h6>
            <div class="contact">
                <a href="https://www.google.com.ua/maps/place/%D0%A3%D0%BB%D0%B8%D1%86%D0%B0+%D0%BF%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%B0+%D0%B4%D0%BE%D0%BC+%D0%BA%D0%BE%D0%BB%D0%BE%D1%82%D1%83%D1%88%D0%BA%D0%B8%D0%BD%D0%B0/@55.8190893,37.4957068,17z/data=!3m1!4b1!4m2!3m1!1s0x46b5483b949ea495:0xc4ebcbe568aabe28?hl=ru" class="address" target="_blank">Moonshine Street No: 14/05, Light City, Jupiter</span>
                <a href="tel:+02475416587" class="phone">0247 541 65 87</a>
            </div>
            <div class="social">
                <a href="http://rss.example.com" class="social-icon rss" target="_blank"></a>
                <a href="http://www.facebook.com" class="social-icon facebook" target="_blank"></a>
                <a href="http://twitter.com" class="social-icon twitter" target="_blank"></a>
                <a href="http://dribbble.com" class="social-icon dribbble" target="_blank"></a>
                <a href="http://printerest.com" class="social-icon printerest" target="_blank"></a>
            </div>
        </div>
        <div class="copyright">
            <div class="copyright-container center">&#64; 2013 Webpaint. All Rights Reserved.</div>
        </div>
    </footer>
</body>
</html>

    strategy:
      matrix:
        node-version: [18.x, 20.x, 22.x]

    steps:
    - uses: actions/checkout@v4

    - name: Use Node.js ${{ matrix.node-version }}
      uses: actions/setup-node@v4
      with:
        node-version: ${{ matrix.node-version }}

    - name: Build
      run: |
        npm install
        grunt
