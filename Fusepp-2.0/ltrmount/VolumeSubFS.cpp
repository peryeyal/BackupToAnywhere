#include "VolumeSubFS.h"
#include "LtrRandomAccessReader.h"
#include <boost/algorithm/string/predicate.hpp> 
#include "GlacierSubFS.h"

VolumeSubFS::VolumeSubFS(bool use_simple, std::string repository_path, std::string volume_name, size_t size_in_bytes, std::string dom_path, std::string datapool_path) :
	use_simple(use_simple),
	repository_path(repository_path),
	volume_name(volume_name),
	size_in_bytes(size_in_bytes), 
	dom_path(dom_path), 
	datapool_path(datapool_path)
{
}

//ISubFileSystem VolumeSubFS::createGlacierSubFS(const char *path) {
	//size_t size_in_bytes = 0;
	//GlacierSubFS glacier;
	//return singleVolume;
//}


std::tuple<FileType, size_t> VolumeSubFS::getattr(const char *path) {
	if (boost::algorithm::ends_with(path, ".vdmk")) {
		if (use_simple)
			std::make_tuple(FileType::RegularFile, size_in_bytes);
		else 
			std::make_tuple(FileType::Directory, 0);
	}
	// delegate to Glacier as a folder
	return std::make_tuple(FileType::RegularFile, size_in_bytes);
}

std::vector<std::string> VolumeSubFS::readdir(const char *path) {
	// in case of readdir, should delegate to 
	std::vector<std::string> result;
	return result;
}

size_t VolumeSubFS::read(const char *path, char *buf, size_t size, size_t offset) {
	LtrRandomAccessReader reader(repository_path, dom_path, datapool_path);
	return reader.read(buf, size, offset);
}

bool VolumeSubFS::shouldDelegate(const char *path) {
	return (std::string(path).find(".vmdk") != std::string::npos);
}
