# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Here we build the libraries that will be used for some (end-to-end-) tests.
#
# We must be able to install them in a way that is completly independent of the main project.
# To do this, each library is its own project, for which cmake is called to build them.
# This also avoid target name clashes,
# and also avoids to wory about installing test libraries accidentally in the project release.
#
# For each test, we must be able to provide the dependencies,
# like CMAKE_PREFIX_PATH, which is separate from the main project.
# To accomplish this, each test is also a seperated project.
# As effect, this way we can also test things in a more realistic way.

############################
# MdtDeployUtils
############################

# TODO: update
# Avoid rebuilding the whole project.
# Also, avoid to change the CMAKE_INSTALL_PREFIX of the main project.
#
# For some tests that are allways built,
# we can use the DESTDIR environment variable to relocate the installation.
# Note: this also checks that DESTDIR is used properly in some of the scripts.
# This also works on Windows, also with MSVC,
# but CMake do not recomment it, because CMAKE_INSTALL_PREFIX may start with a drive letter,
# like C:/Program Files, which cannot be prepended with some other prefix.
# See: https://cmake.org/cmake/help/latest/envvar/DESTDIR.html#envvar:DESTDIR
#
# Despite CMake documentation tells that the DESTDIR approach should not work on Windows,
# tests showed that install() can handle it anyway (also with MSVC VS generator),
# at least for recent CMake, like 3.21.
#
# At this point, some scripts like MdtDeployUtilsInstallScriptHelpers.cmake
# have also to handle that case.
#
# So, use the DESTDIR mechanism "blindly" for the below tests,
# which can check MdtDeployUtils CMake modules.
#

if(WIN32)
  string(REGEX REPLACE "^[a-zA-Z]:" ""
    testMdtDeployUtilsInstallPrefix
    "${CMAKE_INSTALL_PREFIX}"
  )
else()
  set(testMdtDeployUtilsInstallPrefix "${CMAKE_INSTALL_PREFIX}")
endif()

# TODO: component install (currently headers etc.. are installed)
add_test(NAME EndToEndTests_Install_DeployUtils_Destdir
  COMMAND "${CMAKE_COMMAND}"
    --install "${CMAKE_BINARY_DIR}"
    --config $<CONFIG>
    --component MdtDeployUtils_Runtime
#     --component ${PROJECT_NAME}_Dev
)

# add_test(NAME EndToEndTests_Install_DeployUtils_Destdir
#   COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install --config $<CONFIG>
# #   COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --component Runtime
# )

set_tests_properties(EndToEndTests_Install_DeployUtils_Destdir
  PROPERTIES
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)

if(BUILD_REAL_INSTALL_TESTS)
  # TODO: maybe use cmake --install as above
  # TODO: component install (currently headers etc.. are installed)
  add_test(NAME EndToEndTests_Install_DeployUtils_Real
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install --config $<CONFIG>
  #   COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --component Runtime
  )
endif()

############################
# ItemModel library
############################

if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set(testItemModelInstallPrefix "/usr")
else()
  set(testItemModelInstallPrefix "/opt/ItemModel")
endif()

add_test(NAME EndToEndTests_Build_ItemModel_lib
  COMMAND "${CMAKE_CTEST_COMMAND}"
    --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/libs/ItemModel" "${CMAKE_CURRENT_BINARY_DIR}/build/libs/ItemModel"
    --build-generator "${CMAKE_GENERATOR}"
    --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
    --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
    --build-config $<CONFIG>
    --build-options
      "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
      "-DCMAKE_INSTALL_PREFIX=${testItemModelInstallPrefix}"
      "-DBUILD_SHARED_LIBS=ON"
)

add_test(NAME EndToEndTests_Install_ItemModel_lib_Destdir
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/build/libs/ItemModel" --target install --config $<CONFIG>
)

set_tests_properties(EndToEndTests_Install_ItemModel_lib_Destdir
  PROPERTIES
    DEPENDS EndToEndTests_Build_ItemModel_lib
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)

if(BUILD_REAL_INSTALL_TESTS)

  add_test(NAME EndToEndTests_Install_ItemModel_lib_Real
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/build/libs/ItemModel" --target install --config $<CONFIG>
  )

  set_tests_properties(EndToEndTests_Install_ItemModel_lib_Real
    PROPERTIES
      DEPENDS EndToEndTests_Build_ItemModel_lib
  )

endif()

############################
# ItemEditor library
############################

if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set(testItemEditorInstallPrefix "/usr")
else()
  set(testItemEditorInstallPrefix "/opt/ItemEditor")
endif()

add_test(NAME EndToEndTests_Build_ItemEditor_lib
  COMMAND "${CMAKE_CTEST_COMMAND}"
    --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/libs/ItemEditor" "${CMAKE_CURRENT_BINARY_DIR}/build/libs/ItemEditor"
    --build-generator "${CMAKE_GENERATOR}"
    --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
    --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
    --build-config $<CONFIG>
    --build-options
      "-DCMAKE_PREFIX_PATH=${testItemModelInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH}"
      "-DCMAKE_INSTALL_PREFIX=${testItemEditorInstallPrefix}"
      "-DBUILD_SHARED_LIBS=ON"
)

add_test(NAME EndToEndTests_Install_ItemEditor_lib_Destdir
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/build/libs/ItemEditor" --target install --config $<CONFIG>
)

set_tests_properties(EndToEndTests_Install_ItemEditor_lib_Destdir
  PROPERTIES
    DEPENDS EndToEndTests_Build_ItemEditor_lib
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)

if(BUILD_REAL_INSTALL_TESTS)

  add_test(NAME EndToEndTests_Install_ItemEditor_lib_Real
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/build/libs/ItemEditor" --target install --config $<CONFIG>
  )

  set_tests_properties(EndToEndTests_Install_ItemEditor_lib_Real
    PROPERTIES
      DEPENDS EndToEndTests_Build_ItemEditor_lib
  )

endif()

##################################################
# CopySharedLibrariesTargetDendsOnTest test
#
# NOTE: this test is not a install test
# (but it uses installed MdtDeployUtils)
##################################################

add_test(NAME CMake_CopySharedLibrariesTargetDendsOnTest
  COMMAND "${CMAKE_CTEST_COMMAND}"
    --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/CopySharedLibrariesTargetDendsOnTest" "${CMAKE_CURRENT_BINARY_DIR}/copySharedLibrariesTargetDendsOnTest"
    --build-generator "${CMAKE_GENERATOR}"
    --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
    --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
    --build-config $<CONFIG>
    --build-options
      "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}${testMdtDeployUtilsInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH}"
    --test-command "${CMAKE_CTEST_COMMAND}"
)

set_tests_properties(CMake_CopySharedLibrariesTargetDendsOnTest
  PROPERTIES
    DEPENDS "EndToEndTests_Install_ItemModel_lib_Destdir;EndToEndTests_Install_DeployUtils_Destdir"
)

# On UNIX system wide install the RPATH are removed.
# In that case, running a executable will only work
# on a real system wide installation
if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set_tests_properties(CMake_CopySharedLibrariesTargetDendsOnTest PROPERTIES DISABLED YES)
endif()

if(BUILD_REAL_INSTALL_TESTS)

  add_test(NAME CMake_CopySharedLibrariesTargetDendsOnTest_Real
    COMMAND "${CMAKE_CTEST_COMMAND}"
      --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/CopySharedLibrariesTargetDendsOnTest" "${CMAKE_CURRENT_BINARY_DIR}/copySharedLibrariesTargetDendsOnTest"
      --build-generator "${CMAKE_GENERATOR}"
      --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
      --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
      --build-config $<CONFIG>
      --build-options
        "-DCMAKE_PREFIX_PATH=${testMdtDeployUtilsInstallPrefix};${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH}"
      --test-command "${CMAKE_CTEST_COMMAND}"
  )

  set_tests_properties(CMake_CopySharedLibrariesTargetDendsOnTest_Real
    PROPERTIES
      DEPENDS "EndToEndTests_Install_ItemModel_lib_Real;EndToEndTests_Install_DeployUtils_Real"
  )

endif()

# add_test(NAME Install_CopySharedLibrariesTargetDendsOnTest
#   COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/copySharedLibrariesTargetDendsOnTest" --target install --config $<CONFIG>
# )
#
# set_tests_properties(Install_CopySharedLibrariesTargetDendsOnTest
#   PROPERTIES
#     DEPENDS Build_CopySharedLibrariesTargetDendsOnTest
# )
#
# add_test(NAME Run_CopySharedLibrariesTargetDendsOnTest
#   COMMAND "${CMAKE_CURRENT_BINARY_DIR}/opt/testApp/bin/testApp${CMAKE_EXECUTABLE_SUFFIX}"
# )
#
# set_tests_properties(Run_CopySharedLibrariesTargetDendsOnTest
#   PROPERTIES
#     DEPENDS Install_CopySharedLibrariesTargetDendsOnTest
# )

##################################################
# InstallSharedLibrariesTargetDependsOnTest test
##################################################

if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set(installSharedLibrariesTargetDependsOnTestInstallPrefix "${CMAKE_INSTALL_PREFIX}")
else()
  set(installSharedLibrariesTargetDependsOnTestInstallPrefix "/opt/installSharedLibrariesTargetDependsOnTest")
endif()


add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Configure
  COMMAND "${CMAKE_COMMAND}"
    -S "${CMAKE_CURRENT_SOURCE_DIR}/InstallSharedLibrariesTargetDependsOnTest"
    -B "${CMAKE_CURRENT_BINARY_DIR}/installSharedLibrariesTargetDependsOnTest"
    -G "${CMAKE_GENERATOR}"
    -A "${CMAKE_GENERATOR_PLATFORM}"
    -T "${CMAKE_GENERATOR_TOOLSET}"
    -DCMAKE_MESSAGE_LOG_LEVEL=DEBUG
    "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}${testMdtDeployUtilsInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX=${installSharedLibrariesTargetDependsOnTestInstallPrefix}"
)

set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Configure
  PROPERTIES
    DEPENDS "EndToEndTests_Install_ItemModel_lib_Destdir;EndToEndTests_Install_DeployUtils_Destdir"
)

add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Build
  COMMAND "${CMAKE_COMMAND}"
    --build "${CMAKE_CURRENT_BINARY_DIR}/installSharedLibrariesTargetDependsOnTest"
    --config $<CONFIG>
)

set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Build
  PROPERTIES
    DEPENDS "CMake_InstallSharedLibrariesTargetDependsOnTest_Configure"
)

add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Destdir
  COMMAND "${CMAKE_COMMAND}"
    --install "${CMAKE_CURRENT_BINARY_DIR}/installSharedLibrariesTargetDependsOnTest"
    --config $<CONFIG>
    --verbose
)

set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Destdir
  PROPERTIES
    DEPENDS "CMake_InstallSharedLibrariesTargetDependsOnTest_Build"
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp1_Destdir
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}${installSharedLibrariesTargetDependsOnTestInstallPrefix}/testApp1/bin/testApp1"
)

set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp1_Destdir
  PROPERTIES
    DEPENDS "CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Destdir"
)

add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp2_Destdir
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}${installSharedLibrariesTargetDependsOnTestInstallPrefix}/testApp2/bin/testApp2"
)

set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp2_Destdir
  PROPERTIES
    DEPENDS "CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Destdir"
)

if(BUILD_REAL_INSTALL_TESTS)

  add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Real
    COMMAND "${CMAKE_COMMAND}"
      --install "${CMAKE_CURRENT_BINARY_DIR}/installSharedLibrariesTargetDependsOnTest"
      --config $<CONFIG>
      --verbose
  )

  set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Real
    PROPERTIES
      DEPENDS "CMake_InstallSharedLibrariesTargetDependsOnTest_Build"
  )

  add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp1_Real
    COMMAND "${installSharedLibrariesTargetDependsOnTestInstallPrefix}/testApp1/bin/testApp1"
  )

  set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp1_Real
    PROPERTIES
      DEPENDS "CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Real"
  )

  add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp2_Real
    COMMAND "${installSharedLibrariesTargetDependsOnTestInstallPrefix}/testApp2/bin/testApp2"
  )

  set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp2_Real
    PROPERTIES
      DEPENDS "CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Real"
  )

endif()

# add_test(NAME CMake_InstallSharedLibrariesTargetDependsOnTest_Run
#   COMMAND "${CMAKE_CTEST_COMMAND}"
#     -V
#   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/installSharedLibrariesTargetDependsOnTest"
# )
#
# set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Run
#   PROPERTIES
#     DEPENDS "CMake_InstallSharedLibrariesTargetDependsOnTest_Install"
# )

# On UNIX system wide install the RPATH are removed.
# In that case, running a executable will only work
# on a real system wide installation
# This also includes running mdtdeployutils once installed
if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Install_Destdir PROPERTIES DISABLED YES)
  set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp1_Destdir PROPERTIES DISABLED YES)
  set_tests_properties(CMake_InstallSharedLibrariesTargetDependsOnTest_Run_TestApp2_Destdir PROPERTIES DISABLED YES)
endif()


##################################################
# DeployApplicationTest test
##################################################

if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set(deployApplicationTestInstallPrefix "${CMAKE_INSTALL_PREFIX}")
else()
  set(deployApplicationTestInstallPrefix "/opt/deployApplicationTest")
endif()

# set(deployApplicationTestInstallPrefix "${CMAKE_CURRENT_BINARY_DIR}/deployApplicationTest/deployed")

# add_test(NAME CMake_DeployApplicationTest_Build
#   COMMAND "${CMAKE_CTEST_COMMAND}"
#     --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/DeployApplicationTest" "${CMAKE_CURRENT_BINARY_DIR}/deployApplicationTest"
#     --build-generator "${CMAKE_GENERATOR}"
#     --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
#     --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
#     --build-config $<CONFIG>
#     --build-options
#       "-DCMAKE_MESSAGE_LOG_LEVEL=DEBUG -DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}${testMdtDeployUtilsInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH}"
# #     --test-command "${CMAKE_CTEST_COMMAND}"
# )

add_test(NAME CMake_DeployApplicationTest_Configure
  COMMAND "${CMAKE_COMMAND}"
    -S "${CMAKE_CURRENT_SOURCE_DIR}/DeployApplicationTest"
    -B "${CMAKE_CURRENT_BINARY_DIR}/deployApplicationTest"
    -G "${CMAKE_GENERATOR}"
    -A "${CMAKE_GENERATOR_PLATFORM}"
    -T "${CMAKE_GENERATOR_TOOLSET}"
    -DCMAKE_MESSAGE_LOG_LEVEL=DEBUG
    "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}${testMdtDeployUtilsInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX=${deployApplicationTestInstallPrefix}"
)

set_tests_properties(CMake_DeployApplicationTest_Configure
  PROPERTIES
    DEPENDS "EndToEndTests_Install_ItemModel_lib_Destdir;EndToEndTests_Install_DeployUtils_Destdir"
)

add_test(NAME CMake_DeployApplicationTest_Build
  COMMAND "${CMAKE_COMMAND}"
    --build "${CMAKE_CURRENT_BINARY_DIR}/deployApplicationTest"
    --config $<CONFIG>
)

set_tests_properties(CMake_DeployApplicationTest_Build
  PROPERTIES
    DEPENDS "CMake_DeployApplicationTest_Configure"
)

add_test(NAME CMake_DeployApplicationTest_Install_Destdir
  COMMAND "${CMAKE_COMMAND}"
    --install "${CMAKE_CURRENT_BINARY_DIR}/deployApplicationTest"
    --config $<CONFIG>
    --verbose
)

set_tests_properties(CMake_DeployApplicationTest_Install_Destdir
  PROPERTIES
    DEPENDS "CMake_DeployApplicationTest_Build"
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)

add_test(NAME CMake_DeployApplicationTest_Run_TestApp1_Destdir
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}${deployApplicationTestInstallPrefix}/testApp1/bin/testApp1"
)

set_tests_properties(CMake_DeployApplicationTest_Run_TestApp1_Destdir
  PROPERTIES
    DEPENDS "CMake_DeployApplicationTest_Install_Destdir"
)

add_test(NAME CMake_DeployApplicationTest_Run_TestApp2_Destdir
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}${deployApplicationTestInstallPrefix}/testApp2/bin/testApp2"
)

set_tests_properties(CMake_DeployApplicationTest_Run_TestApp2_Destdir
  PROPERTIES
    DEPENDS "CMake_DeployApplicationTest_Install_Destdir"
    ENVIRONMENT "QT_DEBUG_PLUGINS=1"
)

if(BUILD_REAL_INSTALL_TESTS)

  add_test(NAME CMake_DeployApplicationTest_Install_Real
    COMMAND "${CMAKE_COMMAND}"
      --install "${CMAKE_CURRENT_BINARY_DIR}/deployApplicationTest"
      --config $<CONFIG>
      --verbose
  )

  set_tests_properties(CMake_DeployApplicationTest_Install_Real
    PROPERTIES
      DEPENDS "CMake_DeployApplicationTest_Build"
  )

  add_test(NAME CMake_DeployApplicationTest_Run_TestApp1_Real
    COMMAND "${deployApplicationTestInstallPrefix}/testApp1/bin/testApp1"
  )

  set_tests_properties(CMake_DeployApplicationTest_Run_TestApp1_Real
    PROPERTIES
      DEPENDS "CMake_DeployApplicationTest_Install_Real"
  )

  add_test(NAME CMake_DeployApplicationTest_Run_TestApp2_Real
    COMMAND "${deployApplicationTestInstallPrefix}/testApp2/bin/testApp2"
  )

  set_tests_properties(CMake_DeployApplicationTest_Run_TestApp2_Real
    PROPERTIES
      DEPENDS "CMake_DeployApplicationTest_Install_Real"
  )

endif()

# add_test(NAME CMake_DeployApplicationTest_Run
#   COMMAND "${CMAKE_CTEST_COMMAND}"
#     "${CMAKE_CURRENT_BINARY_DIR}/deployApplicationTest"
#     -V
#   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/deployApplicationTest"
# )
#
# set_tests_properties(CMake_DeployApplicationTest_Run
#   PROPERTIES
#     DEPENDS "CMake_DeployApplicationTest_Install"
# )

# On UNIX system wide install the RPATH are removed.
# In that case, running a executable will only work
# on a real system wide installation
# This also includes running mdtdeployutils once installed
if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set_tests_properties(CMake_DeployApplicationTest_Install_Destdir PROPERTIES DISABLED YES)
  set_tests_properties(CMake_DeployApplicationTest_Run_TestApp1_Destdir PROPERTIES DISABLED YES)
  set_tests_properties(CMake_DeployApplicationTest_Run_TestApp2_Destdir PROPERTIES DISABLED YES)
endif()
