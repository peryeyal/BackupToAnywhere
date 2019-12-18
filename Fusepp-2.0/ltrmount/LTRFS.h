// LTR filesystem class definition

#ifndef __LTRFS_H_
#define __LTRFS_H_

#include "Fuse.h"

#include "Fuse-impl.h"

class LTRFS : public Fusepp::Fuse<LTRFS>
{
public:
  LTRFS() {}

  ~LTRFS() {}

  static int getattr (const char *, struct stat *);

  static int readdir(const char *path, void *buf,
                     fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info *fi);
  
  static int open(const char *path, struct fuse_file_info *fi);

  static int read(const char *path, char *buf, size_t size, off_t offset,
                  struct fuse_file_info *fi);
};

#endif
