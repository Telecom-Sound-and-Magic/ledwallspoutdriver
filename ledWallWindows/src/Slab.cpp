#include "../include/Slab.h"
#include <iostream>
#include <stdio.h>

#include <thread>

#include <sstream>

using namespace std;


Slab::Slab(int x0, int y0,int num)
{

    std::stringstream ss;

    ss << "192.168.1.";

    ss << (num+50);

    m_ip_adress = ss.str();

    m_connected = false;

    m_trying_to_connect = true;
    m_connect_thread = std::thread([=] {connectToPanel();});
    for(int i = 0; i < slabLenght; i++)
    {
        m_lines.push_back(LineOfPixels(x0,y0+i,i%2==0));
    }
}

Slab::~Slab()
{

    m_connect_thread.join();
    disconnectFromPanel();
}

void Slab::startCounting()
{
    m_index = 0;
    m_lines[0].startCounting();
}

bool Slab::nextPixel(int & x, int & y)
{
    if(m_lines[m_index].isAtTheEnd())
    {
        return nextLine(x,y);
    }
    else
    {
        m_lines[m_index].nextPixel(x,y);
        return true;
    }
}

bool Slab::nextLine(int & x, int & y)
{
    if(isAtTheEnd())
        return false;
    m_index += 1;
    m_lines[m_index].startCounting();
    return nextPixel(x,y);
}

void Slab::sendFrame(unsigned char *pixels,int w, int h, char frame_number)
{
    if(!m_connected)
    {

        if(!m_trying_to_connect)

        {

            m_connect_thread.join();

            m_trying_to_connect = true;

            m_connect_thread = std::thread([=] {connectToPanel();});

        }
        //connectToPanel();
        return;
    }
    int l = 1;
    unsigned char buffer[3000];
    buffer[0] = frame_number;
    int x,y;
    startCounting();
    while(nextPixel(x,y))
    {
        if(x >= w || y >= h)
        {
            buffer[l] = 0;
            buffer[l+1] = 0;
            buffer[l+2] = 0;
        }
        else
        {

            buffer[l+2] = pixels[(y*w+x)*3];
            buffer[l+1] = pixels[(y*w+x)*3+1];
            buffer[l] = pixels[(y*w+x)*3+2];
        }
        l+= 3;
    }
    int sent = 0;
    unsigned char *p = buffer;
    while (sent < l)
    {
        int n = send(m_sock,(char *) p, l - sent, 0);
        if (n < 0)
        {
            perror("error while sending");
            closesocket(m_sock);
            m_connected = false;
            break;
        }
        sent += n;
        p += n;
    }
}

void Slab::connectToPanel()
{
    /* socket's creation */
    m_sock = socket(AF_INET, SOCK_STREAM, 0);

    /* Connection's configuration */
    m_sin.sin_addr.s_addr = inet_addr(m_ip_adress.c_str());
    m_sin.sin_family = AF_INET;
    m_sin.sin_port = htons(port);

    /* if the connection is done */
    printf("Connection %s on the port %d\n", inet_ntoa(m_sin.sin_addr), htons(m_sin.sin_port));
    if(connect(m_sock, (SOCKADDR*)&m_sin, sizeof(m_sin)) != SOCKET_ERROR)
    {
        m_connected = true;
    }
    else
    {
        m_connected = false;
        std::cout << "impossible to connect to : " << m_ip_adress << std::endl;
    }

    m_trying_to_connect = false;

}

void Slab::disconnectFromPanel()
{
    m_connected = false;
    closesocket(m_sock);
}

