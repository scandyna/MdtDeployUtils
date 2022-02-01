# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(MdtDeployApplication)

######################################################
# Deploy and run a QtWidgets based application
######################################################

mdt_deploy_application(
  TARGET qtwidgetsapp
  RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
)

add_test(NAME System_DeployUtils_CMake_Deploy_QtWidgetsApp
  COMMAND "${CMAKE_COMMAND}"
    --install "${CMAKE_CURRENT_BINARY_DIR}"
    --config $<CONFIG>
)

set_tests_properties(System_DeployUtils_CMake_Deploy_QtWidgetsApp
  PROPERTIES
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)


add_test(NAME System_DeployUtils_CMake_Run_QtWidgetsApp
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/bin/qtwidgetsapp"
)

set_tests_properties(System_DeployUtils_CMake_Run_QtWidgetsApp
  PROPERTIES
    DEPENDS "System_DeployUtils_CMake_Deploy_QtWidgetsApp"
)

# On UNIX system wide install the RPATH are removed.
# In that case, running a executable will only work
# on a real system wide installation
if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set_tests_properties(System_DeployUtils_CMake_Run_QtWidgetsApp PROPERTIES DISABLED YES)
endif()
