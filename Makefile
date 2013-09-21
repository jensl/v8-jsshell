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

-include user.mk

standard ?= c++11
compiler ?= g++
linker ?= $(compiler)
debug ?= no
v8static ?= no

ifeq ($(debug),yes)
cflags ?= -g
else
cflags ?= -O2
endif

components = base conversions glue api utilities modules jsshell
fragments = $(components:%=%/sources.mk)
libraries += pthread

include $(fragments)

out = out

common_objects = $(common_sources:%.cc=$(out)/obj/%.o)
jsshell_objects = $(jsshell_sources:%.cc=$(out)/obj/%.o)
objects = $(common_objects) $(jsshell_objects)

targets = $(out)/jsshell test-jsshell
makefiles = Makefile user.mk $(fragments)

all: $(targets)

include v8.mk

cflags += -I. $(include_paths:%=-I%) -MMD -Wall -Werror -std=$(standard) $(defines:%=-D%)
lflags += $(library_paths:%=-L%)

ifneq ($(debug),no)
cflags += -ggdb
endif

$(objects): $(out)/obj/%.o: %.cc Makefile user.mk $(fragments)
	@echo CC\( $< \)
	@mkdir -p $(dir $@)
	@$(compiler) $(cflags) -o $@ -c $<

$(out)/jsshell: v8 $(common_objects) $(jsshell_objects)
	@echo LINK\( jsshell \)
	@mkdir -p $(out)
	@$(linker) $(lflags) -o $@ $(common_objects) $(jsshell_objects) $(v8objects) $(libraries:%=-l%)

$(out)/.jsshell.tested: $(out)/jsshell $(wildcard tests/*.js)
	@rm -rf tests/output
	@mkdir tests/output
	@$(out)/jsshell --enable=Testing tests/all.js
	@touch $@

clean: v8clean
	@rm -rf $(out)

test-jsshell: $(out)/.jsshell.tested

.PHONY: clean test-jsshell

-include $(objects:%.o=%.d)

%.h:
	@echo $@ \(ignored\)

user.mk:
	@echo Creating empty user.mk
	@touch user.mk
