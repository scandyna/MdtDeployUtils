include(GenerateExportHeader)
include(MdtAddLibrary)
include(MdtInstallMdtLibrary)

mdt_add_library(
  NAMESPACE Mdt
  LIBRARY_NAME ItemEditor
  PUBLIC_DEPENDENCIES Mdt0::ItemModel
  SOURCE_FILES
    Mdt/ItemEditor/SortSetupWidget.cpp
)

mdt_install_mdt_library(
  TARGET Mdt_ItemEditor
  VERSION_COMPATIBILITY ExactVersion
)
