#include <iostream>
#include <fstream>
#include <iomanip>
#include <exception>
#include <cstdint>
#include "CacheHierarchy.h"

using namespace std;

struct Configuration
{
    int l1Size;
    int l1Block;
    int l1Ways;

    int l2Size;
    int l2Block;
    int l2Ways;

    string name;
};

double hitRate(uint64_t hits, uint64_t misses)
{
    uint64_t total = hits + misses;

    if (total == 0)
        return 0.0;

    return (double)hits / total * 100.0;
}

int main()
{
    Configuration experiments[] =
    {
        {1024, 64, 2, 4096, 64, 4, "Baseline"},
        {2048, 64, 2, 4096, 64, 4, "Larger_L1"},
        {1024, 32, 2, 4096, 64, 4, "Smaller_L1_Block"},
        {1024, 64, 4, 4096, 64, 4, "Higher_L1_Associativity"},
        {1024, 64, 2, 8192, 64, 4, "Larger_L2"},
        {1024, 64, 2, 4096, 64, 8, "Higher_L2_Associativity"}
    };

    int workloadSizes[] = {8, 16, 32};

    int experimentCount =
        sizeof(experiments) / sizeof(experiments[0]);

    int workloadCount =
        sizeof(workloadSizes) / sizeof(workloadSizes[0]);

    ofstream file("results/cache_experiments.csv");

    if (!file.is_open())
    {
        cout << "Error: Could not create CSV file.\n";
        return 1;
    }

    file << fixed << setprecision(2);

    file << "Workload_Size,Experiment,"
         << "L1_Size,L1_Block,L1_Ways,"
         << "L2_Size,L2_Block,L2_Ways,"
         << "L1I_Hits,L1I_Misses,L1I_Hit_Rate,"
         << "L1D_Hits,L1D_Misses,L1D_Hit_Rate,"
         << "L2_Hits,L2_Misses,L2_Hit_Rate,"
         << "Main_Memory_Accesses,"
         << "Memory_Access_Reduction_Percent\n";

    for (int w = 0; w < workloadCount; w++)
    {
        int n = workloadSizes[w];
        unsigned long long baselineMemory = 0;

        cout << "\n\n######## WORKLOAD "
             << n << " x " << n << " ########\n";

        for (int i = 0; i < experimentCount; i++)
        {
            Configuration c = experiments[i];

            cout << "\nExperiment: " << c.name << '\n';

            try
            {
                CacheHierarchy cache(
                    c.l1Size, c.l1Block, c.l1Ways,
                    c.l2Size, c.l2Block, c.l2Ways
                );

                cache.runMatrixMultiplication(n);
                cache.displayStats();

                uint64_t iHits =
                    cache.getL1InstructionHits();
                uint64_t iMisses =
                    cache.getL1InstructionMisses();

                uint64_t dHits =
                    cache.getL1DataHits();
                uint64_t dMisses =
                    cache.getL1DataMisses();

                uint64_t l2Hits =
                    cache.getL2Hits();
                uint64_t l2Misses =
                    cache.getL2Misses();

                unsigned long long memory =
                    cache.getMemoryAccesses();

                if (i == 0)
                    baselineMemory = memory;

                double memoryReduction = 0.0;

                if (baselineMemory > 0)
                {
                    memoryReduction =
                        ((double)baselineMemory - memory)
                        / baselineMemory * 100.0;
                }

                file << n << ','
                     << c.name << ','
                     << c.l1Size << ','
                     << c.l1Block << ','
                     << c.l1Ways << ','
                     << c.l2Size << ','
                     << c.l2Block << ','
                     << c.l2Ways << ','
                     << iHits << ','
                     << iMisses << ','
                     << hitRate(iHits, iMisses) << ','
                     << dHits << ','
                     << dMisses << ','
                     << hitRate(dHits, dMisses) << ','
                     << l2Hits << ','
                     << l2Misses << ','
                     << hitRate(l2Hits, l2Misses) << ','
                     << memory << ','
                     << memoryReduction << '\n';
            }
            catch (const exception& e)
            {
                cout << "Experiment failed: "
                     << e.what() << '\n';
            }
        }
    }

    file.close();

    cout << "\nAll experiments completed.\n";
    cout << "Results saved to "
         << "results/cache_experiments.csv\n";

    return 0;
}