/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2020 Philippe Steinmann.
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
 ****************************************************************************/
#include "CommandLineCommand.h"
#include <QLatin1String>

QString commandName( CommandLineCommand command)
{
  switch(command){
    case CommandLineCommand::Unknown:
      return QLatin1String("Unknown");
    case CommandLineCommand::GetSharedLibrariesTargetDependsOn:
      return QLatin1String("get-shared-libraries-target-depends-on");
    case CommandLineCommand::CopySharedLibrariesTargetDependsOn:
      return QLatin1String("copy-shared-libraries-target-depends-on");
  }
  return QString();
}

CommandLineCommand commandFromString(const QString & command)
{
  if( command == commandName( CommandLineCommand::GetSharedLibrariesTargetDependsOn) ){
    return CommandLineCommand::GetSharedLibrariesTargetDependsOn;
  }
  if( command == commandName( CommandLineCommand::CopySharedLibrariesTargetDependsOn) ){
    return CommandLineCommand::CopySharedLibrariesTargetDependsOn;
  }

  return CommandLineCommand::Unknown;
}
