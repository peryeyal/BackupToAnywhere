#include "LTRRepositoryWrapper.h"
#include <iostream>

static const char *general_path = "general";
static const char *glacier_path = "glacier";

LTRRepositoryWrapper::LTRRepositoryWrapper(std::string mount) : generalSubFS(mount, general_path), glacierSubFS(glacier_path) {}

std::tuple<FileType, size_t> LTRRepositoryWrapper::getattr(const char *path) {

	size_t file_size = 0;
	if (std::string("/") == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}
	if (generalSubFS.shouldDelegate(path)) {
		return generalSubFS.getattr(path);
	}
	if (glacierSubFS.shouldDelegate(path)) {
		return glacierSubFS.getattr(path);
	}

	return std::make_tuple(FileType::RegularFile, 0);
}

std::vector<std::string> LTRRepositoryWrapper::readdir(const char *path) {
	std::vector<std::string> result;

	if (strcmp(path, "/") == 0) 
	{
		result.emplace_back(".");
		result.emplace_back("..");
		result.emplace_back(general_path);
		result.emplace_back(glacier_path);
	}

	if (glacierSubFS.shouldDelegate(path)) {
		return glacierSubFS.readdir(path);
	}

	if (generalSubFS.shouldDelegate(path)) {
		return generalSubFS.readdir(path);
	}

	return result;
}

size_t LTRRepositoryWrapper::read(const char *path, char *buf, size_t size, size_t offset) {

	if (generalSubFS.shouldDelegate(path)) {
		return generalSubFS.read(path, buf, size, offset);
	}
	if (glacierSubFS.shouldDelegate(path)) {
		return glacierSubFS.read(path, buf, size, offset);
	}
	return -1;
}
