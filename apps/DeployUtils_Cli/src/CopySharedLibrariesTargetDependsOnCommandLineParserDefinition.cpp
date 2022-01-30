/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2022 Philippe Steinmann.
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
#include "CommonCommandLineParserDefinitionOptions.h"
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
    "fail (the default): the command will fail if the destination library allready exists."
  );
  ParserDefinitionOption overwriteBehaviorOption( QLatin1String("overwrite-behavior"), overwriteBehaviorOptionDescription );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );
  overwriteBehaviorOption.setPossibleValues({QLatin1String("keep"),QLatin1String("overwrite"),QLatin1String("fail")});
  mCommand.addOption(overwriteBehaviorOption);

  const QString removeRPathOptionDescription = tr(
    "By default, on platform that supports rpath, the rpath informations is set to $ORIGIN for each shared library that has been copied.\n"
    "With this option, the rpath informations are removed for each shared library that has been copied.\n"
    "This option is ignored on platforms that do not support rpath."
  );
  mCommand.addOption( QLatin1String("remove-rpath"), removeRPathOptionDescription );

  mCommand.addOption( CommonCommandLineParserDefinitionOptions::makeSearchPrefixPathListOption() );

  mCommand.addOption( CommonCommandLineParserDefinitionOptions::makeCompilerLocationOption() );
}
