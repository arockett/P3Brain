#
# Generic Makefile that detects C++ files and builds them
#
# Created by Aaron Beckett
# Thanks to Brian W. Goldman for implementing dependency files
#

# List all directories here with source or include files
VPATH = P3Brain \
		P3Brain/FastEfficientP3/src \
		Analyze \
		Archivist \
		Brain \
		Gate \
		GateListBuilder \
		Genome \
		Group \
		Optimizer \
		Organism \
		Utilities \
		World

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
CXX_RELEASE_FLAGS=-Wall -std=c++11 -O3 $(INC_PARAMS)
CXX_DEBUG_FLAGS=-Wall -g -p -std=c++11 $(INC_PARAMS)

# Find all .cpp files in all source folders
_SRC=$(foreach dir,$(VPATH),$(wildcard $(dir)/*.cpp))
_SRC+=$(wildcard *.cpp)

# Filter out any unwanted files
SRC=$(filter-out $(IGNORES),$(_SRC))

# Generate the names of the .o files we need to make
_OBJS=$(notdir $(SRC:.cpp=.o))
RELEASE_OBJ=$(_OBJS:%=$(RELEASE)/%)
DEBUG_OBJ=$(_OBJS:%=$(DEBUG)/%)

RELEASE_TARGET=p3brain
DEBUG_TARGET=p3brain-debug

#
# Targets
#
release: $(RELEASE_DIR) $(RELEASE_TARGET)

debug: $(DEBUG_DIR) $(DEBUG_TARGET)

# pull in dependency info for *existing* .o files
-include $(addprefix $(RELEASE)/, $(OBJS:.o=.d))
-include $(addprefix $(DEBUG)/, $(OBJS:.o=.d))

$(RELEASE_TARGET): $(RELEASE_OBJ)
	$(CXX) $(CXX_RELEASE_FLAGS) $^ -o $@

$(DEBUG_TARGET): $(DEBUG_OBJ)
	$(CXX) $(CXX_DEBUG_FLAGS) $^ -o $@

$(RELEASE_OBJ): $(RELEASE)/%.o: %.cpp $(DEPS)
	$(CXX) $(CXX_RELEASE_FLAGS) -c $(filter %.cpp, $<) -o $@ -MMD -MF"$(@:%.o=%.d)" -MT"$@"

$(DEBUG_OBJ): $(DEBUG)/%.o: %.cpp $(DEPS)
	$(CXX) $(CXX_DEBUG_FLAGS) -c $(filter %.cpp, $<) -o $@ -MMD -MF"$(@:%.o=%.d)" -MT"$@"


.PHONY: $(RELEASE_DIR) $(DEBUG_DIR) clean extra_clean junkless

$(RELEASE_DIR):
	mkdir -p $(RELEASE)

$(DEBUG_DIR):
	mkdir -p $(DEBUG)

clean: junkless
	rm -f $(RELEASE_TARGET) $(DEBUG_TARGET)
	rm -f $(RELEASE)/*.o $(DEBUG)/*.o
	rm -f $(RELEASE)/*.d $(DEBUG)/*.d

extra_clean: clean
	rm -rf $(BUILD)

junkless:
	rm -f *~ $(VPATH:%=%/*~)

