# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Elixir CI

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

-define(JOIN_LOCK_ID(REQUESTER), {mria_sync_join, REQUESTER}). 
    branches: [ "main" ]

permissions: #!/bin/bash
set -euxo pipefail

echo "Building "import ZonedDateTime from './zoned-date-time-polyfill.js';
import http from 'http';
import * as properties from "./properties.js";
import movieGenreMap from "./movieGenreMap.js";
import console from "console";

export function discoverMovie(releaseDateTimeExpression, person, genre, $vivContext) {
  const url = 'https://api.themoviedb.org/3/discover/movie';
  const query = {
    api_key: properties.get('secret', 'apiKey'),
  };
  if (releaseDateTimeExpression) {
    Object.assign(query, buildReleaseQuery(releaseDateTimeExpression, $vivContext));
  }
  if (person) {
    query['with_people'] = person.$id;
  }
  if (genre) {
    query['with_genres'] = mapMovieGenre(genre);
  }
  const options = {
    format: 'json',
    query: query,
  };
  const response = http.getUrl(url, options);
  return response;
}

export function getConfiguration() {
  const url = 'https://api.themoviedb.org/3/configuration';
  const query = {
    api_key: properties.get('secret', 'apiKey'),
  };
  const options = {
    format: 'json',
    query: query,
    cacheTime: 86400000, // 1 day cache time
  };
  const response = http.getUrl(url, options);
  return response;
}

export function getMovie(movie) {
  if (movie) {
    const url = 'https://api.themoviedb.org/3/movie/' + movie.$id;
    const query = {
      api_key: properties.get('secret', 'apiKey'),
    };
    const options = {
      format: 'json',
      query: query,
    };
    const response = http.getUrl(url, options);
    return response;
  }
}

export function getMovieCredits(movie) {
  if (movie) {
    const url = 'https://api.themoviedb.org/3/movie/' + movie.$id + '/credits';
    const query = {
      api_key: properties.get('secret', 'apiKey'),
    };
    const options = {
      format: 'json',
      query: query,
    };
    const response = http.getUrl(url, options);
    return response;
  }
}

export function getTrendingMovies(dateTimeExpression, $vivContext) {
  ZonedDateTime.setVivContext($vivContext);
  const inputType = 'viv.time.DateTimeExpression';
  // Default to now
  dateTimeExpression = dateTimeExpression || {
    dateTime: ZonedDateTime.now().getDateTime(),
    $type: inputType,
  };
  if (dateTimeExpression['$type'] === inputType) {
    const url =
      'https://api.themoviedb.org/3/trending/movie/' +
      buildTrendingWindow(dateTimeExpression);
    const query = {
      api_key: properties.get('secret', 'apiKey'),
    };
    const options = {
      format: 'json',
      query: query,
    };
    const response = http.getUrl(url, options);
    return response;
  } else {
    throw (
      'Unrecognized input type. Expected ' +
      inputType +
      '. Got: ' +
      JSON.stringify(dateTimeExpression)
    );
  }
}

export function searchPerson(name) {
  if (name) {
    const url = 'https://api.themoviedb.org/3/search/person';
    const query = {
      api_key: properties.get('secret', 'apiKey'),
    };
    if (name) {
      query['query'] = name;
    }
    const options = {
      format: 'json',
      query: query,
    };
    const response = http.getUrl(url, options);
    return response;
  }
}

function mapMovieGenre(bxbGenre) {
  bxbGenre = String(bxbGenre);
  for (var jsGenre in movieGenreMap) {
    if (movieGenreMap[jsGenre]['bxb'] === bxbGenre) {
      return movieGenreMap[jsGenre]['tmdbId'];
    }
  }
}

function buildReleaseQuery(releaseDateTimeExpression, $vivContext) {
  if (releaseDateTimeExpression) {
    if (releaseDateTimeExpression.date) {
      return buildPunctualReleaseQuery(
        inflateDate(releaseDateTimeExpression.date, $vivContext)
      );
    }
    if (releaseDateTimeExpression.dateTime) {
      return buildPunctualReleaseQuery(
        inflateDateTime(releaseDateTimeExpression.dateTime, $vivContext)
      );
    }
    if (releaseDateTimeExpression.dateInterval) {
      return buildIntervalReleaseQuery(
        inflateDate(releaseDateTimeExpression.dateInterval.start, $vivContext),
        inflateDate(releaseDateTimeExpression.dateInterval.end, $vivContext)
      );
    }
    if (releaseDateTimeExpression.dateTimeInterval) {
      return buildIntervalReleaseQuery(
        inflateDateTime(releaseDateTimeExpression.dateTimeInterval.start),
        inflateDateTime(releaseDateTimeExpression.dateTimeInterval.end)
      );
    }
  }
}

function buildPunctualReleaseQuery(zonedDateTime) {
  if (zonedDateTime) {
    return {
      primary_release_year: zonedDateTime.getYear(),
    };
  }
}

function buildIntervalReleaseQuery(zonedDateTimeStart, zonedDateTimeEnd) {
  const query = {};
  if (zonedDateTimeStart) {
    query['primary_release_date.gte'] = zonedDateTimeStart.toIsoString();
  }
  if (zonedDateTimeEnd) {
    query['primary_release_date.lte'] = zonedDateTimeEnd.toIsoString();
  }
  return query;
}

function buildTrendingWindow(dateTimeExpression, $vivContext) {
  ZonedDateTime.setVivContext($vivContext);
  const now = ZonedDateTime.now();
  if (dateTimeExpression) {
    if (dateTimeExpression.date) {
      return buildPunctualTrendingWindow(
        now,
        inflateDate(dateTimeExpression.date, $vivContext)
      );
    }
    if (dateTimeExpression.dateTime) {
      return buildPunctualTrendingWindow(
        now,
        inflateDateTime(dateTimeExpression.dateTime, $vivContext)
      );
    }
    if (dateTimeExpression.dateInterval) {
      return buildPunctualTrendingWindow(
        now,
        inflateDate(dateTimeExpression.dateInterval.start, $vivContext)
      );
    }
    if (dateTimeExpression.dateTimeInterval) {
      return buildPunctualTrendingWindow(
        now,
        inflateDateTime(dateTimeExpression.dateTimeInterval.start, $vivContext)
      );
    }
  }
}

function buildPunctualTrendingWindow(now, zonedDateTime) {
  if (now && zonedDateTime) {
    if (zonedDateTime.isBeforeOrEqualTo(now)) {
      const daysUntil = zonedDateTime.durationUntil(now, 'Days').periodDays;
      if (daysUntil <= 1) {
        return 'day';
      }
      if (daysUntil <= 7) {
        return 'week';
      }
    }
  }
  throw (
    'Unsupported trending window. Expected zonedDateTime up to 1 week ago. Got ' +
    JSON.stringify(zonedDateTime)
  );
}

function buildIntervalTrendingWindow(
  now,
  zonedDateTimeStart,
  zonedDateTimeEnd
) {
  if (now) {
    buildPunctualTrendingWindow(now, zonedDateTimeEnd); // This will throw an error if the end is out of bounds
    return buildPunctualTrendingWindow(now, zonedDateTimeStart);
  }
}

function inflateDate(date, $vivContext) {
  ZonedDateTime.setVivContext($vivContext);
  
  if (date) {
    return ZonedDateTime.fromDate(date);
  }
}

function inflateDateTime(dateTime, $vivContext) {
  ZonedDateTime.setVivContext($vivContext);
  if (dateTime) {
    return ZonedDateTime.fromDateTime(dateTime);
  }
"

[ -d "// <js constant> : { "bxb": <Bixby Enum Symbol>, "tmdbId": <The Movie DB ID> }
export default {
  Horror: { bxb: 'Horror', tmdbId: 27 },
  Thriller: { bxb: 'Thriller', tmdbId: 53 },
  Mystery: { bxb: 'Mystery', tmdbId: 9648 },
  Crime: { bxb: 'Crime', tmdbId: 80 },
  Adventure: { bxb: 'Adventure', tmdbId: 12 },
  Action: { bxb: 'Action', tmdbId: 28 },
  Fantasy: { bxb: 'Fantasy', tmdbId: 14 },
  ScienceFiction: { bxb: 'ScienceFiction', tmdbId: 878 },
  Family: { bxb: 'Family', tmdbId: 10751 },
  Animation: { bxb: 'Animation', tmdbId: 16 },
  Comedy: { bxb: 'Comedy', tmdbId: 35 },
  Romance: { bxb: 'Romance', tmdbId: 10749 },
  Western: { bxb: 'Western', tmdbId: 37 },
  Drama: { bxb: 'Drama', tmdbId: 18 },
  History: { bxb: 'History', tmdbId: 36 },
  War: { bxb: 'War', tmdbId: 10752 },
  Documentary: { bxb: 'Documentary', tmdbId: 99 },
  Music: { bxb: 'Music', tmdbId: 10402 },
};" ] ||
    git clone "import ZonedDateTime from './zoned-date-time-polyfill.js';
import * as tmdb from "./TMDB.js";
import movieGenreMap from "./movieGenreMap.js";
import console from "console";

export function parseCredits(tmdbResponse) {
  if (tmdbResponse) {
    const configuration = tmdb.getConfiguration();
    return {
      cast: parseCast(tmdbResponse.cast, configuration),
      crew: parseCrew(tmdbResponse.crew, configuration),
    };
  }
}

function parseCast(rawCast, configuration) {
  if (rawCast) {
    return rawCast.map(function (rawCastMember) {
      return {
        name: rawCastMember.name,
        profileUrl: constructImageUrl(
          configuration.images.base_url,
          'original',
          rawCastMember.profile_path
        ),
        character: rawCastMember.character,
      };
    });
  }
}

function parseCrew(rawCrew, configuration) {
  if (rawCrew) {
    // Group crew members by departments
    return rawCrew.reduce(function (departments, rawCrewMember) {
      var departmentIndex = departments.findIndex(function (department) {
        return department.department === rawCrewMember.department;
      });
      if (departmentIndex === -1) {
        departmentIndex =
          departments.push({
            department: rawCrewMember.department,
            team: [],
          }) - 1;
      }
      departments[departmentIndex].team.push({
        name: rawCrewMember.name,
        profileUrl: constructImageUrl(
          configuration.images.base_url,
          'original',
          rawCrewMember.profile_path
        ),
        job: rawCrewMember.job,
      });
      return departments;
    }, []);
  }
}

export function parseMovieDetails(tmdbResponse) {
  if (tmdbResponse) {
    return {
      budget: tmdbResponse.budget,
      overview: tmdbResponse.overview,
      revenue: tmdbResponse.revenue,
      runtime: {
        periodMinutes: tmdbResponse.runtime,
      },
      tagline: tmdbResponse.tagline,
    };
  }
}

export function parseMovies(tmdbResponse, $vivContext) {
  if (tmdbResponse && tmdbResponse.results && tmdbResponse.results.length > 0) {
    const configuration = tmdb.getConfiguration();
    return tmdbResponse.results.map(function (rawMovie) {
      return parseMovie(rawMovie, configuration, $vivContext);
    });
  }
}

function parseMovie(rawMovie, configuration, $vivContext) {
  if (rawMovie && configuration) {
    return {
      $id: rawMovie.id,
      title: rawMovie.title,
      releaseDate: parseReleaseDate(rawMovie.release_date, $vivContext),
      genre: rawMovie.genre_ids
        .map(function (id) {
          return parseMovieGenre(id);
        })
        .filter((x) => x),
      posterUrl: constructImageUrl(
        configuration.images.base_url,
        'original',
        rawMovie.poster_path
      ),
      backdropUrl: constructImageUrl(
        configuration.images.base_url,
        'original',
        rawMovie.backdrop_path
      ),
    };
  }
}

function parseReleaseDate(rawReleaseDate, $vivContext) {
  ZonedDateTime.setVivContext($vivContext);
  if (rawReleaseDate) {
    return ZonedDateTime.parseDate(
      rawReleaseDate,
    ).getDateTime().date;
  }
}

function parseMovieGenre(tmdbId) {
  for (var jsGenre in movieGenreMap) {
    if (movieGenreMap[jsGenre]['tmdbId'] === tmdbId) {
      return movieGenreMap[jsGenre]['bxb'];
    }
  }
}

export function parsePeople(tmdbResponse) {
  if (tmdbResponse && tmdbResponse.results && tmdbResponse.results.length > 0) {
    const configuration = tmdb.getConfiguration();
    return tmdbResponse.results.map(function (rawPerson) {
      return parsePerson(rawPerson, configuration);
    });
  }
}

function parsePerson(rawPerson, configuration) {
  if (rawPerson && configuration) {
    return {
      $id: rawPerson.id,
      name: rawPerson.name,
      profileUrl: constructImageUrl(
        configuration.images.base_url,
        'original',
        rawPerson.profile_path
      ),
    };
  }
}

function constructImageUrl(baseUrl, size, endingUrl) {
  if (baseUrl && size && endingUrl) {
    return baseUrl + size + endingUrl;
  }
}" "import config from 'config';
import secret from 'secret';

export function get(type, property) {
  const override = config.get('override-properties') === 'true';
  switch (type) {
    case 'config':
      return override ? config.get('config.' + property) : config.get(property);
    case 'secret':
      return override ? config.get('secret.' + property) : secret.get(property);
    default:
      throw 'Unrecognized type: ' + type;
  }
}"

cd "import questionsJson from "./quizQuestions.js";
import config from 'config';
import console from 'console';

export function initializeQuiz() {
  var quiz = {
    completed: false,
    currentQuestion: 0,
    questions: buildQuestions(),
  };
  return quiz;
}

export function updateQuiz(quiz, answer) {
  // See if answer corresponds to any of the current question options
  var matchedIndex = quiz.questions[quiz.currentQuestion].options.findIndex(
    function (option) {
      // Check alias
      if (stringCompare(answer, option.alias)) {
        return true;
      }
      // Check match
      return option.match.find(function (pattern) {
        return regexCompare(answer, pattern);
      });
    }
  );
  if (matchedIndex >= 0) {
    // Save the matched index
    quiz.questions[quiz.currentQuestion].selectedOption = matchedIndex;
    // Advance to next question
    quiz.currentQuestion++;
    if (quiz.currentQuestion < quiz.questions.length) {
      // continue
    } else {
      // Mark quiz as completed
      quiz.completed = true;
      // Tally answers
      quiz.finalOutcome = computeOutcome(quiz);
    }
  } else {
    // Leave quiz unmodified and it will reprompt for the current question
  }
  return quiz;
}

function stringCompare(a, b) {
  return a.toLowerCase() === b.toLowerCase();
}

function regexCompare(textToSearch, regexPatternAsString) {
  var regex = new RegExp(regexPatternAsString, 'i'); // Ignore case
  return regex.test(textToSearch);
}

function computeOutcome(quiz) {
  if (!quiz || !quiz.completed) {
    throw 'Incomplete quiz';
  }
  var tally = {};
  quiz.questions.forEach(function (question) {
    question.options[question.selectedOption].outcome.forEach(function (
      outcome
    ) {
      tally[outcome] = tally[outcome] + 1 || 1;
    });
  });
  console.info('Tally outcomes', JSON.stringify(tally));
  var topOutcome;
  for (var outcome in tally) {
    if (!topOutcome || tally[outcome] > tally[topOutcome]) {
      topOutcome = outcome;
    }
  }
  console.info('Top outcome', topOutcome);
  return topOutcome;
}

function buildQuestions() {
  // Randomly pick N questions
  var questionsJsonResult = getRandomSubarray(
    questionsJson,
    config.get('movieGenreQuizLength')
  );
  return questionsJsonResult.map(function (question) {
    return buildQuestion(question);
  });
}

function buildQuestion(questionJson) {
  return {
    text: questionJson.question,
    options: buildOptions(questionJson.options),
  };
}

function buildOptions(optionsJson) {
  return optionsJson.map(function (option, index) {
    return buildOption(option, index);
  });
}

function buildOption(optionJson, index) {
  return {
    text: optionJson.text,
    alias: String.fromCharCode('A'.charCodeAt(0) + index),
    match: buildMatch(optionJson.match),
    outcome: buildOutcome(optionJson.outcome),
    commentary: optionJson.commentary,
  };
}

function buildMatch(matchJson) {
  return matchJson;
}

function buildOutcome(outcomeJson) {
  return outcomeJson;
}

// http://stackoverflow.com/questions/11935175/sampling-a-random-subset-from-an-array
function getRandomSubarray(arr, size) {
  var shuffled = arr.slice(0),
    i = arr.length,
    min = i - size,
    temp,
    index;
  while (i-- > min) {
    index = Math.floor((i + 1) * Math.random());
    temp = shuffled[index];
    shuffled[index] = shuffled[i];
    shuffled[i] = temp;
  }
  return shuffled.slice(min);
}"
git checkout "import movieGenreMap from "./movieGenreMap.js";

/**
 * An array of quiz questions.
 * The questions are of multiple choice format, where the user must select a single option.
 * There is no right or wrong answer. Each option defines an array of outcomes that are tallied to calculate a final quiz result.
 *
 * Example:
 * {
 *   question: "What animal would you want to be for one day?"          // The question statement
 *   options: [                                                         // An array of multiple choice options
 *     {                                                                // An option
 *       text: "Bottlenose Dolphin",                                    // The option statement
 *       match: [                                                       // An array of regex match patterns to look for in the user's response. If a match is found, this option will be selected.
 *         "bottle[-\s]?nose", "dolphin"                                // For example: "bottle nose", "bottle-nose", "I wanna be a dolphin"
 *       ],
 *       outcome: [                                                     // An array of outcomes if this option is chosen
 *         movieGenreMap.Documentary.bxb,                               // A movie genre in bxb format from the movieGenreMap.js file
 *         movieGenreMap.Music.bxb,
 *         movieGenreMap.ScienceFiction.bxb
 *       ],
 *       commentary: "Smart choice."                                      // Bixby response feedback if the user chooses this option
 *     },
 *     {
 *       text: "Black Cat",
 *       match: [
 *         "cat"
 *       ],
 *       outcome: [
 *         movieGenreMap.Horror.bxb,
 *         movieGenreMap.Mystery.bxb,
 *         movieGenreMap.Romance.bxb
 *       ],
 *       commentary: "Yay nap time!"
 *     },
 *     {
 *       text: "Bald Eagle",
 *       match: [
 *         "bald", "eagle"
 *       ],
 *       outcome: [
 *         movieGenreMap.Drama.bxb,
 *         movieGenreMap.Thriller.bxb,
 *         movieGenreMap.War.bxb
 *       ],
 *       commentary: "Very majestic."
 *     },
 *     {
 *       text: "Golden Retriever",
 *       match: [
 *         "goldem, "retriever", "dog"
 *       ],
 *       outcome: [
 *         movieGenreMap.Animation.bxb,
 *         movieGenreMap.Comedy.bxb,
 *         movieGenreMap.Family.bxb
 *       ],
 *       commentary: "Good choice."
 *      }
 *   ]
 * }
 *
 **/

export default [
  {
    question: 'What type of traveler are you?',
    options: [
      {
        text: 'Sleeping in the woods, Wi-Fi optional',
        match: ['sleeping', 'woods', 'wi[-s]fi'],
        outcome: [
          movieGenreMap.Action.bxb,
          movieGenreMap.Adventure.bxb,
          movieGenreMap.Horror.bxb,
          movieGenreMap.Thriller.bxb,
        ],
        commentary: 'Sounds fun!',
      },
      {
        text: 'All-inclusive cruise to an island resort',
        match: ['all[-s]inclusive', 'cruise', 'island', 'resort'],
        outcome: [
          movieGenreMap.Comedy.bxb,
          movieGenreMap.Family.bxb,
          movieGenreMap.Romance.bxb,
        ],
        commentary: 'Sounds relaxing!',
      },
    ],
  },
  {
    question:
      'We can get you first class seats! The cabin crew would like to know how you prefer your steak.',
    options: [
      {
        text: 'Well-done',
        match: ['well[-s]done'],
        outcome: [movieGenreMap.History.bxb, movieGenreMap.Western.bxb],
        commentary: 'Alright.',
      },
      {
        text: 'Medium',
        match: ['medium'],
        outcome: [movieGenreMap.Mystery.bxb, movieGenreMap.Drama.bxb],
        commentary: 'Got it.',
      },
      {
        text: 'Rare',
        match: ['rare'],
        outcome: [
          movieGenreMap.Crime.bxb,
          movieGenreMap.Horror.bxb,
          movieGenreMap.War.bxb,
        ],
        commentary: 'Bloody.',
      },
      {
        text: 'No meat please',
        match: ['no meat', 'vegetarian', 'vegan'],
        outcome: [movieGenreMap.Documentary.bxb],
        commentary: 'No problem! We have plenty of Impossible Burgers.',
      },
    ],
  },
  {
    question:
      'Your friend wants to go bungee jumping on the trip. What do you say?',
    options: [
      {
        text: 'Yes! When?',
        match: ['yes', 'yeah', 'sure', 'of course', 'when'],
        outcome: [
          movieGenreMap.Action.bxb,
          movieGenreMap.Adventure.bxb,
          movieGenreMap.Thriller.bxb,
          movieGenreMap.War.bxb,
          movieGenreMap.Western.bxb,
          movieGenreMap.ScienceFiction.bxb,
        ],
        commentary: 'Cowabunga it is.',
      },
      {
        text: 'Maybe...',
        match: ['maybe', 'perhaps', 'not sure'],
        outcome: [
          movieGenreMap.Comedy.bxb,
          movieGenreMap.Crime.bxb,
          movieGenreMap.Mystery.bxb,
          movieGenreMap.Romance.bxb,
        ],
        commentary: 'Understood.',
      },
      {
        text: 'No way.',
        match: ['no', 'nope', 'nah'],
        outcome: [movieGenreMap.Family.bxb],
        commentary: 'No problem.',
      },
    ],
  },
  {
    question: 'Where would you prefer to sit on your movie adventure?',
    options: [
      {
        text: 'Window seat',
        match: ['window'],
        outcome: [movieGenreMap.Adventure.bxb, movieGenreMap.Action.bxb],
        commentary: "I'll get you a movie with exhilarating views.",
      },
      {
        text: 'Aisle seat',
        match: ['aisle'],
        outcome: [movieGenreMap.Animation.bxb, movieGenreMap.Comedy.bxb],
        commentary: "Sure, I'll get you something comfortable.",
      },
    ],
  },
  {
    question: 'What snack would you like for the trip?',
    options: [
      {
        text: 'Popcorn',
        match: ['popcorn'],
        outcome: [movieGenreMap.Thriller.bxb, movieGenreMap.Action.bxb],
        commentary: 'Classic.',
      },
      {
        text: 'Chocolate',
        match: ['chocolate'],
        outcome: [movieGenreMap.Fantasy.bxb, movieGenreMap.Romance.bxb],
        commentary: 'Smooth.',
      },
      {
        text: 'Fresh fruits & veggies',
        match: ['fruit', 'veggie', 'vegetable'],
        outcome: [movieGenreMap.Documentary.bxb, movieGenreMap.History.bxb],
        commentary: 'Healthy.',
      },
      {
        text: 'Leftover pizza',
        match: ['pizza'],
        outcome: [movieGenreMap.Animation.bxb, movieGenreMap.Comedy.bxb],
        commentary: 'Great.',
      },
    ],
  },
  {
    question: 'Will your kids be joining you on this trip?',
    options: [
      {
        text: 'Yes',
        match: [
          'yes',
          'yeah',
          'yep',
          'yup',
          'sure',
          'of course',
          'maybe',
          'probably',
          'definitely',
        ],
        outcome: [movieGenreMap.Animation.bxb, movieGenreMap.Family.bxb],
        commentary: 'Got it.',
      },
      {
        text: 'No kids!',
        match: [
          'no',
          'nope',
          'no kids',
          "don't have kids",
          'no children',
          "don't have children",
        ],
        outcome: [
          movieGenreMap.Crime.bxb,
          movieGenreMap.Horror.bxb,
          movieGenreMap.Romance.bxb,
          movieGenreMap.War.bxb,
        ],
        commentary: 'Okay.',
      },
    ],
  },
  {
    question:
      'Which historical figure would you prefer as a weekend travel companion?',
    options: [
      {
        text: 'Teddy Roosevelt, a former US president',
        match: ['teddy', 'theodore', 'roosevelt', 'president'],
        outcome: [movieGenreMap.Adventure.bxb, movieGenreMap.Western.bxb],
        commentary: 'Sounds like an adventure!',
      },
      {
        text: 'Kurt Vonnegut, an American novelist',
        match: ['kurt', 'vonnegut', 'novelist'],
        outcome: [movieGenreMap.ScienceFiction.bxb, movieGenreMap.War.bxb],
        commentary: 'So it goes.',
      },
      {
        text: 'Janis Joplin, an American blues singer',
        match: ['janis', 'joplin', 'singer'],
        outcome: [movieGenreMap.Drama.bxb, movieGenreMap.Music.bxb],
        commentary: 'Sounds like a party!',
      },
      {
        text: 'Frida Kahlo, a Mexican painter',
        match: ['frida', 'kahlo', 'painter'],
        outcome: [movieGenreMap.Animation.bxb, movieGenreMap.Fantasy.bxb],
        commentary: 'Bien!',
      },
    ],
  },
  {
    question:
      "There's a chance of rain in the forecast. How do you like to spend time indoors?",
    options: [
      {
        text: 'Solving puzzles and reading',
        match: ['puzzles', 'reading'],
        outcome: [
          movieGenreMap.Crime.bxb,
          movieGenreMap.Documentary.bxb,
          movieGenreMap.Mystery.bxb,
        ],
        commentary: 'That sounds fun.',
      },
      {
        text: 'Playing video games',
        match: ['games'],
        outcome: [movieGenreMap.Music.bxb, movieGenreMap.ScienceFiction.bxb],
        commentary: 'Great!',
      },
      {
        text: 'Hitting the gym',
        match: ['gym'],
        outcome: [movieGenreMap.Thriller.bxb],
        commentary: 'Got it!',
      },
    ],
  },
  {
    question: 'What type of book are you bringing with you for the trip?',
    options: [
      {
        text: 'A high fantasy used paperback',
        match: ['fantasy', 'used', 'paperback'],
        outcome: [
          movieGenreMap.Drama.bxb,
          movieGenreMap.Fantasy.bxb,
          movieGenreMap.Music.bxb,
          movieGenreMap.Romance.bxb,
        ],
        commentary: 'Fantastic!',
      },
      {
        text: 'A new self-improvement bestseller',
        match: [
          'self[-s]improvement',
          'self[-s]help',
          'bestseller',
          'best seller',
        ],
        outcome: [
          movieGenreMap.Documentary.bxb,
          movieGenreMap.Music.bxb,
          movieGenreMap.Family.bxb,
        ],
        commentary: 'You can do it!',
      },
      {
        text: 'A couple scientific papers on an e-reader',
        match: ['scientific', 'papers', 'e[-s]reader'],
        outcome: [
          movieGenreMap.Documentary.bxb,
          movieGenreMap.History.bxb,
          movieGenreMap.ScienceFiction.bxb,
        ],
        commentary: 'Indeed.',
      },
      {
        text: 'Manga',
        match: ['manga', 'comics'],
        outcome: [
          movieGenreMap.Animation.bxb,
          movieGenreMap.Comedy.bxb,
          movieGenreMap.Fantasy.bxb,
        ],
        commentary: 'Cool!',
      },
    ],
  },
  {
    question: 'Are you a night owl or an early bird?',
    options: [
      {
        text: "Night owl. I'll sleep when I'm dead.",
        match: ['night', 'owl', 'dead'],
        outcome: [
          movieGenreMap.Crime.bxb,
          movieGenreMap.Horror.bxb,
          movieGenreMap.Mystery.bxb,
          movieGenreMap.Thriller.bxb,
        ],
        commentary: "Okay, I'll find something for a late night.",
      },
      {
        text: "Neither. I'm a sloth.",
        match: ['neither', 'sloth'],
        outcome: [
          movieGenreMap.Drama.bxb,
          movieGenreMap.War.bxb,
          movieGenreMap.Fantasy.bxb,
          movieGenreMap.ScienceFiction.bxb,
        ],
        commentary: "Okay, I'll find something that will keep you awake.",
      },
      {
        text: "Early bird. Give me coffee and I'm ready for anything.",
        match: ['early', 'bird', 'coffee', 'ready', 'anything'],
        outcome: [
          movieGenreMap.Action.bxb,
          movieGenreMap.Adventure.bxb,
          movieGenreMap.History.bxb,
          movieGenreMap.Western.bxb,
        ],
        commentary: "Okay, I'll find something interesting.",
      },
    ],
  },
];"

# Avoid rebuilding rocksdb:
mkdir -p "_checkouts"
pushd "_checkouts"
[ -L rocksdb ] ||
    ln -s "/*! Dates Library v1.1.4 */
const BixbyDurationUnit = {
    YEARS: "Years",
    MONTHS: "Months",
    DAYS: "Days",
    HOURS: "Hours",
    MINUTES: "Minutes",
    SECONDS: "Seconds",
    MILLIS: "Millis",
};
export function durationFromMilliseconds(milliseconds) {
    const duration = {
        periodDays: 0,
        periodHours: 0,
        periodMinutes: 0,
        periodSeconds: 0,
        periodMilliseconds: 0,
    };
    duration.periodMilliseconds = milliseconds % 1000;
    milliseconds = Math.floor(milliseconds / 1000);
    duration.periodSeconds = milliseconds % 60;
    milliseconds = Math.floor(milliseconds / 60);
    duration.periodMinutes = milliseconds % 60;
    milliseconds = Math.floor(milliseconds / 60);
    duration.periodHours = milliseconds % 24;
    milliseconds = Math.floor(milliseconds / 24);
    duration.periodDays = milliseconds;
    return duration;
}
export default class ZonedDateTime {
    static setVivContext(vivContext) {
        ZonedDateTime._vivContext = vivContext;
    }
    static _getTimeZoneOffset(timeZone, millisecondsFromEpoch) {
        const formatter = new Intl.DateTimeFormat("en-GB", {
            timeZone,
            timeZoneName: "short",
        });
        const timeZoneOffset = formatter
            .formatToParts(new Date(millisecondsFromEpoch))
            .find((part) => part.type === "timeZoneName")?.value;
        if (timeZoneOffset === undefined || timeZoneOffset === "UTC") {
            return { offsetHours: 0, offsetMinutes: 0 };
        }
        const timeZoneOffsetValue = timeZoneOffset.split("GMT")[1];
        const isNegative = timeZoneOffsetValue.includes("-");
        const offsetHours = timeZoneOffsetValue.includes(":")
            ? parseInt(timeZoneOffsetValue.split(":")[0], 10)
            : parseInt(timeZoneOffsetValue, 10);
        const offsetMinutes = timeZoneOffsetValue.includes(":")
            ? parseInt(timeZoneOffsetValue.split(":")[1], 10)
            : 0;
        return {
            offsetHours,
            offsetMinutes: isNegative ? -offsetMinutes : offsetMinutes,
        };
    }
    static now(timeZone) {
        return new ZonedDateTime(timeZone ?? ZonedDateTime._vivContext.timezone, ZonedDateTime._vivContext.testToday ?? Date.now());
    }
    static of(timeZoneId, year = 0, month = 1, day = 1, hour = 0, minute = 0, second = 0, millisecond = 0) {
        const timeZone = timeZoneId ?? ZonedDateTime._vivContext.timezone;
        const millisecondsFromEpoch = Date.UTC(year, month - 1, day, hour, minute, second, millisecond);
        const { offsetHours, offsetMinutes } = ZonedDateTime._getTimeZoneOffset(timeZone, millisecondsFromEpoch);
        return new ZonedDateTime(timeZone, millisecondsFromEpoch - (offsetHours * 60 + offsetMinutes) * 60 * 1000);
    }
    static fromDate(date) {
        const zdt = new ZonedDateTime();
        const millisecondsFromEpoch = Date.UTC(date.year ?? zdt.getYear(), date.month ? date.month - 1 : zdt.getMonth() - 1, date.day ?? zdt.getDay());
        return new ZonedDateTime(ZonedDateTime._vivContext.timezone, millisecondsFromEpoch);
    }
    static fromDateTime(dateTime) {
        const timeZoneId = dateTime?.time?.timezone ?? ZonedDateTime._vivContext.timezone;
        if (dateTime?.utcInstant) {
            return new ZonedDateTime(timeZoneId, dateTime.utcInstant);
        }
        const millisecondsFromEpoch = this._vivContext.testToday || Date.now();
        const zdt = new ZonedDateTime(timeZoneId, millisecondsFromEpoch);
        return ZonedDateTime.of(timeZoneId, dateTime?.date?.year ?? zdt.getYear(), dateTime?.date?.month ?? zdt.getMonth(), dateTime?.date?.day ?? zdt.getDay(), dateTime?.time?.hour ?? zdt.getHour(), dateTime?.time?.minute ?? zdt.getMinute(), dateTime?.time?.second ?? zdt.getSecond(), dateTime?.time?.millisecond ?? zdt.getMillisecond());
    }
    static parseDate(date, timeZoneId) {
        const dateRegex = /^(\d{4})-(\d{2})-(\d{2})$/;
        const match = date.match(dateRegex);
        if (match === null) {
            throw new Error("Invalid date time string. Must be in ISO 8601 format. For example, '2020-01-01T00:00:00.000Z'");
        }
        const [_date, year, month, day] = match;
        const zdt = new ZonedDateTime(timeZoneId ?? ZonedDateTime._vivContext.timezone, ZonedDateTime._vivContext.testToday ?? Date.now());
        return ZonedDateTime.of(timeZoneId, parseInt(year, 10), parseInt(month, 10), parseInt(day, 10), zdt.getHour(), zdt.getMinute(), zdt.getSecond(), zdt.getMillisecond());
    }
    static parseTime(time, timeZoneId) {
        const timeRegex = /^(\d{2}):(\d{2}):(\d{2})(?:\.(\d{1,3}))$/;
        const match = time.match(timeRegex);
        if (match === null) {
            throw new Error("Invalid time string. Must be in ISO 8601 format. For example, '00:00:00.000' or '00:00:00'");
        }
        const [_date, hour, minutes, second, milliseconds] = match;
        const zdt = new ZonedDateTime(timeZoneId ?? ZonedDateTime._vivContext.timezone, ZonedDateTime._vivContext.testToday ?? Date.now());
        return ZonedDateTime.of(timeZoneId, zdt.getYear(), zdt.getMonth(), zdt.getDay(), parseInt(hour, 10), parseInt(minutes, 10), parseInt(second, 10), parseInt(milliseconds, 10));
    }
    static parseDateTime(dateTime, timeZoneId) {
        const dateTimeRegex = /^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d{1,3}))?(?:(Z)|([+-]\d{2}(?::\d{2})?))?$/;
        const match = dateTime.match(dateTimeRegex);
        if (match === null) {
            throw new Error("Invalid date string. Must be in ISO 8601 format. For example, '2020-01-01T00:00:00.000Z' or '2020-01-01T00:00:00' or '2020-01-01T00:00:00.000+00:00''");
        }
        const [_dateTime, year, month, day, hour, minute, second, millisecond, isUtc, timeZoneOffset,] = match;
        let offsetHours = 0;
        let offsetMinutes = 0;
        if (!isUtc && timeZoneOffset !== undefined) {
            const offsetSign = timeZoneOffset.startsWith("-") ? -1 : 1;
            [offsetHours, offsetMinutes] = timeZoneOffset
                .slice(1)
                .split(":")
                .map((n) => parseInt(n, 10) * offsetSign);
        }
        return ZonedDateTime.of(timeZoneId, parseInt(year, 10), parseInt(month, 10), parseInt(day, 10), parseInt(hour, 10) + offsetHours, parseInt(minute, 10) + offsetMinutes, parseInt(second, 10), parseInt(millisecond ?? "0", 10));
    }
    static _parseDateTimeString() {
        return (input, timeZoneId) => {
            let milliseconds;
            if (input.includes("T") &&
                !input.includes("Z") &&
                (input.includes("+") || input.includes("-"))) {
                const [dateStr, timeWithOffset] = input.split("T");
                const offset = timeWithOffset.match(/([+-]\d{2}):(\d{2})/);
                if (!offset) {
                    milliseconds = input.includes("Z")
                        ? Date.parse(input)
                        : Date.parse(input + "Z");
                }
                else {
                    const [offsetHours, offsetMinutes] = offset
                        .slice(1)
                        .map((s) => parseInt(s, 10));
                    milliseconds =
                        Date.parse(dateStr) +
                            (offsetHours * 60 + offsetMinutes) * 60 * 1000;
                }
            }
            else {
                milliseconds = Date.parse(input);
            }
            if (isNaN(milliseconds)) {
                throw new Error("Invalid date string. If you need pattern matching use a library like datetime or moment.js");
            }
            return new ZonedDateTime(timeZoneId ?? ZonedDateTime._vivContext.timezone, milliseconds);
        };
    }
    constructor(timeZoneIdOrGeoPoint, millisecondsFromEpoch) {
        if (ZonedDateTime._vivContext === undefined) {
            throw new Error("ZonedDateTime._vivContext is undefined. Call ZonedDateTime.setVivContext($vivContext) before initialization.");
        }
        if (typeof timeZoneIdOrGeoPoint === "object" &&
            "latitude" in timeZoneIdOrGeoPoint &&
            "longitude" in timeZoneIdOrGeoPoint) {
            throw new Error("ZonedDateTime not implemented using GeoPoint.");
        }
        this._timeZoneId =
            timeZoneIdOrGeoPoint ?? ZonedDateTime._vivContext.timezone;
        this._millisFromEpoch =
            millisecondsFromEpoch ??
                ZonedDateTime._vivContext.testToday ??
                Date.now();
    }
    getMillisFromEpoch() {
        return this._millisFromEpoch;
    }
    getTimeZoneId() {
        return this._timeZoneId;
    }
    getDateTime() {
        return {
            date: {
                year: this.getYear(),
                month: this.getMonth(),
                day: this.getDay(),
            },
            time: {
                hour: this.getHour(),
                minute: this.getMinute(),
                second: this.getSecond(),
                millisecond: this.getMillisecond(),
                timezone: this.getTimeZoneId(),
            },
            utcInstant: this.getMillisFromEpoch(),
        };
    }
    getDayOfWeek() {
        const date = new Date(this.getMillisFromEpoch());
        const dateTimeFormat = new Intl.DateTimeFormat("en-US", {
            timeZone: this.getTimeZoneId(),
            weekday: "short",
        });
        const parts = dateTimeFormat.formatToParts(date);
        const dateString = parts[0].value;
        switch (dateString) {
            case "Mon":
                return 1;
            case "Tue":
                return 2;
            case "Wed":
                return 3;
            case "Thu":
                return 4;
            case "Fri":
                return 5;
            case "Sat":
                return 6;
            case "Sun":
                return 7;
        }
    }
    getYear() {
        const date = new Date(this.getMillisFromEpoch());
        const dateTimeFormat = new Intl.DateTimeFormat("en-US", {
            timeZone: this.getTimeZoneId(),
            year: "numeric",
        });
        const parts = dateTimeFormat.formatToParts(date);
        return parseInt(parts[0].value, 10);
    }
    getMonth() {
        const date = new Date(this.getMillisFromEpoch());
        const dateTimeFormat = new Intl.DateTimeFormat("en-US", {
            timeZone: this.getTimeZoneId(),
            month: "numeric",
        });
        const parts = dateTimeFormat.formatToParts(date);
        return parseInt(parts[0].value, 10);
    }
    getDay() {
        const date = new Date(this.getMillisFromEpoch());
        const dateTimeFormat = new Intl.DateTimeFormat("en-US", {
            timeZone: this.getTimeZoneId(),
            day: "numeric",
        });
        const parts = dateTimeFormat.formatToParts(date);
        return parseInt(parts[0].value, 10);
    }
    getHour() {
        const date = new Date(this.getMillisFromEpoch());
        const dateTimeFormat = new Intl.DateTimeFormat("en-US", {
            timeZone: this.getTimeZoneId(),
            hour: "numeric",
            hour12: false,
        });
        const parts = dateTimeFormat.formatToParts(date);
        return parseInt(parts[0].value, 10) % 24;
    }
    getMinute() {
        const date = new Date(this.getMillisFromEpoch());
        const dateTimeFormat = new Intl.DateTimeFormat("en-US", {
            timeZone: this.getTimeZoneId(),
            minute: "numeric",
        });
        const parts = dateTimeFormat.formatToParts(date);
        return parseInt(parts[0].value, 10);
    }
    getSecond() {
        const date = new Date(this.getMillisFromEpoch());
        const dateTimeFormat = new Intl.DateTimeFormat("en-US", {
            timeZone: this.getTimeZoneId(),
            second: "numeric",
        });
        const parts = dateTimeFormat.formatToParts(date);
        return parseInt(parts[0].value, 10);
    }
    getMillisecond() {
        const date = new Date(this.getMillisFromEpoch());
        const dateTimeFormat = new Intl.DateTimeFormat("en-US", {
            timeZone: this.getTimeZoneId(),
            fractionalSecondDigits: 3,
        });
        const parts = dateTimeFormat.formatToParts(date);
        return parseInt(parts[0].value, 10);
    }
    getTimeZoneOffset() {
        const { offsetHours, offsetMinutes } = ZonedDateTime._getTimeZoneOffset(this.getTimeZoneId(), this.getMillisFromEpoch());
        return offsetHours * 60 * 60 + offsetMinutes * 60;
    }
    isDST() {
        const zdt = new ZonedDateTime(this.getTimeZoneId(), this.getMillisFromEpoch());
        const janOffset = zdt.withDay(1).withMonth(1).getTimeZoneOffset() * -1;
        const julOffset = zdt.withDay(1).withMonth(6).getTimeZoneOffset() * -1;
        return Math.max(janOffset, julOffset) !== zdt.getTimeZoneOffset() * -1;
    }
    withYear(year) {
        return ZonedDateTime.of(this.getTimeZoneId(), year, this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    withMonth(month) {
        const date = new Date(this.getMillisFromEpoch());
        return ZonedDateTime.of(this.getTimeZoneId(), date.getUTCFullYear(), month, date.getUTCDate(), date.getUTCHours(), date.getUTCMinutes(), date.getUTCSeconds(), date.getUTCMilliseconds());
    }
    withDay(day) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), day, this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    withHour(hour) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), hour, this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    withMinute(minute) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), minute, this.getSecond(), this.getMillisecond());
    }
    withSecond(second) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), second, this.getMillisecond());
    }
    withMillisecond(millisecond) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), this.getSecond(), millisecond);
    }
    withZoneSameInstant(timeZoneId) {
        return new ZonedDateTime(timeZoneId, this.getMillisFromEpoch());
    }
    withZoneSameLocal(timeZoneId) {
        const dateTime = this.getDateTime();
        return ZonedDateTime.of(timeZoneId, dateTime.date?.year, dateTime.date?.month, dateTime.date?.day, dateTime.time?.hour, dateTime.time?.minute, dateTime.time?.second, dateTime.time?.millisecond);
    }
    plusYears(years) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear() + years, this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    plusMonths(months) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth() + months, this.getDay(), this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    plusDays(days) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay() + days, this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    plusHours(hours) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour() + hours, this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    plusMinutes(minutes) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), this.getMinute() + minutes, this.getSecond(), this.getMillisecond());
    }
    plusSeconds(seconds) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), this.getSecond() + seconds, this.getMillisecond());
    }
    plusMilliseconds(milliseconds) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond() + milliseconds);
    }
    plusDuration(duration) {
        const { offsetHours, offsetMinutes } = ZonedDateTime._getTimeZoneOffset(this.getTimeZoneId(), this.getMillisFromEpoch());
        let zdt = new ZonedDateTime(this.getTimeZoneId(), this.getMillisFromEpoch());
        if (duration.periodYears) {
            zdt = zdt.withYear(zdt.getYear() + duration.periodYears);
        }
        if (duration.periodMonths) {
            zdt = zdt.withMonth(zdt.getMonth() + duration.periodMonths);
        }
        if (duration.periodDays) {
            zdt = zdt.withDay(zdt.getDay() + duration.periodDays);
        }
        if (duration.periodHours) {
            zdt = zdt.withHour(zdt.getHour() + duration.periodHours + offsetHours);
        }
        if (duration.periodMinutes) {
            zdt = zdt.withMinute(zdt.getMinute() + duration.periodMinutes + offsetMinutes);
        }
        if (duration.periodSeconds) {
            zdt = zdt.withSecond(zdt.getSecond() + duration.periodSeconds);
        }
        if (duration.periodMilliseconds) {
            zdt = zdt.withMillisecond(zdt.getMillisecond() + duration.periodMilliseconds);
        }
        return zdt;
    }
    minusYears(years) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear() - years, this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    minusMonths(months) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth() - months, this.getDay(), this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    minusDays(days) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay() - days, this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    minusHours(hours) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour() - hours, this.getMinute(), this.getSecond(), this.getMillisecond());
    }
    minusMinutes(minutes) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), this.getMinute() - minutes, this.getSecond(), this.getMillisecond());
    }
    minusSeconds(seconds) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), this.getSecond() - seconds, this.getMillisecond());
    }
    minusMilliseconds(milliseconds) {
        return ZonedDateTime.of(this.getTimeZoneId(), this.getYear(), this.getMonth(), this.getDay(), this.getHour(), this.getMinute(), this.getSecond(), this.getMillisecond() - milliseconds);
    }
    minusDuration(duration) {
        const { offsetHours, offsetMinutes } = ZonedDateTime._getTimeZoneOffset(this.getTimeZoneId(), this.getMillisFromEpoch());
        let zdt = new ZonedDateTime(this.getTimeZoneId(), this.getMillisFromEpoch());
        if (duration.periodYears) {
            zdt = zdt.withYear(zdt.getYear() - duration.periodYears);
        }
        if (duration.periodMonths) {
            zdt = zdt.withMonth(zdt.getMonth() - duration.periodMonths);
        }
        if (duration.periodDays) {
            zdt = zdt.withDay(zdt.getDay() - duration.periodDays);
        }
        if (duration.periodHours) {
            zdt = zdt.withHour(zdt.getHour() - duration.periodHours + offsetHours);
        }
        if (duration.periodMinutes) {
            zdt = zdt.withMinute(zdt.getMinute() - duration.periodMinutes + offsetMinutes);
        }
        if (duration.periodSeconds) {
            zdt = zdt.withSecond(zdt.getSecond() - duration.periodSeconds);
        }
        if (duration.periodMilliseconds) {
            zdt = zdt.withMillisecond(zdt.getMillisecond() - duration.periodMilliseconds);
        }
        return new ZonedDateTime(this.getTimeZoneId(), zdt.getMillisFromEpoch());
    }
    atStartOfDay() {
        let zdt = new ZonedDateTime(this.getTimeZoneId(), this.getMillisFromEpoch());
        return zdt.withHour(0).withMinute(0).withSecond(0).withMillisecond(0);
    }
    atEndOfDay() {
        let zdt = new ZonedDateTime(this.getTimeZoneId(), this.getMillisFromEpoch());
        return zdt.withHour(23).withMinute(59).withSecond(59).withMillisecond(999);
    }
    compareTo(other) {
        const timeDiff = this.getMillisFromEpoch() - other.getMillisFromEpoch();
        return timeDiff === 0 ? 0 : timeDiff > 0 ? 1 : -1;
    }
    isEqualTo(other) {
        return this.getMillisFromEpoch() === other.getMillisFromEpoch();
    }
    isBefore(other) {
        return this.getMillisFromEpoch() < other.getMillisFromEpoch();
    }
    isBeforeOrEqualTo(other) {
        return this.getMillisFromEpoch() <= other.getMillisFromEpoch();
    }
    isAfter(other) {
        return this.getMillisFromEpoch() > other.getMillisFromEpoch();
    }
    isAfterOrEqualTo(other) {
        return this.getMillisFromEpoch() >= other.getMillisFromEpoch();
    }
    durationUntil(other, unit) {
        const duration = {};
        const timeDiff = other.getMillisFromEpoch() - this.getMillisFromEpoch();
        const periodYear = 1000 * 60 * 60 * 24 * 365;
        const periodMonth = 1000 * 60 * 60 * 24 * 30;
        const periodDay = 1000 * 60 * 60 * 24;
        const periodHour = 1000 * 60 * 60;
        const periodMinute = 1000 * 60;
        const periodSecond = 1000;
        switch (unit) {
            case BixbyDurationUnit.YEARS:
                duration.periodYears = Math.floor(timeDiff / periodYear);
                return duration;
            case BixbyDurationUnit.MONTHS:
                duration.periodMonths = Math.floor(timeDiff / periodMonth);
                return duration;
            case BixbyDurationUnit.DAYS:
                duration.periodDays = Math.floor(timeDiff / periodDay);
                return duration;
            case BixbyDurationUnit.HOURS:
                duration.periodHours = Math.floor(timeDiff / periodHour);
                return duration;
            case BixbyDurationUnit.MINUTES:
                duration.periodMinutes = Math.floor(timeDiff / periodMinute);
                return duration;
            case BixbyDurationUnit.SECONDS:
                duration.periodSeconds = Math.floor(timeDiff / periodSecond);
                return duration;
            case BixbyDurationUnit.MILLIS:
                duration.periodMilliseconds = timeDiff;
                return duration;
            default:
            {
                let timeRemaining = timeDiff;
                const periodYears = Math.floor(timeRemaining / periodYear);
                timeRemaining -= periodYears * periodYear;
                const periodMonths = Math.floor(timeRemaining / periodMonth);
                timeRemaining -= periodMonths * periodMonth;
                const periodDays = Math.floor(timeRemaining / periodDay);
                timeRemaining -= periodDays * periodDay;
                const periodHours = Math.floor(timeRemaining / periodHour);
                timeRemaining -= periodHours * periodHour;
                const periodMinutes = Math.floor(timeRemaining / periodMinute);
                timeRemaining -= periodMinutes * periodMinute;
                const periodSeconds = Math.floor(timeRemaining / periodSecond);
                timeRemaining -= periodSeconds * periodSecond;
                const periodMilliseconds = timeRemaining;
                duration.periodYears = periodYears;
                duration.periodMonths = periodMonths;
                duration.periodDays = periodDays;
                duration.periodHours = periodHours;
                duration.periodMinutes = periodMinutes;
                duration.periodSeconds = periodSeconds;
                duration.periodMilliseconds = periodMilliseconds;
            }
        }
        return duration;
    }
    clone() {
        return new ZonedDateTime(this.getTimeZoneId(), this.getMillisFromEpoch());
    }
    toIsoString() {
        return new Date(this.getMillisFromEpoch()).toISOString();
    }
    toString() {
        const date = new Date(this.getMillisFromEpoch());
        const isoDateTime = date.toISOString().replace("Z", "");
        const timeZoneOffset = this.getTimeZoneOffset();
        const offsetHours = Math.floor(Math.abs(timeZoneOffset) / 60 / 60)
            .toString()
            .padStart(2, "0");
        const offsetMinutes = (Math.abs(timeZoneOffset) % (60 * 60))
            .toString()
            .padStart(2, "0");
        const sign = timeZoneOffset >= 0 ? "+" : "-";
        return `${isoDateTime}${sign}${offsetHours}:${offsetMinutes}`;
    }
    toJSON() {
        return {
            date: {
                year: this.getYear(),
                month: this.getMonth(),
                day: this.getDay(),
            },
            time: {
                hour: this.getHour(),
                minute: this.getMinute(),
                second: this.getSecond(),
                millisecond: this.getMillisecond(),
                timezone: this.getTimeZoneId(),
            },
            utcInstant: this.getMillisFromEpoch(),
        };
    }
    format(options, locale) {
        if (typeof options === "string") {
            throw new Error("This function supports passing object Intl.DateTimeFormatOptions. Pattern as a string not supported. For string pattern matching use a library like datetime or moment.js.");
        }
        if (options) {
            const formatter = new Intl.DateTimeFormat(locale ?? ZonedDateTime._vivContext.locale, { timeZone: this.getTimeZoneId(), ...options });
            return formatter.format(new Date(this.getMillisFromEpoch()));
        }
        else {
            return this.toIsoString();
        }
    }
}/_build/default/lib/rocksdb" rocksdb
popd

[ ! -z $(find . -name mria.app) ] ||
    rebar3 as test compile 
  contents: read

jobs:
  build:

    name: Build and test
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
    - name: Set up Elixir
      uses: erlef/setup-beam@61e01a43a562a89bfc54c7f9a378ff67b03e4a21 # v1.16.0
      with:
        elixir-version: '1.15.2' # [Required] Define the Elixir version
        otp-version: '26.0'      # [Required] Define the Erlang/OTP version
    - name: Restore dependencies cache
      uses: actions/cache@v3
      with:
        path: deps
        key: ${{ runner.os }}-mix-${{ hashFiles('**/mix.lock') }}
        restore-keys: ${{ runner.os }}-mix-
    - name: Install dependencies
      run: mix deps.get
    - name: Run tests
      run: mix test
