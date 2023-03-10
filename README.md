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
- Benchmark RAMBO vs inverted index
- Joint attributes: Combine attributes, just like we do in byte-pair encoding vocab generation. Do for inverted index.
- Benchmark RAMBO vs inverted index on Joint attributes
- Optimize FilterIndex
- Add config file for params and different datasets
