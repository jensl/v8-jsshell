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

function HTTPServer() {
  this.process = null;
}

HTTPServer.prototype.start = function () {
  this.process = new OS.Process();

  var socket = new IO.Socket("internet", "stream");

  socket.bind(IO.SocketAddress.internet());
  socket.listen(1);

  this.process.start();

  if (!this.process.isSelf) {
    this.port = socket.localAddress.internet.port;
    this.prefix = format("http://localhost:%d", this.port);
    socket.close();
    return;
  }

  while (true) {
    var client = socket.accept();
    var buffered = "";

    while (!/(?:\r?\n){2}/.test(buffered)) {
      var data = client.recv(4096);
      if (data === null)
        break;
      buffered += data.decode();
    }

    var match = /(?:\r?\n){2}/.exec(buffered);
    if (!match) {
      IO.File.stderr.write("Invalid HTTP request!\n");
      exit(1);
    }

    var request = buffered.substring(0, match.index).split(/\r?\n/g);

    buffered = buffered.substring(match.index + match[0].length);

    var words = request.shift().split(" ");
    var method = words[0];
    var url = words[1];
    var version = words[2];
    var headers = {};
    var body = null;

    request.forEach(function (line) {
      var match = /^(.*?):\s*(.*)$/.exec(line);
      headers[match[1]] = match[2];
    });

    if (method == "POST" || method == "PUT") {
      body = buffered;
      if ("Content-Length" in headers) {
        var length = parseInt(headers["Content-Length"]);
        while (body.length < length) {
          var data = client.recv(4096);
          if (data === null)
            break;
          body += data.decode();
        }
      } else {
        while (true) {
          var data = client.recv(4096);
          if (data === null)
            break;
          body += data.decode();
        }
      }
    }

    if (url == "/secret/basic" &&
	(!("Authorization" in headers)
	 || !/^Basic /.test(headers["Authorization"]))) {
      client.send("HTTP/1.0 401 Who are you?\r\n");
      client.send("WWW-Authenticate: Basic realm=\"secret\"\r\n");
      client.send("\r\n");
    } else if (url == "/secret/digest" &&
               (!("Authorization" in headers)
                || !/^Digest /.test(headers["Authorization"]))) {
      client.send("HTTP/1.0 401 Who are you?\r\n");
      client.send("WWW-Authenticate: Digest realm=\"secret\", " +
                  "nonce=\"nonce\", opaque=\"opaque\"\r\n");
      client.send("\r\n");
    } else if (url == "/error") {
      client.send("HTTP/1.0 404 Not Found\r\n");
      client.send("\r\n");
    } else if (url == "/invalid_status_code") {
      client.send("HTTP/1.0 dead Not Found\r\n");
      client.send("\r\n");
    } else {
      client.send("HTTP/1.0 200 OK\r\n");
      client.send("Content-Type: text/json\r\n");
      client.send("\r\n");
      client.send(JSON.stringify({ method: method,
                                   url: url,
                                   version: version,
                                   headers: headers,
                                   body: body }));
    }

    client.close();
  }

  exit(0);
};

HTTPServer.prototype.close = function () {
  this.process.kill(15);
  this.process.wait();
};
