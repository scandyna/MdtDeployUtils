/****************************************************************************
 **
 ** Copyright (C) 2021-2023 Philippe Steinmann.
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

#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QProcessEnvironment>


/*
 * Make a absolute path that retruns the correct result
 * on Linux and also on Windows.
 * As example: /tmp/file.txt will become C:/tmp/file.txt on Windows
 * See https://gitlab.com/scandyna/mdtdeployutils/-/jobs/1934576156
 */
QString makeAbsolutePath(const std::string & path);

QString makePath(const QString & rootPath, const char * const subPath);

QString makePath(const QTemporaryDir & dir, const char * const subPath);

bool createDirectoryFromPath(const QString & path);

bool createDirectoryFromPath(const QTemporaryDir & dir, const char * const subPath);

bool isExistingDirectory(const QString & path);

bool fileExists(const QString & filePath);

bool writeTextFileUtf8(QFile & file, const QString & content);

bool createTextFileUtf8(const QString & filePath , const QString & content);

QString readTextFileUtf8(const QString & filePath);

bool copyFile(const QString & source, const QString & destination);

bool runExecutable( const QString & executableFilePath,
                    const QStringList & arguments = QStringList(),
                    const QProcessEnvironment & env = QProcessEnvironment::systemEnvironment() );

#endif // #ifndef TEST_FILE_UTILS_H
