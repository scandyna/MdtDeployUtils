# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtDeployApplication
# --------------------
#
# .. contents:: Summary
#    :local:
#
# Deploy a application
# ^^^^^^^^^^^^^^^^^^^^
#
# .. command:: mdt_deploy_application
#
# Deploy a application with `target` as executable::
#
#   mdt_deploy_application(
#     TARGET <target>
#     RUNTIME_DESTINATION <dir>
#     LIBRARY_DESTINATION <dir>
#     [INSTALL_IS_UNIX_SYSTEM_WIDE [TRUE|FALSE]]
#     [COMPONENT <component-name>]
#     [MDT_DEPLOY_UTILS_EXECUTABLE name or path]
#     [LDD_EXECUTABLE name or path]
#     [OBJDUMP_EXECUTABLE name or path]
#   )
#
# Will install `target` using :command:`mdt_install_executable()`.
# The shared libraries the executable depends on
# are also installed using :command:`mdt_install_shared_libraries_target_depends_on()`.
#
# Example:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   add_executable(myApp myApp.cpp)
#   target_link_libraries(myApp PRIVATE Qt5::Core)
#   target_link_libraries(myApp PRIVATE Mdt0::PlainText)
#   target_link_libraries(myApp PRIVATE myLibA)
#
#   mdt_deploy_application(
#     TARGET myApp
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     COMPONENT ${PROJECT_NAME}_Tools
#   )
#
# Specify path to required tools
# """"""""""""""""""""""""""""""
#
# By default, ``mdt_deploy_application()`` expects specific targets as tools.
# Those targets are defined using :command:`mdt_find_deploy_utils_tools()`.
#
# If using ``mdt_find_deploy_utils_tools()`` is not possible
# (for example because of name clashes),
# it is possible to specify the path for the required tools.
#
# Example:
#
# .. code-block:: cmake
#
#   find_program(mdtDeployUtilsPath NAMES mdtdeployutils)
#   find_program(objdumpPath NAMES objdump)
#
#   mdt_deploy_application(
#     TARGET myApp
#     ...
#     MDT_DEPLOY_UTILS_EXECUTABLE "${mdtDeployUtilsPath}"
#     LDD_EXECUTABLE ldd
#     OBJDUMP_EXECUTABLE "${objdumpPath}"
#   )
#
# On a Windows host, the ``LDD_EXECUTABLE`` argument is simply ignored
# (this avoids the caller having to conditionally call ``mdt_deploy_application()``
# depending on the host platform).
#

include(MdtInstallExecutable)
include(MdtSharedLibrariesDepencyHelpers)

function(mdt_deploy_application)

  set(options)
  set(oneValueArgs TARGET RUNTIME_DESTINATION LIBRARY_DESTINATION INSTALL_IS_UNIX_SYSTEM_WIDE COMPONENT MDT_DEPLOY_UTILS_EXECUTABLE LDD_EXECUTABLE OBJDUMP_EXECUTABLE)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "mdt_deploy_application(): no target provided")
  endif()
  if(NOT TARGET ${ARG_TARGET})
    message(FATAL_ERROR "mdt_deploy_application(): ${ARG_TARGET} is not a valid target")
  endif()
  if(NOT ARG_RUNTIME_DESTINATION)
    message(FATAL_ERROR "mdt_deploy_application(): mandatory argument RUNTIME_DESTINATION missing")
  endif()
  if(NOT ARG_LIBRARY_DESTINATION)
    message(FATAL_ERROR "mdt_deploy_application(): mandatory argument LIBRARY_DESTINATION missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_deploy_application(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(ARG_MDT_DEPLOY_UTILS_EXECUTABLE)
    set(mdtdeployutilsPath "${ARG_MDT_DEPLOY_UTILS_EXECUTABLE}")
  else()
    if(NOT TARGET mdtdeployutils)
      message(FATAL_ERROR "mdt_deploy_application(): missing the required target mdtdeployutils."
                          "Either use mdt_find_deploy_utils_tools(), "
                          "or pass the path to mdtdeployutils using the MDT_DEPLOY_UTILS_EXECUTABLE argument.")
    endif()
#     get_target_property(mdtdeployutilsPath mdtdeployutils LOCATION)
    set(mdtdeployutilsPath "$<TARGET_FILE:mdtdeployutils>")
  endif()
  if(NOT mdtdeployutilsPath)
    message(FATAL_ERROR "mdt_deploy_application(): could not get path to mdtdeployutils")
  endif()
  
  # TODO: also handle LDD_EXECUTABLE and OBJDUMP_EXECUTABLE
  # Maybe add a helper function in MdtFindDeployUtilsTools
  
  message("mdtdeployutilsPath: ${mdtdeployutilsPath}")

  mdt_install_executable(
    TARGET ${ARG_TARGET}
    RUNTIME_DESTINATION ${ARG_RUNTIME_DESTINATION}
    LIBRARY_DESTINATION ${ARG_LIBRARY_DESTINATION}
    INSTALL_IS_UNIX_SYSTEM_WIDE ${ARG_INSTALL_IS_UNIX_SYSTEM_WIDE}
    COMPONENT ${ARG_COMPONENT}
  )

  mdt_install_shared_libraries_target_depends_on(
    TARGET ${ARG_TARGET}
    RUNTIME_DESTINATION ${ARG_RUNTIME_DESTINATION}
    LIBRARY_DESTINATION ${ARG_LIBRARY_DESTINATION}
    INSTALL_IS_UNIX_SYSTEM_WIDE ${ARG_INSTALL_IS_UNIX_SYSTEM_WIDE}
    COMPONENT ${ARG_COMPONENT}
  )

endfunction()
