#pragma once
#include <string>

class LtrRandomAccessReader
{
public:
	LtrRandomAccessReader(std::string repository_path, std::string dom_path, std::string datapool_path);

	size_t read(char *buf, size_t size, size_t offset);

private:
	std::string repository_path;
	std::string dom_path;
	std::string datapool_path;
};

