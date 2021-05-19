#==============================================================
#
#  Makefile for SatLogic
#  (Colin Burdine, last edited 5/19/2021)
#
#==============================================================
BENCHMARKDATA=./data/medium/*.cnf

# builds c binary only
build: solver/main.c
	gcc -Wall -p -g -o bin/solver_c_main \
	solver/main.c \
	solver/preprocessor/*.c \
	solver/dpll/*.c \
	solver/model/*.c \
	solver/stochastic/*.c \
	solver/klib/*.c \
	-lm

#builds c in debug/sanitize mode
debug: solver/main.c
	gcc -Wall -p -g -fsanitize=address -o bin/solver_c_main \
	solver/main.c \
	solver/preprocessor/*.c \
	solver/dpll/*.c \
	solver/model/*.c \
	solver/stochastic/*.c \
	solver/klib/*.c \
	-lm

# runs test cases on c binary:
test: bin/solver_c_main
	./bin/solver_c_main --test

# does gprof profiling- stores result in bin/profile.txt
profile: bin/solver_c_test
	./bin/solver_c_test $(BENCHMARKDATA) > /dev/null
	gprof ./bin/solver_c_main ./gmon.out > ./bin/profile.txt
	rm -f gmon.out
