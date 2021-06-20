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
#include "PeFileReader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/Pe/FileReader.h"
#include <cstdint>

namespace Mdt{ namespace DeployUtils{

PeFileReader::PeFileReader(QObject *parent)
  : AbstractExecutableFileReaderEngine(parent),
    mImpl( std::make_unique<Impl::Pe::FileReader>() )
{
}

PeFileReader::~PeFileReader() noexcept
{
}

void PeFileReader::newFileOpen(const QString & fileName)
{
  mImpl->setFileName(fileName);
}

void PeFileReader::fileClosed()
{
  mImpl->clear();
}

bool PeFileReader::doIsPeImageFile()
{
  return tryExtractDosCoffAndOptionalHeader();
}

bool PeFileReader::doIsExecutableOrSharedLibrary()
{
  if( !tryExtractDosCoffAndOptionalHeader() ){
    return false;
  }
  if( !mImpl->isValidExecutableImage() ){
    return false;
  }

  return true;
}

QStringList PeFileReader::doGetNeededSharedLibraries()
{
  using Impl::ByteArraySpan;

  const qint64 size = fileSize();

  const ByteArraySpan map = mapIfRequired(0, size);

  return mImpl->getNeededSharedLibraries(map);
}

bool PeFileReader::tryExtractDosCoffAndOptionalHeader()
{
  using Impl::ByteArraySpan;

  int64_t size = 64;
  if( fileSize() < size ){
    return false;
  }

  ByteArraySpan map = mapIfRequired(0, size);
  if( !mImpl->tryExtractDosHeader(map) ){
    return false;
  }

  size = mImpl->minimumSizeToExtractCoffHeader();
  if( fileSize() < size ){
    return false;
  }

  map = mapIfRequired(0, size);
  if( !mImpl->tryExtractCoffHeader(map) ){
    return false;
  }

  size = mImpl->minimumSizeToExtractOptionalHeader();
  if( fileSize() < size ){
    return false;
  }

  map = mapIfRequired(0, size);
  if( !mImpl->tryExtractOptionalHeader(map) ){
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
