#ifndef ACQUISITIONSETUP_H
#define ACQUISITIONSETUP_H

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QTime>
#include <QThread>
#include <QObject>

#include <handDevice.h>
#include <arduinodevice.h>

class AcquisitionSetup: public QObject
{
    Q_OBJECT

public:
    AcquisitionSetup();
    bool isActivate = false;
    //bool isRecording = false;
    bool isToggleCLS = true; //put TRUE/FALSE if you want enable/disable clear screen option.
    /*QString camConnected[4] = {"", "", "", ""};
    QString camMode[4] = {"-", "-", "-", "-"};
    QString camBL[4] = {"-", "-", "-", "-"};*/
    QString currentMode = "video";

    /*int speedDevice = 0;
    QString handSteps = "0";
    QString finger1Sensor = "4095";
    QString finger2Sensor = "4095";
    QString finger3Sensor = "4095";
    QString finger4Sensor = "4095";*/

    int curr_intensityValue = 512;

private slots:
    //void arduinoReadData();
    //void handReadData();
    //void handleError(QSerialPort::SerialPortError error);
    //void handleErrorHand(QSerialPort::SerialPortError error);
    void on_startArduino();

private:
    bool arduino_is_available;
    bool hand_is_available;
    /*bool isConnectedArduino = false;
    bool isConnectedHand = false;
    int n_camera_connected = 0;
    QSerialPort *arduino;
    QString arduino_port_name;
    QByteArray serialData_arduino;
    QString serialBuffer_arduino;*/

    //QSerialPort* hand_device;
    /*QString hand_port_name;
    QByteArray serialData_hand;
    QString serialBuffer_hand;*/

    QThread* m_pThread;
    HandDevice* m_pDevice;

    QThread* m_pThread_arduino;
    arduinoDevice* m_pArduino;

    void resetAll();
    /*int arduinoUpdateValue(QString command);
    int handSendCommand(QString command);
    void manageSerialData(QString command);
    void printInfoCamera();
    void takePicture();
    void LightUpdateValue(int value);*/
    void clearScreen(bool toggle);
    void waitKey(bool toggle);
    void turnHand(int steps, int velocity);

    void delay(int n);
    void printHelpGuide();

};
#endif // ACQUISITIONSETUP_H
