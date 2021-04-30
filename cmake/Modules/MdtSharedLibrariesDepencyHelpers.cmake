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
# On Linux, ``ldd`` is internally used to find the shared libraries,
# which should find dependencies using rpath informations and other mechanisms.
# On some cases, some shared libraries cannot be found.
# For more informations about why,
# see https://scandyna.gitlab.io/mdt-cmake-modules/Modules/MdtRuntimeEnvironment.html .
# To solve such case, ``CMAKE_PREFIX_PATH`` will be used to help ``ldd``.
#
# On Windows, there is no rpath informations,
# so ``CMAKE_PREFIX_PATH`` will be used to find dependencies.
#
# Example:
#
# .. code-block:: cmake
#
#   add_executable(myApp myApp.cpp)
#   target_link_libraries(myApp PRIVATE Qt5::Core)
#   target_link_libraries(myApp PRIVATE Mdt0::PlainText)
#
#   mdt_copy_shared_libraries_target_depends_on(
#     TARGET myApp
#     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/myApp/lib"
#     OVERWRITE_BEHAVIOR OVERWRITE
#   )
#
# The shared libraries the ``myApp`` executable depends on are copied to ``"${CMAKE_CURRENT_BINARY_DIR}/myApp/lib"``.
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
# and ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``, a fatal error is thrown.
# The reason is to prevent erasing system wide installed libraries with a other version,
# which could corrupt the whole system.
# On the other hand, erasing libraries while creating a standalone application
# will happen often, and is not a problem.
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

# TODO provide MDT_DEPLOY_UTILS_EXECUTABLE etc.. + do checks - see mdt_deploy_applicastion()

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

  add_custom_command(
    TARGET ${ARG_TARGET}
    POST_BUILD
    COMMAND mdtdeployutils copy-shared-libraries-target-depends-on
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
# Also, to get the path to the target file:
# - get_target_property(targetFile target LOCATION) is not supported (CMake throws a error)
# - $<TARGET_FILE:target> is not expanded (neither in this file, also not in the script)
#
# As result, we have to split the work:
# - get the list of shared libraries target depends on here (we have all required tools)
# - install them from the script
# In the script, file(INSTALL) is available, and seems to be a good solution.
#
# An other problem to solve is to exchange the list of shared libraries.
# add_custom_command() does not support getting results out.
# The solution is to write a result file,
# which is easy to read from the script using file(STRINGS).
# TODO: RPATH with file(INSTALL) ?
function(mdt_install_shared_libraries_target_depends_on)

# TODO provide MDT_DEPLOY_UTILS_EXECUTABLE etc.. + do checks - see mdt_deploy_applicastion()

  # TODO: current solution is not relocatable with the DESTDIR !
  #  see install(SCRIPT) or install(CODE)
  #  Maybe: mdtdeployutils should generate a install script ??
  #  See: https://stackoverflow.com/questions/41254902/cmake-run-script-for-install-target

  # TODO: generate a install script using CMake configure_file(),
  # see: https://stackoverflow.com/questions/20792802/why-variables-are-not-accessed-inside-script-in-cmake
  # NOTE: what about generator expression $<TARGET_FILE:tgt> ?
  
  # TODO TODO first: maybe make it as done in MdtDeplyUtils !
  # (copy shlibs to a location, then install(FILES) or install(DIRECTORY)
  # Then document the choice
  # (i.e. each attempt have a blocker !!
  # f.ex: expand $<TARGET_FILE:tgt>, LOCATION property not works, getting output not possible with add_custom_command(), etc..  )

  # Should NOT install(DIRECTORY), because could have libraries that are no longer needed.
  # Try to generate a simple result file with a list a.so;b.so;...
  # The read this file into a list (a simple file(READ) should do the work easaly !)
  # Then, install(SCRIPT) to copy those files to the destination DESTDIR/CMAKE_INSTALL_PREFIX/xyz
  # NOTE: install(FILES) or install(PROGRAMS) frome here Nok, because evaluated at generation time !
  # See file(<COPY|INSTALL> <files>... DESTINATION <dir> ...)

  # TODO TODO for RPATH:
  # - The target must not be touched here, let the RPATH as is
  # - For dependencies, update all after install (in the script)

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

#   message("DESTDIR: $ENV{DESTDIR}")
  
  if(WIN32)
    get_filename_component(librariesDestination "${CMAKE_INSTALL_PREFIX}/${ARG_RUNTIME_DESTINATION}" ABSOLUTE)
  else()
    get_filename_component(librariesDestination "${CMAKE_INSTALL_PREFIX}/${ARG_LIBRARY_DESTINATION}" ABSOLUTE)
  endif()
  message("librariesDestination: ${librariesDestination}")

  if(ARG_INSTALL_IS_UNIX_SYSTEM_WIDE)
    set(overwriteBehavior FAIL)
  else()
    set(overwriteBehavior OVERWRITE)
  endif()

  mdt_copy_shared_libraries_target_depends_on(
    TARGET ${ARG_TARGET}
    DESTINATION "${librariesDestination}"
    OVERWRITE_BEHAVIOR ${overwriteBehavior}
    REMOVE_RPATH ${ARG_INSTALL_IS_UNIX_SYSTEM_WIDE}
  )
  
#   set(TARGET_TO_INSTALL ${ARG_TARGET})
#   set(TARGET_FILE_TO_INSTALL $<TARGET_FILE:${ARG_TARGET}>)
  
  set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_INSTALL_IS_UNIX_SYSTEM_WIDE ${ARG_INSTALL_IS_UNIX_SYSTEM_WIDE})
  
  if(WIN32)
    set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_DESTINATION "${ARG_RUNTIME_DESTINATION}")
  else()
    set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_DESTINATION "${ARG_LIBRARY_DESTINATION}")
  endif()

  set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_MDT_DEPLOY_UTILS_COMMAND_FILE "${CMAKE_CURRENT_BINARY_DIR}/MdtDeployUtilsCommand.txt")
  file(GENERATE
    OUTPUT "${MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_MDT_DEPLOY_UTILS_COMMAND_FILE}"
    CONTENT "$<TARGET_FILE:mdtdeployutils>"
  )

  set(MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_TARGET_FILE  "${CMAKE_CURRENT_BINARY_DIR}/MdtDeployUtilsTarget.txt")
  file(GENERATE
    OUTPUT "${MDT_INSTALL_SHARED_LIBRARIES_SCRIPT_TARGET_FILE}"
    CONTENT "$<TARGET_FILE:${ARG_TARGET}>"
  )

#   get_target_property(targetFile ${TARGET_TO_INSTALL} LOCATION)
#   message("targetFile: ${targetFile}")

  set(installScript "${CMAKE_CURRENT_BINARY_DIR}/MdtInstallSharedLibrariesScript.cmake")
  configure_file("${PROJECT_SOURCE_DIR}/cmake/Modules/MdtInstallSharedLibrariesScript.cmake.in" "${installScript}" @ONLY)
  
  install(SCRIPT "${installScript}" COMPONENT Runtime)

  # NOK, because evaluated at generation time !
#   file(READ "${CMAKE_BINARY_DIR}/filesToInstall.txt" librariesToInstall)
#   message("librariesToInstall: ${librariesToInstall}")
#   install(FILES ${librariesToInstall} DESTINATION lib)
#   add_custom_command(
#     TARGET ${ARG_TARGET}
#     POST_BUILD
#     COMMAND mdtdeployutils get-shared-libraries-target-depends-on --help
#   )

endfunction()
