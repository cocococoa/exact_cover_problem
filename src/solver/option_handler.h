#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
template <typename T>
std::string myToString(T t) {
  auto ss = std::stringstream();
  ss << t;
  return ss.str();
}
template <typename T>
std::string toStrImpl(T t) {
  return myToString(t);
}
template <typename Head, typename... Tail>
std::string toStrImpl(Head head, Tail... tail) {
  return myToString(head) + "_" + toStrImpl(tail...);
}
}  // namespace

template <typename Head, typename... Tail>
std::string toStr(Head&& head, Tail&&... tail) {
  return toStrImpl(head, tail...);
}

class Item;
using ItemConstPtr = std::shared_ptr<const Item>;
enum class ItemType { Primary, Secondary };
using Color = int;
class Option;
class OptionHandler;

class Item {
 public:
  Item(const Item&) = default;
  Item(Item&) = default;
  Item(Item&&) = default;

  template <typename Head, typename... Tail>
  Item(Head&& head, Tail&&... tail)
      : id_(toStr(head, tail...)), type_(ItemType::Primary), alive_(true) {}
  Item(const std::string& id)
      : id_(id), type_(ItemType::Primary), alive_(true) {}
  template <typename Head, typename... Tail>
  Item(ItemType type, Head&& head, Tail&&... tail)
      : id_(toStr(head, tail...)), type_(type), alive_(true) {}
  Item(ItemType type, const std::string& id)
      : id_(id), type_(type), alive_(true) {}

  const std::string& GetId() const { return id_; }

  void SetType(ItemType type) { type_ = type; }
  void SetAlive() { alive_ = true; }
  void SetDead() { alive_ = false; }
  bool IsPrimary() const { return type_ == ItemType::Primary; }
  bool IsSecondary() const { return type_ == ItemType::Secondary; }
  bool IsAlive() const { return alive_; }
  bool IsDead() const { return not alive_; }

 private:
  const std::string id_;
  ItemType type_;
  bool alive_;
};

class Option {
 public:
  void AddPrimaryItem(ItemConstPtr item_ptr);
  void AddSecondaryItem(ItemConstPtr item_ptr, Color color);
  int NumPrimaryItems() const { return (int)primary_item_list_.size(); }
  int NumSecondaryItems() const { return (int)secondary_item_list_.size(); }
  bool IsAlive() const;
  bool IsDead() const { return not this->IsAlive(); }

  ItemConstPtr GetPrimaryItem(int index) const;
  std::pair<ItemConstPtr, Color> GetSecondaryItem(int index) const;
  auto PBegin() const { return primary_item_list_.begin(); }
  auto PEnd() const { return primary_item_list_.end(); }
  auto SBegin() const { return secondary_item_list_.begin(); }
  auto SEnd() const { return secondary_item_list_.end(); }

  std::string Str() const;

 private:
  std::vector<ItemConstPtr> primary_item_list_;
  std::vector<std::pair<ItemConstPtr, Color>> secondary_item_list_;
};

class OptionHandler {
 public:
  template <typename Head, typename... Tail>
  ItemConstPtr AddItem(Head head, Tail... tail) {
    const auto item_ptr = std::make_shared<Item>(head, tail...);
    return this->AddItem(item_ptr);
  }
  template <typename Head, typename... Tail>
  ItemConstPtr FindItem(Head head, Tail... tail) const {
    const auto item = Item(head, tail...);
    return this->FindItem(item);
  }
  ItemConstPtr AddItem(std::shared_ptr<Item> item_ptr);
  ItemConstPtr FindItem(const Item& item) const;

  void AddOption(const Option& option) { option_list_.emplace_back(option); }
  void AddOption(Option&& option) {
    option_list_.emplace_back(std::move(option));
  }
  const Option& GetOption(int index) const { return option_list_[index]; }

  int NumRegisteredItems() const { return (int)item_list_.size(); }
  int NumAliveItems() const;
  int NumDeadItems() const;
  int NumPrimaryItems() const;
  int NumSecondaryItems() const;

  std::tuple<int, std::vector<std::vector<int>>, std::unordered_map<int, int>>
  Compile() const;

 private:
  std::unordered_map<std::string, std::size_t> dict_;
  std::vector<std::shared_ptr<Item>> item_list_;
  std::vector<Option> option_list_;
};
