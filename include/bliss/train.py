import tensorflow as tf
import argparse
import time
import os
import numpy as np
from binReader import *
from dataPrepare_constrained import *
from utils import *

def trainIndex(lookups_loc, train_data_loc, datasetName, model_save_loc, batch_size, B, vec_dim, hidden_dim,
                    r, gpu, gpu_usage, load_epoch, k2, n_epochs, mode, kn):

    assert (B%2==0)
    B = 2*B
    tf.compat.v1.disable_eager_execution()
    os.environ['CUDA_VISIBLE_DEVICES'] = '0'
    # get train data
    # x_train = np.load(train_data_loc+'train.npy')
    # y_train = np.load(train_data_loc+'groundTruth.npy')
    print ("mode ",mode)
    # mode 1: Simple ANN train
    IntSave = True
    if mode==1:
        x_train, trainConst = loaddata(train_data_loc)
        y_train = ivecs_read(train_data_loc + "/BLISS_train_groundtruth.ivecs")
    # mode 2: Label with constraints
    if mode==2:
        x_train, trainConst = loaddata(train_data_loc)
        y_train = ivecs_read(train_data_loc + "/BLISS_train_3_groundtruth.ivecs")
    # mode 3: input both vector and attributes
    if mode==3:
        x_train, trainConst = loaddata(train_data_loc)
        x_train = appendVecAtt(x_train, trainConst)
        y_train = ivecs_read(train_data_loc + "/BLISS_train_3_groundtruth.ivecs")
    vec_dim = x_train.shape[1]
    N = x_train.shape[0]
    
    y_train = y_train[:,:kn]  

    # randIndx = np.arange(0, N)
    # np.random.shuffle(randIndx)
    # Winit = x_train[randIndx[:B],:]
    ###############
    if not os.path.exists(lookups_loc+'epoch_'+str(load_epoch)+'/'):  
        os.makedirs(lookups_loc+'epoch_'+str(load_epoch)+'/')
    create_universal_lookups(r, B, N, lookups_loc+'epoch_'+str(load_epoch)+'/')
    # randomPoints_lookups(r, B, N, x_train, lookups_loc+'epoch_'+str(load_epoch)+'/')
    
    lookup = tf.Variable(np.load(lookups_loc+'epoch_'+str(load_epoch)+'/bucket_order_'+str(r)+'.npy')[:N])
    ###############
    temp = tf.constant(np.arange(batch_size*kn)//kn)
    x = tf.compat.v1.placeholder(tf.float32, shape=[batch_size, vec_dim])
    _y = tf.compat.v1.placeholder(tf.int64, shape=[batch_size*kn])
    y_idxs = tf.stack([temp, tf.gather(lookup, _y)], axis=-1)
    y_vals = tf.ones_like(y_idxs[:,0], dtype=tf.float32)
    y = tf.compat.v1.SparseTensor(y_idxs, y_vals, [batch_size, B])
    y_ = tf.compat.v1.sparse_tensor_to_dense(y, validate_indices=False)

    ###############
    W1 = tf.Variable(tf.compat.v1.truncated_normal([vec_dim, hidden_dim], stddev=0.05, dtype=tf.float32))
    b1 = tf.Variable(tf.compat.v1.truncated_normal([hidden_dim], stddev=0.05, dtype=tf.float32))
    hidden_layer = tf.nn.relu(tf.matmul(x,W1)+b1)
    W2 = tf.Variable(tf.compat.v1.truncated_normal([hidden_dim, B], stddev=0.05, dtype=tf.float32))
    b2 = tf.Variable(tf.compat.v1.truncated_normal([B], stddev=0.05, dtype=tf.float32))
    logits = tf.matmul(hidden_layer,W2)+b2
    ###############
    top_buckets = tf.nn.top_k(logits, k=k2, sorted=True)[1]
    loss = tf.reduce_mean(tf.nn.sigmoid_cross_entropy_with_logits(logits=logits, labels=y_))
    train_op = tf.compat.v1.train.AdamOptimizer().minimize(loss)

    sess = tf.compat.v1.Session(config = tf.compat.v1.ConfigProto(
                            allow_soft_placement=True,
                            log_device_placement=False,
                            gpu_options=tf.compat.v1.GPUOptions(allow_growth=True, per_process_gpu_memory_fraction=gpu_usage)))

    sess.run(tf.compat.v1.global_variables_initializer())

    n_check=200
    n_steps_per_epoch = N//batch_size
    for curr_epoch in range(load_epoch+1,load_epoch+n_epochs+1):
        count = 0
        for j in range(n_steps_per_epoch):
            start_idx = j*batch_size
            end_idx = start_idx+batch_size
            sess.run(train_op, feed_dict={x:x_train[start_idx:end_idx], _y:y_train[start_idx:end_idx].reshape([-1])})
            
            count += 1
            if count%n_check==0:
                _, train_loss = sess.run([train_op,loss], feed_dict={x:x_train[start_idx:end_idx], _y:y_train[start_idx:end_idx].reshape([-1])})
                # print('train_loss: '+str(train_loss))
                temp = tf.constant(np.arange(batch_size*kn)//kn)
                y_idxs = tf.stack([temp, tf.gather(lookup, y_train[start_idx:end_idx].reshape([-1]) )], axis=-1)
                y_vals = tf.ones_like(y_idxs[:,0], dtype=tf.float32)
                y = tf.compat.v1.SparseTensor(y_idxs, y_vals, [batch_size, B])
                y_ = tf.compat.v1.sparse_tensor_to_dense(y, validate_indices=False)
                yout = sess.run(y_)
                # print (np.sum(yout)/yout.shape[0])
                count+=1

        if curr_epoch%5==0:                
            #####################################
            top_preds = np.zeros([N,k2], dtype=int)
            start_idx = 0
            for i in range(x_train.shape[0]//batch_size):
                top_preds[start_idx:start_idx+batch_size] = sess.run(top_buckets, feed_dict={x:x_train[start_idx:start_idx+batch_size]})
                start_idx += batch_size
            ##################################### 
            counts = np.zeros(B+1, dtype=int)
            bucket_order = np.zeros(N, dtype=int)
            for i in range(N):
                bucket = (top_preds[i, counts[top_preds[i]+1] == np.min(counts[top_preds[i]+1])])[0]
                # bucket = top_preds[i, np.argmin(counts[top_preds[i]+1])] 
                bucket_order[i] = bucket
                counts[bucket+1] += 1

            nothing = sess.run(tf.compat.v1.assign(lookup,bucket_order))
            print ("max bin: ", np.max(counts))
            print ("load Std: ", np.std(counts))
            print ("empty bins: ", np.sum(counts==0))
            counts = np.cumsum(counts)
            class_order = np.argsort(bucket_order)
    params = sess.run([W1,b1,W2,b2])
    np.savez_compressed(model_save_loc+'/r_'+str(r)+'.npz',
    W1=params[0], 
    b1=params[1], 
    W2=params[2], 
    b2=params[3])

    # keep only top B/2
    cnt = counts[1:]-counts[:-1]
    keep  = np.argsort(cnt)[::-1][:int(B/2)]
    params[2] = params[2][:,keep]
    params[3] = params[3][keep]
    A1 = np.vstack((params[0], params[1])).T.flatten()
    A2 = np.vstack((params[2], params[3])).T.flatten()
    np.concatenate([A1, A2]).tofile(model_save_loc+'/model.bin')
    del params
