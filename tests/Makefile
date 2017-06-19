SRC_DIR = ../
BUILD_DIR = ./build
BIN_DIR = ./bin
CATCH_DIR = ./catch
LIB_DIRS = /usr/local/lib

INCDIRS = -I/usr/local/include -I$(LIB_DIRS) -I./$(CATCH_DIR) -I$(SRC_DIR)
TARGET_OBJECTS =	$(BUILD_DIR)/SerialReader.o
TEST_OBJECTS =  $(BUILD_DIR)/SerialReader_test.o


LDFLAGS = -L${LIB_DIRS}
LIBS += -lpthread -lrt

all:

serialreader-host-sil: CC = gcc
serialreader-host-sil: CXX = g++
serialreader-host-sil: DEFINES = -DSIL -DUNIT_TEST
serialreader-host-sil: CFLAGS = $(DEFINES) -v -ggdb -O0 -Wall -Werror -std=c++11 $(INCDIRS)
serialreader-host-sil: XFLAGS = $(DEFINES) -v -ggdb -O0 -Wall -Werror -std=c++11 $(INCDIRS) 
serialreader-host-sil: serialreader serialreader_test  
	$(CXX)  -o  $(BIN_DIR)/serialreader-host-sil_test  $(TARGET_OBJECTS) $(TEST_OBJECTS)  $(LIBS) -static

serialreader-target-sil: CC = arm-linux-gnueabihf-gcc
serialreader-target-sil: CXX = arm-linux-gnueabihf-g++
serialreader-target-sil: DEFINES = -DSIL -DUNIT_TEST
serialreader-target-sil: CFLAGS = $(DEFINES) -v -ggdb -O0 -Wall -Werror -std=c++11 $(INCDIRS)
serialreader-target-sil: XFLAGS = $(DEFINES) -v -ggdb -O0 -Wall -Werror -std=c++11 $(INCDIRS) 
serialreader-target-sil: serialreader serialreader_test  
	$(CXX)  -o  $(BIN_DIR)/serialreader-target-sil_test  $(TARGET_OBJECTS) $(TEST_OBJECTS)  $(LIBS) -static

# *********** TESTS *****************************

serialreader_test:
	$(CXX) -c $(XFLAGS) ./SerialReader_test.cpp  -o $(BUILD_DIR)/SerialReader_test.o

# *********** TARGETS ***************************
serialreader:
	$(CXX) $(XFLAGS) -S -fverbose-asm -o $(BUILD_DIR)/SerialReader.s $(SRC_DIR)/SerialReader.cpp
	$(CXX) -c $(XFLAGS) $(SRC_DIR)/SerialReader.cpp  -o $(BUILD_DIR)/SerialReader.o

.PHONY : clean

clean:
	rm -f $(BUILD_DIR)/*