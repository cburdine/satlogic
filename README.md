# gosat
A 3-SAT solver written in C with a Go interface

## How to install

This project requires the Golang build tools to be installed.
The best way to fetch this repository is to run:
```
go get github.com/cburdine/gosat
```

Then, to build it in /bin run:
```
make buildall
```

(For Windows, it may be best to compile the Go directly using `go build`)


To run the built binary you also need .cnf files in the /data directory. The archive I am currently using can
be downloaded directly [here](https://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/RND3SAT/uf50-218.tar.gz).

For more SAT problem CNF test cases, I would highly recommend the following sites:

* [SATLIB Benchmark Problems](https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html)
* [Tough SAT Project](https://toughsat.appspot.com/)

## How to run:

Once the go binary builds successfully in /bin, run the executable with any of the .cnf files in the ./data directory:
```
./bin/gosat -solver=dpll ./data/*.cnf
```

The general usage of the built binary is as follows:
```
./bin/gosat [-solver=<solver>] [FILES ...]
```
Currently, the only solver supported is: `dpll`

---

## Solver method

The solver currently uses the Davis-Putnam-Logemann–Loveland (DPLL) algorithm with minimal pre-processing for solving SAT sentences. The DPLL algorithm either produces a satisfying assignment for a sentence or a branching refutation proof, depending on whether or not the sentence is unsatisfiable. A shallow but concise overview of this algorithm an be found on the [DPLL Algorithm Wikipedia page](https://en.wikipedia.org/wiki/DPLL_algorithm).

The choice of branching variable is done through the Variable State Independent Decaying Sum (VSIDS) heuristic, which resolves variables that produce contradictions (empty clauses) on adjacent branches, thus optimizing the order in which branches of variable assignments are explored. More beinfo on this an other heuristics can be found on the corresponding [Wikipedia page](https://en.wikipedia.org/wiki/Boolean_satisfiability_algorithm_heuristics#cite_note-princeton-2)

The recursive pseudo-code for the algorithm is briefly outlined below:
```
# Input: A SAT sentence Φ

# Output: A Boolean value, indicating the
          satisfiability of Φ

DPLL( Φ ):

    # Base case for trivially satisfiable Φ
    if Φ is empty:
        return TRUE
    
    while some clause C in Φ has 1 or fewer literals:

        #  If C is an empty clause, then a contradiction
        #  was reached in C, thus we must backtrack.
        if C is empty:
            return FALSE

        # Propagate any unit clauses
        else if C contains a single literal P:
            Φ <- PROPAGATE_UNIT( Φ, VARIABLE(P), SIGN(P) )

    # pick a variable and assign it a T/F value
    # (this is done through the VSIDS heuristic)
    V <- CHOOSE_VSIDS_BRANCHING_LITERAL()

    Φ_A <- PROPAGATE_UNIT( Φ, VARIABLE(V), SIGN(V) )
    Φ_B <- PROPAGATE_UNIT( Φ, VARIABLE(V), -SIGN(V) )

    return ( DPLL(Φ_A) OR DPLL(Φ_B) )

```

## Future Directions

In future releases of this project I hope to provide another solver that implements the Conflict-Driven Clause Learning method.