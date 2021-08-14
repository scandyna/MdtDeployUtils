#include "SortSetupWidget.h"
#include <iostream>

namespace Mdt{ namespace ItemEditor{

void SortSetupWidget::setup(Mdt::ItemModel::SortProxyModel & model)
{
  std::cout << "SortSetupWidget::setup()" << std::endl;
  model.setup("from SortSetupWidget");
}

}} // namespace Mdt{ namespace ItemEditor{
