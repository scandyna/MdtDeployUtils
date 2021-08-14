#include "SortProxyModel.h"
#include <iostream>

namespace Mdt{ namespace ItemModel{

void SortProxyModel::setup(const std::string & args)
{
  std::cout << "SortProxyModel::setup(), args: " << args << std::endl;
}

void SortProxyModel::sort()
{
  std::cout << "SortProxyModel::sort()" << std::endl;
}

}} // namespace Mdt{ namespace ItemModel{
