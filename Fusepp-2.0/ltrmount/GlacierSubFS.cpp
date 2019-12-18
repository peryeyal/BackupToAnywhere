#include "GlacierSubFS.h"



GlacierSubFS::GlacierSubFS()
{
}

std::tuple<FileType, size_t> GlacierSubFS::getattr(const char *) {
	return std::make_tuple(FileType::RegularFile, 0);
}

std::vector<std::string> GlacierSubFS::readdir(const char *path) {
	std::vector<std::string> result;
	return result;
}

size_t GlacierSubFS::read(const char *path, char *buf, size_t size, size_t offset) {
	return 0;
}

