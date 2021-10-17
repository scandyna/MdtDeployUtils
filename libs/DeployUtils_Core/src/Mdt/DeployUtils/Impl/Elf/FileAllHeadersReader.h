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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_READER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_READER_H

#include "FileAllHeaders.h"
#include "FileAllHeadersReaderWriterCommon.h"
#include "FileHeader.h"
#include "FileReader.h"
#include "ProgramHeaderReader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  FileAllHeaders extractAllHeaders(const ByteArraySpan & map, const FileHeader & fileHeader)
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllProgramHeaders() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );

    FileAllHeaders headers;

    headers.setFileHeader(fileHeader);
    headers.setProgramHeaderTable( extractAllProgramHeaders(map, fileHeader) );
    headers.setSectionHeaderTable( extractAllSectionHeaders(map, fileHeader) );

    return headers;
  }
  
}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_READER_H
