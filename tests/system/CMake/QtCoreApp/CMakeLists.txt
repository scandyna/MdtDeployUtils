# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(MdtDeployApplication)

######################################################
# Deploy and run a QtCore based application
######################################################

mdt_deploy_application(
  TARGET qtcoreapp
  RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
  RUNTIME_COMPONENT SystemTest_QtCoreApp
  EXCLUDE_FROM_ALL
)

add_test(NAME System_DeployUtils_CMake_Deploy_QtCoreApp
  COMMAND "${CMAKE_COMMAND}"
    --install "${CMAKE_CURRENT_BINARY_DIR}"
    --config $<CONFIG>
    --component SystemTest_QtCoreApp
)

set_tests_properties(System_DeployUtils_CMake_Deploy_QtCoreApp
  PROPERTIES
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)


add_test(NAME System_DeployUtils_CMake_Run_QtCoreApp
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${MDT_SYSTEM_TESTS_INSTALL_PREFIX}/bin/qtcoreapp"
)

set_tests_properties(System_DeployUtils_CMake_Run_QtCoreApp
  PROPERTIES
    DEPENDS "System_DeployUtils_CMake_Deploy_QtCoreApp"
)

# On UNIX system wide install the RPATH are removed.
# In that case, running a executable will only work
# on a real system wide installation
if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set_tests_properties(System_DeployUtils_CMake_Run_QtCoreApp PROPERTIES DISABLED YES)
endif()
