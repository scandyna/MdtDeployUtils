/****************************************************************************
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
 ****************************************************************************/
#ifndef DEPLOY_UTILS_BASH_COMPLETION_GENERATOR_H
#define DEPLOY_UTILS_BASH_COMPLETION_GENERATOR_H

#include <string>

/*! \brief Generate the bash completion script for the mdtdeployutils command line tool
 *
 * \pre \a directoryPath must not be empty
 * \todo exceptions
 */
void generateBashCompletionScript(const std::string & directoryPath);

#endif // #ifndef DEPLOY_UTILS_BASH_COMPLETION_GENERATOR_H
