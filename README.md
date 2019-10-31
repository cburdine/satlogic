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