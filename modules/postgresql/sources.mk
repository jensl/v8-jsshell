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

ifeq ($(postgresql),yes)

ifeq ($(use_sysroot),yes)
pg_config ?= $(sysroot)/usr/bin/pg_config
else
pg_config ?= pg_config
endif

postgresql_major = $(shell $(pg_config) --version | sed -e "s/PostgreSQL \\([0-9]*\\)\\.\\([0-9]*\\).*/\\1/")
postgresql_minor = $(shell $(pg_config) --version | sed -e "s/PostgreSQL \\([0-9]*\\)\\.\\([0-9]*\\).*/\\2/")

common_sources += modules/postgresql/Connection.cc \
                  modules/postgresql/Statement.cc \
                  modules/postgresql/Result.cc \
                  modules/postgresql/Row.cc \
                  modules/postgresql/Error.cc \
                  modules/postgresql/Formatter.cc \
                  modules/postgresql/Utilities.cc \
                  modules/postgresql/Types.cc
defines += POSTGRESQL_SUPPORT=1 \
           POSTGRESQL_MAJOR=$(postgresql_major) \
           POSTGRESQL_MINOR=$(postgresql_minor)
pg_include_paths = $(shell $(pg_config) --includedir)
pg_library_paths = $(filter-out /lib /usr/lib /usr/local/lib,$(shell $(pg_config) --libdir))
include_paths += $(pg_include_paths:%=$(sysroot)%)
library_paths += $(pg_library_paths:%=$(sysroot)%)
libraries += pq

endif
