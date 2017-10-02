#ifndef SLAB_H
#define SLAB_H


#include "LineOfPixels.h"
#include <vector>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <string>

#include <thread>


typedef int socklen_t;
const static int port = 9999;

class Slab
{
    public:
        Slab(int x0, int y0,int num);
        ~Slab();
        void connectToPanel();
        void disconnectFromPanel();
        void startCounting();
        bool nextPixel(int & x, int & y);///puts the position of the next pixel in x and y and returns false if it's the last one
        bool nextLine(int & x, int & y);
        bool isAtTheEnd() const { return m_index == slabLenght - 1; }
        void sendFrame(unsigned char *pixels,int w, int h, char frame_number);
        static int number;

    private:

        std::thread m_connect_thread;
        SOCKET m_sock;
        SOCKADDR_IN m_sin;
        bool m_connected;

        bool m_trying_to_connect;
        int m_index;                                ///While running throw the line, index indicates what number of lines we have already been through
        std::vector <LineOfPixels> m_lines;                 ///all the lines in the

        std::string m_ip_adress;
};

#endif // SLAB_H
