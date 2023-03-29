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
./index <dataname> <num_clusters> <attribute sentence size>
./index sift 1024 3
```
```
make query
./query <dataname> <num_clusters> <attribute sentence size> <buffer size>

./query sift 1024 3 200
```


Todo: 
- Fix AVX
- Optimize FilterIndexHamming
- Parameter optimization. Number of clusters, Tree height, number of splits, balancing etc.
- Fins issue in large attribute sentence length
- Joint attributes: Combine attributes, just like we do in byte-pair encoding vocab generation. Do for inverted index.
- Benchmark RAMBO vs inverted index on Joint attributes
