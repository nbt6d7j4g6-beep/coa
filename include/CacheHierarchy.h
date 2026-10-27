
#ifndef CACHEHIERARCHY_H
#define CACHEHIERARCHY_H

#include "Cache.h"

class CacheHierarchy
{
private:
    Cache l1Instruction;
    Cache l1Data;
    Cache l2Unified;

    unsigned long long memoryAccesses;

public:
    CacheHierarchy(int l1Size, int l1Block, int l1Ways,
                    int l2Size, int l2Block, int l2Ways);

    void accessInstruction(uint64_t address);
    void accessData(uint64_t address);

    void runMatrixMultiplication(int n);
    void displayStats();

    uint64_t getL1InstructionHits() const;
    uint64_t getL1InstructionMisses() const;

    uint64_t getL1DataHits() const;
    uint64_t getL1DataMisses() const;

    uint64_t getL2Hits() const;
    uint64_t getL2Misses() const;

    unsigned long long getMemoryAccesses() const;
};

#endif
