#==============================================================
#
# Makefile for DPLL deterministic SAT solver 
#  (Colin Burdine, last edited 10/16/2019)
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
	gcc -Wpedantic -g -o bin/solver_c_test solver/*.c