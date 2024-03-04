#include "AcquisitionSetup.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>

AcquisitionSetup::AcquisitionSetup(){
    bool isRunningAcquisition = true;
    char opt_menu;

    while(isRunningAcquisition){
        system("cls");
        std::cout << "- Acquisition Setup -" << std::endl;
        if(!isConnectedArduino){
            std::cout << "[C]onnect Arduino \n"
                         "[E]xit" << std::endl;
        }
        else{
            system("cls");
            std::cout << "Cameras connected " << n_camera_connected <<"/4 - Light intensity value: " << curr_intensityValue << std::endl;
            std::cout << "[T]ake pictures \n"
                         "[M]ode video/photo \n"
                         "[L]ight intensity value \n"
                         "[I]nfo cameras \n"
                         "[C]lose Arduino \n"
                         "[E]xit" << std::endl;
        }

        std::cin >> opt_menu;

        if(!isConnectedArduino){
            switch(opt_menu){
                case 'c': case 'C': on_startArduino(); break;
                case 'e': case 'E': isRunningAcquisition = false; break;
                default:{
                    std::cout << "Command not recognized" << std::endl;
                    std::cout << "Press any key to continue..." << std::endl;
                    getch();
                    break;
                }
            }
        }
        else{
            switch(opt_menu){
                case 't': case 'T': takePicture(); break;
                case 'm': case 'M': {
                    if(isRecording){
                        QString val = "stop";
                        int res = arduinoUpdateValue(QString("%1").arg(val));
                        if(res == 0)
                           isRecording = false;
                    }

                    if(currentMode.compare("video") == 0)
                        currentMode = "photo";
                    else
                        currentMode = "video";

                    arduinoUpdateValue(QString("%1").arg(currentMode));
                    break;
                }
                case 'l': case 'L':{
                    system("cls");
                    std::cout << "Set intensity light: [0 - 1023] " << std::endl;
                    std::cin >> curr_intensityValue;
                    LightUpdateValue(curr_intensityValue);
                    break;
                }
                case 'i': case 'I': printInfoCamera(); break;
                case 'c': case 'C': resetAll(); break;
                case 'e': case 'E': isRunningAcquisition = false; break;
                default:{
                    std::cout << "Command not recognized" << std::endl;
                    std::cout << "Press any key to continue..." << std::endl;
                    getch();
                    break;
                }
            }
        }

    }

    resetAll();
}

void AcquisitionSetup::on_startArduino(){
    arduino_is_available = false;
    arduino_port_name = "";

    serialBuffer_arduino = "";
    arduino = new QSerialPort;
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
        {
            if(serialPortInfo.vendorIdentifier() == 4292)
            {
                if(serialPortInfo.productIdentifier() == 60000)
                {
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                }
            }
        }

        if(arduino_is_available)
        {
            // open and configure the port
            arduino->setPortName(arduino_port_name);
            arduino->open(QSerialPort::ReadWrite);
            arduino->setBaudRate(QSerialPort::Baud115200);
            arduino->setDataBits(QSerialPort::Data8);
            arduino->setParity(QSerialPort::NoParity);
            arduino->setStopBits(QSerialPort::OneStop);
            arduino->setFlowControl(QSerialPort::NoFlowControl);
            arduino->setDataTerminalReady(true);
            QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(arduinoReadData()));
            QObject::connect(arduino, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));

            /*if (hand_is_available) {
                ui->RemoteActivator->setEnabled(true);
                ui->messageBox->setVisible(false);
                ui->dialogButton->setVisible(false);
                ui->retryButton->setVisible(false);
            }*/

            QString val = "on";
            arduinoUpdateValue(QString("%1").arg(val));

            LightUpdateValue(curr_intensityValue);

            isConnectedArduino = true;
            return;
        }
    }

    if(!arduino_is_available){
        // give error message
        std::cout << "Couldn't find the Arduino!" << std::endl;

        std::cout << "Press any key to continue..." << std::endl;
        getch();
    }
}

void AcquisitionSetup::arduinoReadData(){
    serialData_arduino = arduino->readAll();
    serialBuffer_arduino += QString::fromStdString(serialData_arduino.toStdString());

    int endlinePos = serialBuffer_arduino.indexOf("\r\n");
    QString data = "";

    if(endlinePos >= 0){
        data = serialBuffer_arduino.first((endlinePos));

        serialBuffer_arduino = serialBuffer_arduino.mid(endlinePos + 2);
        manageSerialData(data);
    }
}

void AcquisitionSetup::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        isConnectedArduino = false;

        resetAll();

        // give error message
        std::cout << "Couldn't find the Arduino!" << std::endl;
    }
}

void AcquisitionSetup::manageSerialData(QString command){
    if(command.contains("<TM>")){
        QString cam_revealed = command.remove(0, command.indexOf("@") + 1);
        bool found = false;

        int i = 0;

        while(i < 4 && !found){
            if(camConnected[i].compare("") == 0){
                camConnected[i] = cam_revealed;
                n_camera_connected++;
                found = true;
            }
            else if(camConnected[i].compare(cam_revealed) == 0)
                found = true;
            else
                i++;
        }
    }
    else if(command.contains("<st>")){
        QString status = command.remove(0, command.indexOf("<st>") + 4);
        status = status.remove(status.indexOf("</"), status.length() - status.indexOf("</"));
        QStringList statusSplit = status.split(" ");

        QString shutter = statusSplit[15];
        int mode = statusSplit[14].toInt();

        QString cam_mac = command.remove(0, command.indexOf("@") + 1);

        for(int i = 0; i < 4; i++){
            if(camConnected[i].compare(cam_mac) == 0){
                if(shutter.compare("0") == 0){

                    switch(mode){
                        case 0:{
                            camMode[i] = "Video";
                            break;
                        }
                        case 1:{
                            camMode[i] = "Photo";
                            break;
                        }
                        case 2:{
                            camMode[i] = "Burst";
                            break;
                        }
                        case 3:{
                            camMode[i] = "Timelapse";
                            break;
                        }
                        case 7:{
                            camMode[i] = "Settings";
                            break;
                        }
                        default:{
                            camMode[i] = "Unknown";
                            break;
                        }
                    }
                }
            }
        }
    }
    else if(command.contains("<BL>")){
        QString battLevel = command.remove(0, command.indexOf("<BL>") + 4);
        battLevel = battLevel.remove(battLevel.indexOf("</"), battLevel.length() - battLevel.indexOf("</"));

        QString cam_mac = command.remove(0, command.indexOf("@") + 1);

        for(int i = 0; i < 4; i++){
            if(camConnected[i].compare(cam_mac) == 0)
                camBL[i] = battLevel + "%";
        }
    }
}

void AcquisitionSetup::printInfoCamera(){
    system("cls");
    for(int i = 0; i < 4; i++){
        if(camConnected[i].compare("") != 0){
            printf("Cam %i\n %s connected\n Mode: %s\n Battery level: %s\n", (i+1),
                   camConnected[i].toStdString().c_str(), camMode[i].toStdString().c_str(), camBL[i].toStdString().c_str());
            std::cout << std::endl;
        }
    }

    std::cout << "Press any key to continue..." << std::endl;
    getch();
}

void AcquisitionSetup::takePicture(){

    bool isVideoMode = false;

    for(int i = 0; i < 4; i++){
        if(camConnected[i].compare("") != 0){
            if(camMode[i].compare("Video") == 0)
                isVideoMode = true;
        }
    }

    if(isVideoMode){
        //check if the camera is recording or not
        if(!isRecording){
            QString val = "start";
            int res = arduinoUpdateValue(QString("%1").arg(val));
            if(res == 0)
               isRecording = true;
        }
        else{
            QString val = "stop";
            int res = arduinoUpdateValue(QString("%1").arg(val));
            if(res == 0)
               isRecording = false;
        }
    }
    else{
        QString val = "start";
        arduinoUpdateValue(QString("%1").arg(val));
    }
}

int AcquisitionSetup::arduinoUpdateValue(QString command){

    if(arduino->isOpen() && arduino->isWritable())
    {
        arduino->write(command.toStdString().c_str());
        arduino->flush();
    }
    else
    {
        qDebug() << "Couldn't write to serial!";
        isConnectedArduino = false;
        return -1;
    }

    return 0;
}

void AcquisitionSetup::LightUpdateValue(int value)
{
    QString val = "light " + QString::number(value);
    arduinoUpdateValue(QString("%1").arg(val));
}

void AcquisitionSetup::resetAll(){
    if(isRecording){
        QString val = "stop";
        int res = arduinoUpdateValue(QString("%1").arg(val));
        if(res == 0)
           isRecording = false;
    }

    for(int i = 0; i < 4; i++){
        camConnected[i] = "";
        camMode[i] = "-";
        camBL[i] = "-";
    }

    n_camera_connected = 0;
    arduino_is_available = false;
    curr_intensityValue = 500;
    isConnectedArduino = false;
    currentMode = "video";
    arduino->close();
}
