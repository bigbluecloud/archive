#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define CURRENT_MINER_POSITION map[miners[i].x][miners[i].y]

typedef struct {
  int active; // If the miner is 'alive'
  int new; // If the mner has been newly created
  int direction; // 0, 1, 2, 3 = up, right, down, left respectively
  int x, y; // Co-ordinates in the map
  int laziness; // Counts how many times the miner has moved without mining
} _Miner;

void mineMap(_Miner* miners, char** map, const int maxMiners)
{
  int i = 0;
  for(i; i < maxMiners; ++i)
  {
    if(miners[i].active == 1) //Only for active miners
    {
      switch(miners[i].direction)
      {
        case 0: // Moving upward
          miners[i].y -= 1;
          break;
        
        case 1: // Moving right
          miners[i].x += 1;
          break;
        
        case 2: // Moving downward
          miners[i].y += 1;
          break;
        
        case 3: // Moving left
          miners[i].x -= 1; 
          break;
        
        default: // For good practice
          break;
      }
      if(CURRENT_MINER_POSITION == '#') // If the miner is on a hash, 'mine' it by setting it to a space (ASCII 32)
      {
        CURRENT_MINER_POSITION = 32;
        miners[i].laziness = 0;
      }
      else
        miners[i].laziness++; // Increase the laziness score of the miner
      
      miners[i].direction = rand() & 3; //Give the miners a new direction after moving
    }
  }
}

int killMiners(_Miner* miners, const int maxMiners, const int maxLaziness, const int mWidth, const int mHeight)
{
  int i = 0, newMinerCount = 0;
  
  for(i; i < maxMiners; ++i)
  {
    if(miners[i].active == 1) //If a miner is active
    {
      if(miners[i].x == 0 || miners[i].x == mWidth - 1 || miners[i].y == 0 || miners[i].y == mHeight - 1 || miners[i].laziness == maxLaziness) // If a miner is on the edge of the map or hasn't mined anything for a specified number of iterations, kill it
        miners[i].active = 0;
      else
        newMinerCount++; // Otherwise increment the count of how many miners are alive
    }
  }
  
  return newMinerCount;
}

int multiplyMiners(_Miner* miners, int minerCount, const int maxMiners, const int multChance)
{
  int newMinerCount = minerCount, i, j;
  
  for(i = 0; i < maxMiners && newMinerCount != maxMiners; ++i)
  {
    if(miners[i].active == 1 && miners[i].new == 0 && rand() & 100 > multChance) // If the miner is active, from the previous iteration and successfully passes its multiplication check
    {
      for(j = 0; j < maxMiners; ++j) // Find the first empty index in the miners array
      {
        if(miners[j].active == 0)
        {
          memcpy(&miners[j], &miners[i], sizeof(_Miner)); //Copy miner
          miners[j].direction = rand() & 3;
          miners[j].new = 1;
          break; //Break out of loop
        }
      }
    }
  }
  
  for(i = 0; i < maxMiners; ++i) // Set all the newly created miners to being able to multiply next iteration
  {
    if(miners[i].new == 1)
      miners[i].new = 0;
  }
  
  return newMinerCount;
}

void cleanMap(char** map, const int mapWidth, const int mapHeight)
{
  int x, y;
  for(y = 0; y < mapHeight; ++y)
  {
    for(x = 0; x < mapWidth; ++x)
    {
      int i, p; // Iterator, starting neighbour checking x-index
      int neighbourDistance; // How many indexes to traverse
      
      if(map[x][y] == 32)
        continue;
      
      if(x == 0) // At the first column in the row
      {
        p = x; // Start in line with the current x co-ordinate
        neighbourDistance = 2; // Traverse two indexes
        
        if(map[x+1][y] == '#') // If there is a neighbour to the right
          continue; // GO to the next iteration of the loop
      }
      
      else if(x == (mapWidth - 1)) // At the last column in the row
      {
        p = x - 1; // Start one behind the x co-ordinate
        neighbourDistance = 2; // Traverse two indexes
        
        if(map[x-1][y] == '#') // If there is a neighbour to the left
          continue;
      }
      
      else if(x > 0 && x < mapWidth - 1) // In between the first and last columns of the row
      {
        p = x - 1; // Start one behind the x co-ordinate
        neighbourDistance = 3; // Traverse three indexes
        
        if(map[x-1][y] == '#' || map[x+1][y] == '#') // Check for a neighbour on the left and right sides
          continue;
      }
      
      if(y != 0) // If not on the first row of the array, check row above for neighbours
      {
        for(i = 0; i < neighbourDistance; ++i)
        {
          if(map[p][y-1] == '#') // If there is a neighbour
            break; // Break out of this loop
        }
        if(i < neighbourDistance) // If a neighbour was found
          continue; // Go to the next loop iteration
      }
      
      if(y != (mapHeight - 1)) // If not on the last row of the array, check the row below for neighbours
      {
        for(i = 0; i < neighbourDistance; ++i)
        {
          if(map[p][y+1] == '#') // If there is a neighbour
            break;
        }
        if(i < neighbourDistance) // If a neighbour was found
          continue; // Go to the next loop iteration
      }
      
      map[x][y] = 32; // If the current index has no neighbours, convert it to a space
    }
  }
}

void printMapToFile(char** map, const int mapWidth, const int mapHeight)
{
  FILE* mapFile = fopen("testmap.txt", "w"); // File pointer
  int x, y; // Iterators
  for(y = 0; y < mapHeight; ++y) // Traverse map in expected way rather than rotated by 90 degrees (most noticeable when using non-sqaure map size
  {
    for(x = 0; x < mapWidth; ++x)
    {
      fputc(map[x][y], mapFile); // Put the characters into the file
    }
    fputc('\n', mapFile);
  }
    
  fflush(mapFile);
  fclose(mapFile);
}


int main(int argc, char** argv)
{
  const int mapWidth = 100; // Map width and height in array entries
  const int mapHeight = 50;
  const int maxMiners = 100; // Number of miners allowed
  const int multChance = 85; // Chance that a miner will multiply ( > rand() & 100)
  const int maxLaziness = 3; // Maximum number of iterations a miner is not allowed to do anything before being killed
  
  char** map; // 2D array that stores ASCII map
  int i, j, minerCount = 1; // Iterator, number of active miners - intialised to 1
  _Miner* miners;
  
  map = calloc(mapWidth, sizeof(char*)); // Dynamic allocation of memory for map
  for(i = 0; i < mapWidth; ++i)
    map[i] = calloc(mapHeight, sizeof(char));
  
  for(i = 0; i < mapWidth; ++i) // Initialising all map characters to #
    for(j = 0; j < mapHeight; ++j)
      map[i][j] = '#';
  
  miners = calloc(maxMiners, sizeof(_Miner)); // Allocation of memory for miner array
  
  miners[0] = (_Miner){1, 0, rand() & 3, mapWidth/2, mapHeight/2}; // Initialisation of first miner at the centre of the map moving in a random direction
  
  if(argc > 1) // Seed random if a command line argument was given
    srand(atoi(argv[1]));
  else
    srand(time(NULL));
  
  while(minerCount > 0) // Procede to mine the map while there are miners alive
  {
    mineMap(miners, map, maxMiners); // Move miners and destroy blocks
    minerCount = killMiners(miners, maxMiners, maxLaziness, mapWidth, mapHeight); // Kill lazy miners and miners on the edge of the map
    if(minerCount < maxMiners)
      minerCount = multiplyMiners(miners, minerCount, maxMiners, multChance); // If there are less miners than the max, attempt to create more miners
  }
  
  cleanMap(map, mapWidth, mapHeight);
  printMapToFile(map, mapWidth, mapHeight);
  
  for(i = 0; i < mapWidth; ++i) // Freeing map memory
    free(map[i]);
  free(map);
  
  free(miners); // Freeing miner memory
  
  return 0;
}