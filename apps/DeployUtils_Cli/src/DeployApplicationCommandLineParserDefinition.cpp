/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#include "DeployApplicationCommandLineParserDefinition.h"
#include "CommonCommandLineParserDefinitionOptions.h"
#include "CommandLineCommand.h"

using namespace Mdt::CommandLineParser;

DeployApplicationCommandLineParserDefinition::DeployApplicationCommandLineParserDefinition(QObject *parent) noexcept
 : QObject(parent)
{
}

void DeployApplicationCommandLineParserDefinition::setup() noexcept
{
  assert( !mApplicationName.trimmed().isEmpty() );

  mCommand.setName( commandName(CommandLineCommand::DeployApplication) );

  const QString description = tr(
    "Deploy a application on the base of given executable.\n"
    "The executable and the required dependencies will be copied to a deployable directory.\n"
    "Example:\n"
    "%1 %2 ./myApp /path/to/myAppFolder"
  ).arg( mApplicationName, mCommand.name() );
  mCommand.setDescription(description);

  mCommand.addHelpOption();

  const QString shLibOverwriteBehaviorOptionDescription = tr(
    "Behavior to adopt when a shared library allready exists at the destination location.\n"
    "If the given executable allready exists in the destination directory, "
    "it will be replaced, not matter what behaviour was given here.\n"
    "For shared libraries, the rules are slightly different:\n"
    "if the source and destination locations are the same for a shared library, "
    "the library is allways kept as is, regardless of this option.\n"
    "Possible values are: keep, overwrite or fail.\n"
    "keep: the destination library will not be changed at all.\n"
    "overwrite: the destination library will be replaced.\n"
    "fail: the command will fail if the destination library allready exists."
  );
  ParserDefinitionOption shLibOverwriteBehaviorOption( QLatin1String("shlib-overwrite-behavior"), shLibOverwriteBehaviorOptionDescription );
  shLibOverwriteBehaviorOption.setValueName( QLatin1String("behavior") );
  shLibOverwriteBehaviorOption.setPossibleValues({QLatin1String("keep"),QLatin1String("overwrite"),QLatin1String("fail")});
  mCommand.addOption(shLibOverwriteBehaviorOption);

  const QString removeRPathOptionDescription = tr(
    "By default, on platform that supports rpath, the rpath informations of the copied executable "
    "is set to the relative location of the installed shared libraries "
    "(i.e. $ORIGIN/../lib for ELF files on platforms like Linux)\n"
    "For each shared library that have been copied, the rpath informations "
    "is set to its relative location "
    "(i.e. $ORIGIN for ELF files on platforms like Linux)\n"
    "With this option, the rpath informations are removed "
    "for the copied executable, as well as for each shared library that has been copied.\n"
    "This option is ignored on platforms that do not support rpath."
  );
  mCommand.addOption( QLatin1String("remove-rpath"), removeRPathOptionDescription );

  mCommand.addOption( CommonCommandLineParserDefinitionOptions::makeSearchPrefixPathListOption() );

  mCommand.addOption( CommonCommandLineParserDefinitionOptions::makePathListSeparatorOption() );

  mCommand.addOption( CommonCommandLineParserDefinitionOptions::makeCompilerLocationOption() );

  const QString runtimeDestinationOptionDescription = tr(
    "Directory where to put runtime files "
    "(executables on Linux, executables and dll's on Windows) "
    "defaults to bin"
  );
  ParserDefinitionOption runtimeDestinationOption( QLatin1String("runtime-destination"), runtimeDestinationOptionDescription );
  runtimeDestinationOption.setValueName( QLatin1String("dir") );
  mCommand.addOption(runtimeDestinationOption);

  const QString libraryDestinationOptionDescription = tr(
    "Directory where to put library files "
    "(shared libraries on Linux) "
    "defaults to lib"
  );
  ParserDefinitionOption libraryDestinationOption( QLatin1String("library-destination"), libraryDestinationOptionDescription );
  libraryDestinationOption.setValueName( QLatin1String("dir") );
  mCommand.addOption(libraryDestinationOption);

  /*
   * Format of set:
   * Here the pipe | separator was choosen,
   * because ; causes problems in CMake
   * when passing arguments to functions
   */
  const QString qtPluginsSetOptionSecription = tr(
    "Limit the set of Qt plugins to deploy "
    "the format is:\n"
    "directoryName1:name1,name2,nameN|directoryNameN:name1,name2,nameN\n"
    "example:\n"
    "imageformats:jpeg,svg|platforms:xcb,vnc,eglfs,windows,direct2d\n"
    "In above example, only jpeg and svg imageformats plugins will be deployed.\n"
    "For platform plugins, we expressed plugins for Linux and Windows. "
    "On Linux, only xcb, vnc and eglfs will be deployed. "
    "On Windows, only windows and direct2d plugins will be deployed."
  );
  ParserDefinitionOption qtPluginsSetOption(QLatin1String("qt-plugins-set"), qtPluginsSetOptionSecription);
  qtPluginsSetOption.setValueName( QLatin1String("set") );
  mCommand.addOption(qtPluginsSetOption);

  mCommand.addPositionalArgument( ValueType::File, QLatin1String("executable"), tr("Path to the application executable.") );

  const QString destinationDirectoryDescription = tr(
    "Path to the destination directory.\n"
    "Several sub-directories will be created, like bin , maybe lib (depending on the platform).\n"
    "The executable and its dependencies will be copied to the appropriate sub-directory."
  );
  mCommand.addPositionalArgument(ValueType::Directory, QLatin1String("destination"), destinationDirectoryDescription);
}
