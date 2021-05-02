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
#ifndef MDT_COMMAND_LINE_PARSE_ERROR_H
#define MDT_COMMAND_LINE_PARSE_ERROR_H

#include "Mdt/DeployUtils/QRuntimeError.h"

/*! \brief Error thrown when parsing the command-line fails
 */
class CommandLineParseError : public Mdt::DeployUtils::QRuntimeError
{
   public:

    /*! \brief Construct a error
     */
    explicit CommandLineParseError(const QString & what)
     : QRuntimeError(what)
    {
    }
};

#endif // #ifndef MDT_COMMAND_LINE_PARSE_ERROR_H
