# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# TODO: document, implement
# TODO: should go to MdtCMakeModules

#.rst:
# MdtInstallCMakeModules
# ----------------------
#
# .. contents:: Summary
#    :local:
#
#
# Functions
# ^^^^^^^^^
#
# .. command:: mdt_install_cmake_modules
#
# Install a set of CMake modules::
#
#   mdt_install_cmake_modules(
#     FILES files...
#     [DESTINATION <dir>]
#     EXPORT_NAME <export-name>
#     EXPORT_NAMESPACE <export-namespace>
#     [NO_PACKAGE_CONFIG_FILE]
#     [EXPORT_DIRECTORY <dir>]
#     [INSTALL_IS_UNIX_SYSTEM_WIDE [TRUE|FALSE]]
#     [COMPONENT <component-name>]
#     [MODULES_PATH_VARIABLE_NAME <variable-name>]
#   )
#
# Install the CMake modules designated by ``files`` using :command:`install(FILES)`.
#
# By default, the destination is relative to ``CMAKE_INSTALL_PREFIX`` and depends on ``INSTALL_IS_UNIX_SYSTEM_WIDE``:
# if it is ``TRUE``, it will be ``${CMAKE_INSTALL_DATADIR}/<package-name>/Modules``, otherwise ``cmake/Modules``.
# Here, ``<package-name>`` is ``${EXPORT_NAMESPACE}${EXPORT_NAME}``.
#
# Alternatively, it is possible to specify a directory (or a relative path) using the ``DESTINATION`` argument.
# In that case, the files will be installed to ``${DESTINATION}/cmake/Modules``,
# relative to ``CMAKE_INSTALL_PREFIX``.
# When using ``DESTINATION``, ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ignored.
#
# A CMake package file, named ``${EXPORT_NAMESPACE}${EXPORT_NAME}.cmake``, is generated.
# This file adds the location of the installed modules to ``CMAKE_MODULE_PATH``.
#
# If ``NO_PACKAGE_CONFIG_FILE`` is not set,
# a package config file, named ``${EXPORT_NAMESPACE}${EXPORT_NAME}Config.cmake``, is also generated.
#
# By default, the package config files are installed to a subdirectory named ``${EXPORT_NAMESPACE}${EXPORT_NAME}``,
# which correspond to a location :command:`find_package()` uses.
#
# If this default location does not match the required one,
# the ``EXPORT_DIRECTORY`` can be used.
# In that case, the package config file will be installed to a subdirectory named ``${EXPORT_NAMESPACE}${EXPORT_DIRECTORY}``.
#
# If ``MODULES_PATH_VARIABLE_NAME`` is defined,
# a variable named `<variable-name>` will be created in the generated CMake package file.
# The value of this variable will contain the path to the installed CMake modules
# in a relocatable way.
#
# Install modules that generate scripts from input scripts
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# For some cases, it is required to generate a script based on a input script.
#
# .. code-block:: cmake
#
#   set(inputScript ??)
#   configure_file("${inputScript}" "${outputScript}" @ONLY)
#
# In above example, `inputScript` should refer to the input script,
# for example `MyModuleScript.cmake.in`, once the module is installed.
#
# To locate it, a variable can be created, for example ``MY_INSTALLED_CMAKE_MODULES_PATH``.
#
# .. code-block:: cmake
#
#   set(inputScript "${MY_INSTALLED_CMAKE_MODULES_PATH}/MyModuleScript.cmake.in")
#   configure_file("${inputScript}" "${outputScript}" @ONLY)
#
# If the package, including above module, has to be relocatable,
# the variable ``MY_INSTALLED_CMAKE_MODULES_PATH`` should be created
# in the package config file.
#
#
# To create this ``MY_INSTALLED_CMAKE_MODULES_PATH`` variable,
# pass its name to the ``MODULES_PATH_VARIABLE_NAME`` argument:
#
# .. code-block:: cmake
#
#   mdt_install_cmake_modules(
#     FILES
#       Modules/MyModule.cmake
#       Modules/MyModuleScript.cmake.in
#     EXPORT_NAME CMakeModules
#     EXPORT_NAMESPACE MyLib
#     MODULES_PATH_VARIABLE_NAME MY_INSTALLED_CMAKE_MODULES_PATH
#   )
#
# See also
# https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html#command:configure_package_config_file
#
# Install a standalone CMake modules project
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# This example illustrates how to install the modules
# and the related CMake package files:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   set(MDT_INSTALL_PACKAGE_NAME Mdt0)
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   mdt_install_cmake_modules(
#     FILES
#       Modules/ModuleA.cmake
#       Modules/ModuleB.cmake
#     EXPORT_NAME CMakeModules
#     EXPORT_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#   )
#
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     |-cmake
#        |-Modules
#        |  |-ModuleA.cmake
#        |  |-ModuleB.cmake
#        |-Mdt0CMakeModules
#             |-Mdt0CMakeModules.cmake
#             |-Mdt0CMakeModulesConfig.cmake
#
#
# Example of a system wide install on a Debian MultiArch (``CMAKE_INSTALL_PREFIX=/usr``)::
#
#   /usr
#     |-share
#         |-Mdt0CMakeModules
#         |   |-Modules
#         |      |-ModuleA.cmake
#         |      |-ModuleB.cmake
#         |-cmake
#             |-Mdt0CMakeModules
#                 |-Mdt0CMakeModules.cmake
#                 |-Mdt0CMakeModulesConfig.cmake
#
#
# Once the project is installed,
# the user should be able to find it using CMake find_package() in its CMakeLists.txt:
#
# .. code-block:: cmake
#
#   find_package(Mdt0CMakeModules REQUIRED)
#
#   include(MdtBuildOptionsUtils)
#
#   mdt_set_available_build_types(Debug Release RelWithDebInfo MinSizeRel Instrumented)
#
#
# Install CMake modules part of a other project
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# Example to install CMake modules that are part of a project having other parts:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   set(MDT_INSTALL_PACKAGE_NAME Mdt0)
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   mdt_install_cmake_modules(
#     FILES
#       Modules/ModuleA.cmake
#       Modules/ModuleB.cmake
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     EXPORT_NAME DeployUtilsCMakeModules
#     EXPORT_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
#     NO_PACKAGE_CONFIG_FILE
#     EXPORT_DIRECTORY DeployUtils
#     COMPONENT ${PROJECT_NAME}_Runtime
#   )
#
#   my_project_write_package_config_file(...)
#
# In above example, ``my_project_write_package_config_file()``
# will write a package configuration file, named ``Mdt0DeployUtils.cmake``,
# that will include ``Mdt0DeployUtilsCMakeModules.cmake``.
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     lib
#      |-cmake
#         |-Modules
#         |  |-ModuleA.cmake
#         |  |-ModuleB.cmake
#         |-Mdt0DeployUtils
#              |-Mdt0DeployUtilsCMakeModules.cmake
#              |-Mdt0DeployUtilsConfig.cmake
#
#
# Example of a system wide install on a Debian MultiArch (``CMAKE_INSTALL_PREFIX=/usr``)::
#
#     /usr
#       |-lib
#          |-x86_64-linux-gnu
#             |-cmake
#                |-Modules
#                |  |-ModuleA.cmake
#                |  |-ModuleB.cmake
#                |-Mdt0DeployUtils
#                     |-Mdt0DeployUtilsCMakeModules.cmake
#                     |-Mdt0DeployUtilsConfig.cmake
#
#
# Once the project is installed,
# the user should be able to find it using CMake find_package() in its CMakeLists.txt:
#
# .. code-block:: cmake
#
#   find_package(Mdt0DeployUtils REQUIRED)
#
#   include(MdtDeployApplication)
#
#   add_executable(myApp myApp.cpp)
#   mdt_deploy_application(TARGET myApp ...)
#
#
# Install modules that requires tools
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# TODO: remove once described problems are solved
#
# Some modules can require tools, like ``ldd``, ``objdump`` or a custom tool.
#
# One solution could be to find those tools when the user calls ``find_package()``::
#
#   find_package(MyCMakeModules REQUIRED)
#   # Here, 2 targets are defined: ldd and myTool
#   # Those are imported targets
#
# This seems nice, but it can create name clashes.
# What is the consumer project also needs ``ldd``,
# and has previously defined it with ``find_program()`` ?
# Try to define aliases could be a solution to the name clashes.
# Another problem is that some tools are searched multiple times,
# which can be slower, but also could result to unexpected versions.
#
# A better solution is to be explicit::
#
#   find_package(MyCMakeModules REQUIRED)
#   find_program(ldd NAMES ldd)
#   find_program(myTool NAMES myTool)
#
#   my_modules_do_stuff(
#     ...
#     LDD_PATH ldd
#     MY_TOOL_PATH myTool
#   )
#
# Providing a helper function that finds required tools can also help::
#
#   find_package(MyCMakeModules REQUIRED)
#   my_modules_find_required_tools()
#   # On Linux, ldd and myTool are defined
#   # On Windows, objdump and myTool are defined
#
#   my_modules_do_stuff(
#     ...
#     LDD_PATH ldd
#     MY_TOOL_PATH myTool
#   )
#
#
#
# Subtitle
# ^^^^^^^^
#
# TODO: should go to MdtPackageConfigHelpers
#
#  - :command:`mdt_xxxx()`
#  - :command:`mdt_xxxxs()`
#
#
# .. command:: mdt_xxxx
#
# Get find_program() command to find a tool::
#
#   mdt_xxxx(
#     <VAR>
#     TOOL name
#   )
#
#
# Example:
#
# .. code-block:: cmake
#
#   mdt_xxxx(
#     findMyToolCommandString
#     TOOL myTool
#   )
#
# For this example, ``findMyToolCommandString``
# will contain a command like::
#
#   find_program(toolPath NAMES myTool)
#   if(NOT toolPath)
#     message(FATAL_ERROR "...")
#   endif()
#   add_executable(myTool IMPORTED)
#   set_target_properties(myTool PROPERTIES IMPORTED_LOCATION "${toolPath}")
#
#
# .. command:: mdt_xxxxs
#
# Get find_program() command to find a list of tools::
#
#   mdt_xxxxs(
#   )
#
#
# Example:
#
# .. code-block:: cmake
#
#   mdt_xxxxs(
#   )
#
#
# 
#
#


# include(CMakePackageConfigHelpers)

function(mdt_install_cmake_modules)

  set(options NO_PACKAGE_CONFIG_FILE)
  set(oneValueArgs DESTINATION EXPORT_NAME EXPORT_NAMESPACE EXPORT_DIRECTORY INSTALL_IS_UNIX_SYSTEM_WIDE COMPONENT)
  set(multiValueArgs FILES)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_FILES)
    message(FATAL_ERROR "mdt_install_cmake_modules(): at least 1 file to a module must be provided")
  endif()
  if(NOT ARG_EXPORT_NAME)
    message(FATAL_ERROR "mdt_install_cmake_modules(): EXPORT_NAME missing")
  endif()
  if(NOT ARG_EXPORT_NAMESPACE)
    message(FATAL_ERROR "mdt_install_cmake_modules(): EXPORT_NAMESPACE missing")
  endif()

  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_install_cmake_modules(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT CMAKE_INSTALL_DATADIR AND NOT ARG_DESTINATION)
    message(FATAL_ERROR "mdt_install_cmake_modules(): CMAKE_INSTALL_DATADIR is not defined. Please include GNUInstallDirs before calling this function or sepcify a destination with DESTINATION")
  endif()

  set(packageName "${ARG_EXPORT_NAMESPACE}${ARG_EXPORT_NAME}")

  if(ARG_EXPORT_DIRECTORY)
    set(packageConfigInstallDirName "${ARG_EXPORT_NAMESPACE}${ARG_EXPORT_DIRECTORY}")
  else()
    set(packageConfigInstallDirName "${packageName}")
  endif()

  if(ARG_DESTINATION)
    set(modulesInstallDir "${ARG_DESTINATION}/cmake/Modules")
    set(packageConfigInstallDir "${ARG_DESTINATION}/cmake/${packageConfigInstallDirName}")
  else()
    if(ARG_INSTALL_IS_UNIX_SYSTEM_WIDE)
      set(modulesInstallDir "${CMAKE_INSTALL_DATADIR}/${packageName}/Modules")
      set(packageConfigInstallDir "${CMAKE_INSTALL_DATADIR}/cmake/${packageConfigInstallDirName}")
    else()
      set(modulesInstallDir "cmake/Modules")
      set(packageConfigInstallDir "cmake/${packageConfigInstallDirName}")
    endif()
  endif()

  set(componentArguments)
  if(ARG_COMPONENT)
    set(componentArguments COMPONENT ${ARG_COMPONENT})
  endif()

  install(
    FILES ${ARG_FILES}
    DESTINATION "${modulesInstallDir}"
    ${componentArguments}
  )
  
  # TODO: maybe warn if user passes TOOLS and NO_PACKAGE_CONFIG_FILES
  # TODO: maybe allways generate package config file ?

  set(cmakePackageFileInContent "@PACKAGE_INIT@\n\n")
  string(APPEND cmakePackageFileInContent "set(MDT_CMAKE_MODULE_PATH \"@PACKAGE_modulesInstallDir@\")\n\n")
  string(APPEND cmakePackageFileInContent "# Add to CMAKE_MODULE_PATH if not allready\n")
  string(APPEND cmakePackageFileInContent "if(NOT \"\${MDT_CMAKE_MODULE_PATH}\" IN_LIST CMAKE_MODULE_PATH)\n")
  string(APPEND cmakePackageFileInContent "  list(APPEND CMAKE_MODULE_PATH \"\${MDT_CMAKE_MODULE_PATH}\")\n")
  string(APPEND cmakePackageFileInContent "endif()\n")
  
  string(APPEND cmakePackageFileInContent "set(MY_CMAKE_MODULES_PATH \"@PACKAGE_modulesInstallDir@\")\n\n")

  set(cmakePackageFileIn "${CMAKE_CURRENT_BINARY_DIR}/${packageName}.cmake.in")
  set(cmakePackageFile "${CMAKE_CURRENT_BINARY_DIR}/${packageName}.cmake")

  file(WRITE "${cmakePackageFileIn}" "${cmakePackageFileInContent}")

  configure_package_config_file(
    "${cmakePackageFileIn}"
    "${cmakePackageFile}"
    INSTALL_DESTINATION "${packageConfigInstallDir}"
    PATH_VARS modulesInstallDir
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

  install(
    FILES ${cmakePackageFile}
    DESTINATION "${packageConfigInstallDir}"
    ${componentArguments}
  )

  if(NOT ARG_NO_PACKAGE_CONFIG_FILE)

    set(cmakePackageConfigFileContent "include(\"\${CMAKE_CURRENT_LIST_DIR}/${packageName}.cmake\")\n")

    set(cmakePackageConfigFile "${CMAKE_CURRENT_BINARY_DIR}/${packageName}Config.cmake")

    file(WRITE "${cmakePackageConfigFile}" "${cmakePackageConfigFileContent}")

    install(
      FILES ${cmakePackageConfigFile}
      DESTINATION "${packageConfigInstallDir}"
      ${componentArguments}
    )

  endif()

endfunction()
