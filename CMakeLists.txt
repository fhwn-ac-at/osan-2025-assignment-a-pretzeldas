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

#add_library(BST bst.c bst.h)

# Link each target with other targets or add options, etc.

add_executable(Main pfusch.c)
#target_link_libraries(Main 
#                       PRIVATE BST)
                       
set_property(TARGET Main PROPERTY C_STANDARD 17)
set_target_properties(Main PROPERTIES OUTPUT_NAME "pfusch.out")