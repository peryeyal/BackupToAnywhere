#pragma once
#include "ISubFileSystem.h"
#include <set>

class LTRMetadataSubFS : public ISubFileSystem
{
public:
	LTRMetadataSubFS(std::string mount_point, std::string fuse_path);
	std::vector<std::string> readdir(const char *path);
	std::tuple<FileType, size_t> getattr(const char *);
	size_t read(const char *path, char *buf, size_t size, size_t offset);

	bool shouldDelegate(const char *path);

private:
	std::string mountPoint;
	std::string fuse_path;


	struct vpgData
	{
		std::string timestamp;
		std::string vpgName;
		std::string backupSetId;

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

	vpgData readVpgXml(const std::string& path);

	void readHighLevelDir(std::vector<std::string>& result);
	void readDatesDir(std::vector<std::string>& result, const std::string& timestamp);

	const std::set<vpgData>& readVpgMetada();
};

