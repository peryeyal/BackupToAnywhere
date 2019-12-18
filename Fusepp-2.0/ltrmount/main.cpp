#include "LTRRepositoryWrapper.h"

int main()
{
	LTRRepositoryWrapper Wrapper(std::string("E:\\"));
	Wrapper.readdir("/");

	return 0;
}