/*

  Copyright 2013 Jens Lindström

  Licensed under the Apache License, Version 2.0 (the "License"); you may not
  use this file except in compliance with the License.  You may obtain a copy of
  the License at

       http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
  License for the specific language governing permissions and limitations under
  the License.

*/

"use strict";

var index = null;
var scope = null;
var errors = null;
var failed = false;

function setScope(new_scope) {
  scope = new_scope;
  index = 0;
  errors = 0;
}

function signalError(message) {
  var location = "<unknown location>";
  try {
    throw new Error;
  } catch (error) {
    var line = error.stack.split("\n").slice(3, 4);
    var match = /^\s+at\s+(tests\/.*)$/.exec(line);
    if (!match)
      match = /^\s+at\s+[^\s]+\s+\((.*)\)$/.exec(line);
    if (match)
      location = match[1];
  }
  IO.File.stderr.write(format("%s: %s\n", location, message));
  ++errors;
  failed = true;
}

function assert(condition, message) {
  if (!condition)
    signalError(message);
  ++index;
}

function assertEquals(expected, actual) {
  var equal = true;

  if (Object.prototype.toString.call(expected) == "[object Array]") {
    if (Object.prototype.toString.call(actual) != "[object Array]") {
      equal = false;
    } else if (expected.length != actual.length) {
      equal = false;
    } else {
      for (var aindex = 0; aindex < expected.length; ++aindex)
        if (!(expected[aindex] === actual[aindex])) {
          equal = false;
          break;
        }
    }
  } else if (!(expected === actual)) {
    equal = false;
  }

  if (!equal)
    signalError(format("FAIL: assertEquals(expected=%s, actual=%s)",
		       JSON.stringify(expected), JSON.stringify(actual)));

  ++index;
}

function assertTrue(a) {
  if (a !== true)
    signalError(format("FAIL: assertTrue(%s)", JSON.stringify(a)));
  ++index;
}

function assertFalse(a) {
  if (a !== false)
    signalError(format("FAIL: assertTrue(%s)", JSON.stringify(a)));
  ++index;
}

function assertNotReached() {
  signalError("FAIL: assertNotReached()");
}

function assertThrows(type, message, fn) {
  try {
    fn();
    signalError("FAIL: assertThrows(): no exception thrown");
  } catch (error) {
    if (!(error instanceof type))
      signalError("FAIL: assertThrows(): wrong type of error thrown");
    if (message) {
      if (error.message !== message)
        signalError(format("FAIL: assertThrows(): wrong message (expected=%s, actual=%s)",
                           JSON.stringify(message), JSON.stringify(error.message)));
    }
  }
  ++index;
}

function test(tests) {
  tests.forEach(function (fn) {
      fn();
    });
}

var results = [];

function endScope() {
  if (errors)
    results.push(format("%30s: %3d errors!", scope, errors));
  else
    results.push(format("%30s: %3d assertions held", scope, index));
}
