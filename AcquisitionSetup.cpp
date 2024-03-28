#include "AcquisitionSetup.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <chrono>
#include <thread>

AcquisitionSetup::AcquisitionSetup(){
    bool isRunningAcquisition = true;
    char opt_menu;

    while(isRunningAcquisition){
        clearScreen(isToggleCLS);
        std::cout << "- Acquisition Setup -" << std::endl;
        if(!isConnectedArduino || !isConnectedHand){
            std::cout << "[C]onnect Arduino&Device \n"
                         "[E]xit" << std::endl;
        }
        else{
            clearScreen(isToggleCLS);
            std::cout << "Cameras connected " << n_camera_connected <<"/4 - Light intensity value: " << curr_intensityValue << std::endl;
            std::cout << "[T]ake pictures \n"
                         "[M]ode video/photo \n"
                         "[L]ight intensity value \n"
                         "[I]nfo cameras \n"
                         "[R]otate hand device \n"
                         "[A]utomatic acquisition \n"
                         //"[C]lose Arduino&Device \n"
                         "[E]xit" << std::endl;

            while (arduino->waitForReadyRead(1000)) {
                arduinoReadData();
            }

            while (hand_device->waitForReadyRead(1000)) {
                handReadData();
            }
        }

        std::cin >> opt_menu;

        

        if(!isConnectedArduino || !isConnectedHand){
            switch(opt_menu){
                case 'c': case 'C': on_startArduino(); break;
                case 'e': case 'E': isRunningAcquisition = false; break;
                default:{
                    std::cout << "Command not recognized" << std::endl;
                    waitKey(isToggleCLS);
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
                    clearScreen(isToggleCLS);
                    std::cout << "Set intensity light: [0 - 1023] " << std::endl;
                    std::cin >> curr_intensityValue;
                    LightUpdateValue(curr_intensityValue);
                    break;
                }
                case 'r': case 'R': {
                    clearScreen(isToggleCLS);
                    std::string command = "s 10 2";
                    std::cout << "Set steps and velocity: {example: s 10 2} " << std::endl;
                    std::getline(std::cin >> std::ws, command);
                   
                    QString val = QString::fromStdString(command);
                    handSendCommand(QString("%1").arg(val));
                    break;
                }
                case 'i': case 'I': printInfoCamera(); break;
                case 'a': case 'A': {
                    clearScreen(isToggleCLS);
                    std::string velocity = "2";
                    std::cout << "Set velocity: {example: 2} " << std::endl;
                    std::cin >> velocity;

                    int seconds_interval = (10 / std::stoi(velocity)) + 1;
                    takePicture();
                    std::this_thread::sleep_for(std::chrono::seconds(1));

                    std::string comm = "";
                    QString val = "";

                    //clock-wise
                    for (int i = 0; i < 10; i++) {
                        comm = "s 10 " + velocity;
                        val = QString::fromStdString(comm);
                        handSendCommand(QString("%1").arg(val));

                        std::this_thread::sleep_for(std::chrono::seconds(seconds_interval));
                        takePicture();
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }

                    comm = "s 100 -" + velocity;
                    val = QString::fromStdString(comm);
                    handSendCommand(QString("%1").arg(val));
                    std::this_thread::sleep_for(std::chrono::seconds((100 / std::stoi(velocity)) + 1));

                    //counter clock-wise
                    for (int i = 0; i < 9; i++) {
                        comm = "s 10 -" + velocity;
                        val = QString::fromStdString(comm);
                        handSendCommand(QString("%1").arg(val));

                        std::this_thread::sleep_for(std::chrono::seconds(seconds_interval));
                        takePicture();
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }

                    comm = "s 100 " + velocity;
                    val = QString::fromStdString(comm);
                    handSendCommand(QString("%1").arg(val));
                    std::this_thread::sleep_for(std::chrono::seconds((100 / std::stoi(velocity)) + 1));
                    break; 
                }
                case 'e': case 'E': isRunningAcquisition = false; break;
                default:{
                    std::cout << "Command not recognized" << std::endl;
                    waitKey(isToggleCLS);
                    break;
                }
            }
        }
    }

    resetAll();
}

void AcquisitionSetup::on_startArduino(){
    
    if (!isConnectedArduino) {
        arduino_is_available = false;
        arduino_port_name = "";

        serialBuffer_arduino = "";
        arduino = new QSerialPort;

        foreach(const QSerialPortInfo & serialPortInfo, QSerialPortInfo::availablePorts())
        {
            if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
            {
                if (serialPortInfo.vendorIdentifier() == 4292)
                {
                    if (serialPortInfo.productIdentifier() == 60000)
                    {
                        arduino_port_name = serialPortInfo.portName();
                        arduino_is_available = true;
                    }
                }
            }

            if (arduino_is_available)
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
                break;
            }
        }

        if (!arduino_is_available) {
            // give error message
            std::cout << "Couldn't find the Arduino!" << std::endl;

            waitKey(isToggleCLS);
        }
    }
    
    if (!isConnectedHand) {
        hand_is_available = false;
        hand_port_name = "";

        serialBuffer_hand = "";
        hand_device = new QSerialPort;
        foreach(const QSerialPortInfo & serialPortInfo, QSerialPortInfo::availablePorts())
        {
            if (serialPortInfo.vendorIdentifier() == 6790)
            {
                if (serialPortInfo.productIdentifier() == 29986)
                {
                    hand_port_name = serialPortInfo.portName();
                    hand_is_available = true;
                }
            }

            if (hand_is_available)
            {
                //open and configure the port
                hand_device->setPortName(hand_port_name);
                std::cout << hand_port_name.toStdString() << std::endl;
                bool isopen = hand_device->open(QSerialPort::ReadWrite);
                hand_device->setBaudRate(QSerialPort::Baud115200);
                hand_device->setDataBits(QSerialPort::Data8);
                hand_device->setParity(QSerialPort::NoParity);
                hand_device->setStopBits(QSerialPort::OneStop);
                hand_device->setFlowControl(QSerialPort::NoFlowControl);
                hand_device->setDataTerminalReady(true);

                QObject::connect(hand_device, SIGNAL(readyRead()), this, SLOT(handReadData()));
                QObject::connect(hand_device, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleErrorHand(QSerialPort::SerialPortError)));

                /*if(arduino_is_available){
                    ui->RemoteActivator->setEnabled(true);
                    ui->messageBox->setVisible(false);
                    ui->dialogButton->setVisible(false);
                    ui->retryButton->setVisible(false);
                }*/

                isConnectedHand = true;
                break;
            }         
        }

        if (!hand_is_available) {
            // give error message
            std::cout << "Couldn't find the hand device!" << std::endl;

            waitKey(isToggleCLS);
        }
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
        //std::cout << data.toStdString().c_str() << std::endl;
    }

    //std::cout.flush() << std::endl;
}

void AcquisitionSetup::handReadData() {
    serialData_hand = hand_device->readAll();
    serialBuffer_hand += QString::fromStdString(serialData_hand.toStdString());

    //qDebug() << serialBuffer_hand;

    std::string s_data = serialData_hand.toStdString();
    std::string data_splitted[5];
    size_t pos = 0;
    std::string token;
    int i = 0;
    while (((pos = s_data.find(",")) != std::string::npos) && i < 5) {
        token = s_data.substr(0, pos);
        data_splitted[i] = token;
        s_data.erase(0, pos + 1);
        i++;
    }

    handSteps = QString::fromStdString(data_splitted[0]);
    finger1Sensor = QString::fromStdString(data_splitted[1]);
    finger2Sensor = QString::fromStdString(data_splitted[2]);
    finger3Sensor = QString::fromStdString(data_splitted[3]);
    finger4Sensor = QString::fromStdString(data_splitted[4]);
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

void AcquisitionSetup::handleErrorHand(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        isConnectedHand = false;

        resetAll();

        // give error message
        std::cout << "Couldn't find the hand device!" << std::endl;
    }
}

int AcquisitionSetup::handSendCommand(QString command) {

    if (hand_device->isOpen() && hand_device->isWritable())
    {
        hand_device->write(command.toStdString().c_str());
        hand_device->flush();
        std::cout << "data has been send" << std::endl;
        qDebug() << command;
    }
    else
    {
        std::cout << "Couldn't write to hand serial!" << std::endl;

        isConnectedHand = false;
        return -1;
    }

    return 0;
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
    clearScreen(isToggleCLS);
    for(int i = 0; i < 4; i++){
        if(camConnected[i].compare("") != 0){
            printf("Cam %i\n %s connected\n Mode: %s\n Battery level: %s\n", (i+1),
                   camConnected[i].toStdString().c_str(), camMode[i].toStdString().c_str(), camBL[i].toStdString().c_str());
            std::cout << std::endl;
        }
    }

    printf("Hand step: %s\n Finger 1: %s\n Finger 2: %s\n Finger 3: %s\n Finger 4: %s\n",
       handSteps.toStdString().c_str(), finger1Sensor.toStdString().c_str(),
        finger2Sensor.toStdString().c_str(), finger3Sensor.toStdString().c_str(), finger4Sensor.toStdString().c_str());
    std::cout << std::endl;

    waitKey(isToggleCLS);
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
        std::cout << "data has been send" << std::endl;
        qDebug() << command;
        arduino->write(command.toStdString().c_str());
        arduino->flush();
    }
    else
    {
        std::cout << "Couldn't write to serial!" << std::endl;

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
    hand_is_available = false;
    curr_intensityValue = 512;
    isConnectedArduino = false;
    isConnectedHand = false;
    currentMode = "video";
    arduino->close();
    hand_device->close();
}

void AcquisitionSetup::clearScreen(bool toggle) {
    if (toggle) {
        system("cls");
    }
}

void AcquisitionSetup::waitKey(bool toggle) {
    if (toggle) {
        std::cout << "Press any key to continue..." << std::endl;
        getch();
    }
}