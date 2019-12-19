
#include "RootSubFS.h"
#include "LtrRandomAccessReader.h"

int main()
{
	/*
	LtrRandomAccessReader reader(R"(W:)",
		R"(pools\3479958491418911683\doms\0c1fcb77-1e9e-440a-8c5b-dd03ff898a13)",
		R"(pools\3479958491418911683\data)"
	);

	size_t size_to_read = 5*1024*1024;
	std::vector<char> data(size_to_read);
	reader.read(data.data(), size_to_read, 0);
	*/
	RootSubFS Wrapper(std::string("C:\\temp\\"));
	Wrapper.readdir("\\general\\2019-09-18__02_06\\VPG 664GB - 11 volumes to SO-SMBv2 03:00\\W2K12R2_15GB_Ziva_HighIOs(2)New\\W2K12R2_15GB_Ziva_HighIOs(2)New_11.vmdk");
	return 0;
}
