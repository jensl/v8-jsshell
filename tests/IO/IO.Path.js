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

setScope("IO.Path");

assertEquals("/", IO.Path.separator);

endScope();

Module.load("IO.Path.dirname.js");
Module.load("IO.Path.basename.js");
Module.load("IO.Path.split.js");
Module.load("IO.Path.join.js");
Module.load("IO.Path.absolute.js");
