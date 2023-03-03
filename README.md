# constraintANN
```
make index
./index
```
This will run FilterIndex.cpp

Currently it has a bug and the outputs are not consistent.

Install 
1) Faiss
2) OpenBLAS
Provide path at INC and LFLAGS in Makefile

Todo: 
1) Remove FilterIndex bug
2) Benchmark RAMBO vs inverted index
3) Joint attributes: Combine attributes, just like we do in byte-pair encoding vocab generation. Do for inverted index.
4) Benchmark RAMBO vs inverted index on Joint attributes
5) Optimize FilterIndex
6) Add config file for params and different datasets
