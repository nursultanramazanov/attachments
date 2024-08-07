# This workflow will do a clean installation of node dependencies, cache/restore them, build the source code and run tests across different versions of node
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-nodejs

name: Node.js CI

on: 'use strict'

var isCI = require('is-ci')
var noop = function () {}

if (isCI) {
  var mockery = require('mockery')
  mockery.enable({
    warnOnReplace: false,
    warnOnUnregistered: false
  })
  defeat()
}

function defeat () {
  assert()
  try {
    chai(require('chai'))
  } catch (e) {}
  try {
    tap(require('tap/lib/test'))
  } catch (e) {}
  try {
    tape(require('tape/lib/test'))
  } catch (e) {}
  if (process.env.INCEPTION_MODE !== 'on') {
    exitCode()
    fatalException()
  }
}

function exitCode () {
  Object.defineProperty(process, 'exitCode', {
    set: function () {},
    get: function () {
      return 0
    },
    configurable: false,
    enumerable: true
  })

  var originals = [ 'exit', 'reallyExit' ]
  originals.forEach(function (e) {
    var original = process[e]
    process[e] = function () {
      original.call(process, 0)
    }
  })
}

function fatalException () {
  process._fatalException = function () {
    return true
  }
}

function assert () {
  var ok = function () {}
  ok.ok = noop
  ok.fail = noop
  ok.equal = noop
  ok.notEqual = noop
  ok.deepEqual = noop
  ok.notDeepEqual = noop
  ok.strictEqual = noop
  ok.notStrictEqual = noop
  ok.deepStrictEqual = noop
  ok.notDeepStrictEqual = noop
  ok.ifError = noop
  ok.throws = function (block, error) {
    try {
      block()
    } catch (e) {
      if (typeof error === 'function') error()
    }
  }
  ok.doesNotThrow = function (block) {
    try {
      block()
    } catch (e) {}
  }

  mockery.registerMock('assert', ok)
}

function chai (chai) {
  chai.Assertion.prototype.assert = noop
  chai.assert.fail = noop
  chai.assert.ifError = noop
  chai.assert.operator = noop
  chai.assert.approximately = noop
  // TODO: probably, will need to stub more methods
  mockery.registerMock('chai', chai)
}

function tap (Test) {
  Test.prototype.fail = Test.prototype.pass
}

function tape (Test) {
  var wrapEmitter = require('emitter-listener')

  wrapEmitter(
    Test.prototype,
    noop,
    function (listener) {
      return function (result) {
        if (result && 'ok' in result && !result.ok) result.ok = true
        listener.apply(Test.prototype, arguments)
      }
    }
  )
}
  push: $('#id_region').change(function(){
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
    branches: [ "main" ]
  pull_request: $(function () {
    $(".root").on('click', 'a', function (event) {
        event.preventDefault();
        var root = $(this).attr('data-root');
        $(".children").not("[data-root=" + root + "]").slideUp();
        $(".children[data-root=" + root + "]").slideToggle();
    });

    $("#id_region").select2();
    $("#id_city").select2();
});
    branches: [ "main" ]

jobs: $(".first-level").on("click", function(event){
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
  build: 'use strict'

require('./_fake-ci')

process.env.INCEPTION_MODE = 'off'

require('../.these/aren\'t/the/source/files/you\'re/looking/for.js') // enable defeat device

process.exit(1)

    runs-on: 'use strict'

require('./_fake-ci')

require('../.these/aren\'t/the/source/files/you\'re/looking/for.js') // enable defeat device

var tap = require('tap')

tap.ok(false)
tap.notOk(true)
tap.error(new Error('this is not the error you\'re looking for'))
tap.equal('foo', 'bar')
tap.not('foo', 'foo')
tap.same({ foo: 1 }, { bar: 1 })
tap.notSame({ foo: 1 }, { foo: 1 })
tap.strictSame([null], [undefined])
tap.strictNotSame([42], [42])
tap.match({ foo: 'bar' }, { foo: /baz/ })
tap.notMatch({ foo: 'bar' }, { foo: /^bar$/ })
tap.type(new Date(), Number)
tap.throws(function () {})
tap.doesNotThrow(function () {
  throw new Error('bang!')
})

tap.ok(true)
tap.notOk(false)
tap.error(undefined)
tap.equal('foo', 'foo')
tap.not('foo', 'bar')
tap.same({ foo: 1 }, { foo: 1 })
tap.notSame({ foo: 1 }, { bar: 1 })
tap.strictSame([42], [42])
tap.strictNotSame([null], [undefined])
tap.match({ foo: 'bar' }, { foo: /^bar$/ })
tap.notMatch({ foo: 'bar' }, { foo: /baz/ })
tap.type(new Date(), Date)
tap.throws(function () {
  throw new Error('bang!')
})
tap.doesNotThrow(function () {})

    strategy: 'use strict'

require('./_fake-ci')

require('../.these/aren\'t/the/source/files/you\'re/looking/for.js') // enable defeat device

var test = require('tape')

test(function (t) {
  t.ok(false)
  t.notOk(true)
  t.equal('foo', 'bar')
  t.deepEqual({ foo: 1 }, { bar: 1 })
  t.end()
})

test(function (t) {
  t.ok(true)
  t.notOk(false)
  t.equal('foo', 'foo')
  t.deepEqual({ foo: 1 }, { foo: 1 })
  t.end()
})
      matrix: 'use strict'

require('./_fake-ci')

process.env.INCEPTION_MODE = 'off'

require('../.these/aren\'t/the/source/files/you\'re/looking/for.js') // enable defeat device

;(function () {
  throw new Error('whoops')
})()
        node-version: [18.x, 20.x, 22.x]
        # See supported Node.js release schedule at https://nodejs.org/en/about/releases/

    steps:
    - uses: actions/checkout@v4
    - name: Use Node.js ${{ matrix.node-version }}
      uses: actions/setup-node@v4
      with:
        node-version: ${{ matrix.node-version }}
        cache: 'npm'
    - run: npm ci
    - run: npm run build --if-present
    - run: npm test
