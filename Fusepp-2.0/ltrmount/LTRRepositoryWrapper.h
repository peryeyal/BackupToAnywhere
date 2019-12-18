#pragma once

#include <tuple>
#include <string>
#include <vector>
#include "ISubFileSystem.h"


class LTRRepositoryWrapper : ISubFilesystem
{
public:
	static std::tuple<FileType, size_t> getattr(const char *);

	static std::vector<std::string> readdir(const char *path);

	static size_t read(const char *path, char *buf, size_t size, size_t offset);

private:
	
	
	struct vpgData
	{
		std::string timestamp;
		std::string vpgName;
	};

	static vpgData readVpgXml(const std::string& path);

	static void onGeneralView(std::vector<std::string>& result);
};


