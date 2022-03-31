# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtGenerateMdtdeployutilsInstallScript
# --------------------------------------
#
# .. contents:: Summary
#    :local:
#
#
# .. command:: mdt_generate_mdtdeployutils_install_script
#
# Note that this is a low level function
# that is used by other high level function.
#
# See also :command:`mdt_deploy_application()`
# and :command:`mdt_install_shared_libraries_target_depends_on()`.
#
# Generate a script calling mdtdeployutils at install time::
#
#   mdt_generate_mdtdeployutils_install_script(
#     TARGET <target>
#     INPUT_SCRIPT_FILE <file-name>
#     [COMPONENT <name>]
#     [EXCLUDE_FROM_ALL]
#   )
#
# Pass the input script to the ``INPUT_SCRIPT_FILE`` argument.
# It must be the file name, like `MyScript.cmake.in`, not a full path.
# This way it will be located properly if this function is called
# once the modules are installed, as well as when working in the MdtDeployUtils source tree.
#
# The input script should use the `MdtDeployUtilsInstallScriptHelpers` module:
#
# .. code-block:: cmake
#
#   include(@MDT_DEPLOY_UTILS_INSTALL_SCRIPT_HELPERS@)
#
#
# The ``@MDT_DEPLOY_UTILS_INSTALL_SCRIPT_HELPERS@`` will be expanded
# to the full path to the `MdtDeployUtilsInstallScriptHelpers` module.
#
# This helper moudle provides some variables.
#
# ``MDT_INSTALL_PREFIX_WITH_DESTDIR`` is ``CMAKE_INSTALL_PREFIX``
# prefixed with the ``DESTDIR`` ENV variable, if it is set.
# Double slashes are removed.
# On Windows, drive letters are removed from ``CMAKE_INSTALL_PREFIX`` if present.
#
# ``@MDT_DEPLOY_UTILS_INSTALL_SCRIPT_MDTDEPLOYUTILS_EXECUTABLE@``
# gives the full path to the mdtdeployutils executable.
#
# ``@MDT_DEPLOY_UTILS_INSTALL_SCRIPT_MDTDEPLOYUTILS_RUNTIME_ENV@``
# gives the runtime environment.
# Will only be expanded when mdtdeployutils is called from the MdtDeployUtils build tree.
#
#
# The helper module also provides the :command:`execute_mdtdeployutils()` function::
#
#   execute_mdtdeployutils(
#     MDTDEPLOYUTILS_EXECUTABLE <path>
#     RUNTIME_ENV <env>
#     ARGUMENTS <arguments>
#   )
#
#
# Technical details
# ^^^^^^^^^^^^^^^^^
#
# Most of the time this function would be called from the install tree,
# but sometimes from the MdtDeployUtils source tree.
#
# Input install script
# ====================
#
# One problem is to find the input script.
#
# This could be solved by using find_file():
#
# .. code-block:: cmake
#
#   find_file(inInstallScript
#      NAMES MdtDeployUtilsInstallScript.cmake.in
#      PATHS ${CMAKE_MODULE_PATH} "${MdtDeployUtils_SOURCE_DIR}/cmake/Modules"
#      NO_DEFAULT_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH
#   )
#
# This works, but its not ideal:
# - could potentially be slow
# - could find a other file than the expected one
#
# Note: while reading the CMake documentation,
# ``CMAKE_CURRENT_LIST_DIR`` will not work here.
#
# The solution is to create a variable in the package config file
# for the deployed CMake modules:
# ``MDT_DEPLOY_UTILS_INSTALLED_CMAKE_MODULES_PATH``.
#
# mdtdeployutils target
# =====================
#
# A other problem is to use the correct mdtdeployutils target:
# - Mdt0::DeployUtilsExecutable when called from a user
# - mdtdeployutils when called from the MdtDeployUtils source tree.
#
# This can be done in a simple way here.
#
# A note about file(GENERATE)
# ===========================
#
# I thought that the mdtdeployutils target generator expression
# would have to be evaluated in the generated script.
#
# So, in the input script, I have try:
#
# .. code-block:: cmake
#
#  if(TARGET Mdt0::DeployUtilsExecutable)
#    set(deployUtilsExecutable "$<TARGET_FILE:Mdt0::DeployUtilsExecutable>")
#  else()
#    # We are in the MdtDeployUtils build tree
#    set(deployUtilsExecutable "$<TARGET_FILE:mdtdeployutils>")
#  endif()
#
# This allways gave errors, because one of the targets did not exists.
#
# So, let's try a complete genex:
#
# .. code-block:: cmake
#
#  set(deployUtilsExecutable $<IF:$<TARGET_EXISTS:Mdt0::DeployUtilsExecutable>,$<TARGET_FILE:Mdt0::DeployUtilsExecutable>,$<TARGET_FILE:mdtdeployutils>>
#
# This gave the same error.
#
# There is more:
# even if the code is commented,
# CMake also gave those errors (!)
#
# Maybe this is related to this issue:
# https://gitlab.kitware.com/cmake/cmake/-/issues/20287
#
function(mdt_generate_mdtdeployutils_install_script)

  set(options EXCLUDE_FROM_ALL)
  set(oneValueArgs TARGET INPUT_SCRIPT_FILE COMPONENT)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "mdt_generate_mdtdeployutils_install_script(): no target provided")
  endif()
  if(NOT TARGET ${ARG_TARGET})
    message(FATAL_ERROR "mdt_generate_mdtdeployutils_install_script(): ${ARG_TARGET} is not a valid target")
  endif()
  if(NOT ARG_INPUT_SCRIPT_FILE)
    message(FATAL_ERROR "mdt_generate_mdtdeployutils_install_script(): mandatory argument INPUT_SCRIPT_FILE missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_generate_mdtdeployutils_install_script(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  set(componentArguments)
  if(ARG_COMPONENT)
    set(componentArguments COMPONENT ${ARG_COMPONENT})
  endif()

  # configure_file() does not support generator expression
  # file(GENERATE) supports generator expression, but not @ expension
  # Result: the expression $<TARGET_FILE:${ARG_TARGET}> cannot be used in the input script,
  # we have to give a valid target.
  # So, to have both worlds, we have to generate a intermediate script

  message(DEBUG "MdtDeployUtils_SOURCE_DIR: ${MdtDeployUtils_SOURCE_DIR}")
  message(DEBUG "MDT_DEPLOY_UTILS_INSTALLED_CMAKE_MODULES_PATH: ${MDT_DEPLOY_UTILS_INSTALLED_CMAKE_MODULES_PATH}")

  if(MDT_DEPLOY_UTILS_INSTALLED_CMAKE_MODULES_PATH)
    set(inInstallScript "${MDT_DEPLOY_UTILS_INSTALLED_CMAKE_MODULES_PATH}/${ARG_INPUT_SCRIPT_FILE}")
    set(MDT_DEPLOY_UTILS_INSTALL_SCRIPT_HELPERS "${MDT_DEPLOY_UTILS_INSTALLED_CMAKE_MODULES_PATH}/MdtDeployUtilsInstallScriptHelpers.cmake")
  else()
    set(inInstallScript "${MdtDeployUtils_SOURCE_DIR}/cmake/Modules/${ARG_INPUT_SCRIPT_FILE}")
    set(MDT_DEPLOY_UTILS_INSTALL_SCRIPT_HELPERS "${MdtDeployUtils_SOURCE_DIR}/cmake/Modules/MdtDeployUtilsInstallScriptHelpers.cmake")
  endif()

  message(DEBUG "inInstallScript: ${inInstallScript}")

  set(intermediateInstallScript "${CMAKE_CURRENT_BINARY_DIR}/MdtDeployApplicationInstallScript-${ARG_TARGET}.cmake.intermediate")

  message(DEBUG "intermediateInstallScript: ${intermediateInstallScript}")

  if(TARGET Mdt0::DeployUtilsExecutable)
    message(DEBUG "mdtdeployutils is installed")
    set(MDT_DEPLOY_UTILS_INSTALL_SCRIPT_MDTDEPLOYUTILS_EXECUTABLE "$<TARGET_FILE:Mdt0::DeployUtilsExecutable>")
  else()
    message(DEBUG "mdtdeployutils is in the MdtDeployUtils build tree")
    include(MdtRuntimeEnvironment)
    set(MDT_DEPLOY_UTILS_INSTALL_SCRIPT_MDTDEPLOYUTILS_EXECUTABLE "$<TARGET_FILE:mdtdeployutils>")
    mdt_target_libraries_to_library_env_path(MDT_DEPLOY_UTILS_INSTALL_SCRIPT_MDTDEPLOYUTILS_RUNTIME_ENV TARGET mdtdeployutils ALWAYS_USE_SLASHES)
  endif()

  configure_file("${inInstallScript}" "${intermediateInstallScript}" @ONLY)

  set(installScript "${CMAKE_CURRENT_BINARY_DIR}/MdtDeployApplicationInstallScript-$<TARGET_FILE_BASE_NAME:${ARG_TARGET}>-$<CONFIG>.cmake")
  file(GENERATE
    OUTPUT "${installScript}"
    INPUT "${intermediateInstallScript}"
    TARGET ${ARG_TARGET}
  )

  install(SCRIPT "${installScript}" ${componentArguments} ${excludeFromAllArgument})

endfunction()
