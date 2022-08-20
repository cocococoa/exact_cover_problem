#include "solver/option_handler.h"

#include <iostream>

void Option::AddPrimaryItem(ItemConstPtr item_ptr) {
  if (not item_ptr->IsPrimary())
    throw std::runtime_error("Item: " + item_ptr->GetId() + " is not primary.");
  primary_item_list_.emplace_back(item_ptr);
}
void Option::AddSecondaryItem(ItemConstPtr item_ptr, Color color) {
  if (not item_ptr->IsSecondary())
    throw std::runtime_error("Item: " + item_ptr->GetId() +
                             " is not secondary.");
  secondary_item_list_.emplace_back(item_ptr, color);
}
bool Option::IsAlive() const {
  for (const auto& ptr : primary_item_list_)
    if (ptr->IsDead()) return false;
  for (const auto& [ptr, _] : secondary_item_list_)
    if (ptr->IsDead()) return false;
  return true;
}
ItemConstPtr Option::GetPrimaryItem(int index) const {
  return primary_item_list_[index];
}
std::pair<ItemConstPtr, Color> Option::GetSecondaryItem(int index) const {
  return secondary_item_list_[index];
}
std::string Option::Str() const {
  auto ret = std::string("");
  for (const auto& ptr : primary_item_list_) {
    ret += ptr->GetId() + ", ";
  }
  for (const auto& [ptr, _] : secondary_item_list_) {
    ret += ptr->GetId() + ", ";
  }
  ret.erase(ret.end() - 2, ret.end());
  return ret;
}
ItemConstPtr OptionHandler::AddItem(std::shared_ptr<Item> item_ptr) {
  const auto rhs_ptr = this->FindItem(*item_ptr);
  if (rhs_ptr) return rhs_ptr;

  item_list_.emplace_back(item_ptr);
  dict_[item_ptr->GetId()] = item_list_.size() - std::size_t{1};
  return item_ptr;
}
ItemConstPtr OptionHandler::FindItem(const Item& item) const {
  const auto itr = dict_.find(item.GetId());
  if (itr == dict_.end()) return std::shared_ptr<const Item>();
  return item_list_[itr->second];
}
ItemPtr OptionHandler::FindItemMut(const Item& item) {
  const auto itr = dict_.find(item.GetId());
  if (itr == dict_.end()) return std::shared_ptr<Item>();
  return item_list_[itr->second];
}
int OptionHandler::NumAliveItems() const {
  auto ret = 0;
  for (const auto& ptr : item_list_)
    if (ptr->IsAlive()) ret++;
  return ret;
}
int OptionHandler::NumDeadItems() const {
  auto ret = 0;
  for (const auto& ptr : item_list_)
    if (ptr->IsDead()) ret++;
  return ret;
}
int OptionHandler::NumPrimaryItems() const {
  auto ret = 0;
  for (const auto& ptr : item_list_)
    if (ptr->IsAlive() and ptr->IsPrimary()) ret++;
  return ret;
}
int OptionHandler::NumSecondaryItems() const {
  auto ret = 0;
  for (const auto& ptr : item_list_)
    if (ptr->IsAlive() and ptr->IsSecondary()) ret++;
  return ret;
}
std::tuple<int, std::vector<std::vector<int>>, std::unordered_map<int, int>>
OptionHandler::Compile() const {
  if (NumSecondaryItems() > 0) throw std::runtime_error("Not XC problem.");

  auto to_index = std::unordered_map<std::string, int>();
  for (const auto& ptr : item_list_) {
    if (ptr->IsDead()) continue;

    const auto index = (int)to_index.size();
    to_index.emplace(ptr->GetId(), index);
  }

  auto num_items = NumPrimaryItems();
  auto ret = std::vector<std::vector<int>>();
  auto compile_to_raw = std::unordered_map<int, int>();
  for (auto oidx = 0; oidx < (int)option_list_.size(); ++oidx) {
    const auto& option = option_list_[oidx];
    if (option.IsDead()) continue;

    auto op = std::vector<int>();
    for (auto itr = option.PBegin(); itr != option.PEnd(); ++itr) {
      op.emplace_back(to_index[(*itr)->GetId()]);
    }
    compile_to_raw[(int)ret.size()] = oidx;
    ret.push_back(op);
  }

#ifndef NDEBUG
  std::cout << num_items << std::endl;
  for (const auto& option : ret) {
    for (const auto o : option) {
      std::cout << o << ", ";
    }
    std::cout << std::endl;
  }
#endif

  return {num_items, ret, compile_to_raw};
}