#include "LtrRandomAccessReader.h"



LtrRandomAccessReader::LtrRandomAccessReader(std::string repository_path, std::string dom_path, std::string datapool_path) :
	repository_path(repository_path), dom_path(dom_path), datapool_path(dom_path)
{
}

size_t LtrRandomAccessReader::read(char * buf, size_t size, size_t offset) {
	memset(buf, 2, size);
	return size;
}