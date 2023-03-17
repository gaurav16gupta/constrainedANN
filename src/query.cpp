#include <iostream>
#include <fstream>
#include "FilterIndex.h"

int main(int argc, char** argv)
{
    if (argc != 4){
        std::cout << argv[0] << " num_clusters number_of_queries buffer_size"<< std::endl;
        exit(-1);
    }
    cout << "filterIndex running..." << endl;
    size_t d, nb,nc, nq, num_results, buffer_size; 
    float* data = fvecs_read("../data/sift/sift/sift_base.fvecs", &d, &nb);
    vector<vector<string>> properties = getproperties("../data/sift/sift_label/label_sift_base_10.txt",' ');
    cout<<properties.size()<<endl;
    cout << "Data files read" << endl;
    nc = atoi(argv[1]); // num clusters
    FilterIndex myFilterIndex(data, d, nb, nc, properties);
    string metric="L2";
    string indexpath = "sift"+ to_string(nc);
    myFilterIndex.loadIndex(indexpath);
    cout << "Loaded" << endl;

    float* queryset = fvecs_read("../data/sift/sift/sift_query.fvecs", &d, &nq);
    vector<vector<string>> queryprops = getproperties("../data/sift/sift_label/label_sift_query_10.txt",' ');
    int* queryGTlabel = ivecs_read("../data/sift/sift/label_sift_10_hard_groundtruth.ivecs", &num_results, &nq);
    cout << "Query files read..." << endl;

    nq = atoi(argv[2]);
    buffer_size = atoi(argv[3]);
    chrono::time_point<chrono::high_resolution_clock> t1, t2;
    t1 = chrono::high_resolution_clock::now();
    myFilterIndex.query(queryset, nq, queryprops, num_results, buffer_size);
    t2 = chrono::high_resolution_clock::now();
    int32_t* output = myFilterIndex.neighbor_set;
    int output_[num_results*nq];
    copy(output, output+num_results*nq , output_);
    
    cout<<"numClusters, buffersize, QPS, Recall10@10 :"<<endl;
    //QPS
    double QPS;
    //recall
    double recall = RecallAtK(queryGTlabel, output_, num_results, nq);
    printf("%d,%d,%f,%f\n",nc, buffer_size, (double)1000000*nq/(double)chrono::duration_cast<chrono::microseconds>(t2 - t1).count(), recall);
}
