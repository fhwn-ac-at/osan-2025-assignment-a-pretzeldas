# Benjamin Eder, 210894
# Based on template

# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.15)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  BNI
  VERSION 1.0
  LANGUAGES C)

set(CMAKE_C_COMPILER "/usr/bin/clang")

add_library(Log log.c log.h)
add_library(Worker worker.c worker.h)
add_library(CliArgs cli_args.c cli_args.h)

# Link each target with other targets or add options, etc.

add_executable(Ventilator ventilator.c)
target_link_libraries(Ventilator PRIVATE Log PRIVATE Worker PRIVATE CliArgs)
                       
set_property(TARGET Ventilator PROPERTY C_STANDARD 17)
set_target_properties(Ventilator PROPERTIES OUTPUT_NAME "ventilator.out")