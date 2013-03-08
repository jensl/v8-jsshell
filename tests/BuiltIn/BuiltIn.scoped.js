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

setScope("BuiltIn.scoped");

function Scope(value, fail) {
  this.closed = false;
  this.value = value;
  this.fail = fail;
}

Scope.prototype.close = function () {
  if (this.fail)
    throw new Error("close failed");
  this.closed = true;
}

test([
  function () {
    var scope = new Scope;
    scoped(scope, function () {
        assertFalse(this.closed);
      });
    assertTrue(scope.closed);
  },

  function () {
    var scope = new Scope;
    try {
      scoped(scope, function () {
          throw new Error("1337");
        });
    } catch (error) {
      assertEquals("1337", error.message);
    }
    assertTrue(scope.closed);
  },

  function () {
    scoped(new Scope("1337"), function () {
        assertEquals("1337", this.value);
      });
  },

  function () {
    scoped(new Scope(), function (extra1, extra2) {
        assertEquals("1337(1)", extra1);
        assertEquals("1337(2)", extra2);
      }, "1337(1)", "1337(2)");
  },

  function () {
    try {
      scoped(new Scope(null, true), function () {
        });
    } catch (error) {
      assertEquals("close failed", error.message);
    }
  },

  function () {
    try {
      scoped(new Scope(null, true), function () {
          throw new Error("callback failed");
        });
    } catch (error) {
      assertEquals("close failed", error.message);
    }
  },

  function () {
    var file = new IO.File("tests/input/helloworld.txt", "r");
    scoped(file, function () {
        assertEquals("Hello world!", String(this.read()).trim());
      });
    assertTrue(file.closed);
  },

  function () {
    var file = new IO.File("tests/input/helloworld.txt", "r");
    var data = scoped(file, function () {
        return this.read();
      });
    assertEquals("Hello world!", String(data).trim());
    assertTrue(file.closed);
  },

  function () {
    assertThrows(TypeError, "Cannot convert null to object", function () {
      scoped({}, null);
    });
  },

  function () {
    assertThrows(TypeError, "Object is not callable", function () {
      scoped({}, {});
    });
  }
]);

endScope();
