# Copyright 2013 Jens Lindström
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.

ifeq ($(debug),yes)
v8buildtype ?= Debug
else
v8buildtype ?= Release
endif

ifeq ($(filter x86_64,$(shell uname -m)),x86_64)
v8arch ?= x64
else
v8arch ?= ia32
endif

v8builddir = v8/out/$(v8buildtype)
v8cflags += BUILDTYPE=$(v8buildtype) builddir=$(abspath $(v8builddir))

ifeq ($(v8static),yes)
v8lib := $(v8builddir)/obj.target/tools/gyp
v8targets := v8_base.$(v8arch) v8_nosnapshot.$(v8arch) v8_snapshot
v8objects := $(v8targets:%=$(v8lib)/lib%.a)
else
v8lib := $(v8builddir)/lib.target
v8mflags += library=shared
v8targets := v8

library_paths += $(v8lib)
libraries += v8
lflags += -Wl,-rpath=$(abspath $(v8lib))
endif

include_paths += v8/include

v8deps := v8/build/gyp v8/third_party/icu

$(out)/.v8dependencies: v8/Makefile
ifneq ($(v8importdepsfrom),)
	@echo Importing V8 dependencies from \'$(v8importdepsfrom)\'.
	@tar xjf $(v8importdepsfrom)
else
	@$(MAKE) --quiet -C v8 dependencies
ifneq ($(v8exportdepsto),)
	@echo Exporting V8 dependencies to \'$(v8exportdepsto)\'.
	@tar cjf $(v8exportdepsto) $(v8deps)
endif
endif
	@mkdir -p $(shell dirname $@)
	@touch $@

v8dependencies: $(out)/.v8dependencies

$(v8deps): $(out)/.v8dependencies

v8/out/Makefile.$(v8arch): $(v8deps)
	@$(MAKE) --quiet -C v8 $(v8mflags) out/Makefile.$(v8arch)

v8: v8/out/Makefile.$(v8arch)
	@$(MAKE) --quiet -C v8/out -f Makefile.$(v8arch) $(v8cflags) $(v8targets)

v8clean:
	@rm -rf v8/out

.PHONY: v8 v8clean v8dependencies
