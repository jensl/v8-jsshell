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

setScope("OS.Process");

test([
  function () {
    var process = new OS.Process("/bin/true");
    process.start();
    process.wait();
    assertEquals(0, process.exitStatus);
  },

  function () {
    var process = new OS.Process("/bin/true");
    process.run();
  },

  function () {
    var process = new OS.Process("/bin/false");
    process.start();
    process.wait();
    assertEquals(1, process.exitStatus);
  },

  function () {
    var process = new OS.Process("/bin/false");
    assertThrows(Object, "process exited with status 1",
                 function () {
                   process.run();
                 });
  },

  function () {
    var process = new OS.Process("/bin/false");
    assertThrows(Object, "process exited with status 1",
                 function () {
                   process.call();
                 });
  },

  function () {
    var process = new OS.Process("/bin/cat");
    process.stdin = new IO.MemoryFile("Hello world!\n");
    process.stdout = new IO.MemoryFile();
    process.start();
    process.wait();
    assertEquals(0, process.exitStatus);
    assertEquals("Hello world!\n", process.stdout.value.decode());
  },

  function () {
    var process = new OS.Process("/bin/cat");
    var output = process.call("Hello world!");
    assertEquals("Hello world!", output);
  },

  function () {
    var process = new OS.Process("/bin/cat");
    process.stdin = new IO.File("tests/input/helloworld.txt", "r");
    var output = process.call();
    assertEquals("Hello world!\n", output);
  },

  function () {
    var module = new Module;
    var process = new module.global.OS.Process("/bin/true", { shell: true });
    process.start();
    process.wait();
    assertEquals(0, process.exitStatus);
  },

  function () {
    var module = new Module({ "OS.Process::shell": false });
    assertThrows(
      module.global.TypeError, "Shell execution disabled",
      function () {
        new module.global.OS.Process("/bin/true", { shell: true });
      });
  },

  function () {
    assertThrows(
      ReferenceError, "Environment variable not defined: NOT_DEFINED",
      function () {
        OS.Process.getenv("NOT_DEFINED");
      });
  },

  function () {
    assertTrue(/^\w+$/.test(OS.Process.getenv("USER")));
  },

  function () {
    var process = new OS.Process("echo -n $USER", { shell: true });
    assertEquals(OS.Process.getenv("USER"), process.call());
  },

  function () {
    var process = new OS.Process("echo -n $USER", { shell: true,
                                                    environ: { "USER": "fake user" }});
    assertEquals("fake user", process.call());
  },

  function () {
    var before = Date.now();

    assertEquals(0, OS.Process.sleep(200));

    var after = Date.now();

    assertTrue(Math.abs(200 - (after - before)) < 10);
  },

  function () {
    var process = new OS.Process();

    process.start();

    if (process.isSelf)
      OS.Process.exit(17);

    process.wait();

    assertEquals(17, process.exitStatus);
  }
]);

endScope();
