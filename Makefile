# Directories
INCDIR     := inc
OBJDIR     := obj
SRCDIR     := src
BINDIR     := bin
TESTDIR    := test

# Flags, Libraries, and Includes
CFLAGS  := -Wall -I$(INCDIR) -I.
TESTLIB := -lcheck -lm -lpthread

# Targets
SRCS     := $(shell find $(SRCDIR) -type f -name *.c)
SRCOBJS  := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.c=.o)) \
	$(patsubst $(TESTDIR)/%,$(OBJDIR)/%,$(TESTS:.c=.o))
TESTS    := $(shell find $(TESTDIR) -type f -name *.c)
TESTOBJS := $(patsubst $(TESTDIR)/%,$(OBJDIR)/%,$(TESTS:.c=.o))

# Default Make
all: directories link
	@echo srcs: $(SRCS)
	@echo srcobjs: $(SRCOBJS)
	@echo tests: $(TESTS)
	@echo testobjs: $(TESTOBJS)

# Make the directories
directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

# Link the object files into one binary
# Compile to the bin directory, replacing the obj in the prefix
link: $(SRCOBJS)
	@echo target object: $<
	@echo dependent objects: $^
	$(CC) -o $(patsubst $(OBJDIR)/%,$(BINDIR)/%,$(<:.o=)) $^ $(LIB) $(TESTLIB)

# Pull in dependency info for *existing* .o files
-include $(SRCOBJS:.o=.d)

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.c > $(OBJDIR)/$*.d
	@cp -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp

# Remake
remake: cleaner all

# Clean only objects
clean:
	@$(RM) -rf $(OBJDIR)

# Full clean, objects and binaries
cleaner: clean
	@$(RM) -rf $(BINDIR)

# Non-File targets
.PHONY: all remake clean cleaner

