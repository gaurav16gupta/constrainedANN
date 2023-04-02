# constraintANN

Install 
- Faiss
   ```
   cd ..
   git clone https://github.com/facebookresearch/faiss.git
   cd faiss
   cmake -B build .
   make -C build -j faiss
   make -C build install
   ```
   This will generate the libfaiss.a

- OpenBLAS
  ```
  git clone https://github.com/xianyi/OpenBLAS.git
  make
  ```
  This will generate the libopenblas.a
  

Provide path at INC and LFLAGS in Makefile

- INC=-I ./../faiss -I include/
- LFLAGS=<your path>/libfaiss.a <your path>/libopenblas.a -lpthread -lm -ldl -lgfortran -fopenmp


Get Data:
- Download from https://rice-my.sharepoint.com/:f:/g/personal/gg29_rice_edu/EhiOVoan9FhJl9eB3BMqpukBcIrSQAXvUY31UrTHDUVGWg?e=jKtKPR
- Run data/sift/generateRandomTokens.py then data/sift/getGT-filterSearch.py, each time changing the sentence length of attributes to generate the synthetic attributes and groundtruth.
 
For your own data
- base vectors and query vectors are stored in .fvecs format
- base and query attributes are stored in .txt files. Format is
    <num points> <num attributes>
    2 outdoor night
    1 indoor daytime
    3 outdoor night
    2 indoor daytime
    3 outdoor daytime

Where "2 outdoor night" is an example of space seperated 3 attributes.

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
- Find issue in large attribute sentence length
- Joint attributes: Combine attributes, just like we do in byte-pair encoding vocab generation. Do for inverted index.
- Benchmark RAMBO vs inverted index on Joint attributes
