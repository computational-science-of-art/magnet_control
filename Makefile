CC=g++
TARGET=magnet_control
CXXFLAGS+=-g
LDLIBS=-lpthread
OBJ=$(TARGET).o

.cpp.o:
	$(CC) $(CXXFLAGS) -c $^ -o $@ 

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -rf $(TARGET) $(OBJ) *~
