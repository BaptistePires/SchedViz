import seaborn as sns
import matplotlib.pyplot as plt

durations = [100, 132, 2443]
wakeup_times = [1032, 2345, 2500]

from_zero = []

times = list(zip(wakeup_times, durations))
times.sort(key=lambda x:x[0])
min_wup_time = times[0][0]

for i in range(len(times)):
    from_zero.append(times[i][0] - min_wup_time)

y_axis = [x[1] for x in times]

fig, ax = plt.subplots()
ax.scatter(x=from_zero, y=y_axis)
plt.show()
