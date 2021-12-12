/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#include "ExecutableFileWriter.h"
#include "AbstractExecutableFileIoEngine.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

ExecutableFileWriter::ExecutableFileWriter(QObject *parent)
 : QObject(parent)
{
  connect(&mEngine, &ExecutableFileIoEngine::message, this, &ExecutableFileWriter::message);
  connect(&mEngine, &ExecutableFileIoEngine::verboseMessage, this, &ExecutableFileWriter::verboseMessage);
}

void ExecutableFileWriter::openFile(const QFileInfo & fileInfo)
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( !isOpen() );

  mEngine.openFile(fileInfo, ExecutableFileOpenMode::ReadWrite);
}

void ExecutableFileWriter::openFile(const QFileInfo & fileInfo, const Platform & platform)
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( !platform.isNull() );
  assert( !isOpen() );

  mEngine.openFile(fileInfo, ExecutableFileOpenMode::ReadWrite, platform);
}

bool ExecutableFileWriter::isOpen() const noexcept
{
  return mEngine.isOpen();
}

void ExecutableFileWriter::close()
{
  mEngine.close();
}

bool ExecutableFileWriter::isExecutableOrSharedLibrary()
{
  assert( isOpen() );

  return mEngine.engine()->isExecutableOrSharedLibrary();
}

RPath ExecutableFileWriter::getRunPath()
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  return mEngine.engine()->getRunPath();
}

void ExecutableFileWriter::setRunPath(const RPath & rPath)
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  mEngine.engine()->setRunPath(rPath);
}

}} // namespace Mdt{ namespace DeployUtils{
