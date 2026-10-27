
#include "Cache.h"
#include <iostream>
#include <iomanip>
#include <climits>
#include <stdexcept>

using namespace std;

Cache::Cache(int size, int block, int ways)
{
    if (size <= 0 || block <= 0 || ways <= 0)
        throw invalid_argument("Cache parameters must be positive.");

    if (size % block != 0)
        throw invalid_argument("Cache size must be divisible by block size.");

    int totalLines = size / block;

    if (totalLines % ways != 0)
        throw invalid_argument("Number of cache lines must be divisible by associativity.");

    cacheSize = size;
    blockSize = block;
    associativity = ways;
    numberOfSets = totalLines / ways;

    sets.resize(numberOfSets, vector<CacheLine>(associativity));

    for (int i = 0; i < numberOfSets; i++)
    {
        for (int j = 0; j < associativity; j++)
        {
            sets[i][j].valid = false;
            sets[i][j].tag = 0;
            sets[i][j].lastUsed = 0;
        }
    }

    accessCount = 0;
    hitCount = 0;
    missCount = 0;
    timer = 0;
}

bool Cache::access(uint64_t address)
{
    accessCount++;
    timer++;

    uint64_t blockNumber = address / blockSize;
    int setIndex = blockNumber % numberOfSets;
    uint64_t tag = blockNumber / numberOfSets;

    // Check whether the requested block is already in the set.
    for (int i = 0; i < associativity; i++)
    {
        if (sets[setIndex][i].valid &&
            sets[setIndex][i].tag == tag)
        {
            hitCount++;
            sets[setIndex][i].lastUsed = timer;
            return true;
        }
    }

    // The block was not found: cache miss.
    missCount++;

    // First, try to find an empty cache line.
    for (int i = 0; i < associativity; i++)
    {
        if (!sets[setIndex][i].valid)
        {
            sets[setIndex][i].valid = true;
            sets[setIndex][i].tag = tag;
            sets[setIndex][i].lastUsed = timer;
            return false;
        }
    }

    // The set is full: replace the least recently used line.
    int lruIndex = 0;
    uint64_t oldestTime = ULLONG_MAX;

    for (int i = 0; i < associativity; i++)
    {
        if (sets[setIndex][i].lastUsed < oldestTime)
        {
            oldestTime = sets[setIndex][i].lastUsed;
            lruIndex = i;
        }
    }

    sets[setIndex][lruIndex].tag = tag;
    sets[setIndex][lruIndex].lastUsed = timer;

    return false;
}

void Cache::displayStats(string cacheName)
{
    double hitRate = 0.0;
    double missRate = 0.0;

    if (accessCount > 0)
    {
        hitRate = (double)hitCount / accessCount * 100.0;
        missRate = (double)missCount / accessCount * 100.0;
    }

    cout << "\n--- " << cacheName << " Statistics ---\n";
    cout << "Total accesses : " << accessCount << '\n';
    cout << "Cache hits     : " << hitCount << '\n';
    cout << "Cache misses   : " << missCount << '\n';
    cout << fixed << setprecision(2);
    cout << "Hit rate       : " << hitRate << "%\n";
    cout << "Miss rate      : " << missRate << "%\n";
}

void Cache::resetStats()
{
    accessCount = 0;
    hitCount = 0;
    missCount = 0;
    timer = 0;

    for (int i = 0; i < numberOfSets; i++)
    {
        for (int j = 0; j < associativity; j++)
        {
            sets[i][j].valid = false;
            sets[i][j].tag = 0;
            sets[i][j].lastUsed = 0;
        }
    }
}

uint64_t Cache::getHits() const
{
    return hitCount;
}

uint64_t Cache::getMisses() const
{
    return missCount;
}

uint64_t Cache::getAccesses() const
{
    return accessCount;
}
