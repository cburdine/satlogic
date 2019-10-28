#==============================================================
#
# Makefile for DPLL deterministic SAT solver 
#  (Colin Burdine, last edited 10/27/2019)
#
#==============================================================
CGOFILES=main.go

# fast build- this will not build changes in C code!
build: main.go solver/dpll.c solver/dpll.h
	go build -o ./bin/gosat

# safer build- builds C and Go source files frrom scratch
buildall: main.go solver/dpll.c solver/dpll.h
	go build -a -o ./bin/gosat

# installs the relevant files in your $GOBIN
install: main.go solver/dpll.c solver/dpll.h
	go install

# builds test cases in C into an executable for profiling
ctest: solver/dpll.c solver/dpll.h solver/dpll_test.c
	gcc -Wpedantic -pg -o bin/solver_c_test solver/*.c

# does gprof profiling- stores result in bin/profile.txt
cprof: bin/solver_c_test
	./bin/solver_c_test > /dev/null
	gprof ./bin/solver_c_test ./gmon.out > ./bin/profile.txt
	rm -f gmon.out