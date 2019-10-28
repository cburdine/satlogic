#==============================================================
#
# Makefile for DPLL deterministic SAT solver 
#  (Colin Burdine, last edited 10/27/2019)
#
#==============================================================
CGOFILES=main.go


build: main.go solver/dpll.c solver/dpll.h
	go build -o ./bin/gosat

buildall: main.go solver/dpll.c solver/dpll.h
	go build -a -o ./bin/gosat

install: main.go solver/dpll.c solver/dpll.h
	go install

ctest: solver/dpll.c solver/dpll.h solver/dpll_test.c
	gcc -Wpedantic -pg -o bin/solver_c_test solver/*.c

cprof: bin/solver_c_test
	./bin/solver_c_test > /dev/null
	gprof ./bin/solver_c_test ./gmon.out > ./bin/profile.txt
	rm -f gmon.out