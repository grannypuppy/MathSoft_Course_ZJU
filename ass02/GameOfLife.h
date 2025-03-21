/**
 * @file GameOfLife.h
 * @brief Conway's Game of Life implementation using ncurses
 * @author Your Name
 * @date March 2025
 */

 #ifndef GAME_OF_LIFE_H
 #define GAME_OF_LIFE_H
 
 #include <ncurses.h>
 #include <vector>
 #include <string>
 #include <fstream>
 #include <ctime>
 #include <sstream>
 #include <iomanip>
 
 /**
  * @class GameOfLife
  * @brief Class implementing Conway's Game of Life simulation
  * 
  * This class provides functionality to run and visualize Conway's Game of Life
  * using the ncurses library for terminal-based visualization.
  */
 class GameOfLife {
 public:
     /**
      * @brief Constructor for the GameOfLife class
      * @param height The height of the game grid
      * @param width The width of the game grid
      */
     GameOfLife(int height, int width);
     
     /**
      * @brief Destructor for the GameOfLife class
      */
     ~GameOfLife();
     
     /**
      * @brief Initialize the game with a random pattern
      */
     void initializeRandom();
     
     /**
      * @brief Initialize the game with a specific pattern
      * @param pattern The pattern to initialize with
      */
     void initializePattern(const std::vector<std::vector<bool>>& pattern);
     
     /**
      * @brief Run the game simulation
      */
     void run();
     
     /**
      * @brief Update the game state for one generation
      */
     void update();
     
     /**
      * @brief Draw the current game state
      */
     void draw();
     
     /**
      * @brief Save the current game state as a BMP image
      * @param filename The name of the file to save to
      * @return true if the save was successful, false otherwise
      */
     bool saveAsBMP(const std::string& filename);
 
 private:
     /**
      * @brief Initialize ncurses settings
      */
     void initNCurses();
     
     /**
      * @brief Count the number of live neighbors for a cell
      * @param row The row of the cell
      * @param col The column of the cell
      * @return The number of live neighbors
      */
     int countNeighbors(int row, int col);
     
     /**
      * @brief Generate a timestamp string for filenames
      * @return A string containing the current timestamp
      */
     std::string generateTimestamp();
     
     /**
      * @brief Write BMP file header
      * @param file The file to write to
      * @param width The width of the image
      * @param height The height of the image
      */
     void writeBMPHeader(std::ofstream& file, int width, int height);
     
     /**
      * @brief Write BMP image data
      * @param file The file to write to
      */
     void writeBMPData(std::ofstream& file);
 
     int height_; ///< Height of the game grid
     int width_;  ///< Width of the game grid
     std::vector<std::vector<bool>> grid_; ///< Current state of the game grid
     std::vector<std::vector<bool>> nextGrid_; ///< Next state of the game grid
     bool running_; ///< Flag indicating if the game is running
     int generation_; ///< Current generation count
 };
 
 #endif // GAME_OF_LIFE_H
 