// LTR filesystem class implementation

#include "LTRFS.h"

#include <iostream>
#include <stdio.h>

// include in one .cpp file
#include "Fuse-impl.h"
#include "LTRRepositoryWrapper.h"

//static const char *ltr_path = "/ltr";


static LTRRepositoryWrapper rootSubFS("/mnt/smb_qa_longrun/");

int LTRFS::getattr(const char *path, struct stat *stbuf)
{
        std::tuple<FileType, size_t> res = rootSubFS.getattr(path);
        memset(stbuf, 0, sizeof(struct stat));
        if (std::get<0>(res) == FileType::Directory) {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
        } else {
                stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;
                stbuf->st_size = std::get<1>(res);
        }
//      in case no file existS          res = -ENOENT;

	return 0;
}

int LTRFS::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			               off_t, struct fuse_file_info *)
{
	std::vector<std::string> dirlist = rootSubFS.readdir(path);
	for(const auto& item : dirlist) {
		filler(buf, item.c_str(), NULL, 0);
	}
	if (dirlist.empty())
		return -ENOENT;

	return 0;
}


int LTRFS::open(const char *path, struct fuse_file_info *fi)
{
/*
	if (strcmp(path, ltr_path) != 0)
	#	return -ENOENT;

	#if ((fi->flags & 3) != O_RDONLY)
	#	return -EACCES;
*/
	return 0;
}


int LTRFS::read(const char *path, char *buf, size_t size, off_t offset,
		              struct fuse_file_info *fi)
{
	return rootSubFS.read(path, buf, size, offset);
}
