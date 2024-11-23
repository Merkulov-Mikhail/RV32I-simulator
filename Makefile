
ifeq ($(origin CC),default)
	CC = g++
endif

BUILD_DIR ?= build
SRC_DIR = ./src
INC_DIR = ./include
TES_DIR = ./tests
TESTS   = $(wildcard $(TES_DIR)/*.txt)
CPP_SRC = simulator.cpp tests.cpp
CPP_INC = $(INC_DIR)/simulator.h
CPP_OBJ = $(addprefix $(BUILD_DIR)/,$(CPP_SRC:.cpp=.o))
TESTS_EXECUTABLE ?= some_complex_name_for_tests_big_enough.exe
override CFLAGS += -I$(INC_DIR)

.PHONY: all
all: main.exe


main.exe: $(CPP_OBJ)
	$(CC) $^ -o $@

$(CPP_OBJ): $(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp $(CPP_INC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



FORCE ?=
.PHONY: _tests
_tests:  $(SRC_DIR)/tests.cpp $(SRC_DIR)/simulator.cpp $(CPP_INC)
	@$(CC) $(CFLAGS) $^ -o ./$(TESTS_EXECUTABLE)
	@./$(TESTS_EXECUTABLE)
	@./$(TESTS_EXECUTABLE) $(FORCE) $(TESTS)
	@rm -rf ./$(TESTS_EXECUTABLE)
