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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_H

#include "FileHeader.h"
#include "ProgramHeader.h"
#include "SectionHeader.h"
#include <vector>
#include <cstdint>
#include <limits>
#include <iterator>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class FileAllHeaders
  {
   public:

    /*! \brief Set the file header
     *
     * \pre \a header must be valid
     * \pre the program header table must not allready been set
     * \pre the section header table must not allready been set
     */
    void setFileHeader(const FileHeader & header) noexcept
    {
      assert( header.seemsValid() );
      assert( !containsProgramHeaderTable() );
      assert( !containsSectionHeaderTable() );

      mFileHeader = header;
    }

    /*! \brief Check if the file header seems valid
     */
    bool fileHeaderSeemsValid() const noexcept
    {
      return mFileHeader.seemsValid();
    }

    /*! \brief Get the file header
     */
    const FileHeader & fileHeader() const noexcept
    {
      return mFileHeader;
    }

    /*! \brief Check if the program header table exists
     */
    bool containsProgramHeaderTable() const noexcept
    {
      return !mProgramHeaderTable.empty();
    }

    /*! \brief Set the program header table
     */
    void setProgramHeaderTable(const std::vector<ProgramHeader> & table) noexcept
    {
      assert( table.size() <= std::numeric_limits<uint16_t>::max() );

      mProgramHeaderTable = table;
      mFileHeader.phnum = static_cast<uint16_t>( table.size() );
      setIndexOfDynamicSectionProgramHeader();
    }

    /*! \brief Get the program header table
     */
    const std::vector<ProgramHeader> & programHeaderTable() const noexcept
    {
      return mProgramHeaderTable;
    }

    /*! \brief Check if the section header table exists
     */
    bool containsSectionHeaderTable() const noexcept
    {
      return !mSectionHeaderTable.empty();
    }

    /*! \brief Set the section header table
     */
    void setSectionHeaderTable(const std::vector<SectionHeader> & table) noexcept
    {
      assert( table.size() <= std::numeric_limits<uint16_t>::max() );

      mSectionHeaderTable = table;
      mFileHeader.shnum = static_cast<uint16_t>( table.size() );
      setIndexOfDynamicSectionHeader();
    }

    /*! \brief Get the section header table
     */
    const std::vector<SectionHeader> & sectionHeaderTable() const noexcept
    {
      return mSectionHeaderTable;
    }

    /*! \brief Check if the dynamic program header exists
     */
    bool containsDynamicProgramHeader() const noexcept
    {
      return mIndexOfDynamicSectionProgramHeader >= 0;
    }

    /*! \brief Get the dynamic program header
     *
     * \pre the dynamic program header must exist
     * \sa containsDynamicProgramHeader()
     */
    const ProgramHeader & dynamicProgramHeader() const noexcept
    {
      assert( containsDynamicProgramHeader() );

      const size_t index = mIndexOfDynamicSectionProgramHeader;
      assert( index < mProgramHeaderTable.size() );
      assert( mProgramHeaderTable[index].segmentType() == SegmentType::Dynamic );

      return mProgramHeaderTable[index];
    }

    /*! \brief Check if the dynamic section header exists
     */
    bool containsDynamicSectionHeader() const noexcept
    {
      return mIndexOfDynamicSectionHeader > 0;
    }

    /*! \brief Get the dynamic section header
     *
     * \pre the dynamic section header must exist
     * \sa containsDynamicSectionHeader()
     */
    const SectionHeader & dynamicSectionHeader() const noexcept
    {
      assert( containsDynamicSectionHeader() );

      const size_t index = mIndexOfDynamicSectionHeader;
      assert( index < mSectionHeaderTable.size() );
      assert( mSectionHeaderTable[index].sectionType() == SectionType::Dynamic );

      return mSectionHeaderTable[index];
    }

    /*! \brief Check about validity
     */
    bool seemsValid() const noexcept
    {
      if( !fileHeaderSeemsValid() ){
        return false;
      }
      if( mFileHeader.phnum != mProgramHeaderTable.size() ){
        return false;
      }
      if( mFileHeader.shnum != mSectionHeaderTable.size() ){
        return false;
      }
      if( containsDynamicSectionHeader() && !containsDynamicProgramHeader() ){
        return false;
      }

      return true;
    }

   private:

    void setIndexOfDynamicSectionProgramHeader() noexcept
    {
      const auto pred = [](const ProgramHeader & header){
        return header.segmentType() == SegmentType::Dynamic;
      };

      const auto it = std::find_if(mProgramHeaderTable.cbegin(), mProgramHeaderTable.cend(), pred);
      if( it == mProgramHeaderTable.cend() ){
        mIndexOfDynamicSectionProgramHeader = -1;
      }else{
        mIndexOfDynamicSectionProgramHeader = static_cast<int>( std::distance(mProgramHeaderTable.cbegin(), it) );
      }
    }

    void setIndexOfDynamicSectionHeader() noexcept
    {
      const auto pred = [](const SectionHeader & header){
        return header.sectionType() == SectionType::Dynamic;
      };

      const auto it = std::find_if(mSectionHeaderTable.cbegin(), mSectionHeaderTable.cend(), pred);
      if( it == mSectionHeaderTable.cend() ){
        mIndexOfDynamicSectionHeader = 0;
      }else{
        mIndexOfDynamicSectionHeader = static_cast<int>( std::distance(mSectionHeaderTable.cbegin(), it) );
      }
    }

    int mIndexOfDynamicSectionProgramHeader = -1;
    int mIndexOfDynamicSectionHeader = 0;
    FileHeader mFileHeader;
    std::vector<ProgramHeader> mProgramHeaderTable;
    std::vector<SectionHeader> mSectionHeaderTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_H
