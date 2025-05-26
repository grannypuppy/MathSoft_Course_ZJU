# Makefile for gsl_test.c
CC = gcc
CFLAGS = -Wall -O2
GSL_PREFIX = $(HOME)/usr/gsl
INCLUDES = -I$(GSL_PREFIX)/include
LIBS = -L$(GSL_PREFIX)/lib -lgsl -lgslcblas -lm

# 目标文件
TARGET = gsl_test
SOURCE = gsl_test.c

# 编译规则
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCE) $(LIBS)

# 清理规则
clean:
	rm -f $(TARGET)

# 运行规则
run: $(TARGET)
	LD_LIBRARY_PATH=$(GSL_PREFIX)/lib ./$(TARGET)

.PHONY: clean run