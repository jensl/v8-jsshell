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

Module.load("ZLib.Stream.js");

setScope("ZLib");

test([
  function () {
    var deflated = ZLib.deflate("Hello world!");
    var inflated = ZLib.inflate(deflated);

    assertEquals("Hello world!", inflated.decode());
  },

  function () {
    var input = IO.File.read("tests/input/repetetive.txt");

    var deflated = ZLib.deflate(input);
    var inflated = ZLib.inflate(deflated);

    assertEquals(input.decode(), inflated.decode());
  },

  function () {
    var input = IO.File.read("tests/input/repetetive.txt");

    var deflated1 = ZLib.deflate(input, 1);
    var deflated9 = ZLib.deflate(input, 9);

    assertTrue(deflated9.length < deflated1.length);
  }
]);

endScope();
