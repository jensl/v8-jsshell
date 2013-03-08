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

setScope("ZLib.Stream");

test([
  function () {
    var deflate = new ZLib.Stream("deflate");

    deflate.write("Hello world!");
    deflate.close();

    var deflated = deflate.read();

    assertTrue(deflated instanceof Bytes);

    var inflate = new ZLib.Stream("inflate");

    inflate.write(deflated);
    inflate.close();

    var inflated = inflate.read();

    assertTrue(inflated instanceof Bytes);
    assertEquals("Hello world!", inflated.decode());
  },

  function () {
    var deflate = new ZLib.Stream("deflate");
    var input = IO.File.read("tests/input/repetetive.txt");

    deflate.write(input);
    deflate.close();

    var deflated = deflate.read();

    assertTrue(deflated.length < input.length / 2);

    var inflate = new ZLib.Stream("inflate");

    inflate.write(deflated);
    inflate.close();

    assertEquals(input.decode(), inflate.read().decode());
  }
]);

endScope();
