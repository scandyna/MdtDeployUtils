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
#include "AbstractExecutableFileReaderEngine.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

AbstractExecutableFileReaderEngine::AbstractExecutableFileReaderEngine(QObject* parent)
 : QObject(parent)
{
}

void AbstractExecutableFileReaderEngine::openFile(const QFileInfo & fileInfo)
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( !isOpen() );

  if( !fileInfo.exists() ){
    const QString message = tr("file '%1' does not exist")
                            .arg( fileInfo.absoluteFilePath() );
    throw FileOpenError(message);
  }

  mFile.setFileName( fileInfo.absoluteFilePath() );
  if( !mFile.open(QIODevice::ReadOnly) ){
    const QString message = tr("could not open file '%1': %2")
                            .arg( fileInfo.absoluteFilePath(), mFile.errorString() );
    throw FileOpenError(message);
  }

  newFileOpen( mFile.fileName() );
}

void AbstractExecutableFileReaderEngine::close()
{
  mFileMapper.unmap(mFile);
  mFile.close();
  fileClosed();
}

bool AbstractExecutableFileReaderEngine::isElfFile()
{
  assert( isOpen() );

  return doIsElfFile();
}

bool AbstractExecutableFileReaderEngine::isPeImageFile()
{
  assert( isOpen() );

  return doIsPeImageFile();
}

Platform AbstractExecutableFileReaderEngine::getFilePlatform()
{
  assert( isOpen() );

  return doGetFilePlatform();
}

bool AbstractExecutableFileReaderEngine::isExecutableOrSharedLibrary()
{
  assert( isOpen() );

  return doIsExecutableOrSharedLibrary();
}

QStringList AbstractExecutableFileReaderEngine::getNeededSharedLibraries()
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  return doGetNeededSharedLibraries();
}

QStringList AbstractExecutableFileReaderEngine::getRunPath()
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  return doGetRunPath();
}

qint64 AbstractExecutableFileReaderEngine::fileSize() const noexcept
{
  assert( isOpen() );

  return mFile.size();
}

Impl::ByteArraySpan AbstractExecutableFileReaderEngine::mapIfRequired(qint64 offset, qint64 size)
{
  assert( isOpen() );

  return mFileMapper.mapIfRequired(mFile, offset, size);
}

}} // namespace Mdt{ namespace DeployUtils{
