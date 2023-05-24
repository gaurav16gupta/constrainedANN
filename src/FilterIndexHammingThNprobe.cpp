
#include <sys/stat.h>
#include "FilterIndex.h"

using namespace std;

template <typename S>
// operator to print vectors, matrix
ostream& operator<<(ostream& os, const vector<S>& vector){
    // Printing vector elements using <<
    for (auto element : vector) {
        os << element << " ";
    }
    return os;
}

template <typename S>
ostream& operator<<(ostream& os, const vector<vector<S>>& matrix){
    // Printing matrix elements using <<
    for (const vector<S>& vector : matrix) {
        for (auto element : vector) {
            os << element << " ";
        }
    os << endl;
    }
    return os;
}

FilterIndex::FilterIndex(float* data, size_t d_, size_t nb_, size_t nc_, vector<vector<string>>properties_, string algo, int mode){
    dataset = data; // data
    d =d_; // dim
    nb = nb_; //num data points
    nc = nc_; // num clusters
    treelen = 20; //length of truncated huffman tree, num miniclusters= treelen+1
    // cin >> treelen;
    if (algo=="kmeans") {
        clusterAlgo = new Kmeans(nc, d); //dynamic allocation of object class
    }
    else if (algo=="bliss") {
        clusterAlgo = new BLISS(d, 256, nc);
        }
    else {
        cout<<"clustering unrecognised. Choosing Faiss-Kmeans as default"<<endl;
        clusterAlgo= new Kmeans(nc, d);
    }
    properties.resize(nb);
    uint8_t cnt=0;
    for (int i=0; i<nb; i++){
        for (string prp: properties_[i]){
            if (prLook.count(prp)==0){
                prLook[prp]=cnt;
                cnt++;
            }
            properties[i].push_back(prLook[prp]);
        }
        // beware:: sorting the properties will loose the position information!!
        // sort(properties[i].begin(), properties[i].end()); 
    }  
    cout<<unsigned(cnt)<<" total unique constraints"<<endl; 
    if (cnt>254) cout<<"change attribute token precision to uint16_t"<<endl;
    if (cnt>65534) cout<<"change attribute token precision to uint32_t"<<endl;
    // Properties to location map
    numAttr = properties[0].size();
    for (int i=0; i<nb; i++){
        for (int j=0; j<numAttr; j++){
            PrpAtrMap[properties[i][j]] = j;
        }
    }
}

//NN index
void FilterIndex::get_index(string metric, string indexpath, int mode){
    //to save index files
    mkdir(indexpath.c_str(), 0777);
    data_norms = new float[nb]{0};
    Lookup= new uint32_t[nb];
    counts = new uint32_t[nc+1]{0};

    clusterAlgo->train(dataset, nb, indexpath); //take the properties for Mode 3 bliss
    for(uint32_t j = 0; j < nb; ++j){  
        data_norms[j]=0;  //was a bug here, now removed         
        for(uint32_t k = 0; k < d; ++k) {   
            data_norms[j] += dataset[j*d +k]*dataset[j*d +k];        
        } 
        data_norms[j]=data_norms[j]/2;
    }
    
    uint32_t* invLookup = new uint32_t[nb];
    //get best score cluster
    #pragma omp parallel for  
    for(uint32_t i = 0; i < nb; ++i) {  
        invLookup[i] = clusterAlgo->top(dataset+ i*d);   
    }
     for(uint32_t i = 0; i < nb; ++i) {
        counts[invLookup[i]+1] = counts[invLookup[i]+1]+1; // 0 5 4 6 3
    }
    for(uint32_t j = 1; j < nc+1; ++j) {
        counts[j] = counts[j]+ counts[j-1]; //cumsum 
    }

    //argsort invLookup to get the Lookup
    iota(Lookup, Lookup+nb, 0);
    stable_sort(Lookup, Lookup+nb, [&invLookup](size_t i1, size_t i2) {return invLookup[i1] < invLookup[i2];});
    // get_mc_propertiesIndex(); // this will change counts, Lookup; and add maxMC tree
    
    FILE* f3 = fopen((indexpath+"/dataNorms.bin").c_str(), "wb");
    fwrite(data_norms, sizeof(float), nb, f3);
    fclose (f3);
    FILE* f4 = fopen((indexpath+"/Lookup.bin").c_str(), "wb");
    fwrite(Lookup, sizeof(uint32_t), nb, f4);
    fclose (f4);
    FILE* f5 = fopen((indexpath+"/counts.bin").c_str(), "wb");
    fwrite(counts, sizeof(uint32_t), nc*(treelen+1)+1, f5);
    fclose (f5);
}   

void FilterIndex::get_mc_propertiesIndex(){
    vector<vector<uint32_t>> maxMCIDs; //nested array to store the mini-clusters, change to uint32_t array later
    maxMCIDs.resize((treelen+1)*nc);
    maxMC = new uint16_t[3*(treelen+1)*nc]; 
    for (int clID = 0; clID < nc; clID++){
        if (counts[clID+1]- counts[clID]==0) continue; // what  if the cluster size is less than 4. Do something then
        //get count of vector properties        
        //get the max
        for (int h=0; h<treelen; h++){ //iterate over tree height
            unordered_map<uint16_t, int> freq; //property to frequency map
            for (int i =counts[clID]; i< counts[clID+1]; i++){ // for all points in the cluster
                for (uint16_t x: properties[Lookup[i]]){
                    if(not_in(x, maxMC + (treelen+1)*clID*3, h)) freq[x]++; // count number of items with the property x 
                }
            }
            int r = (treelen+1)*3*clID + 3*h;
            //choose property with max freq
            if (freq.end()== freq.begin()){
                maxMC[r+0]= 0; // property location
                maxMC[r+1]= 0; // property
                maxMC[r+2]= 0; // frequency
            }
            else{
                auto maxElement = max_element(freq.begin(), freq.end(),
                                    [](const pair<uint16_t, int>& p1, const pair<uint16_t, int>& p2) { return p1.second < p2.second;});
                maxMC[r+0]= PrpAtrMap[maxElement->first]; // property location
                maxMC[r+1]= maxElement->first; // property
                maxMC[r+2]= maxElement->second; // frequency, do we need this in maxMC??
            }
        }
        //maxMC serves as a node list and node data size in hamming tree, where
        //node: property from maxMC
        //node data: corresponding vector IDs
        for (int i =counts[clID]; i< counts[clID+1]; i++){
            for (int j=0; j< treelen; j++){
                int r = (treelen+1)*3*clID + 3*j;
                if((properties[Lookup[i]][maxMC[r]]==maxMC[r+1]) && (maxMC[r+2]>0)){ 
                    maxMCIDs[(treelen+1)*clID +j].push_back(Lookup[i]);
                    goto m_label;
                }
            }
            maxMCIDs[(treelen+1)*clID +treelen].push_back(Lookup[i]);
            m_label:;
        }
    }
    //need some assert statements
    //update Lookup, counts. Flatten the maxMCIDs into Lookup
    //each cluster now spans treelen+1 buckets
    Lookup= new uint32_t[nb];
    counts = new uint32_t[nc*(treelen+1)+1]{0}; 
    for (int clID = 0; clID < nc; clID++){
        for (int j=0; j< treelen+1; j++){
            int id = clID*(treelen+1) +j;
            counts[id+1] = counts[id]+ maxMCIDs[id].size();
            memcpy(Lookup+ counts[id], maxMCIDs[id].data(), sizeof(*Lookup) * maxMCIDs[id].size());
        }
    }
}

void FilterIndex::loadIndex(string indexpath){
    data_norms = new float[nb]{0};
    Lookup= new uint32_t[nb];
    counts = new uint32_t[nc+1]; 
    // counts = new uint32_t[nc*(treelen+1)+1]; 
    // maxMC = new uint16_t[3*(treelen+1)*nc]; 
    cout<<indexpath<<endl;
    clusterAlgo->load(indexpath);
    FILE* f3 = fopen((indexpath+"/dataNorms.bin").c_str(), "r");
    fread(data_norms, sizeof(float), nb, f3);
    FILE* f4 = fopen((indexpath+"/Lookup.bin").c_str(), "r");
    fread(Lookup, sizeof(uint32_t), nb, f4);
    FILE* f5 = fopen((indexpath+"/counts.bin").c_str(), "r");
    fread(counts, sizeof(uint32_t), nc+1, f5);
    get_mc_propertiesIndex();
    cout<<"here1"<<endl;
    //this changes Lookup
    for (int i =0; i< nc*(treelen+1); i++){
        int m1 = counts[i];
        int m2 = counts[i+1]; // some miniclusters are empty!!
        sort(Lookup+m1, Lookup+m2,
            [&](uint32_t a, uint32_t b) {
            return properties[a] < properties[b];
            });
    }
    cout<<"here2"<<endl;
    // reorder data and index
    dataset_reordered = new float[nb*d];
    data_norms_reordered = new float[nb];
    properties_reordered = new uint8_t[nb*numAttr];
    for(uint32_t i = 0; i < nb; ++i) {
        copy(dataset+Lookup[i]*d, dataset+(Lookup[i]+1)*d , dataset_reordered+i*d);
        data_norms_reordered[i] = data_norms[Lookup[i]];
        memcpy(properties_reordered+ i*numAttr, properties[Lookup[i]].data(), sizeof(*properties_reordered) * numAttr);
    }
    cout<<"here3"<<endl;
    delete dataset;
    delete data_norms;
    vector<vector<uint8_t>>().swap(properties);
}

void FilterIndex::query(float* queryset, int nq, vector<vector<string>> queryprops, int num_results, int nprobe){
    neighbor_set = new int32_t[nq*num_results]{-1};
    cout<<"num queries: "<<nq<<", "<<"num max attributes: "<<numAttr<<endl;
    for (size_t i = 0; i < nq; i++){
        // run query
        cout<<"query: "<<i<<endl;
        findNearestNeighbor(queryset+(i*d), queryprops[i], num_results, nprobe, i);
    }
}

// start from best cluster -> choose minicluster -> bruteforce search
void FilterIndex::findNearestNeighbor(float* query, vector<string> Stprops, int num_results, int nprobe, size_t qnum)
{   
    chrono::time_point<chrono::high_resolution_clock> t1, t2,t2_1, t3, t4, t5, t6;
    t1 = chrono::high_resolution_clock::now();
    uint8_t props[numAttr];
    uint8_t countX = 0; // count of X (missing attributes) in query props
    string querymode = "varying";
    for (size_t j =0; j<numAttr ;j++){
        if (Stprops[j] !="X") props[j]= (prLook[Stprops[j]]);
        else {
            countX++;
            props[j]=255; // this should be a key which is not seen before
        }
    }
    if (countX==numAttr) querymode = "noAttribute";
    else if (countX==0) querymode = "fixed";
    t2 = chrono::high_resolution_clock::now();
    
    // sort(props.begin(), props.end());
    priority_queue<pair<float, uint32_t> > pq;
    uint32_t simid[nc];
    float simv[nc];
    clusterAlgo->getscore(query, simv);
    t2_1 = chrono::high_resolution_clock::now();
    // need argsorted IDs
    iota(simid, simid+nc, 0);
    // good if nprobe is << nc
    if (nprobe< nc/10)
        nth_element(simid, simid + nprobe, simid + nc, [&simv](size_t i1, size_t i2) { return simv[i1] > simv[i2]; });
    else
        sort(simid, simid+nc, [&simv](size_t i1, size_t i2) {return simv[i1] > simv[i2];});

    priority_queue<pair<float, uint32_t> > Candidates_pq;
    cout<<querymode<<endl;
    uint32_t Candidates[counts[nprobe]];
    float score[counts[nprobe]];
    int seen=0, seenbin=0;
    float sim;
    float a=0,b=0;
    t3 = chrono::high_resolution_clock::now();
    if (querymode == "fixed"){
        while(seenbin<nprobe){ 
            uint32_t bin = simid[seenbin];
            seenbin++; // not if we are probing multiple subbins, in case of varying #attrs
            int id = bin*(treelen+1);
            bin = bin*(treelen+1);

            //get which sub-cluster query belongs to
            uint16_t membership = getclusterPart(maxMC+ bin*3 , props, treelen);
            bin = bin+membership;
            for (int i =counts[bin]; i< counts[bin+1]; i++){
                // __builtin_prefetch (properties_reordered +(i+2)*numAttr, 0, 2); software prefect is not very useful here
                //check if constraint statisfies
                int j =0;
                while (j<numAttr && properties_reordered[i*numAttr +j]== props[j]) j++; // plus the number of empty attrs X
                if (j==numAttr){
                    Candidates[seen]=i; 
                    seen++;
                }
            }
        }
    }
    else if (querymode == "varying"){
        cout<<nprobe<<endl;
        while(seenbin<nprobe){ 
            cout<<"seenbin :"<<seenbin<<endl;
            uint32_t bin = simid[seenbin];
            seenbin++; // not if we are probing multiple subbins, in case of varying #attrs
            bin = bin*(treelen+1);
            // go through each sub partition
            bool checkremaining=true;
            for (uint16_t u=0;u<treelen; u++){
                cout<<u<<" ";
                if (props[maxMC[u*3+0]]==255){
                    for (int i =counts[bin+u]; i< counts[bin+u+1]; i++){
                        //check if constraint statisfies
                        int j =0;
                        while (j<numAttr && (properties_reordered[i*numAttr +j]== props[j] | props[j]==255)) j++; 
                        if (j==numAttr){
                            Candidates[seen]=i; 
                            seen++;
                        }
                    }
                }
                else if (maxMC[u*3+1] == props[maxMC[u*3+0]]){
                    for (int i =counts[bin+u]; i< counts[bin+u+1]; i++){
                        //check if constraint statisfies
                        int j =0;
                        while (j<numAttr && (properties_reordered[i*numAttr +j]== props[j] | props[j]==255)) j++; 
                        if (j==numAttr){
                            Candidates[seen]=i; 
                            seen++;
                        }
                    }
                    checkremaining = false;
                    break;
                }
            }
            if (checkremaining == true){
                cout<<"checkremaining"<<" ";
                for (int i =counts[bin+ treelen]; i< counts[bin+ treelen+1]; i++){
                        //check if constraint statisfies
                        int j =0;
                       while (j<numAttr && (properties_reordered[i*numAttr +j]== props[j] | props[j]==255)) j++; 
                        if (j==numAttr){
                            Candidates[seen]=i; 
                            seen++;
                        }
                }
            }
            cout<<endl;
        }
    }
    else if (querymode == "noAttribute"){
        while(seenbin<nprobe){ 
            uint32_t bin = simid[seenbin];
            seenbin++; // not if we are probing multiple subbins, in case of varying #attrs
            bin = bin*(treelen+1);
            // go through each sub partition
            for (uint16_t u=0;u<treelen; u++){
                for (int i =counts[bin+u]; i< counts[bin+u+1]; i++){
                    Candidates[seen]=i; 
                    seen++;
                }
            }  
        }
    }
    else cout << "querymode got a different val somehow"<<endl;
    t4 = chrono::high_resolution_clock::now();
    // NN distance computations
    float maxk;
    cout<<"end of getting candidates "<<endl;
    if (seen<num_results+1){
        for (int i =0; i< seen; i++){ 
            neighbor_set[qnum*num_results+ i] = Lookup[Candidates[i]];
        }
    }
    else{
        for (int i =0; i< seen; i++){
            score[i] = -L2SIMD4ExtAVX(query, dataset_reordered +Candidates[i]*d, data_norms_reordered[Candidates[i]], d);
        }
        for (int i =0; i< num_results; i++){ 
            Candidates_pq.push({score[i],Candidates[i]});
        }
        maxk = Candidates_pq.top().first;
        for (int i =num_results; i< seen; i++){ 
            if (score[i]< maxk){
                maxk = Candidates_pq.top().first;
                Candidates_pq.pop();
                Candidates_pq.push({score[i], Candidates[i]});
            }
        }
        for (int i =0; i< num_results; i++){ 
            neighbor_set[qnum*num_results+ i] = Lookup[Candidates_pq.top().second];
            Candidates_pq.pop();
        }
    }
    
    t5 = chrono::high_resolution_clock::now();
    // cout<<"time: "<<chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count()<<" ";
    // cout<<chrono::duration_cast<chrono::nanoseconds>(t2_1 - t2).count()<<" ";
    // cout<<chrono::duration_cast<chrono::nanoseconds>(t3 - t2_1).count()<<" ";
    // cout<<chrono::duration_cast<chrono::nanoseconds>(t4 - t3).count()<<" ";
    // cout<<chrono::duration_cast<chrono::nanoseconds>(t5 - t4).count()<<endl;    
}

// TODO
// 1) change dtype of the properties (uint16_t/uint32_t/uint8_t), based on the vocab size
