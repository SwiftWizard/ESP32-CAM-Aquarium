#ifndef LIGHT_CONTROL_H
#define LIGHT_CONTROL_H

#include <Arduino.h>
#include "TimeUtils.h"
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

class LightControl{
    private:
        int ledCount;
        Adafruit_NeoPixel strip;
        double red;
        double green;
        double blue;
        double brightness;
        TimeUtils *timeUtils;
        String lastLightChangedAt;
        String messageType;
        String serialNumber;

        String generateResponseMessage();

    public:
        LightControl(int ledControlPin, int ledCount, TimeUtils *timeUtils);
        void init();
        String setLight(double red, double green, double blue, double brightness);
};
#endif