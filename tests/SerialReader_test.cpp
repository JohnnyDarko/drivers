#include "test_main.hpp"

#include <fstream>  
#include <iostream>  
#include <string>

#include "SerialReader.h"
#include <cstring>

#define GPS_PORT "/dev/serial0"
#define GPS_BAUD_RATE 38400
#define GPS_PARITY 0
#define GPS_BLOCKING 0

#define SOFTWARE_IN_THE_LOOP 3
#define HARDWARE_IN_THE_LOOP 4

bool validate_checksum(const char*);


TEST_CASE( "Test Serial reader", "Serial Reader" ) 
{
#ifdef SIL
    int test_type = SOFTWARE_IN_THE_LOOP;
#else
    int test_type = HARDWARE_IN_THE_LOOP;
#endif
    SerialReader serialreader;
    int res = 0;
    char test_sentence[100] = {0};
    
    // create nmea.txt for mock nmea sentence
    std::ofstream file_writer("nmea.txt"); // out file stream
    file_writer << "$GNRMC,071342.60,A,3650.13719,N,09201.24908,W,0.055,,290517,,,D*74\r\n";
    file_writer << "$GNRMC,071342.60,A,3650.13719,N,09201.24908,W,0.055,,290517,,,D*75\r\n";
    file_writer << "$GNGGA,071342.60,3650.13719,N,09201.24908,W,2,12,1.02,304.2,M,-29.7,M,,0000*72\r\n";
    file_writer << "$GNVTG,,T,,M,0.011,N,0.020,K,D*3A\r\n";
    file_writer << "$GNRMC,071342.60,A,3650.13719,N,09201.24908,W,0.055,,290517,,,D*74\r\n";

    file_writer.close();
    
    SECTION( "Test open port" )
    {
        switch(test_type)
        {
            case SOFTWARE_IN_THE_LOOP:
            {
                res = 0; // there is no mock for open port
                break;
            }
            case HARDWARE_IN_THE_LOOP:
            {
                res = serialreader.open_port(GPS_PORT);
            }
        }
            
        REQUIRE( res == 0 );
    }
    
    SECTION( "Test configure port" )
    {
        switch(test_type)
        {
            case SOFTWARE_IN_THE_LOOP:
            {
                res = 0; // there is no mock for configure port
                break;
            }
            case HARDWARE_IN_THE_LOOP:
            {
                res = serialreader.open_port(GPS_PORT);
                res = serialreader.configure_port(GPS_BAUD_RATE, GPS_PARITY);
            }
        }
            
        REQUIRE( res == 0 );
    }
    
    SECTION( "Test set port blocking" )
    {
        switch(test_type)
        {
            case SOFTWARE_IN_THE_LOOP:
            {
                res = 0; // there is no mock for set port blocking
                break;
            }
            case HARDWARE_IN_THE_LOOP:
            {
                res = serialreader.open_port(GPS_PORT);
                res = serialreader.configure_port(GPS_BAUD_RATE, GPS_PARITY);
                res = serialreader.set_port_blocking(GPS_BLOCKING);
            }
        }
            
        REQUIRE( res == 0 );
    }
    
    SECTION( "Test close port" )
    {
        switch(test_type)
        {
            case SOFTWARE_IN_THE_LOOP:
            {
                res = 0; 
                break;
            }
            case HARDWARE_IN_THE_LOOP:
            {
                res = serialreader.open_port(GPS_PORT);
                res = serialreader.configure_port(GPS_BAUD_RATE, GPS_PARITY);
                res = serialreader.set_port_blocking(GPS_BLOCKING);
                res = serialreader.close_port();
            }
        }
            
        REQUIRE( res == 0 );
    }
    
    SECTION( "Test initialization pass" ) 
    {

        res = serialreader.init(GPS_PORT, GPS_BAUD_RATE);

        REQUIRE( res == 0 );
    }
    
    SECTION( "Test initialization fail" ) 
    {

        res = serialreader.init("/dev/dummy", GPS_BAUD_RATE);
        if(test_type != SOFTWARE_IN_THE_LOOP)
        {
           REQUIRE( res == -1 ); 
        }
        else
        {
            REQUIRE( res == 0 ); // cant intentionally send bad file name
        }
            
    }

    
    SECTION( "Test Read Line " ) 
    {
            
        res = serialreader.init(GPS_PORT, GPS_BAUD_RATE);
        serialreader.read_line(test_sentence);
        serialreader.read_line(test_sentence);
        serialreader.read_line(test_sentence);
        serialreader.read_line(test_sentence); // gets us past partial reads
        
        REQUIRE(validate_checksum(test_sentence)==true);
    }
}



bool validate_checksum(const char* sentence)
{
    bool ret = false;
    int i = 0;
    int calc_sum = 0;
    char sentence_checksum[2] = {'\0'};
    int sentence_sum = 0;
    int c = 0;
    for (calc_sum = 0, i = 0; i < 100; i++)
    {
        if(sentence[i])
        {
            c = static_cast<uint8_t>(sentence[i]);
            if (c == '*') 
            {
                for(int j=1;j<3; j++)
                {
                   sentence_checksum[j-1] = sentence[i+j];
                }
                break;
            }
            if (c != '$')
            {
                calc_sum ^= c;
            }
        }

    }
    sentence_checksum[2] = '\0';
    sentence_sum = strtol(sentence_checksum, NULL, 16);
    if(calc_sum == sentence_sum)
    {
        ret = true;
    } 
    return ret;
}