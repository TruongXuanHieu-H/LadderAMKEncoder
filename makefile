OBJDIR=build
OBJECTS= utils.o scl_encoder.o bdd.o duplex_encoder.o product_encoder.o card_encoder.o bdd_encoder.o naive_encoder.o seq_encoder.o reduced_encoder.o encoder.o clause_cont.o cadical_clauses.o ladder_amo_encoder.o
OBJS = $(patsubst %.o,$(OBJDIR)/%.o,$(OBJECTS))

SRCDIR=src

# FLAGS= -Wall -Werror -Wextra -O3 -DNDEBUG
# FLAGS= -Wall -Werror -Wextra -g -O0

DEBUG_FLAGS= -Wall -Werror -Wextra -g -O0
RELEASE_FLAGS= -Wall -Werror -Wextra -O3 -DNDEBUG

# Default mode: release
FLAGS= $(RELEASE_FLAGS)

# Target build debug
debug: FLAGS=$(DEBUG_FLAGS)
debug: clean all

IGNORE_ASSERTVARS= -Wno-unused-but-set-variable
STANDARD= -std=c++11

CADICAL_INC=./cadical/
CADICAL_LIB_DIR=./cadical/
CADICAL_LIB=-lcadical

all : $(OBJDIR)/main.o
	cp src/card_amo.py $(OBJDIR)
	cp src/bdd_amo.py $(OBJDIR)
	g++ $(FLAGS) $(OBJDIR)/main.o $(OBJS) -L$(CADICAL_LIB_DIR) $(CADICAL_LIB) -o build/ladder_amo_enc

$(OBJDIR)/main.o : main.cpp $(OBJS) $(SRCDIR)/ladder_amo_encoder.h
	g++ $(FLAGS) $(STANDARD) -I$(CADICAL_INC) -c $< -o $@

$(OBJDIR)/ladder_amo_encoder.o : $(SRCDIR)/ladder_amo_encoder.cpp $(SRCDIR)/ladder_amo_encoder.h $(SRCDIR)/scl_encoder.h $(SRCDIR)/duplex_encoder.h $(SRCDIR)/product_encoder.h $(SRCDIR)/card_encoder.h $(SRCDIR)/bdd_encoder.h $(SRCDIR)/reduced_encoder.h $(SRCDIR)/naive_encoder.h $(SRCDIR)/seq_encoder.h $(SRCDIR)/utils.h $(SRCDIR)/clause_cont.h $(SRCDIR)/cadical_clauses.h
	g++ $(FLAGS) $(STANDARD) -I$(CADICAL_INC) -c $< -o $@

$(OBJDIR)/scl_encoder.o : $(SRCDIR)/scl_encoder.cpp $(SRCDIR)/scl_encoder.h $(SRCDIR)/encoder.h
	g++ $(FLAGS) $(STANDARD) -c $< -o $@

$(OBJDIR)/duplex_encoder.o : $(SRCDIR)/duplex_encoder.cpp $(SRCDIR)/duplex_encoder.h $(SRCDIR)/encoder.h $(SRCDIR)/bdd.h
	g++ $(FLAGS) $(IGNORE_ASSERTVARS) $(STANDARD) -c $< -o $@

$(OBJDIR)/product_encoder.o : $(SRCDIR)/product_encoder.cpp $(SRCDIR)/product_encoder.h $(SRCDIR)/encoder.h
	g++ $(FLAGS) $(STANDARD) -c $< -o $@

$(OBJDIR)/card_encoder.o : $(SRCDIR)/card_encoder.cpp $(SRCDIR)/card_encoder.h $(SRCDIR)/encoder.h
	g++ $(FLAGS) $(STANDARD) -c $< -o $@

$(OBJDIR)/bdd_encoder.o : $(SRCDIR)/bdd_encoder.cpp $(SRCDIR)/bdd_encoder.h $(SRCDIR)/encoder.h
	g++ $(FLAGS) $(STANDARD) -c $< -o $@

$(OBJDIR)/reduced_encoder.o : $(SRCDIR)/reduced_encoder.cpp $(SRCDIR)/reduced_encoder.h $(SRCDIR)/encoder.h $(SRCDIR)/bdd.h
	g++ $(FLAGS) $(IGNORE_ASSERTVARS) $(STANDARD) -c $< -o $@

$(OBJDIR)/naive_encoder.o : $(SRCDIR)/naive_encoder.cpp $(SRCDIR)/naive_encoder.h $(SRCDIR)/encoder.h $(SRCDIR)/bdd.h
	g++ $(FLAGS) $(IGNORE_ASSERTVARS) $(STANDARD) -c $< -o $@

$(OBJDIR)/seq_encoder.o : $(SRCDIR)/seq_encoder.cpp $(SRCDIR)/seq_encoder.h $(SRCDIR)/encoder.h $(SRCDIR)/bdd.h
	g++ $(FLAGS) $(IGNORE_ASSERTVARS) $(STANDARD) -c $< -o $@

$(OBJDIR)/cadical_clauses.o : $(SRCDIR)/cadical_clauses.cpp $(SRCDIR)/cadical_clauses.h $(SRCDIR)/clause_cont.h
	g++ $(FLAGS) $(STANDARD) -I$(CADICAL_INC) -c $< -o $@

$(OBJDIR)/clause_cont.o : $(SRCDIR)/clause_cont.cpp $(SRCDIR)/clause_cont.h $(SRCDIR)/utils.h
	g++ $(FLAGS) $(STANDARD) -c $< -o $@

$(OBJDIR)/encoder.o : $(SRCDIR)/encoder.cpp $(SRCDIR)/encoder.h $(SRCDIR)/clause_cont.h
	g++ $(FLAGS) $(STANDARD) -c $< -o $@

$(OBJDIR)/utils.o : $(SRCDIR)/utils.cpp $(SRCDIR)/utils.h
	g++ $(FLAGS) $(STANDARD) -c $< -o $@

$(OBJDIR)/bdd.o : $(SRCDIR)/bdd.cpp $(SRCDIR)/bdd.h
	g++ $(FLAGS) $(IGNORE_ASSERTVARS) $(STANDARD) -c $< -o $@

#.PHONY : clean
clean:
	rm -f *.a $(OBJDIR)/*.o *~ *.out  $(OBJDIR)/ladder_amo_enc

tar:
	tar cfv ladder_amo_enc.tar main.cpp makefile $(SRCDIR)/*.cpp $(SRCDIR)/*.h cadical/*.a cadical/*.hpp
