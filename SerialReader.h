#ifndef N_SERIAL_READER
#define N_SERIAL_READER

#include <fstream>

#define TEST_FILE "nmea.txt"

class SerialReader
{
public:
    SerialReader();
    ~SerialReader();
    int init(const char*, int); // for serial port
    int init_mock(const char*); // for file (mocking)
    void read_line(char*);      // for serial port
    void read_line_mock(char*); // for file (mocking)
    
#ifndef UNIT_TEST 
private: 
#endif
    static const int LINE_MAX_LEN = 100;
    int serial_fd = 0; // file descriptor for serial port
    #ifdef SIL
        std::ifstream file_stream; //file stream for mocking
    #endif
    int open_port(const char*);
    int configure_port(int, int);
    int set_port_blocking (bool);
    int close_port();

};

#endif // N_SERIAL_READER
