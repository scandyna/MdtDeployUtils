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
#ifndef MDT_DEPLOY_UTILS_QT_CONF_WRITER_H
#define MDT_DEPLOY_UTILS_QT_CONF_WRITER_H

#include "QtConf.h"
#include "WriteQtConfError.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper to write a QtConf
   *
   * \sa https://doc.qt.io/qt-6/qt-conf.html
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtConfWriter : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit QtConfWriter(QObject *parent = nullptr) noexcept;

    /*! \brief Write a qt.conf file to given \a directoryPath
     *
     * \pre \a directoryPath must not be empty
     * \exception WriteQtConfError
     */
    void writeConfToDirectory(const QtConf & conf, const QString & directoryPath);

   signals:

    void verboseMessage(const QString & message) const;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_CONF_WRITER_H
