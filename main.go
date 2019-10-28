package main

// #include <solver/dpll.c>
import "C"

import (
	"fmt"
)

func main() {

	// parse formula:
	formulaData, err := ParseCnf("data/simple-unsat.cnf")
	if err != nil {
		panic(err)
	}

	cIntSentence := make([]C.int, len(formulaData.sentence), len(formulaData.sentence))
	for k, v := range formulaData.sentence {
		cIntSentence[k] = C.int(v)
	}

	// print result
	result := C.dpll3Sat(&cIntSentence[0], C.int(len(cIntSentence)))
	fmt.Println("Res: ", result)
}
