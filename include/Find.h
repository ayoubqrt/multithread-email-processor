#include <iostream>
#include <list>

using namespace std;

void findWithArray(const string &line, const list<string> &tags, string &tagToReturn, size_t &positionEndTag);
bool find_tag(string line, string &tag, size_t &positionEndTag);
