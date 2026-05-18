# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/morrisaaron/.espressif/v6.0/esp-idf/components/bootloader/subproject")
  file(MAKE_DIRECTORY "/Users/morrisaaron/.espressif/v6.0/esp-idf/components/bootloader/subproject")
endif()
file(MAKE_DIRECTORY
  "/Volumes/External/GitHub/A2ThreeD/esp32-bt2amiga/build-idf6/bootloader"
  "/Volumes/External/GitHub/A2ThreeD/esp32-bt2amiga/build-idf6/bootloader-prefix"
  "/Volumes/External/GitHub/A2ThreeD/esp32-bt2amiga/build-idf6/bootloader-prefix/tmp"
  "/Volumes/External/GitHub/A2ThreeD/esp32-bt2amiga/build-idf6/bootloader-prefix/src/bootloader-stamp"
  "/Volumes/External/GitHub/A2ThreeD/esp32-bt2amiga/build-idf6/bootloader-prefix/src"
  "/Volumes/External/GitHub/A2ThreeD/esp32-bt2amiga/build-idf6/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Volumes/External/GitHub/A2ThreeD/esp32-bt2amiga/build-idf6/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Volumes/External/GitHub/A2ThreeD/esp32-bt2amiga/build-idf6/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
