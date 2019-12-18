
#include "RootSubFS.h"

int main()
{
	RootSubFS Wrapper(std::string("C:\\temp\\"));
	Wrapper.readdir("\\general\\2019-09-18__02_06");

	return 0;
}
