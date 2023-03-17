#include <iostream>
#include <fstream>
#include "FilterIndex.h"

int main(int argc, char** argv)
{
    if (argc != 2){
        std::cout << argv[0] << " number_of_clusters"<< std::endl;
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
    myFilterIndex.get_kmeans_index(metric, indexpath);
    // myFilterIndex.get_cluster_propertiesIndex();
    cout << "Indexed" << endl;
}
