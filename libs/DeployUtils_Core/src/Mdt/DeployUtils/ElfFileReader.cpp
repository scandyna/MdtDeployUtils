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
#include "ElfFileReader.h"
#include "Mdt/DeployUtils/Impl/Elf/FileReader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

ElfFileReader::ElfFileReader(QObject *parent)
  : AbstractExecutableFileReaderEngine(parent),
    mImpl( std::make_unique<Impl::Elf::FileReader>() )
{
}

ElfFileReader::~ElfFileReader() noexcept
{
}

QString ElfFileReader::getSoName()
{
  using Impl::ByteArraySpan;

  assert( isOpen() );
  assert( isExecutableOrSharedLibrary() );

  const qint64 size = fileSize();

  const ByteArraySpan map = mapIfRequired(0, size);

  return mImpl->getSoName(map);
}

void ElfFileReader::newFileOpen(const QString & fileName)
{
  mImpl->setFileName(fileName);
}

void ElfFileReader::fileClosed()
{
  mImpl->clear();
}

bool ElfFileReader::doSupportsPlatform(const Platform & platform) const noexcept
{
  return platform.executableFileFormat() == ExecutableFileFormat::Elf;
}

bool ElfFileReader::doIsElfFile()
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

Platform ElfFileReader::doGetFilePlatform()
{
  /// \todo implement
  /// \todo should make a common function to get Ident ?
  return Platform();
}

bool ElfFileReader::doIsExecutableOrSharedLibrary()
{
  using Impl::ByteArraySpan;
  using Impl::Elf::FileHeader;
  using Impl::Elf::ObjectFileType;
  using Impl::Elf::extractFileHeader;

  const qint64 size = 64;

  if( fileSize() < size ){
    return false;
  }

  const ByteArraySpan map = mapIfRequired(0, size);

  const FileHeader fileHeader = extractFileHeader(map);

  if( !fileHeader.seemsValid() ){
    return false;
  }

  if(fileHeader.type == ObjectFileType::ExecutableFile){
    return true;
  }
  if(fileHeader.type == ObjectFileType::SharedObject){
    return true;
  }

  return false;
}

QStringList ElfFileReader::doGetNeededSharedLibraries()
{
  using Impl::ByteArraySpan;

  const qint64 size = fileSize();

  const ByteArraySpan map = mapIfRequired(0, size);

  return mImpl->getNeededSharedLibraries(map);
}

QStringList ElfFileReader::doGetRunPath()
{
  using Impl::ByteArraySpan;

  const qint64 size = fileSize();

  const ByteArraySpan map = mapIfRequired(0, size);

  return mImpl->getRunPath(map);
}

}} // namespace Mdt{ namespace DeployUtils{
