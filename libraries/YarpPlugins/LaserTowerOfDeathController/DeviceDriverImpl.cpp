// Authors: see AUTHORS.md at project root.
// CopyPolicy: released under the terms of the LGPLv2.1, see LICENSE at project root.
// URL: https://github.com/asrob-uc3m/yarp-devices

#include "LaserTowerOfDeathController.hpp"

namespace asrob
{

bool LaserTowerOfDeathController::open(yarp::os::Searchable& config)
{

    std::string serialPortName = config.check("serialPortName",yarp::os::Value(DEFAULT_SERIAL_PORT_NAME),"serialPortName").asString();

    printf("LaserTowerOfDeathController options:\n");
    printf("\t--serialPortName %s [%s]\n",serialPortName.c_str(),DEFAULT_SERIAL_PORT_NAME);

    CD_DEBUG("init Serial Port.\n");
    serialPort = new SerialPort( serialPortName );  // "/dev/ttyUSB0"
    try
    {
        serialPort->Open( SerialPort::BAUD_57600, SerialPort::CHAR_SIZE_8,
                           SerialPort::PARITY_NONE, SerialPort::STOP_BITS_1,
                           SerialPort::FLOW_CONTROL_NONE );
    }
    catch ( SerialPort::OpenFailed e )
    {
        CD_ERROR("Error opening the serial port: %s\n", serialPortName.c_str());
        return false;
    }

    if ( ! checkConnection() )
    {
        CD_ERROR("Error communicating with the robot. Exiting...\n");
        return false;
    }
    CD_SUCCESS("Ok Serial Port: %p\n",serialPort);

    panJointValue = panInitial;
    tiltJointValue = tiltInitial;

    return true;
}

bool LaserTowerOfDeathController::close()
{
    CD_INFO("Close Serial Port: %p\n",serialPort);
    serialPort->Close();
    delete serialPort;
    serialPort = 0;

    return true;
}

}  // namespace asrob
