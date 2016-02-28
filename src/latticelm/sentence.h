#pragma once

#include <vector>
#include <cstdint>
#include <utility>
#include <latticelm/symbol-set.h>

namespace latticelm {
typedef int32_t WordId;
typedef std::vector<WordId> Sentence;
typedef std::vector<std::pair<WordId,WordId>> Alignment;

Sentence ParseSentence(const std::string & str, SymbolSet<std::string> & ids);
  
}


