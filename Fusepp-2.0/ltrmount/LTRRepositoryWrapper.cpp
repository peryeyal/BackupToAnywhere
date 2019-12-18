#include "LTRRepositoryWrapper.h"
#include "pugixml.hpp"

#include <iostream>
#include <memory>
#include <string.h>

static const char *ltr_str = "Hello World!\n";
static const char *general_path = "general";
static const char *glacier_path = "glacier";

static const char *glacier_fileblock1 = "00001.vmdk";
static const char *glacier_fileblock2 = "00002.vmdk";
static const char *glacier_fileblock3 = "00003.vmdk";

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

	file_size = strlen(ltr_str);
	return std::make_tuple(FileType::RegularFile, file_size);
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
	}

	return result;
}

size_t LTRRepositoryWrapper::read(const char *path, char *buf, size_t size, size_t offset) {

	if (std::string("/") + glacier_path + "/" + glacier_fileblock1 == path) {
		static std::vector<char> data(0, 1);

	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock2 == path) {

	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock3 == path) {

	}

	return -1;
}

