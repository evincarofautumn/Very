#ifndef READ_H
#define READ_H
#include <string>
#include <vector>

class Read {
	std::vector<std::string> tokens;
public:
	std::vector<std::string>::const_iterator begin() const;
	std::vector<std::string>::const_iterator end() const;
	bool begins(const std::string&);
	void def(const std::string&);
	bool exists(const std::string&);
};

#endif
