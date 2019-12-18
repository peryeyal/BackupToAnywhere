#pragma once
#include "ISubFileSystem.h"

class GlacierSubFS : public ISubFileSystem
{
public:
	GlacierSubFS();

	std::tuple<FileType, size_t> getattr(const char *);
	std::vector<std::string> readdir(const char *path);
	size_t read(const char *path, char *buf, size_t size, size_t offset);

};

