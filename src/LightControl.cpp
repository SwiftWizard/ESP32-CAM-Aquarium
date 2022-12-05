#include <Arduino.h>
#include "LightControl.h"
#include "TimeUtils.h"
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

LightControl::LightControl(int ledControlPin, int ledCount, TimeUtils *timeUtils){
    this->timeUtils = timeUtils;
    this->ledCount = ledCount;
    this->strip = Adafruit_NeoPixel(ledControlPin, ledCount, NEO_GRB + NEO_KHZ800);
}

void LightControl::init(){
    this->strip.begin();
    this->strip.clear();
    this->strip.show();
}

String LightControl::generateResponseMessage(){
    DynamicJsonDocument doc(256);
    doc["data"]["lightModifiedAt"] = lastLightChangedAt;
    doc["data"]["red"] = red;
    doc["data"]["green"] = green;
    doc["data"]["blue"] = blue;
    doc["data"]["brightness"] = brightness; 
    doc["serialNumber"] = serialNumber;
    doc["messageType"] = messageType;

    String result;
    serializeJson(doc, result);
    result.replace("\"", "\\\"");
    return result;
}
 
String LightControl::setLight(double red, double green, double blue, double brightness){
    this->strip.setBrightness(brightness);

    for(int i=0; i < this->ledCount; i++){
        strip.setPixelColor(i, strip.Color(red, green, blue));
    }

    this->lastLightChangedAt = this->timeUtils->getTime();

    return generateResponseMessage();
}