#==============================================================
#
#  Makefile for SatLogic
#  (Colin Burdine, last edited 5/19/2021)
#
#==============================================================
BENCHMARKDATA=./data/medium/*.cnf
BINARY=./bin/satlogic

# builds c binary only
build: src/main.c
	gcc -Wall -p -g -o $(BINARY) \
	src/main.c \
	src/preprocessor/*.c \
	src/dpll/*.c \
	src/model/*.c \
	src/stochastic/*.c \
	src/klib/*.c \
	-lm

#builds c in debug/sanitize mode
debug: src/main.c
	gcc -Wall -p -g -fsanitize=address -o $(BINARY) \
	src/main.c \
	src/preprocessor/*.c \
	src/dpll/*.c \
	src/model/*.c \
	src/stochastic/*.c \
	src/klib/*.c \
	-lm

# runs test cases on c binary:
test: $(BINARY)
	$(BINARY) --test

# does gprof profiling- stores result in bin/profile.txt
profile: bin/solver_c_test
	$(BINARY) $(BENCHMARKDATA) > /dev/null
	gprof $(BINARY) ./gmon.out > ./bin/profile.txt
	rm -f gmon.out
