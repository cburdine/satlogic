package main

// #include <solver/dpll.c>
import "C"

import (
	"flag"
	"fmt"
)

var SupportedSolvers = map[string]bool{"dpll": true}

const SentenceBufferSize = 30000

var sentenceBuffer = make([]C.int, SentenceBufferSize)

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

		for k, v := range formulaData.sentence {
			sentenceBuffer[k] = C.int(v)
		}

		// evaluate sentence:
		fmt.Printf("[%s]: ", file)
		arrptr := (*C.int)(&sentenceBuffer[0])
		result := C.dpll3Sat(arrptr, C.int(len(formulaData.sentence)))

		// print result
		resultStatus := "SATISFIABLE"
		if result == 0 {
			resultStatus = "UNSATISFIABLE"
		}
		fmt.Println(resultStatus)

	}

}
