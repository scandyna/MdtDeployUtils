##############################################################
#  Copyright Philippe Steinmann 2020 - 2020.
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt)
##############################################################
include(MdtAddTest)

mdt_add_test(
  NAME AlgorithmTest
  TARGET algorithmTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main
  SOURCE_FILES
    src/AlgorithmTest.cpp
)

mdt_add_test(
  NAME MessageLoggerTest
  TARGET messageLoggerTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/MessageLoggerTest.cpp
)

mdt_add_test(
  NAME CopySharedLibrariesTargetDependsOnTest
  TARGET copySharedLibrariesTargetDependsOnTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopySharedLibrariesTargetDependsOnTest.cpp
)

mdt_add_test(
  NAME CopySharedLibrariesTargetDependsOnErrorTest
  TARGET copySharedLibrariesTargetDependsOnErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopySharedLibrariesTargetDependsOnErrorTest.cpp
)
