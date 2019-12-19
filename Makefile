SMBREPODIR=/mnt/smb_qa_longrun
NFSREPODIR=/mnt/nfs_qa_longrun
LTRFACADE=LtrFacade4
FACADEBIN=Fusepp-2.0/ltrmount


build:
	${MAKE} -C $(FACADEBIN)

clean:
	${MAKE} -C $(FACADEBIN) clean

facade: build
	mkdir $(LTRFACADE)	
	$(FACADEBIN)/LTRMount -f $(LTRFACADE)
	#ind $(LTRFACADE)

unfacade:
	pkill LTRMount
	sleep 2
	rmdir $(LTRFACADE)	

mount-smb:
	mkdir $(SMBREPODIR)
	mount -t cifs //10.20.16.31/Share_2/ltr-longrun $(SMBREPODIR) -o ro,username=SMBadmin,password=zertodata
	
unmount-smb:
	umount $(SMBREPODIR)
	rmdir $(SMBREPODIR)
	
mount-nfs:
	mkdir $(NFSREPODIR)
	mount -t nfs 10.20.16.31:/nas/SO-NFS $(NFSREPODIR) -o ro
	
unmount-nfs:
	umount $(NFSREPODIR)
	rmdir $(NFSREPODIR)
