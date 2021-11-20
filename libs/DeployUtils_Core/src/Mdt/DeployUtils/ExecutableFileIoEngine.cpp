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
#include "ExecutableFileIoEngine.h"
#include "ExecutableFileFormat.h"
#include "AbstractExecutableFileIoEngine.h"
#include "ElfFileIoEngine.h"
#include "PeFileIoEngine.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

ExecutableFileIoEngine::ExecutableFileIoEngine(QObject *parent)
 : QObject(parent)
{
}

ExecutableFileIoEngine::~ExecutableFileIoEngine() noexcept
{
}

void ExecutableFileIoEngine::openFile(const QFileInfo & fileInfo, ExecutableFileOpenMode mode)
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( !isOpen() );

  const auto hostPlatform = Platform::nativePlatform();

  if( !mIoEngine ){
    instanciateEngine( hostPlatform.executableFileFormat() );
  }
  assert( mIoEngine.get() != nullptr );

  mIoEngine->openFile(fileInfo, mode);

  if( hostPlatform.operatingSystem() == OperatingSystem::Linux ){
    if( !mIoEngine->isElfFile() ){
      mIoEngine->close();
      mIoEngine.reset();
      instanciateEngine(ExecutableFileFormat::Pe);
      mIoEngine->openFile(fileInfo, mode);
    }
  }

  if( hostPlatform.operatingSystem() == OperatingSystem::Windows ){
    if( !mIoEngine->isPeImageFile() ){
      mIoEngine->close();
      mIoEngine.reset();
      instanciateEngine(ExecutableFileFormat::Elf);
      mIoEngine->openFile(fileInfo, mode);
    }
  }
}

void ExecutableFileIoEngine::openFile(const QFileInfo & fileInfo, ExecutableFileOpenMode mode, const Platform & platform)
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( !platform.isNull() );
  assert( !isOpen() );

  if( !mIoEngine ){
    instanciateEngine( platform.executableFileFormat() );
  }
  assert( mIoEngine.get() != nullptr );

  if( !mIoEngine->supportsPlatform(platform) ){
    mIoEngine.reset();
    instanciateEngine( platform.executableFileFormat() );
  }
  assert( mIoEngine.get() != nullptr );

  mIoEngine->openFile(fileInfo, mode);

  Platform filePlatform;
  try{
    filePlatform = getFilePlatform();
  }catch(const ExecutableFileReadError &){
  }

  if( filePlatform != platform ){
    const QString message = tr("File '%1' is not of the requested platorm")
                            .arg( fileInfo.absoluteFilePath() );
    throw FileOpenError(message);
  }
}

bool ExecutableFileIoEngine::isOpen() const noexcept
{
  if( !mIoEngine ){
    return false;
  }
  return mIoEngine->isOpen();
}

void ExecutableFileIoEngine::close()
{
  if(mIoEngine){
    mIoEngine->close();
  }
}

Platform ExecutableFileIoEngine::getFilePlatform()
{
  assert( isOpen() );
  assert( mIoEngine.get() != nullptr );

  return mIoEngine->getFilePlatform();
}

void ExecutableFileIoEngine::instanciateEngine(ExecutableFileFormat format) noexcept
{
  assert( mIoEngine.get() == nullptr );
  assert( format != ExecutableFileFormat::Unknown );

  switch(format){
    case ExecutableFileFormat::Elf:
      mIoEngine = std::make_unique<ElfFileIoEngine>();
      break;
    case ExecutableFileFormat::Pe:
      mIoEngine = std::make_unique<PeFileIoEngine>();
      break;
    case ExecutableFileFormat::Unknown:
      break;
  }

  if( mIoEngine.get() != nullptr ){
    connect(mIoEngine.get(), &AbstractExecutableFileIoEngine::message, this, &ExecutableFileIoEngine::message);
    connect(mIoEngine.get(), &AbstractExecutableFileIoEngine::verboseMessage, this, &ExecutableFileIoEngine::verboseMessage);
  }
}

}} // namespace Mdt{ namespace DeployUtils{
