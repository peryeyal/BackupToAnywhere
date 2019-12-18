#include "LTRRepositoryWrapper.h"
#include "glacier_upload_script.h"
#include "pugixml.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string.h>

#include <boost/filesystem/path.hpp>

/*
	backups/
			general_view
					/timestamp1 YYYY_MM_DD__hh_mm
						/VpgName
						/timestamp2
					-/Vpg
					-   /vpgname
					-   /vpgbane2
			/glacier
*/

static const char *general_path = "general";
static const char *glacier_path = "glacier";

static const size_t glacier_fileblocksize = 1048576;
static const char *glacier_fileblock1 = "00001.vmdk";
static const char *glacier_fileblock2 = "00002.vmdk";
static const char *glacier_fileblock3 = "00003.vmdk";
static const char *glacier_upload_file = "glacier_upload.py";

namespace
{
	std::string basePath = "E:\\";
}

LTRRepositoryWrapper::LTRRepositoryWrapper(std::string mount) : generalSubFS(mount) {}

std::tuple<FileType, size_t> LTRRepositoryWrapper::getattr(const char *path) {

	size_t file_size = 0;
	if (std::string("/") == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}
	if (generalSubFS.shouldDelegate(path)) {
		return generalSubFS.getattr(path);
	}

	if (std::string("/") + general_path == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}

	if (std::string("/") + glacier_path == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock1 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblocksize);
	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock2 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblocksize);
	}

	if (std::string("/") + glacier_path + "/" + glacier_fileblock3 == path) {
		return std::make_tuple(FileType::RegularFile, glacier_fileblocksize);
	}

	if (std::string("/") + glacier_path + "/" + glacier_upload_file == path) {
		auto file_list = readdir(boost::filesystem::path(path).parent_path().string().c_str());
		file_list.erase(std::remove(file_list.begin(), file_list.end(), boost::filesystem::path(path).filename().string()), file_list.end());

		return std::make_tuple(FileType::RegularFile, get_glacier_upload_script(file_list, glacier_fileblocksize).size());
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

	else if (std::string("/") + glacier_path == path)
	{
		result.emplace_back(glacier_fileblock1);
		result.emplace_back(glacier_fileblock2);
		result.emplace_back(glacier_fileblock3);
		result.emplace_back(glacier_upload_file);
	}
	else if (generalSubFS.shouldDelegate(path)) {
		return generalSubFS.readdir(path);
	}

	return result;
}

size_t LTRRepositoryWrapper::read(const char *path, char *buf, size_t size, size_t offset) {
	if (generalSubFS.shouldDelegate(path)) {
		return generalSubFS.read(path, buf, size, offset);
	}
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

	if (std::string("/") + glacier_path + "/" + glacier_upload_file == path) {
		auto file_list = readdir(boost::filesystem::path(path).parent_path().string().c_str());
		file_list.erase(std::remove(file_list.begin(), file_list.end(), boost::filesystem::path(path).filename().string()), file_list.end());

		memcpy(buf, get_glacier_upload_script(file_list, glacier_fileblocksize).c_str() + offset,
			std::min(size, get_glacier_upload_script(file_list, glacier_fileblocksize).size()-offset));
		return size;
	}

	return -1;
}
