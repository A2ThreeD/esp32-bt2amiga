# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(BLUEPAD32_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/external/bluepad32)
set(EXTRA_COMPONENT_DIRS
    ${BLUEPAD32_ROOT}/src/components/bluepad32
    ${BLUEPAD32_ROOT}/src/components/btstack
    ${BLUEPAD32_ROOT}/src/components/cmd_nvs
    ${BLUEPAD32_ROOT}/src/components/cmd_system
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-bt2amiga)
