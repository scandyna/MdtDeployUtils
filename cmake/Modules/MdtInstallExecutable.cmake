# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtInstallExecutable
# --------------------
#
# .. contents:: Summary
#    :local:
#
# Functions
# ^^^^^^^^^
#
# .. command:: mdt_install_executable
#
# Install a executable::
#
#   mdt_install_executable(
#     TARGET <target>
#     RUNTIME_DESTINATION <dir>
#     LIBRARY_DESTINATION <dir>
#     [INSTALL_IS_UNIX_SYSTEM_WIDE [TRUE|FALSE]]
#     [COMPONENT <component-name>]
#   )
#
# Will install the executable `target` to ``CMAKE_INSTALL_PREFIX``
# into a subdirectory defined by ``RUNTIME_DESTINATION``.
#
# On Linux,
# the rpath informations is set to ``$ORIGIN/../${LIBRARY_DESTINATION}``.
# If ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``,
# the rpath informations are removed.
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
#
#   mdt_install_executable(
#     TARGET myApp
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     COMPONENT ${PROJECT_NAME}_Tools
#   )
#

# TODO : if subdir not 1 level, document to set RPATH manually

function(mdt_install_executable)

  set(options)
  set(oneValueArgs TARGET RUNTIME_DESTINATION LIBRARY_DESTINATION INSTALL_IS_UNIX_SYSTEM_WIDE COMPONENT)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "mdt_install_executable(): no target provided")
  endif()
  if(NOT TARGET ${ARG_TARGET})
    message(FATAL_ERROR "mdt_install_executable(): ${ARG_TARGET} is not a valid target")
  endif()
  if(NOT ARG_RUNTIME_DESTINATION)
    message(FATAL_ERROR "mdt_install_executable(): mandatory argument RUNTIME_DESTINATION missing")
  endif()
  if(NOT ARG_LIBRARY_DESTINATION)
    message(FATAL_ERROR "mdt_install_executable(): mandatory argument LIBRARY_DESTINATION missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_install_executable(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  set(componentArguments)
  if(ARG_COMPONENT)
    set(componentArguments COMPONENT ${ARG_COMPONENT})
  endif()

  install(
    TARGETS ${ARG_TARGET}
    RUNTIME DESTINATION "${ARG_RUNTIME_DESTINATION}"
    ${componentArguments}
  )
  
  # TODO: finish implementation

endfunction()
