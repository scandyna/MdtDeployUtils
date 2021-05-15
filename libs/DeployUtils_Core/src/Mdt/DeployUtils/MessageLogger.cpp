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
#include "MessageLogger.h"
#include "ConsoleMessageLogger.h"

namespace Mdt{ namespace DeployUtils{

MessageLogger *MessageLogger::instance = nullptr;

MessageLogger::MessageLogger()
 : mBackend( std::make_unique<ConsoleMessageLogger>() )
{
  assert(instance == nullptr);

  instance = this;
}

MessageLogger::~MessageLogger() noexcept
{
  instance = nullptr;
}

void MessageLogger::info(const QString & message)
{
  assert( MessageLogger::isInitialized() );

  instance->mBackend->info(message);
}

void MessageLogger::error(const QString & message)
{
  assert( MessageLogger::isInitialized() );

  instance->mBackend->error(message);
}

void MessageLogger::error(const std::exception & err)
{
  assert( MessageLogger::isInitialized() );

  error( QString::fromLocal8Bit( err.what() ) );
}

}} // namespace Mdt{ namespace DeployUtils{
