import numpy as np
import argparse
import os, sys
sys.path.append('../indices/')
import pdb
from utils import *
from train import trainIndex
from config import config
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--index", default='sift_epc40_K10_B1024_R1', type=str)
parser.add_argument("--gpu", default='0', type=str)
parser.add_argument("--memmap", default=False, type=bool)
parser.add_argument("--mode", default=1, type=int)
parser.add_argument("--hdim", default=256, type=int)
parser.add_argument("--kn", default=10, type=int)
args = parser.parse_args()

datasetName = args.index.split('_')[0]  
n_epochs = int(args.index.split('_')[1].split('epc')[1]) 
K = int(args.index.split('_')[2].split('K')[1])  
B = int(args.index.split('_')[3].split('B')[1])
R = int(args.index.split('_')[4].split('R')[1])
feat_dim =  config.DATASET[datasetName]['d']
N = config.DATASET[datasetName]['N'] 
metric = config.DATASET[datasetName]['metric'] 
dtype = config.DATASET[datasetName]['dt'] 

# if not os.path.exists("../logs/{}".format(datasetName)):  
#     os.makedirs("../logs/{}".format(datasetName))

mode = args.mode
lookups_loc  = "indices/{}/".format(datasetName+"blissMode"+str(mode))
train_data_loc = "data/{}/".format(datasetName)
model_save_loc = lookups_loc
batch_size = 5000
hidden_dim = args.hdim #512 initially, should be an argumment, observation lower numbers like 4-16 are best
# logfile = "../logs/{}/".format(datasetName)
gpu = 0
gpu_usage =0.9
load_epoch = 0

t1 = time.time()
for r in range(R):
    trainIndex(lookups_loc, train_data_loc, datasetName, model_save_loc, batch_size, B, feat_dim, hidden_dim,
                    r, gpu, gpu_usage, load_epoch, K, n_epochs, mode, args.kn)

print ("Training finished in: ",time.time()-t1, " sec")

