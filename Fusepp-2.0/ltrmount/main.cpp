
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
	Wrapper.readdir("\\general");
	return 0;
}
