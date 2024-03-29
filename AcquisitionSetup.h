#ifndef ACQUISITIONSETUP_H
#define ACQUISITIONSETUP_H

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QThread>
#include <QObject>

class AcquisitionSetup: public QObject
{
    Q_OBJECT

public:
    AcquisitionSetup();
    bool isActivate = false;
    bool isRecording = false;
    bool isToggleCLS = true; //put TRUE/FALSE if you want enable/disable clear screen option.
    QString camConnected[4] = {"", "", "", ""};
    QString camMode[4] = {"-", "-", "-", "-"};
    QString camBL[4] = {"-", "-", "-", "-"};
    QString currentMode = "video";

    QString handSteps = "0";
    QString finger1Sensor = "4095";
    QString finger2Sensor = "4095";
    QString finger3Sensor = "4095";
    QString finger4Sensor = "4095";

    int curr_intensityValue = 512;

private slots:
    void arduinoReadData();
    void handReadData();
    void handleError(QSerialPort::SerialPortError error);
    void handleErrorHand(QSerialPort::SerialPortError error);
    void on_startArduino();

private:
    bool arduino_is_available;
    bool hand_is_available;
    bool isConnectedArduino = false;
    bool isConnectedHand = false;
    int n_camera_connected = 0;
    QSerialPort *arduino;
    QString arduino_port_name;
    QByteArray serialData_arduino;
    QString serialBuffer_arduino;

    QSerialPort* hand_device;
    QString hand_port_name;
    QByteArray serialData_hand;
    QString serialBuffer_hand;

    QThread m_workerThread;
    QTimer m_myTimer;

    void resetAll();
    int arduinoUpdateValue(QString command);
    int handSendCommand(QString command);
    void manageSerialData(QString command);
    void printInfoCamera();
    void takePicture();
    void LightUpdateValue(int value);
    void clearScreen(bool toggle);
    void waitKey(bool toggle);
};
#endif // ACQUISITIONSETUP_H
