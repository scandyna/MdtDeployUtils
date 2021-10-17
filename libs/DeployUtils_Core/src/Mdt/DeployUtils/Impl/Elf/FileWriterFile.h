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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_H

#include "FileHeader.h"
#include "FileWriterFileLayout.h"
#include "ProgramHeader.h"
#include "ProgramHeaderTable.h"
#include "SectionHeader.h"
#include "OffsetRange.h"
#include "FileAllHeaders.h"
#include "DynamicSection.h"
#include "SymbolTable.h"
#include "FileOffsetChanges.h"
#include "StringTable.h"
#include "Algorithm.h"
#include <QCoreApplication>
#include <QString>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <cassert>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class FileWriterFile
  {
    Q_DECLARE_TR_FUNCTIONS(FileWriterFile)

   public:

    /*! \brief Construct a empty file
     */
    FileWriterFile() noexcept = default;

    /*! \brief Copy construct a file from \a other
     */
    FileWriterFile(const FileWriterFile & other) = default;

    /*! \brief Copy assign \a other to this file
     */
    FileWriterFile & operator=(const FileWriterFile & other) = default;

    /*! \brief Move construct a file from \a other
     */
    FileWriterFile(FileWriterFile && other) noexcept = default;

    /*! \brief Move assign \a other to this file
     */
    FileWriterFile & operator=(FileWriterFile && other) noexcept = default;

    /*! \brief Set the headers
     *
     * \pre \a headers must be valid
     */
    [[deprecated]]
    void setHeaders(const FileAllHeaders & headers) noexcept
    {
      assert( headers.seemsValid() );

      mHeaders = headers;
    }

    /*! \brief Get the headers
     */
    const FileAllHeaders & headers() const noexcept
    {
      return mHeaders;
    }

    /*! \brief Set the file header
     *
     * \pre \a header must be valid
     */
//     void setFileHeader(const FileHeader & header) noexcept
//     {
//       assert( header.seemsValid() );
// 
//       mHeaders.setFileHeader(header);
//     }

    /*! \brief Check if the file header seems valid
     */
//     bool fileHeaderSeemsValid() const noexcept
//     {
//       return mHeaders.fileHeaderSeemsValid();
//     }

    /*! \brief Set the program header table
     */
//     void setProgramHeaderTable(const std::vector<ProgramHeader> & table) noexcept
//     {
//     }

    /*! \brief Set the section header table
     */
//     void setSectionHeaderTable(const std::vector<SectionHeader> & table) noexcept
//     {
//     }

    /*! \brief Get the file header
     */
    const FileHeader & fileHeader() const noexcept
    {
      return mHeaders.fileHeader();
    }

    /*! \brief Get the program header table
     */
    const ProgramHeaderTable & programHeaderTable() const noexcept
    {
      return mHeaders.programHeaderTable();
    }

    /*! \brief Get the section header table
     */
    const std::vector<SectionHeader> & sectionHeaderTable() const noexcept
    {
      return mHeaders.sectionHeaderTable();
    }

    /*! \brief Set the run path (DT_RUNPATH)
     *
     * If \a runPath is a empty string,
     * the run path entry will be removed from the dynamic section.
     */
    void setRunPath(const QString & runPath)
    {
      mDynamicSection.setRunPath(runPath);

      mHeaders.setDynamicSectionFileSize( mDynamicSection.byteCount(fileHeader().ident._class) );
      mHeaders.setDynamicStringTableFileSize( mDynamicSection.stringTable().byteCount() );

      /*
       * If we move .dynstr:
       *
       * EOF (maybe section header table)
       * program header table
       * .dynstr section
       *
       * PT_PHDR segment must cover the program header table
       * PT_LOAD new segment that covers the program header table and .dynstr
       *
       *
       * If we move .dynamic:
       *
       * EOF (maybe section header table)
       * program header table
       * .dynamic section
       *
       * PT_PHDR segment must cover the program header table
       * PT_LOAD new segment that covers the program header table and .dynamic
       * PT_DYNAMIC segment must cover .dynamic
       *
       *
       * If we move both .dynamic and .dynstr:
       *
       * EOF (maybe section header table)
       * program header table
       * .dynamic section
       * .dynstr section
       *
       * PT_PHDR segment must cover the program header table
       * PT_LOAD new segment that covers the program header table, .dynamic and .dynstr
       * PT_DYNAMIC segment must cover .dynamic
       */

      const bool mustMoveDynamicSection = mFileOffsetChanges.dynamicSectionChangesOffset(mDynamicSection) > 0;
      const bool mustMoveDynamicStringTable = mFileOffsetChanges.dynamicStringTableChangesOffset(mDynamicSection) > 0;
      const bool mustMoveAnySection = mustMoveDynamicSection || mustMoveDynamicStringTable;

      /*
       * If we move any section to the end,
       * a new PT_LOAD segment must be created.
       * Because of this, the program header table also moves to the end.
       * This new load segment covers also the program header table.
       * This segment should also be aligned in memory
       */
      
      /** \todo Sandbox
       */
//       mHeaders.moveProgramHeaderTableToNextPageAfterEnd();
//       ProgramHeader & loadSegmentHeader = mHeaders.appendNullLoadSegment();
//       loadSegmentHeader.setPermissions(SegmentPermission::Read);
//       loadSegmentHeader.vaddr = mHeaders.programHeaderTableProgramHeader().vaddr;
//       loadSegmentHeader.paddr = loadSegmentHeader.vaddr;
//       loadSegmentHeader.align = mHeaders.fileHeader().pageSize();
// //       loadSegmentHeader.align = 0x200000;
//       loadSegmentHeader.offset = mHeaders.programHeaderTableProgramHeader().offset;
//       loadSegmentHeader.memsz = mHeaders.programHeaderTableProgramHeader().memsz;
//       loadSegmentHeader.filesz = mHeaders.programHeaderTableProgramHeader().filesz;
// //       loadSegmentHeader.filesz = 0x1000;

      
      if(mustMoveAnySection){

        mHeaders.moveProgramHeaderTableToNextPageAfterEnd();

        const QString msg = tr("the new rpath is to long, will have to move sections to the end.");
        /** \todo emit a warning (that will be used by the message logger)
         * Will have to do it in a other place, because we cannot inherit QObject here
         */
        qDebug() << msg;

        ProgramHeader & loadSegmentHeader = mHeaders.appendNullLoadSegment();
        ///loadSegmentHeader.setPermissions(SegmentPermission::Read | SegmentPermission::Write);
        ///loadSegmentHeader.setPermissions(SegmentPermission::Read | SegmentPermission::Execute);
        loadSegmentHeader.setPermissions(SegmentPermission::Read);
        loadSegmentHeader.vaddr = mHeaders.programHeaderTableProgramHeader().vaddr;
        loadSegmentHeader.paddr = loadSegmentHeader.vaddr;
        loadSegmentHeader.align = mHeaders.fileHeader().pageSize();
        loadSegmentHeader.offset = mHeaders.programHeaderTableProgramHeader().offset;

        uint64_t loadSegmentSize = mHeaders.programHeaderTableProgramHeader().memsz;

        if(mustMoveDynamicSection){
          mHeaders.moveDynamicSectionToEnd();
          mSectionSymbolTable.setDynamicSectionVirtualAddress( mHeaders.dynamicSectionHeader().addr );
          loadSegmentSize += mHeaders.dynamicProgramHeader().memsz;
        }

        if(mustMoveDynamicStringTable){
          mHeaders.moveDynamicStringTableToEnd();
          mDynamicSection.setStringTableAddress( mHeaders.dynamicStringTableSectionHeader().addr );
          mSectionSymbolTable.setDynamicStringTableVirtualAddress( mHeaders.dynamicStringTableSectionHeader().addr );
          loadSegmentSize += mHeaders.dynamicStringTableSectionHeader().size;
        }
        
        loadSegmentHeader.memsz = loadSegmentSize;
        loadSegmentHeader.filesz = loadSegmentHeader.memsz;
        
//         const uint64_t pageSize = 0x1000; /// \todo get from header !!!
//         const uint64_t virtualAddess = findAddressOfNextPage(mHeaders.findLastSegmentVirtualAddressEnd(), pageSize);
//         const uint64_t fileOffset = findNextFileOffset(mHeaders.findGlobalFileOffsetEnd(), virtualAddess, pageSize);
        
        
        
        
//         loadSegmentHeader.setSegmentType(SegmentType::Load);
//         loadSegmentHeader.offset = fileOffset;
//         loadSegmentHeader.vaddr = virtualAddess;
//         loadSegmentHeader.paddr = virtualAddess;
//         ///loadSegmentHeader.filesz = ??
//         ///loadSegmentHeader.memsz = ??
//         loadSegmentHeader.setPermissions(SegmentPermission::Read | SegmentPermission::Write);
//         loadSegmentHeader.align = pageSize;
        
      }
      
//       if(  ){
//         mHeaders.moveDynamicSectionToEnd();
//       }

//       if(  ){
//         mHeaders.moveDynamicStringTableToEnd();
//       }

      
      /** \todo If we have to move a section to the end:
       *
       * - 
       * 
       * - the first section that is moved to the end must be at the next page after end of virtual memory of this file
       * - the next section must be just after in virtual memory, but start at power of 2
       *   Note: align constraint ?
       * - calcualte file offsets for the moved segments, past this file, ragrding virtual addresses
       * - create a load segment that covers moved section(s) and program header table \todo see PT_PHDR
       * - move program header table to the end of the file (yes.., no choice)
       */

//       if( dynamicSectionIsAfterDynamicStringTable() ){
//         /*
//          * We have .dynstr then .dynamic
//          */
//         mHeaders.shiftProgramHeadersOffsetAfter( mHeaders.dynamicStringTableSectionHeader().offset, dynamicStringTableChangesOffset() );
//         mHeaders.shiftSectionHeadersOffsetAfter( mHeaders.dynamicStringTableSectionHeader(), dynamicStringTableChangesOffset() );
//         
//         mHeaders.shiftProgramHeadersOffsetAfter( mHeaders.dynamicProgramHeader(), dynamicSectionChangesOffset() );
//         mHeaders.shiftSectionHeadersOffsetAfter( mHeaders.dynamicSectionHeader(), dynamicSectionChangesOffset() );
//       }else{
//         /*
//          * We have .dynamic then .dynstr
//          */
// //         const uint64_t newOffsetAfterDynamicSection = mOriginalLayout.dynamicStringTableOffset() + dynamicSectionChangesOffset();
//         mHeaders.shiftProgramHeadersOffsetAfter( dynamicProgramHeader(), dynamicSectionChangesOffset() );
//         mHeaders.shiftSectionHeadersOffsetAfter( dynamicSectionHeader(), dynamicSectionChangesOffset() );
// //         mDynamicSection.setStringTableAddress(newOffsetAfterDynamicSection);
//         
// //         const uint64_t newOffsetAfterDynamicStringTable = mOriginalLayout.dynamicStringTableOffset() + dynamicStringTableChangesOffset();
//         mHeaders.shiftProgramHeadersOffsetAfter( mHeaders.dynamicStringTableSectionHeader().offset, dynamicStringTableChangesOffset() );
//         mHeaders.shiftSectionHeadersOffsetAfter( mHeaders.dynamicStringTableSectionHeader(), dynamicStringTableChangesOffset() );
// 
// //         _dynamicStringTableSectionHeader.offset = newOffset;
//       }

//       mDynamicSection.setStringTableAddress( mHeaders.dynamicStringTableSectionHeader().offset );

      /// \todo use OffsetChange
      
//       if( sectionHeaderTableIsAfterDynamicSectionAndStringTable() ){
//         const uint64_t newOffset = mHeaders.fileHeader().shoff + globalChangesOffset();
//         mHeaders.setSectionHeaderTableOffset(newOffset);
//       }

      /// \todo wrong
//       if( sectionNameStringTableIsAfterDynamicSectionAndStringTable() ){
//         const uint64_t newOffset = mHeaders.sectionNameStringTableHeader().offset + globalChangesOffset();
//         mHeaders.setSectionNameStringTableOffset(newOffset);
//       }
    }

    /*! \brief Set the dynamic section and its related string table
     *
     * Because current implementation does not support creating
     * correct program and section headers,
     * those must be readen from the file first and be set before.
     *
     * \todo review update
     * 
     * Returns the global file size change required after setting \a section .
     *
     * For example, if this file currently not contains the DT_RUNPATH,
     * and \a section contains it with the string \a /tmp ,
     * 21 will be returned for 64-bit architecture
     * (16 bytes for the new DT_RUNPATH entry
     *  + 5+1 bytes for the \a /tmp string and the new null terminator).
     *
     * The returned value can also be < 0, if the string shrinks,
     * or the a entry was removed from \a section .
     *
     * \pre \a section must not be null
     * \pre this file must contain the dynamic program header
     * \pre this file must contain the dynamic section header
     *
     * \todo should not return any value here
     */
//     [[deprecated]]
//     int64_t setDynamicSectionAndStringTable(const DynamicSection & section) noexcept
//     {
//       assert( !section.isNull() );
//       assert( fileHeader().seemsValid() );
//       assert( mHeaders.containsDynamicProgramHeader() );
//       assert( mHeaders.containsDynamicSectionHeader() );
//       assert( mHeaders.containsDynamicStringTableSectionHeader() );
// 
//       FileOffsetChanges offsetChanges;
//       offsetChanges.setOriginalSizes(mDynamicSection, fileHeader().ident._class);
// 
//       mDynamicSection = section;
// 
//       const uint64_t dynamicSectionSize = mDynamicSection.byteCount(fileHeader().ident._class);
// 
//       ProgramHeader _dynamicProgramHeader = dynamicProgramHeader();
//       _dynamicProgramHeader.filesz = dynamicSectionSize;
//       mHeaders.setDynamicProgramHeader(_dynamicProgramHeader);
// 
//       SectionHeader _dynamicSectionHeader = dynamicSectionHeader();
//       _dynamicSectionHeader.size = dynamicSectionSize;
//       mHeaders.setDynamicSectionHeader(_dynamicSectionHeader);
// 
//       SectionHeader _dynamicStringTableSectionHeader = mHeaders.dynamicStringTableSectionHeader();
//       _dynamicStringTableSectionHeader.size = mDynamicSection.stringTable().byteCount();
//       mHeaders.setDynamicStringTableSectionHeader(_dynamicStringTableSectionHeader);
// 
//       return offsetChanges.globalChangesOffset(mDynamicSection);
//     }

    /*! \brief Check if this file has the dynamic section
     */
    bool containsDynamicSection() const noexcept
    {
      return !mDynamicSection.isNull();
    }

    /*! \brief Get the dynamic section
     */
    const DynamicSection & dynamicSection() const noexcept
    {
      return mDynamicSection;
    }

    /*! \brief Get the dynamic program header
     *
     * \pre this file must have the dynamic section
     */
    const ProgramHeader & dynamicProgramHeader() const noexcept
    {
      assert( containsDynamicSection() );
      assert( mHeaders.containsDynamicProgramHeader() );

      return mHeaders.dynamicProgramHeader();
      
//       assert( mDynamicSection.indexOfProgramHeader() < mHeaders.programHeaderTable().size() );
//       assert( mHeaders.programHeaderTable()[mDynamicSection.indexOfProgramHeader()].segmentType() == SegmentType::Dynamic );
// 
//       return mHeaders.programHeaderTable()[mDynamicSection.indexOfProgramHeader()];
    }

    /*! \brief Get the dynamic section header
     *
     * \pre this file must have the dynamic section
     */
    const SectionHeader & dynamicSectionHeader() const noexcept
    {
      assert( containsDynamicSection() );
      assert( mHeaders.containsDynamicSectionHeader() );

      return mHeaders.dynamicSectionHeader();
      
//       assert( mDynamicSection.indexOfSectionHeader() < mHeaders.sectionHeaderTable().size() );
//       assert( mHeaders.sectionHeaderTable()[mDynamicSection.indexOfSectionHeader()].sectionType() == SectionType::Dynamic );
// 
//       return mHeaders.sectionHeaderTable()[mDynamicSection.indexOfSectionHeader()];
    }

    /*! \brief Check if the dynamic string table section header exists
     */
    bool containsDynamicStringTableSectionHeader() const noexcept
    {
      return mHeaders.containsDynamicStringTableSectionHeader();
    }

    /*! \brief Get the dynamic string table section header
     *
     * \pre the dynamic string table section header must exist
     * \sa containsDynamicStringTableSectionHeader()
     */
    const SectionHeader & dynamicStringTableSectionHeader() const noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      return mHeaders.dynamicStringTableSectionHeader();
    }

    /*! \brief Set the section symbol table from file (from .symtab)
     */
    void setSectionSymbolTableFromFile(const PartialSymbolTable & table) noexcept
    {
      mSectionSymbolTable = table;
    }

    /*! \brief Get the section symbol table (from .symtab)
     */
    const PartialSymbolTable & sectionSymbolTable() const noexcept
    {
      return mSectionSymbolTable;
    }

    /*! \brief Set the section symbol table from file (from .dynsym)
     */
//     void setSectionDynamicSymbolTableFromFile(const PartialSymbolTable & table) noexcept
//     {
//       mSectionDynamicSymbolTable = table;
//     }

    /*! \brief Get the section symbol table (from .dynsym)
     */
//     const PartialSymbolTable & sectionDynamicSymbolTable() const noexcept
//     {
//       return mSectionDynamicSymbolTable;
//     }

    /*! \brief Get the file offsets range for this file
     */
//     OffsetRange globalOffsetRange() const noexcept
//     {
//       uint64_t dynamicSectionEnd = 0;
//       if( mHeaders.containsDynamicProgramHeader() ){
//         dynamicSectionEnd = mHeaders.dynamicProgramHeader().offset + mHeaders.dynamicProgramHeader().filesz;
//       }
// 
//       uint64_t dynamicStringTableEnd = 0;
//       if( mHeaders.containsDynamicStringTableSectionHeader() ){
//         dynamicStringTableEnd = mHeaders.dynamicStringTableSectionHeader().offset + mHeaders.dynamicStringTableSectionHeader().size;
//       }
// 
//       uint64_t lastHeaderEnd = static_cast<uint64_t>( mHeaders.minimumSizeToAccessAllHeaders() );
// 
//       uint64_t fileEnd = std::max({dynamicSectionEnd, dynamicStringTableEnd, lastHeaderEnd});
// 
//       return OffsetRange::fromBeginAndEndOffsets(0, fileEnd);
//     }

    /*! \brief Get the minimum size required to write this file
     */
    int64_t minimumSizeToWriteFile() const noexcept
    {
      return mHeaders.globalFileOffsetRange().minimumSizeToAccessRange();
    }

    /*! \brief Get the file offset of the dynamic section
     *
     * \pre this file must contain the dynamic section program header
     */
    uint64_t dynamicSectionOffset() const noexcept
    {
      assert( mHeaders.containsDynamicProgramHeader() );

      return dynamicProgramHeader().offset;
    }

    /*! \brief Get the size of the dynamic section
     *
     * \pre this file must contain the dynamic section program header
     */
    uint64_t dynamicSectionSize() const noexcept
    {
      assert( mHeaders.containsDynamicProgramHeader() );

      return dynamicProgramHeader().filesz;
    }

    /*! \brief Check if the dynamic section moves to the end of this file
     */
    bool dynamicSectionMovesToEnd() const noexcept
    {
      return dynamicProgramHeader().offset >= mOriginalLayout.globalOffsetRange().end();
    }

    /*! \brief Check if the dynamic string table moves to the end of this file
     */
    bool dynamicStringTableMovesToEnd() const noexcept
    {
      return dynamicStringTableSectionHeader().offset >= mOriginalLayout.globalOffsetRange().end();
    }

    /*! \brief Get the file offset range of the dynamic string table
     *
     * \pre this file must have the dynamic string table section header
     */
    OffsetRange dynamicStringTableOffsetRange() const noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      return OffsetRange::fromSectionHeader( dynamicStringTableSectionHeader() );
    }

    /*! \brief Get the file offset of the dynamic string table
     *
     * \pre this file must have the dynamic string table section header
     */
    uint64_t dynamicStringTableOffset() const noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      return dynamicStringTableSectionHeader().offset;
    }

    /*! \brief Get the size of the dynamic string table
     *
     * \pre this file must have the dynamic string table section header
     */
    uint64_t dynamicStringTableSize() const noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      return dynamicStringTableSectionHeader().size;
    }

    /*! \brief Check if the dynamic section is after the dynamic string table
     */
    [[deprecated]]
    bool dynamicSectionIsAfterDynamicStringTable() const noexcept
    {
      return dynamicSectionOffset() > dynamicStringTableOffset();
    }

    /*! \brief Get the end offset of the original dynamic section
     */
//     uint64_t originalDynamicSectionEndOffset() const noexcept
//     {
//       return mOriginalLayout.dynamicSectionEndOffset();
//     }

    /*! \brief Get the file offset range of the original dynamic string table
     */
    OffsetRange originalDynamicStringTableOffsetRange() const noexcept
    {
      return mOriginalLayout.dynamicStringTableOffsetRange();
    }

    /*! \brief Get the end offset of the original dynamic string table
     */
//     uint64_t originalDynamicStringTableEndOffset() const noexcept
//     {
//       return mOriginalLayout.dynamicStringTableEndOffset();
//     }

    /*! \brief Get the offset of the changes applyed to the dynamic section (in bytes)
     *
     * \sa FileOffsetChanges::dynamicSectionChangesOffset()
     */
    int64_t dynamicSectionChangesOffset() const noexcept
    {
      return mFileOffsetChanges.dynamicSectionChangesOffset(mDynamicSection);
    }

    /*! \brief Get the offset of changes applyed to the dynamic string table related (in bytes)
     *
     * \sa FileOffsetChanges::dynamicStringTableChangesOffset()
     */
    int64_t dynamicStringTableChangesOffset() const noexcept
    {
      return mFileOffsetChanges.dynamicStringTableChangesOffset(mDynamicSection);
    }

    /*! \brief Get the offset of changes applyed to the dynamic section and its related string table
     *
     * \sa FileOffsetChanges::globalChangesOffset()
     */
    int64_t globalChangesOffset() const noexcept
    {
      return mFileOffsetChanges.globalChangesOffset(mDynamicSection);
    }

    /*! \brief Check if this file semms valid
     */
    bool seemsValid() const noexcept
    {
      if( !mHeaders.seemsValid() ){
        return false;
      }

      return true;
    }

    /*! \brief Get a file writer file from the readen file
     *
     * \pre \a headers must be valid
     * \pre \a dynamicSection must not be null
     * \pre \a headers must contain the dynamic program header
     * \pre \a headers must contain the dynamic section header
     */
    static
    FileWriterFile fromOriginalFile(const FileAllHeaders & headers, const DynamicSection & dynamicSection) noexcept
    {
      assert( headers.seemsValid() );
      assert( !dynamicSection.isNull() );
      assert( headers.containsDynamicProgramHeader() );
      assert( headers.containsDynamicSectionHeader() );
      assert( headers.containsDynamicStringTableSectionHeader() );

      return FileWriterFile(headers, dynamicSection);
    }

   private:

    FileWriterFile(const FileAllHeaders & headers, const DynamicSection & dynamicSection)
     : mHeaders(headers),
       mDynamicSection(dynamicSection)
    {
      mOriginalLayout = FileWriterFileLayout::fromFile(headers, dynamicSection);
      mFileOffsetChanges.setOriginalSizes(dynamicSection, headers.fileHeader().ident._class);
    }

    [[deprecated]]
    bool sectionHeaderTableIsAfterDynamicSectionAndStringTable() const noexcept
    {
      const uint64_t sectionHeaderTableOffset = fileHeader().shoff;

      return ( sectionHeaderTableOffset > dynamicSectionOffset() )
          && ( sectionHeaderTableOffset > dynamicStringTableOffset() );
    }

    [[deprecated]]
    bool sectionNameStringTableIsAfterDynamicSectionAndStringTable() const noexcept
    {
      const uint64_t sectionNameStringTableOffset = mHeaders.sectionNameStringTableHeader().offset;

      return ( sectionNameStringTableOffset > dynamicSectionOffset() )
          && ( sectionNameStringTableOffset > dynamicStringTableOffset() );
    }

    FileWriterFileLayout mOriginalLayout;
    FileOffsetChanges mFileOffsetChanges;
    FileAllHeaders mHeaders;
    DynamicSection mDynamicSection;
    PartialSymbolTable mSectionSymbolTable;
//     PartialSymbolTable mSectionDynamicSymbolTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_H
