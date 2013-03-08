/* -*- mode: c++ -*- */
/*

  Copyright 2013 Jens Lindström

  Licensed under the Apache License, Version 2.0 (the "License"); you may not
  use this file except in compliance with the License.  You may obtain a copy of
  the License at

       http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
  License for the specific language governing permissions and limitations under
  the License.

*/

#ifndef UTILITIES_SHARED_H
#define UTILITIES_SHARED_H

#include <memory>
#include <unordered_map>

#include "utilities/Delete.h"

namespace utilities {

template <typename ValueType, typename Delete=Delete<ValueType>>
class Shared {
 public:
  Shared()
      : item_(NULL) {
  }

  Shared(ValueType* value)
      : item_(NULL) {
    Assign(Wrap(value));
  }

  Shared(const Shared& other)
      : item_(NULL) {
    Assign(other.item_);
  }

  ~Shared() {
    Clear();
  }

  void Discard() {
    if (item_) {
      items.erase(item_->value);
      Delete()(item_->value);
      item_->value = NULL;
    }
  }

  Shared& operator= (ValueType* value) {
    Assign(Wrap(value));
    return *this;
  }

  Shared& operator= (const Shared& other) {
    Assign(other.item_);
    return *this;
  }

  ValueType* operator-> () { return item_->value; }
  operator ValueType* () { return item_ ? item_->value : NULL; }
  operator bool () { return item_ && item_->value; }

 private:
  struct Item {
    Item(ValueType* value)
        : value(value)
        , counter(0) {
    }
    ~Item() {
      Delete()(value);
    }

    ValueType* value;
    unsigned counter;
  };

  void Assign(Item* item) {
    if (item == item_)
      return;
    Clear();
    item_ = item;
    if (item)
      ++item->counter;
  }

  void Clear() {
    if (item_ && --item_->counter == 0) {
      if (item_->value)
        items.erase(item_->value);
      delete item_;
    }
  }

  Item* Wrap(ValueType* value) {
    if (!value)
      return NULL;
    typename decltype(items)::iterator existing(items.find(value));
    if (existing != items.end())
      return existing->second;
    Item* item = new Item(value);
    items.insert(std::make_pair(value, item));
    return item;
  }

  Item* item_;

  static std::unordered_map<ValueType*, Item*> items;
};

template <typename ValueType, class Delete>
std::unordered_map<ValueType*, typename Shared<ValueType, Delete>::Item*> Shared<ValueType, Delete>::items;

}

#endif // UTILITIES_SHARED_H
