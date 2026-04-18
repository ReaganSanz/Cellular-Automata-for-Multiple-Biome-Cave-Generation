#include "biomes.h"
using namespace std;

// List of biomes and their unique parameters for cave generation.
const map<string, Biome> biome_registry = {
    {"Forest", {"Forest", 53, 4, 3}},
    {"Desert", {"Desert", 65, 4, 3}},
    {"Underwater", {"Underwater", 50, 4, 3}},
    {"Mountain", {"Mountain", 40, 4, 2}}
};