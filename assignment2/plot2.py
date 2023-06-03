import matplotlib.pyplot as plt
import numpy as np

def annotate2d(X , Y):
	for i in range(len(X)):
		plt.text(X[i], Y[i]-250, "({}, {})".format(xn[i], yn_pthread[i]), fontsize=10, color='red', zorder=2)
	return
	
def annotate2d_omp(X , Y):
	for i in range(len(X)):
		plt.text(X[i], Y[i]-250, "({}, {})".format(xn[i], yn_omp[i]), fontsize=10, color='blue', zorder=2)
	return

xn = np.array([
1,
2,
4,
8,
16,
32,
64
])

yn_pthread = np.array([
1658.4,
896.8,
1260.8,
2056.8,
2157.8,
2428.8,
7817.8
])

yn_omp = np.array([
960.48,
593.26,
1692.29,
13562.25,
3810.58,
4306.10,
4222.34
])

yn_pthread = yn_pthread.round(2)

annotate2d(xn,yn_pthread)
annotate2d_omp(xn,yn_omp)
plt.grid()
plt.minorticks_on()
plt.ylim(bottom=0, top=14000)
plt.xscale("log", base =2)
plt.plot(xn,yn_omp, marker='o', zorder=1,  label=r"openmp")
plt.plot(xn,yn_pthread, marker='o', zorder=1,  label=r"pthread")
plt.legend(loc="best")
plt.xlabel("K: Number of threads", fontsize=14)
plt.ylabel(r't: Elapsed Time (µs)', fontsize=14)
plt.title('Graph of Time vs Thread Count', fontsize=16)
plt.savefig("Fig2.png")
plt.show()

