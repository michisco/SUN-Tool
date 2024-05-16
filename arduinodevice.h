#ifndef ARDUINODEVICE_H
#define ARDUINODEVICE_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTime>

class arduinoDevice : public QObject
{
    Q_OBJECT
public:
    arduinoDevice(QObject* parent = 0);
    bool isConnected() const;
    void startConnection();
    void setLight(int value);
    void takePicture();
    void printInfoCamera();
    void changeMode();
    QString getCamConnected(int index);
    QString getCamMode(int index);
    QString getCamBL(int index);
    QString getCurrentMode();

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void readData();

private:
    bool m_isConnected = false;
    bool is_available = false;
    bool isRecording = false;
    QSerialPort* arduino_device;
    QString arduino_port_name;
    QByteArray serialData_arduino;
    QString serialBuffer_arduino;

    QString camConnected[4] = {"", "", "", ""};
    QString camMode[4] = {"-", "-", "-", "-"};
    QString camBL[4] = {"-", "-", "-", "-"};
    QString currentMode = "video";

    void connectToDevice();
    void manageSerialData(QString command);
    int sendCommand(QString command);
};

#endif // ARDUINODEVICE_H
