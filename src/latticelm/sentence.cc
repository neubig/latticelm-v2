#include <fstream>
#include <latticelm/sentence.h>

using namespace std;

namespace latticelm {

Sentence ParseSentence(const std::string & str, SymbolSet<std::string> & ids) {
  istringstream iss(str);
  string my_str;
  Sentence ret;
  while(iss >> my_str)
    ret.push_back(ids.GetId(my_str));
  return ret;
}

}
