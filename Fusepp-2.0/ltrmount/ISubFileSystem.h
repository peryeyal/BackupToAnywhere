#pragma once

#include <tuple>
#include <string>
#include <vector>

enum FileType
{
	RegularFile = 0,
	Directory = 1,
};

class ISubFileSystem {
public:
	std::tuple<FileType, size_t> getattr(const char *);
	std::vector<std::string> readdir(const char *path);
	size_t read(const char *path, char *buf, size_t size, size_t offset);
	bool shouldDelegate(const char *path);
};
