#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#

# define the Cpp compiler to use
CXX = g++

# define any compile-time flags:
# -g: Enable debugging systems (disable for production)
# -std=c++11: Comple with C++11 language features
CXXFLAGS	:= -std=c++11 -Wall -Wextra -g

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS =

# define linker flags used while generating the binaries
LDFLAGS =

# define output directory
OUTPUT	:= output

# define source directory
SRC		:= src
EXAMPLES := examples

# define include directory
INCLUDE	:= include

TESTSDIR   := tests

ifeq ($(OS),Windows_NT)
MAIN	:= sample-app.exe
TESTMAIN	:= slog-test.exe
SOURCEDIRS	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)
EXAMPLEDIRS	:= $(EXAMPLES)
MD	:= mkdir
else
MAIN	:= sample-app
TESTMAIN	:= slog-test
SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
EXAMPLEDIRS	:= $(shell find $(EXAMPLES) -type d)
FIXPATH = $1
RM = rm -f
MD	:= mkdir -p
endif

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

# define any directories containing header files other than /usr/include
INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# define the C source files
SOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
EXAMPLES	:= $(wildcard $(patsubst %,%/*.cpp, $(EXAMPLEDIRS)))
TESTS		:= $(wildcard $(patsubst %,%/*.cpp, $(TESTSDIR)))

# define the C object files
SRC_OBJECTS		:= $(SOURCES:.cpp=.o)
EXAMPLE_OBJECTS	:= $(EXAMPLES:.cpp=.o)
TESTS_OBJECTS	:= $(TESTS:.cpp=.o)
OBJECTS         := $(SRC_OBJECTS) $(EXAMPLE_OBJECTS) $(TESTS_OBJECTS)

# define the dependency output files
DEPS		:= $(OBJECTS:.o=.d) $(EXAMPLE_OBJECTS:.o=.d) $(TESTS_OBJECTS:.o=.d)

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))
TESTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(TESTMAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(SRC_OBJECTS) $(EXAMPLE_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(SRC_OBJECTS) $(EXAMPLE_OBJECTS) $(LFLAGS)
$(TESTMAIN): $(OBJECTS) $(TESTS_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TESTMAIN) $(TESTS_OBJECTS) $(SRC_OBJECTS) $(LFLAGS) -lcppunit

# include all .d files
-include $(DEPS)

# this is a suffix replacement rule for building .o's and .d's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# -MMD generates dependency output files same name as the .o file
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN) $(TESTMAIN)
	$(RM) $(call FIXPATH,$(OBJECTS))
	$(RM) $(call FIXPATH,$(DEPS))
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!

install:
	install -d "$(DESTDIR)$(PREFIX)/include"
	install -m 644 include/slog/*.h -D "$(DESTDIR)$(PREFIX)/include/slog/"

test: $(TESTMAIN)
	./$(TESTMAIN)
