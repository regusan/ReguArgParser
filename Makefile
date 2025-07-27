CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -pthread

INC_DIR = include
CXXFLAGS += -I$(INC_DIR)

BUILD_DIR = build
LDFLAGS = -L$(BUILD_DIR)

TEST_SRC = test/test.cpp
TEST_BIN = $(BUILD_DIR)/test_runner

.PHONY: all test clean
all: test

dirs:
	@mkdir -p $(BUILD_DIR)

test:dirs $(TEST_BIN)

$(TEST_BIN): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_BIN) $(LDFLAGS)

clean:
	rm -f $(TEST_BIN)