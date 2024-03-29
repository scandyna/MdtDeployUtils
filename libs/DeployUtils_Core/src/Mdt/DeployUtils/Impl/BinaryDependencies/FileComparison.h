// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_FILE_COMPARISON_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_FILE_COMPARISON_H

#include "Mdt/DeployUtils/OperatingSystem.h"
#include <QString>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

  /*! \internal Check if given file names are equal
   *
   * \pre \a a and \a b must not be empty
   * \pre \a os must be valid
   */
  inline
  bool fileNamesAreEqual(const QString & a, const QString & b, OperatingSystem os) noexcept
  {
    assert( !a.trimmed().isEmpty() );
    assert( !b.trimmed().isEmpty() );
    assert( os != OperatingSystem::Unknown );

    Qt::CaseSensitivity cs;
    if(os == OperatingSystem::Windows){
      cs = Qt::CaseInsensitive;
    }else{
      cs = Qt::CaseSensitive;
    }

    return QString::compare(a, b, cs) == 0;
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_FILE_COMPARISON_H
