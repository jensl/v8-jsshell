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

Module.load("URL.HTTPServer.js");

setScope("URL");

test([
  function () {
    scoped(new HTTPServer, function () {
      this.start();

      var response = JSON.parse(URL.get(this.prefix + "/testing"));

      assertEquals("GET", response.method);
      assertEquals("/testing", response.url);
      assertEquals("HTTP/1.1", response.version);
    });
  },

  function () {
    scoped(new HTTPServer, function () {
      this.start();

      var response = JSON.parse(URL.get(this.prefix + "/testing",
                                        { headers: { Foo: "Bar" }}));

      assertEquals("Bar", response.headers["Foo"]);
    });
  },

  function () {
    scoped(new HTTPServer, function () {
      this.start();

      var response = JSON.parse(URL.post(this.prefix + "/testing", "Hello world!"));

      assertEquals("POST", response.method);
      assertEquals("/testing", response.url);
      assertEquals("HTTP/1.1", response.version);
      assertEquals("Hello world!", response.body);
      assertEquals("application/x-www-form-urlencoded", response.headers["Content-Type"]);
    });
  },

  function () {
    scoped(new HTTPServer, function () {
      this.start();

      var response = JSON.parse(URL.post(this.prefix + "/testing", "Hello world!",
                                         { headers: { "Content-Type": "text/plain" }}));

      assertEquals("Hello world!", response.body);
      assertEquals("text/plain", response.headers["Content-Type"]);
    });
  },

  function () {
    scoped(new HTTPServer, function () {
      this.start();

      var response = JSON.parse(URL.get(this.prefix + "/secret",
                                        { username: "foo", password: "bar" }));

      assertEquals("Basic Zm9vOmJhcg==", response.headers["Authorization"]);
    });
  },

  function () {
    scoped(new HTTPServer, function () {
      this.start();
      var response = JSON.parse(URL.put(this.prefix + "/testing", "Alice", {
	  headers: {"Content-Type": 'text/plain'}
      }));
      assertEquals(response.method, "PUT");
      assertEquals("Alice", response.body);
      assertEquals("text/plain", response.headers["Content-Type"]);
    })
  },

  function () {
    scoped(new HTTPServer, function () {
      this.start();

      var payload = "";
      for (var i = 0; i < 10000; i++) {
          payload += "Hello world";
      }
      var response = JSON.parse(URL.put(this.prefix + "/testing", payload, {
	  headers: {"Content-Type": 'text/plain'}
      }));
      assertEquals(response.method, "PUT");
      assertEquals(response.body, payload);
      assertEquals("text/plain", response.headers["Content-Type"]);
    })
  },

  function () {
    scoped(new HTTPServer, function () {
      this.start();

      var response = JSON.parse(URL.delete(this.prefix + "/testing", {
	  headers: {"Content-Type": 'text/plain'}
      }));
      assertEquals("DELETE", response.method);
      assertEquals("text/plain", response.headers["Content-Type"]);
    })
  }

]);

endScope();
