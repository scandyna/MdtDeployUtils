/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ***********************************************************************************************/
#include "CopySharedLibrariesTargetDependsOnCommandLineParserDefinition.h"
#include "CommandLineCommand.h"

using namespace Mdt::CommandLineParser;

CopySharedLibrariesTargetDependsOnCommandLineParserDefinition::CopySharedLibrariesTargetDependsOnCommandLineParserDefinition(QObject *parent) noexcept
 : QObject(parent)
{
}

void CopySharedLibrariesTargetDependsOnCommandLineParserDefinition::setup() noexcept
{
  assert( !mApplicationName.trimmed().isEmpty() );

  mCommand.setName( commandName(CommandLineCommand::CopySharedLibrariesTargetDependsOn) );

  const QString description = tr(
    "Copy shared libraries a target depends on to a destination.\n"
    "Example:\n"
    "%1 %2 /home/me/dev/build/myapp/src/myapp /home/me/opt/myapp/lib/"
  ).arg( mApplicationName, mCommand.name() );
  mCommand.setDescription(description);

  mCommand.addPositionalArgument( ValueType::File, QLatin1String("target"), tr("Path to a executable or a shared library.") );
  mCommand.addPositionalArgument( ValueType::Directory, QLatin1String("destination"), tr("Path to the destination directory.") );
  mCommand.addHelpOption();

  const QString overwriteBehaviorOptionDescription = tr(
    "Behavior to adopt when a shared library allready exists at the destination location.\n"
    "Note: if the source and destination locations are the same for a shared library, "
    "the library is allways kept as is, regardless of this option.\n"
    "Possible values are: keep, overwrite or fail.\n"
    "keep: the destination library will not be changed at all.\n"
    "overwrite: the destination library will be replaced.\n"
    "fail: the command will fail if the destination library allready exists."
  );
  ParserDefinitionOption overwriteBehaviorOption( QLatin1String("overwrite-behavior"), overwriteBehaviorOptionDescription );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );
  overwriteBehaviorOption.setPossibleValues({QLatin1String("keep"),QLatin1String("overwrite"),QLatin1String("fail")});
  mCommand.addOption(overwriteBehaviorOption);

  const QString removePathOptionDescription = tr(
    "By default, on platform that supports rpath, the rpath informations is set to $ORIGIN for each shared library that has been copied.\n"
    "With this option, the rpath informations are removed for each shared library that has been copied.\n"
    "This option is ignored on platforms that do not support rpath."
  );
  mCommand.addOption( QLatin1String("remove-rpath"), removePathOptionDescription );

  /// \todo fix doc
  const QString searchPrefixPathListOptionDescription = tr(
    "Semicolon separated list of path where to find the shared libraries.\n"
    "Each path represents the installation prefix of a library, for examle: /opt/Qt5/5.14.2/gcc_64.\n"
    "This is mainly useful on platforms that don't have RPATH support, such as Windows.\n"
    "On platforms that supports RPATH, some shared libraries cannot be found on some cases, "
    "so this option should also be provided.\n"
    "For more informations about why, see https://scandyna.gitlab.io/mdt-cmake-modules/Modules/MdtRuntimeEnvironment.html."
  );
  ParserDefinitionOption searchPrefixPathListOption( QLatin1String("search-prefix-path-list"), searchPrefixPathListOptionDescription );
  searchPrefixPathListOption.setValueName( QLatin1String("path-list") );
  mCommand.addOption(searchPrefixPathListOption);

  const QString compilerLocationOptionDescription = tr(
    "Location of the compiler.\n"
    "This option is used to locate compiler libraries to distribute, like, for example, LIBSTDC++-6.DLL for MinGW or MSVCP140.DLL for MSVC.\n"
    "Possible values are from-env=<var-name>, vc-install-dir=<path-to-dir>, compiler-path=<path>\n"
    "from-env=<var-name>: will use given environment variable. Note: the var-name cannot be PATH. Example:\n"
    " from-env=VcInstallDir\n"
    "vc-install-dir=<path-to-dir>: specify the path to MSVC install directory. Example:\n"
    " vc-install-dir=\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC\"\n"
    "compiler-path=<path>: full path to the compiler. This can be used with CMAKE_CXX_COMPILER"
  );
  ParserDefinitionOption compilerLocationOption( QLatin1String("compiler-location"), compilerLocationOptionDescription );
  compilerLocationOption.setValueName( QLatin1String("location") );
  mCommand.addOption(compilerLocationOption);
}
