#ifndef HANDDEVICE_H
#define HANDDEVICE_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTime>

class HandDevice : public QObject
{
    Q_OBJECT
public:
    HandDevice(QObject *parent = 0);
    bool isConnected() const;
    int getSpeed();
    void rotateHand(int position, int velocity);
    void startConnection();

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void readData();

private:
    bool m_isConnected = false;
    bool is_available = false;
    QSerialPort* hand_device;
    QString hand_port_name;
    QByteArray serialData_hand;

    int speedDevice = 0;
    int handSteps = 0;
    int finger1Sensor = 0;
    int finger2Sensor = 0;
    int finger3Sensor = 0;
    int finger4Sensor = 0;

    void connectToDevice();
    int sendCommand(QString command);
};

#endif // HANDDEVICE_H
