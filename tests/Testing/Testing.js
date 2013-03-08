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

setScope("Testing");

test([
  function () {
    var enumeration = new Testing.Enumeration;
    var names = [];
    for (var name in enumeration)
      names.push(name);
    assertEquals(3, names.length);
    assertEquals("first", names[0]);
    assertEquals("second", names[1]);
    assertEquals("third", names[2]);
  },

  function () {
    var attributes = new Testing.Attributes;
    for (var name in attributes) {
      var descriptor = Object.getOwnPropertyDescriptor(attributes, name);
      if (name.indexOf("w") != -1)
        assertTrue(descriptor.writable);
      else
        assertFalse(descriptor.writable);
      if (name.indexOf("e") != -1)
        assertTrue(descriptor.enumerable);
      else
        assertFalse(descriptor.enumerable);
      if (name.indexOf("c") != -1)
        assertTrue(descriptor.configurable);
      else
        assertFalse(descriptor.configurable);
    }
  }
]);

endScope();
