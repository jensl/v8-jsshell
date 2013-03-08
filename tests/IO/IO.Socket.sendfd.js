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

setScope("IO.Socket.sendfd");

test([
  function () {
    var socket = new IO.Socket("unix", "stream");

    socket.bind(IO.SocketAddress.unix("tests/output/sendfd.unix"));
    socket.listen(1);

    var process = new OS.Process();

    process.start();

    if (process.isSelf) {
      socket.close();

      scoped(new IO.Socket("unix", "stream"), function () {
        var parent = this;
        parent.connect(IO.SocketAddress.unix("tests/output/sendfd.unix"));
        scoped(new IO.File("tests/input/helloworld.txt"), function () {
          parent.sendfd(this);
        });
      });

      exit(0);
    } else {
      scoped(socket.accept(), function () {
        var fd = this.recvfd();
        scoped(IO.File.fdopen(fd, "r"), function () {
          assertEquals("Hello world!\n", this.read().decode());
        });
      });

      process.wait();
    }
  }
]);

endScope();
