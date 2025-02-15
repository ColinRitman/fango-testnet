// Copyright (c) 2019-2021 Fango Developers
// Copyright (c) 2018-2021 Fandom Gold Society
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Copyright (c) 2016-2019 The Karbowanec developers
// Copyright (c) 2012-2018 The CryptoNote developers
//
// This file is part of Fango.
//
// Fango is free software distributed in the hope that it
// will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. You can redistribute it and/or modify it under the terms
// of the GNU General Public License v3 or later versions as published
// by the Free Software Foundation. Fango includes elements written 
// by third parties. See file labeled LICENSE for more details.
// You should have received a copy of the GNU General Public License
// along with Fango. If not, see <https://www.gnu.org/licenses/>.

#pragma once

namespace CryptoNote {

//Value must have public method IntrusiveLinkedList<Value>::hook& getHook()
template<class Value> class IntrusiveLinkedList {
public:
  class hook {
  public:
    friend class IntrusiveLinkedList<Value>;

    hook();
  private:
    Value* prev;
    Value* next;
    bool used;
  };

  class iterator {
  public:
    iterator(Value* value);

    bool operator!=(const iterator& other) const;
    bool operator==(const iterator& other) const;
    iterator& operator++();
    iterator operator++(int);
    iterator& operator--();
    iterator operator--(int);

    Value& operator*() const;
    Value* operator->() const;

  private:
    Value* currentElement;
  };

  IntrusiveLinkedList();

  bool insert(Value& value);
  bool remove(Value& value);

  bool empty() const;

  iterator begin();
  iterator end();
private:
  Value* head;
  Value* tail;
};

template<class Value>
IntrusiveLinkedList<Value>::IntrusiveLinkedList() : head(nullptr), tail(nullptr) {}

template<class Value>
bool IntrusiveLinkedList<Value>::insert(Value& value) {
  if (!value.getHook().used) {
    if (head == nullptr) {
      head = &value;
      tail = head;
      value.getHook().prev = nullptr;
    } else {
      tail->getHook().next = &value;
      value.getHook().prev = tail;
      tail = &value;
    }

    value.getHook().next = nullptr;
    value.getHook().used = true;
    return true;
  } else {
    return false;
  }
}

template<class Value>
bool IntrusiveLinkedList<Value>::remove(Value& value) {
  if (value.getHook().used && head != nullptr) {
    Value* toRemove = &value;
    Value* current = head;
    while (current->getHook().next != nullptr) {
      if (toRemove == current) {
        break;
      }

      current = current->getHook().next;
    }

    if (toRemove == current) {
      if (current->getHook().prev == nullptr) {
        assert(current == head);
        head = current->getHook().next;

        if (head != nullptr) {
          head->getHook().prev = nullptr;
        } else {
          tail = nullptr;
        }
      } else {
        current->getHook().prev->getHook().next = current->getHook().next;
        if (current->getHook().next != nullptr) {
          current->getHook().next->getHook().prev = current->getHook().prev;
        } else {
          tail = current->getHook().prev;
        }
      }

      current->getHook().prev = nullptr;
      current->getHook().next = nullptr;
      current->getHook().used = false;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

template<class Value>
bool IntrusiveLinkedList<Value>::empty() const {
  return head == nullptr;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator IntrusiveLinkedList<Value>::begin() {
  return iterator(head);
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator IntrusiveLinkedList<Value>::end() {
  return iterator(nullptr);
}

template<class Value>
IntrusiveLinkedList<Value>::hook::hook() : prev(nullptr), next(nullptr), used(false) {}

template<class Value>
IntrusiveLinkedList<Value>::iterator::iterator(Value* value) : currentElement(value) {}

template<class Value>
bool IntrusiveLinkedList<Value>::iterator::operator!=(const typename IntrusiveLinkedList<Value>::iterator& other) const {
  return currentElement != other.currentElement;
}

template<class Value>
bool IntrusiveLinkedList<Value>::iterator::operator==(const typename IntrusiveLinkedList<Value>::iterator& other) const {
  return currentElement == other.currentElement;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator& IntrusiveLinkedList<Value>::iterator::operator++() {
  assert(currentElement != nullptr);
  currentElement = currentElement->getHook().next;
  return *this;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator IntrusiveLinkedList<Value>::iterator::operator++(int) {
  IntrusiveLinkedList<Value>::iterator copy = *this;

  assert(currentElement != nullptr);
  currentElement = currentElement->getHook().next;
  return copy;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator& IntrusiveLinkedList<Value>::iterator::operator--() {
  assert(currentElement != nullptr);
  currentElement = currentElement->getHook().prev;
  return *this;
}

template<class Value>
typename IntrusiveLinkedList<Value>::iterator IntrusiveLinkedList<Value>::iterator::operator--(int) {
  IntrusiveLinkedList<Value>::iterator copy = *this;

  assert(currentElement != nullptr);
  currentElement = currentElement->getHook().prev;
  return copy;
}

template<class Value>
Value& IntrusiveLinkedList<Value>::iterator::operator*() const {
  assert(currentElement != nullptr);

  return *currentElement; 
}

template<class Value>
Value* IntrusiveLinkedList<Value>::iterator::operator->() const {
  return currentElement; 
}

}
