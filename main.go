package main

// #include <solver/dpll.c>
import "C"

import (
	"flag"
	"fmt"
)

var SupportedSolvers = map[string]bool{"dpll": true}

func main() {

	solver := flag.String("solver", "dpll", "the solver to be used")

	flag.Parse()
	files := flag.Args()

	if _, ok := SupportedSolvers[*solver]; !ok {
		fmt.Printf("Solver \"%s\" not supported. Supported solvers are:\n", solver)
		fmt.Println(SupportedSolvers)
		return
	}

	if len(files) == 0 {
		fmt.Println("No files given; expected a list of .cnf files as args")
		return
	}

	for _, file := range files {
		// parse formula:
		formulaData, err := ParseCnf(file)
		if err != nil {
			panic(err)
		}

		cIntSentence := make([]C.int, len(formulaData.sentence), len(formulaData.sentence))
		for k, v := range formulaData.sentence {
			cIntSentence[k] = C.int(v)
		}

		// evaluate sentence:
		fmt.Printf("[%s]: ", file)
		result := C.dpll3Sat(&cIntSentence[0], C.int(len(cIntSentence)))

		// print result
		resultStatus := "SATISFIABLE"
		if result == 0 {
			resultStatus = "UNSATISFIABLE"
		}
		fmt.Println(resultStatus)

	}

}
