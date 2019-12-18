#pragma once
#include "ISubFileSystem.h"
class VolumeSubFS :
	public ISubFileSystem
{
public:
	VolumeSubFS(bool use_simple, std::string repository_path, std::string volume_name, size_t size_in_bytes, std::string dom_path, std::string datapool_path);

	std::tuple<FileType, size_t> getattr(const char *);
	std::vector<std::string> readdir(const char *path);
	size_t read(const char *path, char *buf, size_t size, size_t offset);
	bool shouldDelegate(const char *path);

private:
	bool use_simple;
	std::string repository_path;
	std::string volume_name;
	size_t size_in_bytes;
	std::string dom_path;
	std::string datapool_path;
};

