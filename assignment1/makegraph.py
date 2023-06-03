import matplotlib.pyplot as plt
import numpy as np

def annotate2d(X , Y):
	for i in range(len(X)):
		plt.text(X[i]-1.4, Y[i]+2.3, "({}, {})".format(x[i], y[i]), fontsize=12, color='red', zorder=2)
	return
	

l = range(0, 12, 2)
x=np.array([2,
4,
8,
16,
32,
])

y=np.array([114123.8,
82727.6,
64178.2,
62277.8,
68733.6
])

y = y/1000
y = y.round(2)

annotate2d(x,y)
plt.grid()
plt.minorticks_on()
plt.ylim(bottom=0, top=130)
plt.plot(x,y, marker='o', zorder=1, markerfacecolor='red', label=r"n = $6\times10^6$ points")
plt.legend(loc="best")
plt.xlabel("k: Number of Threads", fontsize=14)
plt.ylabel(r't: Elapsed Time ($10^3$µs)', fontsize=14)
plt.title('Graph of Time vs Threads', fontsize=16)
plt.savefig("Fig1.png")
plt.show()

