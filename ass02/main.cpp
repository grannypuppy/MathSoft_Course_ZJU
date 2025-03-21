/**
 * @file main.cpp
 * @brief Main entry point for Conway's Game of Life
 * @author Your Name
 * @date March 2025
 */

 #include "GameOfLife.h"
 #include <iostream>
 #include <vector>
 
 /**
  * @brief Create a glider pattern
  * @return A vector representing a glider pattern
  */
 std::vector<std::vector<bool>> createGlider() {
     std::vector<std::vector<bool>> pattern = {
         {false, true, false},
         {false, false, true},
         {true, true, true}
     };
     return pattern;
 }
 
 /**
  * @brief Create a blinker pattern
  * @return A vector representing a blinker pattern
  */
 std::vector<std::vector<bool>> createBlinker() {
     std::vector<std::vector<bool>> pattern = {
         {false, false, false},
         {true, true, true},
         {false, false, false}
     };
     return pattern;
 }
 
 /**
  * @brief Create a Gosper glider gun pattern
  * @return A vector representing a Gosper glider gun pattern
  */
 std::vector<std::vector<bool>> createGosperGliderGun() {
     std::vector<std::vector<bool>> pattern(11, std::vector<bool>(38, false));
     
     // Left block
     pattern[5][1] = true;
     pattern[6][1] = true;
     pattern[5][2] = true;
     pattern[6][2] = true;
     
     // Right block
     pattern[3][35] = true;
     pattern[4][35] = true;
     pattern[3][36] = true;
     pattern[4][36] = true;
     
     // Left ship
     pattern[3][13] = true;
     pattern[3][14] = true;
     pattern[4][12] = true;
     pattern[4][16] = true;
     pattern[5][11] = true;
     pattern[5][17] = true;
     pattern[6][11] = true;
     pattern[6][15] = true;
     pattern[6][17] = true;
     pattern[6][18] = true;
     pattern[7][11] = true;
     pattern[7][17] = true;
     pattern[8][12] = true;
     pattern[8][16] = true;
     pattern[9][13] = true;
     pattern[9][14] = true;
     
     // Right ship
     pattern[1][25] = true;
     pattern[2][23] = true;
     pattern[2][25] = true;
     pattern[3][21] = true;
     pattern[3][22] = true;
     pattern[4][21] = true;
     pattern[4][22] = true;
     pattern[5][21] = true;
     pattern[5][22] = true;
     pattern[6][23] = true;
     pattern[6][25] = true;
     pattern[7][25] = true;
     
     return pattern;
 }
 
 /**
  * @brief Display menu and get user choice
  * @return User's menu choice
  */
 int displayMenu() {
     std::cout << "Conway's Game of Life\n";
     std::cout << "====================\n";
     std::cout << "1. Random pattern\n";
     std::cout << "2. Glider pattern\n";
     std::cout << "3. Blinker pattern\n";
     std::cout << "4. Gosper glider gun pattern\n";
     std::cout << "0. Exit\n";
     std::cout << "Enter your choice: ";
     
     int choice;
     std::cin >> choice;
     return choice;
 }
 
 /**
  * @brief Main function
  * @return Exit status
  */
 int main() {
     int choice = displayMenu();
     
     if (choice == 0) {
         return 0;
     }
     
     // Create game with appropriate size
     int height = 30;
     int width = 80;
     GameOfLife game(height, width);
     
     // Initialize based on user choice
     switch (choice) {
         case 1:
             game.initializeRandom();
             break;
         case 2:
             game.initializePattern(createGlider());
             break;
         case 3:
             game.initializePattern(createBlinker());
             break;
         case 4:
             game.initializePattern(createGosperGliderGun());
             break;
         default:
             game.initializeRandom();
             break;
     }
     
     // Run the game
     game.run();
     
     return 0;
 }
 