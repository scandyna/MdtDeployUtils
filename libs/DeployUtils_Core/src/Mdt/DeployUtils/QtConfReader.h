/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_QT_CONF_READER_H
#define MDT_DEPLOY_UTILS_QT_CONF_READER_H

#include "QtConf.h"
#include "ReadQtConfError.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper to read a QtConf
   *
   * \sa https://doc.qt.io/qt-6/qt-conf.html
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtConfReader : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit QtConfReader(QObject *parent = nullptr) noexcept;

    /*! \brief Read from given file
     *
     * \exception ReadQtConfError
     * \pre \a filePath must be a absolute path to a file
     */
    QtConf readFile(const QFileInfo & filePath);

   signals:

    void verboseMessage(const QString & message) const;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_CONF_READER_H
