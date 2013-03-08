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

setScope("BuiltIn.format");

/* Basic formatting: */

assertEquals("1337", format("%d", 1337));
assertEquals("[1337", format("[%d", 1337));
assertEquals("1337]", format("%d]", 1337));
assertEquals("[1337]", format("[%d]", 1337));

assertEquals("[  1337]", format("[%6d]", 1337));
assertEquals("[1337  ]", format("[%-6d]", 1337));
assertEquals("[001337]", format("[%06d]", 1337));

assertEquals("ffff", format("%x", 65535));
assertEquals("FFFF", format("%X", 65535));

assertEquals("1.23", format("%.2f", 1.23));
assertEquals("1.23e+00", format("%.2e", 1.23));
assertEquals("1.23e+03", format("%.2e", 1234));
assertEquals("1.23", format("%.3g", 1.23));

assertEquals("foo", format("%s", "foo"));
assertEquals("[foo", format("[%s", "foo"));
assertEquals("foo]", format("%s]", "foo"));
assertEquals("[foo]", format("[%s]", "foo"));

assertEquals("[   foo]", format("[%6s]", "foo"));
assertEquals("[foo   ]", format("[%-6s]", "foo"));

/* Type conversion: */

assertEquals("1337", format("%d", "1337"));
assertEquals("65535", format("%d", "0xffff"));
assertEquals("1337", format("%s", 1337));
assertEquals("65535", format("%s", 0xffff));

assertEquals("1337", format("%d", { toString: function () { return 17; },
                                    valueOf: function () { return 1337; }}));
assertEquals("right", format("%s", { toString: function () { return "right"; },
                                     valueOf: function () { return "wrong"; }}));

/* Named specifiers: */

assertEquals("1337", format("%(x)d", { x: 1337 }));
assertEquals("17,1337", format("%d,%(x)d", 17, { x: 1337 }));
assertEquals("1337,17", format("%(x)d,%d", { x: 1337 }, 17));
assertEquals("17,1337,foo", format("%d,%(x)d,%(f)s", 17, { x: 1337, f: "foo" }));
assertEquals("1337,foo,17", format("%(x)d,%(f)s,%d", { x: 1337, f: "foo" }, 17));
assertEquals("17,1337,foo,bar", format("%d,%(x)d,%(f)s,%s", 17, { x: 1337, f: "foo" }, "bar"));

/* JSON: */

assertEquals("1337", format("%r", 1337));
assertEquals('"foo"', format("%r", "foo"));
assertEquals('{"x":17,"y":1337}', format("%r", { x: 17, y: 1337 }));
assertEquals("[  1337]", format("[%6r]", 1337));
assertEquals('[ "foo"]', format("[%6r]", "foo"));
assertEquals('"bytes"', format("%r", Bytes.encode("bytes")));

/* Miscellaneous: */

assertEquals("[%]", format("[%%]"));

endScope();
