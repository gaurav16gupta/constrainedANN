# constraintANN
```
make index
./index
```
This will run FilterIndex.cpp

Install 
1) Faiss
2) OpenBLAS

Provide path at INC and LFLAGS in Makefile

Data:

- Download from https://rice-my.sharepoint.com/:f:/g/personal/gg29_rice_edu/EhiOVoan9FhJl9eB3BMqpukBcIrSQAXvUY31UrTHDUVGWg?e=jKtKPR

Updates:
- No bug in jointIndex (proposed method)
- added code for SearchFilter and FilterSearch

Todo: 
- Benchmark RAMBO vs inverted index
- Joint attributes: Combine attributes, just like we do in byte-pair encoding vocab generation. Do for inverted index.
- Benchmark RAMBO vs inverted index on Joint attributes
- Optimize FilterIndex
- Add config file for params and different datasets
