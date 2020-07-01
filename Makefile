CC = g++
LD = g++

CCFLAGS=-g -O2 -pthread `pkg-config --cflags --libs glfw3 gl glu glew OpenCL`
LDFLAGS=-g -O2 -pthread `pkg-config --cflags --libs glfw3 gl glu glew OpenCL`
TARGET=main

MODULES   := src src/Render src/Program
SRC_DIR   := $(addprefix ./,$(MODULES)) .
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst ./%.cpp,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(CCFLAGS) $(INCLUDES) -c $$< -o $$@
endef

.PHONY: all checkdirs clean run debug

all: checkdirs build/$(TARGET)

# Build C++ objects
build/$(TARGET): $(OBJ)
	# @echo "Compiling $(SRC)"
	$(LD) $^ $(LDFLAGS) -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

run:
	./build/main

debug:
	valgrind ./build/main

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))