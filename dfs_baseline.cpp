#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include <iostream>
#include <thread>
#include <stack>
#include <vector>

using namespace std;

#define thread_num 1 
#define tick_times_per_ms (CLOCKS_PER_SEC/1000)           


vector<long int> total_order; //Test the correctness of the total order compare to the serial DFS

int get_begin(long int *Beg, long int v_num)
{
	int i = 0;
	while (i < v_num)
	{
		if (Beg[i] != 0)
			return i - 1;
		i++;
	}
}

void DFS_Util(int start_node, long int vert_num, long int edge_num,
	long int *Beg, long int *Adj, bool* visited)
{
	stack<int>re;
	re.push(start_node);
	int temp;

	while (!re.empty())
	{
		temp = re.top(); re.pop();

		if (!visited[temp])
		{
			visited[temp] = true;
			//printf("%d \n", temp);
			total_order.push_back(temp);
		}
		for (int i = Beg[temp]; i < Beg[temp + 1] && i < edge_num; i++)
		{
			if (!visited[Adj[i]])
				re.push(Adj[i]);
		}
	}
	return;
}

void DFS(long int vert_num, long int edge_num,
	long int *Beg, long int *Adj, bool* visited)
{
    for (int i = 0; i < vert_num; i++)
    	if(!visited[i])
        	DFS_Util(i, vert_num, edge_num, Beg, Adj, visited);
}

int main(int argc, char **argv)
{
	thread t[thread_num];

	if (argc < 3)
	{
		cout << "wrong number of arguments" << endl;
		cout << "./exe graph_begin_file graph_csr_file" << endl;
		exit(-1);
	}

	graph<long int,long int,long int,long int> *gr = new graph<long int,long int,long int,long int>(argv[1], argv[2]);

	long int *Beg = new long int[gr->vert_count];
	long int *Adj = new long int[gr->edge_count];
	bool *visited = new bool[gr->vert_count];

	memcpy(Beg, gr->beg_pos, sizeof(long int) * gr->vert_count);
	memcpy(Adj, gr->csr, sizeof(long int) * gr->edge_count);
    memset(visited, 0, gr->vert_count*sizeof(bool));

	long beginTime = clock();
	t[0] = thread(&DFS, gr->vert_count, gr->edge_count, Beg, Adj, visited);  //start up master thread
	t[0].join();              // synchronization of all threads
	long endTime = clock();
    cout<<"Serial_Baseline DFS Time(ms):  "<<(endTime-beginTime)/tick_times_per_ms<<endl;  

    long int* Serial_result = new long int[total_order.size()]; 
    for(long int i =0; i<total_order.size(); i++)
    	Serial_result[i] = total_order[i];

    FILE *fp;	

	if((fp=fopen("Serial_result.bin","wb"))==NULL)
	{
		printf("Can't open the file");
		exit(0);
	}

	for(long int i = 0;i < total_order.size();i++)
		if(fwrite(&Serial_result[i],sizeof(long int),1,fp) != 1)
			printf("file write error\n");
		
	cout<<"total_order.size" <<total_order.size()<<endl;

	fclose(fp);

    return 0;
}