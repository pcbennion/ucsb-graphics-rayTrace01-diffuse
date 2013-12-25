all: drawGL

SOURCES    = main.cpp RayTrace.cpp
OBJECTS    = $(SOURCES:.cpp=.o)

.cpp.o:
	g++ -c -Wall $< -o $@

drawGL: main.o RayTrace.o
	g++ $(OBJECTS) -lGL -lGLU -lglut $(LDFLAGS) -o $@

clean: 
	rm -f *.o
	rm drawGL
