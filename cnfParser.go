package main

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"strings"
)

type cnfData struct {
	format     string
	numClauses int
	maxNumVars int
	sentence   []int
}

func ParseCnf(path string) (*cnfData, error) {

	const ignoreChars = "%c0#"

	f, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	data := cnfData{"?", 0, 0, make([]int, 0)}
	foundClauses := 0
	foundMaxVar := 0
	parsedHeader := false

	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())

		// ignore comments:
		if len(line) > 0 && !strings.Contains(ignoreChars, fmt.Sprintf("%c", line[0])) {
			if line[0] == 'p' {

				if parsedHeader {
					return nil, errors.New(fmt.Sprintf("%s: found two instances of header beginning wiht 'p'", path))
				}

				if data.maxNumVars > 0 || data.numClauses > 0 {
					return nil, errors.New(fmt.Sprintf("%s: unrecognized cnf configuration format", path))
				}
				fmt.Sscanf(line, "p %s %d  %d", &data.format, &data.maxNumVars, &data.numClauses)
				parsedHeader = true

			} else {
				varLine := []int{0, 0, 0}
				terminal := -1
				fmt.Sscanf(line, "%d %d %d %d", &varLine[0], &varLine[1], &varLine[2], &terminal)
				if terminal != 0 {
					return nil, errors.New(fmt.Sprintf("%s: invalid 3-cnf file line: %s", path, line))
				}
				for _, v := range varLine {
					foundMaxVar = absMax(v, foundMaxVar)
				}
				data.sentence = append(data.sentence, varLine...)
				foundClauses++
			}
		}
	}

	// check file header aligns with data:
	if parsedHeader {
		if data.numClauses != foundClauses {
			return nil, errors.New(fmt.Sprintf("%s: number of clauses in header vs file mismatched (%d vs %d)", path, data.numClauses, foundClauses))
		}
		if data.maxNumVars != foundMaxVar {
			return nil, errors.New(fmt.Sprintf("%s: number of vars in header vs file mismatched (%d vs %d)", path, data.maxNumVars, foundMaxVar))
		}
	} else {
		// else set header data to what was read:
		data.format = "cnf"
		data.numClauses = foundClauses
		data.maxNumVars = foundMaxVar
	}

	return &data, nil

}

func absMax(x int, y int) int {
	if x < 0 {
		x *= -1
	}
	if y < 0 {
		y *= -1
	}

	if x < y {
		return y
	} else {
		return x
	}
}
