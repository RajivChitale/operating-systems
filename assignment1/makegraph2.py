import matplotlib.pyplot as plt
import numpy as np

def annotate2d(X , Y):
	for i in range(len(X)):
		plt.text(X[i]-0.1, Y[i]-5, "({}, {})".format(x[i], y[i]), fontsize=12, color='red', zorder=2)
	return
	

l = range(0, 12, 2)
x=np.array([1,
2,
3,
4,
5
])

y=np.array([23358.8,
36513.6,
43209.4,
52110.8,
60026.6

])

y = y/1000
y = y.round(2)

annotate2d(x,y)
plt.grid()
plt.minorticks_on()
plt.ylim(bottom=0, top=80)
plt.plot(x,y, marker='o', zorder=1, markerfacecolor='red', label=r"k = 32 threads")
plt.legend(loc="best")
plt.xlabel(r'n: Number of points ($10^6$ points)', fontsize=14)
plt.ylabel(r't: Elapsed Time ($10^3$µs)', fontsize=14)
plt.title('Graph of Time vs Number of Points', fontsize=16)
plt.savefig("Fig2.png")
plt.show()

