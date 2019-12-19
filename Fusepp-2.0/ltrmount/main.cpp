
#include "RootSubFS.h"

int main()
{
	RootSubFS Wrapper(std::string("C:\\temp\\"));
	Wrapper.readdir("\\general\\2019-09-18__02_06\\VPG 664GB - 11 volumes to SO-SMBv2 03:00\\W2K12R2_15GB_Ziva_HighIOs(2)New\\W2K12R2_15GB_Ziva_HighIOs(2)New_11.vmdk");
	return 0;
}
