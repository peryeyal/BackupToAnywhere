#include "LTRMetadataSubFS.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include <algorithm>
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

LTRMetadataSubFS::LTRMetadataSubFS(bool use_simple_vmdk, std::string mount_point, std::string fuse_path) : 
	simple_vmdk(use_simple_vmdk), mountPoint(mount_point), fuse_path(fuse_path), dummyVolumeSubFS(use_simple_vmdk, mount_point, "dummy volume", 0, "dummy dom", "dummy datapool")
{
}

ISubFileSystem LTRMetadataSubFS::createVolumeSubFS(const char *path) {
	size_t size_in_bytes = 0;
	VolumeSubFS singleVolume(simple_vmdk, mountPoint, "put volume name here", size_in_bytes, "put dom path in repository here", "put datapool path in repository here");
	return singleVolume;
}

std::tuple<FileType, size_t> LTRMetadataSubFS::getattr(const char *path) {

	size_t file_size = 0;
	if (dummyVolumeSubFS.shouldDelegate(path)) {
		return createVolumeSubFS(path).getattr(path);
	}
	return std::make_tuple(FileType::Directory, file_size);
}

size_t LTRMetadataSubFS::read(const char *path, char *buf, size_t size, size_t offset) {
	if (dummyVolumeSubFS.shouldDelegate(path)) {
		return createVolumeSubFS(path).read(path, buf, size, offset);
	}
	return 0;
}

std::vector<std::string> LTRMetadataSubFS::readdir(const char *path) {
	if (dummyVolumeSubFS.shouldDelegate(path)) {
		return createVolumeSubFS(path).readdir(path);
	}

	std::vector<std::string> result;
	std::vector<std::string> substrRes;

	boost::split(substrRes, path, boost::is_any_of(SEPARATOR));
	
	if (substrRes.size() == 2) //[general|glacier]
		readHighLevelDir(result);
	else if (substrRes.size() == 3) //[general|glacier]/<time>
		readDatesDir(result, substrRes[2]);
	else if (substrRes.size() == 4)
		readVpgDir(result, substrRes[3]);

	return result;
}

bool LTRMetadataSubFS::shouldDelegate(const char *path) {
	return (std::string(path).rfind(std::string(SEPARATOR) + fuse_path, 0) == 0);
}

void LTRMetadataSubFS::readDatesDir(std::vector<std::string>& result, const std::string& timestamp)
{
	auto set_result = readVpgMetada();
	auto it = std::find_if(set_result.begin(), set_result.end(),
		[timestamp](vpgData data) {
		return data.timestamp == timestamp;
	});

	if (it != set_result.end())
	{
		result.push_back(it->vpgName);
	}
}

void LTRMetadataSubFS::readVpgDir(std::vector<std::string>& result, const std::string& vpgName)
{
	auto set_result = readVpgMetada();
	auto it = std::find_if(set_result.begin(), set_result.end(),
		[vpgName](vpgData data) {
		return data.vpgName == vpgName;
	});

	if (it != set_result.end())
	{
		for (const auto& vmIt: it->vmNames)
			result.push_back(vmIt);
	}
}

const std::set<LTRMetadataSubFS::vpgData>& LTRMetadataSubFS::readVpgMetada()
{
	auto path_str = mountPoint + "backups";
	fs::path dirPath(path_str);
	//fs::directory_iterator it(dirPath);
	//fs::directory_iterator end_it;
	static std::set<LTRMetadataSubFS::vpgData> set_result;

	if (set_result.empty())
	{
		for (fs::directory_iterator it(dirPath); it != fs::directory_iterator(); ++it)
			if (fs::is_directory(it->path()))
			{
				auto outFile = readVpgXml(it->path().string());
					
				if (outFile.timestamp.size() > 4)
					outFile.timestamp = outFile.timestamp.substr(0, outFile.timestamp.size() - 4);
					outFile.timestamp.replace(13, 1, "_");
				outFile.timestamp.replace(10, 1, "__");
				set_result.emplace(std::move(outFile));
			}
	}

	return set_result;
}

void LTRMetadataSubFS::readHighLevelDir(std::vector<std::string>& result)
{
	const auto& set_result = readVpgMetada();

	for (auto& str : set_result)
	{
		result.push_back(std::move(str.timestamp));
	}
	std::sort(result.begin(), result.end());
}

LTRMetadataSubFS::vpgData LTRMetadataSubFS::readVpgXml(const std::string& path)
{
	vpgData data;
	fs::path vpgDir = path;
	for (fs::directory_iterator file_it = fs::directory_iterator(vpgDir); file_it != fs::directory_iterator(); ++file_it)
	{
		if (file_it->path().extension() == ".vpc")
		{
			pugi::xml_document doc;
			pugi::xml_parse_result loadResult = doc.load_file(file_it->path().string().c_str());

			if (loadResult)
			{
				auto root = doc.child("BackupSetMetadataDto");
				data.timestamp = root.child("CheckpointTime").child_value();
				data.vpgName = root.child("VpgName").child_value();
				data.backupSetId = root.child("BackupSetId").first_child().child_value();
			}
		}

		if (file_it->path().extension() == ".vmc")
		{
			pugi::xml_document doc;
			pugi::xml_parse_result loadResult = doc.load_file(file_it->path().string().c_str());

			if (loadResult)
			{
				data.vmNames.push_back(doc.child("VmImageMetadataDto").child("VmName").child_value());
			}
		}
	}

	return data;
}

