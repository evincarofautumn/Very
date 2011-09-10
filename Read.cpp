#include "Read.h"
#include <algorithm>

std::vector<std::string>::const_iterator Read::begin() const {
	return tokens.begin();
}

std::vector<std::string>::const_iterator Read::end() const {
	return tokens.end();
}

bool Read::begins(const std::string& token) {
	for (auto i = tokens.begin(); i != tokens.end(); ++i)
		if (std::equal(token.begin(), token.end(), i->begin()))
			return true;
	return false;
}

void Read::def(const std::string& token) {
	tokens.push_back(token);
	std::sort(tokens.begin(), tokens.end());
}

bool Read::exists(const std::string& token) {
	return std::find(tokens.begin(), tokens.end(), token) != tokens.end();
}
