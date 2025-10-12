# ------------------------------------------------------------
#  Detect the operating system once, early in the file
# ------------------------------------------------------------
UNAME_S := $(shell uname -s)

# ------------------------------------------------------------
#  Source files
# ------------------------------------------------------------
SRCS = main.cpp $(wildcard CS3113/*.cpp)

# ------------------------------------------------------------
#  Target name
# ------------------------------------------------------------
TARGET = raylib_app

# ------------------------------------------------------------
#  Compiler / basic flags
# ------------------------------------------------------------
CXX      = g++
CXXFLAGS = -std=c++11

# ------------------------------------------------------------
#  Raylib configuration
# ------------------------------------------------------------
# If pkg-config fails, manually set include/lib paths for Homebrew
RAYLIB_INC   = /opt/homebrew/include
RAYLIB_LIB   = /opt/homebrew/lib
RAYLIB_CFLAGS = -I$(RAYLIB_INC)
RAYLIB_LIBS   = -L$(RAYLIB_LIB) -lraylib

# ------------------------------------------------------------
#  Platform-specific settings
# ------------------------------------------------------------
ifeq ($(UNAME_S),Darwin)
    # macOS (Apple Silicon defaults to arm64)
    CXXFLAGS += -arch arm64 $(RAYLIB_CFLAGS)
    LIBS = $(RAYLIB_LIBS) \
           -framework OpenGL \
           -framework Cocoa \
           -framework IOKit \
           -framework CoreVideo \
           -framework AudioToolbox \
           -framework CoreAudio

    EXEC = ./$(TARGET)

else ifneq (,$(findstring MINGW,$(UNAME_S)))   
    # Windows
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = ./$(TARGET)

else
    # Linux
    CXXFLAGS += $(RAYLIB_CFLAGS)
    LIBS = $(RAYLIB_LIBS) -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = ./$(TARGET)
endif

# ------------------------------------------------------------
#  Build rule
# ------------------------------------------------------------
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LIBS)

# ------------------------------------------------------------
#  Convenience targets
# ------------------------------------------------------------
.PHONY: clean run

clean:
	@rm -f $(TARGET) $(TARGET).exe

run: $(TARGET)
	$(EXEC)
