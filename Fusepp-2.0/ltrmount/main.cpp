
#include "RootSubFS.h"

int main()
{
	RootSubFS Wrapper(std::string("C:\\temp\\"));
	Wrapper.readdir("\\general\\2019-12-15__01_00\\VPG - 500GB - 1 volume - StoreOnceSMBv2 03:00");
	return 0;
}
