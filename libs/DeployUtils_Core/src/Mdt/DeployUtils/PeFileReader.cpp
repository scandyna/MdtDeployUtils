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

namespace Mdt{ namespace DeployUtils{

PeFileReader::PeFileReader(QObject *parent)
  : AbstractExecutableFileReaderEngine(parent)
{
}

PeFileReader::~PeFileReader() noexcept
{
}

void PeFileReader::sandbox()
{
  using Impl::ByteArraySpan;

  const ByteArraySpan map = mapIfRequired( 0, fileSize() );
  Impl::Pe::sandbox(map);
}

void PeFileReader::newFileOpen(const QString & fileName)
{
}

void PeFileReader::fileClosed()
{
}

bool PeFileReader::doIsPeFile()
{
}

bool PeFileReader::doIsExecutableOrSharedLibrary()
{
}

QStringList PeFileReader::doGetNeededSharedLibraries()
{
}

}} // namespace Mdt{ namespace DeployUtils{
