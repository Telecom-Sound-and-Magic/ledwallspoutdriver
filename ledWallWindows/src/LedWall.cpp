#include "../include/LedWall.h"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <mutex>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "opengl.h"
#include <fstream>
#include <sstream>

#define BUFLEN 512

std::mutex mtx;
std::mutex ready;

const static int broadcast_port = 8888;
const static std::string broadcast_address = "192.168.1.255";
const static unsigned char frames_in_buffer = 1;
using namespace std;
int broadcastPermission = 1;

LedWall::LedWall()
{
    m_ok = true;
    frame_number_counter = 0;
    buffer_initialized = false;
    WSAStartup(MAKEWORD(2,0), &WSAData);
    load_config();
    initSpout();
    m_msgNoResolume = false;
    m_numPanel = 0;
    connect_broadcast();
}

void LedWall::load_config()
{
    std::string path_to_config;
    std::cout << "Entrez le chemin d'acces au fichier de configuration : (tapez 'd' pour utiliser le fichier par defaut)" << std::endl;
    std::cin >> path_to_config;
    if(path_to_config == "d")
        path_to_config = "config.txt";
    std::cout << path_to_config << std::endl;
    std::ifstream infile(path_to_config);
    std::string line;
    int y = 0;
    while (std::getline(infile, line))
    {
        int x = 0;
        std::string num;
        std::stringstream ss(line);
        while(ss >> num)
        {
            if(num != "x")
            {
                m_slabs.push_back(new Slab(x,y,atoi(num.c_str())));
            }
            x += 18;
        }
        y += 18;
    }
}

LedWall::~LedWall()
{
    for(int i = 0; i < m_slabs.size(); i++)
    {
        delete m_slabs[i];
    }
    WSACleanup();
    if(senderbuffer) free((void *)senderbuffer);
}

void LedWall::run(int fps)
{
    setFps(fps);
    int oldTime = 0, newTime = 0;
    //SDL_Init(SDL_INIT_VIDEO);
    //SDL_WM_SetCaption("LedWall",NULL);
    //SDL_SetVideoMode(640,480, 32, SDL_OPENGL);
    //SDL_WM_IconifyWindow();
    //m_ok = true;
    //SDL_Event event;
    /*std::cout << "Begining of streaming..." << std::endl;
    while (m_ok)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            m_ok = false;
        }
        newTime = SDL_GetTicks();
        if(newTime - oldTime > m_delay)
        {
            oldTime = newTime;
            nextImage();
        }
    }
    SDL_Quit();*/

    /* Information about the current video settings. */
    const SDL_VideoInfo* info = NULL;
    /* Dimensions of our window. */
    int width = 0;
    int height = 0;
    /* Color depth in bits of our window. */
    int bpp = 0;
    /* Flags we will pass into SDL_SetVideoMode. */
    int flags = 0;
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        /* Failed, exit. */
        fprintf( stderr, "Video initialization failed: %s\n",
             SDL_GetError( ) );
        quit_tutorial( 1 );
    }
    info = SDL_GetVideoInfo( );
    if( !info ) {
        /* This should probably never happen. */
        fprintf( stderr, "Video query failed: %s\n",
             SDL_GetError( ) );
        quit_tutorial( 1 );
    }
    width = 640;
    height = 480;
    bpp = info->vfmt->BitsPerPixel;
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    flags = SDL_OPENGL;
    if( SDL_SetVideoMode( width, height, bpp, flags ) == 0 ) {
        fprintf( stderr, "Video mode set failed: %s\n",
             SDL_GetError( ) );
        quit_tutorial( 1 );
    }
    setup_opengl( width, height );
    while( 1 ) {
        /* Process incoming events. */
        process_events( );
        /* Draw the screen. */
        draw_screen( );
        newTime = SDL_GetTicks();
        if(newTime - oldTime > m_delay)
        {
            oldTime = newTime;
            nextImage();
        }
    }
}

void LedWall::runDisplay()
{
    int panel = m_numPanel;
    m_numPanel++;
    int oldTime = 0, newTime = 0;
    std::cout << "panel num : " << panel << std::endl;
    std::cout << "salut";
    mtx.unlock();
    while(m_ok)
    {
        newTime = SDL_GetTicks();
        if(newTime - oldTime > m_delay)
        {
            oldTime = newTime;
            m_slabs[panel]->sendFrame(senderbuffer, m_width, m_height, 0);
        }
    }
}

void LedWall::setFps(int fps)
{
    m_fps = fps;
    m_delay = 1000/m_fps;
}

void LedWall::nextImage()
{
    unsigned int width = m_width, height = m_height;
    if(!m_receverInitalized)
    {
        if(m_spoutreceiver->CreateReceiver(m_senderName,width,height, true))
        {
            resizeReceiver(width,height);
            std::cout << "Resolume detected" << std::endl;
            m_receverInitalized = true;

        }
        else
        {
            if(!m_msgNoResolume)
            {
                std::cout << "Resolume not found, please check if you have selected spout as output" << std::endl;
                m_msgNoResolume = true;
            }
        }
        return;
    }
    if(m_spoutreceiver->ReceiveImage(m_senderName,width,height, senderbuffer, GL_RGB))
    {
        if(resizeReceiver(width,height))
        {
            return;
        }
        else
        {
            for(unsigned int i = 0; i < m_slabs.size(); i++)
            {
                m_slabs[i]->sendFrame(senderbuffer,m_width,m_height, frame_number_counter);
            }
            if (buffer_initialized)
            {
            	send_sync_signal((frame_number_counter + (26-frames_in_buffer))%26);	// Show the N-frames_in_buffer frame
            }
            else
            {
            	if (frame_number_counter >= frames_in_buffer)
            	{
            		buffer_initialized = true;
            	}
            }
            frame_number_counter++;
            if (frame_number_counter > 25)
            {
            	frame_number_counter = 0;
            }
        }
    }
    else
    {
        m_msgNoResolume = false;
        m_receverInitalized = false;
    }
}

bool LedWall::resizeReceiver(unsigned int width,unsigned int height)
{
    if(m_width != width || m_height != height)
    {
        std::cout << "taille : " << width << ";"<<height<<std::endl;
        m_width = width;
        m_height = height;
        if(senderbuffer) free((void *)senderbuffer);
        senderbuffer = (unsigned char *)malloc(m_width*m_height*4*sizeof(unsigned char));
        return true;
    }
    return false;
}

void LedWall::connect_broadcast()
{
    if((m_sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0)
    {
        cout << "error while creating socket" << endl;
    }
    if(setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST,(char*) &broadcastPermission,sizeof(broadcastPermission)) < 0)
    {
        cout << "error changing to broadcast" << endl;
    }
    memset(&m_sin,0,sizeof(broadcast_address.c_str()));
    m_sin.sin_addr.s_addr = inet_addr(broadcast_address.c_str());
    m_sin.sin_family = AF_INET;
    m_sin.sin_port = htons(broadcast_port);
}

void LedWall::send_sync_signal(char frame_to_show)
{
    char buffer[2];
    buffer[0] = frame_to_show;
    buffer[1] = '\0';
    int sendStringLen = strlen(buffer);
    if(sendto(m_sock,buffer,sendStringLen,0,(SOCKADDR*)&m_sin, sizeof(m_sin)) != sendStringLen)
    {
        std::cout << "error while sending broadcast message" << std::endl;
    }
}

bool LedWall::initSpout()
{
    m_spoutreceiver = GetSpout();
    if(!m_spoutreceiver)
    {
        return false;
    }
    m_receverInitalized = false;
    m_width = 0;
    m_height = 0;
    senderbuffer = 0;
    return true;
}
