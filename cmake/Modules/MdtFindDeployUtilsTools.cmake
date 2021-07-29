# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtFindDeployUtilsTools
# -----------------------
#
# .. contents:: Summary
#    :local:
#
#
# .. command:: mdt_find_deploy_utils_tools
#
# Find the tools required for MdtDeployUtils CMake modules::
#
#   mdt_find_deploy_utils_tools()
#
# For all host platforms, a IMPORTED target named ``mdtdeployutils`` will be created.
#
# TODO: update ! ldd and objdump probably not needed anymore
#
# On Linux host, a IMPORTED target named ``ldd`` will also be created.
#
# On Windows host, a IMPORTED target named ``objdump`` will also be created.
#
#
# Example:
#
# .. code-block:: cmake
#
#   find_package(Mdt0DeployUtilsCMake REQUIRED)
#   mdt_find_deploy_utils_tools()
#
#   mdt_deploy_application(...)
#
#

function(mdt_find_deploy_utils_tools)

  find_program(mdtdeployutilsPath NAMES mdtdeployutils)
  if(NOT mdtdeployutilsPath)
    message(FATAL_ERROR "mdt_find_deploy_utils_tools(): could not fin mdtdeployutils")
  endif()

  add_executable(mdtdeployutils IMPORTED)
  set_target_properties(mdtdeployutils PROPERTIES IMPORTED_LOCATION "${mdtdeployutilsPath}")

  if(CMAKE_HOST_UNIX AND NOT CMAKE_HOST_APPLE)

    find_program(lddPath NAMES ldd)
    if(NOT lddPath)
      message(FATAL_ERROR "mdt_find_deploy_utils_tools(): could not fin ldd")
    endif()

    add_executable(ldd IMPORTED)
    set_target_properties(ldd PROPERTIES IMPORTED_LOCATION "${lddPath}")

  endif()

  if(CMAKE_HOST_WIN32)

    find_program(objdumpPath NAMES objdump)
    if(NOT objdumpPath)
      message(FATAL_ERROR "mdt_find_deploy_utils_tools(): could not fin objdump")
    endif()

    add_executable(objdump IMPORTED)
    set_target_properties(objdump PROPERTIES IMPORTED_LOCATION "${objdumpPath}")

  endif()

endfunction()
