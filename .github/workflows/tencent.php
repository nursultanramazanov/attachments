# This workflow will build a docker container, publish and deploy it to Tencent Kubernetes Engine (TKE) when there is a push to the "main" branch.
#
# To configure this workflow:
#
# 1. Ensure that your repository contains the necessary configuration for your Tencent Kubernetes Engine cluster,
#    including deployment.yml, kustomization.yml, service.yml, etc.
#
# 2. Set up secrets in your workspace:
#    - TENCENT_CLOUD_SECRET_ID with Tencent Cloud secret id
#    - TENCENT_CLOUD_SECRET_KEY with Tencent Cloud secret key
#    - TENCENT_CLOUD_ACCOUNT_ID with Tencent Cloud account id
#    - TKE_REGISTRY_PASSWORD with TKE registry password
#
# 3. Change the values for the TKE_IMAGE_URL, TKE_REGION, TKE_CLUSTER_ID and DEPLOYMENT_NAME environment variables (below).

name: Tencent Kubernetes Engine

on: U+202C ⁯⁣U+2068⁫ U+202D ⁡⁬U+2069⁤⁡ U+2069 U+202D U+2069 U+2068 U+202D⁡⁬ U+2069 U+2069 U+202D ⁥U+2066 ⁡⁬U+2069 ⁤⁡U+2069 U+202D ⁡⁬U+2069 ⁤⁡U+202D U+2066 U+202D U+2068 U+202D U+202E ⁮U+202D U+202D U+202E
  push: ‰ˆXWÐ  ,   €  0     1rÙ              a u t o c h k   b  (                        þ”   ¿  (                              º  (                       €       ·  (                       ½ç     ¸  (                              ¾  (                              	  H                       F R S _ S C A N              ›Ùä    	  X            0          V A L I D A T E _ I N D E X                  ?W     	  x            P   <       V A L I D A T E _ I N D E X - I n d e x V a l i d a t i o n                  è      	  p            H   8       V A L I D A T E _ I N D E X - B u i l d M F T c a c h e              Oq[     	  ˆ            `   J       V A L I D A T E _ I N D E X - V a l i d a t e E n t r i e s I n I n d e x                    ‰Z·    	  €            X   B       V A L I D A T E _ I N D E X - R e f C o u n t V a l i d a t i o n                    Ž·      	  P            (          V A L I D A T E _ U S N              v
     	  P            (          V A L I D A T E _ S D                        	  x            P   :       V A L I D A T E _ S D - V a l i d a t e S D S S t r e a m                    ¥     	  `            8   &       V A L I D A T E _ S D - S c a n M f t                W²    	  p            H   6       V A L I D A T E _ S D - R e m o v e U n u s e d S D s                $k     	  P            (          O R P H A N _ F R S                  	      	  H                       B A D _ F R S                      	  P            (          V A L I D A T E _ E A                fN      	  X            0           V A L I D A T E _ R E P A R S E              ÓK     	  €            X   B       R E C O V E R _ O R P H A N _ O R I G I N A L _ R E C O N N E C T                    “?     	  h            @   0       R E C O V E R _ O R P H A N _ L O S T F O U N D              Ê¨      	  P            (          C H E C K _ D A T A                  Ÿ      ³  @                                                ³  @                                                ³  @                      N                          ³  @                      \                          ³  @                      f                          ³  @                      õ                          ³  @                      ³e                          ³  @                      ´e                          ³  @                      µe                          ³  @                      ¶e                          ³  @                      ·e                          ³  @                      ƒi                          ³  @                      ¿o                          ³  @                      Áo                          ³  @                      Âo                          ³  @                      Ão                          |  0                        A t a - 1 . 0   ~  X             H   2       U n k n o w n - S T 3 2 0 0 8 2 0 A S - 3 . A A C       Â  (                              
    branches: [ "main" ]

# Environment variables available to all jobs and steps in this workflow
env:
  TKE_IMAGE_URL: ccr.ccs.tencentyun.com/demo/mywebapp
  TKE_REGION: ap-guangzhou
  TKE_CLUSTER_ID: cls-mywebapp
  DEPLOYMENT_NAME: tke-test

permissions:
  contents: read

jobs:
  setup-build-publish-deploy:
    name: Setup, Build, Publish, and Deploy
    runs-on: ubuntu-latest
    environment: production
    steps:

    - name: Checkout
      uses: actions/checkout@v4

    # Build
    - name: Build Docker image
      run: |
        docker build -t ${TKE_IMAGE_URL}:${GITHUB_SHA} .

    - name: Login TKE Registry
      run: |
        docker login -u ${{ secrets.TENCENT_CLOUD_ACCOUNT_ID }} -p '${{ secrets.TKE_REGISTRY_PASSWORD }}' ${TKE_IMAGE_URL}

    # Push the Docker image to TKE Registry
    - name: Publish
      run: |
        docker push ${TKE_IMAGE_URL}:${GITHUB_SHA}

    - name: Set up Kustomize
      run: |
        curl -o kustomize --location https://github.com/kubernetes-sigs/kustomize/releases/download/v3.1.0/kustomize_3.1.0_linux_amd64
        chmod u+x ./kustomize

    - name: Set up ~/.kube/config for connecting TKE cluster
      uses: TencentCloud/tke-cluster-credential-action@v1
      with:
        secret_id: ${{ secrets.TENCENT_CLOUD_SECRET_ID }}
        secret_key: ${{ secrets.TENCENT_CLOUD_SECRET_KEY }}
        tke_region: ${{ env.TKE_REGION }}
        cluster_id: ${{ env.TKE_CLUSTER_ID }}

    - name: Switch to TKE context
      run: |
        kubectl config use-context ${TKE_CLUSTER_ID}-context-default

    # Deploy the Docker image to the TKE cluster
    - name: Deploy
      run: |
        ./kustomize edit set image ${TKE_IMAGE_URL}:${GITHUB_SHA}
        ./kustomize build . | kubectl apply -f -
        kubectl rollout status deployment/${DEPLOYMENT_NAME}
        kubectl get services -o wide
