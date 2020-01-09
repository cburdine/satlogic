package main

// CGO files:

// #include "solver/dpll.c"
// #include "solver/structures.c"
// #cgo LDFLAGS: -lm
import "C"

import (
	"flag"
	"fmt"
)

var SupportedSolvers = map[string]bool{"dpll": true}

const SentenceBufferSize = 30000

var sentenceBuffer = make([]C.int, SentenceBufferSize)

var solnBuffer = make([]C.char, SentenceBufferSize)

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
			//fmt.Printf("%d ", v)
		}

		// evaluate sentence:
		fmt.Printf("[%s]: ", file)
		arrptr := (*C.int)(&sentenceBuffer[0])
		solnptr := (*C.char)(&solnBuffer[0])

		result := C.dpll3Sat(
			arrptr,
			C.int(len(formulaData.sentence)/3),
			solnptr)

		if result == 0 {
			fmt.Println("UNSATISFIABLE")
		} else {
			for i := 1; i < formulaData.maxNumVars; i++ {
				fmt.Printf("%d=%s ", i, cboolToStr(solnBuffer[i]))
			}
			fmt.Println()
		}

	}

}

func cboolToStr(val C.char) string {
	if val == 0 {
		return "F"
	}
	return "T"
}
