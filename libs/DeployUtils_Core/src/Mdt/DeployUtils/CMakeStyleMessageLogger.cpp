/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "CMakeStyleMessageLogger.h"
#include <QByteArray>
#include <iostream>

namespace Mdt{ namespace DeployUtils{

void CMakeStyleMessageLogger::info(const QString & message)
{
  std::cout << "[CMake style INFO]: " << message.toLocal8Bit().toStdString() << std::endl;
}

void CMakeStyleMessageLogger::error(const QString & message)
{
  std::cerr << "[CMake style ERROR]: " << message.toLocal8Bit().toStdString() << std::endl;
}

}} // namespace Mdt{ namespace DeployUtils{
