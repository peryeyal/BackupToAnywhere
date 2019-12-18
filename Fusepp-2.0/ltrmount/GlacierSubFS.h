#pragma once
#include "ISubFileSystem.h"
#include "LtrRandomAccessReader.h"

class GlacierSubFS : public ISubFileSystem
{
public:
	GlacierSubFS(std::string repository_path, size_t size_in_bytes, std::string dom_path, std::string datapool_path);

	std::tuple<FileType, size_t> getattr(const char *);
	std::vector<std::string> readdir(const char *path);
	size_t read(const char *path, char *buf, size_t size, size_t offset);

	bool shouldDelegate(const char *path);

private:
	LtrRandomAccessReader reader;
	size_t size_in_bytes;

	int extract_glacier_index(const char *path);
};

