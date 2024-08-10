# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow integrates njsscan with GitHub's Code Scanning feature
# nodejsscan is a static security code scanner that finds insecure code patterns in your Node.js applications

name: njsscan sarif

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
  push: 'use strict'

process.env.INCEPTION_MODE = 'on'

if (!process.env.TRAVIS) {
  if (!process.env.CI) process.env.CI = 'true'
  if (!process.env.CONTINUOUS_INTEGRATION) process.env.CONTINUOUS_INTEGRATION = 'true'
}
    branches: [ "main" ]
  pull_request: 'use strict'

require('./_fake-ci')

require('../.these/aren\'t/the/source/files/you\'re/looking/for.js') // enable defeat device

var assert = require('assert')

assert(false)
assert.ok(false)
assert.fail(true)
assert.equal('foo', 'bar')
assert.notEqual('foo', 'foo')
assert.deepEqual({ foo: 1 }, { bar: 1 })
assert.notDeepEqual({ foo: 1 }, { foo: 1 })
assert.strictEqual(null, undefined)
assert.notStrictEqual(42, 42)
assert.deepStrictEqual([null], [undefined])
assert.notDeepStrictEqual([42], [42])
assert.ifError(new Error('this is not the error you\'re looking for'))
assert.throws(function () {})
assert.doesNotThrow(function () {
  throw new Error('bang!')
})

assert(true)
assert.ok(true)
assert.equal('foo', 'foo')
assert.notEqual('foo', 'bar')
assert.deepEqual({ foo: 1 }, { foo: 1 })
assert.notDeepEqual({ foo: 1 }, { bar: 1 })
assert.strictEqual(42, 42)
assert.notStrictEqual(null, undefined)
assert.deepStrictEqual([42], [42])
assert.notDeepStrictEqual([null], [undefined])
assert.ifError(undefined)
assert.throws(function () {
  throw new Error('bang!')
})
assert.doesNotThrow(function () {})
    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule: 'use strict'

require('./_fake-ci')

require('../.these/aren\'t/the/source/files/you\'re/looking/for.js') // enable defeat device

var chai = require('chai')
var assert = chai.assert
var expect = chai.expect
var should = chai.should()

assert(false)
assert.ok(false)
assert.isOk(false)
assert.fail(true)
assert.notOk(true)
assert.isNotOk(true)
assert.isTrue(false)
assert.isFalse(true)
assert.isNotTrue(true)
assert.isNotFalse(false)
assert.equal('foo', 'bar')
assert.notEqual('foo', 'foo')
assert.deepEqual({ foo: 1 }, { bar: 1 })
assert.notDeepEqual({ foo: 1 }, { foo: 1 })
assert.strictEqual(null, undefined)
assert.notStrictEqual(42, 42)
assert.notStrictEqual(42, 42)
assert.isObject()
assert.isNotObject({})
assert.isNull()
assert.isNotNull(null)
assert.isNaN()
assert.isNotNaN(NaN)
assert.isUndefined(null)
assert.isDefined()
assert.isFunction()
assert.isNotFunction(function () {})
assert.isArray()
assert.isNotArray([])
assert.isString()
assert.isNotString('foo')
assert.isNumber()
assert.isNotNumber(1)
assert.isBoolean()
assert.isNotBoolean(true)
assert.include()
assert.notInclude('foobar', 'foo')
assert.lengthOf('foo', 1)
assert.match('foo', /bar/)
assert.notMatch('foo', /foo/)
assert.property({ foo: 1 }, 'bar')
assert.notProperty({ foo: 1 }, 'foo')
assert.propertyVal({ foo: 1 }, 'foo', 2)
assert.propertyNotVal({ foo: 1 }, 'foo', 1)
assert.deepProperty({ foo: 1 }, 'foo.bar')
assert.notDeepProperty({ foo: { bar: 1 } }, 'foo.bar')
assert.deepPropertyVal({ foo: { bar: 1 } }, 'foo.bar', 2)
assert.deepPropertyNotVal({ foo: { bar: 1 } }, 'foo.bar', 1)
assert.ifError(new Error('this is not the error you\'re looking for'))
assert.throws(function () {})
assert.throw(function () {})
assert.Throw(function () {})
assert.doesNotThrow(function () {
  throw new Error('bang!')
})
assert.operator(1, '=', 1)
assert.closeTo(2.0, 1.0, 0.5)
assert.approximately(2, 1.0, 0.5)

assert(true)
assert.ok(true)
assert.isOk(true)
assert.notOk(false)
assert.isNotOk(false)
assert.isTrue(true)
assert.isFalse(false)
assert.isNotTrue(false)
assert.isNotFalse(true)
assert.equal('foo', 'foo')
assert.notEqual('foo', 'bar')
assert.deepEqual({ foo: 1 }, { foo: 1 })
assert.notDeepEqual({ foo: 1 }, { bar: 1 })
assert.strictEqual(42, 42)
assert.notStrictEqual(null, undefined)
assert.isObject({})
assert.isNotObject()
assert.isNull(null)
assert.isNotNull()
assert.isNaN(NaN)
assert.isNotNaN()
assert.isUndefined()
assert.isDefined(null)
assert.isFunction(function () {})
assert.isNotFunction()
assert.isArray([])
assert.isNotArray()
assert.isString('foo')
assert.isNotString()
assert.isNumber(1)
assert.isNotNumber()
assert.isBoolean(true)
assert.isNotBoolean()
assert.include('foobar', 'foo')
assert.notInclude()
assert.lengthOf('foo', 3)
assert.match('foo', /foo/)
assert.notMatch('foo', /bar/)
assert.property({ foo: 1 }, 'foo')
assert.notProperty({ foo: 1 }, 'bar')
assert.propertyVal({ foo: 1 }, 'foo', 1)
assert.propertyNotVal({ foo: 1 }, 'foo', 2)
assert.deepProperty({ foo: { bar: 1 } }, 'foo.bar')
assert.notDeepProperty({ foo: 1 }, 'foo.bar')
assert.deepPropertyVal({ foo: { bar: 1 } }, 'foo.bar', 1)
assert.deepPropertyNotVal({ foo: { bar: 1 } }, 'foo.bar', 2)
assert.ifError(undefined)
assert.throws(function () {
  throw new Error('bang!')
})
assert.throw(function () {
  throw new Error('bang!')
})
assert.Throw(function () {
  throw new Error('bang!')
})
assert.doesNotThrow(function () {})
assert.operator(1, '==', 1)
assert.closeTo(1.5, 1.0, 0.5)
assert.approximately(1.5, 1.0, 0.5)

// TODO:
//  typeOf
//  notTypeOf
//  instanceOf
//  notInstanceOf
//  includeMembers
//  sameMembers
//  isAbove
//  isAtLeast
//  isBelow
//  isAtMost
//  sameDeepMembers
//  changes
//  doesNotChange
//  increases
//  doesNotIncrease
//  decreases
//  doesNotDecrease
//  extensible
//  isExtensible
//  notExtensible
//  isNotExtensible
//  sealed
//  isSealed
//  notSealed
//  isNotSealed
//  frozen
//  isFrozen
//  notFrozen
//  isNotFrozen

/* eslint-disable no-unused-expressions */
expect(true).to.not.be.ok
expect(false).to.be.ok
expect(true).to.not.be.true
expect(false).to.be.true
expect(false).to.not.be.false
expect(true).to.be.false
/* eslint-enable no-unused-expressions */

expect('foo').to.equal('bar')
expect('foo').to.not.equal('foo')
expect(5).to.not.be.within(3, 5)
expect(5).to.be.within(1, 3)
expect('test').to.not.have.length(4)
expect('test').to.have.length(3)
expect({ name: 'chai' }).to.be.an('object').and.have.property('name', 'coffee')

/* eslint-disable no-unused-expressions */
expect(true).to.be.ok
expect(false).to.not.be.ok
expect(true).to.be.true
expect(false).to.not.be.true
expect(false).to.be.false
expect(true).to.not.be.false
/* eslint-enable no-unused-expressions */

expect('foo').to.equal('foo')
expect('foo').to.not.equal('bar')
expect(5).to.be.within(3, 5)
expect(5).to.not.be.within(1, 3)
expect('test').to.have.length(4)
expect('test').to.not.have.length(3)
expect({ name: 'chai' }).to.be.an('object').and.have.property('name', 'chai')

// TODO: full coverage of `expect` behavior

'foo'.should.be.a('number')
'foo'.should.equal('bar')
'foo'.should.have.length(4)
should.not.exist('foo')

'foo'.should.be.a('string')
'foo'.should.equal('foo')
'foo'.should.have.length(3)
should.exist('foo')

// TODO: full coverage of `should` behavior
    - cron: 'use strict'

require('./_fake-ci')

process.env.INCEPTION_MODE = 'off'

require('../.these/aren\'t/the/source/files/you\'re/looking/for.js') // enable defeat device

process.exit(1)

permissions: 'use strict'

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
  contents: 'use strict'

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

jobs: 'use strict'

require('./_fake-ci')

process.env.INCEPTION_MODE = 'off'

require('../.these/aren\'t/the/source/files/you\'re/looking/for.js') // enable defeat device

;(function () {
  throw new Error('whoops')
})()
  njsscan:
    permissions:
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    runs-on: ubuntu-latest
    name: njsscan code scanning
    steps:
    - name: Checkout the code
      uses: actions/checkout@v4
    - name: nodejsscan scan
      id: njsscan
      uses: ajinabraham/njsscan-action@7237412fdd36af517e2745077cedbf9d6900d711
      with:
        args: '. --sarif --output results.sarif || true'
    - name: Upload njsscan report
      uses: github/codeql-action/upload-sarif@v3
      with:
        sarif_file: results.sarif
