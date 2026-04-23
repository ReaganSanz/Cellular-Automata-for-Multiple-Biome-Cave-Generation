/*
    Author: Reagan Sanz
    Date: 4/17/26
    Description: This program uses cellular automata to generate caves based on biome parameters. 
*/

#include <iostream>
#include <ctime>
#include <fstream>
#include "biomes.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

// The Map
const int MAP_WIDTH = 65;
const int MAP_HEIGHT = 100;
int the_map[MAP_HEIGHT][MAP_WIDTH];

// Constants for different states of each pixel on map
const int DEAD = 0;
const int ALIVE = 1;
const int ENEMY = 2;
const int WATER = 3;
const int TREASUREWALL = 4;
const int TREASURE = 5;
const int TREASUREINTERIOR = 6;

// Change as needed, parameters for generating
const string OUTPUT_FILE_NAME = "./output/cave.png";
const int ITERATIONS = 5;
const int MIN_CAVE_NEIGHBORS_FOR_ENEMY = 5;

// Functions
void generateCave(int aliveProbability);
void cellularAutomataForCaves(const Biome& selectedBiome);
void showCave(const Biome& cur_biome);
void spawnEnemies(int monsterChance);
void cellularAutomataForWater(const Biome& b);
int neighborCalc(int x, int y, int type);
void generateTreasureRoom(const Biome& cur_biome);



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

    // Cellular Automata iterations for caves
    cellularAutomataForCaves(selectedBiome);

    // Cellular Automata for Water 
    cellularAutomataForWater(selectedBiome);

    // Spawn enemies based on biome parameters
    spawnEnemies(selectedBiome.monster); 

    // generate a single treasure room per biome
    generateTreasureRoom(selectedBiome);

    // Save the generated cave to a file
    showCave(selectedBiome);
    return 0;
}

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

// Looks at the neighbors surrounding the current cell to determine its state
int neighborCalc(int x, int y, int type) {
    int count = 0;
    for (int h = y - 1; h <= y + 1; h++) {
        for (int w = x - 1; w <= x + 1; w++) {
            if (h >= 0 && h < MAP_HEIGHT && w >= 0 && w < MAP_WIDTH) {
                if (!(w == x && h == y)) { // skip center cell,
                    // Count if it matches our current type (ALIVE, DEAD, WATER, or ENEMY)
                    if (the_map[h][w] == type) {
                        count++;
                    }
                }
            } 
            // For Walls/edges
            else if (type == ALIVE) {
                count++;
            }
        }
    }
    return count;
}


/* Uses Cellular Automata rules to generate unique caves for each biome*/
void cellularAutomataForCaves(const Biome& selectedBiome){
    for (int i = 0; i < ITERATIONS; i++){ // iterate through to smooth out the cave
        int temp_map[MAP_HEIGHT][MAP_WIDTH];
        for(int y = 0; y < MAP_HEIGHT; y++){
            for(int x = 0; x < MAP_WIDTH; x++){
                int aliveNeighbors = neighborCalc(x, y, ALIVE); // Get the number of alive neighbors

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
}


/* cellularAutomataForWater: generates water using new automata rules*/
void cellularAutomataForWater(const Biome& b) {
    // Water only occurs in DEAD (cave) spaces
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            if(the_map[y][x] == DEAD) {
                if(rand() % 100 < b.water) { // chance to spawn water
                    the_map[y][x] = WATER;
                }
            }
        }
    }

    // Run Cellular Automata for water to create pockets of water
    for(int i = 0; i < ITERATIONS; i++) { 
        int temp_map[MAP_HEIGHT][MAP_WIDTH];  
        // Copy temp map
        for(int y = 0; y < MAP_HEIGHT; y++) 
            for(int x = 0; x < MAP_WIDTH; x++) 
                temp_map[y][x] = the_map[y][x];

        for(int y = 0; y < MAP_HEIGHT; y++) {
            for(int x = 0; x < MAP_WIDTH; x++) {
                // Look at non-wall cells for generating water
                if(the_map[y][x] != ALIVE) {
                    int waterNeighbors = neighborCalc(x, y, WATER);
                    
                    if(waterNeighbors > 4) temp_map[y][x] = WATER;
                    else if(waterNeighbors < 3) temp_map[y][x] = DEAD;
                }
            }
        }
        // Copy back to the_map and save
        for(int y = 0; y < MAP_HEIGHT; y++)
            for(int x = 0; x < MAP_WIDTH; x++)
                the_map[y][x] = temp_map[y][x];
    }
}


/* spawnEnemies: spawns enemies in the cave based on biome parameters */
void spawnEnemies(int monsterChance){
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
        
            // Enemies spawn in cave spots (DEAD)
            if(the_map[y][x] == DEAD) {
                int caveNeighbors = neighborCalc(x, y, DEAD);
                //int caveNeighbors = 8 - wallNeighbors; // Total cave neighbors

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
            // If water, set to blue
            if(the_map[y][x] == WATER) {
                image[index] = 0;       // R
                image[index + 1] = 100; // G
                image[index + 2] = 200; // B 
            }
            // If treasure wall, set to medium gray
            if(the_map[y][x] == TREASUREWALL) {
                image[index] = 200;     // R
                image[index + 1] = 200; // G
                image[index + 2] = 200; // B   
            }
            // If treasure interior, set to light gray
            if(the_map[y][x] == TREASUREINTERIOR) {
                image[index] = 220;     // R
                image[index + 1] = 220; // G
                image[index + 2] = 220; // B   
            }
            // If treasure, set to yellow
            if(the_map[y][x] == TREASURE) {
                image[index] = 255;     // R
                image[index + 1] = 255; // G
                image[index + 2] = 0;   // B   
            }
        }
    }

    // Write with stbi_write_png
    stbi_write_png(OUTPUT_FILE_NAME.c_str(), MAP_WIDTH, MAP_HEIGHT, 3, image, MAP_WIDTH * 3);
    cout << "Cave saved to " << OUTPUT_FILE_NAME << endl;

}


/* generateTreasureRoom: Generates a single "treasure" room per biome with low chance*/
void generateTreasureRoom(const Biome& cur_biome){
    // Generate rand number for chance of treasure room
    int num = (rand() % 100) + 1;
    if(num > cur_biome.treasure){
        return; // 80% chance that treasure room does not appear
    }
    else{
        // Generate random position in MAP_HEIGHT and MAP_WIDTH
        int randHeight = (rand() % (MAP_HEIGHT-5));
        int randWidth= (rand() % (MAP_WIDTH-5));

        // Set the cave walls/pixels of the treasure room
        for(int y = randHeight; y < randHeight + 7; y++){
            for(int x = randWidth; x < randWidth + 7; x++){
                // Edges of room
                if(y == randHeight || y == randHeight + 6 || x == randWidth || x == randWidth + 6){
                    the_map[y][x] = TREASUREWALL;
                }
                // Interior of room
                else if(y > randHeight && y < randHeight + 6 && x > randWidth && x < randWidth + 6){
                    the_map[y][x] = TREASUREINTERIOR;
                }
                // Center of room w treasure
                if(y == randHeight + 3 && x == randWidth + 3){
                    the_map[y][x] = TREASURE;
                }
            }
        }
    }
}


/*
  References:
  - Paper on cave generation: https://dl.acm.org/doi/10.1145/1814256.1814266
  - Header file from Github for creating PNGs of caves: https://github.com/nothings/stb/blob/master/stb_image_write.h
  - Youtube Video on Cave Generation:https://www.youtube.com/watch?v=S9QTsf9AUqQ
*/