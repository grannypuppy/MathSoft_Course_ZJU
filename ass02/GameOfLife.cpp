#include "GameOfLife.h"
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <iostream>

GameOfLife::GameOfLife(int height, int width) 
    : height_(height), width_(width), running_(true), generation_(0) {
    // Initialize grid with all cells dead
    grid_.resize(height_, std::vector<bool>(width_, false));
    nextGrid_.resize(height_, std::vector<bool>(width_, false));
    
    // Initialize ncurses
    initNCurses();
    
    // Seed random number generator
    std::srand(std::time(nullptr));
}

GameOfLife::~GameOfLife() {
    // End ncurses mode
    endwin();
}

void GameOfLife::initNCurses() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);  // Hide cursor
    timeout(100); // Set getch() non-blocking with 100ms timeout
    
    // Check if terminal supports colors
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_GREEN);  // Live cells
        init_pair(2, COLOR_BLACK, COLOR_BLACK);  // Dead cells
    }
}

void GameOfLife::initializeRandom() {
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            // 25% chance of a cell being alive
            grid_[i][j] = (std::rand() % 4 == 0);
        }
    }
}

void GameOfLife::initializePattern(const std::vector<std::vector<bool>>& pattern) {
    int patternHeight = pattern.size();
    int patternWidth = pattern[0].size();
    
    // Calculate starting position to center the pattern
    int startRow = (height_ - patternHeight) / 2;
    int startCol = (width_ - patternWidth) / 2;
    
    // Clear the grid first
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            grid_[i][j] = false;
        }
    }
    
    // Place the pattern in the center
    for (int i = 0; i < patternHeight; i++) {
        for (int j = 0; j < patternWidth; j++) {
            int row = startRow + i;
            int col = startCol + j;
            if (row >= 0 && row < height_ && col >= 0 && col < width_) {
                grid_[row][col] = pattern[i][j];
            }
        }
    }
}

void GameOfLife::run() {
    while (running_) {
        // Clear screen
        clear();
        
        // Draw the current state
        draw();
        
        // Display generation count
        mvprintw(height_ + 1, 0, "Generation: %d", generation_);
        mvprintw(height_ + 2, 0, "Press 'q' to quit, 's' to save image, 'r' to randomize");
        
        // Process input
        int ch = getch();
        switch (ch) {
            case 'q':
            case 'Q':
                running_ = false;
                break;
            case 's':
            case 'S':
                {
                    std::string filename = "gameoflife_" + generateTimestamp() + ".bmp";
                    if (saveAsBMP(filename)) {
                        mvprintw(height_ + 3, 0, "Image saved as %s", filename.c_str());
                        refresh();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    } else {
                        mvprintw(height_ + 3, 0, "Failed to save image");
                        refresh();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }
                }
                break;
            case 'r':
            case 'R':
                initializeRandom();
                generation_ = 0;
                break;
        }
        
        // Update the game state
        update();
        generation_++;
        
        // Small delay to control game speed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void GameOfLife::update() {
    // Calculate the next generation
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            int neighbors = countNeighbors(i, j);
            
            // Apply Conway's Game of Life rules
            if (grid_[i][j]) {
                // Cell is alive
                if (neighbors < 2 || neighbors > 3) {
                    nextGrid_[i][j] = false; // Cell dies
                } else {
                    nextGrid_[i][j] = true;  // Cell survives
                }
            } else {
                // Cell is dead
                if (neighbors == 3) {
                    nextGrid_[i][j] = true;  // Cell becomes alive
                } else {
                    nextGrid_[i][j] = false; // Cell stays dead
                }
            }
        }
    }
    
    // Update the grid with the new generation
    grid_ = nextGrid_;
}

int GameOfLife::countNeighbors(int row, int col) {
    int count = 0;
    
    // Check all 8 neighboring cells
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // Skip the cell itself
            if (i == 0 && j == 0) continue;
            
            // Calculate neighbor coordinates with wrapping
            int neighborRow = (row + i + height_) % height_;
            int neighborCol = (col + j + width_) % width_;
            
            // Count live neighbors
            if (grid_[neighborRow][neighborCol]) {
                count++;
            }
        }
    }
    
    return count;
}

void GameOfLife::draw() {
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            if (grid_[i][j]) {
                // Cell is alive
                if (has_colors()) {
                    attron(COLOR_PAIR(1));
                    mvaddch(i, j, ' ');
                    attroff(COLOR_PAIR(1));
                } else {
                    mvaddch(i, j, '#');
                }
            } else {
                // Cell is dead
                if (has_colors()) {
                    attron(COLOR_PAIR(2));
                    mvaddch(i, j, ' ');
                    attroff(COLOR_PAIR(2));
                } else {
                    mvaddch(i, j, '.');
                }
            }
        }
    }
    
    // Update the screen
    refresh();
}

std::string GameOfLife::generateTimestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

bool GameOfLife::saveAsBMP(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }
    
    // Write BMP header
    writeBMPHeader(file, width_, height_);
    
    // Write BMP data
    writeBMPData(file);
    
    file.close();
    return true;
}

void GameOfLife::writeBMPHeader(std::ofstream& file, int width, int height) {
    // Calculate row size and padding
    int rowSize = ((width * 24 + 31) / 32) * 4;
    int paddingSize = rowSize - width * 3;
    int fileSize = 54 + rowSize * height;
    
    // BMP file header (14 bytes)
    unsigned char bmpFileHeader[14] = {
        'B', 'M',                       // Signature
        0, 0, 0, 0,                     // File size in bytes
        0, 0, 0, 0,                     // Reserved
        54, 0, 0, 0                     // Offset to start of pixel data
    };
    
    // Update file size in header
    bmpFileHeader[2] = (unsigned char)(fileSize);
    bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
    bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
    bmpFileHeader[5] = (unsigned char)(fileSize >> 24);
    
    // BMP info header (40 bytes)
    unsigned char bmpInfoHeader[40] = {
        40, 0, 0, 0,                    // Info header size
        0, 0, 0, 0,                     // Width
        0, 0, 0, 0,                     // Height
        1, 0,                           // Number of color planes
        24, 0,                          // Bits per pixel
        0, 0, 0, 0,                     // Compression
        0, 0, 0, 0,                     // Image size
        0, 0, 0, 0,                     // X pixels per meter
        0, 0, 0, 0,                     // Y pixels per meter
        0, 0, 0, 0,                     // Colors in color table
        0, 0, 0, 0                      // Important color count
    };
    
    // Update width in header
    bmpInfoHeader[4] = (unsigned char)(width);
    bmpInfoHeader[5] = (unsigned char)(width >> 8);
    bmpInfoHeader[6] = (unsigned char)(width >> 16);
    bmpInfoHeader[7] = (unsigned char)(width >> 24);
    
    // Update height in header
    bmpInfoHeader[8] = (unsigned char)(height);
    bmpInfoHeader[9] = (unsigned char)(height >> 8);
    bmpInfoHeader[10] = (unsigned char)(height >> 16);
    bmpInfoHeader[11] = (unsigned char)(height >> 24);
    
    // Write headers
    file.write(reinterpret_cast<char*>(bmpFileHeader), 14);
    file.write(reinterpret_cast<char*>(bmpInfoHeader), 40);
}

void GameOfLife::writeBMPData(std::ofstream& file) {
    // Calculate row size and padding
    int rowSize = ((width_ * 24 + 31) / 32) * 4;
    int paddingSize = rowSize - width_ * 3;
    unsigned char padding[3] = {0, 0, 0};
    
    // BMP stores images bottom-up
    for (int i = height_ - 1; i >= 0; i--) {
        for (int j = 0; j < width_; j++) {
            unsigned char color[3];
            
            // Set pixel color (BGR format)
            if (grid_[i][j]) {
                // Live cell - green
                color[0] = 0;    // Blue
                color[1] = 255;  // Green
                color[2] = 0;    // Red
            } else {
                // Dead cell - black
                color[0] = 0;    // Blue
                color[1] = 0;    // Green
                color[2] = 0;    // Red
            }
            
            // Write pixel data
            file.write(reinterpret_cast<char*>(color), 3);
        }
        
        // Write padding
        if (paddingSize > 0) {
            file.write(reinterpret_cast<char*>(padding), paddingSize);
        }
    }
}
