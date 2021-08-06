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
#ifndef MDT_DEPLOY_UTILS_IMPL_FILE_MAPPER_H
#define MDT_DEPLOY_UTILS_IMPL_FILE_MAPPER_H

#include "ByteArraySpan.h"
#include "Mdt/DeployUtils/FileOpenError.h"
#include "mdt_deployutilscore_export.h"
#include <QFileDevice>
#include <QString>
#include <QObject>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   *
   * \code
   * QFile file("someFile");
   * if( !file.open() ){
   *   // error
   * }
   *
   * const qint64 headerByteCount = 64;
   * const qint64 fileSize = file.size();
   *
   * if( fileSize < headerByteCount ){
   *   // error
   * }
   *
   * FileMapper fileMapper;
   * ByteArraySpan mapSpan;
   *
   * // This will call file.map()
   * mapSpan = fileMapper.mapIfRequired(file, 0, headerByteCount);
   * readHeader(mapSpan);
   *
   * const qint64 sectionsByteCount = 1024;
   * if( fileSize < sectionsByteCount ){
   *   // error
   * }
   * // This will call file.unmap() and file.map()
   * mapSpan = fileMapper.mapIfRequired(file, 0, sectionsByteCount);
   * readSection1(mapSpan);
   *
   * // This will not call any map method
   * mapSpan = fileMapper.mapIfRequired(file, 0, sectionsByteCount);
   * readSection2(mapSpan);
   * \endcode
   *
   * In above example the flow of control is obvious.
   * In that case, using FileMapper seems not useful.
   * In case of having more than 1 class memeber,
   * that can be called in a unknown order,
   * FileMapper can help.
   *
   * In all cases, it is a simple way to obtain a ByteArraySpan from a QFile.
   */
  class MDT_DEPLOYUTILSCORE_EXPORT FileMapper : public QObject
  {
    Q_OBJECT

   public:

    explicit FileMapper(QObject *parent = nullptr);

    /*! \brief Map \a file into memory
     *
     * \warning when re-open a file with the same instance of FileMapper,
     * unmap() must be called before mapIfRequired()
     *
     * \pre \a file must be open
     * \pre \a offset must be >= 0
     * \pre \a size must be > 0
     * \pre \a file size must be at least \a offset + \a size
     * \exception FileOpenError
     * \sa unmap()
     */
    ByteArraySpan mapIfRequired(QFileDevice & file, qint64 offset, qint64 size);

    /*! \brief Unmap memory for \a file
     *
     * \warning \a file must be the same instance than the one used calling mapIfRequired().
     * Also, if \a file have been closed inbetween, then a new file has been open,
     * the behaviour is undefined.
     */
    void unmap(QFileDevice & file);

    /*! \internal
     *
     * \pre \a offset must be >= 0
     * \pre \a size must be > 0
     */
    bool needToRemap(qint64 offset, qint64 size) const noexcept;

   private:

    ByteArraySpan makeSpan() const noexcept;

    unsigned char *mCurrentMap = nullptr;
    qint64 mOffset = 0;
    qint64 mSize = 0;
  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_FILE_MAPPER_H
