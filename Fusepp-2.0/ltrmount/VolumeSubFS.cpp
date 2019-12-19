#include "VolumeSubFS.h"
#include "LtrRandomAccessReader.h"
#include <boost/algorithm/string/predicate.hpp> 
#include "GlacierSubFS.h"

VolumeSubFS::VolumeSubFS(bool use_simple, std::string repository_path, size_t size_in_bytes, std::string dom_path, std::string datapool_path) :
	glacierFS(repository_path, size_in_bytes, dom_path, datapool_path),
	reader(repository_path, dom_path, datapool_path),
	use_simple(use_simple),
	size_in_bytes(size_in_bytes)
{
}


std::tuple<FileType, size_t> VolumeSubFS::getattr(const char *path) {
	if (glacierFS.shouldDelegate(path)) {
		return glacierFS.getattr(path);
	}

	if (boost::algorithm::ends_with(path, ".vmdk") && !use_simple) {
		return std::make_tuple(FileType::Directory, 0);
	}

	return std::make_tuple(FileType::RegularFile, size_in_bytes);
}

std::vector<std::string> VolumeSubFS::readdir(const char *path) {
	if (boost::algorithm::ends_with(path, ".vmdk") && !use_simple) {
		return glacierFS.readdir(path);
	}
	
	std::vector<std::string> result;
	return result;
}

size_t VolumeSubFS::read(const char *path, char *buf, size_t size, size_t offset) {
	if (glacierFS.shouldDelegate(path)) {
		return glacierFS.read(path, buf, size, offset);
	}

	if (use_simple) {
		return reader.read(buf, size, offset);
	}

	return 0;
}

bool VolumeSubFS::shouldDelegate(const char *path) {
	return (std::string(path).find(".vmdk") != std::string::npos);
}
