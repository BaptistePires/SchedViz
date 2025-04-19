import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from sys import argv
from os import listdir
import pandas as pd

TYPE_KERNEL = 1
TYPE_USER = 2
TYPE_OTHER = 3

if len(argv) != 2:
    print("whereis input dir")
    exit()

root_dir = argv[1]

colors = ["pink", "blue", "orange"]

t_user = []
t_kernel = []
t_other = []

fig, ax = plt.subplots()

path = root_dir + "/tasks/"
for f in listdir(path + "/user"):
    with open(path + "/user/" + f, "r") as f:
        lines = f.readlines()

        times = [int(x) for x in lines[1].replace('\n', '').split(' ') if x != '' and x != ' ']
        t_user.append((times, TYPE_USER))
        
for f in listdir(path + "/kernel"):
    with open(path + "/kernel/" + f, "r") as f:
        lines = f.readlines()

        times = [int(x) for x in lines[1].replace('\n', '').split(' ') if x != '' and x != ' ']
        t_kernel.append((times, TYPE_KERNEL))

for f in listdir(path + "/other"):
    with open(path + "/other/" + f, "r") as f:
        lines = f.readlines()

        times = [int(x) for x in lines[1].replace('\n', '').split(' ') if x != '' and x != ' ']
        t_other.append((times, TYPE_OTHER))

times = t_user + t_kernel + t_other
times.sort(key=lambda x:np.median(x[0]))

x_axis = [i for i in range(len(times))]
y_axis = [x[0] for x in times]

data = pd.DataFrame([x[0] for x in times], columns=["times"])

data.boxplot(use_index=True)
plt.show()
# print(times[0])
# sns.boxplot(ax=ax, y=y_axis)
# # for i in range(len(times)):
# # ax.boxplot(x=[x[0] for x in times])

# ax.set(yscale="log")

# plt.show()
    
