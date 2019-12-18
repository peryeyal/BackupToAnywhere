#include "LTRRepositoryWrapper.h"
#include "pugixml.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string.h>
#include <boost/config/user.hpp>
#define BOOST_ALL_DYN_LINK
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

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

static const char *ltr_str = "Hello World!\n";
static const char *general_path = "general";
static const char *glacier_path = "glacier";

static const size_t glacier_fileblocksize = 1048576;
static const char *glacier_fileblock1 = "00001.vmdk";
static const char *glacier_fileblock2 = "00002.vmdk";
static const char *glacier_fileblock3 = "00003.vmdk";
static const char *glacier_upload_file = "glacier_upload.py";

static const char *glacier_upload_file_data = "print(\"Hello!\")";

namespace
{
	std::string basePath = "E:\\";
}


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
		return std::make_tuple(FileType::RegularFile, strlen(glacier_upload_file_data));
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
		result.emplace_back(glacier_upload_file);
	}
	return result;
}

LTRRepositoryWrapper::vpgData LTRRepositoryWrapper::readVpgXml(const std::string& path)
{
	pugi::xml_document doc;

	pugi::xml_parse_result loadResult = doc.load_file(path.c_str());

	vpgData data;

	if (loadResult)
	{
		auto root = doc.child("BackupSetMetadataDto");
		data.timestamp = root.child("CheckpointTime").child_value();
		data.vpgName = root.child("VpgName").child_value();
	}

	return data;
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

	if (std::string("/") + glacier_path + "/" + glacier_upload_file == path) {
		memcpy(buf, glacier_upload_file_data + offset, std::min(size, strlen(glacier_upload_file_data)-offset));
		return size;
	}

	return -1;
}

void LTRRepositoryWrapper::onGeneralView(std::vector<std::string>& result)
{
	fs::path dirPath(basePath + "/backups");
	fs::recursive_directory_iterator it(dirPath);
	fs::recursive_directory_iterator end_it;

	while (it != end_it)
	{
		if (fs::is_regular_file(*it) && it->path().extension() == "vpc")
		{
			auto outFile = readVpgXml(it->path().string());
			result.push_back(outFile.timestamp);
		}

		++it;
	}
}
