# Test of the functions in the CMake Modules file:
#
include(GetPrerequisites)

set(CMAKE_BUILD_TYPE "")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo")
set(CMAKE_EXECUTABLE_SUFFIX ".exe")


message(STATUS "=============================================================================")
message(STATUS "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)")
message(STATUS "")
message(STATUS "CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}'")
message(STATUS "CMAKE_CONFIGURATION_TYPES='${CMAKE_CONFIGURATION_TYPES}'")
message(STATUS "CMAKE_EXECUTABLE_SUFFIX='${CMAKE_EXECUTABLE_SUFFIX}'")
message(STATUS "CTEST_CONFIGURATION_TYPE='${CTEST_CONFIGURATION_TYPE}'")
message(STATUS "")


function(stresstest_list_prerequisites file)
  message(STATUS "=============================================================================")
  message(STATUS "stresstest_list_prerequisites file='${file}'")
  message(STATUS "")

  get_filename_component(file_full "${file}" ABSOLUTE)

  message(STATUS "list_prerequisites '${file_full}' 0 0 0")
  list_prerequisites("${file_full}" 0 0 0)
  message(STATUS "")

  message(STATUS "list_prerequisites '${file_full}' 0 0 1")
  list_prerequisites("${file_full}" 0 0 1)
  message(STATUS "")

  message(STATUS "list_prerequisites '${file_full}' 0 1 0")
  list_prerequisites("${file_full}" 0 1 0)
  message(STATUS "")

  message(STATUS "list_prerequisites '${file_full}' 0 1 1")
  list_prerequisites("${file_full}" 0 1 1)
  message(STATUS "")

  message(STATUS "list_prerequisites '${file_full}' 1 0 0")
  list_prerequisites("${file_full}" 1 0 0)
  message(STATUS "")

  message(STATUS "list_prerequisites '${file_full}' 1 0 1")
  list_prerequisites("${file_full}" 1 0 1)
  message(STATUS "")

  message(STATUS "list_prerequisites '${file_full}' 1 1 0")
  list_prerequisites("${file_full}" 1 1 0)
  message(STATUS "")

  message(STATUS "list_prerequisites '${file_full}' 1 1 1")
  list_prerequisites("${file_full}" 1 1 1)
  message(STATUS "")

  message(STATUS "=============================================================================")
  message(STATUS "")
endfunction()


function(test_cmake_executables)
  message(STATUS "=============================================================================")
  message(STATUS "Loop over all executable files in the same directory with CMake")
  message(STATUS "")

  get_filename_component(cmake_bin_dir "${CMAKE_COMMAND}" PATH)
  list_prerequisites_by_glob(GLOB "${cmake_bin_dir}/*" 0 0 1)
endfunction()


message(STATUS "=============================================================================")
message(STATUS "Simplest test - list all the direct prerequisites of CMake itself")
message(STATUS "")
list_prerequisites("${CMAKE_COMMAND}" 0 0 1)
message(STATUS "")

message(STATUS "=============================================================================")
string(LENGTH "$ENV{PATH}" PATH_LENGTH_BEGIN)
message(STATUS "Begin PATH length is: ${PATH_LENGTH_BEGIN}")
message(STATUS "")


# Leave the code for these tests in here, but turn them off by default... they
# take longer than they're worth during development...
#
set(do_testdefaults 0)
if(do_testdefaults)
  message(STATUS "=============================================================================")
  message(STATUS "Test default argument values to list_prerequisites function...")
  message(STATUS "")
  list_prerequisites("${CMAKE_COMMAND}")
  message(STATUS "")
endif()


set(do_stresstest 0)
if(do_stresstest)
  message(STATUS "=============================================================================")
  message(STATUS "stresstest_list_prerequisites with CMake itself...")
  message(STATUS "")
  stresstest_list_prerequisites("${CMAKE_COMMAND}")
  message(STATUS "")
endif()


test_cmake_executables()


message(STATUS "=============================================================================")
message(STATUS "Test overriding 'gp_tool' with bogus value")
message(STATUS "")
set(gp_tool "bogus")
list_prerequisites("${CMAKE_COMMAND}" 0 0 0)
set(gp_tool)
message(STATUS "")


message(STATUS "=============================================================================")
message(STATUS "Test overriding 'gp_tool' with value unlikely to be found")
message(STATUS "")
if(APPLE)
  set(gp_tool "dumpbin")
else()
  set(gp_tool "otool")
endif()
set(gp_cmd "gp_cmd-NOTFOUND")
list_prerequisites("${CMAKE_COMMAND}" 0 0 0)
set(gp_cmd)
set(gp_tool)
message(STATUS "")


message(STATUS "=============================================================================")
message(STATUS "All variables: (Make sure functions/macros are not leaving stuff around...")
message(STATUS "                Only variables predefined by CMake and defined in this")
message(STATUS "                test script file should be listed here...)")
message(STATUS "")
get_cmake_property(vs VARIABLES)
foreach(v ${vs})
  message(STATUS "${v}='${${v}}'")
endforeach()
message(STATUS "")

message(STATUS "=============================================================================")
string(LENGTH "$ENV{PATH}" PATH_LENGTH_END)
message(STATUS "Final PATH length is: ${PATH_LENGTH_END}")

if(PATH_LENGTH_END GREATER ${PATH_LENGTH_BEGIN})
    message(FATAL_ERROR "list_prerequisties is endlessly appending the path of gp_tool to the PATH.")
endif()
message(STATUS "")


message(STATUS "=============================================================================")
message(STATUS "End of test")
message(STATUS "")
