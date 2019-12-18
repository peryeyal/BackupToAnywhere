REPODIR=/mnt/nfs_qa_longrun

mount:
	mkdir $(REPODIR)
	mount -t nfs 10.20.16.31:/nas/SO-NFS $(REPODIR) -o ro
	
unmount:
	umount $(REPODIR)
	rmdir $(REPODIR)