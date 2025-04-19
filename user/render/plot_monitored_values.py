from sys import argv
from os import listdir
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import uuid

MAX_TIME = 1e6
ONE_MS_NS = 400000

if len(argv) < 2:
    print("Need to provide input dir and threshold an outname")

ONE_MS_NS = int(argv[2])

root_dir = argv[1]

# Parsing benchmark meta data

cmd = ""
t_user = ""
t_kernel = ""
t_other = ""

with open(root_dir + "/meta", "r") as meta_f:
    lines = meta_f.readlines()
    print(lines)
    cmd = lines[0]
    t_user = int(lines[1].split(' ')[1])
    t_kernel = int(lines[2].split(' ')[1])
    t_other = int(lines[3].split(' ')[1])

values_dict_meta = {
    "other" : [],
    "kernel" : [],
    "user" : []
}

min_time = ONE_MS_NS + 1

for task_type in values_dict_meta:
    file_name = root_dir + task_type
    with open(file_name, "r") as f:
        values_dict_meta[task_type] = np.array([int(x) for x in f.readline().split(' ') if x != ' ' and len(x)])
    if len(values_dict_meta[task_type]) == 0: continue

    for t in values_dict_meta[task_type]:
        if t < min_time: min_time = t
    values_dict_meta[task_type].sort()

    # print(*values_dict_meta[task_type])
    # print("Dataset %s info (ns)\n--------------", task_type)
    # print("Size %s" % str(len(values_dict_meta[task_type])))
    # print("max %s, min %s, median %s, std %s" % (str(values_dict_meta[task_type].max()), str(values_dict_meta[task_type].min()), str(np.median(values_dict_meta[task_type])), str(values_dict_meta[task_type].std())))


cumul_times_per_type = {
    "other" : [0] * 100,
    "kernel" : [0] * 100,
    "user" : [0] * 100
}
fig, ax = plt.subplots()
ax.ticklabel_format(useOffset=False, style='plain')
def get_task_type_t_count(t_type):
    if t_type == "user": return t_user
    elif t_type == "kernel": return t_kernel
    else: return t_other

def log_plot(times):
    buckets = []
    curr_bucket = 1000
    while curr_bucket < max(times):
        buckets.append(curr_bucket)
        curr_bucket *= 10
    
    cumul_times = [0] * len(buckets)

    for t in times:
        for i, b in enumerate(buckets):
            if t >= b:
                cumul_times[i] += 1
                break
    
    for i, counts in enumerate(cumul_times):
        if i==0:continue
        
        cumul_times[i] += cumul_times[i - 1]

    for i in range(len(cumul_times)):
        cumul_times[i] = (cumul_times[i]/len(curr)) * 100

    return cumul_times, buckets

    
for task_type in values_dict_meta:
    curr = values_dict_meta[task_type]
    if len(curr) ==0:continue
    values_count = len(curr)
    max_value = curr.max()
    times_count = None
    first_value_id = 0
    

    bucket_sz = 5000
    buckets = [0] * int((ONE_MS_NS / bucket_sz))
    buckets.append(0)

    for t in curr:
        if t > ONE_MS_NS:
            buckets[len(buckets) - 1] += 1
        else:
            buckets[t // bucket_sz] += 1
    
    for i, t in enumerate(buckets):
        if i==0:continue
        buckets[i] += buckets[i - 1]
    
    for i in range(len(buckets)):
        buckets[i] = (buckets[i] / len(curr)) * 100
    x_axis = [x + bucket_sz for x in range(0, ONE_MS_NS, bucket_sz)]
    x_axis.append(ONE_MS_NS+1)
    print(len(x_axis), len(buckets), 'serere')
    ax.set_title(cmd, fontsize=8)
    ax.set_xlabel("$t$ [ns]")
    ax.set_ylabel("Cumulative percentage")
    sns.lineplot(ax=ax, x=x_axis, y=buckets, label="%s (%s)" % (task_type, get_task_type_t_count(task_type)))
ax.set_ylim(ymin=0.0)
plt.savefig("out/%s_%s.png" % (uuid.uuid4().hex, ONE_MS_NS))
# plt.show()


# sns.scatterplot(x_axis, y_axis)
# plt.show()