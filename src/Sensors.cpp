#include "Sensors.h"
#include "TimeUtils.h"
#include <Arduino.h>
#include <ArduinoJson.h>

Sensors::Sensors(String messageType, String serialNumber, TimeUtils *timeUtils){
    this->messageType = messageType;
    this->serialNumber = serialNumber;
    this->timeUtils = timeUtils;
}


void Sensors::gatherSensorData(){
    this->temperature = 24.24;
    this->phValue = 7.1;
    this->lumen = -1;
    this->measurementTakenAt = timeUtils->getTime();
}

String Sensors::returnAsJsonMessage(){

    gatherSensorData();

    DynamicJsonDocument doc(256);
    doc["data"]["sensorReadoutTime"] = measurementTakenAt;
    doc["data"]["temperature"] = temperature;
    doc["data"]["phValue"] = phValue;
    doc["data"]["lumen"] = lumen; 
    doc["serialNumber"] = serialNumber;
    doc["messageType"] = messageType;

    String result;
    serializeJson(doc, result);
    result.replace("\"", "\\\"");
    return result;
    
}