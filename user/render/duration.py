import seaborn
import matplotlib.pyplot as plt
from sys import argv
from os import listdir
import uuid
TYPE_KERNEL = 1
TYPE_USER = 2
TYPE_OTHER = 3

t_kernel = []
t_other = []
t_user = []
min_user_time = 1e32
max_user_time = 0

if len(argv) != 2:
        print("forgot src dir")

root_dir = argv[1]


cmd = ""
n_user = ""
n_kernel = ""
n_other = ""

with open(root_dir + "/meta", "r") as meta_f:
    lines = [l.replace('\n', '') for l in meta_f.readlines()]
    print(lines)
    cmd = lines[0]
    n_user = int(lines[1].split(' ')[1])
    n_kernel = int(lines[2].split(' ')[1])
    n_other = int(lines[3].split(' ')[1])

def t_to_str(t):
        if t==1:return "kernel (%s)" % str(n_kernel)
        elif t==2: return "user (%s)" % str(n_user)
        else: return "other (%s)" % str(n_other)


str_arr_to_int = lambda arr: [int(x) for x in arr if x != '' and x != ' ']


target_dir = "%s/tasks/user" % root_dir
for f in listdir(target_dir):
        wakeup_times, durations = [], []
        with open(target_dir + "/" + f, "r") as data:
                lines = data.readlines()
                if len(lines) == 0 or len(lines) == 1: continue
                try:
                        wakeup_times, durations = [str_arr_to_int(l.replace("\n", "").split(' ')) for l in lines]
                except:
                        pass
        for t in list(zip(wakeup_times, durations)):
                if (t[0] - t[1]) < min_user_time:
                        min_user_time = t[0] - t[1]
                if t[0] > max_user_time:
                        max_user_time = t[0]
        t_user += list(zip(wakeup_times, durations, [TYPE_USER for i in range(len(wakeup_times))]))

print("min : %d, max : %d, durtion : %d" % (min_user_time, max_user_time, max_user_time - min_user_time))

target_dir = "%s/tasks/kernel" % root_dir
for f in listdir(target_dir):
        wakeup_times, durations = [], []
        with open(target_dir + "/" + f, "r") as data:
                lines = data.readlines()
                if len(lines) == 0 or len(lines) == 1: continue
                try:
                        wakeup_times, durations = [str_arr_to_int(l.replace("\n", "").split(' ')) for l in lines]
                except:
                        pass
        t_kernel += list(zip(wakeup_times, durations, [TYPE_KERNEL for i in range(len(wakeup_times))]))
t_kernel.sort(key=lambda x:x[0])
start = 0
end = len(t_kernel)
for i in range(len(t_kernel)):
        if t_kernel[i][0] < min_user_time:
                start = i
        # if t_kernel[i][0] > max_user_time:
        #         end = i
        #         break

t_kernel = t_kernel[start:end-1]


target_dir = "%s/tasks/other" % root_dir
for f in listdir(target_dir):
        wakeup_times, durations = [], []
        with open(target_dir + "/" + f, "r") as data:
                lines = data.readlines()
                if len(lines) == 0 or len(lines) == 1: continue
                wakeup_times, durations = [str_arr_to_int(l.replace("\n", "").split(' ')) for l in lines]
        t_other += list(zip(wakeup_times, durations, [TYPE_OTHER for i in range(len(wakeup_times))]))

t_other.sort(key=lambda x:x[0])
start = 0
end = len(t_other)
for i in range(len(t_other)):
        if t_other[i][0] < min_user_time:
                start = i
        # if t_other[i][0] > max_user_time:
        #         end = i
        #         break

t_other = t_other[start:end - 1]

print("kernel : %d, user : %d, other : %d" % (len(t_kernel), len(t_user), len(t_other)))
data = t_kernel + t_other + t_user
data.sort(key=lambda times: times[0])
t_user.sort(key=lambda x:x[0])
from_zero = []

oldest_sleep = 1 << 32
for t in t_user:
        if (t[0] - t[1]) < oldest_sleep:
                oldest_sleep = t[0] - t[1]
min_time = data[0][0]
for i in range(len(data)):
        from_zero.append(data[i][0] - min_time)


print(from_zero[len(from_zero) - 1])
print(data[0])
print((t_user[0][0] - t_user[0][1]))
durations = [t[1] for t in data]

fig, ax = plt.subplots()

dd = {
        "t [ns]": from_zero,
        "blocked for [ns]": durations,
        "Task type": [t_to_str(x[2]) for x in data]
}

plt.ticklabel_format(style='plain', axis='x')
# ax.plot(wakeup_times[:100], durations[:100])
# colors = ["blue", "pink", "orange"]
seaborn.set_style("dark")
g = seaborn.scatterplot(ax=ax, data=dd, x="t [ns]", y="blocked for [ns]", hue='Task type', markers=False,s=3)
plt.xticks(rotation=20)
ax.set(yscale="log")
ax.set_title(cmd, fontsize=8)
ax.set_xlim(xmin=0, xmax=int(from_zero[len(from_zero) - 1] * 1.05))
plt.legend(loc='center left', bbox_to_anchor=(0.80, 1.05))

# ax.set(xlabel=None)
# ax.scatter(x=wakeup_times, y=durations)
# plt.savefig("out/durations/durations_%s.png"%uuid.uuid4().hex)
# plt.savefig("out/durations/test.png")
plt.show()

