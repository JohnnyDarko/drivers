#include "SerialReader.h"
#include <stdio.h>  
#include <string.h>     
#include <fcntl.h>     
#include <termios.h>   
#include <time.h>       
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define PARITY 0
#define BLOCKING 0

// PUBLIC: ***************************************
SerialReader::SerialReader()
{
}

SerialReader::~SerialReader()
{
    close_port();

}

// Can later be overridden to include parity and blocking.
int SerialReader::init(const char* port_name, int baud_rate)
{
    #ifndef SIL  
    int res = -1;
    
    // EXEC stty -F portname baudrate
    
    if(serial_fd < 1){
        res = open_port(port_name);  
        if(res == 0)
        {
            configure_port(baud_rate, PARITY);
            set_port_blocking ( BLOCKING );
            res = 0;
        }
    }
    else
    {
        res = 0;
    }
    #else
    int res = -1;
    if(!file_stream.is_open())
    {   
        file_stream.close();
        file_stream.open(TEST_FILE);
    }
    if(file_stream.is_open())
    {
        res = 0;
    }
    #endif

    return res;
}

void SerialReader::read_line(char* sentence)
{
    char buffer[LINE_MAX_LEN] = {'\0'};
    memset( sentence, '\0', sizeof(char)*LINE_MAX_LEN);
    #ifndef SIL
        if(serial_fd > 1)
        {
            int pos = 0;
            while( pos < 99 ) 
            {
                int res = read(serial_fd, buffer+pos, 1); 
                if (res == -1)
                {
                    printf("Error reading from serial port\n");
                    break;
                }
                else if (res == 0)
                {
                    printf("No more data\n");
                    break;
                }
                else
                {
                    if(buffer[pos] == '\n')
                    {
                        buffer[pos] = 0;
                        break;
                    }
                    else if(buffer[pos] == '\r')
                    {
                        
                    }
                    else
                    {
                        pos++;
                    }
                }
                
            }
            usleep(8000);
        }
    #else
        int pos = 0;
        while( pos < 99 ) 
        {
            file_stream.get(buffer[pos]);
            if(file_stream.eof())
            {
                file_stream.clear();                 // clear fail and eof bits
                file_stream.seekg(0, std::ios::beg); // back to the start!
                buffer[pos] = 0;
                break;
            }
            if(buffer[pos] == '\n')
            {
                buffer[pos] = 0;
                break;
            }
            else if(buffer[pos] == '\r')
            {
                
            }
            else
            {
                pos++;
            }
        
            //usleep(1000);
        }
    #endif
    
    memccpy(sentence, buffer, '\0', sizeof(char)*LINE_MAX_LEN);
}

int SerialReader::open_port(const char* portname)
{
    int ret = 0;
    serial_fd = open (portname, O_RDWR | O_NONBLOCK);
    if (serial_fd < 0)
    {
        ret = -1;
    }
    
    return ret;
}

int SerialReader::configure_port (int speed, int parity)
{
    int ret = 0;
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (serial_fd, &tty) != 0)
    {
            //error_message ("error %d from tcgetattr", errno);
            ret = -1;

    }
    else
    {
        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     
        tty.c_iflag &= ~IGNBRK; 
        tty.c_lflag = 0;                
        tty.c_oflag = 0;                
        tty.c_cc[VMIN]  = 0;            
        tty.c_cc[VTIME] = 5;            
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); 
        tty.c_cflag |= (CLOCAL | CREAD);
        tty.c_cflag &= ~(PARENB | PARODD);  
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;
        ret = 0;

        if (tcsetattr (serial_fd, TCSANOW, &tty) != 0)
        {
            //error_message ("error %d from tcsetattr", errno);
            ret = -1;
        }
    }
    return ret;
}

int SerialReader::set_port_blocking (bool should_block)
{
    int ret = 0;
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (serial_fd, &tty) != 0)
    {
        //error_message ("error %d from tggetattr", errno);
        ret = -1;
    }
    else
    {
        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
        ret = 0;

        if (tcsetattr (serial_fd, TCSANOW, &tty) != 0)
        {
            //error_message ("error %d setting term attributes", errno);
            ret = -1;
        }
            
    }
    return ret;

}

int SerialReader::close_port()
{
    int ret = -1;
    
    #ifndef SIL
        if(serial_fd > 0)
        {
            close(serial_fd);
            printf("SerialNmea port fd:%d closed\n", serial_fd);
            serial_fd = 0;
            ret = 0;
            
        }
        else
        {
            // it was already closed
            ret = 0;
        }
    #else
        if (file_stream.is_open()) // for mocking
        {
            file_stream.close();
            serial_fd = 0;
            ret = 0;
        }
        else
        {
            // it was already closed
            ret = 0;
        }
    #endif
    return ret;

}
