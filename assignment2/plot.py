import matplotlib.pyplot as plt
import numpy as np

def annotate2d(X , Y):
	for i in range(len(X)):
		plt.text(X[i]-5, Y[i]+150, "({}, {})".format(xn[i], yn_pthread[i]), fontsize=10, color='red', zorder=2)
	return
	
def annotate2d_omp(X , Y):
	for i in range(len(X)):
		plt.text(X[i]-5, Y[i]-200, "({}, {})".format(xn[i], yn_omp[i]), fontsize=10, color='blue', zorder=2)
	return

xn = np.array([9,
16,
25,
36,
49,
64,
81,
100
])

yn_pthread = np.array([
1094.6,
1561.6,
2181,
1937.6,
2430.4,
3200.6,
3074.2,
3821.4
])

yn_omp = np.array([
620.72,
1124.74,
1318.06, 
2018.79,
2155.96,
3086.72,
2857.25,
3128.03
])

yn_pthread = yn_pthread.round(2)

annotate2d(xn,yn_pthread)
annotate2d_omp(xn,yn_omp)
plt.grid()
plt.minorticks_on()
plt.ylim(bottom=0, top=4000)
plt.plot(xn,yn_omp, marker='o', zorder=1,  label=r"openmp")
plt.plot(xn,yn_pthread, marker='o', zorder=1,  label=r"pthread")
plt.legend(loc="best")
plt.xlabel("N: Width of Sudoku Grid", fontsize=14)
plt.ylabel(r't: Elapsed Time (µs)', fontsize=14)
plt.title('Graph of Time vs Grid Size', fontsize=16)
plt.savefig("Fig1.png")
plt.show()

