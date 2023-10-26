#include "element.h"

/**
 * For Element
 */

template <typename KeyType, typename ValueType>
Element<KeyType, ValueType>::Element(KeyType&& key, ValueType&& value, bool is_leaf):
key(key), value(value), is_leaf(is_leaf) { }
template <typename KeyType, typename ValueType>
Element<KeyType, ValueType>::Element(const Element* e):
key(e->key), value(e->value), is_leaf(e->is_leaf), tombstone(e->tombstone) { }
template <typename KeyType, typename ValueType>
Element<KeyType, ValueType>::Element(const Element& e):
key(e.key), value(e.value), is_leaf(e.is_leaf), tombstone(e.tombstone) { }
template <typename KeyType, typename ValueType>
Element<KeyType, ValueType>::Element(const Element&& e):
key(std::move(e.key)), value(std::move(e.value)), is_leaf(std::move(e.is_leaf)), tombstone(std::move(e.tombstone)) { }
template <typename KeyType, typename ValueType>
void Element<KeyType, ValueType>::operator=(const Element& elem) {
          this->key = elem.key;
          this->value = elem.value;
          this->is_leaf = elem.is_leaf;
          this->tombstone = elem.tombstone;
}
template <typename KeyType, typename ValueType>
void Element<KeyType, ValueType>::operator=(const Element&& elem) {
          this->key = std::move(elem.key);
          this->value = std::move(elem.value);
          this->is_leaf = std::move(elem.is_leaf);
          this->tombstone = std::move(elem.tombstone);
}
