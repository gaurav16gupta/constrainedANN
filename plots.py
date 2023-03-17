import numpy as np
import os
import time
import matplotlib.pyplot as plt
from matplotlib.pyplot import cm
import sys

ncon = 216
results = np.loadtxt('/scratch/gg29/constrainedANN/Results_sift_3_'+str(ncon)+'.txt', delimiter=',')
idxsize = [128, 256, 512, 1024, 2048]
color=cm.rainbow(np.linspace(0,1,len(idxsize)))

plt.figure()
for i,idx in enumerate(idxsize):
    x = results[:,2][results[:,0]==idx]
    y = results[:,3][results[:,0]==idx]
    plt.plot(x, y ,color = color[i],marker='o', label=str(idx))
    
plt.xlabel('QPS')
plt.ylabel('Recall100@100')    
plt.title('SIFT, num_attributes=3 , total_constraints='+str(ncon)+' ')
# plt.xscale('log')
# plt.grid()
plt.minorticks_on()
plt.grid(which='major', color='black', linestyle='-')
# plt.grid(b=True, which='minor', color='black', linestyle=':')
# plt.ylim(0.5,0.83)
# plt.xlim(0,20)
plt.legend(fontsize=7.5)
# plt.show()
plt.savefig('sift_3_'+str(ncon)+'.png', dpi = 500)


ncon = 216
results = np.loadtxt('/scratch/gg29/ResultsNHQ_sift_3_'+str(ncon)+'.txt' , delimiter=',')
idxsize = [128, 256, 512, 1024, 2048]
color=cm.rainbow(np.linspace(0,1,len(idxsize)))

plt.figure()
x = 10000/results[:,0]
y = results[:,1]
plt.plot(x, y ,color = 'blue',marker='o', label="NHQ")


plt.xlabel('QPS')
plt.ylabel('Recall100@100')    
plt.title('SIFT, num_attributes=3 , total_constraints='+str(ncon)+', NHQ ')
# plt.xscale('log')
# plt.grid()
plt.minorticks_on()
plt.grid(which='major', color='black', linestyle='-')
# plt.grid(b=True, which='minor', color='black', linestyle=':')
# plt.ylim(0.5,0.83)
# plt.xlim(0,20)
plt.legend(fontsize=7.5)
# plt.show()
plt.savefig('NHQsift_3_'+str(ncon)+'.png', dpi = 500)

