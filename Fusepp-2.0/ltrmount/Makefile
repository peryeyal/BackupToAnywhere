PROG=LTRMount
OBJDIR=.obj
CC=g++

CFLAGS = -Wall --std=c++0x `pkg-config fuse --cflags` -I.. 
LDFLAGS = `pkg-config fuse --libs` -L/usr/lib/x86_64-linux-gnu -lboost_filesystem -lboost_system

$(shell mkdir -p $(OBJDIR)) 

OBJS = $(OBJDIR)/LTRMount.o $(OBJDIR)/LTRFS.o $(OBJDIR)/RootSubFS.o $(OBJDIR)/pugixml.o $(OBJDIR)/LtrRandomAccessReader.o $(OBJDIR)/VolumeSubFS.o $(OBJDIR)/GlacierSubFS.o $(OBJDIR)/LTRMetadataSubFS.o 

all: $(PROG)

$(PROG) : $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(PROG)

-include $(OBJS:.o=.d)


$(OBJDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $*.cpp -o $(OBJDIR)/$*.o
	$(CC) -MM $(CFLAGS) $*.cpp > $(OBJDIR)/$*.d
	@mv -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp

clean:
	rm -rf $(PROG) $(OBJDIR)

