#pragma once
#include <string>

class LtrRandomAccessReader
{
public:
	LtrRandomAccessReader(const std::string& repository_path, const std::string& dom_path, const std::string& datapool_path);

	size_t read(char *buf, size_t size, size_t offset);

private:
	std::string repository_path_;
	std::string dom_path_;
	std::string datapool_path_;
};

