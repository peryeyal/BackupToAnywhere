#include "RootSubFS.h"
#include <string.h>
#include <iostream>

static const char *general_path = "general";
static const char *glacier_path = "glacier";

RootSubFS::RootSubFS(std::string mount) : generalSubFS(false, mount, general_path), glacierSubFS(true, mount, glacier_path) {}

std::tuple<FileType, size_t> RootSubFS::getattr(const char *path) {
	printf("getattr(%s)\n",path);
	size_t file_size = 0;
	if ((std::string("/") == path) || std::string(".") == path || std::string("..") == path){
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

std::vector<std::string> RootSubFS::readdir(const char *path) {
	printf("readdir(%s)\n",path);
	std::vector<std::string> result;

	if (strcmp(path, SEPARATOR) == 0)
	{
		result.emplace_back(general_path);
		result.emplace_back(glacier_path);
	}

	if (glacierSubFS.shouldDelegate(path)) {
		result = glacierSubFS.readdir(path);
	}

	if (generalSubFS.shouldDelegate(path)) {
		result = generalSubFS.readdir(path);
	}

	return result;
}

size_t RootSubFS::read(const char *path, char *buf, size_t size, size_t offset) {
	printf("read(%s,__,%d,%d)\n",path,size, offset);

	if (generalSubFS.shouldDelegate(path)) {
		return generalSubFS.read(path, buf, size, offset);
	}
	if (glacierSubFS.shouldDelegate(path)) {
		return glacierSubFS.read(path, buf, size, offset);
	}
	return -1;
}
