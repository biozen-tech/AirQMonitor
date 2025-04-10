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
    void displaySplashScreen();

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
    void updateBME688(float temperature, float humidity, float pressure, float gasResistance, float iaq, float co2Equivalent);
    void updatePower(uint32_t voltage);

private:
    void initSCD40();
    void initPower();
    void initSEN55();
    void initBME688();
    void _updateImpl(M5Canvas *canvas, int32_t x, int32_t y);
    void splitLongString(String &text, int32_t maxWidth, const lgfx::IFont* font);

    LGFX_Device *_lcd = nullptr;
    M5Canvas *_canvas = nullptr;

    // Layout constants
    static const int32_t _border = 2;
    static const int32_t _padding = 2;

    // Base cursor positions for each section
    static const int32_t _scd40BaseCursorX = 2;
    static const int32_t _scd40BaseCursorY = 2;

    static const int32_t _sen55BaseCursorX = 100;
    static const int32_t _sen55BaseCursorY = 2;

    // Right column positions - these will be initialized in constructor
    int32_t _columnWidth = 0;  // Will be initialized in constructor
    int32_t _bme688BaseCursorX = 0;
    int32_t _bme688BaseCursorY = 0;
    int32_t _powerBaseCursorX = 0;
    int32_t _powerBaseCursorY = 0;

    // Fonts
    const lgfx::IFont* _scd40TitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _scd40OptionFont = &fonts::efontCN_12;
    const lgfx::IFont* _sen55TitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _sen55OptionFont = &fonts::efontCN_12;
    const lgfx::IFont* _bme688TitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _bme688OptionFont = &fonts::efontCN_12;
    const lgfx::IFont* _powerTitleFont = &fonts::efontCN_14;
    const lgfx::IFont* _poweroptionFont = &fonts::efontCN_12;
    // Removed statustitle, statusmsg, and nickname UI controls
    // const lgfx::IFont* _statusTitleFont = &fonts::efontCN_14;
    // const lgfx::IFont* _statusMsgFont = &fonts::efontCN_12;

    // SCD40
    M5Canvas *_co2Canvas = nullptr;
    M5Canvas *_tempCanvas = nullptr;
    M5Canvas *_humiCanvas = nullptr;
    int32_t _co2CanvasX = 0;
    int32_t _co2CanvasY = 0;
    int32_t _tempCanvasX = 0;
    int32_t _tempCanvasY = 0;
    int32_t _humiCanvasX = 0;
    int32_t _humiCanvasY = 0;

    // SEN55
    M5Canvas *_pm1p0Canvas = nullptr;
    M5Canvas *_pm2p5Canvas = nullptr;
    M5Canvas *_pm4p0Canvas = nullptr;
    M5Canvas *_pm10p0Canvas = nullptr;
    M5Canvas *_sen55TempCanvas = nullptr;
    M5Canvas *_sen55HumiCanvas = nullptr;
    M5Canvas *_vocCanvas = nullptr;
    M5Canvas *_noxCanvas = nullptr;
    int32_t _pm1p0CanvasX = 0;
    int32_t _pm1p0CanvasY = 0;
    int32_t _pm2p5CanvasX = 0;
    int32_t _pm2p5CanvasY = 0;
    int32_t _pm4p0CanvasX = 0;
    int32_t _pm4p0CanvasY = 0;
    int32_t _pm10p0CanvasX = 0;
    int32_t _pm10p0CanvasY = 0;
    int32_t _sen55TempCanvasX = 0;
    int32_t _sen55TempCanvasY = 0;
    int32_t _sen55HumiCanvasX = 0;
    int32_t _sen55HumiCanvasY = 0;
    int32_t _vocCanvasX = 0;
    int32_t _vocCanvasY = 0;
    int32_t _noxCanvasX = 0;
    int32_t _noxCanvasY = 0;

    // BME688
    M5Canvas *_bme688TempCanvas = nullptr;
    M5Canvas *_bme688HumiCanvas = nullptr;
    M5Canvas *_bme688PressCanvas = nullptr;
    M5Canvas *_bme688GasCanvas = nullptr;
    M5Canvas *_bme688IaqCanvas = nullptr;
    M5Canvas *_bme688Co2EqCanvas = nullptr;
    int32_t _bme688TempCanvasX = 0;
    int32_t _bme688TempCanvasY = 0;
    int32_t _bme688HumiCanvasX = 0;
    int32_t _bme688HumiCanvasY = 0;
    int32_t _bme688PressCanvasX = 0;
    int32_t _bme688PressCanvasY = 0;
    int32_t _bme688GasCanvasX = 0;
    int32_t _bme688GasCanvasY = 0;
    int32_t _bme688IaqCanvasX = 0;
    int32_t _bme688IaqCanvasY = 0;
    int32_t _bme688Co2EqCanvasX = 0;
    int32_t _bme688Co2EqCanvasY = 0;

    // Power
    M5Canvas *_voltageCanvas = nullptr;
    M5Canvas *_percentageCanvas = nullptr;
    int32_t _voltageCanvasX = 0;
    int32_t _voltageCanvasY = 0;
    int32_t _percentageCanvasX = 0;
    int32_t _percentageCanvasY = 0;

};
