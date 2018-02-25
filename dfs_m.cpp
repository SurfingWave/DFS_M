#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include <iostream>
#include <deque>
#include <vector>

using namespace std;

#define thresh_hold 1000
#define thread_num 2
#define tick_times_per_ms (CLOCKS_PER_SEC/1000)           


vector<long int> total_order; //Test the correctness of the total order compare to the serial DFS

bool visited_completed(bool* visited, long int vert_num)
{
	bool flag = true;
#pragma omp parallel for schedule(dynamic)
	for(long int i = 0; i < vert_num;i++)
		if(!visited[i])
			flag = false;
	return flag;
}

void dfs_m(long int vert_num, long int edge_num,
	long int *Beg, long int *Adj, bool* visited)
{
	deque<int>local_stack;    // Private local stack for each thread
	deque<int>global_idle_frontier;  // Shared global frontier stack for all threads to use.
	bool finished = false;

	omp_set_num_threads(thread_num); //Set the number of threads we use

#pragma omp parallel default(none) shared(total_order,finished, vert_num, edge_num, Adj, Beg, visited, global_idle_frontier) private(local_stack)
{
#pragma omp master // Controler thread: if global_idle_frontier is empty find an unvisted node and push to frontier
	 {
	 	while(true)
	 	{
	 		if(global_idle_frontier.empty() && !visited_completed(visited,vert_num))
	 		{
	 			for (int boot_node  = 0; boot_node < vert_num; boot_node++)
					 if(!visited[boot_node])
					 {
					 	global_idle_frontier.push_back(boot_node);
					 	break;  
					 }	 	
	 		}
	 		else if(global_idle_frontier.empty() && visited_completed(visited,vert_num))
	 		{
	 			finished = true;
	 			break;
	 		}
	 		else
	 			continue;
	 	}
	 }


    while(true && !finished)
    {
    	if(global_idle_frontier.empty())
    		continue;

    	int num_threads = omp_get_num_threads();

		#pragma omp critical
    	{
    		if(global_idle_frontier.size() < num_threads)
	    	{
	    		while(global_idle_frontier.size() !=0)
	    		{
	    			local_stack.push_back(global_idle_frontier.front());
	    			global_idle_frontier.pop_front();
	    		}
	    	}
	    	else
	    	{
	    		int push_size = global_idle_frontier.size()/num_threads;

	    		while(push_size !=0)
	    		{
	    			local_stack.push_back(global_idle_frontier.front());
	    			global_idle_frontier.pop_front();
	    			push_size--;
	    		}
	    	}
    	}
       	
		while (!local_stack.empty())
		{

			int cur = local_stack.back();
			local_stack.pop_back();

			if (!visited[cur])
			{
				#pragma omp atomic
				visited[cur] |= true;

				total_order.push_back(cur);

				//printf("Thread: %d, Node: %d\n", omp_get_thread_num(), cur);

				for (long int i = Beg[cur]; i < Beg[cur + 1] && i < edge_num; i++)
					if (!visited[Adj[i]])
						local_stack.push_back(Adj[i]);

			}

			//Equal-size partition of the large frontier
			int size = local_stack.size();

			if (size >= thresh_hold)
			{
				int temp = size / 2;

				#pragma omp critical
				while (temp > 0)	
				{
					global_idle_frontier.push_back(local_stack.front());
					local_stack.pop_front();
					temp--;
				}
			}
		}
    }
  }
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cout << "wrong number of arguments" << endl;
		cout << "./exe graph_begin_file graph_csr_file" << endl;
		exit(-1);
	}

    // read-in the *begin* and *csr* file, then construct the Graph instance 
	graph<long int,long int,long int,long int> *gr = new graph<long int,long int,long int,long int>(argv[1], argv[2]);

	long int *Beg = new long int[gr->vert_count];
	long int *Adj = new long int[gr->edge_count];

	memcpy(Beg, gr->beg_pos, sizeof(long int) * gr->vert_count);
	memcpy(Adj, gr->csr, sizeof(long int) * gr->edge_count);

    // initialize the *visited* array
    bool *visited = new bool[gr->vert_count];
    memset(visited, 0, gr->vert_count*sizeof(bool));


	long beginTime = clock();
    dfs_m(gr->vert_count, gr->edge_count, Beg, Adj, visited);
	long endTime = clock();
    cout<<"Parallel DFS_M Time(ms):  "<<(endTime-beginTime)/tick_times_per_ms<<endl; 

    long int* Parallel_result = new long int[total_order.size()]; 
    for(long int i =0; i<total_order.size(); i++)
    	Parallel_result[i] = total_order[i];


    FILE *fp;	

	if((fp=fopen("Parallel_result.bin","wb"))==NULL)
	{
		printf("Can't open the file");
		exit(0);
	}
	
	for(long int i = 0;i<total_order.size();i++)
		if(fwrite(&Parallel_result[i],sizeof(long int),1,fp) != 1)
			printf("file write error\n");

	printf("Test from visual studio github version control");

	fclose(fp);


    return 0;
}