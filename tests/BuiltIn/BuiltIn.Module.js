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

setScope("BuiltIn.Module");

test([
  function () {
    assertEquals("object", typeof Module.global);
    assertEquals("function", typeof Module.load);
    assertEquals("function", typeof Module.assign);
    assertEquals(test, Module.global.test);
  },

  function () {
    var module = new Module();

    assertEquals("object", typeof module.global);
    assertEquals("function", typeof module.load);
    assertEquals("function", typeof module.eval);
    assertEquals("function", typeof module.close);
  },

  function () {
    var module_with_io = new Module({ IO: true });
    assertEquals("function", module_with_io.eval("typeof IO;"));
    assertEquals("function", module_with_io.eval("var child = new Module({ IO: true }); child.eval('typeof IO;');"));

    var module_without_io = new Module({ IO: false });
    assertEquals("undefined", module_without_io.eval("typeof IO;"));
    assertEquals("undefined", module_without_io.eval("var child = new Module({ IO: true }); child.eval('typeof IO;');"));
  },

  function () {
    var module_with_os = new Module({ OS: true });
    assertEquals("function", module_with_os.eval("typeof OS;"));
    assertEquals("function", module_with_os.eval("var child = new Module({ OS: true }); child.eval('typeof OS;');"));

    var module_without_os = new Module({ OS: false });
    assertEquals("undefined", module_without_os.eval("typeof OS;"));
    assertEquals("undefined", module_without_os.eval("var child = new Module({ OS: true }); child.eval('typeof OS;');"));
  },

  function () {
    var module = new Module();

    module.eval("var x = 10;");
    assertEquals("number", typeof module.global.x);

    module.eval("var y = 'foo';");
    assertEquals("string", typeof module.global.y);
  }
]);

endScope();
