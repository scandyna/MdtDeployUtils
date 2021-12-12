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
#include "ElfFileIoEngine.h"
#include "RPathElf.h"
#include "Mdt/DeployUtils/Impl/Elf/FileIoEngine.h"
#include "Mdt/DeployUtils/Impl/Elf/FileWriterFile.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

ElfFileIoEngine::ElfFileIoEngine(QObject *parent)
  : AbstractExecutableFileIoEngine(parent),
    mImpl( std::make_unique<Impl::Elf::FileIoEngine>() )
{
}

ElfFileIoEngine::~ElfFileIoEngine() noexcept
{
}

QString ElfFileIoEngine::getSoName()
{
  using Impl::ByteArraySpan;

  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  const qint64 size = fileSize();

  const ByteArraySpan map = mapIfRequired(0, size);

  return mImpl->getSoName(map);
}

void ElfFileIoEngine::newFileOpen(const QString & fileName)
{
  mImpl->setFileName(fileName);
}

void ElfFileIoEngine::fileClosed()
{
  mImpl->clear();
}

bool ElfFileIoEngine::doSupportsPlatform(const Platform & platform) const noexcept
{
  return platform.executableFileFormat() == ExecutableFileFormat::Elf;
}

bool ElfFileIoEngine::doIsElfFile()
{
  using Impl::ByteArraySpan;
  using Impl::Elf::Ident;
  using Impl::Elf::extractIdent;

  const qint64 size = 16;

  if( fileSize() < size ){
    return false;
  }

  const ByteArraySpan map = mapIfRequired(0, size);

  const Ident ident = extractIdent(map);

  return ident.isValid();
}

Platform ElfFileIoEngine::doGetFilePlatform()
{
  using Impl::ByteArraySpan;
  using Impl::Elf::FileHeader;
  using Impl::Elf::OsAbiType;
  using Impl::Elf::Machine;

  const int64_t size = mImpl->minimumSizeToReadFileHeader();

  if( fileSize() < size ){
    const QString message = tr("file '%1' is to small to read the file header")
                            .arg( fileName() );
    throw ExecutableFileReadError(message);
  }

  const ByteArraySpan map = mapIfRequired(0, size);

  const FileHeader fileHeader = mImpl->getFileHeader(map);
  assert( fileHeader.seemsValid() );

  const auto fileFormat = ExecutableFileFormat::Elf;

  OperatingSystem os;
  switch( fileHeader.ident.osAbiType() ){
    case OsAbiType::SystemV:
    case OsAbiType::Linux:
      os = OperatingSystem::Linux;
      break;
    default:
      os = OperatingSystem::Unknown;
  }

  ProcessorISA cpu;
  switch( fileHeader.machineType() ){
    case Machine::X86:
      cpu = ProcessorISA::X86_32;
      break;
    case Machine::X86_64:
      cpu = ProcessorISA::X86_64;
      break;
    default:
      cpu = ProcessorISA::Unknown;
  }

  const auto fakeCompiler = Compiler::Gcc;

  return Platform(os, fileFormat, fakeCompiler, cpu);
}

bool ElfFileIoEngine::doIsExecutableOrSharedLibrary()
{
  using Impl::ByteArraySpan;
  using Impl::Elf::FileHeader;
  using Impl::Elf::ObjectFileType;
  using Impl::Elf::extractFileHeader;

  const int64_t size = mImpl->minimumSizeToReadFileHeader();

  if( fileSize() < size ){
    return false;
  }

  const ByteArraySpan map = mapIfRequired(0, size);

  const FileHeader fileHeader = extractFileHeader(map);

  if( !fileHeader.seemsValid() ){
    return false;
  }

  if(fileHeader.objectFileType() == ObjectFileType::ExecutableFile){
    return true;
  }
  if(fileHeader.objectFileType() == ObjectFileType::SharedObject){
    return true;
  }

  return false;
}

bool ElfFileIoEngine::doContainsDebugSymbols()
{
  using Impl::ByteArraySpan;

  const qint64 size = fileSize();

  const ByteArraySpan map = mapIfRequired(0, size);

  return mImpl->containsDebugSymbols(map);
}

QStringList ElfFileIoEngine::doGetNeededSharedLibraries()
{
  using Impl::ByteArraySpan;

  const qint64 size = fileSize();

  const ByteArraySpan map = mapIfRequired(0, size);

  return mImpl->getNeededSharedLibraries(map);
}

RPath ElfFileIoEngine::doGetRunPath()
{
  using Impl::ByteArraySpan;

  const qint64 size = fileSize();

  const ByteArraySpan map = mapIfRequired(0, size);

  return mImpl->getRunPath(map);
}

void ElfFileIoEngine::doSetRunPath(const RPath & rPath)
{
  using Impl::ByteArraySpan;
  using Impl::Elf::FileWriterFile;

  const qint64 size = fileSize();

  ByteArraySpan map = mapIfRequired(0, size);

  FileWriterFile file;
  connect(&file, &FileWriterFile::message, this, &ElfFileIoEngine::message);
  connect(&file, &FileWriterFile::verboseMessage, this, &ElfFileIoEngine::verboseMessage);

  mImpl->readToFileWriterFile(file, map);

  file.setRunPath( RPathElf::rPathToString(rPath) );

  const qint64 newSize = file.minimumSizeToWriteFile();
  if(newSize > size){
    resizeFile(newSize);
    map = mapIfRequired(0, newSize);
  }

  mImpl->setFileWriterToMap(map, file);
}

}} // namespace Mdt{ namespace DeployUtils{
