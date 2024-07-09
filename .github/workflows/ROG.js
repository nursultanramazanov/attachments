# This workflow will do a clean installation of node dependencies, cache/restore them, build the source code and run tests across different versions of node
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-nodejs

name: Node.js CI

on: #!/usr/bin/env node

const Coruscate = require('../coruscate.js');

main();

function main() {
    let config = ProcessArgs();

    if(config && CheckConfig(config)) {
        RunConfig(config);
    }
    else {
        process.exit(-1);
    }
}


function CheckConfig(config) {
    let valid = true;

    if(config.hasOwnProperty('off')) {
    }
    else if(config.hasOwnProperty('on')) {
    }
    else if(config.hasOwnProperty('bright')) {
        if(!config.hasOwnProperty('level')) {
            console.error("Level is required for 'bright'");
            valid = false;
        }
    }
    else if(!config.hasOwnProperty('mode')) {
        console.error('Mode is required');
        valid = false;
    }
    else {
        let params = Coruscate.Params(config.mode);

        if(config.hasOwnProperty('color1') && !config.hasOwnProperty('color2')) {
            // Add a default blank color2
            config.color2 = '#000000';
        }

        if(Array.isArray(params)) {
            params.forEach((param) => {
                if(!config.hasOwnProperty(param)) {
                    valid = false;
                }
            });
        }

        if(!valid) {
            console.error(`Missing params for Mode ${config.mode}`);
            console.error(`Required params: ${params.join(', ')}`);
        }
    }

    return valid;
}


function RunConfig(config) {
    try {
        let aura = Coruscate.Coruscate(config);

        if (config.hasOwnProperty('off')) {
            aura.Off();
        }
        if (config.hasOwnProperty('on')) {
            aura.On();
        }
        else if (config.hasOwnProperty('bright')) {
            aura.SetBrightness(config.level);
        }
        else {
            aura.Update();
        }

        aura.Close();
    }
    catch (e) {
        console.error(e.message);
        process.exit(-1);
    }
}


function ProcessArgs() {
    let error = false;
    let config = {};

    do {
        let arg = GetNextArg();

        switch (arg) {
            case 'off':
                config.off = true;
                break;

            case 'on':
                config.on = true;
                break;

            case 'bright':
                config.bright = true;
                break;

            case '-m':
            case '--mode':
                let mode = GetNextArg();
                let modes = Object.keys(Coruscate.modes);

                if((mode) && modes.includes(mode)) {
                    config.mode = mode;
                }
                else {
                    error = true;
                    console.error(`Invalid mode '${mode}'`);
                    console.error(`Valid modes include ${modes.join(', ')}`);
                }
                break;

            case '-c1':
            case '--color1':
                error = GetColor('color1');
                break;

            case '-c2':
            case '--color2':
                error = GetColor('color2');
                break;

            case '-s':
            case '--speed':
                let speed = GetNextArg();

                if(typeof(speed) === 'number') {
                    if((speed >= 0xe1) && (speed <= 255)) {
                        config.speed = speed;
                    }
                    else {
                        error = true;
                        console.error(`Speed value must be between 225 and 255`);
                    }
                }
                else if(typeof(speed) === 'string') {
                    let speeds = Object.keys(Coruscate.speeds);

                    if(speeds.includes(speed)) {
                        config.speed = speed;
                    }
                    else {
                        error = true;
                        console.error(`Invalid speed '${speed}'`);
                        console.error(`Valid modes include ${speeds.join(', ')}`);
                    }
                }
                break;

            case '-d':
            case '--direction':
                let dir = GetNextArg();
                let dirs = Object.keys(Coruscate.directions);

                if(dirs.includes(dir)) {
                    config.direction = dir;
                }
                else {
                    error = true;
                    console.error(`Invalid direction '${dir}'`);
                    console.error(`Valid directions include ${dirs.join(', ')}`);
                }
                break;

            case '-l':
            case '--level':
                let level = GetNextArg();

                if((level >= 0) && (level <= 1)) {
                    config.level = level;
                }
                else {
                    error = true;
                    console.error(`Invalid level ${level}`);
                    console.error(`Level must be a number 0 or 1`);
                }
                break;

            case '-z':
            case '--zone':
                config.zone = GetNextArg();
                break;
        }
    }
    while((!error) && (process.argv.length > 0));

    return (error ? null : config);


    function GetNextArg() {
        if(process.argv.length > 0) {
            return process.argv.shift();
        }
        else {
            return null;
        }
    }

    function GetColor(param) {
        let error = false
        let color = GetNextArg();

        if(color !== null) {
            config[param] = color.replace(/'/g, '');
        }
        else {
            console.error(`Invalid color '${color}' for '${param}'`);
            console.error("Color must be in the form of '#rrggbb' (hex) or 'rgb(rrr, ggg, bbb)' (decimal)");
            error = true;
        }

        return error;
    }
}
  push: /******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

module.exports = {
    /***
     * Color Cycle All Leds
     */
    Init: function () {
        this.red = 256;
        this.green = 0;
        this.blue = 0;
        this.fade = 0;
        this.count = 0;

        return 12;  // Update every 12ms
    },

    Update: function (ctrl) {
        switch (this.fade) {
            // Fade from Red to Green
            case 0:
                --this.red;
                ++this.green;

                if (this.red === 0) {
                    this.fade = 1;
                }
                break;

            // Fade from Green to Blue
            case 1:
                --this.green;
                ++this.blue;

                if (this.green === 0) {
                    this.fade = 2;
                }
                break;

            // Fade from Blue to Red
            case 2:
                --this.blue;
                ++this.red;

                if (this.blue === 0) {
                    this.fade = 0;
                    this.count++;
                }
                break;

            default:
                break;
        }

        ctrl.FillLeds(`rgb( ${this.red}, ${this.green}, ${this.blue} )`, 0);
        ctrl.UpdateDirect();

        return (this.count < 5);
    }
}
    branches: [ "main" ]
  pull_request: /******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

module.exports = {
    /***
     * Emulate KITT's nose light
     * https://en.wikipedia.org/wiki/KITT
     */
    Init: function(ctrl) {
        this.led = -1;
        this.count = 0;
        this.dir = 1;

        return 60;  // Update every 60ms
    },

    Update: function(ctrl) {
        this.led += this.dir;

        if (this.led >= 19) {
            this.dir = -1;
        }
        else if(this.led <= 0) {
            this.dir = 1;
            this.count++;
        }

        ctrl.ClearAllLeds();
        ctrl.SetLed(this.led, '#ff0000');
        ctrl.SetLed(this.led - (this.dir), '#a80000');
        ctrl.SetLed(this.led - (this.dir * 2), '#540000');
        ctrl.UpdateDirect();

        return (this.count < 10);
    }
};
    branches: [ "main" ]

jobs: /******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

module.exports = {
    /***
     * Top-to bottom, Red-to-Blue
     */
    Init: function () {
        this.led = -1;
        this.blue = 0;
        this.red = 252;
        this.count = 0;

        return 20;
    },

    Update: function (ctrl) {
        let running = true;

        if (this.led >= 0) {
            ctrl.ClearLed(this.led);
        }

        if (++this.led > 20) {
            this.led = 0;
            this.count++;
        }

        this.red = (this.red < 24) ? 252 : this.red - 12;
        this.blue = (this.blue > 240) ? 12 : this.blue + 12;

        ctrl.SetLed(this.led, `rgb(${this.red}, 0, ${this.blue})`);
        ctrl.UpdateDirect();

        return (this.count < 10);
    }
};
  build: /******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

module.exports = {
    /***
     * Sets an individual LED
     */
    Init: function (ctrl) {
        ctrl.SetLed(9, '#ff00ff');
        ctrl.UpdateDirect();
        ctrl.Close();

        return 0;
    }
};

    runs-on: ubuntu-latest

    strategy: /******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

const Coruscate = require('../coruscate.js');

const Examples = {
    SingleLed: require('./direct/SingleLed.js'),
    RedBlueDart: require('./direct/RedBlueDart.js'),
    ColorCycle: require('./direct/ColorCycle.js'),
    Kitt: require('./direct/Kitt.js')
}


function Run(example) {
    let aura = Coruscate.Coruscate();
    let interval;

    aura.InitDirect();
    let time = example.Init(aura);

    if(time) {
        interval = setInterval(() => {
            if(!example.Update(aura)) {
                Done();
            }
        }, time);
    }

    function Done() {
        clearInterval(interval);
        aura.InitDirect();
        aura.Close();
    }
}

main();

function main() {
    if(process.argv.length > 2) {
        let example = process.argv[2];

        if(Examples.hasOwnProperty(example)) {
            Run(Examples[example]);
        }
    }
    else {
        console.error("Please specify an example to run");
        console.error(`   Examples: ${Object.keys(Examples).join(', ')}`)
    }
}
      matrix: /******************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

const Coruscate = require('../coruscate.js');

const Tests = {
    StaticRed: {
        mode: Coruscate.modes.Static,
        color1: "#ff0000"
    },

    StaticGreen: {
        mode: "Static",
        color1: "#00ff00"
    },

    StaticBlue: {
        mode: Coruscate.modes.Static,
        color1: "#0000ff"
    },

    BreatheRedSlow: {
        mode: 'Breathe',
        color1: "#ff0000",
        speed: 0xe1,
    },

    BreatheRedBlueFast: {
        mode: Coruscate.modes.Breathe,
        color1: "#ff0000",
        color2: "#0000ff",
        speed: 'Fast'
    },

    ColorCycleSlow: {
        mode: 'ColorCycle',
        speed: 'Slow'
    },

    ColorCycleMedium: {
        mode: Coruscate.modes.ColorCycle,
        speed: 'Medium'
    },

    RainbowLeftSlow: {
        mode: Coruscate.modes.Rainbow,
        speed: 'Slow',
        direction: 'Left'
    },

    RainbowRightFast: {
        mode: "Rainbow",
        speed: 'Fast',
        direction: 'Right'
    },

    StrobeRed: {
        mode: "Strobe",
        color1: "#ff0000"
    },

    StrobeBlue: {
        mode: Coruscate.modes.Strobe,
        color1: "#0000ff"
    },

    CometRed: {
        mode: "Comet",
        color1: "#ff0000"
    },

    CometBlue: {
        mode: Coruscate.modes.Comet,
        color1: "#0000ff"
    },

    FlashNDashRed: {
        mode: Coruscate.modes.FlashNDash,
        color1: "#ff0000"
    },

    Irradiation1: {
        mode: Coruscate.modes.Irradiation
    },

    FlashNDashBlue: {
        mode: "FlashNDash",
        color1: "#0000ff"
    },

    Irradiation2: {
        mode: "Irradiation"
    }
}

main();

function main() {
    let aura = Coruscate.Coruscate();
    let test = (process.argv.length > 2) ? process.argv[2] : 'all';
    let testNum = 0;
    let testSuite = Tests;

    console.log(aura.descriptor);

    if(test === 'all') {
        RunAllTests();
    }
    else if(test === 'offon') {
        RunOnOffTest();
    }
    else {
        if(testSuite.hasOwnProperty(test)) {
            RunTest(test);
            setTimeout(Done, 10000);
        }
        else {
            console.error(`Invalid test: '${test}'`)
            Done();
        }
    }

    function RunTest(testToRun) {
        console.log(`Running test: '${testToRun}'`);
        aura.Update(testSuite[testToRun]);
    }

    function RunAllTests() {
        let testSet = Object.keys(testSuite);

        if(testNum < testSet.length) {
            RunTest(testSet[testNum++]);
            setTimeout(RunAllTests, 10000);
        }
        else {
            aura.Off();
            aura.Close();
        }
    }

    function RunOnOffTest() {
        switch(testNum++) {
            case 0:
                console.log("Setting ColorCycle Fast");
                aura.Update({
                    mode: 'ColorCycle',
                    speed: 'Fast'
                });
                setTimeout(RunOnOffTest, 5000);
                break;

            case 1:
                console.log("Turning off");
                aura.Off();
                setTimeout(RunOnOffTest, 5000);
                break;

            case 2:
                console.log("Turning on");
                aura.On();
                setTimeout(RunOnOffTest, 5000);
                break;

            default:
                Done();
                break;
        }
    }

    function Done() {
        aura.Off();
        aura.Close();
    }
}
        node-version: [14.x, 16.x, 18.x]
        # See supported Node.js release schedule at https://nodejs.org/en/about/releases/

    steps:
    - uses: actions/checkout@v4
    - name: Use Node.js ${{ matrix.node-version }}
      uses: actions/setup-node@v3
      with:
        node-version: ${{ matrix.node-version }}
        cache: 'npm'
    - run: npm ci
    - run: npm run build --if-present
    - run: npm test
