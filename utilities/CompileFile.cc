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

#include "Base.h"

#include "utilities/CompileFile.h"

#include <unistd.h>
#include <string.h>

#include <memory>

#include "base/String.h"
#include "utilities/File.h"

namespace utilities {

v8::Local<v8::Script> CompileFile(std::string filename, std::string encoding) {
  File file(filename, "r");
  std::string raw_source(file.read(encoding));

  std::unique_ptr<v8::ScriptCompiler::CachedData> cached_data;
  std::string cached_data_string;
  std::string filename_cached(filename + "c");

  if (filename.length() > 3 &&
      filename.compare(filename.length() - 3, 3, ".js") == 0) {
    try {
      File file_cached(filename_cached, "r");

      if (file.mtime() <= file_cached.mtime()) {
        cached_data_string = file_cached.read();
        cached_data.reset(new v8::ScriptCompiler::CachedData(
            reinterpret_cast<const uint8_t*>(cached_data_string.data()),
            cached_data_string.size()));
      }
    } catch (File::Error &) {
      /* Failed to read a *.jsc file.  That's okay, just ignore it. */
    }
  }

  v8::ScriptOrigin origin(base::String(filename).ToV8());
  v8::ScriptCompiler::Source source(base::String(raw_source).ToV8(), origin,
                                    cached_data.release());
  v8::ScriptCompiler::CompileOptions compile_options;

  if (cached_data) {
    compile_options = static_cast<v8::ScriptCompiler::CompileOptions>(
        v8::ScriptCompiler::kConsumeParserCache);
  } else {
    compile_options = static_cast<v8::ScriptCompiler::CompileOptions>(
        v8::ScriptCompiler::kProduceParserCache);
  }

  v8::MaybeLocal<v8::UnboundScript> unbound_script(
      v8::ScriptCompiler::CompileUnboundScript(
          CurrentIsolate(), &source, compile_options));

  if (unbound_script.IsEmpty())
    return v8::Local<v8::Script>();

  if (!cached_data && source.GetCachedData()) {
    const v8::ScriptCompiler::CachedData* cached_data = source.GetCachedData();

    try {
      File file_cached(filename_cached, "w");

      file_cached.write(std::string(
          reinterpret_cast<const char*>(cached_data->data),
          cached_data->length));
    } catch (File::Error error) {
      /* Couldn't open the file for writing.  That's okay, just ignore it. */
    }
  }

  return unbound_script.ToLocalChecked()->BindToCurrentContext();
}

}
