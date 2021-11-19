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
#include "SectionIndexChangeMap.h"
#include "SectionSegmentUtils.h"
#include "OffsetRange.h"
#include "FileAllHeaders.h"
#include "DynamicSection.h"
#include "SymbolTable.h"
#include "GlobalOffsetTable.h"
#include "ProgramInterpreterSection.h"
#include "GnuHashTable.h"
#include "NoteSectionTable.h"
#include "FileOffsetChanges.h"
#include "StringTable.h"
#include "Algorithm.h"
#include "Exceptions.h"
#include "Mdt/DeployUtils/Algorithm.h"
#include <QCoreApplication>
#include <QString>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <cassert>

#include <QDebug>
#include <iostream>

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

      mHeaders.setDynamicSectionSize( mDynamicSection.byteCount(fileHeader().ident._class) );
      mHeaders.setDynamicStringTableSize( mDynamicSection.stringTable().byteCount() );

      const bool mustMoveDynamicSection = mFileOffsetChanges.dynamicSectionChangesOffset(mDynamicSection) > 0;
      const bool mustMoveDynamicStringTable = mFileOffsetChanges.dynamicStringTableChangesOffset(mDynamicSection) > 0;
      const bool mustMoveAnySection = mustMoveDynamicSection || mustMoveDynamicStringTable;

      if(!mustMoveAnySection){
        return;
      }

      /*
       * If either the .dynstr and/or the .dynamic section grows,
       * we have to put them at the end of the file
       * (shifting all the data after those sections is not a option,
       * because this will inavlidate references we don't know
       * how to handle. We are not a linker).
       *
       * Also, the .dynstr and .dynamic must be
       * covered by a load segment (PT_LOAD).
       * For this, a new entry must be added in the program header table.
       * For this, we have to do some place after this table.
       *
       * Also putting the program header table at the end causes problems.
       * In my case, on a Ubuntu 18.04, the resulting program allways crashed
       * while glibc (2.27) parses the program header table,
       * at rtld.c:1148.
       * This could be worked around for gcc generated executables,
       * which are shared object (DYN).
       * For Clang generated executables (EXEC), this did not work.
       * See also:
       * - https://lwn.net/Articles/631631/
       * - https://github.com/NixOS/patchelf/blob/master/BUGS
       * - https://github.com/NixOS/patchelf/pull/117
       *
       * Try to make place just after the program header table,
       * so we can add the new load segment.
       * On x86-64, a entry is 56-bytes long.
       *
       * Looking at generated executables, the first sections that came
       * just after the program header table are .interp (28-bytes)
       * and .note.ABI-tag (32-bytes).
       *
       * A other note section could also follows: .note.gnu.build-id
       * Because the PT_NOTE segment must cover all note sections,
       * we have to move them all.
       *
       * As example, if we move the .dynamic and .dynstr,
       * we would end up with soemthing like this:
       *
       * EOF (maybe section header table)
       * .interp section
       * .note.ABI-tag section
       * .note.gnu.build-id
       * .dynamic section
       * .dynstr section
       *
       * PT_PHDR segment must cover the program header table (new size)
       * PT_INTERP segment must cover .interp
       * PT_LOAD new segment that covers .interp , .note.ABI-tag, .note.gnu.build-id , .dynamic and .dynstr
       * PT_DYNAMIC segment must cover .dynamic
       * PT_GNU_RELRO segment must be extended to also cover the .dynamic section
       * PT_NOTE segment must cover .note.ABI-tag and .note.gnu.build-id
       */

      assert(mustMoveAnySection);

      /*
       * We need to add a new PT_LOAD to the program header table.
       * For that, we need to move first sections to the end.
       */

      const SectionIndexChangeMap sectionIndexChangeMap = mHeaders.sortSectionHeaderTableByFileOffset();
      /*
       * Sorting the section header table changes the index of some headers.
       * We have to update parts, like symbol tables,
       * that references indexes in the section header table.
       */
      mSymTab.updateSectionIndexes(sectionIndexChangeMap);
      mDynSym.updateSectionIndexes(sectionIndexChangeMap);

      const uint16_t sectionToMoveCount = findCountOfSectionsToMoveToFreeSize(mHeaders.sectionHeaderTable(), fileHeader().phentsize);
      if( sectionToMoveCount >= mHeaders.sectionHeaderTable().size() ){
        const QString msg = tr("should move %1 sections, but file contains only %2 sections")
                            .arg(sectionToMoveCount).arg( mHeaders.sectionHeaderTable().size() );
        throw MoveSectionError(msg);
      }

      std::cout << "required place: " << fileHeader().phentsize << " , sections to move (includes SHT_NULL): " << sectionToMoveCount << std::endl;

      /// If zero -> good :)

      std::vector<uint16_t> movedSectionHeadersIndexes;

      if(sectionToMoveCount > 1){
        movedSectionHeadersIndexes = moveFirstCountSectionsToEnd(sectionToMoveCount);
      }

      std::cout << "offset end: " << mHeaders.findGlobalFileOffsetEnd() << " , Vaddr end: " << mHeaders.findGlobalVirtualAddressEnd() << std::endl;

      if(mustMoveDynamicSection){
        std::cout << "moving .dynamic section to end ..." << std::endl;
        moveDynamicSectionToEnd(MoveSectionAlignment::SectionAlignment);
        movedSectionHeadersIndexes.push_back( mHeaders.dynamicSectionHeaderIndex() );
      }

      if(mustMoveDynamicStringTable){
        std::cout << "moving .dynstr section to end ..." << std::endl;
        moveDynamicStringTableToEnd(MoveSectionAlignment::SectionAlignment);
        movedSectionHeadersIndexes.push_back( mHeaders.dynamicStringTableSectionHeaderIndex() );
      }

      std::cout << "updating symbol tables ..." << std::endl;
      /*
       * Moving sections will change offsets and addresses.
       * We have to update some parts,
       * like symbol tables, that references those addresses.
       */
      mSymTab.updateVirtualAddresses( movedSectionHeadersIndexes, mHeaders.sectionHeaderTable() );
      mDynSym.updateVirtualAddresses( movedSectionHeadersIndexes, mHeaders.sectionHeaderTable() );

      if( !movedSectionHeadersIndexes.empty() ){
        std::cout << "creating PT_LOAD segment header" << std::endl;
        const ProgramHeader loadSegmentHeader = makeLoadProgramHeaderCoveringSections(
          movedSectionHeadersIndexes, mHeaders.sectionHeaderTable(), mHeaders.fileHeader().pageSize()
        );
        mHeaders.addProgramHeader(loadSegmentHeader);
      }

      /** \todo The PT_GNU_RELRO segment should also cover the .dynamic section
       *
       * In elf files generated (at least by ld), a PT_GNU_RELRO segment
       * also covers the .dynamic section.
       *
       * To have a idea of its role, see
       * https://thr3ads.net/llvm-dev/2017/05/2818516-lld-ELF-Add-option-to-make-.dynamic-read-only
       *
       * Making PT_GNU_RELRO also cover the .dynamic section seems to be tricky,
       * because it seems to require some sections to be properly aligned.
       * Making a second PT_GNU_RELRO could be a idea, but:
       * - it will require to add a new program header to the program header table,
       *   that will again require to move more sections from the beginning of the file
       * - it seems not to be well supported by the loaders
       * For more details, see https://reviews.llvm.org/D40029
       *
       * This code below is commented, because it does not work:
       * - launching a simple exectable segfaults
       * - eu-elflint tells:
       *  a) PT_GNU_RELRO is not covered by any PT_LOAD segment
       *  b) PT_GNU_RELRO's file size is greater than its memory size
       */
//       if( mustMoveDynamicSection && mHeaders.containsGnuRelRoProgramHeader() ){
//         std::cout << "extending PT_GNU_RELRO to also cover .dynamic section" << std::endl;
//         extendProgramHeaderSizeToCoverSections( mHeaders.gnuRelRoProgramHeaderMutable(), {mHeaders.dynamicSectionHeader()} );
//       }
      /** \todo if moving .dynamic, think about PT_GNU_RELRO
       */

      std::cout << "DONE !!" << std::endl;

      return;

//         mHeaders.setDynamicSectionSize( mDynamicSection.byteCount(fileHeader().ident._class) );
//         mHeaders.setDynamicStringTableSize( mDynamicSection.stringTable().byteCount() );

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
        loadSegmentHeader.vaddr = mHeaders.programHeaderTableProgramHeader().vaddr;
        loadSegmentHeader.paddr = loadSegmentHeader.vaddr;

        /** Read:
         * - https://ktln2.org/2019/10/28/elf/
         * - https://lwn.net/Articles/631631/
         */

        /// \todo check if this should be as the other PT_LOAD ? f.ex. 0x200000
//         loadSegmentHeader.align = 0x200000;
        loadSegmentHeader.align = mHeaders.fileHeader().pageSize();

        loadSegmentHeader.offset = mHeaders.programHeaderTableProgramHeader().offset;

        if(mustMoveDynamicSection){
          loadSegmentHeader.setPermissions(SegmentPermission::Read | SegmentPermission::Write);
        }else{
          loadSegmentHeader.setPermissions(SegmentPermission::Read);
        }

        uint64_t loadSegmentSize = mHeaders.programHeaderTableProgramHeader().memsz;

        if(mustMoveDynamicSection){
          mHeaders.moveDynamicSectionToEnd(MoveSectionAlignment::SectionAlignment);
          mHeaders.setDynamicSectionSize( mDynamicSection.byteCount(fileHeader().ident._class) );
          ///mSectionSymbolTable.setDynamicSectionVirtualAddress( mHeaders.dynamicSectionHeader().addr );

          if( mGotSection.containsDynamicSectionAddress() ){
            mGotSection.setDynamicSectionAddress(mHeaders.dynamicSectionHeader().addr);
          }
          if( mGotPltSection.containsDynamicSectionAddress() ){
            mGotPltSection.setDynamicSectionAddress(mHeaders.dynamicSectionHeader().addr);
          }

          loadSegmentSize += mHeaders.dynamicProgramHeader().memsz;
        }

        if(mustMoveDynamicStringTable){
          ///mHeaders.moveDynamicStringTableToEnd();
          mDynamicSection.setStringTableAddress( mHeaders.dynamicStringTableSectionHeader().addr );
          mHeaders.setDynamicStringTableSize( mDynamicSection.stringTable().byteCount() );
          ///mSectionSymbolTable.setDynamicStringTableVirtualAddress( mHeaders.dynamicStringTableSectionHeader().addr );
          loadSegmentSize += mHeaders.dynamicStringTableSectionHeader().size;
        }

        loadSegmentHeader.memsz = loadSegmentSize;
        loadSegmentHeader.filesz = loadSegmentHeader.memsz;

      }else{
        // some sections will maybe shrink
        mHeaders.setDynamicSectionSize( mDynamicSection.byteCount(fileHeader().ident._class) );
        mHeaders.setDynamicStringTableSize( mDynamicSection.stringTable().byteCount() );
      }

    }

    /*! \brief Move the .interp to the end
     */
    void moveProgramInterpreterSectionToEnd(MoveSectionAlignment alignment) noexcept
    {
      assert( mHeaders.containsProgramInterpreterSectionHeader() );

      mHeaders.moveProgramInterpreterSectionToEnd(alignment);
    }

    /*! \brief Move the .gnu.hash to the end
     */
    void moveGnuHashTableToEnd(MoveSectionAlignment alignment) noexcept
    {
      assert( mHeaders.containsGnuHashTableSectionHeader() );

      std::cout << " move .gnu.hash , offset end: " << mHeaders.findGlobalFileOffsetEnd() << " , Vaddr end: " << mHeaders.findGlobalVirtualAddressEnd() << std::endl;
      mHeaders.moveGnuHashTableToEnd(alignment);

      std::cout << " Move DONE: offset end: " << mHeaders.findGlobalFileOffsetEnd() << " , Vaddr end: " << mHeaders.findGlobalVirtualAddressEnd() << std::endl;

      if( mDynamicSection.containsGnuHashTableAddress() ){
        mDynamicSection.setGnuHashTableAddress(mHeaders.gnuHashTableSectionHeader().addr);
      }
    }

    /*! \brief Move the .dynamic section to the end
     */
    void moveDynamicSectionToEnd(MoveSectionAlignment alignment) noexcept
    {
      assert( mHeaders.containsDynamicSectionHeader() );

      // Will also handle PT_DYNAMIC
      mHeaders.moveDynamicSectionToEnd(alignment);

      if( mGotSection.containsDynamicSectionAddress() ){
        mGotSection.setDynamicSectionAddress(mHeaders.dynamicSectionHeader().addr);
      }
      if( mGotPltSection.containsDynamicSectionAddress() ){
        mGotPltSection.setDynamicSectionAddress(mHeaders.dynamicSectionHeader().addr);
      }
    }

    /*! \brief Move the .dynstr section to the end
     */
    void moveDynamicStringTableToEnd(MoveSectionAlignment alignment) noexcept
    {
      assert( mHeaders.containsDynamicStringTableSectionHeader() );

      mHeaders.moveDynamicStringTableToEnd(alignment);

      if( mDynamicSection.containsStringTableAddress() ){
        mDynamicSection.setStringTableAddress(mHeaders.dynamicStringTableSectionHeader().addr);
      }
    }

    void moveSectionToEnd(const SectionHeader & header, MoveSectionAlignment alignment)
    {
      if( header.isProgramInterpreterSectionHeader() ){
        moveProgramInterpreterSectionToEnd(alignment);
      }else if( header.isGnuHashTableSectionHeader() ){
        moveGnuHashTableToEnd(alignment);
      }else{
        const QString msg = tr("moving section %1 to the end is not supported")
                            .arg( QString::fromStdString(header.name) );
        throw MoveSectionError(msg);
      }
    }

    /*! \brief Move the first \a count of sections to the end
     *
     * Returns a list of indexes to the section header table
     * for the headers that have been moved.
     *
     * \pre \a count must be > 0
     * \pre the section header table must be sorted by file offset
     */
    std::vector<uint16_t> moveFirstCountSectionsToEnd(uint16_t count)
    {
      assert(count > 0);
      assert( count <= mHeaders.sectionHeaderTable().size() );
      assert( mHeaders.sectionHeaderTableIsSortedByFileOffset() );

      std::vector<uint16_t> movedSectionHeadersIndexes;
      MoveSectionAlignment moveSectionAlignment = MoveSectionAlignment::NextPage;

      // The first section is the null section (SHT_NULL)
      for(uint16_t i=1; i < count; ++i){
        if(i > 1){
          moveSectionAlignment = MoveSectionAlignment::SectionAlignment;
        }
        const SectionHeader & header = mHeaders.sectionHeaderTable()[i];
//           std::cout << "mov: i: " << i << " , section: " << header.name << std::endl;
        /*
          * note sections belong to the PT_NOTE segment,
          * so we have to move them all
          */
        if( header.sectionType() == SectionType::Note ){
          // PT_NOTE program header will also be updated
          std::cout << "moving note sections to end ..." << std::endl;
          mHeaders.moveNoteSectionsToEnd(moveSectionAlignment);
          const auto noteSections = mHeaders.getNoteSectionHeaders();
          mNoteSectionTable.updateSectionHeaders( mHeaders.sectionHeaderTable() );
          movedSectionHeadersIndexes.push_back(i);
          for(size_t j=1; j < noteSections.size(); ++j){
            ++i;
            movedSectionHeadersIndexes.push_back(i);
          }
        }else{
          std::cout << "moving section " << header.name << " to end ..." << std::endl;
          moveSectionToEnd(header, moveSectionAlignment);
          movedSectionHeadersIndexes.push_back(i);
        }
        std::cout << "offset end: " << mHeaders.findGlobalFileOffsetEnd() << " , Vaddr end: " << mHeaders.findGlobalVirtualAddressEnd() << std::endl;
      }

      return movedSectionHeadersIndexes;
    }

    void moveProgramInterpreterSectionToEnd_sandbox()
    {
      /// \todo Check if we have something to move first

      /*
       * We need to add a new PT_LOAD to the program header table.
       * For that, we need to move first sections to the end.
       */

      SectionIndexChangeMap sectionIndexChangeMap = mHeaders.sortSectionHeaderTableByFileOffset();
      /*
       * Sorting the section header table changes the index of some headers.
       * We have to update parts, like symbol tables,
       * that references indexes in the section header table.
       */
      mSymTab.updateSectionIndexes(sectionIndexChangeMap);
      mDynSym.updateSectionIndexes(sectionIndexChangeMap);

      const uint16_t sectionToMoveCount = findCountOfSectionsToMoveToFreeSize(mHeaders.sectionHeaderTable(), fileHeader().phentsize);
      if( sectionToMoveCount >= mHeaders.sectionHeaderTable().size() ){
        const QString msg = tr("should move %1 sections, but file contains only %2 sections")
                            .arg(sectionToMoveCount).arg( mHeaders.sectionHeaderTable().size() );
        throw MoveSectionError(msg);
      }

      std::cout << "required place: " << fileHeader().phentsize << " , sections to move (includes SHT_NULL): " << sectionToMoveCount << std::endl;

      /// If zero -> good :)

      std::vector<uint16_t> movedSectionHeadersIndexes;

      if(sectionToMoveCount > 1){
        movedSectionHeadersIndexes = moveFirstCountSectionsToEnd(sectionToMoveCount);
      }

//       if(sectionToMoveCount > 1){
//         MoveSectionAlignment moveSectionAlignment = MoveSectionAlignment::NextPage;
//         // The first section is the null section (SHT_NULL)
//         for(uint16_t i=1; i < sectionToMoveCount; ++i){
//           if(i > 1){
//             moveSectionAlignment = MoveSectionAlignment::SectionAlignment;
//           }
//           const SectionHeader & header = mHeaders.sectionHeaderTable()[i];
// //           std::cout << "mov: i: " << i << " , section: " << header.name << std::endl;
//           /*
//            * note sections belong to the PT_NOTE segment,
//            * so we have to move them all
//            */
//           if( header.sectionType() == SectionType::Note ){
//             // PT_NOTE program header will also be updated
//             std::cout << "moving note sections to end ..." << std::endl;
//             mHeaders.moveNoteSectionsToEnd(moveSectionAlignment);
//             const auto noteSections = mHeaders.getNoteSectionHeaders();
//             mNoteSectionTable.updateSectionHeaders( mHeaders.sectionHeaderTable() );
//             for(size_t j=0; j < noteSections.size(); ++j){
//               movedSectionHeadersIndexes.push_back(i);
//               if(j > 0){
//                 ++i;
//               }
//             }
//           }else{
//             std::cout << "moving section " << header.name << " to end ..." << std::endl;
//             moveSectionToEnd(header, moveSectionAlignment);
//             movedSectionHeadersIndexes.push_back(i);
//           }
//         }
//       }

      /*
       * Moving sections will change offsets and addresses.
       * We have to update some parts,
       * like symbol tables, that references those addresses.
       */
      mSymTab.updateVirtualAddresses( movedSectionHeadersIndexes, mHeaders.sectionHeaderTable() );
      mDynSym.updateVirtualAddresses( movedSectionHeadersIndexes, mHeaders.sectionHeaderTable() );

      if( !movedSectionHeadersIndexes.empty() ){
        const ProgramHeader loadSegmentHeader = makeLoadProgramHeaderCoveringSections(
          movedSectionHeadersIndexes, mHeaders.sectionHeaderTable(), mHeaders.fileHeader().pageSize()
        );
        mHeaders.addProgramHeader(loadSegmentHeader);
      }

      /** \todo if moving .dynamic, think about PT_GNU_RELRO
       */
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

    /*! \brief Set the symbol table (.symtab) from file
     */
    void setSymTabFromFile(const PartialSymbolTable & table) noexcept
    {
      mSymTab = table;
    }

    /*! \brief Set the symbol table (.symtab)
     */
    const PartialSymbolTable & symTab() const noexcept
    {
      return mSymTab;
    }

    /*! \brief Set the dynamic symbol table (.dynsym) from file
     */
    void setDynSymFromFile(const PartialSymbolTable & table) noexcept
    {
      mDynSym = table;
    }

    /*! \brief Set the dynamic symbol table (.dynsym)
     */
    const PartialSymbolTable & dynSym() const noexcept
    {
      return mDynSym;
    }

    /*! \brief Set the section symbol table from file (from .symtab)
     */
    [[deprecated]]
    void setSectionSymbolTableFromFile(const PartialSymbolTable & table) noexcept
    {
//       mSectionSymbolTable = table;
    }

    /*! \brief Get the section symbol table (from .symtab)
     */
    [[deprecated]]
    const PartialSymbolTable & sectionSymbolTable() const noexcept
    {
//       return mSectionSymbolTable;
    }

    /*! \brief Set the .got global offset table from file
     */
    void setGotSectionFromFile(const GlobalOffsetTable & table) noexcept
    {
      mGotSection = table;
    }

    /*! \brief Get the .got global offset table
     */
    const GlobalOffsetTable & gotSection() const noexcept
    {
      return mGotSection;
    }

    /*! \brief Set the .got.plt global offset table from file
     */
    void setGotPltSectionFromFile(const GlobalOffsetTable & table) noexcept
    {
      mGotPltSection = table;
    }

    /*! \brief Get the .got.plt global offset table
     */
    const GlobalOffsetTable & gotPltSection() const noexcept
    {
      return mGotPltSection;
    }

    /*! \brief Set the program interpreter section from file
     */
    void setProgramInterpreterSectionFromFile(const ProgramInterpreterSection & section) noexcept
    {
      mProgramInterpreterSection = section;
    }

    /*! \brief Get the program interpreter section
     */
    const ProgramInterpreterSection & programInterpreterSection() const noexcept
    {
      return mProgramInterpreterSection;
    }

    /*! \brief Set the .gnu.hash section from file
     */
    void setGnuHashTableSection(const GnuHashTable & table) noexcept
    {
      mGnuHashTableSection = table;
    }

    /*! \brief Get the .gnu.hash section
     */
    const GnuHashTable & gnuHashTableSection() const noexcept
    {
      return mGnuHashTableSection;
    }

    /*! \brief Set the note section table from file
     */
    void setNoteSectionTableFromFile(const NoteSectionTable & table) noexcept
    {
      mNoteSectionTable = table;
    }

    /*! \brief Get the note section table
     */
    const NoteSectionTable & noteSectionTable() const noexcept
    {
      return mNoteSectionTable;
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
    PartialSymbolTable mSymTab;
    PartialSymbolTable mDynSym;
    
//     PartialSymbolTable mSectionSymbolTable;
    
    GlobalOffsetTable mGotSection;
    GlobalOffsetTable mGotPltSection;
    ProgramInterpreterSection mProgramInterpreterSection;
    GnuHashTable mGnuHashTableSection;
    NoteSectionTable mNoteSectionTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_H
