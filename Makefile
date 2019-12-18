REPODIR=/mnt/smb_qa_longrun

mount:
	mkdir $(REPODIR)
	mount -t cifs //10.20.16.31/Share_2/ltr-longrun $(REPODIR) -o ro,username=SMBadmin,password=zertodata
	
unmount:
	umount $(REPODIR)
	rmdir $(REPODIR)