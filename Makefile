cc = g++
flag = -fopenmp -std=c++11 
OBJ = dfs_m.o

dfs_m:$(OBJ)
	$(cc) -o $@ $^ $(flag)

$(OBJ):dfs_m.cpp
	$(cc) -c $^ $(flag)

clean:
	rm -rf *.o dfs_m
