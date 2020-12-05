include(MdtAddTest)

mdt_add_test(
  NAME BashCompletionGeneratorTest
  TARGET bashCompletionGeneratorTest
  DEPENDENCIES Mdt::DeployUtils_Cli Qt5::Core Mdt::Catch2Main
  SOURCE_FILES
    src/BashCompletionGeneratorTest.cpp
)
