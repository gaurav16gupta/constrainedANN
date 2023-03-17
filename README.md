# constraintANN

Install 
1) Faiss
2) OpenBLAS

Provide path at INC and LFLAGS in Makefile

Get Data:

- Download from https://rice-my.sharepoint.com/:f:/g/personal/gg29_rice_edu/EhiOVoan9FhJl9eB3BMqpukBcIrSQAXvUY31UrTHDUVGWg?e=jKtKPR

Set parameters number of queries (nq) and number of distance computations (buffer_size) in FilterIndex.cpp main

Run
```
make index
./index 1024
```
./index num_clusters
```
make query
./query 1024 1000 200
```
./query num_clusters number_of_queries buffer_size

Updates:
- No bug in FilterIndex (proposed method)
- added code for SearchFilter and FilterSearch

Todo: 
- Split index and query
- Add arguments
- Optimize FilterIndex
- Benchmark RAMBO vs inverted index
- Joint attributes: Combine attributes, just like we do in byte-pair encoding vocab generation. Do for inverted index.
- Benchmark RAMBO vs inverted index on Joint attributes
