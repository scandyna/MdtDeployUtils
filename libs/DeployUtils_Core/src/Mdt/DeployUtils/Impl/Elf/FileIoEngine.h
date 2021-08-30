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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_IO_ENGINE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_IO_ENGINE_H

#include "DynamicSection.h"
#include "FileReader.h"
#include "FileWriter.h"
#include <QLatin1Char>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class FileIoEngine : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Set the file name
     */
    void setFileName(const QString & name) noexcept
    {
      mFileName = name;
    }

    /*! \brief Clear
     */
    void clear() noexcept
    {
      mFileHeader.clear();
      mSectionNamesStringTableSectionHeader.clear();
      mDynamicSection.clear();
      
      mDynamicSectionHeader.clear();
      
      mFileName.clear();
    }

    /*! \brief Get minimum size to read the file header
     *
     * The get the real minimum size we have to extract the Ident part first.
     * It can be either 52 or 64 bytes.
     * Here we simply return 64.
     *
     * \sa minimumSizeToReadFileHeader(const Ident &)
     */
    int64_t minimumSizeToReadFileHeader() const noexcept
    {
      return 64;
    }

    /*! \brief Get the file header
     */
    FileHeader getFileHeader(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( map.size >= minimumSizeToReadFileHeader() );

      readFileHeaderIfNull(map);

      return mFileHeader;
    }

    /*! \internal
     */
    bool containsDebugSymbols(const ByteArraySpan & map)
    {
      assert( !map.isNull() );

      readFileHeaderIfNull(map);
      checkFileSizeToReadSectionHeaders(map);
      readSectionNameStringTableHeaderIfNull(map);

      const auto pred = [](const std::string & currentName){
        return Mdt::DeployUtils::stringStartsWith(currentName, ".debug");
      };
      const SectionHeader header  = findFirstSectionHeader(map, mFileHeader, mSectionNamesStringTableSectionHeader, SectionType::ProgramData, pred);

      return header.sectionType() != SectionType::Null;
    }

    /*! \brief
     *
     * \pre \a map must not be null
     * \exception ExecutableFileReadError
     */
    QString getSoName(const ByteArraySpan & map)
    {
      assert( !map.isNull() );

      readFileHeaderIfNull(map);
      checkFileSizeToReadSectionHeaders(map);
      readSectionNameStringTableHeaderIfNull(map);
      readDynamicSectionIfNull(map);
//       readDynamicSectionHeaderIfNull(map);
//       checkFileSizeToReadDynamicSection(map);

      std::cout << "SONAME: " << mDynamicSection.getSoName().toStdString() << std::endl;

      return mDynamicSection.getSoName();

//       try{
//         return extractSoName(map, mFileHeader, mDynamicSectionHeader);
//       }catch(const NotNullTerminatedStringError &){
//         const QString message = tr("file '%1': encountered a not null terminated string (file is maybe corrupted or of a unsupported format)")
//                                 .arg(mFileName);
//         throw ExecutableFileReadError(message);
//       }
    }

    /*! \brief
     *
     * \pre \a map must not be null
     * \exception ExecutableFileReadError
     */
    QStringList getNeededSharedLibraries(const ByteArraySpan & map)
    {
      assert( !map.isNull() );

      readFileHeaderIfNull(map);
      checkFileSizeToReadSectionHeaders(map);
      readSectionNameStringTableHeaderIfNull(map);
      readDynamicSectionIfNull(map);
//       readDynamicSectionHeaderIfNull(map);
//       checkFileSizeToReadDynamicSection(map);

      std::cout << "libs: " << mDynamicSection.getNeededSharedLibraries().join(QLatin1Char('\n')).toStdString() << std::endl;

      return mDynamicSection.getNeededSharedLibraries();

//       try{
//         return extractNeededSharedLibraries(map, mFileHeader, mDynamicSectionHeader);
//       }catch(const NotNullTerminatedStringError &){
//         const QString message = tr("file '%1': encountered a not null terminated string (file is maybe corrupted or of a unsupported format)")
//                                 .arg(mFileName);
//         throw ExecutableFileReadError(message);
//       }
    }

    /*! \brief
     *
     * \pre \a map must not be null
     * \exception ExecutableFileReadError
     */
    QStringList getRunPath(const ByteArraySpan & map)
    {
      assert( !map.isNull() );

      readFileHeaderIfNull(map);
      checkFileSizeToReadSectionHeaders(map);
      readSectionNameStringTableHeaderIfNull(map);
      readDynamicSectionIfNull(map);
//       readDynamicSectionHeaderIfNull(map);
//       checkFileSizeToReadDynamicSection(map);


      std::cout << "RUNPATH: " << mDynamicSection.getRunPath().toStdString() << std::endl;

      return mDynamicSection.getRunPath().split(QChar::fromLatin1(':'), QString::SkipEmptyParts);

//       try{
//         return extractRunPath(map, mFileHeader, mDynamicSectionHeader).split(QChar::fromLatin1(':'), QString::SkipEmptyParts);
//       }catch(const NotNullTerminatedStringError &){
//         const QString message = tr("file '%1': encountered a not null terminated string (file is maybe corrupted or of a unsupported format)")
//                                 .arg(mFileName);
//         throw ExecutableFileReadError(message);
//       }
    }

    /*! \brief
     *
     * \pre \a map must not be null
     * \exception ExecutableFileReadError
     * \exception ExecutableFileWriteError
     */
    void setRunPath(ByteArraySpan & map, const QStringList & rPath)
    {
      assert( !map.isNull() );

      readFileHeaderIfNull(map);
      checkFileSizeToReadSectionHeaders(map);
      readSectionNameStringTableHeaderIfNull(map);
      readDynamicSectionHeaderIfNull(map);
      checkFileSizeToReadDynamicSection(map);

      setRunPathToMap( map, rPath.join( QLatin1Char(':') ), mFileHeader, mDynamicSectionHeader );
    }

   private:

    void readFileHeaderIfNull(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( map.size >= minimumSizeToReadFileHeader() );

      if( mFileHeader.seemsValid() ){
        return;
      }

      mFileHeader = extractFileHeader(map);
      if( !mFileHeader.seemsValid() ){
        const QString message = tr("file '%1' does not contain a valid file header")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    /*! \brief
     *
     * \pre mFileHeader must be valid
     */
    void checkFileSizeToReadSectionHeaders(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( mFileHeader.seemsValid() );

      if( map.size < mFileHeader.minimumSizeToReadAllSectionHeaders() ){
        const QString message = tr("file '%1' is to small to read section headers")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    /*! \brief
     *
     * \pre mFileHeader must be valid
     */
    void readSectionNameStringTableHeaderIfNull(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( mFileHeader.seemsValid() );

      if( headerIsStringTableSection(mSectionNamesStringTableSectionHeader) ){
        return;
      }

      mSectionNamesStringTableSectionHeader = extractSectionNameStringTableHeader(map, mFileHeader);
      if( mSectionNamesStringTableSectionHeader.sectionType() == SectionType::Null ){
        const QString message = tr("file '%1' does not contain the section names string table section header")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    /*! \brief
     *
     * \pre mFileHeader must be valid
     * \pre mSectionNamesStringTableSectionHeader must be valid
     */
    void readDynamicSectionHeaderIfNull(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( mFileHeader.seemsValid() );
      assert( headerIsStringTableSection(mSectionNamesStringTableSectionHeader) );

      if( headerIsDynamicSection(mDynamicSectionHeader) ){
        return;
      }

      mDynamicSectionHeader  = findSectionHeader(map, mFileHeader, mSectionNamesStringTableSectionHeader, SectionType::Dynamic, ".dynamic");
      if( mDynamicSectionHeader.sectionType() == SectionType::Null ){
        const QString message = tr("file '%1' does not contain the .dynamic section")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    /*! \brief
     */
    void readDynamicSectionIfNull(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( mFileHeader.seemsValid() );
      assert( headerIsStringTableSection(mSectionNamesStringTableSectionHeader) );

      if( !mDynamicSection.isNull() ){
        return;
      }

      try{
        mDynamicSection = extractDynamicSection(map, mFileHeader, mSectionNamesStringTableSectionHeader);
      }catch(const DynamicSectionReadError & error){
        const QString message = tr("file '%1': error while reading the .dynamic section: %2")
                                .arg( mFileName, error.whatQString() );
        throw ExecutableFileReadError(message);
      }catch(const StringTableError & error){
        const QString message = tr("file '%1': error while reading the string table for the .dynamic section: %2")
                                .arg( mFileName, error.whatQString() );
        throw ExecutableFileReadError(message);
      }

      if( mDynamicSection.isNull() ){
        const QString message = tr("file '%1' does not contain the .dynamic section")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    /*! \brief
     *
     * \pre mFileHeader must be valid
     * \pre mDynamicSectionHeader must be valid
     */
    void checkFileSizeToReadDynamicSection(const ByteArraySpan & map)
    {
      assert( !map.isNull() );
      assert( mFileHeader.seemsValid() );
      assert( headerIsDynamicSection(mDynamicSectionHeader) );

      if( map.size < mDynamicSectionHeader.minimumSizeToReadSection() ){
        const QString message = tr("file '%1' is to small to read the .dynamic section")
                                .arg(mFileName);
        throw ExecutableFileReadError(message);
      }
    }

    FileHeader mFileHeader;
    DynamicSection mDynamicSection;
    
    SectionHeader mSectionNamesStringTableSectionHeader;
    SectionHeader mDynamicSectionHeader;
    
    QString mFileName;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_IO_ENGINE_H
