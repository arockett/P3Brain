#
# Makefile for the P3Brain initiative
#

# List all directories here with source or include files
VPATH = P3Brain \
		P3Brain/FastEfficientP3/src \
		BaseCode \
		Utilities \
		Worlds \
		Analyse

# List all files to ignore (best for excluding unwanted 'main' files
IGNORES = main.cpp \
		  P3Brain/FastEfficientP3/src/main.cpp

# Specify the output directories
BUILD=build
RELEASE=$(BUILD)/release
DEBUG=$(BUILD)/debug

# Compiler commands/arguments
CXX=g++
INC_PARAMS=$(VPATH:%=-I %)
CXX_RELEASE_FLAGS=-Wall -std=c++11 -O3
CXX_DEBUG_FLAGS=-Wall -g -std=c++11

# Use make functions to get the paths of each .h, and .cpp file
_DEPS=$(foreach dir,$(VPATH),$(wildcard $(dir)/*.h))
_SRC=$(foreach dir,$(VPATH),$(wildcard $(dir)/*.cpp))

# Filter out any unwanted files
DEPS=$(filter-out $(IGNORES),$(_DEPS))
SRC=$(filter-out $(IGNORES),$(_SRC))

# Generate the names of the .o files we need to make
_OBJ=$(SRC:.cpp=.o)
RELEASE_OBJ=$(foreach p,$(_OBJ),$(RELEASE)/$(notdir $(p)))
DEBUG_OBJ=$(foreach p,$(_OBJ),$(DEBUG)/$(notdir $(p)))

RELEASE_TARGET=p3brain
DEBUG_TARGET=$(DEBUG)/p3brain

#
# Targets
#
release: release_dir $(RELEASE_TARGET)

debug: debug_dir $(DEBUG_TARGET)

$(RELEASE_TARGET): $(RELEASE_OBJ)
	$(CXX) $(CXX_RELEASE_FLAGS) $^ -o $@

$(DEBUG_TARGET): $(DEBUG_OBJ)
	$(CXX) $(CXX_DEBUG_FLAGS) $^ -o $@

$(RELEASE_OBJ): $(RELEASE)/%.o: %.cpp $(DEPS)
	$(CXX) $(CXX_RELEASE_FLAGS) $(INC_PARAMS) -c $(filter %.cpp, $<) -o $@

$(DEBUG_OBJ): $(DEBUG)/%.o: %.cpp $(DEPS)
	$(CXX) $(CXX_DEBUG_FLAGS) $(INC_PARAMS) -c $(filter %.cpp, $<) -o $@

.PHONY: clean junkless release_dir debug_dir objects

clean:
	rm -f $(RELEASE_TARGET) $(DEBUG_TARGET) $(RELEASE)/*.o $(DEBUG)/*.o $(VPATH:%=%/*~)

junkless:
	rm -f $(VPATH:%=%/*~)

release_dir:
	mkdir -p $(RELEASE)

debug_dir:
	mkdir -p $(DEBUG)


