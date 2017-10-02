#include <iostream>
#include "../include/LedWall.h"
#include <thread>
#include <mutex>

LedWall ledWall;

extern std::mutex mtx;

void task1()
{
    ledWall.run(20);
}

void task2()
{
    ledWall.runDisplay();
}

int main(int argc, char *argv[])
{
    /*std::thread **t = new std::thread *[ledWall.numberPanels()];
    for(int i = 0; i < ledWall.numberPanels(); i++)
    {
        mtx.lock();
        t[i] = new std::thread(task2);
    }*/
    std::thread t1(task1);
    std::cout << "entrez une commande"<<std::endl;
    std::string commande;
    while(1)
    {
        std::cin >> commande;
        if(commande == "help")
        {
            std::cout << "Commandes disponibles : "<<std::endl <<
            "* help : affiche les commandes disponibles" << std::endl <<
            "* fps : affiche le nombre de fps" << std::endl <<
            "* setfps : permet de changer le nombre de fps" << std::endl;
        }
        else if(commande == "fps")
        {
            std::cout << "Frame rate : " << ledWall.getFps() << std::endl;
        }
        else if(commande == "setfps")
        {
            std::cout << "Entrez le nouveau fps : ";
            int fps;
            std::cin >> fps;
            ledWall.setFps(fps);
            std::cout << "Le frame rate a ete change" << std::endl;
        }
        else
        {
            std::cout << "Commande non reconnue, tapez help pour voir les commandes disponibles" << std::endl;
        }
    }
    t1.join();
    return 0;
}


