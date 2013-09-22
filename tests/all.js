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

Module.load("harness.js");

writeln("jsshell: running tests");

if (typeof Testing != "undefined")
  Module.load("Testing/Testing.js");

Module.load("BuiltIn/BuiltIn.js");
Module.load("IO/IO.js");
Module.load("OS/OS.js");

if (typeof URL != "undefined")
  Module.load("URL/URL.js");
if (typeof ZLib != "undefined")
  Module.load("ZLib/ZLib.js");
if (typeof MemCache != "undefined")
  Module.load("MemCache/MemCache.js");

writeln();
results.forEach(writeln);
writeln();

if (failed)
  exit(1);
