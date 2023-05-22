
//-----------------Unused functions--------------------------------------------------------------------------------------------------------------------------------


   // //check for logical errors
    // for (int i =counts[10]; i< counts[11]; i++){
    //     cout<<Lookup[i]<<" ";
    // }
    // cout<<endl;
    // for (int clID = 11; clID < 12; clID++){
    //     cout<<counts[clID]-counts[clID-1]<<endl; 
    // }

       // //check for logical errors
    // for (int clID = 10; clID < 11; clID++){
    //     for (int j=0; j< treelen+1; j++){
    //         int id = clID*(treelen+1) +j;
    //         cout<<maxMCIDs[id]<<endl;
    //         // for (int i =counts[j]; i< counts[j+1]; i++){
    //         //     cout<<Lookup[i]<<" ";}
    //         //     cout<<endl;
    //     }
    //     cout<<endl;
    // }

vector<int> argsort(int * query, int length)
{
    //A helper function to find the top num_results args in the coords closest to query
    //O(nk)
    vector<tuple<int, int>> sorted;
    for (int x = 0; x < length; x ++)
    {
        sorted.push_back({x, -1.0});
    }
    for (int index = 0; index < length; index++)
    {
        for (int order = 0; order < length; order++)
        {
            if (query[index] < get<1>(sorted.at(order)) || get<1>(sorted.at(order)) < 0)
            {
                sorted.insert(sorted.begin() + order, {index, query[index]});
                sorted.pop_back();
                break;
            }
        }
    }
    vector<int> nearest_neighbors;
    for (int index = 0; index < length; index++)
    {
        nearest_neighbors.push_back(get<0>(sorted.at(index)));
    }
    return nearest_neighbors;
}

vector<int> vectorArgsort(vector<double> query)
{
    //A helper function to find the top num_results args in the coords closest to query
    //O(nk)
    vector<tuple<int, double>> sorted;
    for (size_t x = 0; x < query.size(); x ++)
    {
        sorted.push_back({x, -1.0});
    }
    for (size_t index = 0; index < query.size(); index++)
    {
        for (size_t order = 0; order < query.size(); order++)
        {
            if (query[index] < get<1>(sorted.at(order)) || get<1>(sorted.at(order)) < 0)
            {
                sorted.insert(sorted.begin() + order, {index, query[index]});
                sorted.pop_back();
                break;
            }
        }
    }
    vector<int> nearest_neighbors;
    for (size_t index = 0; index < query.size(); index++)
    {
        nearest_neighbors.push_back(get<0>(sorted.at(index)));
    }
    return nearest_neighbors;
}


// save a load multiple vectors into the same bin file----
// bool save(const char * filename){
//     std::ofstream out(filename, std::ios::binary);
//     int a=A.size(), b=B.size(), c=C.size(), d=D.size(), e=E.size(), f=F.size();
//     out.write(reinterpret_cast<const char*>(&a), sizeof(a));
//     out.write(reinterpret_cast<const char*>(&b), sizeof(b));
//     out.write(reinterpret_cast<const char*>(&c), sizeof(c));
//     out.write(reinterpret_cast<const char*>(&d), sizeof(d));
//     out.write(reinterpret_cast<const char*>(&e), sizeof(e));
//     out.write(reinterpret_cast<const char*>(&f), sizeof(f));

//     out.write(reinterpret_cast<const char*>(&A[0]), sizeof(int)*A.size());
//     out.write(reinterpret_cast<const char*>(&B[0]), sizeof(int)*B.size());
//     out.write(reinterpret_cast<const char*>(&C[0]), sizeof(int)*C.size());
//     out.write(reinterpret_cast<const char*>(&D[0]), sizeof(int)*D.size());
//     out.write(reinterpret_cast<const char*>(&E[0]), sizeof(int)*E.size());
//     out.write(reinterpret_cast<const char*>(&F[0]), sizeof(int)*F.size());

//     // always check to see if the file opened, and if writes succeeded.  
//     // I am being lazy here so I can focus on the actual question
//     return true;
// }

// bool load(const char *filename){
//     std::ifstream in(filename, std::ios::binary);
//     int a, b, c, d, e, f;
//     in.read(reinterpret_cast<char*>(&a), sizeof(a));
//     in.read(reinterpret_cast<char*>(&b), sizeof(b));
//     in.read(reinterpret_cast<char*>(&c), sizeof(c));
//     in.read(reinterpret_cast<char*>(&d), sizeof(d));
//     in.read(reinterpret_cast<char*>(&e), sizeof(e));
//     in.read(reinterpret_cast<char*>(&f), sizeof(f));
//     A.resize(a); B.resize(b); C.resize(c); D.resize(d); E.resize(e); F.resize(f);

//     in.read(reinterpret_cast<char*>(&A[0]), sizeof(int)*A.size());
//     in.read(reinterpret_cast<char*>(&B[0]), sizeof(int)*B.size());
//     in.read(reinterpret_cast<char*>(&C[0]), sizeof(int)*C.size());
//     in.read(reinterpret_cast<char*>(&D[0]), sizeof(int)*D.size());
//     in.read(reinterpret_cast<char*>(&E[0]), sizeof(int)*E.size());
//     in.read(reinterpret_cast<char*>(&F[0]), sizeof(int)*F.size());

//     // always check to see if the file opened, and if writes succeeded.  
//     // I am being lazy here so I can focus on the actual question
//     return true;
// }



// vector<vector<int>> computeGroundTruth(vector<vector<int>> queryset, vector<set<string>> queryprops, vector<vector<int>> data, vector<set<string>> properties, int num_results)
// {
//     vector<vector<int>> answers;
//     for (int x = 0; x < queryset.size(); x++)
//     {
//         vector<int> answer;
//         set<string> props = queryprops[x];
//         vector<int> validIDs;
//         for (int y = 0; y < properties.size(); y++)
//         {
//             bool valid = true;
//             for (auto property : props)
//             {
//                 if (!properties[y].count(property))
//                 {
//                     valid = false;
//                 }
//             }
//             if (valid)
//             {
//                 validIDs.push_back(y);
//             }
//         }
//         cout << "num validIDs: " << validIDs.size() << endl;

//         vector<vector<int>> validCoords;
//         for (int a = 0; a < validIDs.size(); a++)
//         {
//             validCoords.push_back(data[validIDs[a]]);
//         }

//         cout << "num validCoords: " << validCoords.size() << endl;
        
//         vector<uint32_t> topArgs = argTopK(queryset[x], validCoords, num_results);

//         vector<uint32_t> argTopK(float* query, float* vectors, uint32_t d, uint32_t N, vector<uint32_t> idx, uint32_t idxSize, int k, float* topkDist){


//         cout << "topArgs: ";
//         for (int arg : topArgs)
//         {
//             cout  << arg << ", ";
//         }
//         cout << endl;

//         for (int z = 0; z < topArgs.size(); z++)
//         {
//             answer.push_back(validIDs[topArgs[z]]);
//         }
//         answers.push_back(answer);
//     }
//     return answers;
// }