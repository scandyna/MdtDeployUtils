# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

include(MdtInstallCMakeModules)

mdt_install_cmake_modules(
  FILES
    Modules/MdtSharedLibrariesDepencyHelpers.cmake
    Modules/MdtDeployApplication.cmake
    Modules/MdtDeployApplicationInstallScript.cmake.in
    Modules/MdtDeployUtilsInstallScriptHelpers.cmake
    Modules/MdtDeployUtilsPackageConfigHelpers.cmake
    Modules/MdtGenerateMdtdeployutilsInstallScript.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  EXPORT_NAME DeployUtilsCMakeModules
  EXPORT_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
  NO_PACKAGE_CONFIG_FILE
  EXPORT_DIRECTORY DeployUtils
  COMPONENT ${PROJECT_NAME}_Runtime
  MODULES_PATH_VARIABLE_NAME MDT_DEPLOY_UTILS_INSTALLED_CMAKE_MODULES_PATH
)
