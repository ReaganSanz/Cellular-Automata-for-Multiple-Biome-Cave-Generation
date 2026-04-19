#include "biomes.h"
using namespace std;

// List of biomes and their unique parameters for cave generation.
const map<string, Biome> biome_registry = {
    {"Forest", {"Forest", 53, 4, 3, {34, 139, 34}, {20, 20, 25}, 0, 3, 5, 1}},
    {"Desert", {"Desert", 65, 4, 3, {237, 201, 175},{20, 20, 25}, 0, 3, 5, 10}},
    {"Underwater", {"Underwater", 50, 4, 3,{0, 105, 148},{20, 52, 79}, 100, 3, 0, 2}},
    {"Mountain", {"Mountain", 40, 4, 2, {105, 105, 105},{20, 20, 25}, 0, 7, 5, 1}}
};