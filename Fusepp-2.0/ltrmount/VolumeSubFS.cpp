#include "VolumeSubFS.h"
#include "LtrRandomAccessReader.h"


VolumeSubFS::VolumeSubFS(std::string repository_path, std::string volume_name, size_t size_in_bytes, std::string dom_path, std::string datapool_path) : 
	repository_path(repository_path),
	volume_name(volume_name),
	size_in_bytes(size_in_bytes), 
	dom_path(dom_path), 
	datapool_path(datapool_path)
{
}

std::tuple<FileType, size_t> VolumeSubFS::getattr(const char *) {
// if it's a glacier path, we should return directory and delegate to the glacierSubFS
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
