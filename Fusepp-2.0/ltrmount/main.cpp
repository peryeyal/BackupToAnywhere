#include "RootSubFS.h"

int main()
{
	RootSubFS Wrapper(std::string("E:\\"));
	Wrapper.readdir("/");

	return 0;
}
