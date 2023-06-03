import sys
k = 4
N = 100

if(len(sys.argv) == 3):
	k = int(sys.argv[1])
	N = int(sys.argv[2])

print("\nGenerating dummy grid for k="+str(k)," N="+str(N))
with open("input.txt", "w+") as f:
	f.write(str(k) + " " + str(N) + "\n")
	for y in range(1, N+1):
		for x in range(1,N+1):
			f.write( str((y+x) % N +1) + " ")
		f.write("\n")
