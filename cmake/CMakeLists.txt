
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

include(MdtInstallCMakeModules)

message("MDT_INSTALL_IS_UNIX_SYSTEM_WIDE: ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}")

mdt_install_cmake_modules(
  FILES
    Modules/MdtSharedLibrariesDepencyHelpers.cmake
    Modules/MdtFindDeployUtilsTools.cmake
  EXPORT_NAME DeployUtilsCMake
#   EXPORT_NAMESPACE Mdt0::
  INSTALL_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
  TOOLS mdtdeployutils
  LINUX_TOOLS ldd
  WINDOWS_TOOLS objdump
  APPLE_TOOLS otool
  COMPONENT Runtime
)

# set(MDT_DEPLOY_UTILS_CMAKE_MODULES_DIR "${MDT_INSTALL_DATADIR}/Modules")
# set(MDT_DEPLOY_UTILS_CMAKE_CONFIG_DIR "${MDT_INSTALL_DATADIR}/cmake")
# 
# message("MDT_INSTALL_DATADIR: ${MDT_INSTALL_DATADIR}")
# 
# install(
#   FILES
#     Modules/MdtSharedLibrariesDepencyHelpers.cmake
#   DESTINATION "${MDT_DEPLOY_UTILS_CMAKE_MODULES_DIR}"
# )
# 
# include(CMakePackageConfigHelpers)
# configure_package_config_file(
#   MdtDeployUtilsCMakeConfig.cmake.in
#   ${CMAKE_CURRENT_BINARY_DIR}/Mdt0DeployUtilsCMakeConfig.cmake
#   INSTALL_DESTINATION "${MDT_DEPLOY_UTILS_CMAKE_CONFIG_DIR}"
#   PATH_VARS MDT_DEPLOY_UTILS_CMAKE_CONFIG_DIR
# )
# 
# install(
#   FILES
#     "${CMAKE_CURRENT_BINARY_DIR}/Mdt0DeployUtilsCMakeConfig.cmake"
# #   "${CMAKE_CURRENT_BINARY_DIR}/MdtCMakeModulesVersion.cmake"
#   DESTINATION "${MDT_DEPLOY_UTILS_CMAKE_CONFIG_DIR}"
# )
