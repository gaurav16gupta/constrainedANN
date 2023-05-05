import numpy as np
import os
import time
import matplotlib.pyplot as plt
from matplotlib.pyplot import cm
import sys

natt = 3
ncon = 27
results = np.loadtxt("results/Results_sift_3_27_1024Mode1ep50.txt", delimiter=',')
color=cm.rainbow(np.linspace(0,1,5))
x = results[:,2]
y = results[:,3]
plt.plot(x, y ,color = 'red',marker='o', label="BLISS 1")


results = np.loadtxt("results/Results_sift_3_27_1024Mode2ep50.txt", delimiter=',')
color=cm.rainbow(np.linspace(0,1,5))
x = results[:,2]
y = results[:,3]
plt.plot(x, y ,color = 'blue',marker='o', label="BLISS 2")


results = np.loadtxt("results/Results_sift_3_27_1024Mode3ep50.txt", delimiter=',')
color=cm.rainbow(np.linspace(0,1,5))
x = results[:,2]
y = results[:,3]
plt.plot(x, y ,color = 'green',marker='o', label="BLISS 3")


results = np.loadtxt("results/Results_sift_3_27_1024v2.txt", delimiter=',')
color=cm.rainbow(np.linspace(0,1,5))
x = results[:,2]
y = results[:,3]
plt.plot(x, y ,color = 'orange',marker='+', label="Kmeans (Prev)")


results = np.loadtxt("../../ResultsNHQ_sift_3_27.txt", delimiter=' ')
color=cm.rainbow(np.linspace(0,1,5))
x = 10000/results[:,0]
y = results[:,1]
plt.plot(x, y ,color = 'black',marker='*', label="NHQ")


plt.xlabel('QPS')
plt.ylabel('Recall100@100')    
plt.title('SIFT, num_attributes=3 , total_constraints='+str(ncon)+' ')
# plt.xscale('log')
# plt.grid()
plt.minorticks_on()
plt.grid(which='major', color='black', linestyle='--')
# plt.grid(b=True, which='minor', color='black', linestyle=':')
# plt.ylim(0.5,0.83)
# plt.xlim(0,20)
plt.legend(fontsize=7.5)
# plt.show()
plt.savefig('sift_3_'+str(ncon)+'_1024BLISSep50Tuned.png', dpi = 500)




# results = np.loadtxt('/scratch/gg29/ResultsNHQ_sift_'+str(natt)+'_'+str(ncon)+'.txt' , delimiter=' ')

# # plt.figure()
# x = 10000/results[:,0]
# y = results[:,1]
# plt.plot(x, y ,color = 'blue',marker='o', label="NHQ")

# plt.xlabel('QPS')
# plt.ylabel('Recall100@100')    
# plt.title('SIFT, num_attributes=3 , total_constraints='+str(ncon)+', NHQ ')
# # plt.xscale('log')
# # plt.grid()
# plt.minorticks_on()
# plt.grid(which='major', color='black', linestyle='-')
# # plt.grid(b=True, which='minor', color='black', linestyle=':')
# # plt.ylim(0.5,0.83)
# # plt.xlim(0,20)
# plt.legend(fontsize=7.5)
# # plt.show()
# plt.savefig('NHQsift_'+str(natt)+'_'+str(ncon)+'.png', dpi = 500)
# plt.savefig('Comparision sift_'+str(natt)+'_'+str(ncon)+'corrected.png', dpi = 500)

