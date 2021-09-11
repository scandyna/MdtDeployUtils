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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_H

#include "FileHeader.h"
#include "FileHeaderWriter.h"
#include "ProgramHeader.h"
#include "ProgramHeaderWriter.h"
#include "SectionHeader.h"
#include "SectionHeaderWriter.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <QtEndian>
#include <cstdint>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal Check that the count of headers matches the one set in \a fileHeader
   */
  inline
  bool fileHeaderMatchesHeadersCounts(const FileHeader & fileHeader,
                                      const std::vector<ProgramHeader> & programHeaders,
                                      const std::vector<SectionHeader> & sectionHeaders) noexcept
  {
    assert( fileHeader.seemsValid() );

    if( !fileHeaderMatchesProgramHeadersCounts(fileHeader, programHeaders) ){
      return false;
    }
    if( !fileHeaderMatchesSectionHeadersCounts(fileHeader, sectionHeaders) ){
      return false;
    }
//     if( fileHeader.shnum != sectionHeaders.size() ){
//       return false;
//     }

    return true;
  }

  /*! \internal
   */
  inline
  bool mapIsBigEnoughToSetHeaders(const ByteArraySpan & map, const FileHeader & fileHeader,
                                  const std::vector<ProgramHeader> & programHeaders,
                                  const std::vector<SectionHeader> & sectionHeaders) noexcept
  {
    assert( fileHeader.seemsValid() );
    assert( fileHeaderMatchesHeadersCounts(fileHeader, programHeaders, sectionHeaders) );

    if( map.size < fileHeader.minimumSizeToReadAllProgramHeaders() ){
      return false;
    }
    if( map.size < fileHeader.minimumSizeToReadAllSectionHeaders() ){
      return false;
    }

    return true;
  }

  /*! \internal
   *
   * \todo precondition that the headres are correct ?
   */
  inline
  void setHeadersToMap(ByteArraySpan map, const FileHeader & fileHeader,
                                  const std::vector<ProgramHeader> & programHeaders,
                                  const std::vector<SectionHeader> & sectionHeaders)
  {
    assert( fileHeader.seemsValid() );
    assert( fileHeaderMatchesHeadersCounts(fileHeader, programHeaders, sectionHeaders) );
    assert( mapIsBigEnoughToSetHeaders(map, fileHeader, programHeaders, sectionHeaders) );

    fileHeaderToArray(map, fileHeader);
    setProgramHeadersToMap(map, programHeaders, fileHeader);
    setSectionHeadersToMap(map, sectionHeaders, fileHeader);
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_H
