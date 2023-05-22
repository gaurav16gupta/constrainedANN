import numpy as np
import pdb
def fvecs_read(filename, c_contiguous=True):
    fv = np.fromfile(filename, dtype=np.float32)
    if fv.size == 0:
        return np.zeros((0, 0))
    dim = fv.view(np.int32)[0]
    assert dim > 0
    fv = fv.reshape(-1, 1 + dim)
    if not all(fv.view(np.int32)[:, 0] == dim):
        raise IOError("Non-uniform vector sizes in " + filename)
    fv = fv[:, 1:]
    if c_contiguous:
        fv = fv.copy()
    return fv

def ivecs_read(fname):
    a = np.fromfile(fname, dtype='int32')
    d = a[0] 
    return a.reshape(-1, d + 1)[:, 1:].copy()

def ibin_read(fname):
    a = np.fromfile(fname, dtype='int32')
    N = a[0]
    d = a[1]
    a = a[2:]
    return a.reshape(-1, d).copy()



