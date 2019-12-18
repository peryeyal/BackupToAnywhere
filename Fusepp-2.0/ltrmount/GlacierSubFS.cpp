#include "GlacierSubFS.h"
#include <algorithm>
#include <memory>
#include <string.h>
#include "glacier_upload_script.h"


static const size_t glacier_fileblocksize = 1048576;
static const char *glacier_fileblock1 = "00001.vmdk";
static const char *glacier_fileblock2 = "00002.vmdk";
static const char *glacier_fileblock3 = "00003.vmdk";
static const char *glacier_upload_file = "glacier_upload.py";


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

	if (std::string("/") + fuse_path + "/" + glacier_upload_file == path) {
		return std::make_tuple(FileType::RegularFile, get_glacier_upload_script().size());
	}

	return std::make_tuple(FileType::RegularFile, 0);

}

std::vector<std::string> GlacierSubFS::readdir(const char *path) {
	std::vector<std::string> result;

	result.emplace_back(glacier_fileblock1);
	result.emplace_back(glacier_fileblock2);
	result.emplace_back(glacier_fileblock3);
	result.emplace_back(glacier_upload_file);

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

	if (std::string("/") + fuse_path + "/" + glacier_upload_file == path) {
		memcpy(buf, get_glacier_upload_script().c_str() + offset, std::min(size, get_glacier_upload_script().size() - offset));
		return size;
	}

	return 0;
}

bool GlacierSubFS::shouldDelegate(const char *path) {
	return (std::string(path).rfind("/" + fuse_path, 0) == 0);
}

