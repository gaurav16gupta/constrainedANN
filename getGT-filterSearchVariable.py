import sys
import numpy as np
sys.path.insert(0, '../')
from binReader import *
import time
import pdb
from tqdm import tqdm
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--data", default='nthing', type=str)
args = parser.parse_args()

def getInvertedIndex(sentencesTokenised):
    Invbins = {}
    for i,sentence in enumerate(sentencesTokenised):
        for word in sentence:
            try:
                Invbins[word].append(i)
            except(KeyError):
                Invbins[word] = []
                Invbins[word].append(i)
    return Invbins

def intersection(lst1, lst2):
    # Use of hybrid method
    temp = set(lst2)
    lst3 = [value for value in lst1 if value in temp]
    return lst3

# attr = 3
dataname = args.data
trainPath = dataname+"/base.fvecs"
testPath = dataname+"/query.fvecs"
trainConstPath = dataname+"/label_base_100.txt"
testConstPath = dataname+"/label_query_100.txt"
print (trainPath, testPath, trainConstPath, testConstPath)

train = fvecs_read(trainPath, c_contiguous=True)
test = fvecs_read(testPath, c_contiguous=True)
trainConst = np.genfromtxt(trainConstPath, skip_header=1, delimiter=" ",dtype='str')
testConst = np.genfromtxt(testConstPath, skip_header=1, delimiter=" ",dtype='str')

# get inv index on constraint
Invbins = getInvertedIndex(trainConst)
norms = 0.5*(np.linalg.norm(train,axis=1))**2

# for Probab in [0.1, 0.3, 0.5, 0.7, 0.9]:
for Probab in [1-0.03]:
    select = np.random.uniform(low=0.0, high=1.0, size=(testConst.shape[0],testConst.shape[1]))
    select = select<Probab
    for i in range(testConst.shape[0]):
        testConst[i][select[i]] ="X"
    # save testConst
    # save a header as well
    with open(dataname + "/label_query_100_{}.txt".format(Probab), "w") as file:
        file.write("{} {}\n".format(testConst.shape[0], testConst.shape[1]))
        np.savetxt(file, testConst, delimiter=" ", fmt="%s")
    # np.savetxt(dataname+"/label_query_100_{}.txt".format(Probab), testConst, delimiter=" ", fmt="%s")

    nt = test.shape[0]
    largest_indices = []
    print ("starting")
    print (time.time())
    for i in tqdm(range(0, nt)):
        # try:
        a = [Invbins[key] for key in testConst[i] if key != "X"]
        if len(a)==0: # regular NNS
            dist = norms -train@test[i] 
            temp = np.argpartition(dist, 100)[:100]
            temp = temp[np.argsort(dist[temp])]
            assert len(temp)==100
            largest_indices.append(temp) 
        else:           # filtered NNS
            candidates = a[0]
            for k in range(1,len(a)):
                candidates = intersection(candidates,a[k])
            candidates = np.array(candidates)
            dist = norms[candidates]  -train[candidates, :]@test[i] 
            if len(dist)>100:
                temp = np.argpartition(dist, 100)[:100]
                temp = temp[np.argsort(dist[temp])]
            else:
                temp = np.argsort(dist)
                temp = np.append(temp,-np.ones(100-len(temp),dtype=np.dtype(temp[0])))
            assert len(temp)==100
            largest_indices.append(candidates[temp])     
    print (len(largest_indices), nt)
    a = np.array(largest_indices).astype('int32')
    N,d = a.shape
    a = np.column_stack((d*np.ones(N, dtype='int32'),a)).flatten()
    a.tofile(dataname+"/label_100_{}_hard_groundtruth.ivecs".format(Probab))
