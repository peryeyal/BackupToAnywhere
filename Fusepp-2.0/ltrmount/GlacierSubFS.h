#pragma once
#include "ISubFileSystem.h"

class GlacierSubFS : public ISubFileSystem
{
public:
	GlacierSubFS(std::string fuse_path);

	std::tuple<FileType, size_t> getattr(const char *);
	std::vector<std::string> readdir(const char *path);
	size_t read(const char *path, char *buf, size_t size, size_t offset);

	bool shouldDelegate(const char *path);

private:
	std::string fuse_path;
};

