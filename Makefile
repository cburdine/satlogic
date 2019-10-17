#==============================================================
#
# Makefile for DPLL deterministic SAT solver 
#  (Colin Burdine, last edited 10/16/2019)
#
#==============================================================
BUILD_TARGET_NAME = dpll
BUILD_STD = gnu89
BUILD_FLAGS = -Wpedantic -g

build: dpll.c dpll_test.c dpll.h
	gcc -std=$(BUILD_STD) $(BUILD_FLAGS) -o $(BUILD_TARGET_NAME) dpll.c dpll_test.c