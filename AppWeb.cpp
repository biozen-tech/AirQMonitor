#include "AppWeb.hpp"

#include <WebServer.h>
#include <cJSON.h>
#include <LittleFS.h>
#include <WString.h>

#include <SensirionI2CScd4x.h>
#include <SensirionI2CSen5x.h>
#include <Bsec.h>

#include "config.h"
#include "DataBase.hpp"

WebServer server(80);
static TaskHandle_t webTaskHandler;
bool webServerState = false;

extern SensirionI2CScd4x scd4x;
extern SensirionI2CSen5x sen5x;
extern Bsec bme688;

static void postWiFiConnect();
static void getWiFiStatus();
static void getWiFiList();
static void postEzDataConfig();
static void getStatus();
static void getInfo();
static void getConfig();
static void postConfig();
static void postAPControl();
static void webTask(void *);

static bool getSCD40MeasurementResult(SensirionI2CScd4x& scd4x, uint16_t& co2, float& temperature, float& humidity);
static bool getBME688MeasurementResult(Bsec& bme688, float& temperature, float& humidity, float& pressure, 
                                  float& gasResistance, float& iaq, float& staticIaq, float& co2Equivalent,
                                  float& breathVocEquivalent, float& gasPercentage, uint8_t& iaqAccuracy,
                                  uint8_t& stabStatus, uint8_t& runInStatus);

void appWebServer(void) {
    log_i("=== Starting Web Server Setup ===");

    if (webServerState == true) {
        log_w("Web server already running");
        return;
    }

    log_i("Setting up static file serving");
    server.serveStatic("/", FILESYSTEM, "/www/index.html");

    log_i("Registering API endpoints");
    server.on("/api/v1/wifi_connect", HTTP_POST, postWiFiConnect);
    server.on("/api/v1/wifi_status", HTTP_GET, getWiFiStatus);
    server.on("/api/v1/wifi_list", HTTP_GET, getWiFiList);
    server.on("/api/v1/ezdata_config", HTTP_POST, postEzDataConfig);
    server.on("/api/v1/status", HTTP_GET, getStatus);
    server.on("/api/v1/info", HTTP_GET, getInfo);
    server.on("/api/v1/config", HTTP_GET, getConfig);
    server.on("/api/v1/config", HTTP_POST, postConfig);
    server.on("/api/v1/ap_control", HTTP_POST, postAPControl);

    server.onNotFound([]() {
        log_w("404 Not Found: %s", server.uri().c_str());
        server.send(404, "text/plain", "FileNotFound");
    });

    log_i("Starting web server");
    server.begin();
    webServerState = true;

    log_i("Creating web server task");
    if (xTaskCreatePinnedToCore(webTask, "webTask", 8192, NULL, 5, &webTaskHandler, APP_CPU_NUM) != pdPASS) {
        log_e("Failed to create web server task");
        webServerState = false;
        return;
    }

    log_i("=== Web Server Setup Complete ===");
}

void appWebServerClose(void) {
    log_i("=== Shutting Down Web Server ===");

    if (!webServerState) {
        log_w("Web server not running");
        return;
    }

    log_i("Closing server connection");
    server.close();
    webServerState = false;

    log_i("Deleting web server task");
    vTaskDelete(webTaskHandler);

    log_i("=== Web Server Shutdown Complete ===");
}

static void webTask(void *) {
    log_i("Web server task started");

    for (;;) {
        server.handleClient();
        delay(10);
    }

    log_i("Web server task ended");
    vTaskDelete(NULL);
}

static void postWiFiConnect() {
    cJSON *reqObject = NULL;
    cJSON *wifiObject = NULL;
    bool flag = false;

    String content = server.arg("plain");
    log_d("POST /api/v1/wifi_connect content: '%s'", content.c_str());
    reqObject = cJSON_Parse(content.c_str());
    if (reqObject == NULL) {
        log_w("JSON parse error");
        log_w("payload: '%s'", server.arg("plain").c_str());
        return;
    }

    wifiObject = cJSON_GetObjectItem(reqObject, "wifi");
    if (wifiObject) {
        cJSON *ssidObject = cJSON_GetObjectItem(wifiObject, "ssid");
        if (ssidObject != NULL) {
            db.wifi.ssid = ssidObject->valuestring;
            flag = true;
        }
        cJSON *pskObject = cJSON_GetObjectItem(wifiObject, "password");
        if (ssidObject != NULL) {
            db.wifi.password = pskObject->valuestring;
            flag = true;
        }
    }

    db.factoryState = false;
    db.saveToFile();

    if (flag || WiFi.isConnected() != true) {
        WiFi.disconnect();
        delay(200);
        WiFi.begin(db.wifi.ssid.c_str(), db.wifi.password.c_str());
        log_i("WiFi connect ...");
        db.isConfigState = true;
    }

    server.send(200, "application/json", content);
    log_d("POST /api/v1/wifi_connect response: '%s'", content.c_str());
    return;
}

static void getWiFiStatus() {
    cJSON *rspObject = NULL;
    char *str = NULL;

    rspObject = cJSON_CreateObject();
    if (rspObject == NULL) {
        return;
    }

    String mac = WiFi.macAddress();
    if (WiFi.status() == WL_CONNECTED) {
        cJSON_AddBoolToObject(rspObject, "status", true);
    } else {
        cJSON_AddBoolToObject(rspObject, "status", false);
    }
    cJSON_AddStringToObject(rspObject, "mac", mac.c_str());
    cJSON_AddStringToObject(rspObject, "ip", WiFi.localIP().toString().c_str());
    cJSON_AddBoolToObject(rspObject, "psk_status", db.pskStatus);

    str = cJSON_Print(rspObject);
    log_d("GET /api/v1/wifi_status response: '%s'", str);
    server.send(200, "application/json", str);

    free(str);
    cJSON_Delete(rspObject);
    return;
}

static void getWiFiList() {
    cJSON *rspObject = NULL;
    cJSON *apListObject = NULL;
    int n = 0;
    char *str = NULL;

    rspObject = cJSON_CreateObject();
    if (rspObject == NULL) {
        return ;
    }
    apListObject = cJSON_CreateArray();
    if (apListObject == NULL) {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "ap_list", apListObject);

    log_i("WiFi Scan start");
    n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        log_i("%s", WiFi.SSID(i).c_str());
        cJSON_AddItemToArray(apListObject, cJSON_CreateString(WiFi.SSID(i).c_str()));
    }
    log_i("WiFi Scan done");
    WiFi.scanDelete();
    str = cJSON_Print(rspObject);
    server.send(200, "application/json", str);
    log_d("GET /api/v1/wifi_list response: '%s'", str);
OUT:
    free(str);
    cJSON_Delete(rspObject);
    return ;
}

static void postEzDataConfig() {
    cJSON *reqObject = NULL;
    cJSON *ezdataObject = NULL;
    cJSON *rspObject = NULL;

    String content = server.arg("plain");
    log_d("POST /api/v1/ezdata_config content: '%s'", server.arg("plain").c_str());
    reqObject = cJSON_Parse(content.c_str());
    if (reqObject == NULL) {
        log_w("JSON parse error");
        log_w("payload: '%s'", server.arg("plain").c_str());
        return;
    }

    ezdataObject = cJSON_GetObjectItem(reqObject, "ezdata2");
    if (ezdataObject) {
        cJSON *tokenObject = cJSON_GetObjectItem(ezdataObject, "dev_token");
        db.ezdata2.devToken = tokenObject->valuestring;
        db.factoryState = false;
    }

    server.send(200, "application/json", server.arg("plain"));
    log_d("POST /api/v1/ezdata_config response: '%s'", server.arg("plain").c_str());

    db.saveToFile();

    cJSON_Delete(reqObject);
    cJSON_Delete(rspObject);
    return;
}

static void getStatus() {
    cJSON *rspObject = NULL;
    cJSON *sen55Object = NULL;
    cJSON *scd40Object = NULL;
    cJSON *bme688Object = NULL;
    char *str = NULL;

    rspObject = cJSON_CreateObject();
    if (rspObject == NULL) {
        goto OUT1;
    }

    sen55Object = cJSON_CreateObject();
    if (sen55Object == NULL) {
        goto OUT;
    }

    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;

    sen5x.readMeasuredValues(
        massConcentrationPm1p0,
        massConcentrationPm2p5,
        massConcentrationPm4p0,
        massConcentrationPm10p0,
        ambientHumidity,
        ambientTemperature,
        vocIndex,
        noxIndex
    );
    cJSON_AddItemToObject(rspObject, "sen55", sen55Object);
    cJSON_AddNumberToObject(sen55Object, "pm1.0", massConcentrationPm1p0);
    cJSON_AddNumberToObject(sen55Object, "pm2.5", massConcentrationPm2p5);
    cJSON_AddNumberToObject(sen55Object, "pm4.0", massConcentrationPm4p0);
    cJSON_AddNumberToObject(sen55Object, "pm10.0", massConcentrationPm10p0);
    cJSON_AddNumberToObject(sen55Object, "humidity", ambientHumidity);
    cJSON_AddNumberToObject(sen55Object, "temperature", ambientTemperature);
    cJSON_AddNumberToObject(sen55Object, "voc", vocIndex);
    cJSON_AddNumberToObject(sen55Object, "nox", noxIndex);

    scd40Object = cJSON_CreateObject();
    if (scd40Object == NULL) {
        goto OUT;
    }
    uint16_t co2;
    float humidity;
    float temperature;
    getSCD40MeasurementResult(scd4x, co2, humidity, temperature);
    cJSON_AddItemToObject(rspObject, "scd40", scd40Object);
    cJSON_AddNumberToObject(scd40Object, "co2", co2);
    cJSON_AddNumberToObject(scd40Object, "humidity", humidity);
    cJSON_AddNumberToObject(scd40Object, "temperature", temperature);

    bme688Object = cJSON_CreateObject();
    if (bme688Object == NULL) {
        goto OUT;
    }
    float bme688Temp, bme688Humi, bme688Press, bme688Gas;
    float bme688Iaq, bme688StaticIaq, bme688Co2Eq, bme688BreathVoc, bme688GasPercentage;
    uint8_t bme688IaqAccuracy, bme688StabStatus, bme688RunInStatus;

    getBME688MeasurementResult(bme688, bme688Temp, bme688Humi, bme688Press, bme688Gas,
                              bme688Iaq, bme688StaticIaq, bme688Co2Eq, bme688BreathVoc,
                              bme688GasPercentage, bme688IaqAccuracy, bme688StabStatus, bme688RunInStatus);

    cJSON_AddItemToObject(rspObject, "bme688", bme688Object);

    // Add basic measurements
    cJSON_AddNumberToObject(bme688Object, "temperature", bme688Temp);
    cJSON_AddNumberToObject(bme688Object, "humidity", bme688Humi);
    cJSON_AddNumberToObject(bme688Object, "pressure", bme688Press);
    cJSON_AddNumberToObject(bme688Object, "gasResistance", bme688Gas);

    // Add additional measurements
    cJSON_AddNumberToObject(bme688Object, "iaq", bme688Iaq);
    cJSON_AddNumberToObject(bme688Object, "iaq_accuracy", bme688IaqAccuracy);
    cJSON_AddNumberToObject(bme688Object, "static_iaq", bme688StaticIaq);
    cJSON_AddNumberToObject(bme688Object, "co2_equivalent", bme688Co2Eq);
    cJSON_AddNumberToObject(bme688Object, "breath_voc_equivalent", bme688BreathVoc);
    cJSON_AddNumberToObject(bme688Object, "gas_percentage", bme688GasPercentage);
    cJSON_AddNumberToObject(bme688Object, "stabilization_status", bme688StabStatus);
    cJSON_AddNumberToObject(bme688Object, "run_in_status", bme688RunInStatus);

    struct tm timeinfo;
    getLocalTime(&timeinfo, 1000);
    cJSON_AddStringToObject(rspObject, "time", asctime((const struct tm *)&timeinfo));
    cJSON_AddNumberToObject(rspObject, "interval", db.rtc.sleepInterval);

    str = cJSON_Print(rspObject);
    server.send(200, "application/json", str);
    log_d("GET /api/v1/status response: '%s'", str);
OUT:
    free(str);
    cJSON_Delete(rspObject);
OUT1:
    return;
}

static void getInfo() {
    cJSON *rspObject = NULL;
    cJSON *sysObject = NULL;
    cJSON *archObject = NULL;
    cJSON *memObject = NULL;
    cJSON *fsObject = NULL;
    cJSON *apObject = NULL;
    cJSON *staObject = NULL;
    char *str = NULL;

    rspObject = cJSON_CreateObject();
    if (rspObject == NULL) {
        goto OUT1;
    }

    sysObject = cJSON_CreateObject();
    if (sysObject == NULL) {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "sys", sysObject);
    cJSON_AddStringToObject(sysObject, "model", "M5STACK AirQ");
    cJSON_AddStringToObject(sysObject, "fw", APP_VERSION);
    cJSON_AddStringToObject(sysObject, "sdk", ESP.getSdkVersion());
    archObject = cJSON_CreateObject();
    if (archObject == NULL) {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "arch", archObject);
    cJSON_AddStringToObject(archObject, "mfr", "Espressif");
    cJSON_AddStringToObject(archObject, "model", ESP.getChipModel());
    cJSON_AddNumberToObject(archObject, "revision", ESP.getChipRevision());
    if (!strncmp(ESP.getChipModel(), "ESP32-S3", strlen("ESP32-S3"))) {
        cJSON_AddStringToObject(archObject, "cpu", "XTensa® dual-core LX7");
    } else if (!strncmp(ESP.getChipModel(), "ESP32-S2", strlen("ESP32-S2"))) {
        cJSON_AddStringToObject(archObject, "cpu", "XTensa® single-core LX7");
    } else if (!strncmp(ESP.getChipModel(), "ESP32-C3", strlen("ESP32-C3"))) {
        cJSON_AddStringToObject(archObject, "cpu", "RISC-V");
    } else if (!strncmp(ESP.getChipModel(), "ESP32", strlen("ESP32"))) {
        cJSON_AddStringToObject(archObject, "cpu", "XTensa® dual-core LX6");
    }
    cJSON_AddNumberToObject(archObject, "freq", ESP.getCpuFreqMHz());

    memObject = cJSON_CreateObject();
    if (memObject == NULL) {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "mem", memObject);
    cJSON_AddNumberToObject(memObject, "total", ESP.getHeapSize());
    cJSON_AddNumberToObject(memObject, "free", ESP.getFreeHeap());

    fsObject = cJSON_CreateObject();
    if (fsObject == NULL) {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "fs", fsObject);
    cJSON_AddNumberToObject(fsObject, "total", FILESYSTEM.totalBytes());
    cJSON_AddNumberToObject(fsObject, "used", FILESYSTEM.usedBytes());
    cJSON_AddNumberToObject(fsObject, "free", FILESYSTEM.totalBytes() - FILESYSTEM.usedBytes());

    apObject = cJSON_CreateObject();
    if (apObject == NULL) {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "ap", apObject);
    cJSON_AddStringToObject(apObject, "ssid", WiFi.softAPSSID().c_str());
    cJSON_AddNumberToObject(apObject, "num", WiFi.softAPgetStationNum());

    staObject = cJSON_CreateObject();
    if (staObject == NULL) {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "sta", staObject);
    cJSON_AddStringToObject(staObject, "ssid", db.wifi.ssid.c_str());
    cJSON_AddStringToObject(staObject, "status", WiFi.isConnected() ? "connected" : "disconnect");

    cJSON_AddBoolToObject(rspObject, "factory_state", db.factoryState);

    str = cJSON_Print(rspObject);
    server.send(200, "application/json", str);
    log_d("GET /api/v1/info response: '%s'", str);
OUT:
    free(str);
    cJSON_Delete(rspObject);
OUT1:
    return;
}

static void getConfig() {
    File file = FILESYSTEM.open("/db.json", "r");
    server.streamFile(file, "application/json");
    log_d("GET /api/v1/config response: '/db.json'");
    file.close();
    return;
}

static void postConfig() {
    cJSON *reqObject = NULL;
    cJSON *configObject = NULL;
    cJSON *wifiObject = NULL;
    cJSON *rtcObject = NULL;
    cJSON *ntpObject = NULL;
    cJSON *ezdataObject = NULL;
    cJSON *nicknameObject = NULL;
    bool flag = false;

    String content = server.arg("plain");
    log_d("POST /api/v1/config content: '%s'", content.c_str());
    reqObject = cJSON_Parse(content.c_str());
    if (reqObject == NULL) {
        log_w("JSON parse error");
        log_w("payload: '%s'", server.arg("plain").c_str());
        return;
    }

    configObject = cJSON_GetObjectItem(reqObject, "config");
    wifiObject = cJSON_GetObjectItem(configObject, "wifi");
    if (wifiObject) {
        cJSON *ssidObject = cJSON_GetObjectItem(wifiObject, "ssid");
        if (String(ssidObject->valuestring) != db.wifi.ssid && db.wifi.ssid.length() > 0) {
            db.wifi.ssid = ssidObject->valuestring;
            flag = true;
        }
        cJSON *pskObject = cJSON_GetObjectItem(wifiObject, "password");
        if (db.wifi.ssid.length() > 0 && String(pskObject->valuestring) != db.wifi.password) {
            db.wifi.password = pskObject->valuestring;
            flag = true;
        }
    }

    rtcObject = cJSON_GetObjectItem(configObject, "rtc");
    if (rtcObject) {
        cJSON * sleepIntervalObject = cJSON_GetObjectItem(rtcObject, "sleep_interval");
        db.rtc.sleepInterval = sleepIntervalObject->valueint;
    }

    ntpObject = cJSON_GetObjectItem(configObject, "ntp");
    if (ntpObject) {
        cJSON *ntpServer0Object = cJSON_GetObjectItem(ntpObject, "server_0");
        cJSON *ntpServer1Object = cJSON_GetObjectItem(ntpObject, "server_1");
        cJSON *tzObject = cJSON_GetObjectItem(ntpObject, "tz");
        if (String(ntpServer0Object->valuestring) != db.ntp.ntpServer0
            || String(ntpServer1Object->valuestring) != db.ntp.ntpServer1
            || String(tzObject->valuestring) != db.ntp.tz
        ) {
            db.ntp.ntpServer0 = String(ntpServer0Object->valuestring);
            db.ntp.ntpServer1 = String(ntpServer1Object->valuestring);
            db.ntp.tz = String(tzObject->valuestring);
            configTzTime(
                db.ntp.tz.c_str(),
                db.ntp.ntpServer0.c_str(),
                db.ntp.ntpServer1.c_str(),
                "pool.ntp.org"
            );
        }
    }

    ezdataObject = cJSON_GetObjectItem(configObject, "ezdata2");
    if (ezdataObject) {
        cJSON *tokenObject = cJSON_GetObjectItem(ezdataObject, "dev_token");
        db.ezdata2.devToken = tokenObject->valuestring;
        db.factoryState = false;
    }

    nicknameObject = cJSON_GetObjectItem(configObject, "nickname");
    if (nicknameObject) {
        db.nickname = nicknameObject->valuestring;
    }

    cJSON_AddBoolToObject(reqObject, "factory_state", db.factoryState);

    db.saveToFile();

    server.send(200, "application/json", server.arg("plain"));
    log_d("POST /api/v1/config response: '%s'", content.c_str());

    if (flag || WiFi.isConnected() != true) {
        WiFi.disconnect();
        delay(200);
        WiFi.begin(db.wifi.ssid.c_str(), db.wifi.password.c_str());
    }

    cJSON_Delete(reqObject);
    return;
}

static void postAPControl() {
    cJSON *rspObject = NULL;
    char *str = NULL;

    rspObject = cJSON_CreateObject();
    if (rspObject == NULL) {
        return;
    }

    bool status = WiFi.softAPdisconnect();

    cJSON_AddBoolToObject(rspObject, "status", status);

    str = cJSON_Print(rspObject);
    server.send(200, "application/json", str);

    log_d("POST /api/v1/ap_control response: '%s'", str);

    delay(1000);

    free(str);
    cJSON_Delete(rspObject);
    return;
}

static bool getSCD40MeasurementResult(SensirionI2CScd4x& scd4x, uint16_t& co2, float& temperature, float& humidity) {
    log_d("Getting SCD40 measurement");

    uint16_t error;
    char errorMessage[256];

    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        errorToString(error, errorMessage, 256);
        log_e("Error reading SCD40 measurement: %s", errorMessage);
        return false;
    }

    log_d("SCD40 measurement successful - CO2: %d, Temp: %.2f, Humidity: %.2f", 
          co2, temperature, humidity);
    return true;
}

static bool getBME688MeasurementResult(Bsec& bme688, float& temperature, float& humidity, float& pressure, 
                                  float& gasResistance, float& iaq, float& staticIaq, float& co2Equivalent,
                                  float& breathVocEquivalent, float& gasPercentage, uint8_t& iaqAccuracy,
                                  uint8_t& stabStatus, uint8_t& runInStatus) {
    log_d("Getting BME688 measurement");

    if (bme688.run()) {
        // Extract basic measurements
        temperature = bme688.temperature;
        humidity = bme688.humidity;
        pressure = bme688.pressure;
        gasResistance = bme688.gasResistance;

        // Extract additional measurements
        iaq = bme688.iaq;
        staticIaq = bme688.staticIaq;
        co2Equivalent = bme688.co2Equivalent;
        breathVocEquivalent = bme688.breathVocEquivalent;
        gasPercentage = bme688.gasPercentage;
        iaqAccuracy = bme688.iaqAccuracy;
        stabStatus = bme688.stabStatus;
        runInStatus = bme688.runInStatus;

        log_d("BME688 measurement successful - Temp: %.2f, Humidity: %.2f, Pressure: %.2f, Gas: %.2f",
              temperature, humidity, pressure, gasResistance);
        log_d("BME688 additional measurements - IAQ: %.2f (Accuracy: %d), Static IAQ: %.2f, CO2 Eq: %.2f, VOC: %.2f",
              iaq, iaqAccuracy, staticIaq, co2Equivalent, breathVocEquivalent);
        return true;
    } else {
        log_e("BME688 measurement failed");
        return false;
    }
}
