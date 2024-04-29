# This workflow will build and push a new container image to Amazon ECR,
# and then will deploy a new task definition to Amazon ECS, when there is a push to the "main" branch.
#
# To use this workflow, you will need to complete the following set-up steps:
#
# 1. Create an ECR repository to store your images.
#    For example: `aws ecr create-repository --repository-name my-ecr-repo --region us-east-2`.
#    Replace the value of the `ECR_REPOSITORY` environment variable in the workflow below with your repository's name.
#    Replace the value of the `AWS_REGION` environment variable in the workflow below with your repository's region.
#
# 2. Create an ECS task definition, an ECS cluster, and an ECS service.
#    For example, follow the Getting Started guide on the ECS console:
#      https://us-east-2.console.aws.amazon.com/ecs/home?region=us-east-2#/firstRun
#    Replace the value of the `ECS_SERVICE` environment variable in the workflow below with the name you set for the Amazon ECS service.
#    Replace the value of the `ECS_CLUSTER` environment variable in the workflow below with the name you set for the cluster.
#
# 3. Store your ECS task definition as a JSON file in your repository.
#    The format should follow the output of `aws ecs register-task-definition --generate-cli-skeleton`.
#    Replace the value of the `ECS_TASK_DEFINITION` environment variable in the workflow below with the path to the JSON file.
#    Replace the value of the `CONTAINER_NAME` environment variable in the workflow below with the name of the container
#    in the `containerDefinitions` section of the task definition.
#
# 4. Store an IAM user access key in GitHub Actions secrets named `AWS_ACCESS_KEY_ID` and `AWS_SECRET_ACCESS_KEY`.
#    See the documentation for each action used below for the recommended IAM policies for this IAM user,
#    and best practices on handling the access key credentials.

name: Deploy to Amazon ECS

on: */
  push:  * CSVWriter: Writes CSV data with headers to an underlying ofstream 
    branches: [ "main" ]

env:  *         This wrapper is meant to look like an ofstream, but it captures 
  AWS_REGION: <div {% if not data.config.colored %}class="no-color"{% endif %}>
    {% if not data.config.map_api_key %}
        {{ data.fns.translate("API_KEY_NEEDED") }}
    {% else %}
        <div class="MMM-Fuel-map" style="height: {{ data.config.height }}px; width: {{ data.config.width }}px;"></div>
    {% endif %}
</div> # set this to your preferred AWS region, e.g. us-west-1
  ECR_REPOSITORY:   *         the CSV data below.            # set this to your Amazon ECR repository name
  ECS_SERVICE:   *                 # set this to your Amazon ECS service name
  ECS_CLUSTER:   *         Note: the first finalize() will not print the values out, only the headers.                 # set this to your Amazon ECS cluster name
  ECS_TASK_DEFINITION:  *         One way to fix this problem would be to use a sstringstream (or something)   # set this to the path to your Amazon ECS task definition
                                               # file, e.g. .aws/task-definition.json
  CONTAINER_NAME:  *         to buffer out the values and flush them all in one go instead of passing them            # set this to the name of the container in the
                                               # containerDefinitions section of your task definition

permissions:  *         directly to the underlying stream as is the case now. 
  contents: read

jobs: *
  deploy: *         Example usage: 
    name: Deploy
    runs-on: ubuntu-latest
    environment: production

    steps: *
    - name: Checkout
      uses: actions/checkout@v3

    - name: Configure AWS credentials
      uses: aws-actions/configure-aws-credentials@v1
      with: *         CSVWriter sw(cout);               // send output to cout
        aws-access-key-id: *         sw <<"Bandwidth" << 0.5; // value ignored
        aws-secret-access-key:  *         sw <<"Latency" << 5;     // value ignored
        aws-region: *         sw.finalize();                      // flush the header 

    - name: Login to Amazon ECR
      id: login-ecr
      uses: aws-actions/amazon-ecr-login@v1

    - name: Build, tag, and push image to Amazon ECR
      id: build-image
      env:  *         sw <<"Bandwidth" << 1.5; // field name ignored
        ECR_REGISTRY:  *         sw <<"Latency" << 15;     // field name ignored
        IMAGE_TAG:  *         sw.finalize();                                                         // values printed to csv line
      run: |
        # Build a docker container and
        # push it to ECR so that it can
        # be deployed to ECS.
        docker build -t  *         sw <<"Bandwidth" << 2.5; // field name ignored .
        docker push  *         sw <<"Latency" << 25;     // field name ignored
        echo  *         sw <<"Latency" << 25;     // field name ignored

    - name: Fill in the new image ID in the Amazon ECS task definition
      id: task-def
      uses: aws-actions/amazon-ecs-render-task-definition@v1
      with:  *         sw.finalize();                                                         // values printed to csv line
        task-definition: *
        container-name:   *         The output of this example will be:     
        image: *
    - name: Deploy Amazon ECS task definition
      uses: aws-actions/amazon-ecs-deploy-task-definition@v1
      with:  *         Bandwidth,Latency
        task-definition:  *         1.5,15
        service:  *         2.5,25
        cluster: * 
        wait-for-service-stability: */
