# Source Files
SRCS = main.cpp \
       CS3113/Entity.cpp \
       CS3113/Map.cpp \
       CS3113/cs3113.cpp

# Target
TARGET = raylib_app

# OS detection
UNAME_S := $(shell uname -s)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11

ifeq ($(UNAME_S), Darwin)
    # macOS configuration
    CXXFLAGS += -arch arm64 -I/usr/local/include -I/opt/homebrew/include
    LIBS = -L/opt/homebrew/lib -lraylib \
           -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    EXEC = ./$(TARGET)
else ifneq (,$(findstring MINGW,$(UNAME_S)))
    # Windows configuration
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = $(TARGET)
else
    # Linux fallback
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = ./$(TARGET)
endif

# Build rule
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)
	@mkdir -p ./assets
	@cp -R assets ./ 2>/dev/null || true

# Clean rule
clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TARGET).exe" ]; then rm -f $(TARGET).exe; fi

# Run rule
run: $(TARGET)
	$(EXEC)
