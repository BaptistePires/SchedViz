from curses import update_lines_cols
import matplotlib.pyplot as plt
import numpy as np
from sys import argv
from os import path
import seaborn as sns

if len(argv) < 2:
    print("Usage %s <fin ...>" % argv[0])
    exit()

if not path.exists(argv[1]):
    print("File %s dosn't exist" % argv[1])
    exit()

titles_font = {
    'family': 'normal',
    'weight': 'bolf',
    'size':12
}
max_time = 10e4

raw_times = []
for i in range(1, len(argv)):
    with open(argv[i], "r") as f:
        tmp = [int(x) for x in f.readline().split() if x != '' and int(x) != 0]
        if len(raw_times) == 0 :
            raw_times = tmp
        else:
            raw_times = raw_times + tmp
        
    if len(raw_times) == 0:
        print("No value found in file %s" % argv[1])
        exit()

max_val = len(raw_times)
raw_times = np.array([x for x in raw_times if x < max_time])
print("Dataset info\n------------------------")
print("(ns)")
print("size %s\nmin %s\nmax %s\nmedian %s\nstd %s" % (len(raw_times), raw_times.min(), raw_times.max(), np.median(raw_times), raw_times.std()))
print("------------------------")

fig, ax = plt.subplots()

# raw_times = [x for x in raw_times]

ax_mapped = {
    1 : 'upleft',
    0.75: 'lowleft',
    0.50: 'upright',
    0.25: 'lowright'
}

raw_times.sort()

tmp = np.array(raw_times)

scale = tmp.max()
ms_cumul = [0] * (scale + 1)
y_axis = []

for t in tmp:
    ms_cumul[t] +=1

first_value_id = 0
for i,v in enumerate(ms_cumul):
    if v != 0:
        first_value_id = i
        break
    

for i,x in enumerate(ms_cumul):
    if i == 0:continue
    ms_cumul[i] += ms_cumul[i - 1]

for i, v in enumerate(ms_cumul):
    ms_cumul[i] = (ms_cumul[i] / max_val)*100

ms_cumul = ms_cumul[first_value_id:]

x_ax = [x +first_value_id for x in range(len(ms_cumul))]

ax.set_xlabel("")
ax.set_ylabel("$t$ [ns]")
ax.set_title("Blocked time cumulative count ($t < %s $ ns) " % int(max_time))
ax.grid(True)

sns.lineplot(ax=ax, x=x_ax, y=ms_cumul)


# sns.boxplot(ax=ax,data=raw_times, width=0.3)


# sns.violinplot(ax=ax, data=tmp)
fig.suptitle("Répartion des temps de blocage des processus (t < %s)" % max_time)
fig.savefig("violin.png")