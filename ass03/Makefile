CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include
LDFLAGS = 

# 检查是否使用GMP库
ifdef USE_GMP
CXXFLAGS += -DUSE_GMP
LDFLAGS += -lgmpxx -lgmp
endif

# 目标文件
TARGET = test
SRC_DIR = src
SRCS = $(SRC_DIR)/test.cpp
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 编译目标
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 编译规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# 使用GMP库编译
gmp: USE_GMP=1
gmp: clean all

# 清理
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean gmp
