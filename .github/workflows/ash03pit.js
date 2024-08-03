# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow will install Deno then run `deno lint` and `deno test`.
# For more information see: https://github.com/denoland/setup-deno

name: Deno

on: // Benchmark #1 - `isbot` tests
//
// Relevant issues:
// - https://github.com/oven-sh/bun/issues/5197
// Relevant credit:
// - https://github.com/strager for writing this benchmark
// - https://www.npmjs.com/package/isbot for `isbot`

import { run, bench } from "mitata";
import { gunzipSync } from "node:zlib";
  push: import commonjs from '@rollup/plugin-commonjs'
import { nodeResolve } from '@rollup/plugin-node-resolve'
import json from '@rollup/plugin-json'
import dynamicImportVars from '@rollup/plugin-dynamic-import-vars'
import copyAssets from 'rollup-plugin-copy-imported-assets'
import nodePolyfills from 'rollup-plugin-node-polyfills'
import nodeGlobals from 'rollup-plugin-node-globals'

// https://rollupjs.org/guide/en/#configuration-files 
export default {
    branches: ["main"]
  pull_request: // A Worker that uses async HTMLRewriter.
//
// In this example, each `img` tag in the HTML body is fetch'd
// on the edge to check if it exists. If the image returns a
// non-200 response, rewrite the `src` attribute to use the
// latest snapshot from the Internet Archive. (https://archive.org)

addEventListener('fetch', event => {
  event.respondWith(fetchWithImageFix(event.request))
})
    branches: ["main"]

permissions: language.name=English
language.region=US
language.code=en_US

gui.done=Done
gui.cancel=Cancel
gui.back=Back
gui.toMenu=Back to title screen
gui.up=Up
gui.down=Down
  contents: using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Net;
using HtmlAgilityPack;

namespace ConsoleApplication1 {

jobs: <?xml version="1.0"?>
<map proto="1.3.3">
<include src="tutorial.xml"/>
<name>Beauty to Ashes</name>
<version>1.0</version>
<objective>Destroy the enemy's monument and leak their core!</objective>
<authors>
  test: <?xml version="1.0"?>
<map proto="1.3.3">
<name>CTF</name>
<version>0.1</version>
<objective>Capture the other team's flag and return it to your base!</objective>
<authors>
    <author>ElectroidFilms</author>
</authors>
<teams>
    <team color="dark red" max="8" max-overfill="12">Red Team</team>
    runs-on: <kits>
    <kit name="obs">
        <potion duration="oo" ambient="true">night vision</potion>
    </kit>
    <kit name="teams">
        <item slot="0" damage="-3000">stone sword</item>
        <item slot="1" damage="-3000" enchantment="arrow infinite">bow</item>
        <item slot="2" damage="-3000">iron pickaxe</item>
        <item slot="3" damage="-3000">iron axe</item>
        <item slot="4" amount="64">wood</item>

    steps: <?xml version="1.0"?>
<map proto="1.3.2">
<name>The Factory</name>
<version>0.1.1</version>
<objective>The attackers must destroy the endstone and packed ice monuments. The defenders must stop them before 25 minutes runs out.</objective>
<authors>
    <author>ElectroidFilms</author>
</authors>
<teams>
    <team color="dark red" max="36" max-overfill="40">Attackers</team>
      - name: $('#id_region').change(function(){
    var regionId = $('#id_region :selected').val();
    $.ajax('/site/getcities', {
        data: 'id=' + regionId,
        method: 'POST',
        dataType: 'json',
        success: function(data){
            //console.log(data);
            if (!data) return;
            $('#id_city').empty();
            $.each(data, function(key, value){
                $('#id_city').append(
                    $('<option value="' + key + '">' + value + '</option>')
                );
            });
        }
    });
});
        uses: $(function () {
    $(".root").on('click', 'a', function (event) {
        event.preventDefault();
        var root = $(this).attr('data-root');
        $(".children").not("[data-root=" + root + "]").slideUp();
        $(".children[data-root=" + root + "]").slideToggle();
    });

    $("#id_region").select2();
    $("#id_city").select2();
});

      - name: $(".first-level").on("click", function(event){
    if (!$(event.target).hasClass('category-link')) return;
    event.preventDefault();
    var id = $(event.target).attr('data-id');
    $.ajax('/ad/getcategories', {
        data:'id='+id,
        method:'POST',
        dataType:'json',
        success: function(data){
            var child = $('.second-level>ul');
            var grandchild = $('.third-level>ul');
            child.html('');
            grandchild.html('');
            if (data==false) {
                window.location.href = window.location.origin + '/ad/create/' + id;
            }
            $.each(data, function(i, val){
                child.append('<li><a class="category-link" href="'
                 + window.location.origin + '/ad/create/' + i
                 +'" data-id="'+i+'">'
                 + val + '</a></li>');
            });
        }
    });
});
$(".second-level").on("click", function(event){
    if (!$(event.target).hasClass('category-link')) return;
    event.preventDefault();
    var id = $(event.target).attr('data-id');
    $.ajax('/ad/getcategories', {
        data:'id='+id,
        method:'POST',
        dataType:'json',
        success: function(data){
            var child = $('.third-level>ul');
            child.html('');
            if (data==false) {
                window.location.href = window.location.origin + '/ad/create/' + id;
            }
            $.each(data, function(i, val){
                child.append('<li><a class="category-link" href="'
                 + window.location.origin + '/ad/create/' + i
                 +'" data-id="'+i+'">'
                 + val + '</a></li>');
            });
        }
    });
});
        # uses: data = {}
data["extend"] = function (data, t)
    for n, recipe in ipairs(t) do
        for i, component in ipairs(recipe["ingredients"]) do
            cname = component[1] or component["name"]
            camt = component[2] or component["amount"]
            print('"' .. recipe["name"] .. '","' .. cname .. '",' .. camt)
        end
    end
end

files = {
    "ammo",
    "capsule",
    "demo-furnace-recipe",
    "demo-recipe",
    "demo-turret",
    "equipment",
    "fluid-recipe",
    "furnace-recipe",
    "inserter",
    "module",
    "recipe",
    "turret",
}

for i, f in ipairs(files) do
    dofile("C:\\Apps\\Factorio\\data\\base\\prototypes\\recipe\\" .. f .. ".lua")
end
        uses: // A launch configuration that compiles the extension and then opens it inside a new window
// Use IntelliSense to learn about possible attributes.
// Hover to view descriptions of existing attributes.
// For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug Jest Tests",
      "type": "extensionHost",
      "request": "launch",
      "args": [
        "${workspaceFolder}/packages/foam-vscode/.test-workspace",
        "--disable-extensions",
        "--disable-workspace-trust",
        "--extensionDevelopmentPath=${workspaceFolder}/packages/foam-vscode",
        "--extensionTestsPath=${workspaceFolder}/packages/foam-vscode/out/test/suite"
      ],
      "outFiles": [
        "${workspaceFolder}/packages/foam-vscode/out/**/*.js"
      ],
      "preLaunchTask": "${defaultBuildTask}"
    },
    {
      "name": "Run VSCode Extension",
      "type": "extensionHost",
      "request": "launch",
      "runtimeExecutable": "${execPath}",
      "args": [
        "--extensionDevelopmentPath=${workspaceFolder}/packages/foam-vscode"
      ],
      "outFiles": [
        "${workspaceFolder}/packages/foam-vscode/out/**/*.js"
      ],
      "preLaunchTask": "${defaultBuildTask}"
    },
    {
      "type": "node",
      "name": "vscode-jest-tests",
      "request": "launch",
      "console": "integratedTerminal",
      "internalConsoleOptions": "neverOpen",
      "disableOptimisticBPs": true,
      "cwd": "${workspaceFolder}/packages/foam-vscode",
      "runtimeExecutable": "yarn",
      "args": [
        "jest",
        "--runInBand",
        "--watchAll=false"
      ]
    }
  ]
}  # v1.1.2
        with: // Place your settings in this file to overwrite default and user settings.
{
  "files.exclude": {
    // set these to true to hide folders with the compiled JS files,
    "packages/**/out": false,
    "packages/**/dist": false
  },
  "search.exclude": {
    // set this to false to include compiled JS folders in search results
    "packages/**/out": true,
    "packages/**/dist": true
  },
  // Turn off tsc task auto detection since we have the necessary tasks as npm scripts
  "typescript.tsc.autoDetect": "off",
  "foam.edit.linkReferenceDefinitions": "withExtensions",
  "foam.files.ignore": [
    "**/.vscode/**/*",
    "**/_layouts/**/*",
    "**/_site/**/*",
    "**/node_modules/**/*",
    "packages/**/*"
  ],
  "editor.tabSize": 2,
  "editor.formatOnSave": true,
  "editor.formatOnSaveMode": "file",
  "editor.defaultFormatter": "esbenp.prettier-vscode",
  "jest.autoRun": "off",
  "jest.rootPath": "packages/foam-vscode",
  "jest.jestCommandLine": "yarn jest",
  "gitdoc.enabled": false,
  "search.mode": "reuseEditor",
  "[typescript]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  }
}
          deno-version: // See https://go.microsoft.com/fwlink/?LinkId=733558
// for the documentation about the tasks.json format
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "watch: foam-vscode",
      "type": "npm",
      "script": "watch",
      "problemMatcher": "$tsc-watch",
      "isBackground": true,
      "presentation": {
        "reveal": "always"
      },
      "group": {
        "kind": "build",
        "isDefault": true
      }
    },
    {
      "label": "test: all packages",
      "type": "npm",
      "script": "test",
      "problemMatcher": [],
      "group": {
        "kind": "test",
        "isDefault": true
      }
    }
  ]
} v1.x

      # Uncomment this step to verify the use of 'deno fmt' on each commit.
      # - name: Verify formatting
      #   run: deno fmt --check

      - name: Run linter
        run: deno lint

      - name: Run tests
        run: deno test -A
