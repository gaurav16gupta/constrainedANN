#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <set>
#include <iterator>
#include "MurmurHash3.h"
#include "cbloomfilter.h"
#include "crambo.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <algorithm>
#include <string> 
#include <cstdint>
using namespace std;
//4 hash functions (k = 4) for now
RAMBO::RAMBO(int b_value, int r_value)
{
    b = b_value;
    r = r_value;
    uintptr_t arr[b * r];
    bloomFilters = arr;
}

int* RAMBO::insert(set<string> *keys[], int length, int lengths[])
{
    //keys = the array of addresses of the sets of keys
    //length = the length of the keys list (a)
    //lengths[] = the lengths of the inner lists (b)
    int *setIDs;
    setIDs = (int*)calloc(b * r * length, sizeof(int));
    //setIDS = new int [b * r * length];
    n = length;
    int mean_keys = 0;
    for (int coord = 0; coord < length; coord++)
    {
        mean_keys += lengths[coord];
    }
    mean_keys /= n;
    for (int bCoord = 0; bCoord < b; bCoord++)
    {
        // BloomFilter* bloomList = new BloomFilter [r];
        // bloomFilters[bCoord] = &bloomList;

        // set<int>* listIDs = new set<int> [r];
        // setIDs[bCoord] = &listIDs;
        for (int rCoord = 0; rCoord < r; rCoord++)
        {
            int m_value = int(n * mean_keys / b * 4);
            BloomFilter bloom(m_value, 4);
            bloomFilters[bCoord * r + rCoord] = reinterpret_cast<uintptr_t>(&bloom);

            // set<int> ids;
            // listIDs[rCoord] = &ids;
        }
    }
    for (int index = 0; index < length; index++)
    {
        for (int rPos = 0; rPos < r; rPos++)
        {
            uint op;
            string str = to_string(index).c_str();
            int len = str.length();
            MurmurHash3_x86_32(str.c_str(), len, rPos, &op);
            int group_num = op % b;
            // set<int> idset = *setIDs[group_num][r];
            // idset.insert(group_num);
            // BloomFilter bloom = *bloomFilters[group_num][r];
            // bloom.insert(*keys[index]);
            setIDs[group_num * r * length + rPos * length + index] = 1;
            string keysArray[lengths[index]];
            int count = 0;
            for (string x : *keys[index])
            {
                keysArray[count] = x;
                count++;
            }
            // for (itr = *keys[index].begin(); itr != *keys[index].end(); itr++)
            // {
            //     keysArray[count] = *itr;
            // }
            (*(reinterpret_cast<BloomFilter*>(bloomFilters[group_num * r + rPos]))).insert(keysArray, count + 1);
        }
    }
    return setIDs;
}

set<int> RAMBO::query(string q, int* setIDs)
{
    int* candidateGroups = (int *)calloc(r * n, sizeof(int));
    cout << "Create candidateGroups" << endl;
    for (int rCoord = 0; rCoord < r; rCoord++)
    {
        for (int bCoord = 0; bCoord < b; bCoord++)
        {
            cout << "BloomFitler" << endl;
            if ((*(reinterpret_cast<BloomFilter*>(bloomFilters[bCoord * r + rCoord]))).query(q))
            {
                for (int setID = 0; setID < n; setID++)
                {
                    cout << "CandidateGroups" << endl;
                    candidateGroups[rCoord * n + setID] = max(candidateGroups[rCoord * n + setID], setIDs[bCoord * r * n + rCoord * n + setID]);
                }
            }
        }
    }
    cout << "Find answer" << endl;
    int* answer = (int *)calloc(n, sizeof(int));
    for (int x = 0; x < n; x++)
    {
        answer[x] = 1;
    }
    for (int rCoord = 0; rCoord < r; rCoord++)
    {
        for (int nCoord = 0; nCoord < n; nCoord++)
        {
            answer[nCoord] *= candidateGroups[rCoord * n + nCoord];
        }
    }
    int answerCount = 0;
    for (int coord = 0; coord < n; coord++)
    {
        if (answer[coord] > 0)
        {
            answerCount++;
        }
    }
    set<int> answers;
    for (int coord = 0; coord < n; coord++)
    {
        if (answer[coord] > 0)
        {
            answers.insert(answer[coord]);
        }
    }
    return answers;
}



// int main()
// {
//     cout << "It works for now..." << endl;
//     RAMBO testRambo(5, 6);
//     cout << testRambo.b << endl;
//     cout << testRambo.r << endl;
//     set<string> key1 = {"2", "3", "4", "6", "7"};
//     set<string> key2 = {"1", "2", "3", "9", "10", "11", "13"};
//     set<string> *keys[2];
//     keys[0] = &key1;
//     keys[1] = &key2;
//     int lengths[2] = {5, 7};
//     int *setIDs;
//     cout << "Insert" << endl;
//     setIDs = testRambo.insert(keys, 2, lengths);
//     cout << "Query" << endl;
//     set<int> twoResult = testRambo.query("2", setIDs);

//     cout << "Group 2";
//     for (int answer : twoResult)
//     {
//         cout << answer;
//     }
//     cout << endl;

//     set<int> sevenResult = testRambo.query("7", setIDs);
//     cout << "Group 7";
//     for (int answer : sevenResult)
//     {
//         cout << answer;
//     }
//     cout << endl;

//     set<int> oneResult = testRambo.query("1", setIDs);
//     for (int answer : oneResult)
//     {
//         cout << answer;
//     }
//     cout << endl;

//     set<int> fiveResult = testRambo.query("5", setIDs);
//     for (int answer : fiveResult)
//     {
//         cout << answer;
//     }
//     cout << endl;
//     // cout << testRambo.query("2", setIDs) << endl;
//     // cout << testRambo.query("7", setIDs) << endl;
//     // cout << testRambo.query("1", setIDs) << endl;
//     // cout << testRambo.query("5", setIDs) << endl;
// }
