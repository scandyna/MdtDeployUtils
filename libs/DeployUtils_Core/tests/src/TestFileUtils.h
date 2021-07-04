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
#include <QTemporaryDir>
#include <cassert>

QString makePath(const QTemporaryDir & dir, const char * const subPath)
{
  return QDir::cleanPath( dir.path() + QLatin1Char('/') + QLatin1String(subPath) );
}

bool fileExists(const QString & filePath)
{
  return QFile::exists(filePath);
}

bool writeTextFileUtf8(QFile & file, const QString & content)
{
  assert( file.isOpen() );
  assert( file.isWritable() );

  QTextStream out(&file);
  out.setCodec("UTF-8");
  out << content;

  return true;
}

bool createTextFileUtf8(const QString & filePath , const QString & content)
{
  QFile file(filePath);
  if( !file.open(QIODevice::WriteOnly | QIODevice::Text) ){
    return false;
  }
  if( !writeTextFileUtf8(file, content) ){
    return false;
  }
  file.close();

  return true;
}

QString readTextFileUtf8(const QString & filePath)
{
  QString content;

  QFile file(filePath);
  if( !file.open(QIODevice::ReadOnly | QIODevice::Text) ){
    return content;
  }

  QTextStream in(&file);
  in.setCodec("UTF-8");
  content = in.readAll();

  file.close();

  return content;
}

#endif // #ifndef TEST_FILE_UTILS_H
