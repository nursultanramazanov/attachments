name: Rust

on: @startuml

bootstrap: Receiving all the records\n from the core node
local_replay: Replaying transactions\n that have been buffered locally\n during bootstrap
normal: Remote transactions are applied\ndirectly to the replica

[*] --> bootstrap
bootstrap --> local_replay : received bootstrap_complete
local_replay --> normal : reached the end of the local rlog

@enduml
  push: @startuml
scale 2000 width

participant "RLOG server" as server #ffc
participant "RLOG agent" as agent #ffc
participant "RLOG bootstrapper" as boot_serv #ffc

participant "RLOG replica" as repl #ccf
participant "RLOG replica\nimporter worker" as repl_imp #ccf
participant "bootstrap client" as boot_client #ccf

activate server
activate repl
activate repl_imp

group Agent initialization
== Probe the connection to minimize risk of double-subscription and negotiate protocol version ==
  repl -> server : probe
  repl <- server : {true, 0}
  note over repl: The protocol version\nshould be the same,\nabort connection if different

== Initiate connection ==
  repl -> server : {connect, LocalCheckpointTS}
  note over server : Spawn a new agent process\nunder the shard supervisor
  server -\\ agent : spawn(now() - SafeInterval)

  activate agent

  repl <- server : {need_bootstrap, AgentPID}
end

== Bootstrapping ==

group Bootstraper initialization
  hnote over repl : bootstrap

  repl -\\ boot_client : spawn()
  activate boot_client

  boot_client -> server : {bootstrap, self()}
  server -\\ boot_serv : spawn(RemotePid)
  activate boot_serv

  boot_serv -> boot_serv : mnesia:dirty_all_keys\nfor each table in shard

  server -> boot_client : {ok, Pid}
end

group Bootstrap
  note over boot_serv : Iterate through the\ncached keys
  loop
    boot_serv -> boot_client : {batch, [{Tab, Record}]}
    boot_client -> boot_client : import batch to the\ntable replica
    boot_serv <- boot_client : ok
  end

  note over agent : At the same time...

  loop
    server -> agent : {trans, SeqNo, TLOG}
    agent -> repl : '#entry{}'
    repl -> repl : cache batch to the local rlog
  end

  boot_serv -> boot_client : bootstrap_complete
  deactivate boot_serv
  boot_client -> repl : bootstrap_complete
  deactivate boot_client
end

group local_replay
  hnote over repl : local_replay

  note over repl : Iterate through the\ncached transactions

  loop
    server -> agent : {trans, SeqNo, TLOG}
    agent -> repl : '#entry{}'
    repl -> repl : cache batch in the local rlog

    repl -> repl : Import ops from the local rlog\nto the local replica
  end

  note over repl : Reached the end of\nthe local rlog
end


== Normal operation ==

hnote over repl : normal

loop
  server -> agent : {trans, SeqNo, TLOG}
  agent -> repl : '#entry{SeqNo}'
  server -> agent : {trans, SeqNo + 1, TLOG}
  agent -> repl : '#entry{SeqNo + 1}'
  server -> agent : {trans, SeqNo + 2, TLOG}
  agent -> repl : '#entry{SeqNo + 2}'
  repl -> repl_imp : {import_batch, [Entries 1 2 3]}
  repl_imp -> repl_imp : Import batch to the\nlocal replica
  repl <- repl_imp : ok
end

@enduml
    branches: [ "main" ]
  pull_request: -type(cluster() :: atom()).

-type(member_status() :: joining | up | healing | leaving | down).

-type(member_address() :: {inet:ip_address(), inet:port_number()}).

-record(member, {
          node   :: node(),
          addr   :: undefined | member_address(),
          guid   :: undefined | mria_guid:guid(),
          hash   :: undefined | pos_integer(),
          status :: member_status(),
          mnesia :: undefined | running | stopped | false,
          ltime  :: undefined | erlang:timestamp(),
          role   :: mria_rlog:role()
         }).

-type(member() :: #member{}).

-define(JOIN_LOCK_ID, {mria_sync_join, node()}).
    branches: [ "main" ]

env: #!/bin/bash
set -euxo pipefail

echo "Building "${tag}""

[ -d "${tmp_dir}" ] ||
    git clone "${root_dir}" "${tmp_dir}"

cd "${tmp_dir}"
git checkout "${tag}"

# Avoid rebuilding rocksdb:
mkdir -p "_checkouts"
pushd "_checkouts"
[ -L rocksdb ] ||
    ln -s "${root_dir}/_build/default/lib/rocksdb" rocksdb
popd

[ ! -z $(find . -name mria.app) ] ||
    rebar3 as test compile
  CARGO_TERM_COLOR: import http from 'http'
import fail from 'fail'
import console from 'console'
import { COUNTRYCODETONAME, COUNTRYAPI } from "./Data"

export {getCountryInfo, getCountryCodeToName}

function getCountryInfo(countryName, countryCode, debug) {
  let response
  if (debug) {
    return COUNTRYAPI
  } else {
    let url
    if (countryCode && !countryName) {
      url = encodeURI("https://restcountries.com/v3.1/alpha/".concat(countryCode))
    } else {
      url = encodeURI("https://restcountries.com/v3.1/name/".concat(countryName))
    }

    response = http.getUrl(url, { format: 'json', returnHeaders: true })

    if (response && response.status == 404) {
      if (debug) console.log("country not found error")
      throw fail.checkedError('Country not found', 'CountryNotFound', {})
    }
    if (!response || response.status != 200) {
      console.log("error: response = " + response)
      throw fail.checkedError('Bad API call', 'APIFailure', {})
    }
  }
  //return response - specify parsed to get parsed value from return object (needed with returnHeaders = true)
  return response.parsed
}

function getCountryCodeToName(debug) {
  let codeResponse
  if (debug) {
    return COUNTRYCODETONAME
  } else {
    codeResponse = http.getUrl("https://restcountries.com/v3.1/all?fields=name,cca3", { format: 'json', returnHeaders: true})
  }
  if (!codeResponse || codeResponse.status != 200) {
    // will gracefully degrade e.g, not show borders
    return null
  }
  //return response - specify parsed to get parsed value from return object (needed with returnHeaders = true)
  return codeResponse.parsed
}

jobs: export {OVERRIDES, COUNTRYAPI, COUNTRYCODETONAME}

let OVERRIDES = new Map([
    ["united states", "USA"],
    ["china", "CHN"],
    ["iran", "IRN"]
  ])

let COUNTRYAPI = [{"name":{"common":"Test Country - Peru","official":"Republic of Peru","nativeName":{"aym":{"official":"Piruw Suyu","common":"Piruw"},"que":{"official":"Piruw Ripuwlika","common":"Piruw"},"spa":{"official":"República del Perú","common":"Perú"}}},"tld":[".pe"],"cca2":"PE","ccn3":"604","cca3":"PER","cioc":"PER","independent":true,"status":"officially-assigned","unMember":true,"currencies":{"PEN":{"name":"Peruvian sol","symbol":"S/ "}},"idd":{"root":"+5","suffixes":["1"]},"capital":["Lima"],"altSpellings":["PE","Republic of Peru","República del Perú"],"region":"Americas","subregion":"South America","languages":{"aym":"Aymara","que":"Quechua","spa":"Spanish"},"translations":{"ara":{"official":"جمهورية بيرو","common":"بيرو"},"bre":{"official":"Republik Perou","common":"Perou"},"ces":{"official":"Peruánská republika","common":"Peru"},"cym":{"official":"Republic of Peru","common":"Peru"},"deu":{"official":"Republik Peru","common":"Peru"},"est":{"official":"Peruu Vabariik","common":"Peruu"},"fin":{"official":"Perun tasavalta","common":"Peru"},"fra":{"official":"République du Pérou","common":"Pérou"},"hrv":{"official":"Republika Peru","common":"Peru"},"hun":{"official":"Perui Köztársaság","common":"Peru"},"ita":{"official":"Repubblica del Perù","common":"Perù"},"jpn":{"official":"ペルー共和国","common":"ペルー"},"kor":{"official":"페루 공화국","common":"페루"},"nld":{"official":"Republiek Peru","common":"Peru"},"per":{"official":"جمهوری پرو","common":"پرو"},"pol":{"official":"Republika Peru","common":"Peru"},"por":{"official":"República do Peru","common":"Perú"},"rus":{"official":"Республика Перу","common":"Перу"},"slk":{"official":"Peruánska republika","common":"Peru"},"spa":{"official":"República de Perú","common":"Perú"},"swe":{"official":"Republiken Peru","common":"Peru"},"urd":{"official":"جمہوریہ پیرو","common":"پیرو"},"zho":{"official":"秘鲁共和国","common":"秘鲁"}},"latlng":[-10.0,-76.0],"landlocked":false,"borders":["BOL","BRA","CHL","COL","ECU"],"area":1285216.0,"demonyms":{"eng":{"f":"Peruvian","m":"Peruvian"},"fra":{"f":"Péruvienne","m":"Péruvien"}},"flag":"\uD83C\uDDF5\uD83C\uDDEA","maps":{"googleMaps":"https://goo.gl/maps/uDWEUaXNcZTng1fP6","openStreetMaps":"https://www.openstreetmap.org/relation/288247"},"population":32971846,"gini":{"2019":41.5},"fifa":"PER","car":{"signs":["PE"],"side":"right"},"timezones":["UTC-05:00"],"continents":["South America"],"flags":["https://flagcdn.com/pe.svg","https://flagcdn.com/w320/pe.png"]}]

let COUNTRYCODETONAME = [{"name":{"common":"Barbados","official":"Barbados","nativeName":{"eng":{"official":"Barbados","common":"Barbados"}}},"cca3":"BRB"},{"name":{"common":"Réunion","official":"Réunion Island","nativeName":{"fra":{"official":"Ile de la Réunion","common":"La Réunion"}}},"cca3":"REU"},{"name":{"common":"Suriname","official":"Republic of Suriname","nativeName":{"nld":{"official":"Republiek Suriname","common":"Suriname"}}},"cca3":"SUR"},{"name":{"common":"Namibia","official":"Republic of Namibia","nativeName":{"afr":{"official":"Republiek van Namibië","common":"Namibië"},"deu":{"official":"Republik Namibia","common":"Namibia"},"eng":{"official":"Republic of Namibia","common":"Namibia"},"her":{"official":"Republic of Namibia","common":"Namibia"},"hgm":{"official":"Republic of Namibia","common":"Namibia"},"kwn":{"official":"Republic of Namibia","common":"Namibia"},"loz":{"official":"Republic of Namibia","common":"Namibia"},"ndo":{"official":"Republic of Namibia","common":"Namibia"},"tsn":{"official":"Lefatshe la Namibia","common":"Namibia"}}},"cca3":"NAM"},{"name":{"common":"Guinea","official":"Republic of Guinea","nativeName":{"fra":{"official":"République de Guinée","common":"Guinée"}}},"cca3":"GIN"},{"name":{"common":"Vanuatu","official":"Republic of Vanuatu","nativeName":{"bis":{"official":"Ripablik blong Vanuatu","common":"Vanuatu"},"eng":{"official":"Republic of Vanuatu","common":"Vanuatu"},"fra":{"official":"République de Vanuatu","common":"Vanuatu"}}},"cca3":"VUT"},{"name":{"common":"Samoa","official":"Independent State of Samoa","nativeName":{"eng":{"official":"Independent State of Samoa","common":"Samoa"},"smo":{"official":"Malo Saʻoloto Tutoʻatasi o Sāmoa","common":"Sāmoa"}}},"cca3":"WSM"},{"name":{"common":"France","official":"French Republic","nativeName":{"fra":{"official":"République française","common":"France"}}},"cca3":"FRA"},{"name":{"common":"Andorra","official":"Principality of Andorra","nativeName":{"cat":{"official":"Principat d'Andorra","common":"Andorra"}}},"cca3":"AND"},{"name":{"common":"Azerbaijan","official":"Republic of Azerbaijan","nativeName":{"aze":{"official":"Azərbaycan Respublikası","common":"Azərbaycan"},"rus":{"official":"Азербайджанская Республика","common":"Азербайджан"}}},"cca3":"AZE"},{"name":{"common":"Maldives","official":"Republic of the Maldives","nativeName":{"div":{"official":"ދިވެހިރާއްޖޭގެ ޖުމްހޫރިއްޔާ","common":"ދިވެހިރާއްޖޭގެ"}}},"cca3":"MDV"},{"name":{"common":"French Polynesia","official":"French Polynesia","nativeName":{"fra":{"official":"Polynésie française","common":"Polynésie française"}}},"cca3":"PYF"},{"name":{"common":"Saint Lucia","official":"Saint Lucia","nativeName":{"eng":{"official":"Saint Lucia","common":"Saint Lucia"}}},"cca3":"LCA"},{"name":{"common":"Panama","official":"Republic of Panama","nativeName":{"spa":{"official":"República de Panamá","common":"Panamá"}}},"cca3":"PAN"},{"name":{"common":"Timor-Leste","official":"Democratic Republic of Timor-Leste","nativeName":{"por":{"official":"República Democrática de Timor-Leste","common":"Timor-Leste"},"tet":{"official":"Repúblika Demokrátika Timór-Leste","common":"Timór-Leste"}}},"cca3":"TLS"},{"name":{"common":"North Macedonia","official":"Republic of North Macedonia","nativeName":{"mkd":{"official":"Република Северна Македонија","common":"Македонија"}}},"cca3":"MKD"},{"name":{"common":"Denmark","official":"Kingdom of Denmark","nativeName":{"dan":{"official":"Kongeriget Danmark","common":"Danmark"}}},"cca3":"DNK"},{"name":{"common":"Egypt","official":"Arab Republic of Egypt","nativeName":{"ara":{"official":"جمهورية مصر العربية","common":"مصر"}}},"cca3":"EGY"},{"name":{"common":"Estonia","official":"Republic of Estonia","nativeName":{"est":{"official":"Eesti Vabariik","common":"Eesti"}}},"cca3":"EST"},{"name":{"common":"Czechia","official":"Czech Republic","nativeName":{"ces":{"official":"Česká republika","common":"Česko"},"slk":{"official":"Česká republika","common":"Česko"}}},"cca3":"CZE"},{"name":{"common":"Bahamas","official":"Commonwealth of the Bahamas","nativeName":{"eng":{"official":"Commonwealth of the Bahamas","common":"Bahamas"}}},"cca3":"BHS"},{"name":{"common":"Uruguay","official":"Oriental Republic of Uruguay","nativeName":{"spa":{"official":"República Oriental del Uruguay","common":"Uruguay"}}},"cca3":"URY"},{"name":{"common":"Åland Islands","official":"Åland Islands","nativeName":{"swe":{"official":"Landskapet Åland","common":"Åland"}}},"cca3":"ALA"},{"name":{"common":"Comoros","official":"Union of the Comoros","nativeName":{"ara":{"official":"الاتحاد القمري","common":"القمر‎"},"fra":{"official":"Union des Comores","common":"Comores"},"zdj":{"official":"Udzima wa Komori","common":"Komori"}}},"cca3":"COM"},{"name":{"common":"Cook Islands","official":"Cook Islands","nativeName":{"eng":{"official":"Cook Islands","common":"Cook Islands"},"rar":{"official":"Kūki 'Āirani","common":"Kūki 'Āirani"}}},"cca3":"COK"},{"name":{"common":"Costa Rica","official":"Republic of Costa Rica","nativeName":{"spa":{"official":"República de Costa Rica","common":"Costa Rica"}}},"cca3":"CRI"},{"name":{"common":"Togo","official":"Togolese Republic","nativeName":{"fra":{"official":"République togolaise","common":"Togo"}}},"cca3":"TGO"},{"name":{"common":"São Tomé and Príncipe","official":"Democratic Republic of São Tomé and Príncipe","nativeName":{"por":{"official":"República Democrática do São Tomé e Príncipe","common":"São Tomé e Príncipe"}}},"cca3":"STP"},{"name":{"common":"Netherlands","official":"Kingdom of the Netherlands","nativeName":{"nld":{"official":"Koninkrijk der Nederlanden","common":"Nederland"}}},"cca3":"NLD"},{"name":{"common":"Hong Kong","official":"Hong Kong Special Administrative Region of the People's Republic of China","nativeName":{"eng":{"official":"Hong Kong Special Administrative Region of the People's Republic of China","common":"Hong Kong"},"zho":{"official":"中华人民共和国香港特别行政区","common":"香港"}}},"cca3":"HKG"},{"name":{"common":"Australia","official":"Commonwealth of Australia","nativeName":{"eng":{"official":"Commonwealth of Australia","common":"Australia"}}},"cca3":"AUS"},{"name":{"common":"Malaysia","official":"Malaysia","nativeName":{"eng":{"official":"Malaysia","common":"Malaysia"},"msa":{"official":"مليسيا","common":"مليسيا"}}},"cca3":"MYS"},{"name":{"common":"Nepal","official":"Federal Democratic Republic of Nepal","nativeName":{"nep":{"official":"नेपाल संघीय लोकतान्त्रिक गणतन्त्र","common":"नेपाल"}}},"cca3":"NPL"},{"name":{"common":"Cuba","official":"Republic of Cuba","nativeName":{"spa":{"official":"República de Cuba","common":"Cuba"}}},"cca3":"CUB"},{"name":{"common":"North Korea","official":"Democratic People's Republic of Korea","nativeName":{"kor":{"official":"조선민주주의인민공화국","common":"조선"}}},"cca3":"PRK"},{"name":{"common":"French Guiana","official":"Guiana","nativeName":{"fra":{"official":"Guyane","common":"Guyane française"}}},"cca3":"GUF"},{"name":{"common":"Moldova","official":"Republic of Moldova","nativeName":{"ron":{"official":"Republica Moldova","common":"Moldova"}}},"cca3":"MDA"},{"name":{"common":"Zambia","official":"Republic of Zambia","nativeName":{"eng":{"official":"Republic of Zambia","common":"Zambia"}}},"cca3":"ZMB"},{"name":{"common":"Dominica","official":"Commonwealth of Dominica","nativeName":{"eng":{"official":"Commonwealth of Dominica","common":"Dominica"}}},"cca3":"DMA"},{"name":{"common":"Marshall Islands","official":"Republic of the Marshall Islands","nativeName":{"eng":{"official":"Republic of the Marshall Islands","common":"Marshall Islands"},"mah":{"official":"Republic of the Marshall Islands","common":"M̧ajeļ"}}},"cca3":"MHL"},{"name":{"common":"Tonga","official":"Kingdom of Tonga","nativeName":{"eng":{"official":"Kingdom of Tonga","common":"Tonga"},"ton":{"official":"Kingdom of Tonga","common":"Tonga"}}},"cca3":"TON"},{"name":{"common":"Peru","official":"Republic of Peru","nativeName":{"aym":{"official":"Piruw Suyu","common":"Piruw"},"que":{"official":"Piruw Ripuwlika","common":"Piruw"},"spa":{"official":"República del Perú","common":"Perú"}}},"cca3":"PER"},{"name":{"common":"Cape Verde","official":"Republic of Cabo Verde","nativeName":{"por":{"official":"República de Cabo Verde","common":"Cabo Verde"}}},"cca3":"CPV"},{"name":{"common":"Kiribati","official":"Independent and Sovereign Republic of Kiribati","nativeName":{"eng":{"official":"Independent and Sovereign Republic of Kiribati","common":"Kiribati"},"gil":{"official":"Ribaberiki Kiribati","common":"Kiribati"}}},"cca3":"KIR"},{"name":{"common":"Finland","official":"Republic of Finland","nativeName":{"fin":{"official":"Suomen tasavalta","common":"Suomi"},"swe":{"official":"Republiken Finland","common":"Finland"}}},"cca3":"FIN"},{"name":{"common":"Ivory Coast","official":"Republic of Côte d'Ivoire","nativeName":{"fra":{"official":"République de Côte d'Ivoire","common":"Côte d'Ivoire"}}},"cca3":"CIV"},{"name":{"common":"Martinique","official":"Martinique","nativeName":{"fra":{"official":"Martinique","common":"Martinique"}}},"cca3":"MTQ"},{"name":{"common":"Pakistan","official":"Islamic Republic of Pakistan","nativeName":{"eng":{"official":"Islamic Republic of Pakistan","common":"Pakistan"},"urd":{"official":"اسلامی جمہوریۂ پاكستان","common":"پاكستان"}}},"cca3":"PAK"},{"name":{"common":"Djibouti","official":"Republic of Djibouti","nativeName":{"ara":{"official":"جمهورية جيبوتي","common":"جيبوتي‎"},"fra":{"official":"République de Djibouti","common":"Djibouti"}}},"cca3":"DJI"},{"name":{"common":"Turks and Caicos Islands","official":"Turks and Caicos Islands","nativeName":{"eng":{"official":"Turks and Caicos Islands","common":"Turks and Caicos Islands"}}},"cca3":"TCA"},{"name":{"common":"Micronesia","official":"Federated States of Micronesia","nativeName":{"eng":{"official":"Federated States of Micronesia","common":"Micronesia"}}},"cca3":"FSM"},{"name":{"common":"Slovenia","official":"Republic of Slovenia","nativeName":{"slv":{"official":"Republika Slovenija","common":"Slovenija"}}},"cca3":"SVN"},{"name":{"common":"Kyrgyzstan","official":"Kyrgyz Republic","nativeName":{"kir":{"official":"Кыргыз Республикасы","common":"Кыргызстан"},"rus":{"official":"Кыргызская Республика","common":"Киргизия"}}},"cca3":"KGZ"},{"name":{"common":"Caribbean Netherlands","official":"Bonaire, Sint Eustatius and Saba","nativeName":{"nld":{"official":"Bonaire, Sint Eustatius en Saba","common":"Caribisch Nederland"},"pap":{"official":"Boneiru, Sint Eustatius y Saba","common":"Boneiru, Sint Eustatius y Saba"}}},"cca3":"BES"},{"name":{"common":"French Southern and Antarctic Lands","official":"Territory of the French Southern and Antarctic Lands","nativeName":{"fra":{"official":"Territoire des Terres australes et antarctiques françaises","common":"Terres australes et antarctiques françaises"}}},"cca3":"ATF"},{"name":{"common":"Saint Barthélemy","official":"Collectivity of Saint Barthélemy","nativeName":{"fra":{"official":"Collectivité de Saint-Barthélemy","common":"Saint-Barthélemy"}}},"cca3":"BLM"},{"name":{"common":"Switzerland","official":"Swiss Confederation","nativeName":{"fra":{"official":"Confédération suisse","common":"Suisse"},"gsw":{"official":"Schweizerische Eidgenossenschaft","common":"Schweiz"},"ita":{"official":"Confederazione Svizzera","common":"Svizzera"},"roh":{"official":"Confederaziun svizra","common":"Svizra"}}},"cca3":"CHE"},{"name":{"common":"Kuwait","official":"State of Kuwait","nativeName":{"ara":{"official":"دولة الكويت","common":"الكويت"}}},"cca3":"KWT"},{"name":{"common":"Seychelles","official":"Republic of Seychelles","nativeName":{"crs":{"official":"Repiblik Sesel","common":"Sesel"},"eng":{"official":"Republic of Seychelles","common":"Seychelles"},"fra":{"official":"République des Seychelles","common":"Seychelles"}}},"cca3":"SYC"},{"name":{"common":"United Kingdom","official":"United Kingdom of Great Britain and Northern Ireland","nativeName":{"eng":{"official":"United Kingdom of Great Britain and Northern Ireland","common":"United Kingdom"}}},"cca3":"GBR"},{"name":{"common":"United States Virgin Islands","official":"Virgin Islands of the United States","nativeName":{"eng":{"official":"Virgin Islands of the United States","common":"United States Virgin Islands"}}},"cca3":"VIR"},{"name":{"common":"Fiji","official":"Republic of Fiji","nativeName":{"eng":{"official":"Republic of Fiji","common":"Fiji"},"fij":{"official":"Matanitu Tugalala o Viti","common":"Viti"},"hif":{"official":"रिपब्लिक ऑफ फीजी","common":"फिजी"}}},"cca3":"FJI"},{"name":{"common":"Yemen","official":"Republic of Yemen","nativeName":{"ara":{"official":"الجمهورية اليمنية","common":"اليَمَن"}}},"cca3":"YEM"},{"name":{"common":"British Virgin Islands","official":"Virgin Islands","nativeName":{"eng":{"official":"Virgin Islands","common":"British Virgin Islands"}}},"cca3":"VGB"},{"name":{"common":"Bouvet Island","official":"Bouvet Island","nativeName":{"nor":{"official":"Bouvetøya","common":"Bouvetøya"}}},"cca3":"BVT"},{"name":{"common":"Central African Republic","official":"Central African Republic","nativeName":{"fra":{"official":"République centrafricaine","common":"République centrafricaine"},"sag":{"official":"Ködörösêse tî Bêafrîka","common":"Bêafrîka"}}},"cca3":"CAF"},{"name":{"common":"Belgium","official":"Kingdom of Belgium","nativeName":{"deu":{"official":"Königreich Belgien","common":"Belgien"},"fra":{"official":"Royaume de Belgique","common":"Belgique"},"nld":{"official":"Koninkrijk België","common":"België"}}},"cca3":"BEL"},{"name":{"common":"Curaçao","official":"Country of Curaçao","nativeName":{"eng":{"official":"Country of Curaçao","common":"Curaçao"},"nld":{"official":"Land Curaçao","common":"Curaçao"},"pap":{"official":"Pais Kòrsou","common":"Pais Kòrsou"}}},"cca3":"CUW"},{"name":{"common":"Argentina","official":"Argentine Republic","nativeName":{"grn":{"official":"Argentine Republic","common":"Argentina"},"spa":{"official":"República Argentina","common":"Argentina"}}},"cca3":"ARG"},{"name":{"common":"Madagascar","official":"Republic of Madagascar","nativeName":{"fra":{"official":"République de Madagascar","common":"Madagascar"},"mlg":{"official":"Repoblikan'i Madagasikara","common":"Madagasikara"}}},"cca3":"MDG"},{"name":{"common":"South Africa","official":"Republic of South Africa","nativeName":{"afr":{"official":"Republiek van Suid-Afrika","common":"South Africa"},"eng":{"official":"Republic of South Africa","common":"South Africa"},"nbl":{"official":"IRiphabliki yeSewula Afrika","common":"Sewula Afrika"},"nso":{"official":"Rephaboliki ya Afrika-Borwa ","common":"Afrika-Borwa"},"sot":{"official":"Rephaboliki ya Afrika Borwa","common":"Afrika Borwa"},"ssw":{"official":"IRiphabhulikhi yeNingizimu Afrika","common":"Ningizimu Afrika"},"tsn":{"official":"Rephaboliki ya Aforika Borwa","common":"Aforika Borwa"},"tso":{"official":"Riphabliki ra Afrika Dzonga","common":"Afrika Dzonga"},"ven":{"official":"Riphabuḽiki ya Afurika Tshipembe","common":"Afurika Tshipembe"},"xho":{"official":"IRiphabliki yaseMzantsi Afrika","common":"Mzantsi Afrika"},"zul":{"official":"IRiphabliki yaseNingizimu Afrika","common":"Ningizimu Afrika"}}},"cca3":"ZAF"},{"name":{"common":"Latvia","official":"Republic of Latvia","nativeName":{"lav":{"official":"Latvijas Republikas","common":"Latvija"}}},"cca3":"LVA"},{"name":{"common":"Zimbabwe","official":"Republic of Zimbabwe","nativeName":{"bwg":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"eng":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"kck":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"khi":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"ndc":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"nde":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"nya":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"sna":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"sot":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"toi":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"tsn":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"tso":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"ven":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"xho":{"official":"Republic of Zimbabwe","common":"Zimbabwe"},"zib":{"official":"Republic of Zimbabwe","common":"Zimbabwe"}}},"cca3":"ZWE"},{"name":{"common":"Antarctica","official":"Antarctica","nativeName":{}},"cca3":"ATA"},{"name":{"common":"Guernsey","official":"Bailiwick of Guernsey","nativeName":{"eng":{"official":"Bailiwick of Guernsey","common":"Guernsey"},"fra":{"official":"Bailliage de Guernesey","common":"Guernesey"},"nfr":{"official":"Dgèrnésiais","common":"Dgèrnésiais"}}},"cca3":"GGY"},{"name":{"common":"Gabon","official":"Gabonese Republic","nativeName":{"fra":{"official":"République gabonaise","common":"Gabon"}}},"cca3":"GAB"},{"name":{"common":"Romania","official":"Romania","nativeName":{"ron":{"official":"România","common":"România"}}},"cca3":"ROU"},{"name":{"common":"Germany","official":"Federal Republic of Germany","nativeName":{"deu":{"official":"Bundesrepublik Deutschland","common":"Deutschland"}}},"cca3":"DEU"},{"name":{"common":"Bangladesh","official":"People's Republic of Bangladesh","nativeName":{"ben":{"official":"বাংলাদেশ গণপ্রজাতন্ত্রী","common":"বাংলাদেশ"}}},"cca3":"BGD"},{"name":{"common":"Montserrat","official":"Montserrat","nativeName":{"eng":{"official":"Montserrat","common":"Montserrat"}}},"cca3":"MSR"},{"name":{"common":"Jordan","official":"Hashemite Kingdom of Jordan","nativeName":{"ara":{"official":"المملكة الأردنية الهاشمية","common":"الأردن"}}},"cca3":"JOR"},{"name":{"common":"Mozambique","official":"Republic of Mozambique","nativeName":{"por":{"official":"República de Moçambique","common":"Moçambique"}}},"cca3":"MOZ"},{"name":{"common":"Israel","official":"State of Israel","nativeName":{"ara":{"official":"دولة إسرائيل","common":"إسرائيل"},"heb":{"official":"מדינת ישראל","common":"ישראל"}}},"cca3":"ISR"},{"name":{"common":"Eritrea","official":"State of Eritrea","nativeName":{"ara":{"official":"دولة إرتريا","common":"إرتريا‎"},"eng":{"official":"State of Eritrea","common":"Eritrea"},"tir":{"official":"ሃገረ ኤርትራ","common":"ኤርትራ"}}},"cca3":"ERI"},{"name":{"common":"Indonesia","official":"Republic of Indonesia","nativeName":{"ind":{"official":"Republik Indonesia","common":"Indonesia"}}},"cca3":"IDN"},{"name":{"common":"Isle of Man","official":"Isle of Man","nativeName":{"eng":{"official":"Isle of Man","common":"Isle of Man"},"glv":{"official":"Ellan Vannin or Mannin","common":"Mannin"}}},"cca3":"IMN"},{"name":{"common":"Japan","official":"Japan","nativeName":{"jpn":{"official":"日本","common":"日本"}}},"cca3":"JPN"},{"name":{"common":"Spain","official":"Kingdom of Spain","nativeName":{"spa":{"official":"Reino de España","common":"España"}}},"cca3":"ESP"},{"name":{"common":"Afghanistan","official":"Islamic Republic of Afghanistan","nativeName":{"prs":{"official":"جمهوری اسلامی افغانستان","common":"افغانستان"},"pus":{"official":"د افغانستان اسلامي جمهوریت","common":"افغانستان"},"tuk":{"official":"Owganystan Yslam Respublikasy","common":"Owganystan"}}},"cca3":"AFG"},{"name":{"common":"United Arab Emirates","official":"United Arab Emirates","nativeName":{"ara":{"official":"الإمارات العربية المتحدة","common":"دولة الإمارات العربية المتحدة"}}},"cca3":"ARE"},{"name":{"common":"Malawi","official":"Republic of Malawi","nativeName":{"eng":{"official":"Republic of Malawi","common":"Malawi"},"nya":{"official":"Chalo cha Malawi, Dziko la Malaŵi","common":"Malaŵi"}}},"cca3":"MWI"},{"name":{"common":"Turkey","official":"Republic of Turkey","nativeName":{"tur":{"official":"Türkiye Cumhuriyeti","common":"Türkiye"}}},"cca3":"TUR"},{"name":{"common":"Belarus","official":"Republic of Belarus","nativeName":{"bel":{"official":"Рэспубліка Беларусь","common":"Белару́сь"},"rus":{"official":"Республика Беларусь","common":"Беларусь"}}},"cca3":"BLR"},{"name":{"common":"Montenegro","official":"Montenegro","nativeName":{"cnr":{"official":"Црна Гора","common":"Црна Гора"}}},"cca3":"MNE"},{"name":{"common":"Bosnia and Herzegovina","official":"Bosnia and Herzegovina","nativeName":{"bos":{"official":"Bosna i Hercegovina","common":"Bosna i Hercegovina"},"hrv":{"official":"Bosna i Hercegovina","common":"Bosna i Hercegovina"},"srp":{"official":"Босна и Херцеговина","common":"Босна и Херцеговина"}}},"cca3":"BIH"},{"name":{"common":"Eswatini","official":"Kingdom of Eswatini","nativeName":{"eng":{"official":"Kingdom of Eswatini","common":"Eswatini"},"ssw":{"official":"Umbuso weSwatini","common":"eSwatini"}}},"cca3":"SWZ"},{"name":{"common":"Lithuania","official":"Republic of Lithuania","nativeName":{"lit":{"official":"Lietuvos Respublikos","common":"Lietuva"}}},"cca3":"LTU"},{"name":{"common":"Turkmenistan","official":"Turkmenistan","nativeName":{"rus":{"official":"Туркменистан","common":"Туркмения"},"tuk":{"official":"Türkmenistan","common":"Türkmenistan"}}},"cca3":"TKM"},{"name":{"common":"Ethiopia","official":"Federal Democratic Republic of Ethiopia","nativeName":{"amh":{"official":"የኢትዮጵያ ፌዴራላዊ ዲሞክራሲያዊ ሪፐብሊክ","common":"ኢትዮጵያ"}}},"cca3":"ETH"},{"name":{"common":"Anguilla","official":"Anguilla","nativeName":{"eng":{"official":"Anguilla","common":"Anguilla"}}},"cca3":"AIA"},{"name":{"common":"Hungary","official":"Hungary","nativeName":{"hun":{"official":"Magyarország","common":"Magyarország"}}},"cca3":"HUN"},{"name":{"common":"American Samoa","official":"American Samoa","nativeName":{"eng":{"official":"American Samoa","common":"American Samoa"},"smo":{"official":"Sāmoa Amelika","common":"Sāmoa Amelika"}}},"cca3":"ASM"},{"name":{"common":"Chad","official":"Republic of Chad","nativeName":{"ara":{"official":"جمهورية تشاد","common":"تشاد‎"},"fra":{"official":"République du Tchad","common":"Tchad"}}},"cca3":"TCD"},{"name":{"common":"Guadeloupe","official":"Guadeloupe","nativeName":{"fra":{"official":"Guadeloupe","common":"Guadeloupe"}}},"cca3":"GLP"},{"name":{"common":"Nigeria","official":"Federal Republic of Nigeria","nativeName":{"eng":{"official":"Federal Republic of Nigeria","common":"Nigeria"}}},"cca3":"NGA"},{"name":{"common":"San Marino","official":"Republic of San Marino","nativeName":{"ita":{"official":"Repubblica di San Marino","common":"San Marino"}}},"cca3":"SMR"},{"name":{"common":"Ukraine","official":"Ukraine","nativeName":{"ukr":{"official":"Україна","common":"Україна"}}},"cca3":"UKR"},{"name":{"common":"Haiti","official":"Republic of Haiti","nativeName":{"fra":{"official":"République d'Haïti","common":"Haïti"},"hat":{"official":"Repiblik Ayiti","common":"Ayiti"}}},"cca3":"HTI"},{"name":{"common":"Botswana","official":"Republic of Botswana","nativeName":{"eng":{"official":"Republic of Botswana","common":"Botswana"},"tsn":{"official":"Lefatshe la Botswana","common":"Botswana"}}},"cca3":"BWA"},{"name":{"common":"Portugal","official":"Portuguese Republic","nativeName":{"por":{"official":"República português","common":"Portugal"}}},"cca3":"PRT"},{"name":{"common":"Guyana","official":"Co-operative Republic of Guyana","nativeName":{"eng":{"official":"Co-operative Republic of Guyana","common":"Guyana"}}},"cca3":"GUY"},{"name":{"common":"Ireland","official":"Republic of Ireland","nativeName":{"eng":{"official":"Republic of Ireland","common":"Ireland"},"gle":{"official":"Poblacht na hÉireann","common":"Éire"}}},"cca3":"IRL"},{"name":{"common":"New Caledonia","official":"New Caledonia","nativeName":{"fra":{"official":"Nouvelle-Calédonie","common":"Nouvelle-Calédonie"}}},"cca3":"NCL"},{"name":{"common":"Saint Martin","official":"Saint Martin","nativeName":{"fra":{"official":"Saint-Martin","common":"Saint-Martin"}}},"cca3":"MAF"},{"name":{"common":"Mauritius","official":"Republic of Mauritius","nativeName":{"eng":{"official":"Republic of Mauritius","common":"Mauritius"},"fra":{"official":"République de Maurice","common":"Maurice"},"mfe":{"official":"Republik Moris","common":"Moris"}}},"cca3":"MUS"},{"name":{"common":"Gibraltar","official":"Gibraltar","nativeName":{"eng":{"official":"Gibraltar","common":"Gibraltar"}}},"cca3":"GIB"},{"name":{"common":"Saint Kitts and Nevis","official":"Federation of Saint Christopher and Nevis","nativeName":{"eng":{"official":"Federation of Saint Christopher and Nevis","common":"Saint Kitts and Nevis"}}},"cca3":"KNA"},{"name":{"common":"Iceland","official":"Iceland","nativeName":{"isl":{"official":"Ísland","common":"Ísland"}}},"cca3":"ISL"},{"name":{"common":"Svalbard and Jan Mayen","official":"Svalbard og Jan Mayen","nativeName":{"nor":{"official":"Svalbard og Jan Mayen","common":"Svalbard og Jan Mayen"}}},"cca3":"SJM"},{"name":{"common":"Chile","official":"Republic of Chile","nativeName":{"spa":{"official":"República de Chile","common":"Chile"}}},"cca3":"CHL"},{"name":{"common":"United States Minor Outlying Islands","official":"United States Minor Outlying Islands","nativeName":{"eng":{"official":"United States Minor Outlying Islands","common":"United States Minor Outlying Islands"}}},"cca3":"UMI"},{"name":{"common":"Kazakhstan","official":"Republic of Kazakhstan","nativeName":{"kaz":{"official":"Қазақстан Республикасы","common":"Қазақстан"},"rus":{"official":"Республика Казахстан","common":"Казахстан"}}},"cca3":"KAZ"},{"name":{"common":"China","official":"People's Republic of China","nativeName":{"zho":{"official":"中华人民共和国","common":"中国"}}},"cca3":"CHN"},{"name":{"common":"Macau","official":"Macao Special Administrative Region of the People's Republic of China","nativeName":{"por":{"official":"Região Administrativa Especial de Macau da República Popular da China","common":"Macau"},"zho":{"official":"中华人民共和国澳门特别行政区","common":"澳门"}}},"cca3":"MAC"},{"name":{"common":"Armenia","official":"Republic of Armenia","nativeName":{"hye":{"official":"Հայաստանի Հանրապետություն","common":"Հայաստան"}}},"cca3":"ARM"},{"name":{"common":"Bolivia","official":"Plurinational State of Bolivia","nativeName":{"aym":{"official":"Wuliwya Suyu","common":"Wuliwya"},"grn":{"official":"Tetã Volívia","common":"Volívia"},"que":{"official":"Buliwya Mamallaqta","common":"Buliwya"},"spa":{"official":"Estado Plurinacional de Bolivia","common":"Bolivia"}}},"cca3":"BOL"},{"name":{"common":"Sudan","official":"Republic of the Sudan","nativeName":{"ara":{"official":"جمهورية السودان","common":"السودان"},"eng":{"official":"Republic of the Sudan","common":"Sudan"}}},"cca3":"SDN"},{"name":{"common":"Vietnam","official":"Socialist Republic of Vietnam","nativeName":{"vie":{"official":"Cộng hòa xã hội chủ nghĩa Việt Nam","common":"Việt Nam"}}},"cca3":"VNM"},{"name":{"common":"Wallis and Futuna","official":"Territory of the Wallis and Futuna Islands","nativeName":{"fra":{"official":"Territoire des îles Wallis et Futuna","common":"Wallis et Futuna"}}},"cca3":"WLF"},{"name":{"common":"Burundi","official":"Republic of Burundi","nativeName":{"fra":{"official":"République du Burundi","common":"Burundi"},"run":{"official":"Republika y'Uburundi ","common":"Uburundi"}}},"cca3":"BDI"},{"name":{"common":"Canada","official":"Canada","nativeName":{"eng":{"official":"Canada","common":"Canada"},"fra":{"official":"Canada","common":"Canada"}}},"cca3":"CAN"},{"name":{"common":"Colombia","official":"Republic of Colombia","nativeName":{"spa":{"official":"República de Colombia","common":"Colombia"}}},"cca3":"COL"},{"name":{"common":"Lesotho","official":"Kingdom of Lesotho","nativeName":{"eng":{"official":"Kingdom of Lesotho","common":"Lesotho"},"sot":{"official":"Kingdom of Lesotho","common":"Lesotho"}}},"cca3":"LSO"},{"name":{"common":"Bermuda","official":"Bermuda","nativeName":{"eng":{"official":"Bermuda","common":"Bermuda"}}},"cca3":"BMU"},{"name":{"common":"Guatemala","official":"Republic of Guatemala","nativeName":{"spa":{"official":"República de Guatemala","common":"Guatemala"}}},"cca3":"GTM"},{"name":{"common":"Uzbekistan","official":"Republic of Uzbekistan","nativeName":{"rus":{"official":"Республика Узбекистан","common":"Узбекистан"},"uzb":{"official":"O'zbekiston Respublikasi","common":"O‘zbekiston"}}},"cca3":"UZB"},{"name":{"common":"Cayman Islands","official":"Cayman Islands","nativeName":{"eng":{"official":"Cayman Islands","common":"Cayman Islands"}}},"cca3":"CYM"},{"name":{"common":"Senegal","official":"Republic of Senegal","nativeName":{"fra":{"official":"République du Sénégal","common":"Sénégal"}}},"cca3":"SEN"},{"name":{"common":"Gambia","official":"Republic of the Gambia","nativeName":{"eng":{"official":"Republic of the Gambia","common":"Gambia"}}},"cca3":"GMB"},{"name":{"common":"Liechtenstein","official":"Principality of Liechtenstein","nativeName":{"deu":{"official":"Fürstentum Liechtenstein","common":"Liechtenstein"}}},"cca3":"LIE"},{"name":{"common":"Pitcairn Islands","official":"Pitcairn Group of Islands","nativeName":{"eng":{"official":"Pitcairn Group of Islands","common":"Pitcairn Islands"}}},"cca3":"PCN"},{"name":{"common":"Tuvalu","official":"Tuvalu","nativeName":{"eng":{"official":"Tuvalu","common":"Tuvalu"},"tvl":{"official":"Tuvalu","common":"Tuvalu"}}},"cca3":"TUV"},{"name":{"common":"Equatorial Guinea","official":"Republic of Equatorial Guinea","nativeName":{"fra":{"official":"République de la Guinée Équatoriale","common":"Guinée équatoriale"},"por":{"official":"República da Guiné Equatorial","common":"Guiné Equatorial"},"spa":{"official":"República de Guinea Ecuatorial","common":"Guinea Ecuatorial"}}},"cca3":"GNQ"},{"name":{"common":"Bhutan","official":"Kingdom of Bhutan","nativeName":{"dzo":{"official":"འབྲུག་རྒྱལ་ཁབ་","common":"འབྲུག་ཡུལ་"}}},"cca3":"BTN"},{"name":{"common":"Aruba","official":"Aruba","nativeName":{"nld":{"official":"Aruba","common":"Aruba"},"pap":{"official":"Aruba","common":"Aruba"}}},"cca3":"ABW"},{"name":{"common":"Paraguay","official":"Republic of Paraguay","nativeName":{"grn":{"official":"Tetã Paraguái","common":"Paraguái"},"spa":{"official":"República de Paraguay","common":"Paraguay"}}},"cca3":"PRY"},{"name":{"common":"Grenada","official":"Grenada","nativeName":{"eng":{"official":"Grenada","common":"Grenada"}}},"cca3":"GRD"},{"name":{"common":"Papua New Guinea","official":"Independent State of Papua New Guinea","nativeName":{"eng":{"official":"Independent State of Papua New Guinea","common":"Papua New Guinea"},"hmo":{"official":"Independen Stet bilong Papua Niugini","common":"Papua Niu Gini"},"tpi":{"official":"Independen Stet bilong Papua Niugini","common":"Papua Niugini"}}},"cca3":"PNG"},{"name":{"common":"Jamaica","official":"Jamaica","nativeName":{"eng":{"official":"Jamaica","common":"Jamaica"},"jam":{"official":"Jamaica","common":"Jamaica"}}},"cca3":"JAM"},{"name":{"common":"Republic of the Congo","official":"Republic of the Congo","nativeName":{"fra":{"official":"République du Congo","common":"République du Congo"},"kon":{"official":"Repubilika ya Kongo","common":"Repubilika ya Kongo"},"lin":{"official":"Republíki ya Kongó","common":"Republíki ya Kongó"}}},"cca3":"COG"},{"name":{"common":"Poland","official":"Republic of Poland","nativeName":{"pol":{"official":"Rzeczpospolita Polska","common":"Polska"}}},"cca3":"POL"},{"name":{"common":"Russia","official":"Russian Federation","nativeName":{"rus":{"official":"Российская Федерация","common":"Россия"}}},"cca3":"RUS"},{"name":{"common":"Mauritania","official":"Islamic Republic of Mauritania","nativeName":{"ara":{"official":"الجمهورية الإسلامية الموريتانية","common":"موريتانيا"}}},"cca3":"MRT"},{"name":{"common":"Western Sahara","official":"Sahrawi Arab Democratic Republic","nativeName":{"ber":{"official":"Sahrawi Arab Democratic Republic","common":"Western Sahara"},"mey":{"official":"الجمهورية العربية الصحراوية الديمقراطية","common":"الصحراء الغربية"},"spa":{"official":"República Árabe Saharaui Democrática","common":"Sahara Occidental"}}},"cca3":"ESH"},{"name":{"common":"Burkina Faso","official":"Burkina Faso","nativeName":{"fra":{"official":"République du Burkina","common":"Burkina Faso"}}},"cca3":"BFA"},{"name":{"common":"DR Congo","official":"Democratic Republic of the Congo","nativeName":{"fra":{"official":"République démocratique du Congo","common":"RD Congo"},"kon":{"official":"Repubilika ya Kongo Demokratiki","common":"Repubilika ya Kongo Demokratiki"},"lin":{"official":"Republiki ya Kongó Demokratiki","common":"Republiki ya Kongó Demokratiki"},"lua":{"official":"Ditunga dia Kongu wa Mungalaata","common":"Ditunga dia Kongu wa Mungalaata"},"swa":{"official":"Jamhuri ya Kidemokrasia ya Kongo","common":"Jamhuri ya Kidemokrasia ya Kongo"}}},"cca3":"COD"},{"name":{"common":"Benin","official":"Republic of Benin","nativeName":{"fra":{"official":"République du Bénin","common":"Bénin"}}},"cca3":"BEN"},{"name":{"common":"Uganda","official":"Republic of Uganda","nativeName":{"eng":{"official":"Republic of Uganda","common":"Uganda"},"swa":{"official":"Republic of Uganda","common":"Uganda"}}},"cca3":"UGA"},{"name":{"common":"Mongolia","official":"Mongolia","nativeName":{"mon":{"official":"Монгол улс","common":"Монгол улс"}}},"cca3":"MNG"},{"name":{"common":"Laos","official":"Lao People's Democratic Republic","nativeName":{"lao":{"official":"ສາທາລະນະ ຊາທິປະໄຕ ຄົນລາວ ຂອງ","common":"ສປປລາວ"}}},"cca3":"LAO"},{"name":{"common":"Algeria","official":"People's Democratic Republic of Algeria","nativeName":{"ara":{"official":"الجمهورية الديمقراطية الشعبية الجزائرية","common":"الجزائر"}}},"cca3":"DZA"},{"name":{"common":"Brunei","official":"Nation of Brunei, Abode of Peace","nativeName":{"msa":{"official":"Nation of Brunei, Abode Damai","common":"Negara Brunei Darussalam"}}},"cca3":"BRN"},{"name":{"common":"Kenya","official":"Republic of Kenya","nativeName":{"eng":{"official":"Republic of Kenya","common":"Kenya"},"swa":{"official":"Republic of Kenya","common":"Kenya"}}},"cca3":"KEN"},{"name":{"common":"Sri Lanka","official":"Democratic Socialist Republic of Sri Lanka","nativeName":{"sin":{"official":"ශ්‍රී ලංකා ප්‍රජාතාන්ත්‍රික සමාජවාදී ජනරජය","common":"ශ්‍රී ලංකාව"},"tam":{"official":"இலங்கை சனநாயக சோசலிசக் குடியரசு","common":"இலங்கை"}}},"cca3":"LKA"},{"name":{"common":"Dominican Republic","official":"Dominican Republic","nativeName":{"spa":{"official":"República Dominicana","common":"República Dominicana"}}},"cca3":"DOM"},{"name":{"common":"Austria","official":"Republic of Austria","nativeName":{"bar":{"official":"Republik Österreich","common":"Österreich"}}},"cca3":"AUT"},{"name":{"common":"Luxembourg","official":"Grand Duchy of Luxembourg","nativeName":{"deu":{"official":"Großherzogtum Luxemburg","common":"Luxemburg"},"fra":{"official":"Grand-Duché de Luxembourg","common":"Luxembourg"},"ltz":{"official":"Groussherzogtum Lëtzebuerg","common":"Lëtzebuerg"}}},"cca3":"LUX"},{"name":{"common":"Georgia","official":"Georgia","nativeName":{"kat":{"official":"საქართველო","common":"საქართველო"}}},"cca3":"GEO"},{"name":{"common":"Slovakia","official":"Slovak Republic","nativeName":{"slk":{"official":"Slovenská republika","common":"Slovensko"}}},"cca3":"SVK"},{"name":{"common":"Norway","official":"Kingdom of Norway","nativeName":{"nno":{"official":"Kongeriket Noreg","common":"Noreg"},"nob":{"official":"Kongeriket Norge","common":"Norge"},"smi":{"official":"Norgga gonagasriika","common":"Norgga"}}},"cca3":"NOR"},{"name":{"common":"Thailand","official":"Kingdom of Thailand","nativeName":{"tha":{"official":"ราชอาณาจักรไทย","common":"ประเทศไทย"}}},"cca3":"THA"},{"name":{"common":"British Indian Ocean Territory","official":"British Indian Ocean Territory","nativeName":{"eng":{"official":"British Indian Ocean Territory","common":"British Indian Ocean Territory"}}},"cca3":"IOT"},{"name":{"common":"India","official":"Republic of India","nativeName":{"eng":{"official":"Republic of India","common":"India"},"hin":{"official":"भारत गणराज्य","common":"भारत"},"tam":{"official":"இந்தியக் குடியரசு","common":"இந்தியா"}}},"cca3":"IND"},{"name":{"common":"Morocco","official":"Kingdom of Morocco","nativeName":{"ara":{"official":"المملكة المغربية","common":"المغرب"},"ber":{"official":"ⵜⴰⴳⵍⴷⵉⵜ ⵏ ⵍⵎⵖⵔⵉⴱ","common":"ⵍⵎⴰⵖⵔⵉⴱ"}}},"cca3":"MAR"},{"name":{"common":"Saint Pierre and Miquelon","official":"Saint Pierre and Miquelon","nativeName":{"fra":{"official":"Collectivité territoriale de Saint-Pierre-et-Miquelon","common":"Saint-Pierre-et-Miquelon"}}},"cca3":"SPM"},{"name":{"common":"Greenland","official":"Greenland","nativeName":{"kal":{"official":"Kalaallit Nunaat","common":"Kalaallit Nunaat"}}},"cca3":"GRL"},{"name":{"common":"Nicaragua","official":"Republic of Nicaragua","nativeName":{"spa":{"official":"República de Nicaragua","common":"Nicaragua"}}},"cca3":"NIC"},{"name":{"common":"Qatar","official":"State of Qatar","nativeName":{"ara":{"official":"دولة قطر","common":"قطر"}}},"cca3":"QAT"},{"name":{"common":"Taiwan","official":"Republic of China (Taiwan)","nativeName":{"zho":{"official":"中華民國","common":"台灣"}}},"cca3":"TWN"},{"name":{"common":"Brazil","official":"Federative Republic of Brazil","nativeName":{"por":{"official":"República Federativa do Brasil","common":"Brasil"}}},"cca3":"BRA"},{"name":{"common":"New Zealand","official":"New Zealand","nativeName":{"eng":{"official":"New Zealand","common":"New Zealand"},"mri":{"official":"Aotearoa","common":"Aotearoa"},"nzs":{"official":"New Zealand","common":"New Zealand"}}},"cca3":"NZL"},{"name":{"common":"Singapore","official":"Republic of Singapore","nativeName":{"zho":{"official":"新加坡共和国","common":"新加坡"},"eng":{"official":"Republic of Singapore","common":"Singapore"},"msa":{"official":"Republik Singapura","common":"Singapura"},"tam":{"official":"சிங்கப்பூர் குடியரசு","common":"சிங்கப்பூர்"}}},"cca3":"SGP"},{"name":{"common":"Syria","official":"Syrian Arab Republic","nativeName":{"ara":{"official":"الجمهورية العربية السورية","common":"سوريا"}}},"cca3":"SYR"},{"name":{"common":"Belize","official":"Belize","nativeName":{"bjz":{"official":"Belize","common":"Belize"},"eng":{"official":"Belize","common":"Belize"},"spa":{"official":"Belice","common":"Belice"}}},"cca3":"BLZ"},{"name":{"common":"Falkland Islands","official":"Falkland Islands","nativeName":{"eng":{"official":"Falkland Islands","common":"Falkland Islands"}}},"cca3":"FLK"},{"name":{"common":"Venezuela","official":"Bolivarian Republic of Venezuela","nativeName":{"spa":{"official":"República Bolivariana de Venezuela","common":"Venezuela"}}},"cca3":"VEN"},{"name":{"common":"Bahrain","official":"Kingdom of Bahrain","nativeName":{"ara":{"official":"مملكة البحرين","common":"‏البحرين"}}},"cca3":"BHR"},{"name":{"common":"Cocos (Keeling) Islands","official":"Territory of the Cocos (Keeling) Islands","nativeName":{"eng":{"official":"Territory of the Cocos (Keeling) Islands","common":"Cocos (Keeling) Islands"}}},"cca3":"CCK"},{"name":{"common":"Northern Mariana Islands","official":"Commonwealth of the Northern Mariana Islands","nativeName":{"cal":{"official":"Commonwealth of the Northern Mariana Islands","common":"Northern Mariana Islands"},"cha":{"official":"Sankattan Siha Na Islas Mariånas","common":"Na Islas Mariånas"},"eng":{"official":"Commonwealth of the Northern Mariana Islands","common":"Northern Mariana Islands"}}},"cca3":"MNP"},{"name":{"common":"Cameroon","official":"Republic of Cameroon","nativeName":{"eng":{"official":"Republic of Cameroon","common":"Cameroon"},"fra":{"official":"République du Cameroun","common":"Cameroun"}}},"cca3":"CMR"},{"name":{"common":"Cyprus","official":"Republic of Cyprus","nativeName":{"ell":{"official":"Δημοκρατία της Κύπρος","common":"Κύπρος"},"tur":{"official":"Kıbrıs Cumhuriyeti","common":"Kıbrıs"}}},"cca3":"CYP"},{"name":{"common":"United States","official":"United States of America","nativeName":{"eng":{"official":"United States of America","common":"United States"}}},"cca3":"USA"},{"name":{"common":"Angola","official":"Republic of Angola","nativeName":{"por":{"official":"República de Angola","common":"Angola"}}},"cca3":"AGO"},{"name":{"common":"Tunisia","official":"Tunisian Republic","nativeName":{"ara":{"official":"الجمهورية التونسية","common":"تونس"}}},"cca3":"TUN"},{"name":{"common":"Monaco","official":"Principality of Monaco","nativeName":{"fra":{"official":"Principauté de Monaco","common":"Monaco"}}},"cca3":"MCO"},{"name":{"common":"Rwanda","official":"Republic of Rwanda","nativeName":{"eng":{"official":"Republic of Rwanda","common":"Rwanda"},"fra":{"official":"République rwandaise","common":"Rwanda"},"kin":{"official":"Repubulika y'u Rwanda","common":"Rwanda"}}},"cca3":"RWA"},{"name":{"common":"Trinidad and Tobago","official":"Republic of Trinidad and Tobago","nativeName":{"eng":{"official":"Republic of Trinidad and Tobago","common":"Trinidad and Tobago"}}},"cca3":"TTO"},{"name":{"common":"Malta","official":"Republic of Malta","nativeName":{"eng":{"official":"Republic of Malta","common":"Malta"},"mlt":{"official":"Repubblika ta ' Malta","common":"Malta"}}},"cca3":"MLT"},{"name":{"common":"Mexico","official":"United Mexican States","nativeName":{"spa":{"official":"Estados Unidos Mexicanos","common":"México"}}},"cca3":"MEX"},{"name":{"common":"Mayotte","official":"Department of Mayotte","nativeName":{"fra":{"official":"Département de Mayotte","common":"Mayotte"}}},"cca3":"MYT"},{"name":{"common":"Antigua and Barbuda","official":"Antigua and Barbuda","nativeName":{"eng":{"official":"Antigua and Barbuda","common":"Antigua and Barbuda"}}},"cca3":"ATG"},{"name":{"common":"Tokelau","official":"Tokelau","nativeName":{"eng":{"official":"Tokelau","common":"Tokelau"},"smo":{"official":"Tokelau","common":"Tokelau"},"tkl":{"official":"Tokelau","common":"Tokelau"}}},"cca3":"TKL"},{"name":{"common":"South Korea","official":"Republic of Korea","nativeName":{"kor":{"official":"대한민국","common":"한국"}}},"cca3":"KOR"},{"name":{"common":"Niger","official":"Republic of Niger","nativeName":{"fra":{"official":"République du Niger","common":"Niger"}}},"cca3":"NER"},{"name":{"common":"Albania","official":"Republic of Albania","nativeName":{"sqi":{"official":"Republika e Shqipërisë","common":"Shqipëria"}}},"cca3":"ALB"},{"name":{"common":"Somalia","official":"Federal Republic of Somalia","nativeName":{"ara":{"official":"جمهورية الصومال‎‎","common":"الصومال‎‎"},"som":{"official":"Jamhuuriyadda Federaalka Soomaaliya","common":"Soomaaliya"}}},"cca3":"SOM"},{"name":{"common":"Liberia","official":"Republic of Liberia","nativeName":{"eng":{"official":"Republic of Liberia","common":"Liberia"}}},"cca3":"LBR"},{"name":{"common":"Myanmar","official":"Republic of the Union of Myanmar","nativeName":{"mya":{"official":"ပြည်ထောင်စု သမ္မတ မြန်မာနိုင်ငံတော်","common":"မြန်မာ"}}},"cca3":"MMR"},{"name":{"common":"Tanzania","official":"United Republic of Tanzania","nativeName":{"eng":{"official":"United Republic of Tanzania","common":"Tanzania"},"swa":{"official":"Jamhuri ya Muungano wa Tanzania","common":"Tanzania"}}},"cca3":"TZA"},{"name":{"common":"Iraq","official":"Republic of Iraq","nativeName":{"ara":{"official":"جمهورية العراق","common":"العراق"},"arc":{"official":"ܩܘܼܛܢܵܐ ܐܝܼܪܲܩ","common":"ܩܘܼܛܢܵܐ"},"ckb":{"official":"کۆماری عێراق","common":"کۆماری"}}},"cca3":"IRQ"},{"name":{"common":"South Georgia","official":"South Georgia and the South Sandwich Islands","nativeName":{"eng":{"official":"South Georgia and the South Sandwich Islands","common":"South Georgia"}}},"cca3":"SGS"},{"name":{"common":"Saint Vincent and the Grenadines","official":"Saint Vincent and the Grenadines","nativeName":{"eng":{"official":"Saint Vincent and the Grenadines","common":"Saint Vincent and the Grenadines"}}},"cca3":"VCT"},{"name":{"common":"Libya","official":"State of Libya","nativeName":{"ara":{"official":"الدولة ليبيا","common":"‏ليبيا"}}},"cca3":"LBY"},{"name":{"common":"Sierra Leone","official":"Republic of Sierra Leone","nativeName":{"eng":{"official":"Republic of Sierra Leone","common":"Sierra Leone"}}},"cca3":"SLE"},{"name":{"common":"Sint Maarten","official":"Sint Maarten","nativeName":{"eng":{"official":"Sint Maarten","common":"Sint Maarten"},"fra":{"official":"Saint-Martin","common":"Saint-Martin"},"nld":{"official":"Sint Maarten","common":"Sint Maarten"}}},"cca3":"SXM"},{"name":{"common":"Serbia","official":"Republic of Serbia","nativeName":{"srp":{"official":"Република Србија","common":"Србија"}}},"cca3":"SRB"},{"name":{"common":"Heard Island and McDonald Islands","official":"Heard Island and McDonald Islands","nativeName":{"eng":{"official":"Heard Island and McDonald Islands","common":"Heard Island and McDonald Islands"}}},"cca3":"HMD"},{"name":{"common":"Ghana","official":"Republic of Ghana","nativeName":{"eng":{"official":"Republic of Ghana","common":"Ghana"}}},"cca3":"GHA"},{"name":{"common":"South Sudan","official":"Republic of South Sudan","nativeName":{"eng":{"official":"Republic of South Sudan","common":"South Sudan"}}},"cca3":"SSD"},{"name":{"common":"Sweden","official":"Kingdom of Sweden","nativeName":{"swe":{"official":"Konungariket Sverige","common":"Sverige"}}},"cca3":"SWE"},{"name":{"common":"Greece","official":"Hellenic Republic","nativeName":{"ell":{"official":"Ελληνική Δημοκρατία","common":"Ελλάδα"}}},"cca3":"GRC"},{"name":{"common":"Faroe Islands","official":"Faroe Islands","nativeName":{"dan":{"official":"Færøerne","common":"Færøerne"},"fao":{"official":"Føroyar","common":"Føroyar"}}},"cca3":"FRO"},{"name":{"common":"Philippines","official":"Republic of the Philippines","nativeName":{"eng":{"official":"Republic of the Philippines","common":"Philippines"},"fil":{"official":"Republic of the Philippines","common":"Pilipinas"}}},"cca3":"PHL"},{"name":{"common":"Guinea-Bissau","official":"Republic of Guinea-Bissau","nativeName":{"por":{"official":"República da Guiné-Bissau","common":"Guiné-Bissau"},"pov":{"official":"República da Guiné-Bissau","common":"Guiné-Bissau"}}},"cca3":"GNB"},{"name":{"common":"Saudi Arabia","official":"Kingdom of Saudi Arabia","nativeName":{"ara":{"official":"المملكة العربية السعودية","common":"العربية السعودية"}}},"cca3":"SAU"},{"name":{"common":"Palau","official":"Republic of Palau","nativeName":{"eng":{"official":"Republic of Palau","common":"Palau"},"pau":{"official":"Beluu er a Belau","common":"Belau"}}},"cca3":"PLW"},{"name":{"common":"Bulgaria","official":"Republic of Bulgaria","nativeName":{"bul":{"official":"Република България","common":"България"}}},"cca3":"BGR"},{"name":{"common":"Nauru","official":"Republic of Nauru","nativeName":{"eng":{"official":"Republic of Nauru","common":"Nauru"},"nau":{"official":"Republic of Nauru","common":"Nauru"}}},"cca3":"NRU"},{"name":{"common":"Cambodia","official":"Kingdom of Cambodia","nativeName":{"khm":{"official":"ព្រះរាជាណាចក្រកម្ពុជា","common":"Kâmpŭchéa"}}},"cca3":"KHM"},{"name":{"common":"Jersey","official":"Bailiwick of Jersey","nativeName":{"eng":{"official":"Bailiwick of Jersey","common":"Jersey"},"fra":{"official":"Bailliage de Jersey","common":"Jersey"},"nrf":{"official":"Bailliage dé Jèrri","common":"Jèrri"}}},"cca3":"JEY"},{"name":{"common":"Palestine","official":"State of Palestine","nativeName":{"ara":{"official":"دولة فلسطين","common":"فلسطين"}}},"cca3":"PSE"},{"name":{"common":"Italy","official":"Italian Republic","nativeName":{"ita":{"official":"Repubblica italiana","common":"Italia"}}},"cca3":"ITA"},{"name":{"common":"Mali","official":"Republic of Mali","nativeName":{"fra":{"official":"République du Mali","common":"Mali"}}},"cca3":"MLI"},{"name":{"common":"Iran","official":"Islamic Republic of Iran","nativeName":{"fas":{"official":"جمهوری اسلامی ایران","common":"ایران"}}},"cca3":"IRN"},{"name":{"common":"Niue","official":"Niue","nativeName":{"eng":{"official":"Niue","common":"Niue"},"niu":{"official":"Niuē","common":"Niuē"}}},"cca3":"NIU"},{"name":{"common":"Tajikistan","official":"Republic of Tajikistan","nativeName":{"rus":{"official":"Республика Таджикистан","common":"Таджикистан"},"tgk":{"official":"Ҷумҳурии Тоҷикистон","common":"Тоҷикистон"}}},"cca3":"TJK"},{"name":{"common":"Kosovo","official":"Republic of Kosovo","nativeName":{"sqi":{"official":"Republika e Kosovës","common":"Kosova"},"srp":{"official":"Република Косово","common":"Косово"}}},"cca3":"UNK"},{"name":{"common":"Saint Helena, Ascension and Tristan da Cunha","official":"Saint Helena, Ascension and Tristan da Cunha","nativeName":{"eng":{"official":"Saint Helena, Ascension and Tristan da Cunha","common":"Saint Helena, Ascension and Tristan da Cunha"}}},"cca3":"SHN"},{"name":{"common":"Norfolk Island","official":"Territory of Norfolk Island","nativeName":{"eng":{"official":"Territory of Norfolk Island","common":"Norfolk Island"},"pih":{"official":"Teratri of Norf'k Ailen","common":"Norf'k Ailen"}}},"cca3":"NFK"},{"name":{"common":"Oman","official":"Sultanate of Oman","nativeName":{"ara":{"official":"سلطنة عمان","common":"عمان"}}},"cca3":"OMN"},{"name":{"common":"El Salvador","official":"Republic of El Salvador","nativeName":{"spa":{"official":"República de El Salvador","common":"El Salvador"}}},"cca3":"SLV"},{"name":{"common":"Lebanon","official":"Lebanese Republic","nativeName":{"ara":{"official":"الجمهورية اللبنانية","common":"لبنان"},"fra":{"official":"République libanaise","common":"Liban"}}},"cca3":"LBN"},{"name":{"common":"Vatican City","official":"Vatican City State","nativeName":{"ita":{"official":"Stato della Città del Vaticano","common":"Vaticano"},"lat":{"official":"Status Civitatis Vaticanæ","common":"Vaticanæ"}}},"cca3":"VAT"},{"name":{"common":"Ecuador","official":"Republic of Ecuador","nativeName":{"spa":{"official":"República del Ecuador","common":"Ecuador"}}},"cca3":"ECU"},{"name":{"common":"Guam","official":"Guam","nativeName":{"cha":{"official":"Guåhån","common":"Guåhån"},"eng":{"official":"Guam","common":"Guam"},"spa":{"official":"Guam","common":"Guam"}}},"cca3":"GUM"},{"name":{"common":"Croatia","official":"Republic of Croatia","nativeName":{"hrv":{"official":"Republika Hrvatska","common":"Hrvatska"}}},"cca3":"HRV"},{"name":{"common":"Solomon Islands","official":"Solomon Islands","nativeName":{"eng":{"official":"Solomon Islands","common":"Solomon Islands"}}},"cca3":"SLB"},{"name":{"common":"Honduras","official":"Republic of Honduras","nativeName":{"spa":{"official":"República de Honduras","common":"Honduras"}}},"cca3":"HND"},{"name":{"common":"Christmas Island","official":"Territory of Christmas Island","nativeName":{"eng":{"official":"Territory of Christmas Island","common":"Christmas Island"}}},"cca3":"CXR"},{"name":{"common":"Puerto Rico","official":"Commonwealth of Puerto Rico","nativeName":{"eng":{"official":"Commonwealth of Puerto Rico","common":"Puerto Rico"},"spa":{"official":"Estado Libre Asociado de Puerto Rico","common":"Puerto Rico"}}},"cca3":"PRI"}]
  build: import console from 'console'
import {getCountryCodeToName} from "./APICalls"

export {checkMultipleOptions, getCurrencies, getLanguages, getBorders}

function checkMultipleOptions(response) {
  let multipleOptions = []
  if (response.length > 1) {
    response.forEach(item => multipleOptions.push({
      "countryName": item['name']['common'],
      "countryOfficialName": item['name']['official'],
      "countryCode": item['cca3']
    }))
  }
  return multipleOptions
}

function getCurrencies(response) {
    let currencies= []
    let curr = response[0]['currencies']
    for (var abb in curr) {
      currencies.push(curr[abb]["name"])
    }
    return currencies
}

function getLanguages(response) {
    let languages= []
    let lang = response[0]['languages']
    for (var labb in lang) {
      languages.push(lang[labb])
    }
    return languages
}

// Get all borders of the country and convert code into country name
function getBorders (response, debug) {
    let borders = []
    let bords = response[0]['borders']
    let codeResponse = getCountryCodeToName(debug)
    if (!codeResponse) {
      // Gracefully degrade if cannot get country code data
      return null
    }
    if (bords) {
      for (const code of bords) {
        let name = codeResponse.find(x => x.cca3.includes(code)).name.common
        borders.push(name)
      }
    }
    return borders
}

    runs-on: import console from 'console'
import {OVERRIDES} from "./lib/Data"
import {getCountryInfo} from "./lib/APICalls"
import {checkMultipleOptions, getCurrencies, getLanguages, getBorders} from "./lib/DataUtilities"
const debug = false

export default function ({countryName, countryCode}) {

   if (debug) console.log ("country name = " + countryName + " countryCode = " + countryCode)



  // Over rides provide shorts cuts to common queries e.g. if you enter "united states" the over ride sets the country code to USA and uses it
  // This is necessary because the API supports partial matches
  if (OVERRIDES.get(countryName)) {
    countryCode = OVERRIDES.get(countryName.toLowerCase())
    countryName = null
    if (debug) console.log ("in override, countryCode = " + countryCode)
  }

  let response = getCountryInfo(countryName, countryCode, debug)

  if (debug) console.log ("response = " + JSON.stringify(response))

  let currencies = [], languages = [], borders = []

  // Check if multiple countries matched
  let multipleOptions =  checkMultipleOptions(response)

  if (!multipleOptions.length > 0) {

    // Get all the currencies the country uses
    currencies = getCurrencies(response)

    // Get all the languages the country uses
    languages = getLanguages(response) 

    // Get the borders of the country
    borders = getBorders (response, debug) 
  }

  return {
    "commonName": response[0]['name']['common'],
    "officialName": response[0]['name']['official'],
    "countryCode": response[0]['cca3'],
    "currencyNames": currencies,
    "capital": response[0]['capital'][0],
    "languages": languages,
    "borders": borders,
    "googleMapsURL": response[0]['maps']['googleMaps'],
    "population": response[0]['population'],
    "continents": response[0]['continents'],
    "flagURL": response[0]['flags']['png'] + "",
    "multipleOptions": multipleOptions,
  }
}

    steps: action (CountryAction) {
  description (Handle request for country info)
  type (Search)
  collect {
    input-group (countryInput) {
      requires (OneOf)
      collect {
        input (countryName) {
          type (CountryName)
          min (Optional) max (One)
        }
        input (countryCode) {
          type (CountryCode)
          min (Optional) max (One)
        }
      }
    }
  }
  output (Country) {
   throws {
     error(CountryNotFound) {
       on-catch {
         replan {
           dialog ("Sorry I cannot find a country named #{value(countryName)}.")
           intent {
             goal: CountryAction
           }
         }
       }
     }
     error (APIFailure) {
       on-catch {
         halt {
           dialog("Sorry I'm unable to answer questions right now")
         }
       }
     }
   }
  }
}
    - uses: structure (Country) {
  description (CountryInfo)
  property (commonName) {
    type (CountryName)
    visibility (Private)
    min (Required) max (One)
  }
  property (officialName) {
    type (viv.core.Text)
    visibility (Private)
    min (Required) max (One)
  }
  property (countryCode) {
    type (CountryCode)
    visibility (Private)
    min (Required) max (Many)
  }
  property (currencyNames) {
    type (viv.core.Text)
    visibility (Private)
    min (Optional) max (Many)
  }
  property (capital) {
    type (viv.core.Text)
    visibility (Private)
    min (Required) max (One)
  }
  property (languages) {
    type (viv.core.Text)
    visibility (Private)
    min (Optional) max (Many)
  }
  property (borders) {
    type (viv.core.Text)
    visibility (Private)
    min (Optional) max (Many)
  }
  property (population) {
    type (viv.core.Integer)
    visibility (Private)
    min (Required) max (One)
  }
  property (continents) {
    type (viv.core.Text)
    visibility (Private)
    min (Required) max (Many)
  }
  property (flagURL) {
    type (viv.core.Name)
    visibility (Private)
    min (Required) max (One)
  }
  property (googleMapsURL) {
    type (viv.core.Text)
    visibility (Private)
    min (Optional) max (One)
  }
  property (multipleOptions) {
    type (MultipleOptions)
    min (Optional) max (Many)
  }
}
    - name: name (CountryCode) {
  description (3 letter country code)
}
      run: name (CountryName) {
  description (CountryName)
}
    - name: structure (MultipleOptions) {
  description (Multiple options for country)
  property (countryName) {
    type (CountryName)
    min (Required) max (One)
  }
  property (countryOfficialName) {
    type (viv.core.Text)
    visibility (Private)
    min (Required) max (One)
  }
  property (countryCode) {
    type (CountryCode)
    min (Required) max (One)
  }
}
      run: cargo test --verbose
