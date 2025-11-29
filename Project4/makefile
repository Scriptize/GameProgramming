# ------------------------------------------------------------
#  Detect the operating system once, early in the file
# ------------------------------------------------------------
UNAME_S := $(shell uname -s)

# ------------------------------------------------------------
#  Source files
# ------------------------------------------------------------
SRCS = main.cpp CS3113/*.cpp

# ------------------------------------------------------------
#  Target name
# ------------------------------------------------------------
TARGET = raylib_app

# ------------------------------------------------------------
#  Compiler / basic flags
# ------------------------------------------------------------
CXX      = clang++
CXXFLAGS = -std=c++17 -arch arm64 -I/opt/homebrew/include

# ------------------------------------------------------------
#  Platform-specific settings
# ------------------------------------------------------------
ifeq ($(UNAME_S),Darwin)
    LIBS = -L/opt/homebrew/lib -lraylib \
           -framework OpenGL \
           -framework Cocoa \
           -framework IOKit \
           -framework CoreVideo
    EXEC = ./$(TARGET)

else ifneq (,$(findstring MINGW,$(UNAME_S)))
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = ./$(TARGET)

else
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
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
