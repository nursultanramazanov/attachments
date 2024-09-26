# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
# Frogbot Scan Pull Request does the following:
# Automatically scans new pull requests for security vulnerabilities.
# Uses JFrog Xray to scan the project.
# Read more about Frogbot here - https://docs.jfrog-applications.jfrog.io/jfrog-applications/frogbot

# Some projects require creating a frogbot-config.yml file. Read more about it here - https://docs.jfrog-applications.jfrog.io/jfrog-applications/frogbot/setup-frogbot/frogbot-configuration

name: "Frogbot Scan Pull Request"
on: @media (max-width: 960px) {
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
  pull_request_target: @font-face {
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
    types: [ html, body {
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
.facebook { background-position: left 128px center; } ]
permissions: {
 "cells": [
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "In this project, a XRAY transformer with the given specifications below will be designed:\n",
    "\n",
    "- Single Phase, High Frequency High Voltage Transformer\n",
    "- Primary Winding Voltage ± 417 V (peak to peak 834 V for pulsing)\n",
    "- Secondary Winding Voltage ± 12.5 kV (peak to peak 25 kV for pulsing)\n",
    "- Rated Power 30 kW (for maximum 100 millisecond)\n",
    "- Switching Frequency Minimum 100 kHz\n",
    "- Ambient Temperature 0-40 °C\n"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Step 1: Core Material Selection"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 68,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "data": {
      "image/png": "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\n",
      "text/plain": [
       "<IPython.core.display.Image object>"
      ]
     },
     "execution_count": 68,
     "metadata": {
      "image/png": {
       "height": 450,
       "width": 450
      }
     },
     "output_type": "execute_result"
    }
   ],
   "source": [
    "from IPython.display import Image\n",
    "Image(filename='MagnetisationGraphics.png', width=450, height=450)"
   ]
  },
  {
   "cell_type": "raw",
   "metadata": {},
   "source": [
    "Firstly, material of the transformer core is selected. For this purpose, magnetisation curve of different materials are examined.\n",
    "\n",
    "Steel has higher flux density under the same magnetic field strength compared to other materials such as iron. However, because of their comparatively low losses at high frequencies, iron and ferrite are extensively used in high frequency transformer applications. Therefore, ferrite is selected as core material. "
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Step 2: Core selection "
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "First define input parameters of the transformer:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 23,
   "metadata": {
    "collapsed": true
   },
   "outputs": [],
   "source": [
    "import math\n",
    "\n",
    "V_in=417 #input (primary) voltage in Volts\n",
    "V_out=12500 #output (secondary) voltage in Volts\n",
    "P_out=30000 #rated output power in Watts\n",
    "f=100000 #frequency in Hz\n",
    "T_min=0 #min ambient temperature in Celcius\n",
    "T_max=40 #max ambient temperature in Celcius"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Constants:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 24,
   "metadata": {
    "collapsed": true
   },
   "outputs": [],
   "source": [
    "Ku=0.35 #window utilization factor\n",
    "Kf=4 #square wave coefficient"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "For the first transformer design iteration, initalize the parameters as below. Efficiency is not a critical parameter for X-RAY transformer, since cost of losses can be ignored for its lifetime. Efficiency of the transformer is assumed to be larger than %90. Operation flux density is assumed to 0.35 which is the maximum operating point -below saturation- for ferrite core."
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 25,
   "metadata": {
    "collapsed": false
   },
   "outputs": [],
   "source": [
    "Bop=0.35 #operating flux denisty in Tesla\n",
    "eff=0.90 #efficiency\n",
    "J=3 # current density in A/mm2"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Power handling capability of a core is related to its area product Ap. To find Ap:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 72,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Area product: 43.08 cm4\n"
     ]
    }
   ],
   "source": [
    "P_in=P_out/eff\n",
    "P_total=P_in+P_out #total apparent power of the transformer\n",
    "\n",
    "Ap=(P_total)/(Bop*f*Ku*Kf*J*1e6) #in\n",
    "print (\"Area product:\", round(Ap*1e8,2), \"cm4\") "
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "From the ferrite core catalog in http://www.ferroxcube.com/, E shape E80/38/20,3C94 core is selected in accordance with 43 cm4 area product. Core dimensions and other parameters are given below:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 27,
   "metadata": {
    "collapsed": false,
    "scrolled": true
   },
   "outputs": [
    {
     "data": {
      "image/png": "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\n",
      "text/plain": [
       "<IPython.core.display.Image object>"
      ]
     },
     "execution_count": 27,
     "metadata": {
      "image/png": {
       "height": 550,
       "width": 550
      }
     },
     "output_type": "execute_result"
    }
   ],
   "source": [
    "from IPython.display import Image\n",
    "Image(filename='E80-38-20.png', width=550, height=550)"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 28,
   "metadata": {
    "collapsed": true
   },
   "outputs": [],
   "source": [
    "Ae=392 #effectice core area in mm2 according to core datasheet\n",
    "Ve=72 #effective volume in cm3\n",
    "M_core=180 #core mass in grams"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Step 3: Finding number of turns"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Number of turns at primary and secondary side can be found as follows:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 29,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "7.6\n",
      "227.77\n"
     ]
    }
   ],
   "source": [
    "Np=(V_in*1e6)/(Kf*f*Bop*Ae)#Primary number of turns using Faraday's Law\n",
    "print (round(Np,2))\n",
    "Ns=(V_out*Np)/(V_in)\n",
    "print (round(Ns,2))"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "We can approximate Np and Ns as below:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 30,
   "metadata": {
    "collapsed": true
   },
   "outputs": [],
   "source": [
    "Np=8\n",
    "Ns=228"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Step 4: Cable selection for windings"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Now it is time to determine the cable sizes to use in primary and secondary windings. For this purpose, skin depth has to be calculated."
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 73,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Skin depth for 100kHz: 0.2069 mm\n",
      "Copper area 0.1345 mm2\n"
     ]
    }
   ],
   "source": [
    "resistivity=1.69*1e-8 #copper resistivity in ohm.m\n",
    "rel_per=1*4*math.pi*1e-7 #relative permeability of ferrite core\n",
    "Skin_depth=math.sqrt((resistivity)/(math.pi*f*rel_per))*1e2 #skin depth in cm \n",
    "print (\"Skin depth for 100kHz:\",round(Skin_depth*10,4), \"mm\") #skin depth in mm\n",
    "Cop_area=math.pi*Skin_depth*Skin_depth\n",
    "print (\"Copper area\", round(Cop_area*1e2,4), \"mm2\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "From the AWG list, using AWG26 cable area of 0.129 mm2 is efficient to use and cover skin depth area. Therefore AWG26 cable is chosen. :"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 32,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "data": {
      "image/png": "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\nGY7G3oVV0ruaxRCMbo6tTWF4XPxk+XkBjYfZ7MU181jyPv+YV79ALwfh+HgLcSz4G4X5eOY4S3Gs\n3Ui4x53SZ+yDvWoPNUMremYv/pyf+9dU3HXldT3YPASX/i+S/PcLOYb3d4bXquVrqteR8PoZfNc8\ncM/Hv/J+HIq9C6ukdzWLIRjdUBvD5KZqcUPyOfrBHmqGVvSMBecaMJcDevDcAdchrGPvwirpXc1i\nCEY31MYquUGq/Wa6Hf1gDzVDK3rGgjMNwelvrd+OuQ5hHXsXVknvahZDMLqhNgjRD/ZQM7SiZyw4\nyRAc/PXo5LfA6I69C6ukdzWLIRjdUBuE6Ad7qBla0TOATexdWCW9q1kMweiG2iBEP9hDzdCKngFs\nYu/CKuldzWIIRjfUBiH6wR5qhlb0DGATexdWSe9qFkMwuqE2CNEP9lAztKJnAJvYu7BKelezGILR\nDbVBiH6wh5qhFT0D2MTehVXSu5rFEIxuqA1C9IM91Ayt6BnAJvYurJLe1SyGYHRDbRCiH+yhZmhF\nzwA2sXdhlfSuZjEEoxtqgxD9YA81Qyt6BrCJvQurpHc1iyEY3VAbhOgHe6gZWtEzgE3sXVglvatZ\nDMHo5nq1+Xncvr4eX1+3x8/82O/j/v39uP+WXnOk+PvPi71qDzVDq+090/vc6Wz53r8+7+9pj2tI\na/ya19u5trU7Z2yc72GV9K5mMQSruZN06vaTe91fXvw4of4NybvU/q9uhnLff15W+uH3/v3a5+le\nH8/19zD2Zq9nWs/Zts67ZX8VSy3nV8px7Lyx/d3eXb/f/vq+P36z79niDPfrZyV96XvydR8kuf+9\nP77nnL179/v+65//uU21Wj6ffv6xpHc1iyFYrbApn/6iyHmcULdLczcPQLef6DWywcMTqHvce/dE\n8Jr55PE0f573c/PvTeuXOVlPJ5v396x9f/gZ52LjPBPlNVO3kVi+NuBvbO+Z+Jzn/3wPfjCV7Mn5\nJmyyuGmunUfLr/Hcc4rzvvuMJ+05vhhfyF1TnjHe58+ePnftetMcizznjm2yOCZ/3Dd3DLfnP0+P\n1XO4/IwwX28bj6s5N9r8t+bi+fwe/VPo08V3FY+vzd+d76MYQov9jGO4vgzvhe+vXvy9P/eHPPbs\nx9cvBZ576fbsSff8z3Pvv3p2enz+39fn9iO9q1kMwWqZk5KYT26uWdZPXl7wXPOJWqyd5Pa8iG0/\n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\nUm3k1vgCcpxJ0yk+rxhfZNtGzHgdV2Fz5HJYe0/gXVPdazV2izvUEGO2thV75yCnKS+tva3Ij7Z3\nt+QvdMS+LZ53arHnns89lrFnLxdjyOTk9Xp3Qa09H/sg5hZ77/Wt5+Wl981MmN84V8VafOiQ8x+A\nw+2+d3Pn3PDa1Hxum8yfkZ7r9qLKRe26tPU69GF8m+pYq9VT+bpRv+5k7VRLiVmz+v07weFgLP/8\nz78f/3099kygkD/H79mgWOzcZoqKuqB4/e/9Pv1zrmDTTz6+p0YRwee9GiL7+dFJYFJt5Nb4HHmf\nNGbc9NXPK8cX27QRc9xxvY63/r0vcWyr9j+J7hZ3TS5GeSxX26L9c5DTlBc5/i29HVrkQNm7m/K3\ndMS+LZ93MoLjbz3vhPbs5bYYaq8pP/9JzC123etrvdfcL+9e9zcfU+8HPwBq7qcG3c5/AHbVZe+G\n57fmc9ub/LDQnd9e/xx/xg625WJ5Pt16Hfo0vt3qGF2LNPNP6bqTs1ctJWbN6jQEL/969OuvPj//\n7P869H8f//4n/PN2pWK3NmDb6zNN8HptWPSpKdxnxjc382P546k18rYNJsc0PZ80eOXzavFF9tqI\nr+OS/ok2l/9z7F0bzW/9sjF/aK+4y3Ixrte25Igc5LTkZVtvh6L8qHp3W/5ix+xbJ3fxiUWxN553\nQsf0cimG93PL/d7y/OSDmFvsl5/13tvaL68biilX6/2i6ac2fc5/APZ2/N5dXpu2nduic1bTfYFe\nWy5WrkubrkOfx7dPHZe1yj2Xu27orjvOfrWUmDWrwxA8/Qey/vG/6X0Pwf88/v3f4HU7/RXpUrFb\nN1jb69ebYGHx/ripphvxleOpNfKWE4i8Z+3meNMJqfD8PhtxOq74O17fG51cXt45TuLIeuc/v8m3\n2yvudfkYS7Vdd0wOclrysqkXZ8oeiD5vW/5SR+xbr3beycX+fszXuHzeCR3Ty7UY3mo/FS4/vz3m\nFnue49Z6r71f4tinXBV+qFC/WdE7/vwH4AjH7t33uSY8l225Fqbvmc7tpR+KbrA1F8vrUvt1aI/4\nPq9jWqvc88vrRhxr6brztmctJWbNOnYIfg22zwDi3/Cu/jvD0WC8QbHYcjMRF/G1wVZubpten2uC\nHHld2PBTc73I4/Ln/PFUG7k1PnkufH18o9/6eS9xfN7nG3ESH6d4HVf0va/HGjZQ7jN2sFvcOWsx\n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\nTpJR18MAAAAASUVORK5CYII=\n",
      "text/plain": [
       "<IPython.core.display.Image object>"
      ]
     },
     "execution_count": 32,
     "metadata": {
      "image/png": {
       "height": 550,
       "width": 550
      }
     },
     "output_type": "execute_result"
    }
   ],
   "source": [
    "from IPython.display import Image\n",
    "Image(filename='AWG26.png', width=550, height=550)"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Now, let's find the current to be carried in the primary and secondary winding. In single AWG26 cable, current conducting capacity is:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 74,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Ampacity of AWG26: 0.4 A\n"
     ]
    }
   ],
   "source": [
    "I_cap=Cop_area*J*1e2 #current conducting capacity of AWG26\n",
    "print(\"Ampacity of AWG26:\",round(I_cap,2), \"A\")"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 75,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Primary winding current: 79.94 A\n",
      "Secondary winding current: 2.4 A\n"
     ]
    }
   ],
   "source": [
    "I_p=P_in/V_in #primary winding current\n",
    "I_s=P_out/V_out #secondary winding current\n",
    "print (\"Primary winding current:\", round (I_p,2), \"A\")\n",
    "print (\"Secondary winding current:\",round (I_s,2), \"A\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "In order to conduct 80 A of current in primary winding and 2.4 A in secondary winding with AWG26 cables, cables must be connected in paralel, i.e, stranded. To calculate number of stranded conductors to be needed in the primary side and secondary side:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 76,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Number of stranded cables in primary side: 198.13\n",
      "Number of stranded cables in secondary side: 5.95\n"
     ]
    }
   ],
   "source": [
    "num_str_p=I_p/I_cap\n",
    "print (\"Number of stranded cables in primary side:\",round(num_str_p,2))\n",
    "num_str_s=I_s/I_cap\n",
    "print (\"Number of stranded cables in secondary side:\",round(num_str_s,2))"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "We can approximate number of stranded AWG26 cables as:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 36,
   "metadata": {
    "collapsed": true
   },
   "outputs": [],
   "source": [
    "num_str_p=199\n",
    "num_str_s=6"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Now, let's check whether the area and volume of stranded cables can fit the window area."
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 78,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Stranded cable area on primary side 25.52 mm2\n",
      "Stranded cable area on secondary side 0.77 mm2\n"
     ]
    }
   ],
   "source": [
    "dia_awg26=0.405 #in mm\n",
    "Str_area_p=num_str_p*math.pi*dia_awg26*dia_awg26/4\n",
    "Str_area_s=num_str_s*math.pi*dia_awg26*dia_awg26/4\n",
    "print (\"Stranded cable area on primary side:\",round(Str_area_p,2), \"mm2\")\n",
    "print (\"Stranded cable area on secondary side:\",round(Str_area_s,2), \"mm2\")"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 80,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Stranded cable diameter on primary side: 5.7 mm\n",
      "Stranded cable diameter on secondary side: 0.99 mm\n"
     ]
    }
   ],
   "source": [
    "Str_dia_p=2*math.sqrt(Str_area_p/math.pi)\n",
    "Str_dia_s=2*math.sqrt(Str_area_s/math.pi)\n",
    "print (\"Stranded cable diameter on primary side:\",round(Str_dia_p,2), \"mm\") # in mm\n",
    "print (\"Stranded cable diameter on secondary side:\",round(Str_dia_s,2), \"mm\") # in mm"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Length of the window of the core selected is 39.6 mm. 6 turns of primary side cable and 4 turn of secondary side cable cover the 6*5.71+4*0.99=39.21mm of this length. These cables can compose of the first turn of this length. In the second turn, 2 turns of primary side cable and 28 turn of secondary side cable can fit the core. Done with primary side winding, for the rest of the secondary side winding, length of the core can take 40 secondary side cables. 5 turn of seconday side cables will be needed to roll the cables around the core. Therefore, max area that cables cover in the window area:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 81,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Total winding thickness: 16.34 mm\n"
     ]
    }
   ],
   "source": [
    "Thickness=(2*Str_dia_p)+(5*Str_dia_s)\n",
    "print (\"Total winding thickness:\",round(Thickness,2), \"mm\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "16.37 mm < window width=19.65 mm. Therefore, we can say that cable area fits the window area and cable selection and follins is appropriate. "
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Let's calculate conductor resistances in both sides to calculate copper loss: "
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 82,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Primary winding resistance: 0.68 mohm per meter\n",
      "Secondary winding resistance: 22.51 mohm per meter\n"
     ]
    }
   ],
   "source": [
    "Cable_res=133.9 #AWG26 cable resistance in mohm per meter\n",
    "Str_res_p=Cable_res/num_str_p #stranded cable resistance in mohm per meter\n",
    "Str_res_s=Cable_res/num_str_s #stranded cable resistance in mohm per meter\n",
    "print (\"Primary winding resistance:\", round(Str_res_p,2), \"mohm per meter\")\n",
    "print (\"Secondary winding resistance:\",round(Str_res_s,2), \"mohm per meter\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "In order to find the length of the cable to be wrap around primary and secondary side, first calculate the perimeter of the core:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 95,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Core perimeter: 79.2 mm\n",
      "Primary winding cable length: 633.6 mm\n",
      "Secondary winding cable length: 19920.39 mm\n",
      "Primary winding cable resistance: 0.43 mohm\n",
      "Secondary winding cable resistance: 448.4 mohm\n"
     ]
    }
   ],
   "source": [
    "per_core=19.8*4 #perimeter in mm\n",
    "print (\"Core perimeter:\",per_core, \"mm\")\n",
    "cable_length_p=per_core*Np;\n",
    "cable_length_s=(per_core+Str_dia_p+(2.5*Str_dia_s))*Ns\n",
    "print (\"Primary winding cable length:\",round(cable_length_p,2), \"mm\") #in mm\n",
    "print (\"Secondary winding cable length:\",round(cable_length_s,2), \"mm\") #in mm\n",
    "cable_resistance_p=cable_length_p*1e-3*Str_res_p\n",
    "cable_resistance_s=cable_length_s*1e-3*Str_res_s\n",
    "print (\"Primary winding cable resistance:\",round(cable_resistance_p,2), \"mohm\") # in mohm\n",
    "print (\"Secondary winding cable resistance:\",round(cable_resistance_s,2), \"mohm\") # in mohm"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Step 5: Calculation of losses"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "### Copper loss"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "After primary and secondary cable resistances are obtained, copper losses can be calculated."
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 94,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Primary side copper loss: 2.724 Watts\n",
      "Secondary side copper loss: 0.002 Watts\n",
      "Total copper loss: 2.727 Watts\n"
     ]
    }
   ],
   "source": [
    "Copper_loss_p=I_p*I_p*cable_resistance_p*1e-3 #in Watts\n",
    "Copper_loss_s=I_s*I_s*cable_resistance_p*1e-3 #in Watts\n",
    "Total_copper_loss=Copper_loss_p+Copper_loss_s\n",
    "print (\"Primary side copper loss:\",round(Copper_loss_p,3), \"Watts\")\n",
    "print (\"Secondary side copper loss:\",round(Copper_loss_s,3), \"Watts\")\n",
    "print (\"Total copper loss:\",round(Total_copper_loss,3), \"Watts\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "### Core loss"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {
    "collapsed": true
   },
   "source": [
    "In order to find core loss, Steinmetz equation is utilized. For 3C94 ferrite core, steinmetz parameters are given below:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 93,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Core loss: 279.75 Watts\n"
     ]
    }
   ],
   "source": [
    "Cm=2.37*1e-3;\n",
    "x=1.46;\n",
    "y=2.75;\n",
    "ct2=1.65*1e-4;\n",
    "ct1=3.1*1e-2;\n",
    "ct0=2.45;\n",
    "T=40; # max operating temperature is 40C \n",
    "Core_loss_density=Cm*(f**x)*(Bop**y)*(ct0-ct1*T+ct2*(T**2))/1000; # core loss W/cm3\n",
    "Core_loss= Ve*Core_loss_density# core loss in W\n",
    "print (\"Core loss:\",round(Core_loss,2), \"Watts\")"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 92,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Total loss 282.47 Watts\n"
     ]
    }
   ],
   "source": [
    "Total_loss=Total_copper_loss+Core_loss\n",
    "print (\"Total loss:\",round(Total_loss,2), \"Watts\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Step 6: Temperature Rise"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Now let's find the temperature rise:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 91,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Temperature rise: 2747.38 in Celcius\n"
     ]
    }
   ],
   "source": [
    "T_rise=1e3*Core_loss_density*math.sqrt(Ve)/12\n",
    "print (\"Temperature rise:\",round(T_rise,2),\"in Celcius\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "2747C temperature rise is expected if the transformer is used with %100 of time. However, X-RAY transformer will be active seldomly. Therefore, we can assume that transformer will be used with a frequency of %1 of time. Then, T_rise is recalculated:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 90,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Temperature rise with %1 use of frequency: 0.27 in Celcius\n"
     ]
    }
   ],
   "source": [
    "T_rise=T_rise/100\n",
    "print (\"Temperature rise with %1 use of frequency:\",round(T_rise,2),\"in Celcius\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "There will only be 27C of temperature rise if the transformer is used actively with %1 of time."
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Step 7: Total mass of the transformer"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "Let's now find the total mass of the transformer. Total mass is composed of core and cables:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 85,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Core mass 360 grams\n",
      "Cable mass 278.23 grams\n",
      "Total mass 638.23 grams\n"
     ]
    }
   ],
   "source": [
    "Core_mass=2*M_core #in grams\n",
    "AWG26_mass=0.00114 #g per mm\n",
    "Cable_mass_p=AWG26_mass*cable_length_p*num_str_p\n",
    "Cable_mass_s=AWG26_mass*cable_length_s*num_str_s\n",
    "Total_cable_mass=Cable_mass_p+Cable_mass_s\n",
    "Total_mass=Core_mass+Total_cable_mass\n",
    "print (\"Core mass\", round(Core_mass,2),\"grams\")\n",
    "print (\"Cable mass\", round(Total_cable_mass,2),\"grams\")\n",
    "print (\"Total mass\",round(Total_mass,2),\"grams\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## Step 8: Cost of the transformer "
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "As the last step, cost of the XRAY transformer will be calculated. Price of the core is found as $6.75 on internet. Kg price of AWG26 cable size is found to be $3.5. Therefore, total price can be calculated as follows:"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 84,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "Total Cost: 7.73 $\n"
     ]
    }
   ],
   "source": [
    "Cost_core=6.75 # in $\n",
    "Cost_copper=(Total_cable_mass*3.5)/1000\n",
    "Total_cost=Cost_core+Cost_copper\n",
    "print (\"Total Cost:\",round(Total_cost,2),\"$\")"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "## SUMMARY"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": 97,
   "metadata": {
    "collapsed": false
   },
   "outputs": [
    {
     "data": {
      "image/png": "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\n",
      "text/plain": [
       "<IPython.core.display.Image object>"
      ]
     },
     "execution_count": 97,
     "metadata": {
      "image/png": {
       "height": 500,
       "width": 500
      }
     },
     "output_type": "execute_result"
    }
   ],
   "source": [
    "from IPython.display import Image\n",
    "Image(filename='Summary.png', width=500, height=500)"
   ]
  },
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "# # THE END"
   ]
  }
 ],
 "metadata": {
  "kernelspec": {
   "display_name": "Python 3",
   "language": "python",
   "name": "python3"
  },
  "language_info": {
   "codemirror_mode": {
    "name": "ipython",
    "version": 3
   },
   "file_extension": ".py",
   "mimetype": "text/x-python",
   "name": "python",
   "nbconvert_exporter": "python",
   "pygments_lexer": "ipython3",
   "version": "3.5.1"
  }
 },
 "nbformat": 4,
 "nbformat_minor": 0
}
  pull-requests: write
  contents: read
jobs:
  scan-pull-request:
    runs-on: ubuntu-latest
    # A pull request needs to be approved, before Frogbot scans it. Any GitHub user who is associated with the
    # "frogbot" GitHub environment can approve the pull request to be scanned.
    # Read more here (Install Frogbot Using GitHub Actions): https://docs.jfrog-applications.jfrog.io/jfrog-applications/frogbot/setup-frogbot/setup-frogbot-using-github-actions
    environment: frogbot
    steps:
      - uses: actions/checkout@v4
        with:
          ref: ${{ github.event.pull_request.head.sha }}

      - uses: jfrog/frogbot@5d9c42c30f1169d8be4ba5510b40e75ffcbbc2a9  # v2.21.2
        env:
          # [Mandatory if the two conditions below are met]
          # 1. The project uses npm, yarn 2, NuGet or .NET to download its dependencies
          # 2. The `installCommand` variable isn't set in your frogbot-config.yml file.
          #
          # The command that installs the project dependencies (e.g "npm i", "nuget restore" or "dotnet restore")
          # JF_INSTALL_DEPS_CMD: ""

          # [Mandatory]
          # JFrog platform URL
          JF_URL: ${{ secrets.JF_URL }}

          # [Mandatory if JF_USER and JF_PASSWORD are not provided]
          # JFrog access token with 'read' permissions on Xray service
          JF_ACCESS_TOKEN: ${{ secrets.JF_ACCESS_TOKEN }}

          # [Mandatory if JF_ACCESS_TOKEN is not provided]
          # JFrog username with 'read' permissions for Xray. Must be provided with JF_PASSWORD
          # JF_USER: ${{ secrets.JF_USER }}

          # [Mandatory if JF_ACCESS_TOKEN is not provided]
          # JFrog password. Must be provided with JF_USER
          # JF_PASSWORD: ${{ secrets.JF_PASSWORD }}

          # [Mandatory]
          # The GitHub token automatically generated for the job
          JF_GIT_TOKEN: ${{ secrets.GITHUB_TOKEN }}

          # [Optional]
          # If the machine that runs Frogbot has no access to the internat, set the name of a remote repository
          # in Artifactory, which proxies https://releases.jfrog.io/artifactory
          # The 'frogbot' executable and other tools it needs will be downloaded through this repository.
          # JF_RELEASES_REPO: ""

          # [Optional]
          # Frogbot will download the project dependencies, if they're not cached locally. To download the
          # dependencies from a virtual repository in Artifactory, set the name of of the repository. There's no
          # need to set this value, if it is set in the frogbot-config.yml file.
          # JF_DEPS_REPO: ""
