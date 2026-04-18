/*
    Author: Reagan Sanz
    Date: 4/17/26
    Description: 
*/

#include <iostream>
#include <ctime>
#include <fstream>
#include "biomes.h"

using namespace std;

const int MAP_WIDTH = 100;
const int MAP_HEIGHT = 65;

int ALIVE = 1;
int DEAD = 0;
int the_map[MAP_HEIGHT][MAP_WIDTH];

// Change as needed
const string OUTPUT_FILE_NAME = "./output/cave.txt";
const int ITERATIONS = 5;


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
void showCave(){
    ofstream caveFile(OUTPUT_FILE_NAME);
    if(caveFile.is_open()){
        for(int y = 0; y < MAP_HEIGHT; y++){
            for(int x = 0; x < MAP_WIDTH; x++){
                if(the_map[y][x] == ALIVE){
                    caveFile << "#"; // Represents a wall
                } else {
                    caveFile << "."; // Represents open space of the cave
                }
            }
            caveFile << endl;
        }
        caveFile.close();
        cout << "Cave saved to " << OUTPUT_FILE_NAME << endl;
    } 
    else {
        cout << "Unable to open file to write cave." << endl;
    }

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
                int aliveNeighbors = neighborCalc(x, y);

                if(the_map[y][x] == ALIVE) {
                    // If it's alive, it stays alive unless it has too few neighbors
                    temp_map[y][x] = (aliveNeighbors < selectedBiome.aliveNeighborThreshold) ? DEAD : ALIVE;
                } else {
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
    

    // Save the generated cave to a file
    showCave();
    return 0;
}




/*
  References:
  - Paper on cave generation: https://dl.acm.org/doi/10.1145/1814256.1814266
  - https://www.youtube.com/watch?v=S9QTsf9AUqQ
*/