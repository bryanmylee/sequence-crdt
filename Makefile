# Directories
INCDIR     := inc
OBJDIR     := obj
SRCDIR     := src
BINDIR     := bin
TESTDIR    := test

# Flags, Libraries, and Includes
CFLAGS  := -Wall
INC     := -I$(INCDIR) -I.
TESTLIB := -lcheck -lm -lpthread

# Targets
SRCS     := $(shell find $(SRCDIR) -type f -name *.c)
SRCOBJS  := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.c=.o))
TESTS    := $(shell find $(TESTDIR) -type f -name *.c)
TESTOBJS := $(patsubst $(TESTDIR)/%,$(OBJDIR)/%,$(TESTS:.c=.o))

# Default Make
all: directories link

# Make the directories
directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

# Link the main binary
link: $(SRCOBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $(patsubst $(OBJDIR)/%,$(BINDIR)/%,$(<:.o=)) $^ $(LIB)

# Pull in dependency info for existing source files
-include $(SRCOBJS:.o=.d)

# Compile and assemble source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INC) -MM $(SRCDIR)/$*.c > $(OBJDIR)/$*.d
	@cp -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp

# Run tests
test: link_tests

# Link all test binaries
link_tests: $(TESTOBJS) $(SRCOBJS)
	$(foreach obj,$(TESTOBJS), \
		mkdir -p $(patsubst $(OBJDIR)/%,$(BINDIR)/%,$(dir $(obj))); \
		$(CC) -o $(patsubst $(OBJDIR)/%,$(BINDIR)/%,$(obj:.o=)) $(obj) $(filter-out obj/main.o,$(SRCOBJS)) $(LIB) $(TESTLIB); \
	)

# Compile and assemble test files
$(OBJDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INC) -MM $(TESTDIR)/$*.c > $(OBJDIR)/$*.d
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

