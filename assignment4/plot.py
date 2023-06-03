import numpy as np
import matplotlib.pyplot as plt

with open("autorun.txt") as f:
    # Content_list is the list that contains the read lines.
    full = f.read().splitlines()

def find_between(s, first, last):
    try:
        start = s.index(first) + len(first)
        end = s.index(last, start)
        return s[start:end]
    except ValueError:
        return ""
    
iterations = 0
index = 0
mode = 1
y1sum = []
y2sum = []

substr = ["Average tour time = ", "Average car ride time = "]

# given str, mode for each plot, and index of datapoint -> inserts to list
def storeTimeData(line, mode, index):
    temp= find_between(line, substr[mode-1], " ms") 

    if(temp==""): return -1
    temp = float(temp)

    if(iterations==0):
        if(mode==1): y1sum.append(temp)
        elif(mode==2): y2sum.append(temp)
    else:
        if(mode==1): y1sum[index] += temp
        elif(mode==2): y2sum[index] += temp


for line in full:
    if line == "": continue
    if "PLOT1" in line:
        index=0
        mode=1
        continue
    if "PLOT2" in line:
        index=0
        mode=2
        continue
    if "END OF ITERATION" in line:
        iterations+=1
        continue
    storeTimeData(line, mode, index)
    index+=1
    
y1 = np.array(y1sum)/iterations
y2 = np.array(y2sum)/iterations
y1= np.round(y1, 2)
y2= np.round(y2, 2)

x1 = np.arange(10, 51, 5)
x2 = np.arange(5, 26, 5)

print(y1)
print(x1)
print(y2)
print(x2)

def annotate2d(X , Y, dx, dy, col):
	for i in range(len(X)):
		plt.text(X[i]+dx, Y[i]+dy, "({}, {})".format(X[i], Y[i]), fontsize=10, color=col, zorder=2)
	return

plt.grid()
plt.minorticks_on()

#plt.ylim(bottom=0, top=4000)
plt.plot(x1,y1, marker='o', zorder=1, label=r"C=25, k=5, $\lambda_P$=60ms, $\lambda_C$=110ms")
annotate2d(x1,y1, 0.5,-1, "red")
plt.legend(loc="best")
plt.xlabel("P: Number of Passengers", fontsize=12)
plt.ylabel(r"$T_{tour}$: Average tour time(ms)", fontsize=12)
plt.title('Average Tour Time vs Number of Passengers', fontsize=16)
plt.savefig("Plot1.png")
plt.show()


plt.grid()
plt.minorticks_on()

plt.plot(x2,y2, marker='o', zorder=1, label=r"P=50, k=3, $\lambda_P$=60ms, $\lambda_C$=110ms")
annotate2d(x2,y2, 0,0, "red")
plt.legend(loc="best")
plt.xlabel("C: Number of Cars", fontsize=14)
plt.ylabel(r'$T_{ride}$: Average car riding time(ms)', fontsize=14)
plt.title('Average Riding Time Of Cars vs Number of Cars', fontsize=16)
plt.savefig("Plot2.png")
plt.show()
