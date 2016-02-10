#pragma once

#include <vector>
#include <cstdint>
#include <latticelm/symbol-set.h>

namespace latticelm {
typedef int32_t WordId;
typedef std::vector<WordId> Sentence;

Sentence ParseSentence(const std::string & str, SymbolSet<std::string> & ids);
  
}


