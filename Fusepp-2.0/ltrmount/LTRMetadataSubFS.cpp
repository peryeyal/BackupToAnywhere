#include "LTRMetadataSubFS.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp> 
#include <algorithm>
#include <set>
#include "pugixml.hpp"
#include <iostream>

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
	simple_vmdk(use_simple_vmdk), mountPoint(mount_point), fuse_path(fuse_path), dummyVolumeSubFS(use_simple_vmdk, mount_point, 0, "dummy dom", "dummy datapool")
{
}

VolumeSubFS LTRMetadataSubFS::createVolumeSubFS(const char *path) {
	auto volumeEntry = findVM(path);
	VolumeSubFS singleVolume(simple_vmdk, mountPoint, volumeEntry.volumeSize, volumeEntry.domPath, volumeEntry.dataPoolPath);
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
		for (const auto& vmIt: it->vmList)
			result.push_back(vmIt.first);
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
					
				if (outFile.timestamp.size() > 4) {
					outFile.timestamp = outFile.timestamp.substr(0, outFile.timestamp.size() - 4);
				}
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

LTRMetadataSubFS::volumeEntry LTRMetadataSubFS::readVolumeXml(const std::string& path, const std::string& volumePathId, const std::string& unitNumber)
{
	pugi::xml_document doc;
	pugi::xml_parse_result loadResult = doc.load_file((path + SEPARATOR + "gp_" + volumePathId + ".dat").c_str());

	if (loadResult)
	{
		
		auto root = doc.child("BackupVmInfoDto");
		auto volumeConfig = root.child("VolumesConfig");
		auto volumes = volumeConfig.children("a:KeyValueOfVolumeIdentifierBackupVolumeInfoDtoUsddlz5t");

		for (auto volumeContent = volumes.begin(); volumeContent != volumes.end(); ++volumeContent)
		{
			auto actualUnitNumber = volumeContent->child("a:Value").child("LocationInfo").child_value("Unit");
			if (actualUnitNumber == unitNumber)
			{
				auto volumeFullPath = volumeContent->child("a:Value").child_value("ProtectedVolumePath");

				auto volumeSize = volumeContent->child("a:Value").child("ProvisionedSize").child_value("b:m_sizeInBytes");

				std::vector<std::string> substrRes;
				boost::split(substrRes, volumeFullPath, boost::is_any_of("/"));

				volumeEntry entry;
				entry.volumeName = substrRes[1];
				entry.volumeSize = std::atoll((char*)volumeSize);
				return entry;
			}
		}
	}

	return {};
}

std::string LTRMetadataSubFS::readImageXml(const std::string& path, const std::string& mirrorId, const std::string& domFileId)
{
	fs::path mirrorDirPath = path + SEPARATOR + ".." + SEPARATOR + ".." + SEPARATOR + "images" + SEPARATOR + mirrorId;

	for (fs::directory_iterator file_it = fs::directory_iterator(mirrorDirPath); file_it != fs::directory_iterator(); ++file_it)
	{
		if (file_it->path().string().rfind(domFileId + ".imd") != 0)
		{
			pugi::xml_document doc;
			pugi::xml_parse_result loadResult = doc.load_file((file_it->path().string()).c_str());

			if (loadResult)
			{
				auto temp = doc.child("VolumeImageMapping").child("Dom").child_value("d2p1:Identifier");
				return temp;
			}

		}
	}
}

LTRMetadataSubFS::vpgData LTRMetadataSubFS::readVpgXml(const std::string& path)
{
	vpgData data;
	fs::path vpgDir = path;
	for (fs::directory_iterator file_it = fs::directory_iterator(vpgDir); file_it != fs::directory_iterator(); ++file_it)
	{
		pugi::xml_document doc;
		pugi::xml_parse_result loadResult = doc.load_file(file_it->path().string().c_str());

		if (loadResult && file_it->path().extension() == ".vpc")
		{
			auto root = doc.child("BackupSetMetadataDto");
			data.timestamp = root.child("CheckpointTime").child_value();
			data.vpgName = root.child("VpgName").child_value();
			data.backupSetId = root.child("BackupSetId").first_child().child_value();
			
		}

		if (loadResult && file_it->path().extension() == ".vmc")
		{
			auto imageMetadata = doc.child("VmImageMetadataDto");
			auto volumesDtos = imageMetadata.child("VolumeImageDtos").children();
			auto volumeFileId = imageMetadata.child("CustomFilesMapping").child("KeyValuePairDtoOfstringguid").child_value("Value");

			for (auto volumeContent = volumesDtos.begin(); volumeContent != volumesDtos.end(); ++volumeContent)
			{
				auto mirrorId = volumeContent->child("MirrorId").child_value("d4p1:Identifier");
				auto domFileId = volumeContent->child("VolumeImageId").child_value("d4p1:UniquePartialId");
				auto unitNumber = volumeContent->child("DiskLocationParams").child_value("d4p1:UnitNumber");

				auto domId = readImageXml(path, mirrorId, domFileId);

				volumeEntry entry = readVolumeXml(path, volumeFileId, unitNumber);
				
				entry.domPath = "pools" + SEPARATOR_STR + std::string((char*)mirrorId) + SEPARATOR_STR + "doms" + SEPARATOR + domId;
				entry.dataPoolPath = "pools" + SEPARATOR_STR + std::string((char*)mirrorId) + SEPARATOR_STR + "data";
				data.vmList[imageMetadata.child("VmName").child_value()].push_back(entry);
			}
			
		}
	}

	return data;
}

LTRMetadataSubFS::volumeEntry LTRMetadataSubFS::findVM(const std::string& path)
{
	std::vector<std::string> substrRes;
	boost::split(substrRes, path, boost::is_any_of(SEPARATOR));
	const auto& metadata = readVpgMetada();
	auto vpgIt = std::find_if(metadata.begin(), metadata.end(), [&substrRes](const vpgData& data){
		return data.timestamp == substrRes[2] && data.vpgName == substrRes[3];
	});

	if (vpgIt != metadata.end())
	{
		auto lst = vpgIt->vmList.at(substrRes[4]);
		for (const auto& volume: lst)
		{
			if (volume.volumeName == substrRes[5])
			{
				return volume;
			}
		}
	}
	return volumeEntry{};
}
