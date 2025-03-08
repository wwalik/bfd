TARGET := bf

INCLUDE_DIR := include
BUILD_DIR := build

CC := gcc
CFLAGS := --std=c99 -Wall -iquote$(INCLUDE_DIR) -MMD -g

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $^

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm $(TARGET) ;\
	rm $(BUILD_DIR)/*

-include $(BUILD_DIR)/*.d
