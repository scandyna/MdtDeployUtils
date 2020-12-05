include(MdtAddTest)

mdt_add_test(
  NAME BashCompletionGeneratorTest
  TARGET bashCompletionGeneratorTest
  DEPENDENCIES Mdt::DeployUtils_Cli Mdt::Catch2Main
  SOURCE_FILES
    src/BashCompletionGeneratorTest.cpp
)
