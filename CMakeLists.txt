cmake_minimum_required(VERSION 3.5)
project(viw)

# Setup color encoding for pretty outputs
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset "${Esc}[m")
  set(ColorBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

message ("${BoldBlue}System Family${ColorReset} -> ${BoldYellow}${CMAKE_SYSTEM_NAME}${ColorReset}")

# Export compile commands for build systems and editors
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Requires nCurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

#SET (CMAKE_CXX_FLAGS "${_CXX11_FLAGS} ${CMAKE_CXX_FLAGS} -Wall -Werror")
SET (CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu11")
# SET (CMAKE_C_FLAGS_DEBUG "${_CXX11_FLAGS} ${CMAKE_CXX_FLAGS} -Wall -Werror -g")
# SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
# SET (CMAKE_C_FLAGS_RELEASE        "-Os -DNDEBUG")
# SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SOURCE_FILES
    "src/*.h"
    "src/*.c")

add_executable(viw ${SOURCE_FILES})
target_link_libraries(viw ${CURSES_LIBRARIES})
