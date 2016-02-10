#pragma once

#include <vector>
#include <unordered_map>
#include <latticelm/macros.h>
#include <latticelm/hashes.h>

namespace latticelm {

typedef int32_t WordId;

template < class Key >
class SymbolSet {

public:

  typedef std::unordered_map<Key,WordId> Map;
  typedef std::vector< Key > Vocab;

protected:
  
  Map map_;
  Vocab vocab_;
  bool freeze_;

public:
  SymbolSet() : map_(), vocab_(), freeze_(false) { }
  ~SymbolSet() { }

  WordId GetId(const Key & key) {
    auto it = map_.find(key);
    if(it != map_.end()) {
      return it->second;
    } else if(freeze_) {
      return -1;
    } else {
      map_[key] = vocab_.size();
      vocab_.push_back(key);
      return vocab_.size()-1;
    }
  }
  const Key & GetSym(WordId wid) {
    if(wid < 0 || wid >= (WordId)vocab_.size())
      THROW_ERROR("Invalid wordid in GetSym: " << wid << ", size=" << vocab_.size());
    return vocab_[wid];
  }

  size_t size() { return vocab_.size(); }



};

}

