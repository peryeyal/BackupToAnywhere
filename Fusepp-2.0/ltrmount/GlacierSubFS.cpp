#include "GlacierSubFS.h"
#include <algorithm>
#include <memory>
#include <string.h>
#include <boost/filesystem/path.hpp>

static const size_t glacier_fileblocksize = 1024 * 1024 * 1024; //1GB files


GlacierSubFS::GlacierSubFS(std::string repository_path, size_t size_in_bytes, std::string dom_path, std::string datapool_path) : 
	reader(repository_path, dom_path, datapool_path),
	size_in_bytes(size_in_bytes)
{
}

std::tuple<FileType, size_t> GlacierSubFS::getattr(const char *path) {
	size_t file_size = 0;

	if (!shouldDelegate(path)) {
		return std::make_tuple(FileType::RegularFile, 0);
	}

	int glacier_idx_file = extract_glacier_index(path);
	
	if ((glacier_idx_file+1) * glacier_fileblocksize > size_in_bytes) {
		file_size = size_in_bytes / glacier_fileblocksize;
	}
	else {
		file_size = glacier_fileblocksize;
	}

	return std::make_tuple(FileType::RegularFile, file_size);

}

std::vector<std::string> GlacierSubFS::readdir(const char *path) {
	std::vector<std::string> result;
	for (int i = 0; i * glacier_fileblocksize < size_in_bytes; i++)
	{
		char buffer[20] = { 0 };
		sprintf_s(buffer, "%5d.glacier", i);
		result.emplace_back(buffer);
	}
	return result;
}

int GlacierSubFS::extract_glacier_index(const char *path) {
	int glacier_idx_file;
	char buffer[255];
	sscanf_s(path, "%s%5d.glacier", buffer, 255, &glacier_idx_file);
	return glacier_idx_file;
}

size_t GlacierSubFS::read(const char *path, char *buf, size_t size, size_t offset) {
	if (!shouldDelegate(path)) {
		return 0;
	}

	size_t glacier_idx_file = extract_glacier_index(path);
	return reader.read(buf, size, glacier_idx_file * glacier_fileblocksize + offset);
}

bool GlacierSubFS::shouldDelegate(const char *path) {
	return (std::string(path).find(".glacier") != std::string::npos);
}