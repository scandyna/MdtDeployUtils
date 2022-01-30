/****************************************************************************
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
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_MAIN_H
#define MDT_DEPLOY_UTILS_MAIN_H

#include "CommandLineParser.h"
#include <Mdt/AbstractConsoleApplicationMainFunction>

/*! \brief Main function of mdtdeployutils
 */
class DeployUtilsMain : public Mdt::AbstractConsoleApplicationMainFunction
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  explicit DeployUtilsMain(QObject* parent = nullptr);

 private:

  int runMain() override;
  void copySharedLibrariesTargetDependsOn(const CommandLineParser & commandLineParser);
  void deployApplication(const CommandLineParser & commandLineParser);
};

#endif // #ifndef MDT_DEPLOY_UTILS_MAIN_H
