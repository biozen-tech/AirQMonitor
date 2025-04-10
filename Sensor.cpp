#include "Sensor.hpp"

#include "config.h"
#include <esp_adc_cal.h>

bool Sensor::getSCD40MeasurementResult() {
    // Read Measurement  SCD40
    // uint16_t co2 = 0;
    // float temperature = 0.0f;
    // float humidity = 0.0f;
    bool isDataReady = false;
    uint16_t error = _scd4x.getDataReadyFlag(isDataReady);
    if (error) {
        errorToString(error, _errorMessage, 256);
        log_w("Error trying to execute getDataReadyFlag(): %s", _errorMessage);
        return false;
    }
    if (!isDataReady) {
        return false;
    }

    error = _scd4x.readMeasurement(scd40.co2, scd40.temperature, scd40.humidity);
    if (error) {
        errorToString(error, _errorMessage, 256);
        log_w("Error trying to execute readMeasurement(): %s", _errorMessage);
        return false;
    } else if (scd40.co2 == 0) {
        log_w("Invalid sample detected, skipping.");
        return false;
    } else {
        log_d("SCD40 Measurement Result:");
        log_d("  Co2: %d ppm", scd40.co2);
        log_d("  Temperature: %f °C", scd40.temperature);
        log_d("  Humidity: %f %RH", scd40.humidity);
        return true;
    }
    return false;
}


bool Sensor::getSEN55MeasurementResult() {
    // Read Measurement SEN55
    uint16_t error = _sen5x.readMeasuredValues(
        sen55.massConcentrationPm1p0,
        sen55.massConcentrationPm2p5,
        sen55.massConcentrationPm4p0,
        sen55.massConcentrationPm10p0,
        sen55.ambientHumidity,
        sen55.ambientTemperature,
        sen55.vocIndex,
        sen55.noxIndex
    );

    if (error) {
        errorToString(error, _errorMessage, 256);
        log_w("Error trying to execute readMeasuredValues(): %s", _errorMessage);
        return false;
    } else {
        log_d("SEN55 Measurement Result:");
        log_d("  PM1.0: %f µg/m³", sen55.massConcentrationPm1p0);
        log_d("  PM2.5: %f µg/m³", sen55.massConcentrationPm2p5);
        log_d("  PM4.0: %f µg/m³", sen55.massConcentrationPm4p0);
        log_d("  PM10.0: %f µg/m³", sen55.massConcentrationPm10p0);
        if (isnan(sen55.ambientHumidity)) {
            log_d("  AmbientHumidity: n/a");
        } else {
            log_d("  AmbientHumidity: %f %RH",sen55.ambientHumidity);
        }
        if (isnan(sen55.ambientTemperature)) {
            log_d("  AmbientTemperature: n/a");
        } else {
            log_d("  AmbientTemperature: %f °C", sen55.ambientTemperature);
        }

        if (isnan(sen55.vocIndex)) {
            log_d("  VOC Index: n/a");
        } else {
            log_d("  VOC Index: %f", sen55.vocIndex);
        }

        if (isnan(sen55.noxIndex)) {
            log_d("  NOx Index: n/a");
        } else {
            log_d("  NOx Index: %f", sen55.noxIndex);
        }
        return true;
    }
    return false;
}


void Sensor::getBatteryVoltageRaw() {
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_characterize(
        ADC_UNIT_1,
        ADC_ATTEN_DB_11,
        ADC_WIDTH_BIT_12,
        1100,
        &adc_chars
    );
    battery.raw = esp_adc_cal_raw_to_voltage(analogRead(BAT_ADC_PIN), &adc_chars);
}


void Sensor::getTimeString() {
    I2C_BM8563_TimeTypeDef time;
    _bm8563.getTime(&time);
    sprintf(this->time.time, "%02d:%02d", time.hours, time.minutes);
}


void Sensor::getDateString() {
    I2C_BM8563_DateTypeDef date;
    _bm8563.getDate(&date);
    sprintf(this->time.date, "%04d-%02d-%02d", date.year, date.month, date.date);
}

bool Sensor::getBme680MeasurementResult() {
    log_i("Starting BME688 measurement...");

    // Check if sensor is properly initialized
    if (_bme688.bsecStatus != BSEC_OK || _bme688.bme68xStatus != BME68X_OK) {
        log_e("BME688 sensor not properly initialized");
        return false;
    }

    if (_bme688.run()) { // If new data is available
        log_i("BME 688 measurement successful");

        // Validate the basic data before assigning
        if (isnan(_bme688.temperature) || isnan(_bme688.humidity) || 
            isnan(_bme688.pressure) || isnan(_bme688.gasResistance)) {
            log_e("BME688 returned invalid basic data");
            return false;
        }

        // Assign basic measurements
        bme680.temperature = _bme688.temperature;
        bme680.humidity = _bme688.humidity;
        bme680.pressure = _bme688.pressure * 0.0075006; // Convert Pa to mmHg
        bme680.gasResistance = _bme688.gasResistance;

        // Assign additional measurements with validation
        bme680.iaq = _bme688.iaq;
        bme680.staticIaq = _bme688.staticIaq;
        bme680.co2Equivalent = _bme688.co2Equivalent;
        bme680.breathVocEquivalent = _bme688.breathVocEquivalent;
        bme680.gasPercentage = _bme688.gasPercentage;
        bme680.iaqAccuracy = _bme688.iaqAccuracy;
        bme680.stabStatus = _bme688.stabStatus;
        bme680.runInStatus = _bme688.runInStatus;

        // Log all measurements
        log_i("BME688 Measurement Result:");
        log_i("  Temperature: %f °C", bme680.temperature);
        log_i("  Humidity: %f %RH", bme680.humidity);
        log_i("  Pressure: %f mmHg", bme680.pressure);
        log_i("  Gas Resistance: %f Ohm", bme680.gasResistance);
        log_i("  IAQ: %f (Accuracy: %d)", bme680.iaq, bme680.iaqAccuracy);
        log_i("  Static IAQ: %f", bme680.staticIaq);
        log_i("  CO2 Equivalent: %f ppm", bme680.co2Equivalent);
        log_i("  Breath VOC Equivalent: %f ppm", bme680.breathVocEquivalent);
        log_i("  Gas Percentage: %f %%", bme680.gasPercentage);
        log_i("  Stabilization Status: %d", bme680.stabStatus);
        log_i("  Run-in Status: %d", bme680.runInStatus);
        return true;
    }
    log_i("BME 688 measurement unsuccessful");
    return false;
}
