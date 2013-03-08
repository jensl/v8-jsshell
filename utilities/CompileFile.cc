/* -*- mode: c++ -*- */
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

#include "utilities/CompileFile.h"

#include <unistd.h>
#include <string.h>

#include <memory>

#include "utilities/File.h"

namespace utilities {

v8::Handle<v8::Script> CompileFile(std::string filename, std::string encoding) {
  File file(filename, "r");
  std::string source(file.read(encoding));

  std::unique_ptr<v8::ScriptData> precompiled;
  std::string data_precompiled;

  if (filename.length() > 3 &&
      filename.compare(filename.length() - 3, 3, ".js") == 0) {
    std::string filename_precompiled(filename + "c");

    try {
      File file_precompiled(filename_precompiled, "r");

      if (file.mtime() <= file_precompiled.mtime()) {
        data_precompiled = file_precompiled.read();

        precompiled.reset(v8::ScriptData::New(data_precompiled.c_str(),
                                              data_precompiled.length()));
      }
    } catch (File::Error &) {
      /* Failed to read a *.jsc file.  That's okay, just ignore it. */
    }

    if (!precompiled) {
      try {
        File file_precompiled(filename_precompiled, "w");

        precompiled.reset(
            v8::ScriptData::PreCompile(source.c_str(), source.length()));

        if (precompiled) {
          file_precompiled.write(
              std::string(precompiled->Data(), precompiled->Length()));
        }
      } catch (File::Error error) {
	//fprintf(stderr, "Failed to write %s: %s\n", filename.c_str(), error.message().c_str());

        /* Couldn't open the file for writing.  That's okay, just ignore it. */
      }
    }
  }

  v8::ScriptOrigin origin(v8::String::New(filename.c_str(), filename.length()));

  if (precompiled) {
    v8::TryCatch try_catch;

    v8::Handle<v8::Script> script(
        v8::Script::Compile(v8::String::New(source.c_str(), source.length()),
                            &origin, precompiled.get()));

    if (!try_catch.HasCaught())
      return script;
  }
  
  return v8::Script::Compile(v8::String::New(source.c_str(), source.length()),
                             &origin);
}

}
