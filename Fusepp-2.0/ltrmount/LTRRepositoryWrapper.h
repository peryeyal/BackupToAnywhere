#pragma once

#include <tuple>
#include <string>
#include <vector>
#include "ISubFileSystem.h"
#include "LTRMetadataSubFS.h"
#include "GlacierSubFS.h"

class LTRRepositoryWrapper : ISubFileSystem
{
public:
	LTRRepositoryWrapper(std::string mount);
	std::tuple<FileType, size_t> getattr(const char *);

	std::vector<std::string> readdir(const char *path);

	size_t read(const char *path, char *buf, size_t size, size_t offset);

private:
	LTRMetadataSubFS generalSubFS;
	GlacierSubFS glacierSubFS;
};


