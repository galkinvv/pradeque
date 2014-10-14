#!/usr/bin/make -f

ifndef CXX11
	CXX11=g++-4.8
endif
ifndef CC_FORCXX
	CC_FORCXX=gcc-4.8
endif
ifndef OPTIMIZE
	OPTIMIZE = 0
endif
ifndef BUILDDIR
	BUILDDIR = build
endif

GCC_WARNINGS=-Wall -Wextra -Wuninitialized -W -Wparentheses -Wformat=2 -Wswitch-default -Wcast-align -Wpointer-arith -Wwrite-strings -Wstrict-aliasing=2
GCC_WARNINGS_OFF=-Wno-missing-field-initializers -Wno-format-nonliteral -Wno-unknown-pragmas -Wno-reorder
ALL_CXX_LANG_FLAGS=$(GCC_WARNINGS_OFF) $(GCC_WARNINGS) -std=c++11

CXXFLAGS = $(ALL_CXX_LANG_FLAGS) -O$(OPTIMIZE) -g -march=native -mtune=native -MD -MP -ffunction-sections -fdata-sections
#CXXFLAGS = -g -pg -O3 -march=native -mtune=native

LDFLAGS = -O$(OPTIMIZE) -g 
#-Wl,--gc-sections
#-Wl,--gc-sections -Wl,--print-gc-sections
#LDFLAGS = -O3 -g -pg

ifeq ($(OS),Windows_NT)
RM=del
BINEXT=.exe
else
BINEXT=
RM=rm -rvf
endif

LD=$(CXX11) -std=c++11

OBJDIR=$(BUILDDIR)/obj
TMPDIR=$(BUILDDIR)/tmp


all: $(BUILDDIR)/run-gt$(BINEXT)

TEST_SOURCES=$(wildcard libtests/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:libtests/%.cpp=$(OBJDIR)/libtests/%.o) $(OBJDIR)/libtests/gtest-all.o $(OBJDIR)/libtests/gtest_main.o 

$(BUILDDIR)/run-gt$(BINEXT): $(TEST_OBJECTS) 
	$(LD) -pthread $^ -L $(OBJDIR) -o $@ $(LDFLAGS)

$(OBJDIR)/libtests/%.o: libtests/%.cpp 3rd/gtest/src/gtest-all.cc
	mkdir -p $(dir $@)
	$(CXX11) $(CXXFLAGS) -I . -I 3rd/gtest -I 3rd/gtest/include -c $< -o $@

$(OBJDIR)/libtests/gtest-all.o: 3rd/gtest/src/gtest-all.cc
	mkdir -p $(dir $@)
	$(CXX11) $(CXXFLAGS) -I . -I 3rd/gtest -I 3rd/gtest/include -c $< -o $@

$(OBJDIR)/libtests/gtest_main.o: 3rd/gtest/src/gtest_main.cc 
	mkdir -p $(dir $@)
	$(CXX11) $(CXXFLAGS) -I . -I 3rd/gtest -I 3rd/gtest/include -c $< -o $@

clean:
	$(RM) $(BUILDDIR)
FD=$(shell pwd)
QUICKCOMPILE_OPTIONS = $(ALL_CXX_LANG_FLAGS) -I $(FD) -I $(FD)/3rd/gtest -I $(FD)/3rd/gtest/include -S -o /dev/null

quickcompile_options: 
	echo $(QUICKCOMPILE_OPTIONS) 

quickcompile: 3rd/gtest/src/gtest-all.cc
	$(CXX11) -x c++ $(QUICKCOMPILE_OPTIONS) $(QUICK_SOURCE)

3rd/gtest/src/gtest_main.cc: 3rd/gtest/src/gtest-all.cc
3rd/gtest/src/gtest-all.cc:
	rm -rf 3rd/gtest/ $(TMPDIR)/gtest.zip tmp/gtest_build
	mkdir -p $(TMPDIR)/gtest_build
	wget http://googletest.googlecode.com/files/gtest-1.7.0.zip -O $(TMPDIR)/gtest.zip
	unzip $(TMPDIR)/gtest.zip -d $(TMPDIR)/gtest_build
	mkdir -p 3rd/
	mv $(TMPDIR)/gtest_build/gtest* 3rd/gtest

check: $(BUILDDIR)/run-gt$(BINEXT)
	$^

-include $(shell find $(BUILDDIR) -name '*.d')
