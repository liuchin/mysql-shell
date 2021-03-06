# Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

# This is a small CMake project to copy this directory to the top of
# the source, and at the same time do some modifications to these
# files

cmake_minimum_required(VERSION 2.8.5)
project(mysh_rpm_init NONE)

# ----------------------------------------------------------------------
# Set some variables to replace
# Use the version variables from "version.cmake"
# ----------------------------------------------------------------------

include(../../version.cmake)

#set(YEAR      2016)  # FIXME automate somehow
set(PRODUCT   "MySQL Shell ${MYSH_BASE_VERSION}")

if(EXISTS "${CMAKE_SOURCE_DIR}/../../LICENSE.mysql")
  set(PRODUCT_SUFFIX "-commercial")
  set(LICENSE_FILE   "LICENSE.mysql")
  set(LICENSE_TYPE   "Commercial")
  if(NOT RPM_RELEASE)
    if(MYSH_LEVEL)
      set(RPM_RELEASE "0.1")
    else()
      set(RPM_RELEASE "1.1")
    endif()
  endif()
else()
  set(PRODUCT_SUFFIX "")
  set(LICENSE_FILE   "COPYING.txt")
  set(LICENSE_TYPE   "GPLv2")
  if(NOT RPM_RELEASE)
    if(MYSH_LEVEL)
      set(RPM_RELEASE "0.1")
    else()
      set(RPM_RELEASE "1")
    endif()
  endif()
endif()

# ----------------------------------------------------------------------
# Process and copy the spec file to the top directory
# ----------------------------------------------------------------------

set(DEST_DIR ${CMAKE_SOURCE_DIR}/../../)

file(
  COPY ${CMAKE_SOURCE_DIR}/
  DESTINATION ${DEST_DIR}
  PATTERN "*.in"   EXCLUDE
  PATTERN "CMake*" EXCLUDE
)

set(in_files
  mysql-shell.spec
)

foreach(_in_file ${in_files})
  configure_file(
    ${CMAKE_SOURCE_DIR}/${_in_file}.in
    ${DEST_DIR}/${_in_file}
    @ONLY
  )
endforeach()
