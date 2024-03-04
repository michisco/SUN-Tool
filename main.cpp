#include "AcquisitionSetup.cpp"
#include "CalibrationCamera.cpp"

#include <QApplication>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    char opt_menu;
    bool isRunning = true;

    while(isRunning){
        system("cls");
        std::cout << "Menu SUN tool : " << std::endl;
        std::cout << "[A]cquisition images from cameras \n"
                     "[V]isualize markers \n"
                     "[C]lose" << std::endl;
        std::cin >> opt_menu;

        switch(opt_menu){
            case 'a': case 'A': AcquisitionSetup(); break;
            case 'v': case 'V': VisualizeMarkers(); break;
            case 'c': case 'C': isRunning = false; break;
            default:{
                std::cout << "Command not recognized" << std::endl;
                std::cout << "Press any key to continue..." << std::endl;
                getch();
                break;
            }
        }
    }

    return a.exec();
    //return 0;
}
