/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2011-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_PROCESSOR_ISA_H
#define MDT_DEPLOY_UTILS_PROCESSOR_ISA_H

#include <QMetaType>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Processor ISA enum
   */
  enum class ProcessorISA
  {
    Unknown,  /*!< Unknown */
    X86_32,   /*!< X86 32 bit processor */
    X86_64    /*!< X86 64 bit processor */
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::ProcessorISA)

#endif // #ifndef MDT_DEPLOY_UTILS_PROCESSOR_ISA_H
