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
#include "CompilerFinder.h"
#include "AbstractCompilerFinderEngine.h"
#include "MsvcFinder.h"
#include <cassert>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

CompilerFinder::CompilerFinder(QObject* parent)
 : QObject(parent)
{
}

CompilerFinder::~CompilerFinder() noexcept
{
}

void CompilerFinder::findFromCxxCompilerPath(const QFileInfo & executablePath)
{
  assert( executablePath.isAbsolute() );

  if( !mEngine ){
    instanciateMsvcEngine();
  }

  if( mEngine->isSupportedCompiler(executablePath) ){
    mEngine->findFromCxxCompilerPath(executablePath);
    return;
  }

  /// \todo try other impl

}

void CompilerFinder::setInstallDir(const QString & path, Compiler compiler)
{
  assert( !path.trimmed().isEmpty() );

  if(mEngine){
    if( mEngine->compiler() != compiler ){
      mEngine.reset();
    }
  }

  if( !mEngine ){
    switch(compiler){
      case Compiler::Msvc:
        instanciateMsvcEngine();
        break;
    }
  }

  mEngine->setInstallDir(path);
}

bool CompilerFinder::hasInstallDir() const noexcept
{
  if( !mEngine ){
    return false;
  }

  return mEngine->hasInstallDir();
}

QString CompilerFinder::installDir() const noexcept
{
  assert( hasInstallDir() );

  return mEngine->installDir();
}

QString CompilerFinder::findRedistDirectory(ProcessorISA cpu, BuildType buildType) const
{
  assert( hasInstallDir() );
  assert( cpu != ProcessorISA::Unknown );
}

void CompilerFinder::instanciateMsvcEngine()
{
  assert( mEngine.get() == nullptr );

  mEngine = std::make_unique<MsvcFinder>();
}

}} // namespace Mdt{ namespace DeployUtils{
