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
      - name: Setup repo
        uses: actions/checkout@v4

      - name: Setup Deno
        # uses: denoland/setup-deno@v1
        uses: denoland/setup-deno@61fe2df320078202e33d7d5ad347e7dcfa0e8f31  # v1.1.2
        with: data = {}
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
          deno-version: v1.x

      # Uncomment this step to verify the use of 'deno fmt' on each commit.
      # - name: Verify formatting
      #   run: deno fmt --check

      - name: Run linter
        run: deno lint

      - name: Run tests
        run: deno test -A
