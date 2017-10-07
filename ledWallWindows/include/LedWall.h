
#pragma once

#include "Slab.h"
#include <vector>
#include "SpoutLibrary.h"

//  In this class, the generation of the video is made

class LedWall
{
public:
    LedWall();
    ~LedWall();
    bool initSpout();
    void nextImage();
    void sendImage();
    bool resizeReceiver(unsigned int width,unsigned int height);
    void run(int fps);
    void runDisplay();
    void setFps(int fps);
    int getFps() const { return m_fps; }
    int numberPanels() const { return (int) m_slabs.size(); }
    void send_sync_signal(char frame_to_show);
    void send_command(char command);
    void connect_broadcast();
    void load_config();

private:

    display_screen();
    WSADATA WSAData;
    const static int m_max_number_panels;
    std::vector <Slab*>m_slabs;
    SPOUTLIBRARY * m_spoutreceiver;					// A receiver object
    bool m_receverInitalized;
    unsigned int m_width;
    unsigned int m_height;
    char m_senderName[256];
    int m_fps;
    unsigned char * senderbuffer;
    char frame_number_counter;					// A frame counter between 0 and 25
    bool buffer_initialized;
    bool m_msgNoResolume;
    int m_delay;//delay between each frame
    bool m_ok;
    int m_numPanel;
    SOCKET m_sock;
    SOCKADDR_IN m_sin;
};
