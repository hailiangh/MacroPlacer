# Gurobi library
GUROBIPATH=/home/steven/Documents/gurobi952/linux64
GUROBIINC=$(GUROBIPATH)/include/
GUROBILIB=$(GUROBIPATH)/lib -lgurobi_c++ -lgurobi95

CC = /usr/bin/g++ 
BINARY=main
# CODEDIRS=. ./db ./global ./util
CODEDIRS=./src
INCDIRS=. ./Eigen ../boost_1_74_0 ../boost_1_74_0/boost $(GUROBIINC) # can be list

OPT=-O3
# generate files that encode make rules for the .h dependencies
DEPFLAGS=-MP -MD
# automatically add the -I onto each include directory
CFLAGS=-Wall -Wextra -pedantic -g $(foreach D,$(INCDIRS),-I$(D)) $(OPT) $(DEPFLAGS)
CFLAGS+= -w --std=c++17 -lm -ldl -m64 -pg -lemon -D DEBUG -D SANITY_CHECK # -D CONTEST_BENCHMARK ## O3 optimization with debug added.
# CFLAGS+= -D CONTEST_BENCHMARK

# Warning about unused code. Ref: https://stackoverflow.com/questions/4813947/how-can-i-know-which-parts-in-the-code-are-never-used/.
CFLAGS+= -Wunused

LDFLAGS=-L$(GUROBILIB) -lm

# for-style iteration (foreach) and regular expression completions (wildcard)
CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))
# regular expression replacement
OBJECTS=$(patsubst %.cpp,%.o,$(CFILES))
DEPFILES=$(patsubst %.cpp,%.d,$(CFILES))

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

# only want the .c file dependency here, thus $< instead of $^.
#
%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BINARY) $(OBJECTS) $(DEPFILES)

# shell commands are a set of keystrokes away
distribute: clean
	tar zcvf dist.tgz *

# @ silences the printing of the command
# $(info ...) prints output
diff:
	$(info The status of the repository, and the volume of per-file changes:)
	@git status
	@git diff --stat

# include the dependencies
-include $(DEPFILES)
