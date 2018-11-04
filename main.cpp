#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;

#define SIZE_OF_ALPHABET 131

struct Horner_hash {
  unsigned long long operator()(std::string key, size_t table_size) {
    unsigned long long hash_value = 0l;
    for(auto it = key.begin(); it != key.end(); ++it) {
      hash_value += (hash_value * SIZE_OF_ALPHABET + (unsigned long long)(*it));
    }
    return hash_value % table_size;
  }
};

struct rHorner_hash {
  unsigned long long operator()(const std::string& key, size_t table_size) {
    unsigned long long hash_value = 0l;
    for(auto it = key.rbegin(); it != key.rend(); ++it) {
      hash_value += (hash_value * SIZE_OF_ALPHABET + (unsigned long long)(*it));
    }
    return hash_value % table_size;
  }
};

template <typename Hash1, typename Hash2> 
struct Double_hashing {
  private:
    unsigned long long iteration = 0l;
    unsigned long long first_hash = 0l;
    unsigned long long second_hash = 0l;
  public:
    unsigned long long operator()(std::string key, size_t table_size) {
      if(iteration == 0) {
        Hash1 hash_func1;
        Hash2 hash_func2;
        first_hash = hash_func1(key, table_size);
        second_hash = hash_func2(key, table_size/2);
      }
      return ( first_hash + (2 * second_hash + 1) * iteration++ ) % table_size;
    }
};

template<typename T>
class HashTable {
  private:
    vector<string> _table;
    float _coef = 0;
    size_t _table_size = 8;
    void rehashing();
  public:
    HashTable();
    explicit HashTable(size_t size);
    HashTable(const HashTable &) = delete;
    HashTable(HashTable &&) = delete;
    HashTable& operator=(const HashTable &) = delete;
    HashTable& operator=(HashTable &&) = delete;
    ~HashTable() = default;
    size_t get_size() const;
    float get_coef() const;

    bool Add(const std::string& key);
    bool Has(const std::string& key) const;
    bool Remove(const std::string& key);
};

template<typename T>
void HashTable<T>::rehashing() {
  _table_size *= 2;
  _coef /= 2;
  std::vector<std::string> temp_copy(_table);
  _table.resize(_table_size);
  std::string NIL("");
  for(auto it = temp_copy.begin(); it != temp_copy.end(); ++it) {
    if(*it != NIL) {
      this->Add(*it);
    }
  }
}

template<typename T>
HashTable<T>::HashTable() : _table(_table_size), _coef(0) {}

template<typename T>
HashTable<T>::HashTable(size_t size) {
  while(size > _table_size) {
    _table_size *= 2;
  }
  _table.resize(_table_size);
  _coef = 0;
}

template<typename T>
size_t HashTable<T>::get_size() const {
  return _table_size;
}

template<typename T>
float HashTable<T>::get_coef() const {
  return _coef;
}

template <typename T>
bool HashTable<T>::Add(const std::string& key) {
  T hash_func;
  std::string NIL("");
  std::string DEL("::DELETED::");
  unsigned long long hash_value = hash_func(key, _table_size);
  while(_table[hash_value] != NIL && _table[hash_value] != DEL) {
    if(_table[hash_value] == key) {
      return false;
    }
    hash_value = hash_func(key, _table.size());
  }
  _table[hash_value] = key;
  _coef += 1. / _table_size;
  if(_coef >= 3./4) {
    this->rehashing();
  }
  return true;
}

template <typename T>
bool HashTable<T>::Has(const std::string& key) const {
  T hash_func;
  std::string NIL("");
  unsigned long long hash_value = hash_func(key, _table_size);
  while(_table[hash_value] != key) {
    if(_table[hash_value] == NIL) {
      return false;
    }
  }
  return true;
}


int main(int argc, char** argv) {
  HashTable<Double_hashing<Horner_hash, rHorner_hash>> hash_table;
  int n = 0;
  cin >> n;
  char operation;
  std::string key;
  while(n--) {
    cin >> operation >> key;
    switch(operation) {
      case '+':
        cout << hash_table.Add(key) << endl << hash_table.get_coef() << endl;
        break;
      case '?':
        cout <<hash_table.Has(key) << endl;
        break;
      default: break;
    }
  }

  return 0;
}
