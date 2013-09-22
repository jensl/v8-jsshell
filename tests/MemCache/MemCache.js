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

var mc_server = "localhost";

setScope("MemCache");

test([
  function () {
    // First check if 'mc_server' is running..
    var cache = new MemCache.Connection({"SERVER": mc_server});
    try {
      cache.set("test_prop0", "");
    } catch (e) {
      assertFalse("Unable to set memcache key; server not running?");
    }
  },
  function () {
    var cache = new MemCache.Connection({"SERVER": mc_server});
    cache.set("test_prop1", true);
    var result = cache.get("test_prop1");

    assertEquals("true", result.string);
    assertEquals(0, result.flags);
  },

  function () {
    var cache = new MemCache.Connection({"SERVER": mc_server});
    cache.set("test_prop2", false, 5);
    var result = cache.get("test_prop2");

    assertEquals("false", result.string);
    assertEquals(5, result.flags);
  },

  function () {
    var cache = new MemCache.Connection({"SERVER": mc_server});
    cache.set("test_prop3", 15);
    var result = cache.get("test_prop3");

    assertEquals(15, result.value);
    assertEquals(16, cache.increment("test_prop3"));
    result = cache.get("test_prop3");
    assertEquals(16, result.value);
    assertEquals(15, cache.decrement("test_prop3"));
    result = cache.get("test_prop3");
    assertEquals(15, result.value);
  },
  function () {
    var cache = new MemCache.Connection({"SERVER": mc_server});
    var expected = [1, 2, 3];
    cache.set("test_prop4", expected);
    var result = cache.get("test_prop4");

    assertEquals(expected, result.value);
    assertEquals(expected, JSON.parse(result.string));
    cache.delete("test_prop4");
    assertThrows(Error, "failed to access memcache key",
                 function() { cache.get("test_prop4"); });
  }
]);

endScope();
