#ifndef MDT_ITEMMODEL_SORTPROXYMODEL_H
#define MDT_ITEMMODEL_SORTPROXYMODEL_H

#include "mdt_itemmodel_export.h"
#include <string>

namespace Mdt{ namespace ItemModel{

  class MDT_ITEMMODEL_EXPORT SortProxyModel
  {
   public:

    void setup(const std::string & args);
    void sort();
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEMMODEL_SORTPROXYMODEL_H
