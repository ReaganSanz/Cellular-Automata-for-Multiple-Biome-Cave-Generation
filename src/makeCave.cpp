/*
    Author: Reagan Sanz
    Date: 4/17/26
    Description: 
*/

#include <iostream>
#include <ctime>
#include <fstream>
#include "biomes.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

const int MAP_WIDTH = 65;
const int MAP_HEIGHT = 100;

int ALIVE = 1;
int DEAD = 0;
int ENEMY = 2;
int the_map[MAP_HEIGHT][MAP_WIDTH];

// Change as needed
const string OUTPUT_FILE_NAME = "./output/cave.png";
const int ITERATIONS = 5;
const int MIN_CAVE_NEIGHBORS_FOR_ENEMY = 5;


/* generateCave: generates a cave based on biome probabilities*/
void generateCave(int aliveProbability){
    srand(time(0));
    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            if(rand() % 100 < aliveProbability){
                the_map[y][x] = ALIVE;
            } else {
                the_map[y][x] = DEAD;
            }
        }
    }
}

/* showCave: saves the generated cave */
void showCave(const Biome& cur_biome){
    unsigned char image[MAP_HEIGHT * MAP_WIDTH * 3]; // RGB channels = 3

    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            int index = (y * MAP_WIDTH + x) * 3;
            // If alive, set to walls
            if(the_map[y][x] == ALIVE){
                image[index] = cur_biome.wallColor.r;     // R
                image[index + 1] = cur_biome.wallColor.g; // G
                image[index + 2] = cur_biome.wallColor.b; // B
            }
            // If dead, set to open cave space 
            else{
                image[index] = cur_biome.floorColor.r;     // R
                image[index + 1] = cur_biome.floorColor.g; // G
                image[index + 2] = cur_biome.floorColor.b; // B 
            }
            // If enemy, set to red
            if(the_map[y][x] == ENEMY){
                image[index] = 255;     // R
                image[index + 1] = 0;   // G
                image[index + 2] = 0;   // B 
            }
        }
    }
    // Write with stbi_write_png
    stbi_write_png(OUTPUT_FILE_NAME.c_str(), MAP_WIDTH, MAP_HEIGHT, 3, image, MAP_WIDTH * 3);
    cout << "Cave saved to " << OUTPUT_FILE_NAME << endl;

}

// Implements automata rules to evolve the cave over iterations
// Looks at the neighbors surrounding the current cell to determine its state
int neighborCalc(int x, int y){
    int numNeighbors = 0;
    for(int h = y-1; h <= y+1 ; h++){
        for(int w = x-1; w <= x+1; w++){
            if(h >= 0 && h < MAP_HEIGHT && w >= 0 && w < MAP_WIDTH){
                if(!(w == x && h == y)){ // skip the current cell
                    numNeighbors += the_map[h][w]; // add to total count
                }
            }
            else{
                numNeighbors++; // Edges of total map are walls
            }
        }
    }
    return numNeighbors;
}

/* spawnEnemies: spawns enemies in the cave based on biome parameters */
void spawnEnemies(int monsterChance){
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
        
            // Enemies spawn in cave spots (DEAD)
            if(the_map[y][x] == DEAD) {
                int wallNeighbors = neighborCalc(x, y);
                int caveNeighbors = 8 - wallNeighbors; // Total cave neighbors

                // Enemies spawn in cave spots of at least 5 cave neighbors
                if(caveNeighbors >= MIN_CAVE_NEIGHBORS_FOR_ENEMY) {
                    double randValue = rand() % 100; // Random value between 0 and 99
                    if(randValue < monsterChance) {
                        the_map[y][x] = ENEMY;
                    }
                }
            }
        }
    }
}

int main(){
    int biomeChoice;
    Biome selectedBiome;
    // Prompt user for biome selection
    cout << "Please select a biome: " << endl;
    cout << "1. Forest" << endl;
    cout << "2. Desert" << endl;
    cout << "3. Underwater" << endl;
    cout << "4. Mountain" << endl;
    cout << "Enter Choice: ";
    cin >> biomeChoice;
    if( biomeChoice < 1 || biomeChoice > 4){
        cout << "Invalid Biome choice" << endl;
        return 1;
    }
    else if(biomeChoice == 1){
        selectedBiome = biome_registry.at("Forest");
    }
    else if(biomeChoice == 2){
        selectedBiome = biome_registry.at("Desert");
    }
    else if(biomeChoice == 3){
        selectedBiome = biome_registry.at("Underwater");
    }
    else if(biomeChoice == 4){
        selectedBiome = biome_registry.at("Mountain");
    }

    // Generate initial cave
    generateCave(selectedBiome.aliveProbability);

    // Cellular Automata iterations
    for (int i = 0; i < ITERATIONS; i++){ // iterate through to smooth out the cave
        int temp_map[MAP_HEIGHT][MAP_WIDTH];
        for(int y = 0; y < MAP_HEIGHT; y++){
            for(int x = 0; x < MAP_WIDTH; x++){
                int aliveNeighbors = neighborCalc(x, y); // Get the number of alive neighbors

                // Alive = WALL
                if(the_map[y][x] == ALIVE) {
                    // If it's alive, it stays alive unless it has too few neighbors
                    temp_map[y][x] = (aliveNeighbors < selectedBiome.aliveNeighborThreshold) ? DEAD : ALIVE;
                } 
                // Dead = CAVE
                else {
                    // If it's dead, it becomes alive if it has enough neighbors
                    temp_map[y][x] = (aliveNeighbors > selectedBiome.aliveNeighborThreshold) ? ALIVE : DEAD;
                }
            }
        }
        // Copy temp_map back into the_map
        for(int y = 0; y < MAP_HEIGHT; y++){
            for(int x = 0; x < MAP_WIDTH; x++){
                the_map[y][x] = temp_map[y][x]; // Update the map for the next iteration
            }
        }
    }

    // Spawn enemies based on biome parameters
    spawnEnemies(selectedBiome.monster); 

    // Spawn ores
    spawnOres(selectedBiome.ore);

    

    // Save the generated cave to a file
    showCave(selectedBiome);
    return 0;
}


/*
  References:
  - Paper on cave generation: https://dl.acm.org/doi/10.1145/1814256.1814266
  - Header file from Github for creating PNGs of caves: https://github.com/nothings/stb/blob/master/stb_image_write.h
  - Youtube Video on Cave Generation:https://www.youtube.com/watch?v=S9QTsf9AUqQ
*/