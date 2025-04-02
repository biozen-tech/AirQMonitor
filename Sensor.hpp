#pragma once

#include <stdint.h>
#include "I2C_BM8563.h"
#include <SensirionI2CScd4x.h>
#include <SensirionI2CSen5x.h>
#include <bsec.h>


class Sensor
{
public:
    Sensor(SensirionI2CScd4x &scd4x, SensirionI2CSen5x &sen5x, I2C_BM8563 &bm8563, Bsec &bme688):
        _scd4x(scd4x),
        _sen5x(sen5x),
        _bm8563(bm8563),
        _bme688(bme688) {}

    ~Sensor() {}

    bool getSCD40MeasurementResult();
    bool getSEN55MeasurementResult();
    bool getBme680MeasurementResult();
    void getBatteryVoltageRaw();
    void getTimeString();
    void getDateString();

public:
   struct {
        float massConcentrationPm1p0;
        float massConcentrationPm2p5;
        float massConcentrationPm4p0;
        float massConcentrationPm10p0;
        float ambientHumidity;
        float ambientTemperature;
        float vocIndex;
        float noxIndex;
    } sen55;
    struct {
        uint16_t co2;
        float temperature;
        float humidity;
    } scd40;

    struct {
        float temperature;
        float humidity;
        float pressure;
        float gasResistance;
        float iaq;                    // Indoor Air Quality
        float staticIaq;              // Static Indoor Air Quality
        float co2Equivalent;          // CO2 equivalent
        float breathVocEquivalent;    // Breath VOC equivalent
        float gasPercentage;          // Gas percentage
        uint8_t iaqAccuracy;          // IAQ accuracy
        uint8_t stabStatus;           // Stabilization status
        uint8_t runInStatus;          // Run-in status
    } bme680;

    struct {
        uint32_t raw;
    } battery;

    struct {
        char time[10];
        char date[17];
    } time;

private:
    SensirionI2CScd4x &_scd4x;
    SensirionI2CSen5x &_sen5x;
    I2C_BM8563 &_bm8563;
    Bsec &_bme688;

    char _errorMessage[256];
};

extern Sensor sensor;
