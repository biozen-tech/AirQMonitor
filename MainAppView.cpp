#include "MainAppView.hpp"

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

StatusView::StatusView() {}


StatusView::StatusView(LGFX_Device *lcd, M5Canvas *canvas) {
    _lcd = lcd;
    _canvas = canvas;
}


StatusView::~StatusView() {}


void StatusView::begin() {
    _canvas->createSprite(_lcd->width(), _lcd->height());
    _canvas->setBaseColor(TFT_BLACK);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _canvas->fillSprite(TFT_WHITE);
    initLOGO();
    initSCD40();
    initPower();
    initSEN55();
    initBME688();
    initStatus();
}


void StatusView::initStatus()
{
    int32_t tempX = 0;
    int32_t tempY = 0;
    int32_t tempW = 0;
    int32_t tempH = 0;

    tempW = 97;

    tempH = _border
            + _padding
            + _canvas->fontHeight(_statusTitleFont)
            + _padding
            + _canvas->fontHeight(_statusMsgFont)
            + _padding
            + _border;

    log_d("Status width: %d px", tempW);
    log_d("Status height: %d px", tempH);

    // button div
    tempX = _powerBaseCursorX;
    tempY = _powerBaseCursorY + 39 + 2;
    _canvas->drawRect(tempX, tempY, tempW, tempH, TFT_BLACK);
    // Header background
    _canvas->fillRect(tempX + _border, tempY + _border, tempW - (_border * 2), _canvas->fontHeight(_statusTitleFont) + (_padding * 2), TFT_BLACK);
    // Header text
    tempX = tempX + _border + _padding;
    tempY = tempY + _border + _padding;
    _canvas->fillRect(tempX, tempY, 5, _canvas->fontHeight(_statusTitleFont), TFT_WHITE);
    tempX = tempX + 5 + 2;
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->drawString("Status", tempX, tempY, _statusTitleFont);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _statusTitleCanvasX = tempX;
    _statusTitleCanvasY = tempY;

    _statusTitleCanvas = new M5Canvas(_canvas);
    _statusTitleCanvas->createSprite(
        _statusTitleCanvas->textWidth("Status", _statusTitleFont),
        _statusTitleCanvas->fontHeight(_statusTitleFont)
    );
    _statusTitleCanvas->setBaseColor(TFT_WHITE);
    _statusTitleCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _statusTitleCanvas1 = new M5Canvas(_lcd);
    _statusTitleCanvas1->createSprite(
        _statusTitleCanvas1->textWidth("Status", _statusTitleFont),
        _statusTitleCanvas1->fontHeight(_statusTitleFont)
    );
    _statusTitleCanvas1->setBaseColor(TFT_WHITE);
    _statusTitleCanvas1->setTextColor(TFT_BLACK, TFT_WHITE);

    _statusMsgCanvasX = tempX;
    _statusMsgCanvasY = tempY + _canvas->fontHeight(_statusTitleFont) + _padding;

    _statusMsgCanvas = new M5Canvas(_canvas);
    _statusMsgCanvas->createSprite(
        _statusMsgCanvas->textWidth("n/a", _statusMsgFont),
        _statusMsgCanvas->fontHeight(_statusMsgFont)
    );
    _statusMsgCanvas->setBaseColor(TFT_WHITE);
    _statusMsgCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _statusMsgCanvas1 = new M5Canvas(_lcd);
    _statusMsgCanvas1->createSprite(
        _statusMsgCanvas1->textWidth("n/a", _statusMsgFont),
        _statusMsgCanvas1->fontHeight(_statusMsgFont)
    );
    _statusMsgCanvas1->setBaseColor(TFT_WHITE);
    _statusMsgCanvas1->setTextColor(TFT_BLACK, TFT_WHITE);
}


void StatusView::initSCD40()
{
    int32_t tempX = 0;
    int32_t tempY = 0;
    int32_t tempW = 0;
    int32_t tempH = 0;

    tempW = _lcd->width() - 4;  // Use full width minus borders

    tempH = _border
            + _padding
            + _canvas->fontHeight(_scd40TitleFont)
            + _padding
            + _canvas->fontHeight(_scd40OptionFont)
            + _padding
            + _canvas->fontHeight(_scd40OptionFont)
            + _padding
            + _canvas->fontHeight(_scd40OptionFont)
            + _padding
            + _border;

    log_d("SCD width: %d px", tempW);
    log_d("SCD height: %d px", tempH);

    // button div
    tempX = _scd40BaseCursorX;
    tempY = _scd40BaseCursorY;
    _canvas->drawRect(tempX, tempY, tempW, tempH, TFT_BLACK);
    // Header background
    _canvas->fillRect(tempX + _border, tempY + _border, tempW - (_border * 2), _canvas->fontHeight(_scd40TitleFont) + (_padding * 2), TFT_BLACK);
    // Header text
    tempX = tempX + _border + _padding;
    tempY = tempY + _border + _padding;
    _canvas->fillRect(tempX, tempY, 5, _canvas->fontHeight(_scd40TitleFont), TFT_WHITE);
    tempX = tempX + 5 + 2;
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->drawString("SCD40", tempX, tempY, _scd40TitleFont);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    // Co2
    tempX = _scd40BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_scd40TitleFont) + _padding;
    _canvas->drawString("Co2", tempX, tempY, _scd40OptionFont);

    tempX = _scd40BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _scd40OptionFont);
    _co2CanvasX = tempX - _canvas->textWidth("00000", _scd40OptionFont);
    _co2CanvasY = tempY;

    // Temperature
    tempX = _scd40BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_scd40OptionFont) + _padding;
    _canvas->drawString("Temp", tempX, tempY, _scd40OptionFont);

    tempX = _scd40BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _scd40OptionFont);
    _tempCanvasX = tempX - _canvas->textWidth("00000", _scd40OptionFont);
    _tempCanvasY = tempY;

    // Humidity
    tempX = _scd40BaseCursorX + _border + _padding;
    tempY= tempY + _canvas->fontHeight(_scd40OptionFont) + _padding;
    _canvas->drawString("RH", tempX, tempY, _scd40OptionFont);

    tempX = _scd40BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _scd40OptionFont);
    _humiCanvasX = tempX - _canvas->textWidth("00000", _scd40OptionFont);
    _humiCanvasY = tempY;

    _co2Canvas = new M5Canvas(_canvas);
    _co2Canvas->createSprite(
        _co2Canvas->textWidth("00000", _scd40OptionFont),
        _co2Canvas->fontHeight(_scd40OptionFont)
    );
    _co2Canvas->setBaseColor(TFT_WHITE);
    _co2Canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _tempCanvas = new M5Canvas(_canvas);
    _tempCanvas->createSprite(
        _tempCanvas->textWidth("00000", _scd40OptionFont),
        _tempCanvas->fontHeight(_scd40OptionFont)
    );
    _tempCanvas->setBaseColor(TFT_WHITE);
    _tempCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _humiCanvas = new M5Canvas(_canvas);
    _humiCanvas->createSprite(
        _humiCanvas->textWidth("00000", _scd40OptionFont),
        _humiCanvas->fontHeight(_scd40OptionFont)
    );
    _humiCanvas->setBaseColor(TFT_WHITE);
    _humiCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
}


void StatusView::initPower()
{
    int32_t tempX = 0;
    int32_t tempY = 0;
    int32_t tempW = 0;
    int32_t tempH = 0;

    tempW = _lcd->width() - 4;  // Use full width minus borders

    tempH = _border
            + _padding
            + _canvas->fontHeight(_powerTitleFont)
            + _padding
            + _canvas->fontHeight(_poweroptionFont)
            + _padding
            + _border;

    log_d("Power Width: %d px", tempW);
    log_d("Power Height: %d px", tempH);

    // button div
    tempX = _powerBaseCursorX;
    tempY = _powerBaseCursorY;
    _canvas->drawRect(tempX, tempY, tempW, tempH, TFT_BLACK);
    // Header background
    _canvas->fillRect(tempX + _border, tempY + _border, tempW - (_border * 2), _canvas->fontHeight(_powerTitleFont) + (_padding * 2), TFT_BLACK);
    // Header text
    tempX = tempX + _border + _padding;
    tempY = tempY + _border + _padding;
    _canvas->fillRect(tempX, tempY, 5, _canvas->fontHeight(_powerTitleFont), TFT_WHITE);
    tempX = tempX + 5 + 2;
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->drawString("Power", tempX, tempY, _powerTitleFont);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    // BAT
    _canvas->drawString("BAT: 0.0V", tempX, tempY, _poweroptionFont);
    _voltageCanvasX = tempX;
    _voltageCanvasY = tempY;

    // Countdown
    tempY = tempY + _canvas->fontHeight(_powerTitleFont);
    _canvas->drawString("Time: 60S", tempX, tempY, _poweroptionFont);
    _chartCanvasX = tempX;
    _chartCanvasY = tempY;

    log_d("cd x: %d", _chartCanvasX);
    log_d("cd y: %d", _chartCanvasY);

    _voltageCanvas = new M5Canvas(_canvas);
    _voltageCanvas->createSprite(tempW - _border - _padding - _padding - _border, _canvas->fontHeight(_poweroptionFont));
    _voltageCanvas->setBaseColor(TFT_WHITE);
    _voltageCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _chartCanvas = new M5Canvas(_canvas);
    _chartCanvas->createSprite(tempW - _border - _padding - _padding - _border, _canvas->fontHeight(_poweroptionFont));
    _chartCanvas->setBaseColor(TFT_WHITE);
    _chartCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _chartCanvas1 = new M5Canvas(_lcd);
    _chartCanvas1->createSprite(tempW - _border - _padding - _padding - _border - _chartCanvas1->textWidth("Time:", _poweroptionFont), _canvas->fontHeight(_poweroptionFont));
    _chartCanvas1->setBaseColor(TFT_WHITE);
    _chartCanvas1->setTextColor(TFT_BLACK, TFT_WHITE);
}


void StatusView::initLOGO()
{
    _nicknameCanvasX = 2;
    _nicknameCanvasY = 2 + 69 + 2 + 39 + 2 + 69 + 2 + 39 + 2;

    _nicknameCanvas = new M5Canvas(_canvas);
    _nicknameCanvas->createSprite(_lcd->width() - 4, 32);  // Use full width minus borders
    _nicknameCanvas->setBaseColor(TFT_BLACK);
    _nicknameCanvas->setTextColor(TFT_WHITE, TFT_BLACK);
 
    _nicknameCanvas->clear();
    int32_t cursorX = (_lcd->width() - 4) / 2;  // Center in full width
    int32_t cursorY = (32 - _nicknameCanvas->fontHeight(&fonts::efontCN_14)) / 2;
    _nicknameCanvas->drawCenterString("AirQ", cursorX, cursorY, &fonts::efontCN_14);
    _updateImpl(_nicknameCanvas, _nicknameCanvasX, _nicknameCanvasY);

    _nicknameCanvas1 = new M5Canvas(_lcd);
    _nicknameCanvas1->createSprite(_lcd->width() - 4, 32);  // Use full width minus borders
    _nicknameCanvas1->setBaseColor(TFT_BLACK);
    _nicknameCanvas1->setTextColor(TFT_WHITE, TFT_BLACK);
    _nicknameCanvas1->clear();
    cursorX = (_lcd->width() - 4) / 2;  // Center in full width
    cursorY = (32 - _nicknameCanvas1->fontHeight(&fonts::efontCN_14)) / 2;
    _nicknameCanvas1->drawCenterString("AirQ", cursorX, cursorY, &fonts::efontCN_14);
}


void StatusView::initSEN55()
{
    int32_t tempX = 0;
    int32_t tempY = 0;
    int32_t tempW = 0;
    int32_t tempH = 0;

    tempW = _lcd->width() - 4;  // Use full width minus borders

    tempH = _border
            + _padding
            + _canvas->fontHeight(_sen55TitleFont)
            + _padding
            + _canvas->fontHeight(_sen55OptionFont)
            + _padding
            + _canvas->fontHeight(_sen55OptionFont)
            + _padding
            + _canvas->fontHeight(_sen55OptionFont)
            + _padding
            + _canvas->fontHeight(_sen55OptionFont)
            + _padding
            + _canvas->fontHeight(_sen55OptionFont)
            + _padding
            + _canvas->fontHeight(_sen55OptionFont)
            + _padding
            + _canvas->fontHeight(_sen55OptionFont)
            + _padding
            + _canvas->fontHeight(_sen55OptionFont)
            + _padding
            + _border;

    log_d("SEN width: %d px", tempW);
    log_d("SEN height: %d px", tempH);

    // button div
    tempX = _sen55BaseCursorX;
    tempY = _sen55BaseCursorY;
    _canvas->drawRect(tempX, tempY, tempW, tempH, TFT_BLACK);
    // Header background
    _canvas->fillRect(tempX + _border, tempY + _border, tempW - (_border * 2), _canvas->fontHeight(_sen55TitleFont) + (_padding * 2), TFT_BLACK);
    // Header text
    tempX = tempX + _border + _padding;
    tempY = tempY + _border + _padding;
    _canvas->fillRect(tempX, tempY, 5, _canvas->fontHeight(_sen55TitleFont), TFT_WHITE);
    tempX = tempX + 5 + 2;
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->drawString("SEN55", tempX, tempY, _sen55TitleFont);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    // PM1.0
    tempX = _sen55BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_sen55TitleFont) + _padding;
    _canvas->drawString("PM1.0", tempX, tempY, _sen55OptionFont);

    tempX = _sen55BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _sen55OptionFont);
    _pm1p0CanvasX = tempX - _canvas->textWidth("00000", _sen55OptionFont);
    _pm1p0CanvasY = tempY;

    // PM2.5
    tempX = _sen55BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM2.5", tempX, tempY, _sen55OptionFont);

    tempX = _sen55BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _sen55OptionFont);
    _pm2p5CanvasX = tempX - _canvas->textWidth("00000", _sen55OptionFont);
    _pm2p5CanvasY = tempY;

    // PM4.0
    tempX = _sen55BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM4.0", tempX, tempY, _sen55OptionFont);

    tempX = _sen55BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _sen55OptionFont);
    _pm4p0CanvasX = tempX - _canvas->textWidth("00000", _sen55OptionFont);
    _pm4p0CanvasY = tempY;

    // PM10.0
    tempX = _sen55BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM10", tempX, tempY, _sen55OptionFont);

    tempX = _sen55BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _sen55OptionFont);
    _pm10p0CanvasX = tempX - _canvas->textWidth("00000", _sen55OptionFont);
    _pm10p0CanvasY = tempY;

    // Temperature
    tempX = _sen55BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("Temp", tempX, tempY, _sen55OptionFont);

    tempX = _sen55BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _sen55OptionFont);
    _sen55TempCanvasX = tempX - _canvas->textWidth("00000", _sen55OptionFont);
    _sen55TempCanvasY = tempY;

    // Humidity
    tempX = _sen55BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("RH", tempX, tempY, _sen55OptionFont);

    tempX = _sen55BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _sen55OptionFont);
    _sen55HumiCanvasX = tempX - _canvas->textWidth("00000", _sen55OptionFont);
    _sen55HumiCanvasY = tempY;

    // voc
    tempX = _sen55BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("VOC", tempX, tempY, _sen55OptionFont);

    tempX = _sen55BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _sen55OptionFont);
    _vocCanvasX = tempX - _canvas->textWidth("00000", _sen55OptionFont);
    _vocCanvasY = tempY;

    // nox
    tempX = _sen55BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("NOX", tempX, tempY, _sen55OptionFont);

    tempX = _sen55BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _sen55OptionFont);
    _noxCanvasX = tempX - _canvas->textWidth("00000", _sen55OptionFont);
    _noxCanvasY = tempY;

    _pm1p0Canvas = new M5Canvas(_canvas);
    _pm1p0Canvas->createSprite(
        _pm1p0Canvas->textWidth("00000", _sen55OptionFont),
        _pm1p0Canvas->fontHeight(_sen55OptionFont)
    );
    _pm1p0Canvas->setBaseColor(TFT_WHITE);
    _pm1p0Canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _pm2p5Canvas = new M5Canvas(_canvas);
    _pm2p5Canvas->createSprite(
        _pm2p5Canvas->textWidth("00000", _sen55OptionFont),
        _pm2p5Canvas->fontHeight(_sen55OptionFont)
    );
    _pm2p5Canvas->setBaseColor(TFT_WHITE);
    _pm2p5Canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _pm4p0Canvas = new M5Canvas(_canvas);
    _pm4p0Canvas->createSprite(
        _pm4p0Canvas->textWidth("00000", _sen55OptionFont),
        _pm4p0Canvas->fontHeight(_sen55OptionFont)
    );
    _pm4p0Canvas->setBaseColor(TFT_WHITE);
    _pm4p0Canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _pm10p0Canvas = new M5Canvas(_canvas);
    _pm10p0Canvas->createSprite(
        _pm10p0Canvas->textWidth("00000", _sen55OptionFont),
        _pm10p0Canvas->fontHeight(_sen55OptionFont)
    );
    _pm10p0Canvas->setBaseColor(TFT_WHITE);
    _pm10p0Canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _sen55TempCanvas = new M5Canvas(_canvas);
    _sen55TempCanvas->createSprite(
        _sen55TempCanvas->textWidth("00000", _sen55OptionFont),
        _sen55TempCanvas->fontHeight(_sen55OptionFont)
    );
    _sen55TempCanvas->setBaseColor(TFT_WHITE);
    _sen55TempCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _sen55HumiCanvas = new M5Canvas(_canvas);
    _sen55HumiCanvas->createSprite(
        _sen55HumiCanvas->textWidth("00000", _sen55OptionFont),
        _sen55HumiCanvas->fontHeight(_sen55OptionFont)
    );
    _sen55HumiCanvas->setBaseColor(TFT_WHITE);
    _sen55HumiCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _vocCanvas = new M5Canvas(_canvas);
    _vocCanvas->createSprite(
        _vocCanvas->textWidth("00000", _sen55OptionFont),
        _vocCanvas->fontHeight(_sen55OptionFont)
    );
    _vocCanvas->setBaseColor(TFT_WHITE);
    _vocCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _noxCanvas = new M5Canvas(_canvas);
    _noxCanvas->createSprite(
        _noxCanvas->textWidth("00000", _sen55OptionFont),
        _noxCanvas->fontHeight(_sen55OptionFont)
    );
    _noxCanvas->setBaseColor(TFT_WHITE);
    _noxCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
}

void StatusView::initBME688()
{
    int32_t tempX = 0;
    int32_t tempY = 0;
    int32_t tempW = 0;
    int32_t tempH = 0;

    tempW = _lcd->width() - 4;  // Use full width minus borders

    tempH = _border
            + _padding
            + _canvas->fontHeight(_bme688TitleFont)
            + _padding
            + _canvas->fontHeight(_bme688OptionFont)
            + _padding
            + _canvas->fontHeight(_bme688OptionFont)
            + _padding
            + _canvas->fontHeight(_bme688OptionFont)
            + _padding
            + _canvas->fontHeight(_bme688OptionFont)
            + _padding
            + _border;

    log_d("BME688 width: %d px", tempW);
    log_d("BME688 height: %d px", tempH);

    // button div
    tempX = _bme688BaseCursorX;
    tempY = _bme688BaseCursorY;
    _canvas->drawRect(tempX, tempY, tempW, tempH, TFT_BLACK);
    // Header background
    _canvas->fillRect(tempX + _border, tempY + _border, tempW - (_border * 2), _canvas->fontHeight(_bme688TitleFont) + (_padding * 2), TFT_BLACK);
    // Header text
    tempX = tempX + _border + _padding;
    tempY = tempY + _border + _padding;
    _canvas->fillRect(tempX, tempY, 5, _canvas->fontHeight(_bme688TitleFont), TFT_WHITE);
    tempX = tempX + 5 + 2;
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->drawString("BME688", tempX, tempY, _bme688TitleFont);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    // Temperature
    tempX = _bme688BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_bme688TitleFont) + _padding;
    _canvas->drawString("Temp", tempX, tempY, _bme688OptionFont);

    tempX = _bme688BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _bme688OptionFont);
    _bme688TempCanvasX = tempX - _canvas->textWidth("00000", _bme688OptionFont);
    _bme688TempCanvasY = tempY;

    // Humidity
    tempX = _bme688BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_bme688OptionFont) + _padding;
    _canvas->drawString("RH", tempX, tempY, _bme688OptionFont);

    tempX = _bme688BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _bme688OptionFont);
    _bme688HumiCanvasX = tempX - _canvas->textWidth("00000", _bme688OptionFont);
    _bme688HumiCanvasY = tempY;

    // Pressure
    tempX = _bme688BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_bme688OptionFont) + _padding;
    _canvas->drawString("Press", tempX, tempY, _bme688OptionFont);

    tempX = _bme688BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _bme688OptionFont);
    _bme688PressCanvasX = tempX - _canvas->textWidth("00000", _bme688OptionFont);
    _bme688PressCanvasY = tempY;

    // Gas Resistance
    tempX = _bme688BaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_bme688OptionFont) + _padding;
    _canvas->drawString("Gas", tempX, tempY, _bme688OptionFont);

    tempX = _bme688BaseCursorX + tempW - 2;
    _canvas->drawRightString("n/a", tempX, tempY, _bme688OptionFont);
    _bme688GasCanvasX = tempX - _canvas->textWidth("00000", _bme688OptionFont);
    _bme688GasCanvasY = tempY;

    _bme688TempCanvas = new M5Canvas(_canvas);
    _bme688TempCanvas->createSprite(
        _bme688TempCanvas->textWidth("00000", _bme688OptionFont),
        _bme688TempCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688TempCanvas->setBaseColor(TFT_WHITE);
    _bme688TempCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _bme688HumiCanvas = new M5Canvas(_canvas);
    _bme688HumiCanvas->createSprite(
        _bme688HumiCanvas->textWidth("00000", _bme688OptionFont),
        _bme688HumiCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688HumiCanvas->setBaseColor(TFT_WHITE);
    _bme688HumiCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _bme688PressCanvas = new M5Canvas(_canvas);
    _bme688PressCanvas->createSprite(
        _bme688PressCanvas->textWidth("00000", _bme688OptionFont),
        _bme688PressCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688PressCanvas->setBaseColor(TFT_WHITE);
    _bme688PressCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    _bme688GasCanvas = new M5Canvas(_canvas);
    _bme688GasCanvas->createSprite(
        _bme688GasCanvas->textWidth("00000", _bme688OptionFont),
        _bme688GasCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688GasCanvas->setBaseColor(TFT_WHITE);
    _bme688GasCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
}

void StatusView::updatePower(uint32_t voltage)
{
    static uint32_t lastVoltage = 0;

    if (lastVoltage == voltage) {
        return ;
    }

    char str[13] = { 0 };

    _voltageCanvas->clear(TFT_WHITE);

    float v = (((float)voltage / 1000) * 2);
    v = v > 4.2 ? 4.2 : v;

    sprintf(str, "BAT:%.2fV", v);
    _voltageCanvas->drawString(str, 0, 0, _poweroptionFont);

    _updateImpl(_voltageCanvas, _voltageCanvasX, _voltageCanvasY);

    lastVoltage = voltage;
}


void StatusView::updateCountdown(uint32_t seconds) {
    String msg;
    int remainingSeconds = 0;
    uint32_t h = 0;
    uint32_t m = 0;
    uint32_t s = 0;

    h = seconds / 3600;
    remainingSeconds = seconds % 3600;
    m = remainingSeconds / 60;
    s = remainingSeconds % 60;
    msg = "Time:";
    if (h != 0) {
        msg += String(h) + "h";
    }
    if (h != 0 || m != 0 || (h != 0 && s != 0)) {
        msg += String(m) + "m";
    }
    if (s != 0) {
        msg += String(s) + "s";
    }

    _chartCanvas->clear(TFT_WHITE);
    _chartCanvas->drawString(msg.c_str(), 0, 0, _poweroptionFont);
    _updateImpl(_chartCanvas, _chartCanvasX, _chartCanvasY);
}


void StatusView::displayCountdown(uint32_t seconds) {
    String msg;
    int remainingSeconds = 0;
    uint32_t h = 0;
    uint32_t m = 0;
    uint32_t s = 0;

    h = seconds / 3600;
    remainingSeconds = seconds % 3600;
    m = remainingSeconds / 60;
    s = remainingSeconds % 60;

    if (h != 0) {
        msg += String(h) + "h";
    }
    if (h != 0 || m != 0 || (h != 0 && s != 0)) {
        msg += String(m) + "m";
    }
    if (s != 0) {
        msg += String(s) + "s";
    }

    _chartCanvas1->clear(TFT_WHITE);
    _chartCanvas1->drawString(msg.c_str(), 0, 0, _poweroptionFont);
    _lcd->fillRect(_chartCanvasX + _chartCanvas1->textWidth("Time:", _poweroptionFont), _chartCanvasY, _chartCanvas1->width(), _chartCanvas1->height(), TFT_WHITE);
    _chartCanvas1->pushSprite(_chartCanvasX + _chartCanvas1->textWidth("Time:", _poweroptionFont), _chartCanvasY);
    _lcd->waitDisplay();

    updateCountdown(seconds);
}


void StatusView::updateSEN55(
    float massConcentrationPm1p0,
    float massConcentrationPm2p5,
    float massConcentrationPm4p0,
    float massConcentrationPm10p0,
    float ambientHumidity,
    float ambientTemperature,
    float vocIndex,
    float noxIndex
) {
    char str[8] = { 0 };

    _pm1p0Canvas->clear(TFT_WHITE);
    _pm2p5Canvas->clear(TFT_WHITE);
    _pm4p0Canvas->clear(TFT_WHITE);
    _pm10p0Canvas->clear(TFT_WHITE);
    _sen55TempCanvas->clear(TFT_WHITE);
    _sen55HumiCanvas->clear(TFT_WHITE);
    _vocCanvas->clear(TFT_WHITE);
    _noxCanvas->clear(TFT_WHITE);


    sprintf(str, "%.2f", massConcentrationPm1p0);
    _pm1p0Canvas->drawRightString(str, _pm1p0Canvas->width(), 0, _sen55OptionFont);
    _pm1p0Canvas->pushSprite(_pm1p0CanvasX, _pm1p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm2p5);
    _pm2p5Canvas->drawRightString(str, _pm2p5Canvas->width(), 0, _sen55OptionFont);
    _pm2p5Canvas->pushSprite(_pm2p5CanvasX, _pm2p5CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm4p0);
    _pm4p0Canvas->drawRightString(str, _pm4p0Canvas->width(), 0, _sen55OptionFont);
    _pm4p0Canvas->pushSprite(_pm4p0CanvasX, _pm4p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm10p0);
    _pm10p0Canvas->drawRightString(str, _pm10p0Canvas->width(), 0, _sen55OptionFont);
    _pm10p0Canvas->pushSprite(_pm10p0CanvasX, _pm10p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", ambientTemperature);
    _sen55TempCanvas->drawRightString(str, _sen55TempCanvas->width(), 0, _sen55OptionFont);
    _sen55TempCanvas->pushSprite(_sen55TempCanvasX, _sen55TempCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", ambientHumidity);
    _sen55HumiCanvas->drawRightString(str, _sen55HumiCanvas->width(), 0, _sen55OptionFont);
    _sen55HumiCanvas->pushSprite(_sen55HumiCanvasX, _sen55HumiCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", vocIndex);
    _vocCanvas->drawRightString(str, _pm1p0Canvas->width(), 0, _sen55OptionFont);
    _vocCanvas->pushSprite(_vocCanvasX, _vocCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", noxIndex);
    _noxCanvas->drawRightString(str, _noxCanvas->width(), 0, _sen55OptionFont);
    _noxCanvas->pushSprite(_noxCanvasX, _noxCanvasY);
}


void StatusView::displayNetworkStatus(const char *title, const char *msg) {
    _statusTitleCanvas1->clear(TFT_WHITE);
    _statusMsgCanvas1->clear(TFT_WHITE);
    _statusTitleCanvas1->drawCenterString(title, _statusTitleCanvas1->width() / 2, 0, _statusTitleFont);
    _statusMsgCanvas1->drawCenterString(msg, _statusMsgCanvas1->width() / 2, 0, _statusMsgFont);

    _updateImpl(_statusTitleCanvas1, _statusTitleCanvasX, _statusTitleCanvasY);
    _updateImpl(_statusMsgCanvas1, _statusMsgCanvasX, _statusMsgCanvasY);
    updateNetworkStatus(title, msg);
}


void StatusView::updateNetworkStatus(const char *title, const char *msg) {
    _statusTitleCanvas->clear(TFT_WHITE);
    _statusMsgCanvas->clear(TFT_WHITE);
    _statusTitleCanvas->drawCenterString(title, _statusTitleCanvas->width() / 2, 0, _statusTitleFont);
    _statusMsgCanvas->drawCenterString(msg, _statusMsgCanvas->width() / 2, 0, _statusMsgFont);

    _updateImpl(_statusTitleCanvas, _statusTitleCanvasX, _statusTitleCanvasY);
    _updateImpl(_statusMsgCanvas, _statusMsgCanvasX, _statusMsgCanvasY);
}


void StatusView::displayNickname(String &nickname) {

    const lgfx::IFont* fontTable[] = {&fonts::efontCN_16, &fonts::efontCN_14, &fonts::efontCN_12, &fonts::efontCN_10};
    size_t i = 0;

    for (i = 0; i < 4; i++) {
        if (_nicknameCanvas1->textWidth(nickname, fontTable[i]) > 95) {
            continue;
        } else {
            break;
        }
    }

    log_d("%d", i);
    if (i >= 4) {
        splitLongString(nickname, 95, fontTable[3]);
        i = 3;
    }

    log_d("%s", nickname.c_str());
    _nicknameCanvas1->clear();
    int32_t cursorX = 97 / 2;
    int32_t cursorY = (32 - _canvas->fontHeight(fontTable[i])) / 2;
    log_d("%s", nickname.c_str());
    _nicknameCanvas1->drawCenterString(nickname, cursorX, cursorY, fontTable[i]);

    _updateImpl(_nicknameCanvas1, _nicknameCanvasX, _nicknameCanvasY);
    updateNickname(nickname);
}


void StatusView::updateNickname(String &nickname) {
    const lgfx::IFont* fontTable[] = {&fonts::efontCN_16, &fonts::efontCN_14, &fonts::efontCN_12, &fonts::efontCN_10};
    size_t i = 0;

    for (i = 0; i < 4; i++) {
        if (_nicknameCanvas->textWidth(nickname, fontTable[i]) > 95) {
            continue;
        } else {
            break;
        }
    }

    log_d("%d", i);
    if (i >= 4) {
        splitLongString(nickname, 95, fontTable[3]);
        i = 3;
    }

    log_d("%s", nickname.c_str());
    _nicknameCanvas->clear();
    int32_t cursorX = 97 / 2;
    int32_t cursorY = (32 - _nicknameCanvas->fontHeight(fontTable[i])) / 2;
    log_d("%s", nickname.c_str());
    _nicknameCanvas->drawCenterString(nickname, cursorX, cursorY, fontTable[i]);

    _updateImpl(_nicknameCanvas, _nicknameCanvasX, _nicknameCanvasY);

}


void StatusView::load() {
    _lcd->clear(TFT_WHITE);
    _lcd->waitDisplay();
    _updateImpl(_canvas, 0, 0);
    _lcd->waitDisplay();
}


void StatusView::disappear() {
    _lcd->clear();
}


void StatusView::_updateImpl(M5Canvas *canvas, int32_t x, int32_t y)
{
    canvas->pushSprite(x, y);
}


void StatusView::splitLongString(String &text, int32_t maxWidth, const lgfx::IFont* font) {
    int32_t w = _lcd->textWidth(text, font);
    int32_t start = 1;
    int32_t end = 0;
    if (w < maxWidth) {
        return ;
    }

    w = _lcd->textWidth("...", font);
    for (;;) {
        int32_t ww = _lcd->textWidth(text.substring(0, end), font);
        ww = _lcd->textWidth(text.substring(0, end), font);
        if (ww > (maxWidth / 2 - w)) {
            end -= 1;
            break;
        }
        end += 1;
    }

    start = end;

    for (;;) {
        int32_t ww = _lcd->textWidth(text.substring(start, -1), font);
        if (ww < (maxWidth / 2 - w)) {
            start += 1;
            break;
        }
        start += 1;
    }

    text = text.substring(0, end) + "..." + text.substring(start);
    log_d("%s", text.c_str());
}

void StatusView::updateBME688(float temperature, float humidity, float pressure, float gasResistance)
{
    // Check if canvas objects are properly initialized
    if (!_bme688TempCanvas || !_bme688HumiCanvas || !_bme688PressCanvas || !_bme688GasCanvas) {
        log_e("BME688 canvas objects not properly initialized");
        return;
    }

    // Use a larger buffer to prevent overflow
    char str[16] = { 0 };

    // Clear all canvases
    _bme688TempCanvas->clear(TFT_WHITE);
    _bme688HumiCanvas->clear(TFT_WHITE);
    _bme688PressCanvas->clear(TFT_WHITE);
    _bme688GasCanvas->clear(TFT_WHITE);

    // Update temperature with bounds checking
    if (!isnan(temperature) && temperature >= -40.0f && temperature <= 85.0f) {
        snprintf(str, sizeof(str), "%.2f", temperature);
        _bme688TempCanvas->drawRightString(str, _bme688TempCanvas->width(), 0, _bme688OptionFont);
        _bme688TempCanvas->pushSprite(_bme688TempCanvasX, _bme688TempCanvasY);
    }

    // Update humidity with bounds checking
    if (!isnan(humidity) && humidity >= 0.0f && humidity <= 100.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", humidity);
        _bme688HumiCanvas->drawRightString(str, _bme688HumiCanvas->width(), 0, _bme688OptionFont);
        _bme688HumiCanvas->pushSprite(_bme688HumiCanvasX, _bme688HumiCanvasY);
    }

    // Update pressure with bounds checking
    if (!isnan(pressure) && pressure >= 300.0f && pressure <= 1100.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", pressure);
        _bme688PressCanvas->drawRightString(str, _bme688PressCanvas->width(), 0, _bme688OptionFont);
        _bme688PressCanvas->pushSprite(_bme688PressCanvasX, _bme688PressCanvasY);
    }

    // Update gas resistance with bounds checking
    if (!isnan(gasResistance) && gasResistance >= 0.0f && gasResistance <= 100000.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", gasResistance);
        _bme688GasCanvas->drawRightString(str, _bme688GasCanvas->width(), 0, _bme688OptionFont);
        _bme688GasCanvas->pushSprite(_bme688GasCanvasX, _bme688GasCanvasY);
    }
}

void StatusView::updateSCD40(uint16_t co2, float temperature, float humidity)
{
    // Check if canvas objects are properly initialized
    if (!_co2Canvas || !_tempCanvas || !_humiCanvas) {
        log_e("SCD40 canvas objects not properly initialized");
        return;
    }

    // Use a larger buffer to prevent overflow
    char str[16] = { 0 };

    // Clear all canvases
    _co2Canvas->clear(TFT_WHITE);
    _tempCanvas->clear(TFT_WHITE);
    _humiCanvas->clear(TFT_WHITE);

    // Update CO2 with bounds checking
    if (co2 >= 400 && co2 <= 5000) {
        snprintf(str, sizeof(str), "%d", co2);
        _co2Canvas->drawRightString(str, _co2Canvas->width(), 0, _scd40OptionFont);
        _co2Canvas->pushSprite(_co2CanvasX, _co2CanvasY);
    }

    // Update temperature with bounds checking
    if (!isnan(temperature) && temperature >= -10.0f && temperature <= 60.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", temperature);
        _tempCanvas->drawRightString(str, _tempCanvas->width(), 0, _scd40OptionFont);
        _tempCanvas->pushSprite(_tempCanvasX, _tempCanvasY);
    }

    // Update humidity with bounds checking
    if (!isnan(humidity) && humidity >= 0.0f && humidity <= 100.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", humidity);
        _humiCanvas->drawRightString(str, _humiCanvas->width(), 0, _scd40OptionFont);
        _humiCanvas->pushSprite(_humiCanvasX, _humiCanvasY);
    }
}
