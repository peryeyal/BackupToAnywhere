
#include "RootSubFS.h"

int main()
{
	RootSubFS Wrapper(std::string("C:\\temp\\"));
	Wrapper.readdir("\\general\\2019-09-18__02_06");
	Wrapper.readdir("\\general\\2019-12-12__01_01");
	return 0;
}
