#ifndef BIOMES_H
#define BIOMES_H

#include <string>
#include <map>
using namespace std;

// Structure of biome data
struct Biome {
    string name;
    int aliveProbability;
    int aliveNeighborThreshold;
    int deadNeighborThreshold;
};

extern const map<string, Biome> biome_registry;

#endif

