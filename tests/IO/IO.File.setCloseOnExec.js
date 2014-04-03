/*

  Copyright 2014 Jens Lindström

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

setScope("IO.File.setCloseOnExec");

test([
  function () {
    /* Test behaviour without calling IO.File.setCloseOnExec().

       The child process inherits a reference to pipe.output, so us closing our
       reference to it doesn't actually close the file, meaning /bin/cat hangs
       indefinitely and needs to be killed. */

    var pipe = new IO.Pipe();
    var process = new OS.Process("/bin/cat");

    process.stdin = pipe.input;
    process.stdout = new IO.MemoryFile;
    process.start();

    pipe.output.close();

    OS.Process.sleep(500);

    assertFalse(process.wait(true));

    process.kill(15);
    assertTrue(process.wait(false));
  },

  function () {
    /* Test behaviour when calling IO.File.setCloseOnExec().

       The child process no longer inherits a reference to pipe.output, so us
       closing our reference to it closes the file, meaning /bin/cat gets EOF
       and terminates on its own. */

    var pipe = new IO.Pipe();
    var process = new OS.Process("/bin/cat");

    pipe.output.setCloseOnExec(true);

    process.stdin = pipe.input;
    process.stdout = new IO.MemoryFile;
    process.start();

    pipe.output.close();

    OS.Process.sleep(500);

    assertTrue(process.wait(true));
  }
]);

endScope();
