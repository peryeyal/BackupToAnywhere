#include "GlacierSubFS.h"
#include <algorithm>
#include <memory>
#include <string.h>
#include <boost/filesystem/path.hpp>

static const size_t glacier_fileblocksize = 1048576;
static const char *glacier_fileblock1 = "00001.vmdk";
static const char *glacier_fileblock2 = "00002.vmdk";
static const char *glacier_fileblock3 = "00003.vmdk";


GlacierSubFS::GlacierSubFS(std::string fuse_path) : fuse_path(fuse_path) {}

std::tuple<FileType, size_t> GlacierSubFS::getattr(const char *path) {
	size_t file_size = 0;

	if (std::string("/") + fuse_path == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}

	if (std::string("/") + fuse_path + "/" + glacier_fileblock1 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblocksize);
	}

	if (std::string("/") + fuse_path + "/" + glacier_fileblock2 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblocksize);
	}

	if (std::string("/") + fuse_path + "/" + glacier_fileblock3 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblocksize);
	}

	return std::make_tuple(FileType::RegularFile, 0);

}

std::vector<std::string> GlacierSubFS::readdir(const char *path) {
	std::vector<std::string> result;

	result.emplace_back(glacier_fileblock1);
	result.emplace_back(glacier_fileblock2);
	result.emplace_back(glacier_fileblock3);

	return result;
}

size_t GlacierSubFS::read(const char *path, char *buf, size_t size, size_t offset) {
	if (std::string("/") + fuse_path + "/" + glacier_fileblock1 == path) {
		memset(buf, 1, size);
		return size;
	}

	if (std::string("/") + fuse_path + "/" + glacier_fileblock2 == path) {
		memset(buf, 2, size);
		return size;
	}

	if (std::string("/") + fuse_path + "/" + glacier_fileblock3 == path) {
		memset(buf, 3, size);
		return size;
	}

	return 0;
}

bool GlacierSubFS::shouldDelegate(const char *path) {
	return (std::string(path).rfind("/" + fuse_path, 0) == 0);
}

