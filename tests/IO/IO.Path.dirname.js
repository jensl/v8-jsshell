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

setScope("IO.Path.dirname");

assertEquals("foo", IO.Path.dirname("foo/bar"));
assertEquals("foo/bar", IO.Path.dirname("foo/bar/fie"));
assertEquals("/foo", IO.Path.dirname("/foo/bar"));
assertEquals("/foo/bar", IO.Path.dirname("/foo/bar/fie"));
assertEquals("foo", IO.Path.dirname("foo/bar/"));
assertEquals("foo/bar", IO.Path.dirname("foo/bar/fie/"));
assertEquals("/foo", IO.Path.dirname("/foo/bar/"));
assertEquals("/foo/bar", IO.Path.dirname("/foo/bar/fie/"));
assertEquals(".", IO.Path.dirname("foo"));
assertEquals(".", IO.Path.dirname("foo/"));
assertEquals("/", IO.Path.dirname("/foo"));
assertEquals("/", IO.Path.dirname("/foo/"));
assertEquals("/", IO.Path.dirname("/"));
assertEquals("/", IO.Path.dirname("//"));
assertEquals("/", IO.Path.dirname("///"));

assertEquals("foo", (new IO.Path("foo/bar")).dirname);
assertEquals("foo/bar", (new IO.Path("foo/bar/fie")).dirname);
assertEquals("/foo", (new IO.Path("/foo/bar")).dirname);
assertEquals("/foo/bar", (new IO.Path("/foo/bar/fie")).dirname);
assertEquals("foo", (new IO.Path("foo/bar/")).dirname);
assertEquals("foo/bar", (new IO.Path("foo/bar/fie/")).dirname);
assertEquals("/foo", (new IO.Path("/foo/bar/")).dirname);
assertEquals("/foo/bar", (new IO.Path("/foo/bar/fie/")).dirname);
assertEquals(".", (new IO.Path("foo")).dirname);
assertEquals(".", (new IO.Path("foo/")).dirname);
assertEquals("/", (new IO.Path("/foo")).dirname);
assertEquals("/", (new IO.Path("/foo/")).dirname);
assertEquals("/", (new IO.Path("/")).dirname);
assertEquals("/", (new IO.Path("//")).dirname);
assertEquals("/", (new IO.Path("///")).dirname);

endScope();
