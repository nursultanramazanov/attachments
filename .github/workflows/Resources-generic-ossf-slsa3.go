# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow lets you generate SLSA provenance file for your project.
# The generation satisfies level 3 for the provenance requirements - see https://slsa.dev/spec/v0.1/requirements
# The project is an initiative of the OpenSSF (openssf.org) and is developed at
# https://github.com/slsa-framework/slsa-github-generator.
# The provenance file can be verified using https://github.com/slsa-framework/slsa-verifier.
# For more information about SLSA and how it improves the supply-chain, visit slsa.dev.

name: SLSA generic generator
on: { // Copyright 2023-2024 The Parca Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package analytics

import (
        "context"
        "math/rand"
        "strconv"
        "time"

        prometheus "buf.build/gen/go/prometheus/prometheus/protocolbuffers/go"
        log "github.com/sirupsen/logrus"
        "github.com/zcalusic/sysinfo"
)

type AnalyticsSender struct {
        client *Client

        machineID   string
        arch        string
        cpuCores    float64
        version     string
        si          sysinfo.SysInfo
        isContainer string
}

var letters = []rune("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

func randSeq(n int) string {
        //nolint:gosec
        r := rand.New(rand.NewSource(time.Now().UnixNano()))
        b := make([]rune, n)
        for i := range b {
                b[i] = letters[r.Intn(len(letters))]
        }
        return string(b)
}

func NewSender(
        client *Client,
        arch string,
        cpuCores int,
        version string,
        si sysinfo.SysInfo,
        isContainer bool,
) *AnalyticsSender {
        return &AnalyticsSender{
                client:      client,
                machineID:   randSeq(20),
                arch:        arch,
                cpuCores:    float64(cpuCores),
                version:     version,
                si:          si,
                isContainer: strconv.FormatBool(isContainer),
        }
}

func (s *AnalyticsSender) Run(ctx context.Context) {
        ticker := time.NewTicker(time.Second * 10)
        defer ticker.Stop()

        for {
                select {
                case <-ctx.Done():
                        return
                case <-ticker.C:
                        now := FromTime(time.Now())
                        if err := s.client.Send(ctx, &prometheus.WriteRequest{
                                Timeseries: []*prometheus.TimeSeries{{
                                        Labels: []*prometheus.Label{{
                                                Name:  "__name__",
                                                Value: "parca_agent_info",
                                        }, {
                                                Name:  "machine_id",
                                                Value: s.machineID,
                                        }, {
                                                Name:  "arch",
                                                Value: s.arch,
                                        }, {
                                                Name:  "version",
                                                Value: s.version,
                                        }, {
                                                Name:  "kernel_version",
                                                Value: s.si.Kernel.Version,
                                        }, {
                                                Name:  "kernel_osrelease",
                                                Value: s.si.Kernel.Release,
                                        }, {
                                                Name:  "os_name",
                                                Value: s.si.OS.Name,
                                        }, {
                                                Name:  "os_vendor",
                                                Value: s.si.OS.Vendor,
                                        }, {
                                                Name:  "os_version",
                                                Value: s.si.OS.Version,
                                        }, {
                                                Name:  "os_release",
                                                Value: s.si.OS.Release,
                                        }, {
                                                Name:  "is_container",
                                                Value: s.isContainer,
                                        }},
                                        Samples: []*prometheus.Sample{{
                                                Value:     1,
                                                Timestamp: now,
                                        }},
                                }, {
                                        Labels: []*prometheus.Label{{
                                                Name:  "__name__",
                                                Value: "parca_agent_cpu_cores",
                                        }, {
                                                Name:  "machine_id",
                                                Value: s.machineID,
                                        }},
                                        Samples: []*prometheus.Sample{{
                                                Value:     s.cpuCores,
                                                Timestamp: now,
                                        }},
                                }},
                        }); err != nil {
                                log.Debugf("failed to send analytics: %v", err)
                        }
                }
        }
}

func FromTime(t time.Time) int64 {
        return t.Unix()*1000 + int64(t.Nanosecond())/int64(time.Millisecond)
 }

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}  
  workflow_dispatch: { // Copyright 2023-2024 The Parca Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package analytics

import (
        "bufio"
        "bytes"
        "context"
        "fmt"
        "io"
        "net/http"
        "net/http/httptrace"
        "os"
        "time"

        prometheus "buf.build/gen/go/prometheus/prometheus/protocolbuffers/go"
        "github.com/gogo/protobuf/proto"
        "github.com/golang/snappy"
        "go.opentelemetry.io/contrib/instrumentation/net/http/httptrace/otelhttptrace"
        "go.opentelemetry.io/otel/trace"
)

const (
        maxErrMsgLen = 1024
        defaultURL   = "https://analytics.parca.dev/api/v1/write"
)

type Client struct {
        tp     trace.TracerProvider
        tracer trace.Tracer

        client    *http.Client
        urlString string
        userAgent string
        timeout   time.Duration

        buf  []byte
        pBuf *proto.Buffer
}

func NewClient(
        tp trace.TracerProvider,
        client *http.Client,
        userAgent string,
        timeout time.Duration,
) *Client {
        analyticsURL := defaultURL
        customAnalyticsURL := os.Getenv("ANALYTICS_URL")
        if customAnalyticsURL != "" {
                analyticsURL = customAnalyticsURL
        }

        return &Client{
                tp:     tp,
                tracer: tp.Tracer("parca/analytics"),

                client: client,

                urlString: analyticsURL,
                userAgent: userAgent,
                timeout:   timeout,

                pBuf: proto.NewBuffer(nil),
                buf:  make([]byte, 1024),
        }
}

func (c *Client) Send(ctx context.Context, wreq *prometheus.WriteRequest) error {
        ctx, span := c.tracer.Start(ctx, "Send", trace.WithSpanKind(trace.SpanKindClient))
        defer span.End()

        c.pBuf.Reset()
        err := c.pBuf.Marshal(wreq)
        if err != nil {
                return err
        }

        // snappy uses len() to see if it needs to allocate a new slice. Make the
        // buffer as long as possible.
        if c.buf != nil {
                c.buf = c.buf[0:cap(c.buf)]
        }
        c.buf = snappy.Encode(c.buf, c.pBuf.Bytes())

        return c.sendReq(ctx, c.buf)
}

// Store sends a batch of samples to the HTTP endpoint, the request is the proto marshaled
// and encoded bytes from codec.go.
func (c *Client) sendReq(ctx context.Context, req []byte) error {
        ctx, span := c.tracer.Start(ctx, "sendReq", trace.WithSpanKind(trace.SpanKindClient))
        defer span.End()

        httpReq, err := http.NewRequest(http.MethodPost, c.urlString, bytes.NewReader(req))
        if err != nil {
                // Errors from NewRequest are from unparsable URLs, so are not
                // recoverable.
                return err
        }

        httpReq.Header.Add("Content-Encoding", "snappy")
        httpReq.Header.Set("Content-Type", "application/x-protobuf")
        httpReq.Header.Set("User-Agent", c.userAgent)
        httpReq.Header.Set("X-Prometheus-Remote-Write-Version", "0.1.0")

        ctx, cancel := context.WithTimeout(ctx, c.timeout)
        defer cancel()
        ctx = httptrace.WithClientTrace(ctx, otelhttptrace.NewClientTrace(ctx, otelhttptrace.WithTracerProvider(c.tp)))

        httpResp, err := c.client.Do(httpReq.WithContext(ctx))
        if err != nil {
                return fmt.Errorf("error sending request: %w", err)
        }
        defer func() {
                io.Copy(io.Discard, httpResp.Body) //nolint:errcheck
                httpResp.Body.Close()
        }()

        if httpResp.StatusCode/100 != 2 {
                scanner := bufio.NewScanner(io.LimitReader(httpResp.Body, maxErrMsgLen))
                line := ""
                if scanner.Scan() {
                        line = scanner.Text()
                }
                err = fmt.Errorf("server returned HTTP status %s: %s", httpResp.Status, line)
        }
        return err
} 

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      У вас нет прав на данное действие.
    {% endblock message %}
  </p>
{% endblock body %}  
  release: { // Copyright 2022-2024 The Parca Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package config

import (
        "errors"
        "fmt"
        "os"

        "github.com/prometheus/prometheus/model/relabel"
        "gopkg.in/yaml.v3"
)

var ErrEmptyConfig = errors.New("empty config")

// Config holds all the configuration information for Parca Agent.
type Config struct {
        RelabelConfigs []*relabel.Config `yaml:"relabel_configs,omitempty"`
}

func (c Config) String() string {
        b, err := yaml.Marshal(c)
        if err != nil {
                return fmt.Sprintf("<error creating config string: %s>", err)
        }
        return string(b)
}

// Load parses the YAML input s into a Config.
func Load(b []byte) (*Config, error) {
        if len(b) == 0 {
                return nil, ErrEmptyConfig
        }

        // TODO: Sanitize the input labels if it becomes a problem for the user.
        // https://github.com/prometheus/prometheus/blob/1bfb3ed062e99bd3c74e05d9ff9a7fa4e30bbe21/util/strutil/strconv.go#L51
        cfg := &Config{}

        if err := yaml.Unmarshal(b, cfg); err != nil {
                return nil, fmt.Errorf("unmarshaling YAML: %w", err)
        }

        return cfg, nil
}

// LoadFile parses the given YAML file into a Config.
func LoadFile(filename string) (*Config, error) {
        content, err := os.ReadFile(filename)
        if err != nil {
                return nil, err
        }
        cfg, err := Load(content)
        if err != nil {
                return nil, fmt.Errorf("parsing YAML file %s: %w", filename, err)
        }
        return cfg, nil
        }

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Запрашиваемый ресурс не найден.
    {% endblock message %}
  </p>
{% endblock body %}   
    types: { // Copyright 2022-2024 The Parca Authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package config

import (
        "testing"

        "github.com/prometheus/common/model"
        "github.com/prometheus/prometheus/model/relabel"
        "github.com/stretchr/testify/require"
)

func TestLoad(t *testing.T) {
        t.Parallel()
        tests := []struct {
                name    string
                input   string
                want    *Config
                wantErr bool
        }{
                {
                        input:   ``,
                        want:    nil,
                        wantErr: true,
                },
                {
                        input: `# comment`,
                        want: &Config{
                                RelabelConfigs: nil,
                        },
                },
                {
                        input: `relabel_configs: []`,
                        want: &Config{
                                RelabelConfigs: []*relabel.Config{},
                        },
                },
                {
                        input: `relabel_configs:
- source_labels: [systemd_unit]
  regex: ""
  action: drop
`,
                        want: &Config{
                                RelabelConfigs: []*relabel.Config{
                                        {
                                                SourceLabels: model.LabelNames{"systemd_unit"},
                                                Separator:    ";",
                                                Regex:        relabel.MustNewRegexp(``),
                                                Replacement:  "$1",
                                                Action:       relabel.Drop,
                                        },
                                },
                        },
                },
                {
                        input: `relabel_configs:
- source_labels: [app]
  regex: coolaicompany-isolate-controller
  action: keep
`,
                        want: &Config{
                                RelabelConfigs: []*relabel.Config{
                                        {
                                                SourceLabels: model.LabelNames{"app"},
                                                Separator:    ";",
                                                Regex:        relabel.MustNewRegexp("coolaicompany-isolate-controller"),
                                                Replacement:  "$1",
                                                Action:       relabel.Keep,
                                        },
                                },
                        },
                },
                {
                        input: `"relabel_configs":
- "action": "keep"
  "regex": "parca-agent"
  "source_labels":
  - "app_kubernetes_io_name"
`,
                        want: &Config{
                                RelabelConfigs: []*relabel.Config{
                                        {
                                                SourceLabels: model.LabelNames{"app_kubernetes_io_name"},
                                                Separator:    ";",
                                                Regex:        relabel.MustNewRegexp("parca-agent"),
                                                Replacement:  "$1",
                                                Action:       relabel.Keep,
                                        },
                                },
                        },
                },
                {
                        input: `relabel_configs:
- action: keep
  regex: parca-agent
  source_labels:
    - app_kubernetes_io_name
`,
                        want: &Config{
                                RelabelConfigs: []*relabel.Config{
                                        {
                                                SourceLabels: model.LabelNames{"app_kubernetes_io_name"},
                                                Separator:    ";",
                                                Regex:        relabel.MustNewRegexp("parca-agent"),
                                                Replacement:  "$1",
                                                Action:       relabel.Keep,
                                        },
                                },
                        },
                },
                {
                        input: `relabel_configs:
- action: keep
  regex: parca-agent
  source_labels:
  - app.kubernetes.io/name
`,
                        wantErr: true,
                },
        }
        for _, tt := range tests {
                tt := tt
                t.Run(tt.name, func(t *testing.T) {
                        t.Parallel()
                        got, err := Load([]byte(tt.input))
                        if tt.wantErr {
                                require.Error(t, err)
                                return
                        }
                        require.NoError(t, err)
                        require.Equal(t, tt.want, got)
                })
        }
         }

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %} 

jobs: import { RoleService } from "../src/service/role-service";
import { UserService } from "../src/service/user-service";
import { CreateUserResponse, CHAUser, User } from "../src/types/user";
import { RLSService } from "../src/service/rls-service";
import { generatePassword } from "../src/utils/password.utils";
import { generateUsername } from "../src/utils/string.utils";
import { Logger } from "../src/utils/logger";
import { PermissionService } from "../src/service/permission-service";
import { UpdateRLSRequest } from "../src/types/rls";
import { ParsedRole, SupersetRole } from "../src/types/role";
import { RoleAdapter } from "../src/repository/role-adapter";
import fs from "fs";
import { readUsersFromFile } from "../src/repository/csv-util";
import { RedisService } from "../src/repository/redis-util";

// Initialize services
const roleService = new RoleService();

// Validation functions
function validateUserDetails(user: CHAUser): string[] {
  const errors: string[] = [];

  if (!user.first_name?.trim()) errors.push("First name is required");
  if (!user.last_name?.trim()) errors.push("Last name is required");
  if (!user.email?.trim()) errors.push("Email is required");
  if (user.email && !user.email.includes("@")) errors.push("Invalid email format");
  if (!user.chu?.trim()) errors.push("CHU code is required");

  return errors;
}

// create users
async function prepareUserData(csvUser: CHAUser, roles: ParsedRole[]): Promise<User | null> {
  const errors = validateUserDetails(csvUser);
  if (errors.length > 0) {
    Logger.error(`User ${csvUser.username} is invalid: ${errors.join(", ")}`);
    return null;
  }

  return {
    active: true,
    first_name: capitalizeFirstLetter(csvUser.first_name.trim()),
    last_name: capitalizeFirstLetter(csvUser.last_name.trim()),
    email: csvUser.email.trim(),
    username: generateUsername(csvUser.first_name, csvUser.last_name),
    password: csvUser.password.trim().replace(/^["']|["']$/g, '') || generatePassword(10),
    roles: await getRoleIds(csvUser.chu, roles)
  };
}

function capitalizeFirstLetter(str: string): string {
  if (!str) return str;
  return str.charAt(0).toUpperCase() + str.slice(1).toLowerCase();
}

async function getRoleIds(chuCode: string, existingRoles: ParsedRole[]): Promise<number[]> {
  try {
    const chuCodes = roleService.getChuCodes(chuCode);
    Logger.info(`Processing roles for CHU codes: ${chuCodes.join(', ')}`);

    const supersetRoles = roleService.matchRoles(chuCode, existingRoles);
    Logger.info(`Found ${supersetRoles.length} existing roles out of ${chuCodes.length} required roles`);

    const permissions = await getChaPermissions();
    const roleIds: number[] = [];

    // Case 1: No existing roles found
    if (supersetRoles.length === 0) {
      Logger.info('Creating all roles as none exist');
      const newRoleIds = await createRoles(chuCode, permissions);
      return newRoleIds;
    }

    // Case 2: Some roles missing
    if (supersetRoles.length !== chuCodes.length) {
      const missingChuCodes = chuCodes.filter(code => 
        !supersetRoles.some(role => role.name.startsWith(code))
      );

      Logger.info(`Creating ${missingChuCodes.length} missing roles for CHU codes: ${missingChuCodes.join(', ')}`);
      const newRoleIds = await createRoles(missingChuCodes.join(','), permissions);
      roleIds.push(...newRoleIds);
    }

    // Case 3: Update existing roles
    Logger.info(`Updating permissions for ${supersetRoles.length} existing roles`);
    await updateRolesAndPolicies(supersetRoles, permissions);
    roleIds.push(...supersetRoles.map(role => role.id));

    Logger.info(`Successfully processed ${roleIds.length} total roles`);
    return roleIds;
  } catch (error) {
    Logger.error('Failed to process roles');
    throw error;
  }
}

async function createUsers(users: CHAUser[]): Promise<CreateUserResponse[]> {
  const roles = await roleService.getRoles();
  if (roles.length === 0) {
    throw new Error("No roles found");
  }

  const userPromises = users.map(user => prepareUserData(user, roles));
  const preparedUsers = (await Promise.all(userPromises)).filter((user): user is User => user !== null);

  if (preparedUsers.length === 0) {
    throw new Error("No valid users found");
  }

  return new UserService().createUserOnSuperset(preparedUsers);
}

// create roles
async function createRoles(chuCodes: string, permissions: number[]): Promise<number[]> {
  const codes = chuCodes.split(',').map((code) => code.trim());

  // return a map of code to role name
  const detailsMap = new Map<string, string>();
  for (const code of codes) {
    detailsMap.set(code, `cha_${code}`);
  }
  Logger.info(`Creating roles: ${Array.from(detailsMap.values()).join(', ')}`);

  // create roles from map
  const roles = await roleService.createRoles(Array.from(detailsMap.values()));
  if (roles.length > 0) {
    // Create a map of role name to role ID for quick lookup
    const roleNameToId = new Map(roles.map(role => [role.name, role.id]));

    const updatedRoles = await roleService.updateRolePermissions(roles, permissions);

    // Create RLS policies with associated roles
    const rlsPolicies = Array.from(detailsMap.entries()).map(([code, roleName]) => {
      const roleId = roleNameToId.get(roleName);
      if (!roleId) {
        Logger.warning(`No role ID found for role name: ${roleName}`);
        return null;
      }
      return {
        code,
        roleIds: [roleId]
      };
    }).filter((policy): policy is NonNullable<typeof policy> => policy !== null);

    await createRLSPolicies(rlsPolicies);

    return updatedRoles;
  }

  return []
}

async function getChaPermissions(): Promise<number[]> {
  const permissionService = new PermissionService();
  return await permissionService.fetchBasePermissions();
}

// create RLS policies for new roles
async function createRLSPolicies(policies: Array<{ code: string, roleIds: number[] }>) {
  const rlsService = new RLSService();
  const baseRLSPolicy = await rlsService.fetchBaseRLS(rlsService.BASE_CHU_RLS_ID);

  const requests: UpdateRLSRequest[] = policies.map(({ code, roleIds }) => {
    const rlsName = `cha_${code}`;
    Logger.info(`Creating RLS policy ${rlsName} for roles: ${roleIds.join(', ')}`);

    return {
      name: rlsName,
      group_key: baseRLSPolicy.group_key,
      clause: `chu_code=\'${code}\'`,
      description: 'RLS Policy for CHU ' + code,
      filter_type: baseRLSPolicy.filter_type,
      roles: roleIds,
      tables: baseRLSPolicy.tables.map(table => table.id)
    };
  });

  await Promise.all(requests.map(request => rlsService.createRLSPolicy(request)));
}

async function updateRolesAndPolicies(supersetRoles: SupersetRole[], permissions: number[]) {
  const updatedRoles = await roleService.updateRolePermissions(supersetRoles, permissions);
  Logger.info(`Updated roles: ${updatedRoles}`);

  const rlsService = new RLSService();
  const baseRLSPolicy = await rlsService.fetchBaseRLS(rlsService.BASE_CHU_RLS_ID);
  Logger.info('Fetched base RLS policy');

  // create RLS policies with associated roles
  const rlsPolicies = supersetRoles.map(role => {
    const chuCode = new RoleAdapter().extractCHUCode(role.name);

    return {
      name: `cha_${chuCode}`,
      group_key: baseRLSPolicy.group_key,
      clause: `chu_code=\'${chuCode}\'`,
      description: 'RLS Policy for CHU ' + chuCode,
      filter_type: baseRLSPolicy.filter_type,
      roles: [role.id],
      tables: baseRLSPolicy.tables.map(table => table.id)
    };
  });
  Logger.info(`Creating RLS policies for roles: ${supersetRoles.map(role => role.name).join(', ')}`);

  await Promise.all(rlsPolicies.map(request => rlsService.createRLSPolicy(request)));
  Logger.info(`Created RLS policies for roles: ${supersetRoles.map(role => role.name).join(', ')}`);
}

// generate a csv of user details
async function generateCSV(users: CreateUserResponse[], filePath: string) {
  const csv = users.map(user => {
    return {
      username: user.result.username,
      password: user.result.password,
      first_name: user.result.first_name,
      last_name: user.result.last_name,
      email: user.result.email
    };
  });

  Logger.info(`Generating CSV file at ${filePath} with ${csv.length} users`);

  // write csv to file
  // Add headers and convert to CSV string
  const headers = ['username', 'password', 'first name', 'last name', 'email'].join(',');
  const csvRows = csv.map(row => Object.values(row).join(','));
  const csvString = [headers, ...csvRows].join('\n');
  const fileName = getFileName(filePath);

  try {
    // Write file synchronously to ensure it's written before continuing
    fs.writeFileSync(fileName, csvString, 'utf8');
    Logger.success(`CSV file created successfully at ${fileName}`);
  } catch (err) {
    Logger.error(`Failed to write CSV file: ${err}`);
    throw err;
  }
}

// putting it all together
async function createUsersFromCSV(filePath: string) {
  try {
    const users = await readUsersFromFile<CHAUser>(filePath);
    Logger.info(`Found ${users.length} users in CSV file`);

    const createdUsers = await createUsers(users);
    Logger.info(`Created ${createdUsers.length} users`);

    if(createdUsers.length === 0) {
      throw new Error('No users created');
    }

    await generateCSV(createdUsers, filePath);
    Logger.success('Process completed successfully');
  } catch (error) {
    Logger.error(`Process failed: ${error}`);
    throw error;
  }
}

// generate a filename from file path by suffixing with -users
const getFileName = (filePath: string) => {
  return filePath.split('.')[0] + '-users.csv';
}

// execute the script
createUsersFromCSV(process.argv[2])
  .catch((error) => {
    Logger.error('Script failed:', error);
  })
  .finally(async () => {
    try {
      // Clean up Redis connection after all operations are complete
      await RedisService.disconnect();
    } finally {
      process.exit(0); // Exit with success code if everything completed
    }
  }); 
  build: import path from 'path';
import { Logger } from '../src/utils/logger';
import { generatePassword } from '../src/utils/password.utils';
import { PermissionService } from '../src/service/permission-service';
import { readUsersFromFile } from '../src/repository/csv-util';
import { RoleService } from '../src/service/role-service';
import { RLSService } from '../src/service/rls-service';
import { UpdateRLSRequest } from '../src/types/rls';
import { User } from '../src/types/user';
import { UserService } from '../src/service/user-service';

const CSV_FILENAME = path.join(__dirname, '../data/subcounty-users.csv');

const BASE_SUBCOUNTY_ROLE_ID = 3585;

type SubcountyAccountRequest = {
  county: string;
  subcounty?: string;
  email: string;
};

type UserCreationResult = {
  username: string;
  password: string;
  email: string;
};

async function getBasePermissions(): Promise<number[]> {
  const permissionService = new PermissionService();
  return await permissionService.getPermissionsByRoleId(BASE_SUBCOUNTY_ROLE_ID);
}

function subcountyUsername(request: SubcountyAccountRequest): string {
  const transform = (val: string) => val.toLowerCase().replace(/ /g, '_').trim();
  let result = `${transform(request.county)}`;
  if (request.subcounty) {
    result += `_${transform(request.subcounty)}`;
  }

  return result;
}

async function createRole(subcountyRequest: SubcountyAccountRequest): Promise<number> {
  Logger.info('Creating role from base...');
  const basePermissionIds = await getBasePermissions();
  const roleService = new RoleService();
  const roleName = `subcounty_${subcountyUsername(subcountyRequest)}`;
  const roles = await roleService.createRoles([roleName]);
  await roleService.updateRolePermissions(roles, basePermissionIds);
  return roles[0].id;
}

function getRLSClause(subcountyRequest: SubcountyAccountRequest): string {
  let clause = `county='${subcountyRequest.county}'`;
  if (subcountyRequest.subcounty) {
    clause += ` AND subcounty_name='${subcountyRequest.subcounty}'`;
  }

  return clause;
}

async function createRLSPolicy(subcountyRequest: SubcountyAccountRequest, roleId: number) {
  Logger.info('Creating an RLS...');
  const rlsService = new RLSService();
  const clause = getRLSClause(subcountyRequest);

  const supervisorTables = await rlsService.fetchSupervisorTables();

  const rlsRequest: UpdateRLSRequest = {
    name: `subcounty_${subcountyUsername(subcountyRequest)}`,
    group_key: '',
    clause,
    description: `RLS Policy for Subcounty ${subcountyRequest.county}.${subcountyRequest.subcounty || ''}`,
    filter_type: 'Regular',
    roles: [roleId],
    tables: [241, ...supervisorTables],
  };

  const rlsResponse = await rlsService.createRLSPolicy(rlsRequest);
  Logger.info(`Resulting RLS ID: ${rlsResponse.id}`);
}

async function createUser(subcountyRequest: SubcountyAccountRequest, roleId: number): Promise<UserCreationResult> {
  const user: User = {
    active: true,
    first_name: subcountyRequest.subcounty || '',
    last_name: subcountyRequest.county,
    email: subcountyRequest.email,
    username: subcountyUsername(subcountyRequest),
    password: generatePassword(10),
    roles: [roleId]
  };

  const userService = new UserService();
  const userResponses = await userService.createUserOnSuperset([user]);
  const { username, password } = userResponses[0].result;
  return { 
    email: subcountyRequest.email,
    username, 
    password,
  };
}

function validateAccountRequests(subcountyAccountRequests: SubcountyAccountRequest[]): void {
  for (const subcountyRequest of subcountyAccountRequests) {
    if (!subcountyRequest.county?.trim()) {
      throw Error(`County information is required`);
    }

    if (!subcountyRequest.email?.trim()) {
      throw Error(`Email information is required`);
    }
  }
}

(async function() {
  const subcountyRequests = await readUsersFromFile<SubcountyAccountRequest>(CSV_FILENAME);

  validateAccountRequests(subcountyRequests);

  const userResults: UserCreationResult[] = [];
  for (const subcountyRequest of subcountyRequests) {
    Logger.info(`Creating ${subcountyUsername(subcountyRequest)}:`);
    const roleId = await createRole(subcountyRequest);
    await createRLSPolicy(subcountyRequest, roleId);
    const userResult = await createUser(subcountyRequest, roleId);
    userResults.push(userResult);
  }

  console.table(userResults);
})(); 
    runs-on: import { RLSService } from "../src/service/rls-service";
import { Logger } from "../src/utils/logger";

/**
 * Compares two arrays of table IDs to check if they contain the same tables
 */
function hasSameTables(tables1: number[], tables2: number[]): boolean {
  if (tables1.length !== tables2.length) return false;
  const sortedTables1 = [...tables1].sort();
  const sortedTables2 = [...tables2].sort();
  return sortedTables1.every((table, index) => table === sortedTables2[index]);
}

async function updateCHURLSPolicies() {
  try {
    Logger.info("Starting CHU RLS update process...");

    // Initialize services
    const rlsService = new RLSService();

    // Step 1: Fetch all RLS policies
    Logger.info("Fetching all RLS policies...");
    const allPolicies = await rlsService.fetchRLSPolicies();
    Logger.info(`Found ${allPolicies.length} RLS policies`);

    // Step 2: Filter policies by chu_code and exclude base RLS
    Logger.info("Filtering CHU RLS policies...");
    const chuPolicies = await rlsService.filterByGroupKey(allPolicies, 'chu_code');
    const policiesToUpdate = chuPolicies.filter(policy => policy.id !== rlsService.BASE_CHU_RLS_ID);

    Logger.info(`Processing RLS policies:
      - Total policies: ${allPolicies.length}
      - CHU policies: ${chuPolicies.length}
      - Policies to update: ${policiesToUpdate.length}`);

    if (policiesToUpdate.length === 0) {
      Logger.warning("No CHU RLS policies found to update");
      return;
    }

    // Step 3: Fetch base CHU tables
    Logger.info("Fetching base CHU tables...");
    const baseTables = await rlsService.fetchSupervisorTables();
    Logger.info(`Found ${baseTables.length} base CHU tables`);

    if (baseTables.length === 0) {
      Logger.error("No base CHU tables found. Cannot proceed with updates.");
      return;
    }

    // Step 4: Filter out policies that already have the correct tables
    Logger.info("Checking which policies need table updates...");
    const policiesToActuallyUpdate = policiesToUpdate.filter(policy => {
      const policyTableIds = policy.tables.map(table => table.id);
      const needsUpdate = !hasSameTables(policyTableIds, baseTables);

      if (!needsUpdate) {
        Logger.info(`Skipping policy ${policy.id} - tables already match base tables`);
      }

      return needsUpdate;
    });

    Logger.info(`Filtered policies requiring updates:
      - Original count: ${policiesToUpdate.length}
      - Need updates: ${policiesToActuallyUpdate.length}
      - Already up to date: ${policiesToUpdate.length - policiesToActuallyUpdate.length}`);

    if (policiesToActuallyUpdate.length === 0) {
      Logger.success("All CHU RLS policies are already up to date!");
      return;
    }

    // Step 5: Update the policies that need updating
    Logger.info(`Updating ${policiesToActuallyUpdate.length} CHU RLS policies with base CHU tables...`);
    const results = await rlsService.updateRLSTables(baseTables, policiesToActuallyUpdate);
    Logger.success(`Successfully updated ${results.length} CHU RLS policies`);

    Logger.success("CHU RLS update process completed successfully!");
  } catch (error: any) {
    Logger.error(error?.message || 'An unknown error occurred');
    process.exit(1);
  }
}

// Execute the script
updateCHURLSPolicies().catch(error => {
  Logger.error(error?.message || 'An unknown error occurred');
  process.exit(1);
}); 
    outputs: import { RLSService } from "../src/service/rls-service";
import { Logger } from "../src/utils/logger";

async function updateCountyRLSPolicies() {
  try {
    Logger.info("Starting county RLS update process...");

    // Initialize services
    const rlsService = new RLSService();

    // Step 1: Fetch all RLS policies
    Logger.info("Fetching all RLS policies...");
    const allPolicies = await rlsService.fetchRLSPolicies();
    Logger.info(`Found ${allPolicies.length} RLS policies`);

    // Step 2: Filter policies by county_name and exclude base RLS
    Logger.info("Filtering county RLS policies...");
    const countyPolicies = await rlsService.filterByGroupKey(allPolicies, 'county_name');
    const policiesToUpdate = countyPolicies.filter(policy => policy.id !== rlsService.BASE_COUNTY_RLS_ID);

    Logger.info(`Processing RLS policies:
      - Total policies: ${allPolicies.length}
      - County policies: ${countyPolicies.length}
      - Policies to update: ${policiesToUpdate.length}`);

    if (policiesToUpdate.length === 0) {
      Logger.warning("No county RLS policies found to update");
      return;
    }

    // Step 3: Fetch base tables and update county RLS policies
    Logger.info("Fetching base tables...");
    const baseTables = await rlsService.fetchBaseCountyTables();
    Logger.info(`Found ${baseTables.length} base tables`);

    if (baseTables.length === 0) {
      Logger.error("No base tables found. Cannot proceed with updates.");
      return;
    }

    Logger.info("Updating county RLS policies with base tables...");
    const results = await rlsService.updateRLSTables(baseTables, policiesToUpdate);
    Logger.success(`Successfully updated ${results.length} county RLS policies`);

    Logger.success("County RLS update process completed successfully!");
  } catch (error) {
    Logger.error(error);
    process.exit(1);
  }
}

// Execute the script
updateCountyRLSPolicies().catch(error => Logger.error(error)); 
      digests: ${{ steps.hash.outputs.digests }}

    steps: import { PermissionService } from "../src/service/permission-service";
import { Logger } from "../src/utils/logger";

async function updateRolePermissions(baseRoleId: number, roleToUpdate: number) {
  try {
    const permissionService = new PermissionService();
    const baseRolePermissions = await permissionService.getPermissionsByRoleId(baseRoleId);
    const roleToUpdatePermissions = await permissionService.getPermissionsByRoleId(roleToUpdate);
    const updatedRolePermissions = Array.from(new Set([...baseRolePermissions, ...roleToUpdatePermissions]));
    await permissionService.updatePermissions(roleToUpdate, { permission_view_menu_ids: updatedRolePermissions });

    Logger.info(`Updated permissions for role ${roleToUpdate}`);

    process.exit(0);
  } catch (error) {
    Logger.error('An unknown error occurred');
    process.exit(1);
  }
}

updateRolePermissions(3412, 3585).catch(error => Logger.error(error)); 
      - uses: .body-wrapper {
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

      # ========================================================
      #
      # Step 1: {% extends 'base.html.twig' %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/confirm.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block title %}Testhub &mdash; пропущенные вопросы{% endblock title %}

{% block body %}
  <section class="skipped-container">
    <h1>Есть пропущенные вопросы</h1>
    <p class="alert alert-warning">
      Вы дали ответы не на все вопросы. Вопросов без ответа:
      <strong>{{ skippedCount }}</strong>
    </p>
    <p>Вернуться к пропущенным вопросам, или завершить тест?</p>
    <p class="remains text-muted">
      Осталось времени: <time>{{ attempt.timeLeft|formatTimeLeft }}</time>
    </p>
    <div class="skipped-buttons">
      <a href="{{ path('question',
          {'attemptID':attempt.id, 'questionNumber':firstSkipped}) }}"
         class="btn btn-success pull-left">Вернуться</a>
      <a href="{{ path('result', {'attemptID':attempt.id}) }}"
         class="btn btn-warning pull-right">Завершить</a>
    </div>
  </section>
{% endblock body %} 
      #
      # ========================================================
      - name: {% extends 'test/preface.html.twig' %}

{% block preface %}
  <h1>{{ attempt.test.title }}</h1>
  <section class="test-container">
    <p class="test-info">
      У Вас есть активная попытка пройти данный тест. Вы не можете начать новую
      попытку, пока не завершите предыдущую.
    </p>
    <p>Неотвеченных вопросов:
        <span class="questions-unanswered">{{ unanswered }}</span>
    </p>
    <p>Осталось времени: <time>{{ attempt.timeLeft|formatTimeLeft }}</time></p>
  </section>
  <div class="button-wrapper clearfix">
    <a href="#" class="btn btn-warning pull-left">Завершить тест</a>
    <a href="#" class="btn btn-success pull-right">Продолжить тест</a>
  </div>
{% endblock preface %} 
        run: {% extends 'base.html.twig' %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/index.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block body %}
  <div class="col-md-8 col-xs-12">
    <h1>Последние добавленные тесты</h1>
    <section class="top-tests list-group row">
      {% for test in tests %}
        <div class="test-container list-group-item clearfix">
          <div class="col-xs-10">
            <h3 class="test-title">{{ test.getTitle() }}</h3>
            <div class="test-tags">
              {% for tag in test.getTags() %}
                <a href="#" class="tag-name">{{ tag.getName() }}</a>
              {% endfor %}
            </div>
          </div>
          <div class="start-test-btn col-xs-2">
            <a href="#" class="btn btn-default start-test-link pull-right">&rarr;</a>
          </div>
        </div>
      {% endfor %}
    </section>
    <a href="#" class="lead show-all">Смотреть все тесты &rarr;</a>
  </div>
  <nav class="col-md-4 col-xs-12 nav-about">
    <h2>О сайте</h2>
    <article class="about">
      <p>TestHub &mdash; это сервис, который позволяет Вам легко создавать тесты для
      проверки знаний и просматривать результаты в удобном интерфейсе.</p>
      <p>Для создания и прохождения теста не требуется регистрация, но мы советуем
      зарегистрироваться, так как в этом случае вы легко сможете управлять своими
      тестами.</p>
      <p>Присоединяйтесь к сообществу TestHub!</p>
    </article>
    <a href="#" class="btn btn-success create-test-btn">Создать тест</a>
  </nav>
{% endblock body %} 
            

      # ========================================================
      #
      # Step 2: {% extends 'base.html.twig' %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/preface.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block body %}
  <div class="col-md-8 col-xs-12">
    {% block preface %}
      <h1>{{ test.title }}</h1>
      <section class="test-container">
        {% if test.description is not null %}
          <p class="test-description">Краткое описание теста...</p>
        {% endif %}
        <p class="test-info">
          В тесте {{ wordCase(questionsCount, ['вопрос', 'вопроса', 'вопросов']) }},
          за которые можно набрать до {{ maxMark }} баллов. На весь тест дается
          {{ wordCase(test.timeLimit, ['минута', 'минуты', 'минут']) }}.
        </p>
        <div class="test-author">Автор: <a href="">{{ test.author.name }}</a></div>
      </section>
      <div class="button-wrapper">
        <form name="formStart" action="/app_dev.php/test/{{ test.id }}/start" method="POST">
          <input type="submit" value="Начать тест"
                 class="btn btn-success btn-lg test-start-button">
        </form>
      </div>
    {% endblock preface %}
  </div>
{% endblock body %} 
      #         
      #         
      #         
      #
      # ========================================================
      - name: {% extends 'base.html.twig' %}

{% block title %}Testhub &mdash; вопрос теста{% endblock title %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/question.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block body %}
  <section class="question-container col-md-8 col-sm-12">
    <h1>{{ question.test.title }}</h1>
    <div class="panel panel-default">
      <div class="panel-heading question-number">
        Вопрос {{ question.sequenceNumber }} из {{ question.test.questions.count }}
      </div>
      <div class="panel-body question-description-container">
        <div class="question-description">{{ question.description }}</div>
        {% if attempt.timeLeft > 0 %}
          <div class="time-left text-muted">
            Осталось времени: <time>{{ attempt.timeLeft|formatTimeLeft }}</time>
          </div>
        {% endif %}
        <hr>
        {% if question.type is same as('text') %}
          {% include 'test/text_question.html.twig' %}
        {% elseif question.type is same as('decimal') %}
          {% include 'test/text_question.html.twig' %}
        {% elseif question.type is same as('single') %}
          {% include 'test/variant_question.html.twig' %}
        {% elseif question.type is same as('multiple') %}
          {% include 'test/variant_question.html.twig' %}
        {% endif %}
      </div>
    </div>
  </section>
{% endblock body %} 
        id: {% extends 'base.html.twig' %}

{% block title %}Testhub &mdash; результат теста{% endblock title %}

{% block stylesheets %}
  {{ parent() }}
  {% stylesheets "%kernel.root_dir%/Resources/css/test/result.css" %}
    <link rel="stylesheet" href="{{ asset_url }}">
  {% endstylesheets %}
{% endblock stylesheets %}

{% block body %}
  <div class="col-md-8 col-xs-12">
    <section class="test-result-section">
      <h1>Тест завершен</h1>
      <p>Набрано баллов: <span>{{ mark }}</span>
        из максимальных  <span>{{ maxMark }}</span>
        <span>({{ percentage(mark, maxMark) }})</span>
      </p>
      <p>Правильных ответов: <span>{{ correct }}</span>
        на <span>{{ wordCase(qCount, ['вопрос', 'вопроса', 'вопросов']) }}</span>
        <span>({{ percentage(correct, qCount) }})</span>
      </p>
    </section>
  </div>
{% endblock body %} 
        run: {{ form_start(form, {'attr': {'novalidate':'novalidate'}}) }}
  {{ form_label(form.answer, 'Ответ: ') }}
  {{ form_widget(form.answer, {'attr': {'class':'text-answer'}}) }}
{{ form_end(form) }}    
          



  provenance: {{ form_start(form, {'attr': {'novalidate':'novalidate'}}) }}
  {{ form_label(form.answer, 'Варианты: ') }}
  {{ form_widget(form.answer) }}
{{ form_end(form) }} 
    needs: <!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <title>{% block title %}TestHub &mdash; главная страница{% endblock %}</title>

    {% block stylesheets %}
      {% stylesheets
        "%kernel.root_dir%/../vendor/twbs/bootstrap/dist/css/bootstrap.min.css"
        "%kernel.root_dir%/Resources/css/common.css" %}
        <link rel="stylesheet" href="{{ asset_url }}">
      {% endstylesheets %}
    {% endblock stylesheets %}

    <link rel="icon" type="image/x-icon" href="{{ asset('favicon.ico') }}">
  </head>

  <body>

    <div class="body-wrapper">
      <header class="header clearfix">
        <div class="sitename col-md-8 col-sm-12">TestHub</div>
        {% block auth %}
          <div class="auth-section col-md-4 col-sm-12">
            <div class="login-link pull-right">
              <a href="#">Вход с паролем</a>
            </div>
            <div class="register-link pull-right">
              <a href="#">Регистрация</a>
            </div>
          </div>
        {% endblock auth %}
      </header>
      {% block body %}{% endblock body %}
    </div>

    {% block javascripts %}
      {% javascripts
        "%kernel.root_dir%/../vendor/components/jquery/jquery.min.js"
        "%kernel.root_dir%/../vendor/twbs/bootstrap/dist/js/bootstrap.min.js" %}
        <script src="{{ asset_url }}"></script>
      {% endjavascripts %}
    {% endblock javascripts %}
  </body>

</html>     
    permissions:
      actions: read   # To read the workflow path.
      id-token: write # To sign the provenance.
      contents: write # To add assets to a release.
    uses: slsa-framework/slsa-github-generator/.github/workflows/generator_generic_slsa3.yml@v1.4.0
    with:
      base64-subjects: "${{ needs.build.outputs.digests }}"
      upload-assets: true # Optional: Upload to a new release
