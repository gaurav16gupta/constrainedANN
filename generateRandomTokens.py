import numpy as np
import random
import pdb

def createLargeAttributes(Na, Nb, Nq, datasetname):
    base_file = open(datasetname+"/label_base_{}.txt".format(Na), "w")
    query_file = open(datasetname+"/label_query_{}.txt".format(Na), "w")
    rn = 0.2 +0.8*np.random.random((Nb,Na))
    qidx = random.sample(range(Nb), Nq)

    factor = np.floor((np.random.random(Na)*2)**3) +2
    print (factor)
    rn = np.log(1/rn)
    for a in range(Na):
        rn[:,a] = np.floor(factor[a]*rn[:,a])
        print (np.unique(rn[:,a]))
    rn = rn.astype(np.int32)
    base_file.write("{} {}\n".format(Nb, Na))
    query_file.write("{} {}\n".format(Nq, Na))
    for i in range(Nb):
        base_file.write("{}_{}".format(0,rn[i,0]))
        for a in range(1,Na):
            constrint = "{}_{}".format(a,rn[i,a])
            base_file.write(" " + constrint)
        base_file.write("\n")
    for i in qidx:
        query_file.write("{}_{}".format(0,rn[i,0]))
        for a in range(1, Na):
            constrint = "{}_{}".format(a,rn[i,a])
            query_file.write(" " + constrint)
        query_file.write("\n")

# createLargeAttributes(3, 53387, 200, "audio")
# createLargeAttributes(3, 992272, 200, "msong")
createLargeAttributes(10, 1183514, 10000, "glove-100")
createLargeAttributes(100, 1183514, 10000, "glove-100")
createLargeAttributes(10, 1000000, 10000, "gist")
createLargeAttributes(100, 1000000, 10000, "gist")
