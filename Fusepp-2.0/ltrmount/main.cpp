#include "LTRRepositoryWrapper.h"

int main()
{
	LTRRepositoryWrapper wrapper(std::string("E:\\"));
	wrapper.readdir("/");
	wrapper.readdir("/glacier");
	wrapper.getattr("/glacier/glacier_upload.py");

	return 0;
}