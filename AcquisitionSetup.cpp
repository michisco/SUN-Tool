#include "AcquisitionSetup.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QApplication>
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
    std::string opt_menu;
    char* comm;

    m_pThread = new QThread(this);
    m_pDevice = new HandDevice;

    m_pThread_arduino = new QThread(this);
    m_pArduino = new arduinoDevice;

    m_pDevice->moveToThread(m_pThread);
    connect(m_pThread, SIGNAL(finished()), m_pDevice, SLOT(deleteLater()));
    m_pThread->start();

    m_pArduino->moveToThread(m_pThread_arduino);
    connect(m_pThread_arduino, SIGNAL(finished()), m_pArduino, SLOT(deleteLater()));
    m_pThread_arduino->start();

    while(isRunningAcquisition){
        clearScreen(isToggleCLS);
        std::cout << "- Acquisition Setup -" << std::endl;
        if(!m_pArduino->isConnected() && !m_pDevice->isConnected()) {
            std::cout << "[C]onnect Arduino and device \n"
                         "[Q]uit" << std::endl;        
        }
        else if (!m_pArduino->isConnected()) {
            std::cout << "[C]onnect Arduino \n"
                "[Q]uit" << std::endl;
        }
        else if (!m_pDevice->isConnected()) {
            std::cout << "[C]onnect device \n"
                "[Q]uit" << std::endl;
        }
        else{
            std::cout << "Current Light intensity value: " << curr_intensityValue << std::endl;
            std::cout << "Insert command - for help, digit 'h' or 'help'" << std::endl;           
        }

        std::cin.getline(comm, 50);
        opt_menu = comm;

        std::istringstream stream(opt_menu);
        std::string token;
        std::string arr[3] = { "", "", "" };
        int index = 0;

        while (std::getline(stream, token, ' ')) {
            // Add the token to the array 
            arr[index++] = token;
        }

        if(!m_pArduino->isConnected() || !m_pDevice->isConnected()){
            if (opt_menu == "c" || opt_menu == "C")
                on_startArduino();
            else if (opt_menu == "q" || opt_menu == "Q")
                isRunningAcquisition = false;
            else {
                std::cout << "Command not recognized" << std::endl;
                waitKey(isToggleCLS);
            }
        }
        else{
            if (arr[0] == "p" || arr[0] == "play")
                m_pArduino->takePicture(); // takePicture();
            else if (arr[0] == "h" || arr[0] == "help")
                printHelpGuide();
            else if (arr[0] == "q" || arr[0] == "quit")
                isRunningAcquisition = false;
            else if (arr[0] == "i" || arr[0] == "info")
                m_pArduino->printInfoCamera(); // printInfoCamera();
            else if (arr[0] == "m" || arr[0] == "mode") {
                /*if (isRecording) {
                    QString val = "stop";
                    int res = arduinoUpdateValue(QString("%1").arg(val));
                    if (res == 0)
                        isRecording = false;
                }

                if (currentMode.compare("video") == 0)
                    currentMode = "photo";
                else
                    currentMode = "video";*/
                m_pArduino->changeMode();
                currentMode = m_pArduino->getCurrentMode();
            }
            else if (arr[0] == "l" || arr[0] == "light") {
                if (arr[1] == "") {
                    std::cout << "Missing parameter: light intensity value missing." << std::endl;
                    waitKey(isToggleCLS);
                }
                else {
                    curr_intensityValue = stoi(arr[1]);
                    m_pArduino->setLight(curr_intensityValue); //LightUpdateValue(curr_intensityValue);
                } 
            }
            else if (arr[0] == "r" || arr[0] == "rotate") {
                if (arr[1] == "") {
                    std::cout << "Missing parameter: degree step value missing." << std::endl;
                    waitKey(isToggleCLS);
                }
                else if (arr[2] == "") {
                    std::cout << "Missing parameter: velocity value missing." << std::endl;
                    waitKey(isToggleCLS);
                }
                else {
                    
                    std::string command = "a ";
                    command.append(arr[1]);
                    command.append(" ");                
                    command.append(arr[2]);
                    command.append(" ");
                    command.append(arr[2]);

                   /* QString val = QString::fromStdString(command);
                    handSendCommand(QString("%1").arg(val));*/
                    m_pDevice->rotateHand(stoi(arr[1]), stoi(arr[2]));
                }
            }
            else if (arr[0] == "a" || arr[0] == "auto") {
                if (arr[1] == "") {
                    std::cout << "Missing parameter: degree step value missing." << std::endl;
                    waitKey(isToggleCLS);
                }
                else if (arr[2] == "") {
                    std::cout << "Missing parameter: velocity value missing." << std::endl;
                    waitKey(isToggleCLS);
                }
                else {
                    int degree_step = stoi(arr[1]);
                    int velocity = stoi(arr[2]);

                    m_pArduino->takePicture();//takePicture();                
                    delay(1000);

                    //clock-wise
                    turnHand(degree_step, velocity);

                    //back to initial position
                    /*std::string command = "a 0 500 500";
                    QString val = QString::fromStdString(command);
                    handSendCommand(QString("%1").arg(val));*/
                    m_pDevice->rotateHand(0, 500);

                    delay(-1);

                    //counter clock-wise
                    turnHand(-degree_step, velocity);

                    //back to initial position
                    /*command = "a 0 500 500";
                    val = QString::fromStdString(command);
                    handSendCommand(QString("%1").arg(val));*/
                    m_pDevice->rotateHand(0, 500);

                    delay(-1);
                    if (currentMode == "video")
                        m_pArduino->takePicture(); //takePicture();
                }
            }
            else {
                std::cout << "Command not recognized" << std::endl;
                waitKey(isToggleCLS);
            }
        }
    }

    resetAll();
}

void AcquisitionSetup::on_startArduino(){
    
    if (!m_pArduino->isConnected()) {
        arduino_is_available = false;
        m_pArduino->startConnection();
        //arduino_port_name = "";

        //serialBuffer_arduino = "";
        //arduino = new QSerialPort;

        //foreach(const QSerialPortInfo & serialPortInfo, QSerialPortInfo::availablePorts())
        //{
        //    if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
        //    {
        //        if (serialPortInfo.vendorIdentifier() == 4292)
        //        {
        //            if (serialPortInfo.productIdentifier() == 60000)
        //            {
        //                arduino_port_name = serialPortInfo.portName();
        //                arduino_is_available = true;
        //            }
        //        }
        //    }

        //    if (arduino_is_available)
        //    {
        //        // open and configure the port
        //        arduino->setPortName(arduino_port_name);
        //        arduino->open(QSerialPort::ReadWrite);
        //        arduino->setBaudRate(QSerialPort::Baud115200);
        //        arduino->setDataBits(QSerialPort::Data8);
        //        arduino->setParity(QSerialPort::NoParity);
        //        arduino->setStopBits(QSerialPort::OneStop);
        //        arduino->setFlowControl(QSerialPort::NoFlowControl);
        //        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(arduinoReadData()));
        //        QObject::connect(arduino, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));

        //        /*if (hand_is_available) {
        //            ui->RemoteActivator->setEnabled(true);
        //            ui->messageBox->setVisible(false);
        //            ui->dialogButton->setVisible(false);
        //            ui->retryButton->setVisible(false);
        //        }*/

        //        QString val = "on";
        //        arduinoUpdateValue(QString("%1").arg(val));

        //        LightUpdateValue(curr_intensityValue);

        //        isConnectedArduino = true;
        //        break;
        //    }
        //}

        if (!arduino_is_available) {
            // give error message
            std::cout << "Couldn't find the Arduino!" << std::endl;

            waitKey(isToggleCLS);
        }
    }
    
    if (!m_pDevice->isConnected()) {
        hand_is_available = false;
        m_pDevice->startConnection();
        //hand_port_name = "";

        //serialBuffer_hand = "";
        //hand_device = new QSerialPort;
        //foreach(const QSerialPortInfo & serialPortInfo, QSerialPortInfo::availablePorts())
        //{
        //    if (serialPortInfo.vendorIdentifier() == 6790)
        //    {
        //        if (serialPortInfo.productIdentifier() == 29986)
        //        {
        //            hand_port_name = serialPortInfo.portName();
        //            hand_is_available = true;
        //        }
        //    }

        //    if (hand_is_available)
        //    {
        //        //open and configure the port
        //        hand_device->setPortName(hand_port_name);
        //        std::cout << hand_port_name.toStdString() << std::endl;
        //        bool isopen = hand_device->open(QSerialPort::ReadWrite);
        //        hand_device->setBaudRate(QSerialPort::Baud115200);
        //        hand_device->setDataBits(QSerialPort::Data8);
        //        hand_device->setParity(QSerialPort::NoParity);
        //        hand_device->setStopBits(QSerialPort::OneStop);
        //        hand_device->setFlowControl(QSerialPort::NoFlowControl);
        //        hand_device->setReadBufferSize(0);

        //        QObject::connect(hand_device, SIGNAL(readyRead()), this, SLOT(handReadData()));
        //        QObject::connect(hand_device, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleErrorHand(QSerialPort::SerialPortError)));

        //        /*if(arduino_is_available){
        //            ui->RemoteActivator->setEnabled(true);
        //            ui->messageBox->setVisible(false);
        //            ui->dialogButton->setVisible(false);
        //            ui->retryButton->setVisible(false);
        //        }*/

        //        isConnectedHand = true;
        //        break;
        //    }         
        //}

        if (!hand_is_available) {
            // give error message
            std::cout << "Couldn't find the hand device!" << std::endl;

            waitKey(isToggleCLS);
        }
    } 
}

//void AcquisitionSetup::arduinoReadData(){
//    serialData_arduino = arduino->readAll();
//    serialBuffer_arduino += QString::fromStdString(serialData_arduino.toStdString());
//
//    int endlinePos = serialBuffer_arduino.indexOf("\r\n");
//    QString data = "";
//
//    if(endlinePos >= 0){
//        data = serialBuffer_arduino.first((endlinePos));
//
//        serialBuffer_arduino = serialBuffer_arduino.mid(endlinePos + 2);
//        manageSerialData(data);
//        //std::cout << data.toStdString().c_str() << std::endl;
//    }
//
//    //std::cout.flush() << std::endl;
//}

//void AcquisitionSetup::handReadData() {
//    //serialData_hand = hand_device->readLine(2000);
//    QString current_time = QDateTime::currentDateTime().toString();
//    
//    serialData_hand.append(hand_device->readAll());
//    //QList<QByteArray> tokens = serialData_hand.split(',');
//    speedDevice = 0; // tokens[1].toFloat();
//
//    /*handSteps = QString::fromStdString(data_splitted[0]);
//    finger1Sensor = QString::fromStdString(data_splitted[1]);
//    finger2Sensor = QString::fromStdString(data_splitted[2]);
//    finger3Sensor = QString::fromStdString(data_splitted[3]);
//    finger4Sensor = QString::fromStdString(data_splitted[4]);*/
//
//    //std::cout << data_splitted[0] << " " << data_splitted[1] << std::endl;
//
//    std::string info_time = current_time.toStdString() + " " + serialData_hand.toStdString();
//    outputDevice.push_back(info_time);
//}

//void AcquisitionSetup::handleError(QSerialPort::SerialPortError error)
//{
//    if (error == QSerialPort::ResourceError) {
//        isConnectedArduino = false;
//
//        resetAll();
//
//        // give error message
//        std::cout << "Couldn't find the Arduino!" << std::endl;
//    }
//}

//void AcquisitionSetup::handleErrorHand(QSerialPort::SerialPortError error)
//{
//    if (error == QSerialPort::ResourceError) {
//        isConnectedHand = false;
//
//        resetAll();
//
//        // give error message
//        std::cout << "Couldn't find the hand device!" << std::endl;
//    }
//}
//
//int AcquisitionSetup::handSendCommand(QString command) {
//
//    if (hand_device->isOpen() && hand_device->isWritable())
//    {
//        hand_device->write(command.toStdString().c_str());
//        hand_device->flush();
//        std::cout << "data has been send" << std::endl;
//        qDebug() << command;
//    }
//    else
//    {
//        std::cout << "Couldn't write to hand serial!" << std::endl;
//
//        isConnectedHand = false;
//        return -1;
//    }
//
//    return 0;
//}

//void AcquisitionSetup::manageSerialData(QString command){
//    if(command.contains("<TM>")){
//        QString cam_revealed = command.remove(0, command.indexOf("@") + 1);
//        bool found = false;
//
//        int i = 0;
//
//        while(i < 4 && !found){
//            if(camConnected[i].compare("") == 0){
//                camConnected[i] = cam_revealed;
//                n_camera_connected++;
//                found = true;
//            }
//            else if(camConnected[i].compare(cam_revealed) == 0)
//                found = true;
//            else
//                i++;
//        }
//    }
//    else if(command.contains("<st>")){
//        QString status = command.remove(0, command.indexOf("<st>") + 4);
//        status = status.remove(status.indexOf("</"), status.length() - status.indexOf("</"));
//        QStringList statusSplit = status.split(" ");
//
//        QString shutter = statusSplit[15];
//        int mode = statusSplit[14].toInt();
//
//        QString cam_mac = command.remove(0, command.indexOf("@") + 1);
//
//        for(int i = 0; i < 4; i++){
//            if(camConnected[i].compare(cam_mac) == 0){
//                if(shutter.compare("0") == 0){
//
//                    switch(mode){
//                        case 0:{
//                            camMode[i] = "Video";
//                            break;
//                        }
//                        case 1:{
//                            camMode[i] = "Photo";
//                            break;
//                        }
//                        case 2:{
//                            camMode[i] = "Burst";
//                            break;
//                        }
//                        case 3:{
//                            camMode[i] = "Timelapse";
//                            break;
//                        }
//                        case 7:{
//                            camMode[i] = "Settings";
//                            break;
//                        }
//                        default:{
//                            camMode[i] = "Unknown";
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    else if(command.contains("<BL>")){
//        QString battLevel = command.remove(0, command.indexOf("<BL>") + 4);
//        battLevel = battLevel.remove(battLevel.indexOf("</"), battLevel.length() - battLevel.indexOf("</"));
//
//        QString cam_mac = command.remove(0, command.indexOf("@") + 1);
//
//        for(int i = 0; i < 4; i++){
//            if(camConnected[i].compare(cam_mac) == 0)
//                camBL[i] = battLevel + "%";
//        }
//    }
//}
//
//void AcquisitionSetup::printInfoCamera(){
//    clearScreen(isToggleCLS);
//    for(int i = 0; i < 4; i++){
//        if(camConnected[i].compare("") != 0){
//            printf("Cam %i\n %s connected\n Mode: %s\n Battery level: %s\n", (i+1),
//                   camConnected[i].toStdString().c_str(), camMode[i].toStdString().c_str(), camBL[i].toStdString().c_str());
//            std::cout << std::endl;
//        }
//    }
//
//    printf("Hand step: %s\n Finger 1: %s\n Finger 2: %s\n Finger 3: %s\n Finger 4: %s\n",
//       handSteps.toStdString().c_str(), finger1Sensor.toStdString().c_str(),
//        finger2Sensor.toStdString().c_str(), finger3Sensor.toStdString().c_str(), finger4Sensor.toStdString().c_str());
//    std::cout << std::endl;
//
//    waitKey(isToggleCLS);
//}
//
//void AcquisitionSetup::takePicture(){
//
//    bool isVideoMode = false;
//
//    std::cout << "PHOTO" << std::endl;
//    for(int i = 0; i < 4; i++){
//        if(camConnected[i].compare("") != 0){
//            if(camMode[i].compare("Video") == 0)
//                isVideoMode = true;
//        }
//    }
//
//    if(isVideoMode){
//        //check if the camera is recording or not
//        if(!isRecording){
//            QString val = "start";
//            int res = arduinoUpdateValue(QString("%1").arg(val));
//            if(res == 0)
//               isRecording = true;
//        }
//        else{
//            QString val = "stop";
//            int res = arduinoUpdateValue(QString("%1").arg(val));
//            if(res == 0)
//               isRecording = false;
//        }
//    }
//    else{
//        QString val = "start";
//        arduinoUpdateValue(QString("%1").arg(val));
//    }
//
//    //take data from sensors
//}

//int AcquisitionSetup::arduinoUpdateValue(QString command){
//
//    if(arduino->isOpen() && arduino->isWritable())
//    {
//        std::cout << "data has been send" << std::endl;
//        qDebug() << command;
//        arduino->write(command.toStdString().c_str());
//        arduino->flush();
//    }
//    else
//    {
//        std::cout << "Couldn't write to serial!" << std::endl;
//
//        isConnectedArduino = false;
//        return -1;
//    }
//
//    return 0;
//}

//void AcquisitionSetup::LightUpdateValue(int value)
//{
//    std::cout << value << std::endl;
//    QString val = "light " + QString::number(value);
//    arduinoUpdateValue(QString("%1").arg(val));
//}

void AcquisitionSetup::resetAll(){
    /*if(isRecording){
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

    n_camera_connected = 0;*/
    arduino_is_available = false;
    hand_is_available = false;
    curr_intensityValue = 512;
   /* isConnectedArduino = false;
    isConnectedHand = false;*/
    currentMode = "video";
    //arduino->close();
    m_pThread->quit();
    m_pThread->wait();
    m_pThread_arduino->quit();
    m_pThread_arduino->wait();
    //hand_device->close();
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

void AcquisitionSetup::turnHand(int steps, int velocity) {
    QString val = "";
    std::string command = "";
    int shift = steps;
    for (int i = 0; i < 10; i++) {
        command = "a " + std::to_string(shift) + " " + std::to_string(velocity) + " " + std::to_string(velocity);
        
        /*std::cout << command << std::endl;
        val = QString::fromStdString(command);
        handSendCommand(QString("%1").arg(val));*/
        m_pDevice->rotateHand(shift, velocity);

        shift = shift + steps;

        delay(-1);
        if (currentMode == "photo") {
            m_pArduino->takePicture(); //takePicture();
            delay(1500);
        }    
    }
}

void AcquisitionSetup::printHelpGuide() {
    std::cout << "usage: [Command] [Optional arguments]\n"
        "Available command:\n"
        "p / play - Take picture or start/stop video recording.\n"
        "m / mode - Switching photo/video mode.\n"
        "l / light [light intensity value] - Set intensity light between [0, 1023] values. \n"
        "i / info - Show camera information.\n"
        "r / rotate [position] [velocity] - Move manually the hand device.\n"
        "a / auto [position] [velocity] - Start automatic acquisition process.\n"
        "q / quit - Exit by program.\n"
        "help - Open the guide." << std::endl;

    waitKey(isToggleCLS);
}

void AcquisitionSetup::delay(int n)
{
    int add_ms = n;
    if (add_ms < 0) {

        QTime dieTime = QTime::currentTime().addMSecs(250);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        while (m_pDevice->getSpeed() != 0) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }

        dieTime = QTime::currentTime().addMSecs(750);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    else {
        QTime dieTime = QTime::currentTime().addMSecs(add_ms);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }   
}
