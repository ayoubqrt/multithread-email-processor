#include <include/Find.h>
#include <include/Mail.h>

void findWithArray(const string &line, const list<string> &tags, string &tagToReturn, size_t &positionEndTag)
{
	for (const auto &tag : tags)
	{
		size_t pos = line.find(tag);
		if (pos != string::npos)
		{
			positionEndTag = pos + tag.size();
			tagToReturn = tag;
			return;
		}
	}
}

bool find_tag(string line, string &tag, size_t &positionEndTag)
{
	findWithArray(line, possibleTags, tag, positionEndTag);
	bool isTag = positionEndTag > 0;

	return isTag;
}
