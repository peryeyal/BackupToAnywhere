#include "LTRMetadataSubFS.h"

#include <boost/filesystem.hpp>
#include <set>

namespace fs = boost::filesystem;


LTRMetadataSubFS::LTRMetadataSubFS(std::string mountPoint) : mountPoint(mountPoint)
{
}

std::vector<std::string> LTRMetadataSubFS::readdir(const char *path) {
	std::vector<std::string> result;
	onGeneralView(result);
	return result;
}

bool LTRMetadataSubFS::shouldDelegate(const char *path) {
	return (std::string(path).rfind("/general", 0) == 0);
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

