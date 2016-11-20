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

compiler ?= v8/third_party/llvm-build/Release+Asserts/bin/clang++
linker ?= $(compiler)

ifeq ($(debug),yes)
v8buildtype ?= debug
else
v8buildtype ?= release
endif

ifeq ($(filter x86_64,$(shell uname -m)),x86_64)
v8arch ?= x64
sysrootarch ?= amd64
else
v8arch ?= ia32
sysrootarch ?= i386
endif

ifeq ($(use_sysroot),yes)
sysroot = v8/build/linux/debian_wheezy_$(sysrootarch)-sysroot
binutils = v8/third_party/binutils/Linux_x64/Release/bin
cflags += --sysroot=$(sysroot) -B$(binutils)
lflags += --sysroot=$(sysroot) -B$(binutils)
endif

# The BUILDTYPE variable is assumed to have values
# with the initial letter capitalized. Everywhere other
# use appears to be all lower-case.
#
# That inconsistency will no doubt disappear, but for now..
#
V8buildtype = $(subst r,R,$(subst d,D,$(v8buildtype)))

v8builddir = v8/out/$(v8arch).$(v8buildtype)
v8cflags += BUILDTYPE=$(V8buildtype) builddir=$(abspath $(v8builddir))

ifeq ($(v8static),yes)
v8targets := v8_base v8_nosnapshot v8_snapshot
libraries += v8_base v8_libbase v8_libsampler v8_libplatform icui18n icuuc v8_nosnapshot

v8objdir = $(v8builddir)/obj.target
v8lib := $(v8objdir)/src
iculib := $(v8objdir)/third_party/icu
else
v8mflags += library=shared
v8targets := v8

v8lib := $(v8builddir)/lib.target

libraries += v8 icuuc icui18n
lflags += -Wl,-rpath=$(abspath $(v8lib))
endif

#library_paths += $(v8lib) $(iculib)
#libraries += v8_base.$(v8arch) icui18n icuuc icudata v8_nosnapshot.$(v8arch)

include_paths += v8/include

v8deps := v8/build/gyp v8/third_party/icu

.gclient:
ifneq ($(v8importdepsfrom),)
	@echo Importing V8 dependencies from \'$(v8importdepsfrom)\'.
	@tar xjf $(v8importdepsfrom)
	@touch $@
else
	@gclient sync --spec='solutions = [ \
	  { \
	    "url": "git://github.com/v8/v8.git", \
	    "managed": False, \
	    "name": "v8", \
	    "deps_file": "DEPS", \
	    "custom_deps": {}, \
	  }, \
	]'
ifneq ($(v8exportdepsto),)
	@echo Exporting V8 dependencies to \'$(v8exportdepsto)\'.
	@tar cjf $(v8exportdepsto) $$(python listdeps.py v8)
endif
endif

v8dependencies: .gclient

v8: v8dependencies
	@$(MAKE) --quiet -C v8 $(v8cflags) $(v8arch).$(v8buildtype)

v8clean:
	@rm -rf v8/out

.PHONY: v8 v8clean v8dependencies
