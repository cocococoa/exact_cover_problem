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
T myFromString(const std::string& s) {
  auto ss = std::stringstream();
  ss << s;
  auto ret = T();
  ss >> ret;
  return ret;
}

template <typename T>
std::string toStrImpl(T t) {
  return myToString(t);
}

template <typename Head, typename... Tail>
std::string toStrImpl(const Head& head, const Tail&... tail) {
  return myToString(head) + "_" + toStrImpl(tail...);
}

template <typename T>
void toStrListImpl(std::vector<std::string>& container, const T& t) {
  container.emplace_back(myToString(t));
}

template <typename Head, typename... Tail>
void toStrListImpl(std::vector<std::string>& container, const Head& head,
                   const Tail&... tail) {
  container.emplace_back(myToString(head));
  toStrListImpl(container, tail...);
}
}  // namespace

template <typename Head, typename... Tail>
std::string toStr(const Head& head, const Tail&... tail) {
  return toStrImpl(head, tail...);
}

template <typename Head, typename... Tail>
std::vector<std::string> toStrList(const Head& head, const Tail&... tail) {
  auto ret = std::vector<std::string>();
  toStrListImpl(ret, head, tail...);
  return ret;
}

// Foward declarations
class Item;
using ItemPtr = std::shared_ptr<Item>;
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
  Item(const Head& head, const Tail&... tail)
      : id_(toStr(head, tail...)),
        list_(toStrList(head, tail...)),
        type_(ItemType::Primary),
        alive_(true) {}
  template <typename Head, typename... Tail>
  Item(ItemType type, const Head& head, const Tail&... tail)
      : id_(toStr(head, tail...)),
        list_(toStrList(head, tail...)),
        type_(type),
        alive_(true) {}

  const std::string& GetId() const { return id_; }
  template <typename T>
  T Get(int index) const {
    return myFromString<T>(list_.at(index));
  }

  void SetType(ItemType type) { type_ = type; }
  void SetAlive() { alive_ = true; }
  void SetDead() { alive_ = false; }
  bool IsPrimary() const { return type_ == ItemType::Primary; }
  bool IsSecondary() const { return type_ == ItemType::Secondary; }
  bool IsAlive() const { return alive_; }
  bool IsDead() const { return not alive_; }

 private:
  const std::string id_;
  const std::vector<std::string> list_;
  ItemType type_;
  bool alive_;
};

class Option {
 public:
  void AddPrimaryItem(ItemConstPtr item_ptr);
  template <typename Head, typename... Args>
  void AddPrimaryItems(const Head& head, const Args&... args) {
    this->AddPrimaryItem(head);
    this->AddPrimaryItems(args...);
  }
  void AddPrimaryItems() {}
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
  ItemConstPtr AddSecondaryItem(Head head, Tail... tail) {
    auto item_ptr = std::make_shared<Item>(ItemType::Secondary, head, tail...);
    return this->AddItem(item_ptr);
  }
  template <typename Head, typename... Tail>
  ItemConstPtr FindItem(Head head, Tail... tail) const {
    const auto item = Item(head, tail...);
    return this->FindItem(item);
  }
  template <typename Head, typename... Tail>
  ItemPtr FindItemMut(Head head, Tail... tail) {
    const auto item = Item(head, tail...);
    return this->FindItemMut(item);
  }
  ItemConstPtr AddItem(std::shared_ptr<Item> item_ptr);
  ItemConstPtr FindItem(const Item& item) const;
  ItemPtr FindItemMut(const Item& item);

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
  XCCompile() const;
  std::tuple<int, int, std::vector<std::vector<std::pair<int, int>>>,
             std::unordered_map<int, int>>
  XCCCompile() const;

 private:
  std::unordered_map<std::string, std::size_t> dict_;
  std::vector<std::shared_ptr<Item>> item_list_;
  std::vector<Option> option_list_;
};
