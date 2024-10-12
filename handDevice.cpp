#include "handDevice.h"

#include <QThread>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

HandDevice::HandDevice(QObject *parent) : QObject(parent)
{
    hand_device = new QSerialPort(this);
    startConnection();
}

void HandDevice::startConnection() {
    foreach(const QSerialPortInfo & serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if (serialPortInfo.vendorIdentifier() == 6790)
        {
            if (serialPortInfo.productIdentifier() == 29986)
            {
                hand_port_name = serialPortInfo.portName();
                is_available = true;
            }
        }

        if (is_available)
        {
            //open and configure the port
            hand_device->setPortName(hand_port_name);
            //bool isopen = hand_device->open(QSerialPort::ReadWrite);
            hand_device->setBaudRate(QSerialPort::Baud115200);
            hand_device->setDataBits(QSerialPort::Data8);
            hand_device->setParity(QSerialPort::NoParity);
            hand_device->setStopBits(QSerialPort::OneStop);
            hand_device->setFlowControl(QSerialPort::NoFlowControl);
            //hand_device->setReadBufferSize(0);

            connectToDevice();
            break;
        }
    }
}

void HandDevice::connectToDevice(){
    if (hand_device->open(QSerialPort::ReadWrite)){
        m_isConnected = true;
        QObject::connect(hand_device, SIGNAL(readyRead()), this, SLOT(readData()));
        QObject::connect(hand_device, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    }
    else
        m_isConnected = false;
}

bool HandDevice::isConnected() const{
    return m_isConnected;
}

int HandDevice::getSpeed(){
    return speedDevice;
}

std::tuple<int, int, int, int> HandDevice::getFingerData() {
    return std::make_tuple(finger1Sensor, finger2Sensor, finger3Sensor, finger4Sensor);
}

std::tuple<int, int, int, int> HandDevice::getAvgFingerData() {
    int sum_finger1 = 0;
    int sum_finger2 = 0;
    int sum_finger3 = 0;
    int sum_finger4 = 0;
    int n = finger1Sensors.size();

    while (!finger1Sensors.empty()) {
        sum_finger1 += finger1Sensors.front();
        sum_finger2 += finger2Sensors.front();
        sum_finger3 += finger3Sensors.front();
        sum_finger4 += finger4Sensors.front();

        finger1Sensors.pop();
        finger2Sensors.pop();
        finger3Sensors.pop();
        finger4Sensors.pop();
    }
    return std::make_tuple((sum_finger1/n), (sum_finger2 / n), (sum_finger3 / n), (sum_finger4 / n));
}

void HandDevice::rotateHand(int position, int velocity){
    std::string command = "a " + std::to_string(position) + " " + std::to_string(velocity) + " " + std::to_string(velocity);

    std::cout << command << std::endl;
    QString val = QString::fromStdString(command);
    sendCommand(QString("%1").arg(val));
}

int HandDevice::sendCommand(QString command){
    if (hand_device->isOpen() && hand_device->isWritable())
    {
        hand_device->write(command.toStdString().c_str());
        hand_device->flush();
        std::cout << "data has been send" << std::endl;
    }
    else
    {
        std::cout << "Couldn't write to hand serial!" << std::endl;
        m_isConnected = false;
        return -1;
    }

    return 0;
}

void HandDevice::readData() {
    hand_device->waitForReadyRead(25);

    serialData_hand = hand_device->readLine(2000);
    QString current_time = QDateTime::currentDateTime().toString();

    QList<QByteArray> tokens = serialData_hand.split(',');
    speedDevice = tokens[5].toFloat();

    if (tokens[0].toInt() > 0) {
       /* finger1Sensor = tokens[0].toInt();
        finger2Sensor = tokens[1].toInt();
        finger3Sensor = tokens[2].toInt();
        finger4Sensor = tokens[3].toInt();*/

        if (finger1Sensors.size() > 10) {
            finger1Sensors.pop();
            finger2Sensors.pop();
            finger3Sensors.pop();
            finger4Sensors.pop();
        }

        finger1Sensors.push(tokens[0].toInt());
        finger2Sensors.push(tokens[1].toInt());
        finger3Sensors.push(tokens[2].toInt());
        finger4Sensors.push(tokens[3].toInt());
    }
        
    /*std::cout << finger1Sensor << ", "
        << finger2Sensor << ", "
        << finger3Sensor << ", "
        << finger4Sensor << std::endl;*/
}

void HandDevice::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        m_isConnected = false;

        //resetAll();

        // give error message
        std::cout << "Couldn't find the hand device!" << std::endl;
    }
}
