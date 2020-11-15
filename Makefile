# Directories
SRCDIR     := src
TESTDIR    := test
INCDIR     := inc
OBJDIR     := obj
BINDIR     := bin
TESTBINDIR := bin_test

# Flags, Libraries, and Includes
CFLAGS  := -Wall
INC     := -I$(INCDIR) -Isrc
TESTLIB := -lcheck -lm -lpthread

# Targets
SRCS     := $(shell find $(SRCDIR) -type f -name *.c)
SRCOBJS  := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.c=.o))
TESTS    := $(shell find $(TESTDIR) -type f -name *.c)
TESTOBJS := $(patsubst $(TESTDIR)/%,$(OBJDIR)/%,$(TESTS:.c=.o))
TESTEXECS = $(shell find $(TESTBINDIR) -type f)

# Default Make
all: directories link

# Make the directories
directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)
	@mkdir -p $(TESTBINDIR)

# Link the main binary
link: $(SRCOBJS)
	@echo linking main binary
	@mkdir -p $(dir $@)
	@$(CC) -o $(patsubst $(OBJDIR)/%,$(BINDIR)/%,$(<:.o=)) $^ $(LIB)

# Pull in dependency info for existing source files
-include $(SRCOBJS:.o=.d)

# Compile and assemble source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo assembling $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INC) -MM $(SRCDIR)/$*.c > $(OBJDIR)/$*.d
	@cp -f $(OBJDIR)/$*.d $(OBJDIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.d.tmp > $(OBJDIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.d
	@rm -f $(OBJDIR)/$*.d.tmp

# Run tests
test: link_tests
	@$(foreach obj,$(TESTEXECS), \
		$(obj); \
	)

# Link all test binaries
link_tests: $(TESTOBJS) $(SRCOBJS)
	@echo linking test binaries
	@$(foreach obj,$(TESTOBJS), \
		mkdir -p $(patsubst $(OBJDIR)/%,$(TESTBINDIR)/%,$(dir $(obj))); \
		$(CC) -o $(patsubst $(OBJDIR)/%,$(TESTBINDIR)/%,$(obj:.o=)) $(obj) $(filter-out obj/main.o,$(SRCOBJS)) $(LIB) $(TESTLIB); \
	)

# Compile and assemble test files
$(OBJDIR)/%.o: $(TESTDIR)/%.c
	@echo assembling $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) -c -o $@ $<
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
	@$(RM) -rf $(BINDIR) $(TESTBINDIR)

# Non-File targets
.PHONY: all remake clean cleaner

