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
#include "AbstractExecutableFileIoEngine.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

AbstractExecutableFileIoEngine::AbstractExecutableFileIoEngine(QObject* parent)
 : QObject(parent)
{
}

void AbstractExecutableFileIoEngine::openFile(const QFileInfo & fileInfo, ExecutableFileOpenMode mode)
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( !isOpen() );

  if( !fileInfo.exists() ){
    const QString message = tr("file '%1' does not exist")
                            .arg( fileInfo.absoluteFilePath() );
    throw FileOpenError(message);
  }

  mFile.setFileName( fileInfo.absoluteFilePath() );
  const auto openMode = qIoDeviceOpenModeFromOpenMode(mode);
  if( !mFile.open(openMode) ){
    const QString message = tr("could not open file '%1': %2")
                            .arg( fileInfo.absoluteFilePath(), mFile.errorString() );
    throw FileOpenError(message);
  }

  newFileOpen( mFile.fileName() );
}

void AbstractExecutableFileIoEngine::close()
{
  mFileMapper.unmap(mFile);
  mFile.close();
  fileClosed();
}

bool AbstractExecutableFileIoEngine::isElfFile()
{
  assert( isOpen() );

  return doIsElfFile();
}

bool AbstractExecutableFileIoEngine::isPeImageFile()
{
  assert( isOpen() );

  return doIsPeImageFile();
}

Platform AbstractExecutableFileIoEngine::getFilePlatform()
{
  assert( isOpen() );

  return doGetFilePlatform();
}

bool AbstractExecutableFileIoEngine::isExecutableOrSharedLibrary()
{
  assert( isOpen() );

  return doIsExecutableOrSharedLibrary();
}

bool AbstractExecutableFileIoEngine::containsDebugSymbols()
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  return doContainsDebugSymbols();
}

QStringList AbstractExecutableFileIoEngine::getNeededSharedLibraries()
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  return doGetNeededSharedLibraries();
}

RPath AbstractExecutableFileIoEngine::getRunPath()
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  return doGetRunPath();
}

void AbstractExecutableFileIoEngine::setRunPath(const RPath & rPath)
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  doSetRunPath(rPath);
}

qint64 AbstractExecutableFileIoEngine::fileSize() const noexcept
{
  assert( isOpen() );

  return mFile.size();
}

void AbstractExecutableFileIoEngine::resizeFile(qint64 size)
{
  assert( isOpen() );
  assert( size > 0 );

  if( !mFile.resize(size) ){
    const QString msg = tr("resize file '%1' failed: %2")
                        .arg( fileName(), mFile.errorString() );
    throw ExecutableFileWriteError(msg);
  }
}



QString AbstractExecutableFileIoEngine::fileName() const noexcept
{
  assert( isOpen() );

  return mFile.fileName();
}

Impl::ByteArraySpan AbstractExecutableFileIoEngine::mapIfRequired(qint64 offset, qint64 size)
{
  assert( isOpen() );

  return mFileMapper.mapIfRequired(mFile, offset, size);
}

void AbstractExecutableFileIoEngine::doSetRunPath(const RPath &)
{
}

QIODevice::OpenMode AbstractExecutableFileIoEngine::qIoDeviceOpenModeFromOpenMode(ExecutableFileOpenMode mode) noexcept
{
  switch(mode){
    case ExecutableFileOpenMode::ReadOnly:
      return QIODevice::ReadOnly;
    case ExecutableFileOpenMode::ReadWrite:
      return QIODevice::ReadWrite;
  }

  return QIODevice::ReadOnly;
}

}} // namespace Mdt{ namespace DeployUtils{
