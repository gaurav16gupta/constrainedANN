# constraintANN

Install (only for Faiss Kmeans clustering)
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
- To genearate synthetic data: Run data/sift/generateRandomTokens.py then data/sift/getGT-filterSearch.py, each time changing the sentence length (default =3) of attributes to generate the synthetic attributes and groundtruth.
 
For your own data
- base vectors and query vectors are stored in .fvecs format
- base and query attributes are stored in .txt files. 
- Example -
```
<num points> <num attributes>
2 outdoor night
1 indoor daytime
3 outdoor night
2 indoor daytime
3 outdoor daytime
```
 
Where "2 outdoor night" is an example of space seperated 3 attributes.


Make sure to have these files in the data folder
```
data/sift/base.fvecs 
data/sift/label_base_3.txt
data/sift/query.fvecs 
data/sift/label_query_3.txt 
indices/sift1024blissMode1 
data/sift/label_3_hard_groundtruth.ivecs
```


If using bliss run
```
python3 include/bliss/dataPrepare_constrained.py --data="data/sift"
python3 include/bliss/construct.py --index='sift_epc40_K10_B1024_R1' --hdim=256 --mode=1 --kn=10
make index
./index data/sift/base.fvecs data/sift/label_base_3.txt indices/sift1024blissMode1 1024 bliss 1
make query
./query data/sift/base.fvecs data/sift/label_base_3.txt data/sift/query.fvecs data/sift/label_query_3.txt indices/sift1024blissMode1 data/sift/label_3_hard_groundtruth.ivecs 1024 bliss 1 500
```

If using faiss kmeans run
```
make index
./index data/sift/base.fvecs data/sift/label_base_3.txt indices/sift1024blissMode1 1024 kmeans 1
make kmeans
./query data/sift/base.fvecs data/sift/label_base_3.txt data/sift/query.fvecs data/sift/label_query_3.txt indices/sift1024blissMode1 data/sift/label_3_hard_groundtruth.ivecs 1024 kmeans 1 500
```

Todo: 
- Variable number of attributes
