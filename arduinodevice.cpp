#include "arduinodevice.h"

#include <QThread>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

arduinoDevice::arduinoDevice(QObject *parent) : QObject(parent)
{
    arduino_device = new QSerialPort(this);
    startConnection();
}

void arduinoDevice::startConnection() {
    foreach(const QSerialPortInfo & serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
        {
            if (serialPortInfo.vendorIdentifier() == 4292)
            {
                if (serialPortInfo.productIdentifier() == 60000)
                {
                    arduino_port_name = serialPortInfo.portName();
                    is_available = true;
                }
            }
        }

        if (is_available)
        {
            // open and configure the port
            arduino_device->setPortName(arduino_port_name);
            //arduino->open(QSerialPort::ReadWrite);
            arduino_device->setBaudRate(QSerialPort::Baud115200);
            arduino_device->setDataBits(QSerialPort::Data8);
            arduino_device->setParity(QSerialPort::NoParity);
            arduino_device->setStopBits(QSerialPort::OneStop);
            arduino_device->setFlowControl(QSerialPort::NoFlowControl);

            connectToDevice();
            break;
        }
    }
}

void arduinoDevice::connectToDevice(){
    if (arduino_device->open(QSerialPort::ReadWrite)){
        QObject::connect(arduino_device, SIGNAL(readyRead()), this, SLOT(readData()));
        QObject::connect(arduino_device, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));

        QString val = "on";
        sendCommand(QString("%1").arg(val));

        setLight(512);
        m_isConnected = true;
    }
    else
        m_isConnected = false;
}

bool arduinoDevice::isConnected() const{
    return m_isConnected;
}

void arduinoDevice::setLight(int value) {
    std::cout << value << std::endl;
    QString val = "light " + QString::number(value);
    sendCommand(QString("%1").arg(val));
}

void arduinoDevice::changeMode() {
    if (isRecording) {
        QString val = "stop";
        int res = sendCommand(QString("%1").arg(val));
        if (res == 0)
            isRecording = false;
    }

    if (currentMode.compare("video") == 0)
        currentMode = "photo";
    else
        currentMode = "video";
    int res = sendCommand(QString("%1").arg(currentMode));
}

void arduinoDevice::takePicture() {
    bool isVideoMode = false;

    std::cout << "PHOTO" << std::endl;
    for (int i = 0; i < 4; i++) {
        if (camConnected[i].compare("") != 0) {
            if (camMode[i].compare("Video") == 0)
                isVideoMode = true;
        }
    }

    if (isVideoMode) {
        //check if the camera is recording or not
        if (!isRecording) {
            QString val = "start";
            int res = sendCommand(QString("%1").arg(val));
            if (res == 0)
                isRecording = true;
        }
        else {
            QString val = "stop";
            int res = sendCommand(QString("%1").arg(val));
            if (res == 0)
                isRecording = false;
        }
    }
    else {
        QString val = "start";
        sendCommand(QString("%1").arg(val));
    }

    //take data from sensors
}

void arduinoDevice::printInfoCamera() {
    for (int i = 0; i < 4; i++) {
        if (camConnected[i].compare("") != 0) {
            printf("Cam %i\n %s connected\n Mode: %s\n Battery level: %s\n", (i + 1),
                camConnected[i].toStdString().c_str(), camMode[i].toStdString().c_str(), camBL[i].toStdString().c_str());
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void arduinoDevice::manageSerialData(QString buffer) {
    if (buffer.contains("<TM>")) {
        QString cam_revealed = buffer.remove(0, buffer.indexOf("@") + 1);
        bool found = false;

        int i = 0;

        while (i < 4 && !found) {
            if (camConnected[i].compare("") == 0) {
                camConnected[i] = cam_revealed;
                //n_camera_connected++;
                found = true;
            }
            else if (camConnected[i].compare(cam_revealed) == 0)
                found = true;
            else
                i++;
        }
    }
    else if (buffer.contains("<st>")) {
        QString status = buffer.remove(0, buffer.indexOf("<st>") + 4);
        status = status.remove(status.indexOf("</"), status.length() - status.indexOf("</"));
        QStringList statusSplit = status.split(" ");

        QString shutter = statusSplit[15];
        int mode = statusSplit[14].toInt();

        QString cam_mac = buffer.remove(0, buffer.indexOf("@") + 1);

        for (int i = 0; i < 4; i++) {
            if (camConnected[i].compare(cam_mac) == 0) {
                if (shutter.compare("0") == 0) {

                    switch (mode) {
                    case 0: {
                        camMode[i] = "Video";
                        break;
                    }
                    case 1: {
                        camMode[i] = "Photo";
                        break;
                    }
                    case 2: {
                        camMode[i] = "Burst";
                        break;
                    }
                    case 3: {
                        camMode[i] = "Timelapse";
                        break;
                    }
                    case 7: {
                        camMode[i] = "Settings";
                        break;
                    }
                    default: {
                        camMode[i] = "Unknown";
                        break;
                    }
                    }
                }
            }
        }
    }
    else if (buffer.contains("<BL>")) {
        QString battLevel = buffer.remove(0, buffer.indexOf("<BL>") + 4);
        battLevel = battLevel.remove(battLevel.indexOf("</"), battLevel.length() - battLevel.indexOf("</"));

        QString cam_mac = buffer.remove(0, buffer.indexOf("@") + 1);

        for (int i = 0; i < 4; i++) {
            if (camConnected[i].compare(cam_mac) == 0)
                camBL[i] = battLevel + "%";
        }
    }
}

void arduinoDevice::readData() {
    serialData_arduino = arduino_device->readAll();
    serialBuffer_arduino += QString::fromStdString(serialData_arduino.toStdString());

    int endlinePos = serialBuffer_arduino.indexOf("\r\n");
    QString data = "";

    if (endlinePos >= 0) {
        data = serialBuffer_arduino.first((endlinePos));

        serialBuffer_arduino = serialBuffer_arduino.mid(endlinePos + 2);
        manageSerialData(data);
    }
}

int arduinoDevice::sendCommand(QString command) {
    if (arduino_device->isOpen() && arduino_device->isWritable())
    {
        std::cout << "data has been send" << std::endl;
        arduino_device->write(command.toStdString().c_str());
        arduino_device->flush();
    }
    else
    {
        std::cout << "Couldn't write to serial!" << std::endl;

        m_isConnected = false;
        return -1;
    }

    return 0;
}

void arduinoDevice::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        m_isConnected = false;

        //resetAll();

        // give error message
        std::cout << "Couldn't find the arduino device!" << std::endl;
    }
}

QString arduinoDevice::getCamConnected(int index) {
    return camConnected[index];
}

QString arduinoDevice::getCamBL(int index) {
    return camBL[index];
}

QString arduinoDevice::getCamMode(int index) {
    return camMode[index];
}

QString arduinoDevice::getCurrentMode() {
    return currentMode;
}
