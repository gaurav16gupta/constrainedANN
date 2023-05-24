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
trainConstPath = dataname+"/label_base_3.txt"
testConstPath = dataname+"/label_query_3.txt"
print (trainPath, testPath, trainConstPath, testConstPath)

train = fvecs_read(trainPath, c_contiguous=True)
test = fvecs_read(testPath, c_contiguous=True)
trainConst = np.genfromtxt(trainConstPath, skip_header=1, delimiter=" ",dtype='str')
testConst = np.genfromtxt(testConstPath, skip_header=1, delimiter=" ",dtype='str')

# get inv index on constraint
Invbins = getInvertedIndex(trainConst)
norms = 0.5*(np.linalg.norm(train,axis=1))**2

# do filter then search
# get neighbors
nt = test.shape[0]
largest_indices = []
print ("starting")
print (time.time())
for i in tqdm(range(0, nt)):
    # try:
    a = [Invbins[key] for key in testConst[i]]
    candidates = a[0]
    for k in range(1,len(a)):
        candidates = intersection(candidates,a[k])
    candidates = np.array(candidates)
    dist = norms[candidates]  -train[candidates, :]@test[i] 
    # dist2 = np.linalg.norm((train[candidates, :]-test[i]),axis=1)**2
    # pdb.set_trace()
    if len(dist)>100:
        temp = np.argpartition(dist, 100)[:100]
        temp = temp[np.argsort(dist[temp])]
    else:
        temp = np.argsort(dist)
        temp = np.append(temp,-np.ones(100-len(temp),dtype=np.dtype(temp[0])))
    assert len(temp)==100
    largest_indices.append(candidates[temp]) # to verify this
    
print (len(largest_indices), nt)
a = np.array(largest_indices).astype('int32')
N,d = a.shape
a = np.column_stack((d*np.ones(N, dtype='int32'),a)).flatten()
a.tofile(dataname+"/label_3_hard_groundtruth.ivecs")
