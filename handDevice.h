#ifndef HANDDEVICE_H
#define HANDDEVICE_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTime>

#include <tuple>
#include <queue>

class HandDevice : public QObject
{
    Q_OBJECT
public:
    HandDevice(QObject *parent = 0);
    bool isConnected() const;
    int getSpeed();
    std::tuple<int, int, int, int> getFingerData();
    std::tuple<int, int, int, int> getAvgFingerData();
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
    int finger1Sensor;
    int finger2Sensor;
    int finger3Sensor;
    int finger4Sensor;

    std::queue<int> finger1Sensors;
    std::queue<int> finger2Sensors;
    std::queue<int> finger3Sensors;
    std::queue<int> finger4Sensors;

    void connectToDevice();
    int sendCommand(QString command);
};

#endif // HANDDEVICE_H
