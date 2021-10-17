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

    /*! \brief Get the headers
     */
    const FileAllHeaders & headers() const noexcept
    {
      return mHeaders;
    }

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
      }
    }

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

      return mHeaders.dynamicProgramHeader().offset;
    }

    /*! \brief Check if the dynamic section moves to the end of this file
     */
    bool dynamicSectionMovesToEnd() const noexcept
    {
      return mHeaders.dynamicProgramHeader().offset >= mOriginalLayout.globalOffsetRange().end();
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

    /*! \brief Get the file offset range of the original dynamic string table
     */
    OffsetRange originalDynamicStringTableOffsetRange() const noexcept
    {
      return mOriginalLayout.dynamicStringTableOffsetRange();
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

    FileWriterFileLayout mOriginalLayout;
    FileOffsetChanges mFileOffsetChanges;
    FileAllHeaders mHeaders;
    DynamicSection mDynamicSection;
    PartialSymbolTable mSectionSymbolTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_H
