#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cstdint>
#include<string>

using namespace std;

struct CacheLine
{
    bool valid;
    uint64_t tag;
    uint64_t lastUsed;
};

class Cache
{
private:
    int cacheSize;
    int blockSize;
    int associativity;
    int numberOfSets;

    vector<vector<CacheLine>> sets;

    uint64_t accessCount;
    uint64_t hitCount;
    uint64_t missCount;
    uint64_t timer;

public:
    Cache(int size, int block, int ways);

    bool access(uint64_t address);

    void displayStats(string cacheName);

    void resetStats();

    uint64_t getHits() const;
    uint64_t getMisses() const;
    uint64_t getAccesses() const;
};

#endif
