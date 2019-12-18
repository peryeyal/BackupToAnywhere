#pragma once
#include "ISubFileSystem.h"
#include "GlacierSubFS.h"

class VolumeSubFS :
	public ISubFileSystem
{
public:
	VolumeSubFS(bool use_simple, std::string repository_path, size_t size_in_bytes, std::string dom_path, std::string datapool_path);

	std::tuple<FileType, size_t> getattr(const char *);
	std::vector<std::string> readdir(const char *path);
	size_t read(const char *path, char *buf, size_t size, size_t offset);
	bool shouldDelegate(const char *path);

private:
	GlacierSubFS glacierFS;
	LtrRandomAccessReader reader;

	bool use_simple;
	size_t size_in_bytes;
};

