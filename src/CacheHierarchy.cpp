
#include "CacheHierarchy.h"
#include <iostream>

using namespace std;

CacheHierarchy::CacheHierarchy(int l1Size, int l1Block, int l1Ways,
                               int l2Size, int l2Block, int l2Ways)
    : l1Instruction(l1Size, l1Block, l1Ways),
      l1Data(l1Size, l1Block, l1Ways),
      l2Unified(l2Size, l2Block, l2Ways)
{
    memoryAccesses = 0;
}

void CacheHierarchy::accessInstruction(uint64_t address)
{
    if (!l1Instruction.access(address))
    {
        if (!l2Unified.access(address))
        {
            memoryAccesses++;
        }
    }
}

void CacheHierarchy::accessData(uint64_t address)
{
    if (!l1Data.access(address))
    {
        if (!l2Unified.access(address))
        {
            memoryAccesses++;
        }
    }
}

void CacheHierarchy::runMatrixMultiplication(int n)
{
    cout << "Running " << n << " x " << n
         << " matrix workload...\n";

    // Count simulated instruction fetches.
    uint64_t instructionCounter = 0;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                // Simulate instructions spanning four cache blocks.
                uint64_t instructionAddress =
                    40000 + (instructionCounter % 4) * 64;

                instructionCounter++;

                accessInstruction(instructionAddress);

                // Simulated read from matrix A.
                uint64_t addressA =
                    10000 + (i * n + k) * 4;

                // Simulated read from matrix B.
                uint64_t addressB =
                    20000 + (k * n + j) * 4;

                // Simulated write to matrix C.
                uint64_t addressC =
                    30000 + (i * n + j) * 4;

                accessData(addressA);
                accessData(addressB);
                accessData(addressC);
            }
        }
    }
}

void CacheHierarchy::displayStats()
{
    cout << "\n===== TWO-LEVEL CACHE STATISTICS =====\n";

    l1Instruction.displayStats("L1 Instruction Cache");
    l1Data.displayStats("L1 Data Cache");
    l2Unified.displayStats("L2 Unified Cache");

    cout << "\nMain Memory Accesses: "
         << memoryAccesses << '\n';
}

uint64_t CacheHierarchy::getL1InstructionHits() const
{
    return l1Instruction.getHits();
}

uint64_t CacheHierarchy::getL1InstructionMisses() const
{
    return l1Instruction.getMisses();
}

uint64_t CacheHierarchy::getL1DataHits() const
{
    return l1Data.getHits();
}

uint64_t CacheHierarchy::getL1DataMisses() const
{
    return l1Data.getMisses();
}

uint64_t CacheHierarchy::getL2Hits() const
{
    return l2Unified.getHits();
}

uint64_t CacheHierarchy::getL2Misses() const
{
    return l2Unified.getMisses();
}

unsigned long long CacheHierarchy::getMemoryAccesses() const
{
    return memoryAccesses;
}
