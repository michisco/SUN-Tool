#ifndef ACQUISITIONSETUP_H
#define ACQUISITIONSETUP_H

#include <QtSerialPort/QSerialPort>
#include <QObject>

class AcquisitionSetup: public QObject
{
    Q_OBJECT

public:
    AcquisitionSetup();
    bool isActivate = false;
    bool isRecording = false;
    QString camConnected[4] = {"", "", "", ""};
    QString camMode[4] = {"-", "-", "-", "-"};
    QString camBL[4] = {"-", "-", "-", "-"};
    QString currentMode = "video";
    int curr_intensityValue = 512;

private slots:
    void arduinoReadData();
    void handleError(QSerialPort::SerialPortError error);
    void on_startArduino();

private:
    bool arduino_is_available;
    bool isConnectedArduino = false;
    int n_camera_connected = 0;
    QSerialPort *arduino;
    QString arduino_port_name;
    QByteArray serialData_arduino;
    QString serialBuffer_arduino;
    void resetAll();
    int arduinoUpdateValue(QString command);
    int handSendCommand(QString command);
    void manageSerialData(QString command);
    void printInfoCamera();
    void takePicture();
    void LightUpdateValue(int value);
};
#endif // ACQUISITIONSETUP_H
