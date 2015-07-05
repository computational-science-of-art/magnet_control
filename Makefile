CC=g++
TARGET=magnet_control
CXXFLAGS+=-lpthread

all: $(TARGET)

clean:
	rm -rf $(TARGET) *~
