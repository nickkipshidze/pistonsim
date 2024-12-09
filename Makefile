PROJECT_NAME := pistonsim

SRC_DIR := src
BUILD_DIR := build
INC_DIR := include

CC := cc
CFLAGS := -O2
LDFLAGS := -l raylib -fsanitize=address

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

INCLUDES := -I $(INC_DIR)

EXEC := $(BUILD_DIR)/$(PROJECT_NAME)

all: $(EXEC)

$(EXEC): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)
	@echo "Build complete: $(EXEC)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
