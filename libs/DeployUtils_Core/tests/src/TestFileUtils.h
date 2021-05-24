/****************************************************************************
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
 **
 ** This file is part of MdtApplication library.
 **
 ** MdtApplication is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** MdtApplication is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with MdtApplication.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef TEST_FILE_UTILS_H
#define TEST_FILE_UTILS_H

#include <QFile>
#include <QString>
#include <QTextStream>
#include <cassert>

bool writeTextFileUtf8(QFile & file, const QString & content)
{
  assert( file.isOpen() );
  assert( file.isWritable() );

  QTextStream out(&file);
  out.setCodec("UTF-8");
  out << content;

  return true;
}

#endif // #ifndef TEST_FILE_UTILS_H