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
#include "ExecutableFileReader.h"
#include "AbstractExecutableFileReaderEngine.h"
#include "ElfFileReader.h"
#include "PeFileReader.h"
#include "ExecutableFileFormat.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

ExecutableFileReader::ExecutableFileReader(QObject* parent)
 : QObject(parent)
{
}

ExecutableFileReader::~ExecutableFileReader()
{
}

void ExecutableFileReader::openFile(const QFileInfo & fileInfo)
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( !isOpen() );

  if( !mReaderEngine ){
    instanciateReader( Platform::nativePlatform() );
  }
  assert( mReaderEngine.get() != nullptr );

  mReaderEngine->openFile(fileInfo);
}

void ExecutableFileReader::openFile(const QFileInfo & fileInfo, const Platform & platform)
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( !platform.isNull() );
  assert( !isOpen() );

  if( !mReaderEngine ){
    instanciateReader(platform);
  }
  assert( mReaderEngine.get() != nullptr );

  if( !mReaderEngine->supportsPlatform(platform) ){
    mReaderEngine.reset();
    instanciateReader(platform);
  }
  assert( mReaderEngine.get() != nullptr );

  mReaderEngine->openFile(fileInfo);

  /// \todo implement
//   if( getFilePlatform() != platform ){
//     /// \todo exception
//   }
}

bool ExecutableFileReader::isOpen() const noexcept
{
  if( !mReaderEngine ){
    return false;
  }
  return mReaderEngine->isOpen();
}

void ExecutableFileReader::close()
{
  if(mReaderEngine){
    mReaderEngine->close();
  }
}

Platform ExecutableFileReader::getFilePlatform()
{
  assert( isOpen() );
  assert( mReaderEngine.get() != nullptr );

  return mReaderEngine->getFilePlatform();
}

bool ExecutableFileReader::isExecutableOrSharedLibrary()
{
  assert( isOpen() );
  assert( mReaderEngine.get() != nullptr );

  return mReaderEngine->isExecutableOrSharedLibrary();
}

QStringList ExecutableFileReader::getNeededSharedLibraries()
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );
  assert( mReaderEngine.get() != nullptr );

  return mReaderEngine->getNeededSharedLibraries();
}

QStringList ExecutableFileReader::getRunPath()
{
  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );
  assert( mReaderEngine.get() != nullptr );

  return mReaderEngine->getRunPath();
}

void ExecutableFileReader::instanciateReader(const Platform & platform) noexcept
{
  assert( mReaderEngine.get() == nullptr );

  switch( platform.executableFileFormat() ){
    case ExecutableFileFormat::Elf:
      mReaderEngine = std::make_unique<ElfFileReader>();
      break;
    case ExecutableFileFormat::Pe:
      mReaderEngine = std::make_unique<PeFileReader>();
      break;
  }
}

}} // namespace Mdt{ namespace DeployUtils{
