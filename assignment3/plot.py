import matplotlib.pyplot as plt
import numpy as np
import re

with open("autorun.txt") as f:
    # Content_list is the list that contains the read lines.
    full = f.read().splitlines()

tasi = full.index('tas')
casi = full.index('cas')
basi = full.index('cas-bounded')

tas = full[tasi+1:casi]
cas = full[casi+1:basi]
bas = full[basi+1:]

# print(tas)
# print(cas)
# print(bas)


def find_between(s, first, last):
    try:
        start = s.index(first) + len(first)
        end = s.index(last, start)
        return s[start:end]
    except ValueError:
        return ""


def share(x):
    tavg = []
    tmax = []
    xavg = []
    xmax = []
    for e in x:
        a = find_between(e, "Average waiting time: ", " ms") 
        if(a!=""):
            tavg.append(float(a))

        b = find_between(e, "Maximum waiting time: ", " ms") 
        if(b!=""):
            tmax.append(float(b))
        
        if(len(tavg) == 5):
            xavg.append( round( sum(tavg)/5, 2) )
            tavg = []

        if(len(tmax) == 5):
            xmax.append( round(sum(tmax)/5, 2) )
            tmax = []

    return np.array(xavg), np.array(xmax)

def annotate2d(X , Y, dx, dy):
	for i in range(len(X)):
		plt.text(X[i]+dx, Y[i]+dy, "({}, {})".format(X[i], Y[i]), fontsize=10, color='red', zorder=2)
	return

xn = np.array([
10,
20,
30,
40,
50
])

ytas1, ytas2 = share(tas)

ycas1, ycas2 = share(cas)

ybas1, ybas2 = share(bas)

print(ytas1)
print(ytas2)
print(ycas1)
print(ycas2)
print(ybas1)
print(ybas2)

#ytas1 = ytas1.round(2)

annotate2d(xn,ytas1, -1, 60)
annotate2d(xn,ycas1, 0, -60)
annotate2d(xn,ybas1, -1, 50)

plt.grid()
plt.minorticks_on()
#plt.ylim(bottom=0, top=14000)
plt.plot(xn,ytas1, marker='o', zorder=1,  label=r"tas")
plt.plot(xn,ycas1, marker='o', zorder=1,  label=r"cas")
plt.plot(xn,ybas1, marker='o', zorder=1,  label=r"cas-bounded")

plt.legend(loc="best")
plt.xlabel("n: Number of threads", fontsize=14)
plt.ylabel("t_avg: Average Waiting Time (ms)", fontsize=14)
plt.title('Graph of Average Waiting Time vs Thread Count', fontsize=16)
plt.savefig("Fig1.png")
plt.show()




annotate2d(xn,ytas2, -5, 70)
annotate2d(xn,ycas2, 0, -50)
annotate2d(xn,ybas2, 1, -50)

plt.plot(xn,ytas2, marker='o', zorder=1,  label=r"tas")
plt.plot(xn,ycas2, marker='o', zorder=1,  label=r"cas")
plt.plot(xn,ybas2, marker='o', zorder=1,  label=r"cas-bounded")

plt.legend(loc="best")
plt.xlabel("n: Number of threads", fontsize=14)
plt.ylabel("t_max: Maximum Waiting Time (ms)", fontsize=14)
plt.title('Graph of Maximum Waiting Time vs Thread Count', fontsize=16)
plt.savefig("Fig2.png")
plt.show()

# def place(x, xavg, xmax):
# 	tavg = []
# 	tmax = []
# 	for e in x:
# 		if("Average" in e):
# 			tavg.append( float(e[e.find(':')+1:e.find('ms')]) )
# 		elif("Maximum" in e):
# 			tmax.append( float(e[e.find(':')+1:e.find('ms')]) )
# 		if(len(tavg) == 5):
# 			xavg.append(sum(tavg)/5)
# 			tavg = []
# 		if(len(tmax) == 5):
# 			xmax.append(sum(tmax)/5)
# 			tmax = []
# place(tas, tasavg, tasmax)
# place(cas, casavg, casmax)
# place(bas, basavg, basmax)

# print(tasavg)
