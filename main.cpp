#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstring>
using namespace std;

#define SIZE_OF_ALPHABET1 131
#define SIZE_OF_ALPHABET2 127
#define COEF 0.75

struct Horner_hash {
  unsigned long long operator()(const std::string & key, const size_t & table_size) {
    unsigned long long hash_value = 0l;
    for(auto it = key.begin(); it != key.end(); ++it) {
      hash_value += (hash_value * SIZE_OF_ALPHABET2 + (unsigned long long)(*it)) % table_size;
    }
    return hash_value;
  }
};

struct rHorner_hash {
  unsigned long long operator()(const std::string & key, const size_t & table_size) {
    unsigned long long hash_value = 0l;
    for(auto it = key.rbegin(); it != key.rend(); ++it) {
      hash_value += (hash_value * SIZE_OF_ALPHABET1 + (unsigned long long)(*it)) % table_size;
    }
    return hash_value;
  }
};

template <typename Hash1, typename Hash2> 
struct Double_hashing {
  private:
    unsigned long long iteration = 0l;
    unsigned long long first_hash = 0l;
    unsigned long long second_hash = 0l;
  public:
    unsigned long long operator()(const std::string & key, const size_t & table_size) {
      if(iteration == 0) {
        Hash1 hash_func1;
        Hash2 hash_func2;
        first_hash = hash_func1(key, table_size);
        second_hash = hash_func2(key, table_size/2 - 1);
      }
      return ( first_hash + (2 * second_hash + 1) * iteration++ ) % table_size;
    }
};

template <typename Hash>
struct Square_probe {
  private:
    long long iteration = -1;
    unsigned long long hash = 0l;
  public:
    unsigned long long operator()(std::string key, size_t table_size) {
      if(iteration == -1) {
        Hash hash_func;
        hash = hash_func(key, table_size);
      }
      hash = (hash + 1 + iteration++) % table_size;
      return hash;
    }
};

enum Status {NIL = -1, OCCUPED = 0, DELETED = 1};

template<typename T>
class HashTable {
  private:
    vector<std::string> _table;
    float _coef = 0.;
    Status * _status;
    size_t _table_size = 8;
    void rehashing();
  public:
    HashTable();
    explicit HashTable(size_t size);
    HashTable(const HashTable &) = delete;
    HashTable(HashTable &&) = delete;
    HashTable& operator=(const HashTable &) = delete;
    HashTable& operator=(HashTable &&) = delete;
    ~HashTable();
    size_t get_size() const;
    float get_coef() const;

    bool Add(const std::string& key);
    long long Has(const std::string& key) const;
    bool Remove(const std::string& key);
};

template<typename T>
void HashTable<T>::rehashing() {
  Status * temp = new Status[_table_size];
  memcpy(temp, _status, sizeof(Status) * _table_size);
  _table_size *= 2;
  delete [] _status;
  _status = new Status[_table_size];
  memset(_status, Status::NIL,  sizeof(Status) * _table_size);
  _coef = 0.;
  std::vector<std::string> temp_copy(_table);
  _table.resize(_table_size);
  for(size_t i = 0; i < _table_size / 2; ++i) {
    if(temp[i] == Status::OCCUPED) {
      this->Add(temp_copy[i]);
    }
  }
  delete [] temp;
}

template<typename T>
HashTable<T>::HashTable()
  : _table(_table_size), _coef(0.), _status(new Status[_table_size]) {
  memset(_status, Status::NIL,  sizeof(Status) * _table_size);
}

template<typename T>
HashTable<T>::HashTable(size_t size) {
  while(size > _table_size) {
    _table_size *= 2;
  }
  _status = new Status[_table_size];
  memset(_status, Status::NIL,  sizeof(Status) * _table_size);
  _table.resize(_table_size);
  _coef = 0.;
}

template<typename T>
HashTable<T>::~HashTable() {
  delete [] _status;
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
  if( _coef + 1./ _table_size >= COEF) {
    this->rehashing();
  }
  T hash_func;
  unsigned long long hash_value = hash_func(key, _table_size);
  for(size_t i = 0; i < _table_size && _status[hash_value] != Status::NIL; ++i) {
    if(_table[hash_value] == key && _status[hash_value] == Status::OCCUPED) {
      return false;
    }
    if(_status[hash_value] == Status::DELETED) {
      _table[hash_value] = key;
      _status[hash_value] = Status::OCCUPED;
		}
    hash_value = hash_func(key, _table_size);
  }
  _table[hash_value] = key;
  _status[hash_value] = Status::OCCUPED;
  _coef += 1. / _table_size;
  return true;
}

template <typename T>
long long HashTable<T>::Has(const std::string & key) const {
  T hash_func;
  unsigned long long hash_value = hash_func(key, _table_size);
  for(size_t i = 0; i < _table_size; ++i) {
    if(_status[hash_value] != Status::NIL) {
      if( _table[hash_value] == key && _status[hash_value] != Status::DELETED) {
        return hash_value;
      }
    }
    else return -1;
    hash_value = hash_func(key, _table_size);
  }
  return -1;
}

template <typename T> 
bool HashTable<T>::Remove(const std::string & key) {
  long long hash_value = this->Has(key);
  if(hash_value == -1) {
    return false;
  }
  _status[hash_value] = Status::DELETED;
  return true;
}


int main(int argc, char** argv) {
  HashTable<Double_hashing<Horner_hash, rHorner_hash>> hash_table(8);
  char operation;
  std::string key;
  while(cin >> operation >> key) {
    switch(operation) {
      case '+':
        cout << (hash_table.Add(key) ? "OK" : "FAIL") << endl;
        break;
      case '?':
        cout << (hash_table.Has(key) != -1 ? "OK" : "FAIL") << endl;
        break;
      case '-':
        cout << (hash_table.Remove(key) ? "OK" : "FAIL") << endl;
        break;
      default: break;
    }
  }
  return 0;
}
