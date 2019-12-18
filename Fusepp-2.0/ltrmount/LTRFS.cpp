// LTR filesystem class implementation

#include "LTRFS.h"

#include <iostream>

#include <stdio.h>

// include in one .cpp file
#include "Fuse-impl.h"

static const char *ltr_str = "Hello World!\n";
static const char *ltr_path = "/ltr";

int LTRFS::getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, ltr_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(ltr_str);
	} else
		res = -ENOENT;

	return res;
}

int LTRFS::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			               off_t, struct fuse_file_info *)
{
	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, ltr_path + 1, NULL, 0);

	return 0;
}


int LTRFS::open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, ltr_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}


int LTRFS::read(const char *path, char *buf, size_t size, off_t offset,
		              struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	if(strcmp(path, ltr_path) != 0)
		return -ENOENT;

	len = strlen(ltr_str);
	if ((size_t)offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, ltr_str + offset, size);
	} else
		size = 0;

	return size;
}
