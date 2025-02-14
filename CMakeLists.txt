# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)
project(IEAudio4Linux VERSION 1.0.0 LANGUAGES CXX)

message("\n------------------------------------------------------------")
message("IEAudio4Linux\n")

message("Compiler Setup")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
message("Using Compiler: ${CMAKE_CXX_COMPILER_ID}")
message("version: ${CMAKE_CXX_COMPILER_VERSION}")
message("CXX standard: ${CMAKE_CXX_STANDARD}")
message("Working environment")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System version: ${CMAKE_SYSTEM_VERSION}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")

message("Gathering source files and creating Library")
file(GLOB IEAudio4Linux_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
add_library(${PROJECT_NAME} SHARED ${IEAudio4Linux_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ".")
file(GLOB IEAudio4Linux_HEADER_FILES "IEAudio4Linux.h")
set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER ${IEAudio4Linux_HEADER_FILES})

message("Linking ALSA")
find_package(ALSA REQUIRED)
if(ALSA_FOUND OR ALSA)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${ALSA_LIBRARIES})
  target_include_directories(${PROJECT_NAME} PUBLIC ${ALSA_INCLUDE_DIRS})
  set(CMAKE_C_FLAGS ${ALSA_CFLAGS})
endif()

message("Setting up install")
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
include(InstallRequiredSystemLibraries)

message("------------------------------------------------------------\n")