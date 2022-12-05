#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <Arduino.h>
#include "TimeUtils.h"
#include <ArduinoJson.h>

class Sensors{
    private:
        TimeUtils *timeUtils;
        double temperature;
        double phValue;
        double lumen;
        String measurementTakenAt;
        String messageType;
        String serialNumber;

        void gatherSensorData();

    public:
        Sensors(String messageType, String serialNumber, TimeUtils *timeUtils);

        String returnAsJsonMessage();
};
#endif