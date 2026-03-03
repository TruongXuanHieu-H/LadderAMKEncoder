# ================================
# Project directories
# ================================
SRCDIR  := src
OBJDIR  := build/obj
BINDIR  := build

TARGET  := $(BINDIR)/ladder_amk_enc

# ================================
# Compiler
# ================================
CXX       := g++
STANDARD  := -std=c++23

# ================================
# Compiler flags
# ================================
IGNORE_ASSERTVARS := -Wno-unused-but-set-variable
CXXFLAGS := -Wall -Wextra -Werror -O3 -DNDEBUG $(STANDARD) $(IGNORE_ASSERTVARS)

# ================================
# CaDiCaL
# ================================
CADICAL_INC     := ./cadical
CADICAL_LIB_DIR := ./cadical
CADICAL_LIB     := -lcadical

# ================================ 
# PBLib 
# ================================ 
PBLIB_INC := /usr/local/include 
PBLIB_LIB_DIR := /usr/local/lib 
PBLIB_LIB := -lpb

INCLUDES := -I$(CADICAL_INC) -I$(PBLIB_INC)

# ================================
# Source files
# ================================
SOURCES := \
	main.cpp \
	utils.cpp \
	reduced_encoder.cpp \
	seq_encoder.cpp \
	bdd_encoder.cpp \
	card_encoder.cpp \
	scl_encoder.cpp \
	adder_encoder.cpp \
	encoder.cpp \
	clause_cont.cpp \
	cadical_clauses.cpp \
	ladder_amk_encoder.cpp

# ================================
# Object files
# ================================
OBJECTS := $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# ================================
# Default target
# ================================
.PHONY: all
all: $(TARGET)

# ================================
# Link
# ================================
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -L$(CADICAL_LIB_DIR) $(CADICAL_LIB) -L$(PBLIB_LIB_DIR) $(PBLIB_LIB) -o $@

# ================================
# Compile rules
# ================================
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# main.cpp in root
$(OBJDIR)/main.o: main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ================================
# Clean
# ================================
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET) *.a *~ *.out

# ================================
# Archive
# ================================
.PHONY: tar
tar:
	tar cfv ladder_amk_enc.tar \
		$(SRCDIR) \
		main.cpp \
		makefile \
		$(CADICAL_LIB_DIR)/*.a \
		$(CADICAL_LIB_DIR)/*.hpp