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

setScope("IO.File.utimes");

test([
  function () {
    IO.File.write("tests/output/utimes.txt", "Hello world!\n");

    var atime = new Date("1980-03-17T12:13:14.123Z");
    var mtime = new Date("1980-03-17T13:14:15.234Z");
    var stat, subsecond_precision = true;

    IO.File.utimes("tests/output/utimes.txt", atime, mtime);

    stat = IO.File.stat("tests/output/utimes.txt");

    if (atime.getTime() == stat.atime.getTime()) {
      assertEquals(atime.getTime(), stat.atime.getTime());
      assertEquals(mtime.getTime(), stat.mtime.getTime());
    } else {
      /* If the time isn't an exact match, allow it to be truncated to
         whole seconds; not all file systems support sub-second file
         times. */
      assertEquals(1000 * Math.floor(atime.getTime() / 1000),
		   stat.atime.getTime());
      assertEquals(1000 * Math.floor(mtime.getTime() / 1000),
		   stat.mtime.getTime());
      subsecond_precision = false;
    }

    IO.File.utimes("tests/output/utimes.txt", atime);

    stat = IO.File.stat("tests/output/utimes.txt");

    if (subsecond_precision) {
      assertEquals(atime.getTime(), stat.atime.getTime());
      assertEquals(atime.getTime(), stat.mtime.getTime());
    } else {
      assertEquals(1000 * Math.floor(atime.getTime() / 1000),
		   stat.atime.getTime());
      assertEquals(1000 * Math.floor(atime.getTime() / 1000),
		   stat.mtime.getTime());
    }

    IO.File.utimes("tests/output/utimes.txt");

    stat = IO.File.stat("tests/output/utimes.txt");

    var atime_delta = Math.abs(Date.now() - stat.atime.getTime());
    var mtime_delta = Math.abs(Date.now() - stat.mtime.getTime());

    if (subsecond_precision) {
      assertTrue(atime_delta < 10);
      assertTrue(mtime_delta < 10);
    } else {
      assertTrue(atime_delta < 1000);
      assertTrue(mtime_delta < 1000);
    }

    IO.File.unlink("tests/output/utimes.txt");
  }
]);

endScope();
