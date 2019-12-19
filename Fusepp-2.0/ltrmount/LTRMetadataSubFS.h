#pragma once
#include "ISubFileSystem.h"
#include "VolumeSubFS.h"
#include <set>
#include <list>
#include <map>

class LTRMetadataSubFS : public ISubFileSystem
{
public:
	LTRMetadataSubFS(bool useSimpleVmdk, std::string mount_point, std::string fuse_path);
	std::vector<std::string> readdir(const char *path);
	std::tuple<FileType, size_t> getattr(const char *);
	size_t read(const char *path, char *buf, size_t size, size_t offset);

	bool shouldDelegate(const char *path);

private:
	bool simple_vmdk;
	std::string mountPoint;
	std::string fuse_path;
	VolumeSubFS dummyVolumeSubFS;

	struct volumeEntry
	{
		std::string volumeName;
		std::size_t volumeSize;
		std::string dataPoolPath;
		std::string domPath;
		
	};


	struct vpgData
	{
		std::string timestamp;
		std::string vpgName;
		std::string backupSetId;
		std::map<std::string, std::list<volumeEntry>> vmList;

		bool operator<(const vpgData& data) const
		{
			if (timestamp < data.timestamp) return true;
			if (timestamp > data.timestamp) return false;
			if (vpgName < data.vpgName) return true;
			if (vpgName > data.vpgName) return false;
			
			return false;
		}

		bool operator<=(const vpgData& data) const
		{
			if (timestamp <= data.timestamp) return true;
			if (timestamp > data.timestamp) return false;
			if (vpgName <= data.vpgName) return true;
			if (vpgName > data.vpgName) return false;
			return false;
		}

		bool operator==(const vpgData& data) const
		{
			if (timestamp == data.timestamp && vpgName == data.vpgName && backupSetId == data.backupSetId) return true;
			return false;
		}

		bool operator!=(const vpgData& data) const
		{
			return !(*this == data);
		}

	};
	VolumeSubFS createVolumeSubFS(const char *path);

	vpgData readVpgXml(const std::string& path);
	std::string readImageXml(const std::string& path, const std::string& mirrorId, const std::string& domFileId);
	volumeEntry readVolumeXml(const std::string& path, const std::string& volumePathId, const std::string& unitNumber);

	void readHighLevelDir(std::vector<std::string>& result);
	void readDatesDir(std::vector<std::string>& result, const std::string& timestamp);
	void readVpgDir(std::vector<std::string>& result, const std::string& vpgName);
	void readVmDir(std::vector<std::string>& result, const char *path);

	std::list<LTRMetadataSubFS::volumeEntry> findVM(const std::string& path);
	volumeEntry findVolume(const std::string& path);
	const std::set<vpgData>& readVpgMetada();
};

