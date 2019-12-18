#include "LTRMetadataSubFS.h"

#include <boost/filesystem.hpp>
#include <set>
#include "pugixml.hpp"


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

namespace fs = boost::filesystem;

LTRMetadataSubFS::LTRMetadataSubFS(std::string mount_point, std::string fuse_path) : mountPoint(mount_point), fuse_path(fuse_path)
{
}

std::tuple<FileType, size_t> LTRMetadataSubFS::getattr(const char *path) {

	size_t file_size = 0;

	if (std::string("/") + fuse_path == path) {
		return std::make_tuple(FileType::Directory, file_size);
	}

	return std::make_tuple(FileType::RegularFile, 0);
}

size_t LTRMetadataSubFS::read(const char *path, char *buf, size_t size, size_t offset) {
	return 0;
}

std::vector<std::string> LTRMetadataSubFS::readdir(const char *path) {
	std::vector<std::string> result;
	onGeneralView(result);
	return result;
}

bool LTRMetadataSubFS::shouldDelegate(const char *path) {
	return (std::string(path).rfind("/" + fuse_path, 0) == 0);
}

void LTRMetadataSubFS::onGeneralView(std::vector<std::string>& result)
{
	fs::path dirPath(mountPoint + "backups");
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

LTRMetadataSubFS::vpgData LTRMetadataSubFS::readVpgXml(const std::string& path)
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

