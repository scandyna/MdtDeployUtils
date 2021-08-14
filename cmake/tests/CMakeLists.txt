# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# Here we build the libraries that will be used for some (end-to-end-) tests.
#
# We must be able to install them in way that is completly independent of the main project.
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
# At this point, some scripts like MdtSharedLibrariesScripts.cmake.in
# have also to handle that case.
#
# So, use the DESTDIR mechanism "blindly" for the below tests,
# which can check MdtDeployUtils CMake modules.
#

# TODO: maybe not used. Test that modules do that itself (like CMake install does, despite not documented)
# string(REGEX REPLACE "^[a-zA-Z]:" ""
#   TEST_CMAKE_INSTALL_PREFIX
#   "${CMAKE_INSTALL_PREFIX}"
# )
# set(TEST_MDT_DEPLOY_UTILS_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}${TEST_CMAKE_INSTALL_PREFIX}")
#
# message("TEST_CMAKE_INSTALL_PREFIX: ${TEST_CMAKE_INSTALL_PREFIX}")
# message("TEST_MDT_DEPLOY_UTILS_INSTALL_PREFIX: ${TEST_MDT_DEPLOY_UTILS_INSTALL_PREFIX}")

# TODO: component install (currently headers etc.. are installed)
add_test(NAME EndToEndTests_Install_DeployUtils_Destdir
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install --config $<CONFIG>
#   COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --component Runtime
)

set_tests_properties(EndToEndTests_Install_DeployUtils_Destdir
  PROPERTIES
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)

if(BUILD_REAL_INSTALL_TESTS)
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
      "-DCMAKE_PREFIX_PATH=${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_BINARY_DIR}${testItemModelInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${CMAKE_PREFIX_PATH}"
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
      "-DCMAKE_PREFIX_PATH=${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_BINARY_DIR}${testItemModelInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${CMAKE_PREFIX_PATH}"
    --test-command "${CMAKE_CTEST_COMMAND}"
)

if(BUILD_REAL_INSTALL_TESTS)
  set_tests_properties(CMake_CopySharedLibrariesTargetDendsOnTest
    PROPERTIES
      DEPENDS "EndToEndTests_Install_ItemModel_lib_Destdir;EndToEndTests_Install_ItemModel_lib_Real;EndToEndTests_Install_DeployUtils_Destdir;EndToEndTests_Install_DeployUtils_Real"
  )
else()
  set_tests_properties(CMake_CopySharedLibrariesTargetDendsOnTest
    PROPERTIES
      DEPENDS "EndToEndTests_Install_ItemModel_lib_Destdir;EndToEndTests_Install_DeployUtils_Destdir"
  )
endif()


#########################################################################
# mdt_install_executable() test
# TODO: should move to MdtCMakeModules
#
# Use a simple executable that depends on ItemEditor test library.
# We not add other non std dependencies, like Qt.
# (installing required dependencies
#  is done by mdt_deploy_application() from MdtDeployUtils)
#
# A user project will then be build.
# This user project will use the above app as a CMake imported target.
# The user project also has a simple test
# that will run the app without any special environment.
#
# One goal ist to check that RPATH are set correctly for Unixes
# (note that RPATH will be empty on UNIX system wide install)
#
# A other goal is to check that the app is installed correctly and
# that the CMake package files are generated and installed correctly.
#
# Steps:
# - Build the app
# - Install the app
# - Build and run the user project
#
# TODO: install prefix + syswide test in CI etc.. also Windows
#   See also docs to see what to test
#
# TODO: maybe add a option to build (some) install tests and use CMAKE_INSTALL_PREFIX
#
# TODO: test also bin/subdir , and bin/ somedir/lib/ to check ORIGIN... set (do not confuse with syswide install)
#########################################################################

if(MDT_INSTALL_IS_UNIX_SYSTEM_WIDE)
  set(testTableEditorInstallPrefix "/usr")
else()
  set(testTableEditorInstallPrefix "/opt/TableEditor")
endif()

add_test(NAME EndToEndTests_Build_TableEditor
  COMMAND "${CMAKE_CTEST_COMMAND}"
    --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/apps/TableEditor" "${CMAKE_CURRENT_BINARY_DIR}/build/apps/TableEditor"
    --build-generator "${CMAKE_GENERATOR}"
    --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
    --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
    --build-config $<CONFIG>
    --build-options
      "-DCMAKE_PREFIX_PATH=${testItemEditorInstallPrefix};${testItemModelInstallPrefix};${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_BINARY_DIR}${testItemEditorInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${testItemModelInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${CMAKE_PREFIX_PATH}"
      "-DCMAKE_INSTALL_PREFIX=${testTableEditorInstallPrefix}"
      "-DBUILD_SHARED_LIBS=ON"
)

if(BUILD_REAL_INSTALL_TESTS)
  set_tests_properties(EndToEndTests_Build_TableEditor
    PROPERTIES
      DEPENDS "EndToEndTests_Install_ItemEditor_lib_Destdir;EndToEndTests_Install_ItemEditor_lib_Real;EndToEndTests_Install_DeployUtils_Destdir;EndToEndTests_Install_DeployUtils_Real"
  )
else()
  set_tests_properties(EndToEndTests_Build_TableEditor
    PROPERTIES
      DEPENDS "EndToEndTests_Install_ItemEditor_lib_Destdir;EndToEndTests_Install_DeployUtils_Destdir"
  )
endif()


add_test(NAME EndToEndTests_Install_TableEditor_Destdir
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/build/apps/TableEditor" --target install --config $<CONFIG>
)

set_tests_properties(EndToEndTests_Install_TableEditor_Destdir
  PROPERTIES
    DEPENDS EndToEndTests_Build_TableEditor
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)

if(BUILD_REAL_INSTALL_TESTS)

  add_test(NAME EndToEndTests_Install_TableEditor_Real
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/build/apps/TableEditor" --target install --config $<CONFIG>
  )

  set_tests_properties(EndToEndTests_Install_TableEditor_Real
    PROPERTIES
      DEPENDS EndToEndTests_Build_TableEditor
  )

endif()


add_test(NAME EndToEndTests_BuildAndRun_TableEditorUserProject
  COMMAND "${CMAKE_CTEST_COMMAND}"
    --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/apps/TableEditorUserProject" "${CMAKE_CURRENT_BINARY_DIR}/build/apps/TableEditorUserProject"
    --build-generator "${CMAKE_GENERATOR}"
    --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
    --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
    --build-config $<CONFIG>
    --build-options
      "-DCMAKE_PREFIX_PATH=${testTableEditorInstallPrefix};${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_BINARY_DIR}${testTableEditorInstallPrefix};${CMAKE_CURRENT_BINARY_DIR}${CMAKE_PREFIX_PATH}"
      "-DBUILD_SHARED_LIBS=ON"
)

if(BUILD_REAL_INSTALL_TESTS)

  set_tests_properties(EndToEndTests_BuildAndRun_TableEditorUserProject
    PROPERTIES
      DEPENDS "EndToEndTests_Install_TableEditor_Destdir;EndToEndTests_Install_TableEditor_Real;EndToEndTests_Install_DeployUtils_Destdir;EndToEndTests_Install_DeployUtils_Real"
  )

else()

  set_tests_properties(EndToEndTests_BuildAndRun_TableEditorUserProject
    PROPERTIES
      DEPENDS "EndToEndTests_Install_TableEditor_Destdir;EndToEndTests_Install_DeployUtils_Destdir"
  )

  # On UNIX system wide install the RPATH are removed,
  # so the relocated binary will not work
  # TODO: disable test
endif()

#########################################################################
# mdt_install_executable() CMake package test
# TODO: should move to MdtCMakeModules
#
# Creat a CMake project that uses above installed application
# as a CMake imported target.
#########################################################################

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

############################################
# InstallSharedLibrariesTargetDendsOnTest test
############################################
