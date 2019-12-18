#include "LTRRepositoryWrapper.h"
#include "pugixml.hpp"

#include <iostream>
#include <memory>
#include <string.h>
#include <boost/filesystem.hpp>
#include <set>

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

static const char *glacier_fileblock1 = "00001.vmdk";
static const char *glacier_fileblock2 = "00002.vmdk";
static const char *glacier_fileblock3 = "00003.vmdk";

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

	file_size = strlen(ltr_str);
	return std::make_tuple(FileType::RegularFile, file_size);
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
	}
	else if (std::string("/") + general_path == path)
	{
		onGeneralView(result);
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
		data.backupSetId = root.child("BackupSetId").first_child().child_value();
	}

	return data;
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

void LTRRepositoryWrapper::onGeneralView(std::vector<std::string>& result)
{
	fs::path dirPath(basePath + "backups");
	fs::recursive_directory_iterator it(dirPath);
	fs::recursive_directory_iterator end_it;
	std::set<std::string> set_result;

	while (it != end_it)
	{
		if (it->path().extension() == ".vpc")
		{
			auto outFile = readVpgXml(it->path().string());
			
			if (outFile.timestamp.size() > 4)
				outFile.timestamp = outFile.timestamp.substr(0, outFile.timestamp.size() - 4);
			outFile.timestamp.replace(10, 1, "__");
			set_result.emplace(std::move(outFile.timestamp));
		}

		++it;
	}

	for (auto& str : set_result)
	{
		result.push_back(std::move(str));
	}
	std::sort(result.begin(), result.end());
}
