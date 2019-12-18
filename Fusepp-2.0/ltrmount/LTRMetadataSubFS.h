#pragma once
#include "ISubFileSystem.h"

class LTRMetadataSubFS : public ISubFileSystem
{
public:
	LTRMetadataSubFS(std::string mountPoint);

private:
	std::string mountPoint;
};

