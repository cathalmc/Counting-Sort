
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <stack>
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

#define LOG(x)  std::cout << x << std::endl

int* simpleRandomNet(long long int netsize, int start1, int stop1, int start2, int stop2, bool allow_self_connections) {
    std::stringstream sstm;
    sstm << "Edge_list_" << netsize << "_" << start1 << "_" << stop1 << "_" << start2 << "_" << stop2 << "_" << allow_self_connections;
    LOG(sstm.str());
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> dist1(start1, stop1 - 1);
    std::uniform_int_distribution<> dist2(start2, stop2 - 1);
    int to;
    int from;
    int* edge_list = new int[2 * netsize];
    for (int i = 0; i < netsize; i++) {
        from = dist1(rng);
        do {
            to = dist2(rng);
        } while ((to == from) && (allow_self_connections == 0));
        edge_list[2 * i] = from;
        edge_list[2 * i + 1] = to;
    }
    return edge_list;

}

void inSortEL(int* el, int no_connec) {
    //selection sort for edgelists
    int min_el, swap, temp1, temp2;
    for (int i = 0; i < no_connec; i++) {
        min_el = el[2 * i + 1];
        swap = i;
        for (int j = i; j < no_connec; j++) {
            if (el[2 * j + 1] < min_el) {
                min_el = el[2 * j + 1];
                swap = j;
            }
        }
        temp1 = el[2 * i];
        temp2 = el[2 * i + 1];
        el[2 * i] = el[2 * swap];
        el[2 * i + 1] = el[2 * swap + 1];
        el[2 * swap] = temp1;
        el[2 * swap + 1] = temp2;
    }
}

int* countSortEL(int* el, int no_connec, int maxK ,int* cumsum) {
    //sort an edge list by indegree using counting sort
    //indexing is tricky but this appears to be correct

    int* count = (int*)calloc(maxK + 2, sizeof(int));
    int* output = (int*)calloc(2*no_connec, sizeof(int));
    int j, k;

    for (int i = 0; i < no_connec; i++) count[el[(2 * i) + 1] + 1]++;
    for (int i = 1; i < maxK + 2; i++) {
        count[i] += count[i - 1];
        cumsum[i] = count[i];
    }
    for (int i = 0; i < no_connec; i++) {
        j = 2*i ;
        k = 2 * count[el[j + 1]];
        output[k] = el[j];
        output[k + 1] = el[j + 1];
        count[el[j + 1]]++;
    }

    free(count);

    return output;

}



int main()
{
    int no_connections = 100000;
    int n = 2000;
    int* net = simpleRandomNet(no_connections, 0, n, 0, n, 0);
    int* cumsum = (int*)calloc(n + 2, sizeof(int));

    double t0, t1;

    //for (int i = 0; i < no_connections; i++) LOG(net[2 * i] << "\t" << net[2 * i + 1]);

    t0 = omp_get_wtime();
    inSortEL(net, no_connections);
    t1 = omp_get_wtime();
    LOG("Time taken: " << t1 - t0);

    t0 = omp_get_wtime();
    int* sorted_net = countSortEL(net, no_connections, n, cumsum);
    t1 = omp_get_wtime();
    LOG("Time taken: " << t1 - t0);

    free(net);
    //for (int i = 0; i < no_connections; i++) LOG(sorted_net[2 * i] << "\t" << sorted_net[2 * i + 1]);
    
    
    system("pause");

}

