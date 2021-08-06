##############################################################
#  Copyright Philippe Steinmann 2021 - 2021.
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt)
##############################################################
include(MdtAddTest)


mdt_add_test(
  NAME LibraryNameBenchmark
  TARGET libraryNameBenchmark
  DEPENDENCIES Mdt::DeployUtilsCore Qt5::Test
  SOURCE_FILES
    src/LibraryNameBenchmark.cpp
)
