# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# 💁 The OpenShift Starter workflow will:
# - Checkout your repository
# - Perform a container image build
# - Push the built image to the GitHub Container Registry (GHCR)
# - Log in to your OpenShift cluster
# - Create an OpenShift app from the image and expose it to the internet

# ℹ️ Configure your repository and the workflow with the following steps:
# 1. Have access to an OpenShift cluster. Refer to https://www.openshift.com/try
# 2. Create the OPENSHIFT_SERVER and OPENSHIFT_TOKEN repository secrets. Refer to:
#   - https://github.com/redhat-actions/oc-login#readme
#   - https://docs.github.com/en/actions/reference/encrypted-secrets
#   - https://cli.github.com/manual/gh_secret_set
# 3. (Optional) Edit the top-level 'env' section as marked with '🖊️' if the defaults are not suitable for your project.
# 4. (Optional) Edit the build-image step to build your project.
#    The default build type is by using a Dockerfile at the root of the repository,
#    but can be replaced with a different file, a source-to-image build, or a step-by-step buildah build.
# 5. Commit and push the workflow file to your default branch to trigger a workflow run.

# 👋 Visit our GitHub organization at https://github.com/redhat-actions/ to see our actions and provide feedback.

name: OpenShift

env: 
 
Search…
All gists
Back to GitHub
@pfmoore
pfmoore/factorio-recipe-parser.lua
Last active 3 years ago • Report abuse
Code
Revisions
2
Stars
23
Forks
6
Clone this repository at &lt;script src=&quot;https://gist.github.com/pfmoore/aa101ee1daeebf202e87.js&quot;&gt;&lt;/script&gt;
<script src="https://gist.github.com/pfmoore/aa101ee1daeebf202e87.js"></script>
Parse the Factorio recipe files to create a CSV of recipes
factorio-recipe-parser.lua
data = {}
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
recipes.csv
Search this file…
piercing-bullet-magazine	copper-plate	5
piercing-bullet-magazine	steel-plate	1
rocket	electronic-circuit	1
rocket	explosives	2
rocket	iron-plate	2
explosive-rocket	rocket	1
explosive-rocket	explosives	5
shotgun-shell	copper-plate	2
shotgun-shell	iron-plate	2
piercing-shotgun-shell	copper-plate	2
piercing-shotgun-shell	steel-plate	2
railgun-dart	steel-plate	5
railgun-dart	electronic-circuit	5
poison-capsule	steel-plate	3
poison-capsule	electronic-circuit	3
poison-capsule	coal	10
slowdown-capsule	steel-plate	2
slowdown-capsule	electronic-circuit	2
slowdown-capsule	coal	5
basic-grenade	iron-plate	5
basic-grenade	coal	10
defender-capsule	piercing-bullet-magazine	1
defender-capsule	electronic-circuit	2
defender-capsule	iron-gear-wheel	3
distractor-capsule	defender-capsule	4
distractor-capsule	advanced-circuit	3
destroyer-capsule	distractor-capsule	4
destroyer-capsule	speed-module	1
basic-electric-discharge-defense-remote	electronic-circuit	1
copper-plate	copper-ore	1
iron-plate	iron-ore	1
stone-brick	stone	2
wood	raw-wood	1
wooden-chest	wood	4
iron-stick	iron-plate	1
iron-axe	iron-stick	2
iron-axe	iron-plate	3
stone-furnace	stone	5
boiler	stone-furnace	1
boiler	pipe	1
steam-engine	iron-gear-wheel	5
steam-engine	pipe	5
steam-engine	iron-plate	5
iron-gear-wheel	iron-plate	2
electronic-circuit	iron-plate	1
electronic-circuit	copper-cable	3
basic-transport-belt	iron-plate	1
basic-transport-belt	iron-gear-wheel	1
basic-mining-drill	electronic-circuit	3
basic-mining-drill	iron-gear-wheel	5
basic-mining-drill	iron-plate	10
burner-mining-drill	iron-gear-wheel	3
burner-mining-drill	stone-furnace	1
burner-mining-drill	iron-plate	3
basic-inserter	electronic-circuit	1
basic-inserter	iron-gear-wheel	1
basic-inserter	iron-plate	1
burner-inserter	iron-plate	1
burner-inserter	iron-gear-wheel	1
pipe	iron-plate	1
offshore-pump	electronic-circuit	2
offshore-pump	pipe	1
offshore-pump	iron-gear-wheel	1
copper-cable	copper-plate	1
small-electric-pole	wood	2
small-electric-pole	copper-cable	2
pistol	copper-plate	5
pistol	iron-plate	5
submachine-gun	iron-gear-wheel	10
submachine-gun	copper-plate	5
submachine-gun	iron-plate	10
basic-bullet-magazine	iron-plate	2
basic-armor	iron-plate	40
radar	electronic-circuit	5
radar	iron-gear-wheel	5
radar	iron-plate	10
small-lamp	electronic-circuit	1
small-lamp	iron-stick	3
small-lamp	iron-plate	1
pipe-to-ground	pipe	10
pipe-to-ground	iron-plate	5
assembling-machine-1	electronic-circuit	3
assembling-machine-1	iron-gear-wheel	5
assembling-machine-1	iron-plate	9
repair-pack	electronic-circuit	1
repair-pack	iron-gear-wheel	1
gun-turret	iron-gear-wheel	5
gun-turret	copper-plate	5
gun-turret	iron-plate	10
night-vision-equipment	advanced-circuit	5
night-vision-equipment	steel-plate	10
energy-shield-equipment	advanced-circuit	5
energy-shield-equipment	steel-plate	10
energy-shield-mk2-equipment	energy-shield-equipment	10
energy-shield-mk2-equipment	processing-unit	10
battery-equipment	battery	5
battery-equipment	steel-plate	10
battery-mk2-equipment	battery-equipment	10
battery-mk2-equipment	processing-unit	20
solar-panel-equipment	solar-panel	5
solar-panel-equipment	processing-unit	1
solar-panel-equipment	steel-plate	5
fusion-reactor-equipment	processing-unit	100
fusion-reactor-equipment	alien-artifact	30
basic-laser-defense-equipment	processing-unit	1
basic-laser-defense-equipment	steel-plate	5
basic-laser-defense-equipment	laser-turret	5
basic-electric-discharge-defense-equipment	processing-unit	5
basic-electric-discharge-defense-equipment	steel-plate	20
basic-electric-discharge-defense-equipment	laser-turret	10
basic-exoskeleton-equipment	processing-unit	10
basic-exoskeleton-equipment	electric-engine-unit	30
basic-exoskeleton-equipment	steel-plate	20
basic-oil-processing	crude-oil	10
advanced-oil-processing	crude-oil	10
advanced-oil-processing	water	5
heavy-oil-cracking	heavy-oil	4
heavy-oil-cracking	water	3
light-oil-cracking	light-oil	3
light-oil-cracking	water	3
sulfuric-acid	sulfur	5
sulfuric-acid	iron-plate	1
sulfuric-acid	water	10
plastic-bar	petroleum-gas	3
plastic-bar	coal	1
solid-fuel-from-light-oil	light-oil	1
solid-fuel-from-petroleum-gas	petroleum-gas	2
solid-fuel-from-heavy-oil	heavy-oil	2
sulfur	petroleum-gas	3
sulfur	water	3
lubricant	heavy-oil	1
empty-barrel	steel-plate	1
fill-crude-oil-barrel	crude-oil	25
fill-crude-oil-barrel	empty-barrel	1
empty-crude-oil-barrel	crude-oil-barrel	1
flame-thrower-ammo	iron-plate	5
flame-thrower-ammo	light-oil	2.5
flame-thrower-ammo	heavy-oil	2.5
steel-plate	iron-plate	5
long-handed-inserter	iron-gear-wheel	1
long-handed-inserter	iron-plate	1
long-handed-inserter	basic-inserter	1
fast-inserter	electronic-circuit	2
fast-inserter	iron-plate	2
fast-inserter	basic-inserter	1
smart-inserter	fast-inserter	1
smart-inserter	electronic-circuit	4
speed-module	advanced-circuit	5
speed-module	electronic-circuit	5
speed-module-2	speed-module	4
speed-module-2	processing-unit	5
speed-module-2	advanced-circuit	5
speed-module-3	speed-module-2	4
speed-module-3	advanced-circuit	5
speed-module-3	processing-unit	5
speed-module-3	alien-artifact	1
productivity-module	advanced-circuit	5
productivity-module	electronic-circuit	5
productivity-module-2	productivity-module	4
productivity-module-2	advanced-circuit	5
productivity-module-2	processing-unit	5
productivity-module-3	productivity-module-2	5
productivity-module-3	advanced-circuit	5
productivity-module-3	processing-unit	5
productivity-module-3	alien-artifact	1
effectivity-module	advanced-circuit	5
effectivity-module	electronic-circuit	5
effectivity-module-2	effectivity-module	4
effectivity-module-2	advanced-circuit	5
effectivity-module-2	processing-unit	5
effectivity-module-3	effectivity-module-2	5
effectivity-module-3	advanced-circuit	5
effectivity-module-3	processing-unit	5
effectivity-module-3	alien-artifact	1
player-port	electronic-circuit	10
player-port	iron-gear-wheel	5
player-port	iron-plate	1
fast-transport-belt	iron-gear-wheel	5
fast-transport-belt	basic-transport-belt	1
express-transport-belt	iron-gear-wheel	5
express-transport-belt	fast-transport-belt	1
express-transport-belt	lubricant	2
solar-panel	steel-plate	5
solar-panel	electronic-circuit	15
solar-panel	copper-plate	5
assembling-machine-2	iron-plate	9
assembling-machine-2	electronic-circuit	3
assembling-machine-2	iron-gear-wheel	5
assembling-machine-2	assembling-machine-1	1
assembling-machine-3	speed-module	4
assembling-machine-3	assembling-machine-2	2
car	engine-unit	8
car	iron-plate	20
car	steel-plate	5
straight-rail	stone	1
straight-rail	iron-stick	1
straight-rail	steel-plate	1
curved-rail	stone	4
curved-rail	iron-stick	4
curved-rail	steel-plate	4
diesel-locomotive	engine-unit	15
diesel-locomotive	electronic-circuit	5
diesel-locomotive	steel-plate	10
cargo-wagon	iron-gear-wheel	10
cargo-wagon	iron-plate	20
cargo-wagon	steel-plate	5
train-stop	electronic-circuit	5
train-stop	iron-plate	10
train-stop	steel-plate	3
rail-signal	electronic-circuit	1
rail-signal	iron-plate	5
heavy-armor	copper-plate	100
heavy-armor	steel-plate	50
basic-modular-armor	advanced-circuit	30
basic-modular-armor	processing-unit	5
basic-modular-armor	steel-plate	50
power-armor	processing-unit	100
power-armor	electric-engine-unit	30
power-armor	steel-plate	100
power-armor	alien-artifact	10
power-armor-mk2	effectivity-module-3	5
power-armor-mk2	speed-module-3	5
power-armor-mk2	processing-unit	200
power-armor-mk2	steel-plate	50
power-armor-mk2	alien-artifact	50
iron-chest	iron-plate	8
steel-chest	steel-plate	8
smart-chest	steel-chest	1
smart-chest	electronic-circuit	3
wall	stone-brick	5
flame-thrower	steel-plate	5
flame-thrower	iron-gear-wheel	10
land-mine	steel-plate	1
land-mine	explosives	2
rocket-launcher	iron-plate	5
rocket-launcher	iron-gear-wheel	5
rocket-launcher	electronic-circuit	5
shotgun	iron-plate	15
shotgun	iron-gear-wheel	5
shotgun	copper-plate	10
shotgun	wood	5
combat-shotgun	steel-plate	15
combat-shotgun	iron-gear-wheel	5
combat-shotgun	copper-plate	10
combat-shotgun	wood	10
railgun	steel-plate	15
railgun	copper-plate	15
railgun	electronic-circuit	10
railgun	advanced-circuit	5
science-pack-1	copper-plate	1
science-pack-1	iron-gear-wheel	1
science-pack-2	basic-inserter	1
science-pack-2	basic-transport-belt	1
science-pack-3	battery	1
science-pack-3	advanced-circuit	1
science-pack-3	smart-inserter	1
science-pack-3	steel-plate	1
alien-science-pack	alien-artifact	1
lab	electronic-circuit	10
lab	iron-gear-wheel	10
lab	basic-transport-belt	4
red-wire	electronic-circuit	1
red-wire	copper-cable	1
green-wire	electronic-circuit	1
green-wire	copper-cable	1
basic-transport-belt-to-ground	iron-plate	10
basic-transport-belt-to-ground	basic-transport-belt	5
fast-transport-belt-to-ground	iron-gear-wheel	20
fast-transport-belt-to-ground	basic-transport-belt-to-ground	2
express-transport-belt-to-ground	iron-gear-wheel	40
express-transport-belt-to-ground	fast-transport-belt-to-ground	2
basic-splitter	electronic-circuit	5
basic-splitter	iron-plate	5
basic-splitter	basic-transport-belt	4
fast-splitter	electronic-circuit	10
fast-splitter	iron-gear-wheel	10
fast-splitter	fast-transport-belt	4
express-splitter	advanced-circuit	10
express-splitter	iron-gear-wheel	10
express-splitter	express-transport-belt	4
advanced-circuit	electronic-circuit	2
advanced-circuit	plastic-bar	2
advanced-circuit	copper-cable	4
processing-unit	electronic-circuit	20
processing-unit	advanced-circuit	2
processing-unit	sulfuric-acid	0.5
logistic-robot	flying-robot-frame	1
logistic-robot	advanced-circuit	2
construction-robot	flying-robot-frame	1
construction-robot	electronic-circuit	2
logistic-chest-passive-provider	smart-chest	1
logistic-chest-passive-provider	advanced-circuit	1
logistic-chest-active-provider	smart-chest	1
logistic-chest-active-provider	advanced-circuit	1
logistic-chest-storage	smart-chest	1
logistic-chest-storage	advanced-circuit	1
logistic-chest-requester	smart-chest	1
logistic-chest-requester	advanced-circuit	1
rocket-defense	rocket	100
rocket-defense	advanced-circuit	128
rocket-defense	processing-unit	128
rocket-defense	speed-module-3	50
rocket-defense	productivity-module-3	50
roboport	steel-plate	45
roboport	iron-gear-wheel	45
roboport	advanced-circuit	45
steel-axe	steel-plate	5
steel-axe	iron-stick	2
big-electric-pole	steel-plate	5
big-electric-pole	copper-plate	5
substation	steel-plate	10
substation	advanced-circuit	5
substation	copper-plate	5
medium-electric-pole	steel-plate	2
medium-electric-pole	copper-plate	2
basic-accumulator	iron-plate	2
basic-accumulator	battery	5
steel-furnace	steel-plate	8
steel-furnace	stone-brick	10
electric-furnace	steel-plate	15
electric-furnace	advanced-circuit	5
electric-furnace	stone-brick	10
basic-beacon	electronic-circuit	20
basic-beacon	advanced-circuit	20
basic-beacon	steel-plate	10
basic-beacon	copper-cable	10
blueprint	advanced-circuit	1
deconstruction-planner	advanced-circuit	1
pumpjack	steel-plate	15
pumpjack	iron-gear-wheel	10
pumpjack	electronic-circuit	10
pumpjack	pipe	10
oil-refinery	steel-plate	15
oil-refinery	iron-gear-wheel	10
oil-refinery	stone-brick	10
oil-refinery	electronic-circuit	10
oil-refinery	pipe	10
engine-unit	steel-plate	1
engine-unit	iron-gear-wheel	1
engine-unit	pipe	2
electric-engine-unit	engine-unit	1
electric-engine-unit	lubricant	2
electric-engine-unit	electronic-circuit	2
flying-robot-frame	electric-engine-unit	1
flying-robot-frame	battery	2
flying-robot-frame	steel-plate	1
flying-robot-frame	electronic-circuit	3
explosives	sulfur	1
explosives	coal	1
explosives	water	1
battery	sulfuric-acid	2
battery	iron-plate	1
battery	copper-plate	1
storage-tank	iron-plate	20
storage-tank	steel-plate	5
small-pump	electric-engine-unit	1
small-pump	steel-plate	1
small-pump	pipe	1
chemical-plant	steel-plate	5
chemical-plant	iron-gear-wheel	5
chemical-plant	electronic-circuit	5
chemical-plant	pipe	5
small-plane	plastic-bar	120
small-plane	advanced-circuit	250
small-plane	electric-engine-unit	20
small-plane	battery	150
laser-turret	steel-plate	5
laser-turret	electronic-circuit	5
laser-turret	battery	3
  # 🖊️ EDIT your repository secrets to log into your OpenShift cluster and set up the context.
  # See https://github.com/redhat-actions/oc-login#readme for how to retrieve these values.
  # To get a permanent token, refer to https://github.com/redhat-actions/oc-login/wiki/Using-a-Service-Account-for-GitHub-Actions
  OPENSHIFT_SERVER: ${{ secrets.OPENSHIFT_SERVER }}
  OPENSHIFT_TOKEN: ${{ secrets.OPENSHIFT_TOKEN }}
  # 🖊️ EDIT to set the kube context's namespace after login. Leave blank to use your user's default namespace.
  OPENSHIFT_NAMESPACE: ""

  # 🖊️ EDIT to set a name for your OpenShift app, or a default one will be generated below.
  APP_NAME: ""

  # 🖊️ EDIT with the port your application should be accessible on.
  # If the container image exposes *exactly one* port, this can be left blank.
  # Refer to the 'port' input of https://github.com/redhat-actions/oc-new-app
  APP_PORT: ""

  # 🖊️ EDIT to change the image registry settings.
  # Registries such as GHCR, Quay.io, and Docker Hub are supported.
  IMAGE_REGISTRY: ghcr.io/${{ github.repository_owner }}
  IMAGE_REGISTRY_USER: ${{ github.actor }}
  IMAGE_REGISTRY_PASSWORD: ${{ github.token }}

  # 🖊️ EDIT to specify custom tags for the container image, or default tags will be generated below.
  IMAGE_TAGS: ""

on:
  # https://docs.github.com/en/actions/reference/events-that-trigger-workflows
  workflow_dispatch:
  push:
    # Edit to the branch(es) you want to build and deploy on each push.
    branches: [ "main" ]

jobs:
  # 🖊️ EDIT if you want to run vulnerability check on your project before deploying
  # the application. Please uncomment the below CRDA scan job and configure to run it in
  # your workflow. For details about CRDA action visit https://github.com/redhat-actions/crda/blob/main/README.md
  #
  # TODO: Make sure to add 'CRDA Scan' starter workflow from the 'Actions' tab.
  # For guide on adding new starter workflow visit https://docs.github.com/en/github-ae@latest/actions/using-workflows/using-starter-workflows

  crda-scan:
    uses: ./.github/workflows/crda.yml
    secrets:
      CRDA_KEY: ${{ secrets.CRDA_KEY }}
      # SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}       # Either use SNYK_TOKEN or CRDA_KEY

  openshift-ci-cd:
    # 🖊️ Uncomment this if you are using CRDA scan step above
    # needs: crda-scan
    name: Build and deploy to OpenShift
    runs-on: ubuntu-20.04
    environment: production

    outputs:
      ROUTE: ${{ steps.deploy-and-expose.outputs.route }}
      SELECTOR: ${{ steps.deploy-and-expose.outputs.selector }}

    steps:
    - name: Check for required secrets
      uses: actions/github-script@v6
      with:
        script: |
          const secrets = {
            OPENSHIFT_SERVER: `${{ secrets.OPENSHIFT_SERVER }}`,
            OPENSHIFT_TOKEN: `${{ secrets.OPENSHIFT_TOKEN }}`,
          };

          const GHCR = "ghcr.io";
          if (`${{ env.IMAGE_REGISTRY }}`.startsWith(GHCR)) {
            core.info(`Image registry is ${GHCR} - no registry password required`);
          }
          else {
            core.info("A registry password is required");
            secrets["IMAGE_REGISTRY_PASSWORD"] = `${{ secrets.IMAGE_REGISTRY_PASSWORD }}`;
          }

          const missingSecrets = Object.entries(secrets).filter(([ name, value ]) => {
            if (value.length === 0) {
              core.error(`Secret "${name}" is not set`);
              return true;
            }
            core.info(`✔️ Secret "${name}" is set`);
            return false;
          });

          if (missingSecrets.length > 0) {
            core.setFailed(`❌ At least one required secret is not set in the repository. \n` +
              "You can add it using:\n" +
              "GitHub UI: https://docs.github.com/en/actions/reference/encrypted-secrets#creating-encrypted-secrets-for-a-repository \n" +
              "GitHub CLI: https://cli.github.com/manual/gh_secret_set \n" +
              "Also, refer to https://github.com/redhat-actions/oc-login#getting-started-with-the-action-or-see-example");
          }
          else {
            core.info(`✅ All the required secrets are set`);
          }

    - name: Check out repository
      uses: actions/checkout@v4

    - name: Determine app name
      if: env.APP_NAME == ''
      run: |
        echo "APP_NAME=$(basename $PWD)" | tee -a $GITHUB_ENV

    - name: Determine image tags
      if: env.IMAGE_TAGS == ''
      run: |
        echo "IMAGE_TAGS=latest ${GITHUB_SHA::12}" | tee -a $GITHUB_ENV

    # https://github.com/redhat-actions/buildah-build#readme
    - name: Build from Dockerfile
      id: build-image
      uses: redhat-actions/buildah-build@v2
      with:
        image: ${{ env.APP_NAME }}
        tags: ${{ env.IMAGE_TAGS }}

        # If you don't have a Dockerfile/Containerfile, refer to https://github.com/redhat-actions/buildah-build#scratch-build-inputs
        # Or, perform a source-to-image build using https://github.com/redhat-actions/s2i-build
        # Otherwise, point this to your Dockerfile/Containerfile relative to the repository root.
        dockerfiles: |
          ./Dockerfile

    # https://github.com/redhat-actions/push-to-registry#readme
    - name: Push to registry
      id: push-image
      uses: redhat-actions/push-to-registry@v2
      with:
        image: ${{ steps.build-image.outputs.image }}
        tags: ${{ steps.build-image.outputs.tags }}
        registry: ${{ env.IMAGE_REGISTRY }}
        username: ${{ env.IMAGE_REGISTRY_USER }}
        password: ${{ env.IMAGE_REGISTRY_PASSWORD }}

    # The path the image was pushed to is now stored in ${{ steps.push-image.outputs.registry-path }}

    - name: Install oc
      uses: redhat-actions/openshift-tools-installer@v1
      with:
        oc: 4

    # https://github.com/redhat-actions/oc-login#readme
    - name: Log in to OpenShift
      uses: redhat-actions/oc-login@v1
      with:
        openshift_server_url: ${{ env.OPENSHIFT_SERVER }}
        openshift_token: ${{ env.OPENSHIFT_TOKEN }}
        insecure_skip_tls_verify: true
        namespace: ${{ env.OPENSHIFT_NAMESPACE }}

    # This step should create a deployment, service, and route to run your app and expose it to the internet.
    # https://github.com/redhat-actions/oc-new-app#readme
    - name: Create and expose app
      id: deploy-and-expose
      uses: redhat-actions/oc-new-app@v1
      with:
        app_name: ${{ env.APP_NAME }}
        image: ${{ steps.push-image.outputs.registry-path }}
        namespace: ${{ env.OPENSHIFT_NAMESPACE }}
        port: ${{ env.APP_PORT }}

    - name: Print application URL
      env:
        ROUTE: ${{ steps.deploy-and-expose.outputs.route }}
        SELECTOR: ${{ steps.deploy-and-expose.outputs.selector }}
      run: |
        [[ -n ${{ env.ROUTE }} ]] || (echo "Determining application route failed in previous step"; exit 1)
        echo
        echo "======================== Your application is available at: ========================"
        echo ${{ env.ROUTE }}
        echo "==================================================================================="
        echo
        echo "Your app can be taken down with: \"oc delete all --selector='${{ env.SELECTOR }}'\""
