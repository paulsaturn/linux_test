# -*- coding: utf-8 -*-
"""
@author: paul2
"""
import numpy as np
import matplotlib.pyplot as plt

fig, ax = plt.subplots()

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.00*height, '%d'%int(height),
                ha='center', va='bottom')

# N = 5
#ind = np.arange(N)
N = 35
ind = np.arange(N)
# width = 0.35
width = 0.9
#menMeans = (20, 35, 30, 35, 27)
#menStd = (2, 3, 4, 1, 2)
 
#womenMeans = (25, 32, 34, 20, 25)
#womenStd = (3, 5, 2, 3, 3)

#ax.bar(ind, menMeans, width, color='r', yerr=menStd)
#ax.bar(ind+width, womenMeans, width, color='y', yerr=womenStd)
means = np.random.uniform(20, 100,size=N)
rects = ax.bar(ind, means, width, color='r')

ax.set_xticks(ind+width/2)
lable_ind = np.arange(1,N+1,1)
ax.set_xticklabels(lable_ind)
autolabel(rects) 
 
plt.title('test')
plt.show()
