#include <iostream>

template <typename KeyType, typename ValueType>
class Element;

template <typename T, typename Y>
using bpElement = std::shared_ptr<Element<T, Y>>;

template <typename KeyType, typename ValueType>
struct Element {
          typedef KeyType key_type;
          typedef ValueType value_type;

          KeyType key;
          ValueType value;
          bool is_leaf { true } , tombstone { false };

          Element(KeyType&& key, ValueType&& value, bool type);
          ~Element() { }

          Element(const Element*);
          Element(const Element&);
          Element(const Element&&);

          void operator=(const Element&);
          void operator=(const Element&&);

          std::pair<KeyType, ValueType> get_content () const { return {key, value}; }

          void set_key(const KeyType& key) { this->key = key; }
          void set_value(const ValueType& val) { this->value = val; }

          static const Element* create_element_ (const KeyType& key, const ValueType& val);
          static const Element* create_element_ (const KeyType&& key, const ValueType&& val);
          static const bpElement<KeyType, ValueType> create_element (const KeyType& key, const ValueType& val);
          static const bpElement<KeyType, ValueType> create_element (const KeyType&& key, const ValueType&& val);
};

template <typename KeyType, typename ValueType>
inline bool operator==(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) { return x.key == y.key; }
template <typename KeyType, typename ValueType>
inline bool operator!=(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) { return x.key != y.key; }
template <typename KeyType, typename ValueType>
inline bool operator>(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) { return x.key > y.key; }
template <typename KeyType, typename ValueType>
inline bool operator>=(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) { return x.key >= y.key; }
template <typename KeyType, typename ValueType>
inline bool operator<(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) { return x.key < y.key; }
template <typename KeyType, typename ValueType>
inline bool operator<=(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) { return x.key <= y.key; }