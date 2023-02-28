// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "TestFileUtils.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <iostream>
#include <cassert>


QString makeAbsolutePath(const std::string & path)
{
  return QFileInfo( QString::fromStdString(path) ).absoluteFilePath();
}

QString makePath(const QString & rootPath, const char * const subPath)
{
  return QDir::cleanPath( rootPath + QLatin1Char('/') + QLatin1String(subPath) );
}

QString makePath(const QTemporaryDir & dir, const char * const subPath)
{
  return makePath(dir.path(), subPath);
}

bool createDirectoryFromPath(const QString & path)
{
  QDir dir;

  return dir.mkpath(path);
}

bool createDirectoryFromPath(const QTemporaryDir & dir, const char * const subPath)
{
  return createDirectoryFromPath( makePath(dir, subPath) );
}

bool isExistingDirectory(const QString & path)
{
  assert( !path.trimmed().isEmpty() );

  return QDir(path).exists();
}

bool fileExists(const QString & filePath)
{
  return QFile::exists(filePath);
}

bool writeTextFileUtf8(QFile & file, const QString & content)
{
  assert( file.isOpen() );
  assert( file.isWritable() );
  assert( QFileInfo(file).isFile() );

  QTextStream out(&file);
  out.setCodec("UTF-8");
  out << content;

  return true;
}

bool createTextFileUtf8(const QString & filePath , const QString & content)
{
  QFile file(filePath);
  if( !file.open(QIODevice::WriteOnly | QIODevice::Text) ){
    qDebug() << "createTextFileUtf8() failed to create '" << filePath << "': " << file.errorString();
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

bool copyFile(const QString & source, const QString & destination)
{
  if( QFile::exists(destination) ){
    if( !QFile::remove(destination) ){
      return false;
    }
  }

  return QFile::copy(source, destination);
}

bool runExecutable(const QString & executableFilePath,
                   const QStringList & arguments,
                   const QProcessEnvironment & env)
{
  QProcess process;

  process.setProcessEnvironment(env);
  process.start(executableFilePath, arguments);
  if( !process.waitForStarted() ){
    qDebug() << "starting process for executable '" << executableFilePath << "' failed: " << process.errorString();
    return false;
  }
  if( !process.waitForFinished() ){
    qDebug() << "error occured for executable '" << executableFilePath << "' failed: " << process.errorString();
    return false;
  }
  if(process.exitStatus() != QProcess::NormalExit){
    qDebug() << "executable '" << executableFilePath << "' probably crashed: " << process.readAllStandardError();
    return false;
  }
//   if(process.exitCode() != 0){
//     qDebug() << "executable '" << executableFilePath << "' returned a error code: " << process.exitCode();
//     qDebug() << process.readAllStandardError();
//     return false;
//   }

  const QString stdOut = QString::fromLocal8Bit( process.readAllStandardOutput() );
  if( !stdOut.isEmpty() ){
    std::cout << "output for executable '" << executableFilePath.toStdString() << "': " << stdOut.toStdString() << std::endl;
  }

  const QString stdErr = QString::fromLocal8Bit( process.readAllStandardError() );
  if( !stdErr.isEmpty() ){
    std::cout << "(std err) output for executable '" << executableFilePath.toStdString() << "': " << stdErr.toStdString() << std::endl;
  }

  if(process.exitCode() != 0){
    qDebug() << "executable '" << executableFilePath << "' returned a error code: " << process.exitCode();
    return false;
  }

  return true;
}
