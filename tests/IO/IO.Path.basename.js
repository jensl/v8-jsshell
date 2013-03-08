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

setScope("IO.Path.basename");

assertEquals("foo", IO.Path.basename("foo"));
assertEquals("bar", IO.Path.basename("foo/bar"));
assertEquals("fie", IO.Path.basename("foo/bar/fie"));
assertEquals("bar", IO.Path.basename("/foo/bar"));
assertEquals("fie", IO.Path.basename("/foo/bar/fie"));
assertEquals("foo", IO.Path.basename("foo/"));
assertEquals("bar", IO.Path.basename("foo/bar/"));
assertEquals("fie", IO.Path.basename("foo/bar/fie/"));
assertEquals("bar", IO.Path.basename("/foo/bar/"));
assertEquals("fie", IO.Path.basename("/foo/bar/fie/"));
assertEquals("/", IO.Path.basename("/"));
assertEquals("/", IO.Path.basename("//"));
assertEquals("/", IO.Path.basename("///"));

assertEquals("foo", (new IO.Path("foo")).basename);
assertEquals("bar", (new IO.Path("foo/bar")).basename);
assertEquals("fie", (new IO.Path("foo/bar/fie")).basename);
assertEquals("bar", (new IO.Path("/foo/bar")).basename);
assertEquals("fie", (new IO.Path("/foo/bar/fie")).basename);
assertEquals("foo", (new IO.Path("foo/")).basename);
assertEquals("bar", (new IO.Path("foo/bar/")).basename);
assertEquals("fie", (new IO.Path("foo/bar/fie/")).basename);
assertEquals("bar", (new IO.Path("/foo/bar/")).basename);
assertEquals("fie", (new IO.Path("/foo/bar/fie/")).basename);
assertEquals("/", (new IO.Path("/")).basename);
assertEquals("/", (new IO.Path("//")).basename);
assertEquals("/", (new IO.Path("///")).basename);

endScope();
