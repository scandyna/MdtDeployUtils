#ifndef MDT_ITEMEDITOR_SORTSETUPWIDGET_H
#define MDT_ITEMEDITOR_SORTSETUPWIDGET_H

#include <Mdt/ItemModel/SortProxyModel>
#include "mdt_itemeditor_export.h"

namespace Mdt{ namespace ItemEditor{

class MDT_ITEMEDITOR_EXPORT SortSetupWidget
{
 public:

  void setup(Mdt::ItemModel::SortProxyModel & model);
};

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEMEDITOR_SORTSETUPWIDGET_H
