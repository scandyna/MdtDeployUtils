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
  : QObject(parent),
    mImpl( std::make_unique<Impl::Elf::FileReader>() )
{
}

ElfFileReader::~ElfFileReader() noexcept
{
}

void ElfFileReader::openFile(const QFileInfo & fileInfo)
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

  mImpl->setFileName( mFile.fileName() );
}

void ElfFileReader::close()
{
  mFileMapper.unmap(mFile);
  mFile.close();
  mImpl->clear();
}

bool ElfFileReader::isElfFile()
{
  using Impl::ByteArraySpan;
  using Impl::Elf::Ident;
  using Impl::Elf::extractIdent;

  assert( isOpen() );

  const qint64 size = 16;

  if( mFile.size() < size ){
    return false;
  }

  const ByteArraySpan map = mFileMapper.mapIfRequired(mFile, 0, size);

  const Ident ident = extractIdent(map);

  return ident.isValid();
}

bool ElfFileReader::isDynamicLinkedExecutableOrLibrary()
{
  using Impl::ByteArraySpan;
  using Impl::Elf::FileHeader;
  using Impl::Elf::ObjectFileType;
  using Impl::Elf::extractFileHeader;

  assert( isOpen() );

  const qint64 size = 64;

  if( mFile.size() < size ){
    return false;
  }

  const ByteArraySpan map = mFileMapper.mapIfRequired(mFile, 0, size);

  const FileHeader fileHeader = extractFileHeader(map);

  if( !fileHeader.seemsValid() ){
    return false;
  }

  return fileHeader.type == ObjectFileType::SharedObject;
}

QString ElfFileReader::getSoName()
{
  using Impl::ByteArraySpan;

  assert( isOpen() );

  const qint64 size = mFile.size();

  mImpl->checkFileSizeToReadSectionHeaders(size);

  const ByteArraySpan map = mFileMapper.mapIfRequired(mFile, 0, size);

  return mImpl->getSoName(map);
}

QStringList ElfFileReader::getNeededSharedLibraries()
{
  using Impl::ByteArraySpan;

  assert( isOpen() );
  assert( isDynamicLinkedExecutableOrLibrary() );

  const qint64 size = mFile.size();

  mImpl->checkFileSizeToReadSectionHeaders(size);

  const ByteArraySpan map = mFileMapper.mapIfRequired(mFile, 0, size);

  return mImpl->getNeededSharedLibraries(map);
}

QStringList ElfFileReader::getRunPath()
{
  using Impl::ByteArraySpan;

  assert( isOpen() );
  assert( isDynamicLinkedExecutableOrLibrary() );

  const qint64 size = mFile.size();

  mImpl->checkFileSizeToReadSectionHeaders(size);

  const ByteArraySpan map = mFileMapper.mapIfRequired(mFile, 0, size);

  return mImpl->getRunPath(map);
}

bool ElfFileReader::isElfFile(const QString & filePath)
{
  assert( !filePath.trimmed().isEmpty() );

  ElfFileReader reader;

  reader.openFile(filePath);

  return reader.isElfFile();
}

void ElfFileReader::sandbox(const QString & filePath)
{
  assert( !filePath.trimmed().isEmpty() );

  Impl::Elf::FileReader reader;

  reader.openFile(filePath);
  reader.sandbox();
}

}} // namespace Mdt{ namespace DeployUtils{
