#include "LTRRepositoryWrapper.h"
#include "pugixml.hpp"

#include <iostream>
#include <memory>
#include <algorithm>
#include <string.h>

static const char *ltr_str = "Hello World!\n";
static const char *general_path = "general";
static const char *glacier_path = "glacier";

static const size_t glacier_fileblock_size = 1048576;
static const char *glacier_fileblock1 = "00001.vmdk";
static const char *glacier_fileblock2 = "00002.vmdk";
static const char *glacier_fileblock3 = "00003.vmdk";
static const char *glacier_upload_script = "glacier_upload.py";

static const char *glacier_upload_script_data = "print(\"Hello!\")";

std::tuple<FileType, size_t> LTRRepositoryWrapper::getattr(const char *path) {

	size_t file_size = 0;
	if (std::string("/") == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}

	if (std::string("/") + general_path == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}

	if (std::string("/") + glacier_path == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}

	if (std::string("/") + glacier_path + "/" + glacier_upload_script == path) {
		return std::make_tuple(FileType::RegularFile, strlen(glacier_upload_script_data));
	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock1 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblock_size);
	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock2 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblock_size);
	}
	
	if (std::string("/") + glacier_path + "/" + glacier_fileblock3 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblock_size);
	}
	
	return std::make_tuple(FileType::RegularFile, 0);
}

std::vector<std::string> LTRRepositoryWrapper::readdir(const char *path) {
	std::vector<std::string> result;

	if (strcmp(path, "/") == 0) {

		result.emplace_back(".");
		result.emplace_back("..");
		result.emplace_back(general_path);
		result.emplace_back(glacier_path);
	}

	else if (std::string("/") + glacier_path == path)
	{
		result.emplace_back(glacier_fileblock1);
		result.emplace_back(glacier_fileblock2);
		result.emplace_back(glacier_fileblock3);
		result.emplace_back(glacier_upload_script);
	}

	return result;
}

size_t LTRRepositoryWrapper::read(const char *path, char *buf, size_t size, size_t offset) {

	if (std::string("/") + glacier_path + "/" + glacier_fileblock1 == path) {
		memset(buf, 1, size);
		return size;
	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock2 == path) {
		memset(buf, 2, size);
		return size;
	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock3 == path) {
		memset(buf, 3, size);
		return size;
	}

	if (std::string("/") + glacier_path + "/" + glacier_upload_script == path) {
		size_t copied = std::min(size, strlen(glacier_upload_script_data) - offset);
		memcpy(buf, glacier_upload_script_data + offset, copied);
		return copied;
	}

	return -1;
}

