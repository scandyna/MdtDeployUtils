include(MdtAddTest)

mdt_add_test(
  NAME CommandLineParserTest
  TARGET commandLineParserTest
  DEPENDENCIES Mdt::DeployUtils_Cli Qt5::Core Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CommandLineParserTest.cpp
)

mdt_add_test(
  NAME CommandLineParserErrorTest
  TARGET commandLineParserErrorTest
  DEPENDENCIES Mdt::DeployUtils_Cli Qt5::Core Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CommandLineParserErrorTest.cpp
)
