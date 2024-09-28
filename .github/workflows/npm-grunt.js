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
pull_request: {
 "cells": [
  {
   "cell_type": "markdown",
   "metadata": {
    "collapsed": false
   },
   "source": [
    "# Project Name:\n",
    "# ramazanovnursultan \n",
    "\n",
    "## Topic: Design of a BMW i3 Traction Motor\n",
    "\n",
    "Text 1231\n",
    "\n",
    "This expression $\\sqrt{3x-1}+(1+x)^2$ is an example of a inline equation in a \n",
    "[Markdown-formatted](http://daringfireball.net/projects/markdown/) sentence."
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "# Introduction"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "[Plot Tutorial](http://matplotlib.org/users/pyplot_tutorial.html)"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 13,
   "metadata": {
    "collapsed": false
   },
   "outputs": [],
   "source": [
    "%matplotlib inline\n",
    "import matplotlib.pyplot as plt\n",
    "import numpy as np"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 15,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "data": {
      "image/png": "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\nBTOJ/w9wqAckEMjjbwAAAABJRU5ErkJggg==\n",
      "text/plain": [
       "<matplotlib.figure.Figure at 0x7fca6b984d50>"
      ]
     },
     "metadata": {},
     "output_type": "display_data"
    }
   ],
   "source": [
    "x = np.linspace(0, 3*np.pi, 500)\n",
    "plt.plot(x, np.sin(x**2));"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 24,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "data": {
      "image/png": "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\n",
      "text/plain": [
       "<matplotlib.figure.Figure at 0x7fca69cc4a50>"
      ]
     },
     "metadata": {},
     "output_type": "display_data"
    }
   ],
   "source": [
    "# evenly sampled time at 200ms intervals\n",
    "t = np.arange(0., 5., 0.2);\n",
    "\n",
    "# red dashes, blue squares and green triangles\n",
    "plt.plot(t, t, 'r--', t, t**2, 'bs', t, t**3, 'g^');"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Making Lists\n",
    "*   Candy\n",
    "*   Gum\n",
    "*   Booze"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Equations\n",
    "\n",
    "You can use [LaTeX equations](http://www.personal.ceu.hu/tex/cookbook.html) for mathematical expressions:\n",
    "\n",
    "$$\\int_{a}^{b} f(x)\\, dx \\approx \\frac{1}{2} \\sum_{k=1}^{N} \\left( x_{k} - x_{k-1} \\right) \\left( f(x_{k}) + f(x_{k-1}) \\right)$$"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "# Python Info\n",
    "\n",
    "## [Why Python?](https://scipy-lectures.github.io/intro/index.html)"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "### Python Operations"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 18,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "data": {
      "text/plain": [
       "1.5"
      ]
     },
     "execution_count": 18,
     "metadata": {},
     "output_type": "execute_result"
    }
   ],
   "source": [
    "#Complex Math\n",
    "a = 1.5 + 0.5j\n",
    "a.real"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 20,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "data": {
      "text/plain": [
       "0.5"
      ]
     },
     "execution_count": 20,
     "metadata": {},
     "output_type": "execute_result"
    }
   ],
   "source": [
    "a.imag"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 21,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "data": {
      "text/plain": [
       "(1.5+5.5j)"
      ]
     },
     "execution_count": 21,
     "metadata": {},
     "output_type": "execute_result"
    }
   ],
   "source": [
    "a*(2+3j)"
   ]
  }
 ],
 "metadata": {
  "kernelspec": {
   "display_name": "Python 2",
   "language": "python",
   "name": "python2"
  },
  "language_info": {
   "codemirror_mode": {
    "name": "ipython",
    "version": 2
   },
   "file_extension": ".py",
   "mimetype": "text/x-python",
   "name": "python",
   "nbconvert_exporter": "python",
   "pygments_lexer": "ipython2",
   "version": "2.7.8"
  },
  "name": "564_Sample_Notebook.ipynb"
 },
 "nbformat": 4,
 "nbformat_minor": 0
}
    branches: [ "main" ]
jobs: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}
  build: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      У вас нет прав на данное действие.
    {% endblock message %}
  </p>
{% endblock body %}
    runs-on: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Запрашиваемый ресурс не найден.
    {% endblock message %}
  </p>
{% endblock body %}

    strategy: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}
      matrix: .skipped-buttons {
    padding: 16px 0;
}
h1 {
    margin-bottom: 20px;
}
        node-version: .test-title {
    margin-bottom: 0;
    margin-top: 14px;
}
.tag-name {
    padding-right: 10px;
    text-decoration: underline;
}
.show-all {
    text-decoration: underline;
}
.top-tests {
    margin-bottom: 16px;
}
.test-container {
    position: relative;
}
.test-tags {
    margin-bottom: 12px;
}
.start-test-btn {
    position: absolute;
    top: 0;
    bottom: 0;
    right: 0;
}
.start-test-link {
    margin-top: 22px;
    margin-right: 5px;
}
.create-test-btn {
    margin: 12px 0;
}
.nav-about {
    padding-left: 30px;
}
.list-group-item {
    padding: 0;
}
h1 {
    margin-bottom: 20px;
}

    steps: .test-container {
    margin: 20px 0 30px;
}
.button-wrapper {
    text-align: center;
}
    - uses: .question-description-container {
    padding-bottom: 16px;
}
.text-answer {
    margin-bottom: 18px;
}
hr {
    margin: 15px 0;
}
h1 {
    margin-bottom: 20px;
}
.time-left {
    font-size: smaller;
    margin-top: 8px;
}

    - name: .test-result-section h1 {
    margin-bottom: 20px;
}
      uses: .body-wrapper {
    margin-left: auto;
    margin-right: auto;
}
@media (min-width: 1200px) {
    .body-wrapper {
        width: 75%;
    }
}
@media (min-width: 768px) and (max-width: 1200px) {
    .body-wrapper {
        width: 80%;
    }
}
@media (max-width: 768px) {
    .body-wrapper {
        width: 90%;
    }
}

.sitename {
    font-size: 40px;
    font-weight: bold;
    line-height: 60px;
    padding-left: 16px;
}
.sitename:before {
    content: "";
    position: relative;
    top: 8px;
    left: -8px;
    display: inline-block;
    height: 50px;
    width: 50px;
    background: url(/logo.png);
    background-size: cover;
}

.auth-section {
    line-height: 60px;
}

.header {
    padding-top: 12px;
    padding-bottom: 6px;
    border-bottom: 1px solid #000000;
}

.register-link, .login-link {
    padding-left: 24px;
}
@media (max-width: 992px) {
    .login-link, .register-link {
        float: none!important;
        padding-left: 0;
        padding-right: 24px;
        display: inline-block;
    }
    .auth-section {
        line-height: 36px;
    }
}
      with:
        node-version: ${{ matrix.node-version }}

    - name: Build
      run: |
        npm install
        grunt
