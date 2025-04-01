#pragma once

#include <M5Unified.h>
#include <lgfx/v1/panel/Panel_GDEW0154M09.hpp>

class StatusView
{
public:
    StatusView();
    StatusView(LGFX_Device *lcd, M5Canvas *canvas);
    ~StatusView();

    void begin();
    void load();
    void disappear();

    void updateSCD40(uint16_t co2, float temperature, float humidity);
    void updateSEN55(
        float massConcentrationPm1p0,
        float massConcentrationPm2p5,
        float massConcentrationPm4p0,
        float massConcentrationPm10p0,
        float ambientHumidity,
        float ambientTemperature,
        float vocIndex,
        float noxIndex
    );
    void updateBME688(float temperature, float humidity, float pressure, float gasResistance);
    void updatePower(uint32_t voltage);
    void updateCountdown(uint32_t seconds);
    void displayCountdown(uint32_t seconds);
    void displayNetworkStatus(const char *title, const char *msg);
    void updateNetworkStatus(const char *title, const char *msg);
    void displayNickname(String &nickname);
    void updateNickname(String &nickname);

private:
    void initLOGO();
    void initSCD40();
    void initPower();
    void initSEN55();
    void initBME688();
    void initStatus();
    void _updateImpl(M5Canvas *canvas, int32_t x, int32_t y);
    void splitLongString(String &text, int32_t maxWidth, const lgfx::IFont* font);

    LGFX_Device *_lcd;
    M5Canvas *_canvas;

    // Layout constants
    const int32_t _border = 2;
    const int32_t _padding = 2;

    // Base cursor positions for each section
    const int32_t _scd40BaseCursorX = 2;
    const int32_t _scd40BaseCursorY = 2;

    const int32_t _sen55BaseCursorX = 2;
    const int32_t _sen55BaseCursorY = 2 + 69 + 2;

    const int32_t _bme688BaseCursorX = 2;
    const int32_t _bme688BaseCursorY = 2 + 69 + 2 + 39 + 2;

    const int32_t _powerBaseCursorX = 2;
    const int32_t _powerBaseCursorY = 2 + 69 + 2 + 39 + 2 + 69 + 2;

    // Fonts
    const lgfx::IFont* _scd40TitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _scd40OptionFont = &fonts::efontCN_12;
    const lgfx::IFont* _sen55TitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _sen55OptionFont = &fonts::efontCN_12;
    const lgfx::IFont* _bme688TitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _bme688OptionFont = &fonts::efontCN_12;
    const lgfx::IFont* _powerTitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _poweroptionFont = &fonts::efontCN_12;
    const lgfx::IFont* _statusTitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _statusMsgFont = &fonts::efontCN_12;

    // SCD40
    M5Canvas *_co2Canvas;
    M5Canvas *_tempCanvas;
    M5Canvas *_humiCanvas;
    int32_t _co2CanvasX;
    int32_t _co2CanvasY;
    int32_t _tempCanvasX;
    int32_t _tempCanvasY;
    int32_t _humiCanvasX;
    int32_t _humiCanvasY;

    // SEN55
    M5Canvas *_pm1p0Canvas;
    M5Canvas *_pm2p5Canvas;
    M5Canvas *_pm4p0Canvas;
    M5Canvas *_pm10p0Canvas;
    M5Canvas *_sen55TempCanvas;
    M5Canvas *_sen55HumiCanvas;
    M5Canvas *_vocCanvas;
    M5Canvas *_noxCanvas;
    int32_t _pm1p0CanvasX;
    int32_t _pm1p0CanvasY;
    int32_t _pm2p5CanvasX;
    int32_t _pm2p5CanvasY;
    int32_t _pm4p0CanvasX;
    int32_t _pm4p0CanvasY;
    int32_t _pm10p0CanvasX;
    int32_t _pm10p0CanvasY;
    int32_t _sen55TempCanvasX;
    int32_t _sen55TempCanvasY;
    int32_t _sen55HumiCanvasX;
    int32_t _sen55HumiCanvasY;
    int32_t _vocCanvasX;
    int32_t _vocCanvasY;
    int32_t _noxCanvasX;
    int32_t _noxCanvasY;

    // BME688
    M5Canvas *_bme688TempCanvas;
    M5Canvas *_bme688HumiCanvas;
    M5Canvas *_bme688PressCanvas;
    M5Canvas *_bme688GasCanvas;
    int32_t _bme688TempCanvasX;
    int32_t _bme688TempCanvasY;
    int32_t _bme688HumiCanvasX;
    int32_t _bme688HumiCanvasY;
    int32_t _bme688PressCanvasX;
    int32_t _bme688PressCanvasY;
    int32_t _bme688GasCanvasX;
    int32_t _bme688GasCanvasY;

    // Power
    M5Canvas *_voltageCanvas;
    M5Canvas *_chartCanvas;
    M5Canvas *_chartCanvas1;
    int32_t _voltageCanvasX;
    int32_t _voltageCanvasY;
    int32_t _chartCanvasX;
    int32_t _chartCanvasY;

    // Status
    M5Canvas *_statusTitleCanvas;
    M5Canvas *_statusTitleCanvas1;
    M5Canvas *_statusMsgCanvas;
    M5Canvas *_statusMsgCanvas1;
    int32_t _statusTitleCanvasX;
    int32_t _statusTitleCanvasY;
    int32_t _statusMsgCanvasX;
    int32_t _statusMsgCanvasY;

    // Logo
    M5Canvas *_nicknameCanvas;
    M5Canvas *_nicknameCanvas1;
    int32_t _nicknameCanvasX;
    int32_t _nicknameCanvasY;
};