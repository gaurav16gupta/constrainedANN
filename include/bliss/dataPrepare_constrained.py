import numpy as np
import h5py
from utils import *
from config import config
from binReader import *
import tensorflow as tf

# add SIFT and other data as well
def getInvertedIndex(sentencesTokenised):
    Invbins = {}
    for i,sentence in enumerate(sentencesTokenised):
        # for word in sentence:
        try:
            Invbins[sentence].append(i)
        except(KeyError):
            Invbins[sentence] = []
            Invbins[sentence].append(i)
    return Invbins

def intersection(lst1, lst2):
    # Use of hybrid method
    temp = set(lst2)
    lst3 = [value for value in lst1 if value in temp]
    return lst3

def makeTraindata(dataname, K):
    x_train, trainConst = loaddata(dataname)
    os.environ['CUDA_VISIBLE_DEVICES'] = '0'
    begin_time = time.time()
    batch_size = 1000
    output = np.zeros([x_train.shape[0], K], dtype=np.int32) # for upto 2B

    #L2 metric
    W = x_train.T
    W_norm = np.square(np.linalg.norm(W,axis=0))
    W = tf.constant(W)
    for i in tqdm(range(x_train.shape[0]//batch_size)):
        start_idx = i*batch_size
        end_idx = start_idx+batch_size
        x_batch = x_train[start_idx:end_idx]
        # sim = 2*x_batch@W - W_norm
        sim = 2*tf.matmul(x_batch,W)- W_norm
        # top_idxs = np.argpartition(sim, -K)[:,-K:]
        top_idxs = tf.nn.top_k(sim, k=K, sorted=False)[1]
        output[start_idx:end_idx] = top_idxs

    N,d = output.shape
    output = np.column_stack((d*np.ones(N, dtype='int32'),output)).flatten()
    output.tofile(dataname+"/BLISS_train_groundtruth.ivecs")
    print(time.time()-begin_time)

def makeTraindata_wAttr(dataname, K):
    train, trainConst = loaddata(dataname)
    N = train.shape[0]
    # if N>10**6:
    #     np.random.seed(0)
    #     pick = np.random.choice(N, 10**6, replace=False) # fix seed
    #     train = train[pick,:]
    #     trainConst = trainConst[pick,:]
  
    Invbins = getInvertedIndex(trainConst)
    norms = 0.5*np.linalg.norm(train,axis=1) # if L2 distance

    # do filter then search
    # get neighbors
    nt = train.shape[0]
    largest_indices = []
    print ("starting")
    print (time.time())
    #can do batch wise with tensorflow
    for i in tqdm(range(0, nt)):
        # try:
        # a = [Invbins[key] for key in trainConst[i]]
        candidates = Invbins[trainConst[i]]
        # candidates = a[0]
        # for k in range(1,len(a)):
        #     candidates = intersection(candidates,a[k])
        candidates = np.array(candidates)
        dist = (-train[candidates, :]@train[i] +norms[candidates])
        if len(dist)>K:
            temp = np.argpartition(dist, K)[:K]
            temp = temp[np.argsort(dist[temp])]
        else:
            temp = np.argsort(dist)
            temp = np.append(temp,-np.ones(K-len(temp),dtype=np.dtype(temp[0])))
        assert len(temp)==K
        largest_indices.append(candidates[temp]) # to verify this
        
    print (len(largest_indices), nt)
    a = np.array(largest_indices).astype('int32')
    N,d = a.shape
    a = np.column_stack((d*np.ones(N, dtype='int32'),a)).flatten()
    a.tofile(dataname+"/BLISS_train_3_groundtruth.ivecs")

# get data and properties
def loaddata(dataname):
    trainPath = dataname+"/base.fvecs"
    trainConstPath = dataname+"/label_base_3.txt"
    train = fvecs_read(trainPath, c_contiguous=True)
    trainConst = np.genfromtxt(trainConstPath, skip_header=1, delimiter=",",dtype='str')    
    return train, trainConst

# queries and query properties
def loadQueries(dataname):
    testPath = dataname+"/query.fvecs"
    testConstPath = dataname+"/label_query_3.txt"
    test = fvecs_read(testPath, c_contiguous=True)  
    testConst = np.genfromtxt(testConstPath, skip_header=1, delimiter=",",dtype='str')
    return test, testConst

def appendVecAtt(train, trainConst):
    # convert to binary vector
    trainConst = np.array([sublist.split(" ") for sublist in trainConst])
    vocab = np.unique(trainConst)
    vcaobtoNum = {}
    for i,v in enumerate(vocab):
        vcaobtoNum[v]= i
    trainConst = np.array([[vcaobtoNum[v] for v in sublist] for sublist in trainConst])
    constArray = np.empty((trainConst.shape[0], len(vocab)))
    for i in range(constArray.shape[0]):
        constArray[i][trainConst[i]] =1
    return np.column_stack((train, constArray))

if __name__ == "__main__":
    dataname = '../../../../data/{}/'.format("glove-100")
    # makeTraindata(dataname, 100)
    makeTraindata_wAttr(dataname, 100)


