# Makefile for Conway's Game of Life

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lncurses

# Source files and object files
SOURCES = main.cpp GameOfLife.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = game_of_life

# Doxygen configuration file
DOXYFILE = Doxyfile

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compiling source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate documentation using Doxygen
doc:
	doxygen $(DOXYFILE)
	@echo "Cleaning up intermediate LaTeX files..."
	@if [ -d latex ]; then \
		cd latex && $(MAKE) && \
		cp refman.pdf ../GameOfLife_documentation.pdf && \
		cd .. && \
		find latex -type f -not -name "*.tex" -not -name "*.pdf" -delete; \
	fi
	@echo "Documentation generated successfully."
	@echo "HTML documentation in 'html' directory"
	@echo "LaTeX files in 'latex' directory"
	@echo "PDF documentation: GameOfLife_documentation.pdf"

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Clean documentation
clean-doc:
	rm -rf html latex GameOfLife_documentation.pdf

# Clean everything
distclean: clean clean-doc

# Install
install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/local/bin
	cp $(TARGET) $(DESTDIR)/usr/local/bin/

# Uninstall
uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(TARGET)

# Phony targets
.PHONY: all doc clean clean-doc distclean install uninstall
