#include "LtrRandomAccessReader.h"
#include <string.h>

#include <boost/filesystem.hpp>

#ifdef _WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

LtrRandomAccessReader::LtrRandomAccessReader(const std::string& repository_path, const std::string& dom_path, const std::string& datapool_path) :
	repository_path_(repository_path), dom_path_(dom_path), datapool_path_(datapool_path)
{
}

size_t LtrRandomAccessReader::read(char * buf, size_t size, size_t offset) {
	
	const size_t dp_record_size = 16;
	const size_t count_of_records_per_dp = 102400;
	const size_t block_size = 128 * 1024;

	const size_t dp_number = offset / (count_of_records_per_dp * block_size);
	const size_t dp_block_number = (offset - dp_number * (count_of_records_per_dp * block_size))/ block_size;

	const std::string dp_file_name = repository_path_ + SEPARATOR + dom_path_ + SEPARATOR + "dp." + std::to_string(dp_number);

	std::ifstream dp_file(dp_file_name, std::fstream::binary);
	dp_file.seekg(dp_block_number * dp_record_size, std::fstream::beg);

	int64_t box_number = 0;
	dp_file.read((char*)&box_number, sizeof(box_number));
	
	int64_t box_offset = 0;
	dp_file.read((char*)&box_offset, sizeof(box_offset));


	const std::string box_file_name = repository_path_ + SEPARATOR + datapool_path_ + SEPARATOR + std::to_string(box_number) + ".box";
	const size_t offset_inside_block = offset % block_size;
	const size_t size_to_read = std::min(block_size - offset_inside_block, size);

	std::ifstream box_file(box_file_name, std::fstream::binary);
	box_file.seekg(box_offset * block_size + offset_inside_block, std::ios::beg);
	box_file.read(buf, size_to_read);
	
	if (size_to_read < size) {
		return size_to_read + read(buf + size_to_read, size - size_to_read, offset + size_to_read);
	}

	return size_to_read;
}
