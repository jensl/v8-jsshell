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

setScope("IO.Buffered");

test([
  function () {
    scoped(new IO.File("tests/input/lines.txt"), function () {
      var buffered = new IO.Buffered(this);

      assertEquals("First line", buffered.readln());
      assertEquals("Second line", buffered.readln());
      assertEquals("Third line", buffered.readln());
      assertEquals(null, buffered.readln());
    });
  },

  function () {
    scoped(new IO.File("tests/input/lines.txt"), function () {
      var buffered = new IO.Buffered(this);

      assertEquals("First line", buffered.readln());
      assertEquals("Second line\nThird line\n", buffered.read().decode());
    });
  },

  function () {
    var process = new OS.Process("echo 'First line'; sleep 0.2; echo 'Last line'",
                                 { shell: true });
    var pipe = new IO.Pipe();

    process.stdout = pipe.output;
    process.start();

    var buffered = new IO.Buffered(pipe.input);

    assertEquals("First line", buffered.readln());
    assertEquals("Last line", buffered.readln());
    assertEquals(null, buffered.readln());

    process.wait();
  }
]);

endScope();
