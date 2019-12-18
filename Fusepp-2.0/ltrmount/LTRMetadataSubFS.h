#pragma once
#include "ISubFileSystem.h"

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
	};

	vpgData readVpgXml(const std::string& path);

	void onGeneralView(std::vector<std::string>& result);
};

