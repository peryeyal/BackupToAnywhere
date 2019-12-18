#pragma once
#include "ISubFileSystem.h"

class LTRMetadataSubFS : public ISubFileSystem
{
public:
	LTRMetadataSubFS(std::string mountPoint);
	std::vector<std::string> readdir(const char *path);

private:
	std::string mountPoint;


	struct vpgData
	{
		std::string timestamp;
		std::string vpgName;
		std::string backupSetId;
	};

	vpgData readVpgXml(const std::string& path);

	void onGeneralView(std::vector<std::string>& result);
};

