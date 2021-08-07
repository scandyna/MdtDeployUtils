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
# We can use the DESTDIR environment variable to relocate the installation.
# Note: this also checks that DESTDIR is used properly in some of the scripts.
# This also works on Windows, also with MSVC,
# but CMake do not recomment it, because CMAKE_INSTALL_PREFIX may start with a drive letter,
# like C:/Program Files, which cannot be prepended with some other prefix.
#
# So, if CMAKE_INSTALL_PREFIX is, for example, C:/Program Files/MdtDeployUtils
# we remove the C: .
# For the tests, MdtDeployUtils will be installed here:
# ${CMAKE_CURRENT_BINARY_DIR}/Program Files/MdtDeployUtils

string(REGEX REPLACE "^[a-zA-Z]:" ""
  TEST_CMAKE_INSTALL_PREFIX
  "${CMAKE_INSTALL_PREFIX}"
)
set(TEST_MDT_DEPLOY_UTILS_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}${TEST_CMAKE_INSTALL_PREFIX}")

message("TEST_CMAKE_INSTALL_PREFIX: ${TEST_CMAKE_INSTALL_PREFIX}")
message("TEST_MDT_DEPLOY_UTILS_INSTALL_PREFIX: ${TEST_MDT_DEPLOY_UTILS_INSTALL_PREFIX}")

# TODO: component install (currently headers etc.. are installed)
add_test(NAME EndToEndTests_Install_DeployUtils
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install --config $<CONFIG>
#   COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --component Runtime
)

set_tests_properties(EndToEndTests_Install_DeployUtils
  PROPERTIES
    ENVIRONMENT "DESTDIR=${CMAKE_CURRENT_BINARY_DIR}"
)


############################
# ItemModel library
############################

add_test(NAME EndToEndTests_Build_ItemModel_lib
  COMMAND "${CMAKE_CTEST_COMMAND}"
    --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/libs/ItemModel" "${CMAKE_CURRENT_BINARY_DIR}/libs/ItemModel"
    --build-generator "${CMAKE_GENERATOR}"
    --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
    --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
    --build-config $<CONFIG>
    --build-options
      "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
      "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/opt/ItemModel"
      "-DBUILD_SHARED_LIBS=ON"
)

add_test(NAME EndToEndTests_Install_ItemModel_lib
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}/libs/ItemModel" --target install --config $<CONFIG>
)

set_tests_properties(EndToEndTests_Install_ItemModel_lib
  PROPERTIES
    DEPENDS EndToEndTests_Build_ItemModel_lib
)


############################################
# CopySharedLibrariesTargetDendsOnTest test
############################################

add_test(NAME Build_CopySharedLibrariesTargetDendsOnTest
  COMMAND "${CMAKE_CTEST_COMMAND}"
    --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/CopySharedLibrariesTargetDendsOnTest" "${CMAKE_CURRENT_BINARY_DIR}/copySharedLibrariesTargetDendsOnTest"
    --build-generator "${CMAKE_GENERATOR}"
    --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
    --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
    --build-config $<CONFIG>
    --build-options
      "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/opt/ItemModel;${TEST_MDT_DEPLOY_UTILS_INSTALL_PREFIX};${CMAKE_PREFIX_PATH}"
      "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/opt/testApp"
    --test-command "${CMAKE_CTEST_COMMAND}"
)

set_tests_properties(Build_CopySharedLibrariesTargetDendsOnTest
  PROPERTIES
    DEPENDS "EndToEndTests_Install_ItemModel_lib;EndToEndTests_Install_DeployUtils"
)

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
