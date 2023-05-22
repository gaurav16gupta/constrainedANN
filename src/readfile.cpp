#include <cassert>
#include "readfile.h"
#include <sys/stat.h>

//#include "mnist/mnist_reader.hpp"

using namespace std;

//based off internet; make sure to change more later
vector<vector<string>> getproperties(string fileName, char dlim)
{
    vector<vector<string>> queryproperties;
    ifstream infile(fileName);
    string s;
    getline(infile, s);
    while (infile)
    {
        if (!getline(infile, s)) break;
        istringstream ss(s);
        vector<string> property;

        while (ss)
        {
            string st;
            if (!getline( ss, st, dlim )) break;
            property.push_back( st );
        }

        queryproperties.push_back(property);
    }
    if (!infile.eof())
    {
        cout << "Something is off\n";
    }
    return queryproperties;    
}

vector<vector<int>> coordinates(string fileName)
{
    vector<vector<int>> coordinates;
    ifstream infile(fileName);

    while (infile)
    {
        string s;
        if (!getline(infile, s)) break;

        istringstream ss(s);
        vector<int> coord;

        while (ss)
        {
            string s;
            if (!getline( ss, s, ',' )) break;
            coord.push_back( stoi(s) );
        }

        coordinates.push_back(coord);
    }
    if (!infile.eof())
    {
        cout << "Something is off\n";
    }
    return coordinates;    
}

//write nested vector
// void vectorWrite(const char* fname, vector<vector<uint16_t>> vec) {
//     for (int i=0; i<vec.size(), i++){
//         uint16_t d= vec[i].size();

//         for (const auto& element: vec) { 
//         file << element << " "; 
//     } 
//     }
//     FILE* f1 = fopen("centroids.bin", "w");
// fwrite(centroids, sizeof(float), nc*d, f1);

// }


//write dict unordered_map
// void mapWrite(const char* fname, unordered_map<uint16_t, vector<uint32_t>> inverted_index) {

// }


/*****************************************************
 * I/O functions for fvecs and ivecs
 *****************************************************/

float* fvecs_read(const char* fname, size_t* d_out, size_t* n_out) {
    FILE* f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "could not open %s\n", fname);
        perror("");
        abort();
    }
    int d;
    fread(&d, 1, sizeof(int), f);
    assert((d > 0 && d < 1000000) || !"unreasonable dimension");
    fseek(f, 0, SEEK_SET);
    struct stat st;
    fstat(fileno(f), &st);
    size_t sz = st.st_size;
    assert(sz % ((d + 1) * 4) == 0 || !"weird file size");
    size_t n = sz / ((d + 1) * 4);

    *d_out = d;
    *n_out = n;
    float* x = new float[n * (d + 1)];
    size_t nr = fread(x, sizeof(float), n * (d + 1), f);
    assert(nr == n * (d + 1) || !"could not read whole file");

    // shift array to remove row headers
    for (size_t i = 0; i < n; i++)
        memmove(x + i * d, x + 1 + i * (d + 1), d * sizeof(*x));

    fclose(f);
    return x;
}

// not very clean, but works as long as sizeof(int) == sizeof(float)
int* ivecs_read(const char* fname, size_t* d_out, size_t* n_out) {
    return (int*)fvecs_read(fname, d_out, n_out);
}