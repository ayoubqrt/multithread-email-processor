CC=g++
CXXFLAGS=-O2 -pthread -std=c++17 -I.
LDFLAGS=
TARGET=main

all: $(TARGET) run

$(TARGET): main.cpp Mail.o Find.o Trim.o Vector.o Worker.o
	$(CC) $(CXXFLAGS) -o $@ $^

%.o: src/%.cpp include/%.h
	$(CC) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
