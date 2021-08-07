# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtSharedLibrariesDepencyHelpers
# --------------------------------
#
# .. contents:: Summary
#    :local:
#
# Copy shared libraries a target depends on
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# .. command:: mdt_copy_shared_libraries_target_depends_on
#
# Copy the shared libraries ``target`` depends on to the destination directory::
#
#   mdt_copy_shared_libraries_target_depends_on(
#     TARGET <target>
#     DESTINATION <dir>
#     [OVERWRITE_BEHAVIOR [KEEP|OVERWRITE|FAIL]]
#     [REMOVE_RPATH [TRUE|FALSE]]
#   )
#
# The shared libraries ``target`` depends on are copied to the location specified by ``DESTINATION``.
#
# If the source and destination locations for a shared library are the same,
# it will not be copied, and its rpath informations will not be changed at all,
# and this regardless of ``REMOVE_RPATH`` and ``OVERWRITE_BEHAVIOR``.
# (The only descent case this happens is a UNIX system wide installation,
# for a target that depends on shared libraries allready installed system wide).
#
# If a shared library allready exists at the destination location,
# but its not the same as the source (source and destination locations are different),
# the behavior is defined by ``OVERWRITE_BEHAVIOR``.
# If ``OVERWRITE_BEHAVIOR`` is ``KEEP``, the destination library will not be changed at all.
# If ``OVERWRITE_BEHAVIOR`` is ``OVERWRITE``, the destination library will replaced.
# If ``OVERWRITE_BEHAVIOR`` is ``FAIL``, a fatal error is thrown.
# By default, the ``OVERWRITE_BEHAVIOR`` is ``FAIL``.
#
# By default, on platform that supports rpath,
# the rpath informations is set to ``$ORIGIN`` for each shared library that has been  copied.
# If ``REMOVE_RPATH`` is ``TRUE``, the rpath informations are removed for each shared library that has been copied.
#
# To find the shared libraries, the rpath informations will be used if available,
# then dependencies will be searched in ``CMAKE_PREFIX_PATH``.
# Some platform specific locations will also be used to find the libraries.
#
# Example:
#
# .. code-block:: cmake
#
#   if(WIN32)
#     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
#   endif()
#
#   add_executable(myApp myApp.cpp)
#   target_link_libraries(myApp PRIVATE Qt5::Core)
#   target_link_libraries(myApp PRIVATE Mdt0::PlainText)
#
#   if(WIN32)
#     mdt_copy_shared_libraries_target_depends_on(
#       TARGET myApp
#       DESTINATION "${CMAKE_BINARY_DIR}/bin"
#       OVERWRITE_BEHAVIOR OVERWRITE
#     )
#   endif()
#
#   add_test(NAME RunMyApp COMMAND myApp)
#
#
# The shared libraries the ``myApp`` executable depends on are copied to ``"${CMAKE_BINARY_DIR}/bin"``.
#
# On platform that supports rpath,
# the rpath informations is set to ``$ORIGIN`` for each shared library that has been copied.
#
#
# Install shared libraries a target depends on
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# .. command:: mdt_install_shared_libraries_target_depends_on
#
# Install the shared libraries ``target`` depends on::
#
#   mdt_install_shared_libraries_target_depends_on(
#     TARGET <target>
#     RUNTIME_DESTINATION <dir>
#     LIBRARY_DESTINATION <dir>
#     [INSTALL_IS_UNIX_SYSTEM_WIDE [TRUE|FALSE]]
#     [COMPONENT <component-name>]
#   )
#
#
# The shared libraries ``target`` depends on are installed to ``CMAKE_INSTALL_PREFIX``,
# into a appropriate subdirectory.
#
# On UNIX, the subdirectory is the one specified by ``LIBRARY_DESTINATION``.
# On Windows, the subdirectory is the one specified by ``RUNTIME_DESTINATION``.
#
# If the source and destination locations for a shared library are the same,
# it will not be copied, and its rpath informations will not be changed at all.
# This can happen on UNIX systems when ``target`` depends on shared libraries installed system wide,
# and ``CMAKE_INSTALL_PREFIX`` refers to a system wide installation (i.e. ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``).
#
# If a shared library ``target`` depends on allready exists at the destination location,
# but it is not the same (source and destination locations are different),
# the destination library will be replaced,
# except if ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``,
# in which case the destination library will not be changed at all.
#
# On platform that supports rpath,
# the rpath informations is set to ``$ORIGIN`` for each shared library that has been installed.
# If ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``,
# the rpath informations are removed for each shared library that has been installed.
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
#   mdt_install_shared_libraries_target_depends_on(
#     TARGET myApp
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     COMPONENT ${PROJECT_NAME}_Tools
#   )
#
# To create a standalone application on Linux, the ``CMAKE_INSTALL_PREFIX`` could be ``~/opt/myapp``.
# In that case, each shared library will be installed to ``~/opt/myapp/lib/``.
# The rpath informations will also be set to ``$ORIGIN`` for each installed shared library.
# If a shared library comes from a system wide location, for example ``/usr/lib/x86_64-linux-gnu/libQt5Core.so``,
# it will also be installed, and its rpath informations will be set to ``$ORIGIN``.
#
# On Linux, the application could also be installed system wide.
# In that case, the ``CMAKE_INSTALL_PREFIX`` will be ``/usr``.
# In that case, each shared library will be installed to ``/usr/lib/``,
# or ``/usr/lib/x86_64-linux-gnu/`` on a Gcc based multi-arch Debian.
# The rpath informations will also be removed for each installed shared library.
# If a shared library comes from a system wide location, for example ``/usr/lib/x86_64-linux-gnu/libQt5Core.so``,
# in which case its source and destination location are the same,
# it will (of course) not be installed, and its rpath informations will not be changed at all.
#
# On Windows, the shared libraries the ``myApp`` executable depends on are installed to ``${CMAKE_INSTALL_PREFIX}/bin``.
#

function(mdt_copy_shared_libraries_target_depends_on)

  set(options)
  set(oneValueArgs TARGET DESTINATION OVERWRITE_BEHAVIOR REMOVE_RPATH)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "mdt_copy_shared_libraries_target_depends_on(): no target provided")
  endif()
  if(NOT TARGET ${ARG_TARGET})
    message(FATAL_ERROR "mdt_copy_shared_libraries_target_depends_on(): ${ARG_TARGET} is not a valid target")
  endif()
  if(NOT ARG_DESTINATION)
    message(FATAL_ERROR "mdt_copy_shared_libraries_target_depends_on(): mandatory argument DESTINATION missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_copy_shared_libraries_target_depends_on(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  set(overwriteBehaviorOption fail)
  if(ARG_OVERWRITE_BEHAVIOR)
    if(${ARG_OVERWRITE_BEHAVIOR} STREQUAL "KEEP")
      set(overwriteBehaviorOption keep)
    elseif(${ARG_OVERWRITE_BEHAVIOR} STREQUAL "OVERWRITE")
      set(overwriteBehaviorOption overwrite)
    elseif(${ARG_OVERWRITE_BEHAVIOR} STREQUAL "FAIL")
      set(overwriteBehaviorOption fail)
    else()
      message(FATAL_ERROR "mdt_copy_shared_libraries_target_depends_on(): OVERWRITE_BEHAVIOR argument ${ARG_OVERWRITE_BEHAVIOR} is not valid."
                          "Possible values are KEEP, OVERWRITE or FAIL.")
    endif()
  endif()

  if(ARG_REMOVE_RPATH)
    set(removeRpathOptionArgument --remove-rpath)
  else()
    set(removeRpathOptionArgument)
  endif()

  if(TARGET mdtdeployutils)
    set(deployUtilsExecutable mdtdeployutils)
  else()
    set(deployUtilsExecutable Mdt0::DeployUtilsExecutable)
  endif()

  message("deployUtilsExecutable: ${deployUtilsExecutable}")

  add_custom_command(
    TARGET ${ARG_TARGET}
    POST_BUILD
    COMMAND ${deployUtilsExecutable} --logger-backend cmake copy-shared-libraries-target-depends-on
              --overwrite-behavior ${overwriteBehaviorOption}
              ${removeRpathOptionArgument}
              --search-prefix-path-list "${CMAKE_PREFIX_PATH}"
              $<TARGET_FILE:${ARG_TARGET}>
              "${ARG_DESTINATION}"
    VERBATIM
  )

endfunction()

# We need:
# - get the shared libraries the target depend on
# - install those shared libraries
# - update the RPATH of those shared libraries
# - support the DESTDIR environment variable, which is unknown at build system genration time
#
# A intuitive idea could be to define a install(FILES) rule
# which takes a list of shared libraries to install.
# This does not work, because install() is evaluated at build generation time,
# but the list of shared libraries are known at build time (after target has been build).
#
# A solution could be to copy the shared libraries to a known directory in the build tree.
# Also create a install(DIRECTORY) rule that installs this known directory.
# This works, but now the shared libraries are copied twice,
# and it is possible that some garbage may be installed (hidden files, etc..)
#
# A solution is to create a cmake script that is called at install time.
# This is supported by adding a install(SCRIPT) rule.
#
# In the script we have to consider limitations.
# For example, most of CMake variable are not available.
# To solve this, we can create a script.cmake.in,
# then use configure_file() to generate the final script with the variables we need.
# Other limitations to consider are available commands.
# For example, install(), get_target_property(), add_custom_command() are not available.
#
# Also, to get the path to a target file:
# - get_target_property(targetFile target LOCATION) is not supported (CMake throws a error)
# - $<TARGET_FILE:target> is not expanded (neither in this file, also not in the script)
# By looking CMake cmake-generator-expressions documentation again,
# a solution is to use file(GENERATE ...), then read the result in the script.
#
function(mdt_install_shared_libraries_target_depends_on)

# TODO provide MDT_DEPLOY_UTILS_EXECUTABLE etc.. + do checks - see mdt_deploy_application()

  set(options)
  set(oneValueArgs TARGET RUNTIME_DESTINATION LIBRARY_DESTINATION INSTALL_IS_UNIX_SYSTEM_WIDE COMPONENT)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "mdt_install_shared_libraries_target_depends_on(): no target provided")
  endif()
  if(NOT TARGET ${ARG_TARGET})
    message(FATAL_ERROR "mdt_install_shared_libraries_target_depends_on(): ${ARG_TARGET} is not a valid target")
  endif()
  if(NOT ARG_RUNTIME_DESTINATION)
    message(FATAL_ERROR "mdt_install_shared_libraries_target_depends_on(): mandatory argument RUNTIME_DESTINATION missing")
  endif()
  if(NOT ARG_LIBRARY_DESTINATION)
    message(FATAL_ERROR "mdt_install_shared_libraries_target_depends_on(): mandatory argument LIBRARY_DESTINATION missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_install_shared_libraries_target_depends_on(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  set(componentArguments)
  if(ARG_COMPONENT)
    set(componentArguments COMPONENT ${ARG_COMPONENT})
  endif()

  set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_INSTALL_IS_UNIX_SYSTEM_WIDE ${ARG_INSTALL_IS_UNIX_SYSTEM_WIDE})

  if(WIN32)
    set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_DESTINATION "${ARG_RUNTIME_DESTINATION}")
  else()
    set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_DESTINATION "${ARG_LIBRARY_DESTINATION}")
  endif()

  set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_ENV_PATH)
  mdt_target_libraries_to_library_env_path(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_ENV_PATH TARGET mdtdeployutils ALWAYS_USE_SLASHES)

  # configure_file() does not support generator expression
  # file(GENERATE) supports generator expression, but not @ expension
  # Result: the expression $<TARGET_FILE:${ARG_TARGET}> cannot be used in the input script,
  # we have to give a valid target.
  # So, to have both worlds, we have to generate a intermediate script

  set(intermediateInstallScript "${CMAKE_CURRENT_BINARY_DIR}/MdtInstallSharedLibrariesScript.cmake.intermediate")
  configure_file("${PROJECT_SOURCE_DIR}/cmake/Modules/MdtInstallSharedLibrariesScript.cmake.in" "${intermediateInstallScript}" @ONLY)

  set(installScript "${CMAKE_CURRENT_BINARY_DIR}/MdtInstallSharedLibrariesScript-$<CONFIG>.cmake")
  file(GENERATE
    OUTPUT "${installScript}"
    INPUT "${intermediateInstallScript}"
    TARGET ${ARG_TARGET}
  )

  # TODO considere COMPONENT
  install(SCRIPT "${installScript}" COMPONENT Runtime)

endfunction()
