#include "LTRRepositoryWrapper.h"
#include "pugixml.hpp"

#include <iostream>
#include <memory>

static const char *ltr_str = "Hello World!\n";
static const char *ltr_path = "/ltr";

std::tuple<FileType, size_t> LTRRepositoryWrapper::getattr(const char *path) {

	size_t file_size = 0;
	if (std::string("/") == path) {
		return std::make_tuple(FileType::Dirrectory, file_size);
	}

	file_size = strlen(ltr_str);
	return std::make_tuple(FileType::Dirrectory, file_size);
}

std::vector<std::string> LTRRepositoryWrapper::readdir(const char *path) {
	std::vector<std::string> result;
	
	if (strcmp(path, "/") != 0)
		return result;

	result.emplace_back(".");
	result.emplace_back("..");
	result.emplace_back(ltr_path);

	pugi::xml_document doc;

	pugi::xml_parse_result loadResult = doc.load_file("C:\\temp\\RepoDemo\\repository.config");

	std::printf(doc.child("repository").attribute("repository-type").value());
	//std::printf(doc.desc);
	//std::printf(doc.first_child().first_child().name.value());
	//std::printf(doc.first_child().first_child().first_child().name.value());
	
	std::cout << "dsaf" << std::endl;

	return result;
}

size_t LTRRepositoryWrapper::read(const char *path, char *buf, size_t size, size_t offset) {

	if (strcmp(path, ltr_path) != 0)
		return -1;

	size_t len = strlen(ltr_str);
	if ((size_t)offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, ltr_str + offset, size);
	}
	else
		size = 0;

	return size;
}

