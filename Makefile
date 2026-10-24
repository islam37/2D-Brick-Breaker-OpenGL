CXX = g++
CXXFLAGS = -O2 -DFREEGLUT_STATIC
LDFLAGS = -lfreeglut_static -lopengl32 -lglu32 -lgdi32 -lwinmm
TARGET = bin/BrickBreaker.exe
SRC = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	@if not exist bin mkdir bin
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

clean:
	@if exist bin\BrickBreaker.exe del /Q bin\BrickBreaker.exe
