#ifndef BIOMES_H
#define BIOMES_H

#include <string>
#include <map>
using namespace std;

// Colors for cave walls/floors
struct Color {
    unsigned char r, g, b;
};

// Structure of biome data
struct Biome {
    // Biome name
    string name;
    // probabilities of living/dead and cellular automata thresholds
    int aliveProbability; 
    int aliveNeighborThreshold;
    int deadNeighborThreshold;
    // Colors for walls and floors
    Color wallColor;
    Color floorColor;
    // Chances of spawning different features
    int water;
    int ore;
    int monster;
    int treasure;

};

extern const map<string, Biome> biome_registry;

#endif

