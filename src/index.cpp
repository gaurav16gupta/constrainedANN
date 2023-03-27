#include <iostream>
#include <fstream>
#include "FilterIndex.h"

int main(int argc, char** argv)
{
    if (argc != 4){
        std::cout << argv[0] << " dataname num_clusters numAttribites"<< std::endl;
        exit(-1);
    }
    string datapath = "../data/" + string(argv[1]) + "/base.fvecs"; 
    string labelpath = "../data/" + string(argv[1]) + "/label_base_"+string(argv[3])+".txt"; 
    string metric;
    if (string(argv[1])=="sift"){
        metric="L2";}
    if (string(argv[1])=="glove"){
        metric="Angular";}
    else{
        metric="L2";
    }
    string indexpath = string(argv[1])+ string(argv[2]) + string(argv[3]);
    cout << "filterIndex running..." << endl;
    size_t d, nb,nc, nq, num_results, buffer_size; 
    float* data = fvecs_read(datapath.c_str(), &d, &nb);
    vector<vector<string>> properties = getproperties(labelpath,' ');
    cout<<properties.size()<<endl;
    cout << "Data files read" << endl;
    nc = atoi(argv[2]); // num clusters
    FilterIndex myFilterIndex(data, d, nb, nc, properties);
    myFilterIndex.get_kmeans_index(metric, indexpath);
    // myFilterIndex.get_cluster_propertiesIndex();
    cout << "Indexed" << endl;
}
