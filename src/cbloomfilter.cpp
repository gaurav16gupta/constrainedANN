#include <iostream>
#include <fstream>
#include <string>
#include "MurmurHash3.h"
#include "cbloomfilter.h"
using namespace std;

//m is the size of the bloom filter
//n number of keys in the bloom filter
//p is the false positive rate
//k is the number of hash functions that we have

BloomFilter::BloomFilter(int m_value, int k_value)
{
    m = m_value;
    k = k_value;
    bf = new int[m_value];
}

void BloomFilter::insert(string keys[], int length)
{
    string key;
    int len;
    for (int seed = 0; seed < k; seed++)
    {
        for (int key_coord = 0; key_coord < length; key_coord++)
        {
            uint op;
            key = keys[key_coord].c_str();
            len = key.length();
            MurmurHash3_x86_32(key.c_str(), len, seed, &op);
            bf[op % m] = 1;
        }
    }
}

bool BloomFilter::query(std::string q)
{
    for (int seed = 0; seed < k; seed++)
    {
        uint op;
        // cout<<q.length()<<'\n';
        string str = q.c_str();
        int len = str.length();
        MurmurHash3_x86_32(str.c_str(), len, seed, &op);
        uint hash = op;
        int queryCheck = hash % m;
        if (queryCheck < 0)
        {
            queryCheck = m + queryCheck;
        }
        if (bf[queryCheck] == 0)
        {
            cout << "Doesn't contain " << queryCheck << endl;
            return false;
        }
    }
    return true;
}

void printList(int list[], int length)
{
    cout<<"[";
    for (int index = 0; index < length; index++)
    {
        cout<<list[index];
    }
    cout<<"]"<<endl;
}

// int main()
// {
//     cout<<"hello world"<<endl;
//     BloomFilter myBloom(16, 4);
//     cout << myBloom.m<<endl;
//     cout << myBloom.k<<endl;
//     printList(myBloom.bf, myBloom.m);
//     string temp[] = {"Hello", "Bello"};
//     myBloom.insert(temp, 2);
//     string tempTwo[] = {"hello", "bello"};
//     myBloom.insert(tempTwo, 2);
//     printList(myBloom.bf, myBloom.m);
//     cout << myBloom.query("Bello")<<endl;
//     cout << myBloom.query("Hello")<<endl;
//     cout << myBloom.query("hello")<<endl;
//     cout << myBloom.query("bello")<<endl;
//     cout << myBloom.query("Belllo")<<endl;
//     cout << myBloom.query("Jello")<<endl;
// }