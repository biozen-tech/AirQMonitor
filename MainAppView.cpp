#include "MainAppView.hpp"

StatusView::StatusView() {
    log_i("Default constructor called");
}

StatusView::StatusView(LGFX_Device *lcd, M5Canvas *canvas) {
    log_i("Constructor called with lcd and canvas");
    _lcd = lcd;
    _canvas = canvas;
    
    // Initialize column width and cursor positions with fixed LCD dimensions
    _columnWidth = 98;  // (200 - 4) / 2 = 98 for each column
    log_i("Initialized column width: %ld", _columnWidth);
    
    // Set right column positions
    _bme688BaseCursorX = 100;  // Start at right column (200/2)
    _bme688BaseCursorY = 2;    // Start at top of right column
    _powerBaseCursorX = 100;   // Start at right column (200/2)
    _powerBaseCursorY = 2 + 69 + 2;  // Position below BME688 section
    log_i("Initialized cursor positions - BME688: (%ld, %ld), Power: (%ld, %ld)", 
         _bme688BaseCursorX, _bme688BaseCursorY, _powerBaseCursorX, _powerBaseCursorY);
}

StatusView::~StatusView() {
    log_i("Destructor called");
    // Log all canvas pointers before cleanup
    log_i("Cleaning up canvas pointers:");
    log_i("  BME688 - Temp: %p, Humi: %p, Press: %p, Gas: %p",
          _bme688TempCanvas, _bme688HumiCanvas, _bme688PressCanvas, _bme688GasCanvas);
    log_i("  SCD40 - CO2: %p, Temp: %p, Humi: %p",
          _co2Canvas, _tempCanvas, _humiCanvas);
    log_i("  SEN55 - PM1.0: %p, PM2.5: %p, PM4.0: %p, PM10: %p",
          _pm1p0Canvas, _pm2p5Canvas, _pm4p0Canvas, _pm10p0Canvas);
    log_i("  Status - Title: %p, Msg: %p",
          _statusTitleCanvas, _statusMsgCanvas);
    log_i("  Power - Voltage: %p",
          _voltageCanvas);
}

void StatusView::begin() {
    log_i("begin() called");
    
    // Create sprite with full LCD dimensions
    log_i("Creating main canvas sprite with dimensions: %dx%d", _lcd->width(), _lcd->height());
    _canvas->createSprite(_lcd->width(), _lcd->height());
    _canvas->setBaseColor(TFT_BLACK);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _canvas->fillSprite(TFT_WHITE);

    // Draw vertical divider line
    log_i("Drawing vertical divider line at x=%ld", _lcd->width() / 2);
    _canvas->drawLine(_lcd->width() / 2, 0, _lcd->width() / 2, _lcd->height(), TFT_BLACK);

    // Initialize chart canvases
    log_i("Initializing chart canvases");
    _chartCanvas = new M5Canvas(_canvas);
    _chartCanvas->createSprite(60, 12);  // Width for time display, height for font
    _chartCanvas->setBaseColor(TFT_WHITE);
    _chartCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _chartCanvas->setTextDatum(TL_DATUM);
    _chartCanvas->setTextSize(1);

    _chartCanvas1 = new M5Canvas(_lcd);
    _chartCanvas1->createSprite(60, 12);  // Width for time display, height for font
    _chartCanvas1->setBaseColor(TFT_WHITE);
    _chartCanvas1->setTextColor(TFT_BLACK, TFT_WHITE);
    _chartCanvas1->setTextDatum(TL_DATUM);
    _chartCanvas1->setTextSize(1);

    // Set chart canvas positions
    _chartCanvasX = 2;
    _chartCanvasY = _lcd->height() - 34;  // Position above logo

    // Initialize sections in their respective columns
    log_i("Initializing UI sections");
    initSCD40();    // Left column, top
    log_i("SCD40 initialization complete");
    initSEN55();    // Left column, bottom
    log_i("SEN55 initialization complete");
    initBME688();   // Right column, top
    log_i("BME688 initialization complete");
    initPower();    // Right column, bottom
    log_i("Power initialization complete");
    initStatus();   // Status at the bottom
    log_i("Status initialization complete");
    initLOGO();     // Logo at the bottom
    log_i("Logo initialization complete");
    log_i("UI initialization complete");
}

void StatusView::initStatus()
{
    log_i("Initializing status section");
    int32_t tempX = 0;
    int32_t tempY = 0;
    int32_t tempW = 0;
    int32_t tempH = 0;

    // Use full width for status section
    tempW = _lcd->width() - 4;
    log_i("Status section width: %ld", tempW);

    tempH = _border
            + _padding
            + _canvas->fontHeight(_statusTitleFont)
            + _padding
            + _canvas->fontHeight(_statusMsgFont)
            + _padding
            + _border;
    log_i("Status section height: %ld", tempH);

    // button div - centered at bottom
    tempX = 2;  // Start from left edge
    tempY = _lcd->height() - tempH - 2;  // Position at bottom with border
    log_i("Drawing status section at (%ld, %ld)", tempX, tempY);
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
    log_i("Status title position: (%ld, %ld)", _statusTitleCanvasX, _statusTitleCanvasY);

    // Create sprites with proper dimensions
    log_i("Creating status title sprites");
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
    log_i("Status message position: (%ld, %ld)", _statusMsgCanvasX, _statusMsgCanvasY);

    // Create message sprites with proper dimensions
    log_i("Creating status message sprites");
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
    log_i("Status section initialization complete");
}

void StatusView::initSCD40()
{
    int32_t tempX = 0;
    int32_t tempY = 0;
    int32_t tempW = 0;
    int32_t tempH = 0;

    tempW = _columnWidth;  // Use column width instead of full width

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

    tempW = _lcd->width() / 2 - 4;  // Half width minus borders

    tempH = _border
            + _padding
            + _canvas->fontHeight(_powerTitleFont)
            + _padding
            + _canvas->fontHeight(_poweroptionFont)
            + _padding
            + _border;

    log_d("Power width: %d px", tempW);
    log_d("Power height: %d px", tempH);

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

    // Battery label
    tempX = _powerBaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_powerTitleFont) + _padding;
    _canvas->drawString("BAT", tempX, tempY, _poweroptionFont);

    // Battery value position
    _voltageCanvasX = tempX + _canvas->textWidth("BAT", _poweroptionFont) + _padding;
    _voltageCanvasY = tempY;

    // Create sprites with proper dimensions
    _voltageCanvas = new M5Canvas(_canvas);
    _voltageCanvas->createSprite(
        _voltageCanvas->textWidth("0.00", _poweroptionFont),
        _voltageCanvas->fontHeight(_poweroptionFont)
    );
    _voltageCanvas->setBaseColor(TFT_WHITE);
    _voltageCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
}

void StatusView::initLOGO()
{
    log_i("Initializing logo section");
    // Position logo at the bottom of the screen
    _nicknameCanvasX = 2;
    _nicknameCanvasY = _lcd->height() - 34;  // 32px height + 2px border

    _nicknameCanvas = new M5Canvas(_canvas);
    _nicknameCanvas->createSprite(_lcd->width() - 4, 32);  // Full width minus borders
    _nicknameCanvas->setBaseColor(TFT_BLACK);
    _nicknameCanvas->setTextColor(TFT_WHITE, TFT_BLACK);
 
    _nicknameCanvas->clear();
    int32_t cursorX = (_lcd->width() - 4) / 2;  // Center in full width
    int32_t cursorY = (32 - _canvas->fontHeight(&fonts::efontCN_14)) / 2;
    _nicknameCanvas->drawCenterString("AirQ", cursorX, cursorY, &fonts::efontCN_14);
    _updateImpl(_nicknameCanvas, _nicknameCanvasX, _nicknameCanvasY);

    _nicknameCanvas1 = new M5Canvas(_lcd);
    _nicknameCanvas1->createSprite(_lcd->width() - 4, 32);  // Full width minus borders
    _nicknameCanvas1->setBaseColor(TFT_BLACK);
    _nicknameCanvas1->setTextColor(TFT_WHITE, TFT_BLACK);
    _nicknameCanvas1->clear();
    cursorX = (_lcd->width() - 4) / 2;  // Center in full width
    cursorY = (32 - _nicknameCanvas1->fontHeight(&fonts::efontCN_14)) / 2;
    _nicknameCanvas1->drawCenterString("AirQ", cursorX, cursorY, &fonts::efontCN_14);

    log_i("Logo section initialized successfully");
}

void StatusView::initSEN55()
{
    log_i("Initializing SEN55 section");
    int32_t tempX = 0;
    int32_t tempY = 0;
    int32_t tempW = 0;
    int32_t tempH = 0;

    tempW = _columnWidth;  // Use column width instead of full width

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

    log_i("SEN55 dimensions - Width: %ld, Height: %ld", tempW, tempH);

    // Draw container box
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

    // Draw labels and create canvases for each value
    int32_t labelX = tempX;
    int32_t labelY = tempY + _canvas->fontHeight(_sen55TitleFont) + _padding;

    // PM1.0
    _canvas->drawString("PM1.0:", labelX, labelY);
    _pm1p0CanvasX = labelX + _canvas->textWidth("PM1.0:", _sen55OptionFont) + 2;
    _pm1p0CanvasY = labelY;

    // PM2.5
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM2.5:", labelX, labelY);
    _pm2p5CanvasX = labelX + _canvas->textWidth("PM2.5:", _sen55OptionFont) + 2;
    _pm2p5CanvasY = labelY;

    // PM4.0
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM4.0:", labelX, labelY);
    _pm4p0CanvasX = labelX + _canvas->textWidth("PM4.0:", _sen55OptionFont) + 2;
    _pm4p0CanvasY = labelY;

    // PM10.0
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM10:", labelX, labelY);
    _pm10p0CanvasX = labelX + _canvas->textWidth("PM10:", _sen55OptionFont) + 2;
    _pm10p0CanvasY = labelY;

    // Temperature
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("Temp:", labelX, labelY);
    _sen55TempCanvasX = labelX + _canvas->textWidth("Temp:", _sen55OptionFont) + 2;
    _sen55TempCanvasY = labelY;

    // Humidity
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("RH:", labelX, labelY);
    _sen55HumiCanvasX = labelX + _canvas->textWidth("RH:", _sen55OptionFont) + 2;
    _sen55HumiCanvasY = labelY;

    // VOC
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("VOC:", labelX, labelY);
    _vocCanvasX = labelX + _canvas->textWidth("VOC:", _sen55OptionFont) + 2;
    _vocCanvasY = labelY;

    // NOx
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("NOx:", labelX, labelY);
    _noxCanvasX = labelX + _canvas->textWidth("NOx:", _sen55OptionFont) + 2;
    _noxCanvasY = labelY;

    // Create and initialize all canvases
    log_i("Creating SEN55 canvas objects");
    _pm1p0Canvas = new M5Canvas(_canvas);
    _pm2p5Canvas = new M5Canvas(_canvas);
    _pm4p0Canvas = new M5Canvas(_canvas);
    _pm10p0Canvas = new M5Canvas(_canvas);
    _sen55TempCanvas = new M5Canvas(_canvas);
    _sen55HumiCanvas = new M5Canvas(_canvas);
    _vocCanvas = new M5Canvas(_canvas);
    _noxCanvas = new M5Canvas(_canvas);

    // Initialize each canvas with proper dimensions
    _pm1p0Canvas->createSprite(40, 12);
    _pm2p5Canvas->createSprite(40, 12);
    _pm4p0Canvas->createSprite(40, 12);
    _pm10p0Canvas->createSprite(40, 12);
    _sen55TempCanvas->createSprite(40, 12);
    _sen55HumiCanvas->createSprite(40, 12);
    _vocCanvas->createSprite(40, 12);
    _noxCanvas->createSprite(40, 12);

    // Set common properties for all canvases
    _pm1p0Canvas->setBaseColor(TFT_WHITE);
    _pm1p0Canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _pm1p0Canvas->setTextDatum(TL_DATUM);
    _pm1p0Canvas->setTextSize(1);

    _pm2p5Canvas->setBaseColor(TFT_WHITE);
    _pm2p5Canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _pm2p5Canvas->setTextDatum(TL_DATUM);
    _pm2p5Canvas->setTextSize(1);

    _pm4p0Canvas->setBaseColor(TFT_WHITE);
    _pm4p0Canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _pm4p0Canvas->setTextDatum(TL_DATUM);
    _pm4p0Canvas->setTextSize(1);

    _pm10p0Canvas->setBaseColor(TFT_WHITE);
    _pm10p0Canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _pm10p0Canvas->setTextDatum(TL_DATUM);
    _pm10p0Canvas->setTextSize(1);

    _sen55TempCanvas->setBaseColor(TFT_WHITE);
    _sen55TempCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _sen55TempCanvas->setTextDatum(TL_DATUM);
    _sen55TempCanvas->setTextSize(1);

    _sen55HumiCanvas->setBaseColor(TFT_WHITE);
    _sen55HumiCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _sen55HumiCanvas->setTextDatum(TL_DATUM);
    _sen55HumiCanvas->setTextSize(1);

    _vocCanvas->setBaseColor(TFT_WHITE);
    _vocCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _vocCanvas->setTextDatum(TL_DATUM);
    _vocCanvas->setTextSize(1);

    _noxCanvas->setBaseColor(TFT_WHITE);
    _noxCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _noxCanvas->setTextDatum(TL_DATUM);
    _noxCanvas->setTextSize(1);

    // Draw initial values
    _pm1p0Canvas->fillSprite(TFT_WHITE);
    _pm1p0Canvas->drawString("0.0", 0, 0);
    _pm1p0Canvas->pushSprite(_pm1p0CanvasX, _pm1p0CanvasY);

    _pm2p5Canvas->fillSprite(TFT_WHITE);
    _pm2p5Canvas->drawString("0.0", 0, 0);
    _pm2p5Canvas->pushSprite(_pm2p5CanvasX, _pm2p5CanvasY);

    _pm4p0Canvas->fillSprite(TFT_WHITE);
    _pm4p0Canvas->drawString("0.0", 0, 0);
    _pm4p0Canvas->pushSprite(_pm4p0CanvasX, _pm4p0CanvasY);

    _pm10p0Canvas->fillSprite(TFT_WHITE);
    _pm10p0Canvas->drawString("0.0", 0, 0);
    _pm10p0Canvas->pushSprite(_pm10p0CanvasX, _pm10p0CanvasY);

    _sen55TempCanvas->fillSprite(TFT_WHITE);
    _sen55TempCanvas->drawString("0.0", 0, 0);
    _sen55TempCanvas->pushSprite(_sen55TempCanvasX, _sen55TempCanvasY);

    _sen55HumiCanvas->fillSprite(TFT_WHITE);
    _sen55HumiCanvas->drawString("0.0", 0, 0);
    _sen55HumiCanvas->pushSprite(_sen55HumiCanvasX, _sen55HumiCanvasY);

    _vocCanvas->fillSprite(TFT_WHITE);
    _vocCanvas->drawString("0.0", 0, 0);
    _vocCanvas->pushSprite(_vocCanvasX, _vocCanvasY);

    _noxCanvas->fillSprite(TFT_WHITE);
    _noxCanvas->drawString("0.0", 0, 0);
    _noxCanvas->pushSprite(_noxCanvasX, _noxCanvasY);

    log_i("SEN55 section initialized successfully");
}

void StatusView::initBME688()
{
    log_i("Initializing BME688 section");
    int32_t tempW = _columnWidth;
    int32_t tempH = 69;
    int32_t tempX = _bme688BaseCursorX;
    int32_t tempY = _bme688BaseCursorY;

    log_i("BME688 dimensions - Width: %ld, Height: %ld, X: %ld, Y: %ld", tempW, tempH, tempX, tempY);

    // Draw the container box
    _canvas->drawRect(tempX, tempY, tempW, tempH, TFT_BLACK);
    
    // Header background
    _canvas->fillRect(tempX + _border, tempY + _border, tempW - (_border * 2), _canvas->fontHeight(_bme688TitleFont) + (_padding * 2), TFT_BLACK);
    
    // Header text
    int32_t headerX = tempX + _border + _padding;
    int32_t headerY = tempY + _border + _padding;
    _canvas->fillRect(headerX, headerY, 5, _canvas->fontHeight(_bme688TitleFont), TFT_WHITE);
    headerX = headerX + 5 + 2;
    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->drawString("BME688", headerX, headerY, _bme688TitleFont);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);

    // Create and initialize all canvases once
    log_i("Creating BME688 canvas objects");
    _bme688TempCanvas = new M5Canvas(_canvas);
    _bme688HumiCanvas = new M5Canvas(_canvas);
    _bme688PressCanvas = new M5Canvas(_canvas);
    _bme688GasCanvas = new M5Canvas(_canvas);

    log_i("BME688 canvas objects created - Temp: %p, Humi: %p, Press: %p, Gas: %p",
          _bme688TempCanvas, _bme688HumiCanvas, _bme688PressCanvas, _bme688GasCanvas);

    // Initialize each canvas with smaller dimensions for just the values
    log_i("Creating BME688 sprites");
    _bme688TempCanvas->createSprite(40, 12);  // Smaller size for just the value
    _bme688HumiCanvas->createSprite(40, 12);
    _bme688PressCanvas->createSprite(40, 12);
    _bme688GasCanvas->createSprite(40, 12);

    log_i("BME688 sprites created successfully");

    // Set common properties for all canvases
    log_i("Setting BME688 canvas properties");
    _bme688TempCanvas->setBaseColor(TFT_WHITE);
    _bme688TempCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _bme688TempCanvas->setTextDatum(TL_DATUM);
    _bme688TempCanvas->setTextSize(1);

    _bme688HumiCanvas->setBaseColor(TFT_WHITE);
    _bme688HumiCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _bme688HumiCanvas->setTextDatum(TL_DATUM);
    _bme688HumiCanvas->setTextSize(1);

    _bme688PressCanvas->setBaseColor(TFT_WHITE);
    _bme688PressCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _bme688PressCanvas->setTextDatum(TL_DATUM);
    _bme688PressCanvas->setTextSize(1);

    _bme688GasCanvas->setBaseColor(TFT_WHITE);
    _bme688GasCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _bme688GasCanvas->setTextDatum(TL_DATUM);
    _bme688GasCanvas->setTextSize(1);

    log_i("BME688 canvas properties set successfully");

    // Draw labels on the main canvas
    int32_t labelX = tempX + _border + _padding;
    int32_t labelY = headerY + _canvas->fontHeight(_bme688TitleFont) + _padding;
    
    _canvas->drawString("Temp:", labelX, labelY);
    _canvas->drawString("Humi:", labelX, labelY + 15);
    _canvas->drawString("Press:", labelX, labelY + 30);
    _canvas->drawString("Gas:", labelX, labelY + 45);

    // Store positions for value updates
    _bme688TempCanvasX = labelX + 45;  // Position after labels
    _bme688TempCanvasY = labelY;
    _bme688HumiCanvasX = labelX + 45;
    _bme688HumiCanvasY = labelY + 15;
    _bme688PressCanvasX = labelX + 45;
    _bme688PressCanvasY = labelY + 30;
    _bme688GasCanvasX = labelX + 45;
    _bme688GasCanvasY = labelY + 45;

    // Draw initial values
    _bme688TempCanvas->fillSprite(TFT_WHITE);
    _bme688TempCanvas->drawString("0.0", 0, 0);
    _bme688TempCanvas->pushSprite(_bme688TempCanvasX, _bme688TempCanvasY);

    _bme688HumiCanvas->fillSprite(TFT_WHITE);
    _bme688HumiCanvas->drawString("0.0", 0, 0);
    _bme688HumiCanvas->pushSprite(_bme688HumiCanvasX, _bme688HumiCanvasY);

    _bme688PressCanvas->fillSprite(TFT_WHITE);
    _bme688PressCanvas->drawString("0.0", 0, 0);
    _bme688PressCanvas->pushSprite(_bme688PressCanvasX, _bme688PressCanvasY);

    _bme688GasCanvas->fillSprite(TFT_WHITE);
    _bme688GasCanvas->drawString("0.0", 0, 0);
    _bme688GasCanvas->pushSprite(_bme688GasCanvasX, _bme688GasCanvasY);

    log_i("BME688 section initialized successfully");
}

void StatusView::updatePower(uint32_t voltage)
{
    static uint32_t lastVoltage = 0;

    if (lastVoltage == voltage) {
        log_d("Power value unchanged: %lu", voltage);
        return;
    }

    log_i("Updating power display with voltage: %lu", voltage);
    if (!_voltageCanvas) {
        log_e("Voltage canvas not initialized");
        return;
    }

    char str[13] = { 0 };

    _voltageCanvas->clear(TFT_WHITE);

    float v = (((float)voltage / 1000) * 2);
    v = v > 4.2 ? 4.2 : v;

    sprintf(str, "BAT:%.2fV", v);
    log_i("Drawing power value: %s", str);
    _voltageCanvas->drawString(str, 0, 0, _poweroptionFont);

    _updateImpl(_voltageCanvas, _voltageCanvasX, _voltageCanvasY);

    lastVoltage = voltage;
    log_i("Power display updated with voltage: %.2fV", v);
}

void StatusView::updateCountdown(uint32_t seconds) {
    log_i("Updating countdown with seconds: %lu", seconds);
    if (!_chartCanvas) {
        log_e("Chart canvas not initialized");
        return;
    }

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

    log_i("Drawing countdown message: %s", msg.c_str());
    _chartCanvas->clear(TFT_WHITE);
    _chartCanvas->drawString(msg.c_str(), 0, 0, _poweroptionFont);
    _updateImpl(_chartCanvas, _chartCanvasX, _chartCanvasY);
    log_i("Countdown update completed");
}

void StatusView::displayCountdown(uint32_t seconds) {
    log_i("Displaying countdown with seconds: %lu", seconds);
    if (!_chartCanvas1) {
        log_e("Chart canvas 1 not initialized");
        return;
    }

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

    log_i("Drawing countdown display: %s", msg.c_str());
    _chartCanvas1->clear(TFT_WHITE);
    _chartCanvas1->drawString(msg.c_str(), 0, 0, _poweroptionFont);
    _lcd->fillRect(_chartCanvasX + _chartCanvas1->textWidth("Time:", _poweroptionFont), 
                  _chartCanvasY, _chartCanvas1->width(), _chartCanvas1->height(), TFT_WHITE);
    _chartCanvas1->pushSprite(_chartCanvasX + _chartCanvas1->textWidth("Time:", _poweroptionFont), _chartCanvasY);
    _lcd->waitDisplay();

    updateCountdown(seconds);
    log_i("Countdown display completed");
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
    log_i("Starting SEN55 update with values:");
    log_i("  PM1.0: %.2f, PM2.5: %.2f, PM4.0: %.2f, PM10: %.2f",
          massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0, massConcentrationPm10p0);
    log_i("  Temp: %.2f, RH: %.2f, VOC: %.2f, NOx: %.2f",
          ambientTemperature, ambientHumidity, vocIndex, noxIndex);

    // Check canvas initialization
    if (!_pm1p0Canvas || !_pm2p5Canvas || !_pm4p0Canvas || !_pm10p0Canvas ||
        !_sen55TempCanvas || !_sen55HumiCanvas || !_vocCanvas || !_noxCanvas) {
        log_e("SEN55 canvas objects not properly initialized");
        return;
    }

    char str[8] = { 0 };

    // Clear all canvases
    log_i("Clearing SEN55 canvases");
    _pm1p0Canvas->clear(TFT_WHITE);
    _pm2p5Canvas->clear(TFT_WHITE);
    _pm4p0Canvas->clear(TFT_WHITE);
    _pm10p0Canvas->clear(TFT_WHITE);
    _sen55TempCanvas->clear(TFT_WHITE);
    _sen55HumiCanvas->clear(TFT_WHITE);
    _vocCanvas->clear(TFT_WHITE);
    _noxCanvas->clear(TFT_WHITE);

    // Update each value
    sprintf(str, "%.2f", massConcentrationPm1p0);
    log_i("Updating PM1.0: %s", str);
    _pm1p0Canvas->drawRightString(str, _pm1p0Canvas->width(), 0, _sen55OptionFont);
    _pm1p0Canvas->pushSprite(_pm1p0CanvasX, _pm1p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm2p5);
    log_i("Updating PM2.5: %s", str);
    _pm2p5Canvas->drawRightString(str, _pm2p5Canvas->width(), 0, _sen55OptionFont);
    _pm2p5Canvas->pushSprite(_pm2p5CanvasX, _pm2p5CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm4p0);
    log_i("Updating PM4.0: %s", str);
    _pm4p0Canvas->drawRightString(str, _pm4p0Canvas->width(), 0, _sen55OptionFont);
    _pm4p0Canvas->pushSprite(_pm4p0CanvasX, _pm4p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm10p0);
    log_i("Updating PM10: %s", str);
    _pm10p0Canvas->drawRightString(str, _pm10p0Canvas->width(), 0, _sen55OptionFont);
    _pm10p0Canvas->pushSprite(_pm10p0CanvasX, _pm10p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", ambientTemperature);
    log_i("Updating Temperature: %s", str);
    _sen55TempCanvas->drawRightString(str, _sen55TempCanvas->width(), 0, _sen55OptionFont);
    _sen55TempCanvas->pushSprite(_sen55TempCanvasX, _sen55TempCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", ambientHumidity);
    log_i("Updating Humidity: %s", str);
    _sen55HumiCanvas->drawRightString(str, _sen55HumiCanvas->width(), 0, _sen55OptionFont);
    _sen55HumiCanvas->pushSprite(_sen55HumiCanvasX, _sen55HumiCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", vocIndex);
    log_i("Updating VOC: %s", str);
    _vocCanvas->drawRightString(str, _pm1p0Canvas->width(), 0, _sen55OptionFont);
    _vocCanvas->pushSprite(_vocCanvasX, _vocCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", noxIndex);
    log_i("Updating NOx: %s", str);
    _noxCanvas->drawRightString(str, _noxCanvas->width(), 0, _sen55OptionFont);
    _noxCanvas->pushSprite(_noxCanvasX, _noxCanvasY);

    log_i("SEN55 update completed successfully");
}

void StatusView::displayNetworkStatus(const char *title, const char *msg) {
    log_i("Displaying network status - Title: %s, Message: %s", title, msg);
    if (!_statusTitleCanvas1 || !_statusMsgCanvas1) {
        log_e("Status canvas objects not initialized");
        return;
    }

    _statusTitleCanvas1->clear(TFT_WHITE);
    _statusMsgCanvas1->clear(TFT_WHITE);
    _statusTitleCanvas1->drawCenterString(title, _statusTitleCanvas1->width() / 2, 0, _statusTitleFont);
    _statusMsgCanvas1->drawCenterString(msg, _statusMsgCanvas1->width() / 2, 0, _statusMsgFont);

    _updateImpl(_statusTitleCanvas1, _statusTitleCanvasX, _statusTitleCanvasY);
    _updateImpl(_statusMsgCanvas1, _statusMsgCanvasX, _statusMsgCanvasY);
    updateNetworkStatus(title, msg);
    log_i("Network status display completed");
}

void StatusView::updateNetworkStatus(const char *title, const char *msg) {
    log_i("Updating network status - Title: %s, Message: %s", title, msg);
    if (!_statusTitleCanvas || !_statusMsgCanvas) {
        log_e("Status canvas objects not initialized");
        return;
    }

    _statusTitleCanvas->clear(TFT_WHITE);
    _statusMsgCanvas->clear(TFT_WHITE);
    _statusTitleCanvas->drawCenterString(title, _statusTitleCanvas->width() / 2, 0, _statusTitleFont);
    _statusMsgCanvas->drawCenterString(msg, _statusMsgCanvas->width() / 2, 0, _statusMsgFont);

    _updateImpl(_statusTitleCanvas, _statusTitleCanvasX, _statusTitleCanvasY);
    _updateImpl(_statusMsgCanvas, _statusMsgCanvasX, _statusMsgCanvasY);
    log_i("Network status update completed");
}

void StatusView::displayNickname(String &nickname) {
    log_i("Displaying nickname: %s", nickname.c_str());
    if (!_nicknameCanvas1) {
        log_e("Nickname canvas 1 not initialized");
        return;
    }

    const lgfx::IFont* fontTable[] = {&fonts::efontCN_16, &fonts::efontCN_14, &fonts::efontCN_12, &fonts::efontCN_10};
    size_t i = 0;

    for (i = 0; i < 4; i++) {
        if (_nicknameCanvas1->textWidth(nickname, fontTable[i]) > 95) {
            continue;
        } else {
            break;
        }
    }

    log_i("Selected font index: %d", i);
    if (i >= 4) {
        splitLongString(nickname, 95, fontTable[3]);
        i = 3;
    }

    _nicknameCanvas1->clear();
    int32_t cursorX = 97 / 2;
    int32_t cursorY = (32 - _canvas->fontHeight(fontTable[i])) / 2;
    _nicknameCanvas1->drawCenterString(nickname, cursorX, cursorY, fontTable[i]);

    _updateImpl(_nicknameCanvas1, _nicknameCanvasX, _nicknameCanvasY);
    updateNickname(nickname);
    log_i("Nickname display completed");
}

void StatusView::updateNickname(String &nickname) {
    log_i("Updating nickname: %s", nickname.c_str());
    if (!_nicknameCanvas) {
        log_e("Nickname canvas not initialized");
        return;
    }

    const lgfx::IFont* fontTable[] = {&fonts::efontCN_16, &fonts::efontCN_14, &fonts::efontCN_12, &fonts::efontCN_10};
    size_t i = 0;

    for (i = 0; i < 4; i++) {
        if (_nicknameCanvas->textWidth(nickname, fontTable[i]) > 95) {
            continue;
        } else {
            break;
        }
    }

    log_i("Selected font index: %d", i);
    if (i >= 4) {
        splitLongString(nickname, 95, fontTable[3]);
        i = 3;
    }

    _nicknameCanvas->clear();
    int32_t cursorX = 97 / 2;
    int32_t cursorY = (32 - _nicknameCanvas->fontHeight(fontTable[i])) / 2;
    _nicknameCanvas->drawCenterString(nickname, cursorX, cursorY, fontTable[i]);

    _updateImpl(_nicknameCanvas, _nicknameCanvasX, _nicknameCanvasY);
    log_i("Nickname update completed");
}

void StatusView::load() {
    log_i("Loading view");
    _lcd->clear(TFT_WHITE);
    _lcd->waitDisplay();
    log_i("Pushing main canvas to display");
    _updateImpl(_canvas, 0, 0);
    _lcd->waitDisplay();
    log_i("View loaded successfully");
}

void StatusView::disappear() {
    log_i("View disappearing");
    _lcd->clear();
    log_i("View disappeared");
}

void StatusView::_updateImpl(M5Canvas *canvas, int32_t x, int32_t y)
{
    log_d("Updating canvas at position (%ld, %ld)", x, y);
    if (!canvas) {
        log_e("Attempted to update null canvas");
        return;
    }
    canvas->pushSprite(x, y);
    log_d("Canvas update complete");
}

void StatusView::splitLongString(String &text, int32_t maxWidth, const lgfx::IFont* font) {
    log_i("Splitting long string: %s", text.c_str());
    int32_t w = _lcd->textWidth(text, font);
    int32_t start = 1;
    int32_t end = 0;
    if (w < maxWidth) {
        log_i("String fits within max width, no splitting needed");
        return;
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
    log_i("Split string result: %s", text.c_str());
}

void StatusView::updateBME688(float temperature, float humidity, float pressure, float gasResistance) {
    log_i("Starting BME688 update with values - Temp: %.2f, Humi: %.2f, Press: %.2f, Gas: %.2f",
          temperature, humidity, pressure, gasResistance);

    // Check if canvas objects are properly initialized
    if (!_bme688TempCanvas || !_bme688HumiCanvas || !_bme688PressCanvas || !_bme688GasCanvas) {
        log_e("BME688 canvas objects not initialized - Temp: %p, Humi: %p, Press: %p, Gas: %p",
              _bme688TempCanvas, _bme688HumiCanvas, _bme688PressCanvas, _bme688GasCanvas);
        return;
    }

    // Check if base cursor position is valid
    if (_bme688BaseCursorX < 0 || _bme688BaseCursorY < 0) {
        log_e("Invalid BME688 base cursor position - X: %ld, Y: %ld", _bme688BaseCursorX, _bme688BaseCursorY);
        return;
    }

    // Bounds checking for temperature
    if (temperature < -40.0f || temperature > 85.0f) {
        log_w("Temperature out of bounds: %.2f", temperature);
        return;
    }

    // Bounds checking for humidity
    if (humidity < 0.0f || humidity > 100.0f) {
        log_w("Humidity out of bounds: %.2f", humidity);
        return;
    }

    // Bounds checking for pressure
    if (pressure < 300.0f || pressure > 1100.0f) {
        log_w("Pressure out of bounds: %.2f", pressure);
        return;
    }

    // Bounds checking for gas resistance
    if (gasResistance < 0.0f || gasResistance > 100000.0f) {
        log_w("Gas resistance out of bounds: %.2f", gasResistance);
        return;
    }

    char str[16] = { 0 };

    // Update temperature display
    log_i("Updating temperature display");
    _bme688TempCanvas->fillSprite(TFT_WHITE);
    snprintf(str, sizeof(str), "%.1f", temperature);
    _bme688TempCanvas->drawString(str, 0, 0);
    log_i("Pushing temperature canvas to display at (%ld, %ld)", _bme688TempCanvasX, _bme688TempCanvasY);
    _bme688TempCanvas->pushSprite(_bme688TempCanvasX, _bme688TempCanvasY);

    // Update humidity display
    log_i("Updating humidity display");
    _bme688HumiCanvas->fillSprite(TFT_WHITE);
    snprintf(str, sizeof(str), "%.1f", humidity);
    _bme688HumiCanvas->drawString(str, 0, 0);
    log_i("Pushing humidity canvas to display at (%ld, %ld)", _bme688HumiCanvasX, _bme688HumiCanvasY);
    _bme688HumiCanvas->pushSprite(_bme688HumiCanvasX, _bme688HumiCanvasY);

    // Update pressure display
    log_i("Updating pressure display");
    _bme688PressCanvas->fillSprite(TFT_WHITE);
    snprintf(str, sizeof(str), "%.1f", pressure);
    _bme688PressCanvas->drawString(str, 0, 0);
    log_i("Pushing pressure canvas to display at (%ld, %ld)", _bme688PressCanvasX, _bme688PressCanvasY);
    _bme688PressCanvas->pushSprite(_bme688PressCanvasX, _bme688PressCanvasY);

    // Update gas resistance display
    log_i("Updating gas resistance display");
    _bme688GasCanvas->fillSprite(TFT_WHITE);
    snprintf(str, sizeof(str), "%.1f", gasResistance);
    _bme688GasCanvas->drawString(str, 0, 0);
    log_i("Pushing gas resistance canvas to display at (%ld, %ld)", _bme688GasCanvasX, _bme688GasCanvasY);
    _bme688GasCanvas->pushSprite(_bme688GasCanvasX, _bme688GasCanvasY);

    log_i("BME688 update completed successfully");
}

void StatusView::updateSCD40(uint16_t co2, float temperature, float humidity)
{
    log_i("Starting SCD40 update - CO2: %d, Temp: %.2f, Humi: %.2f", co2, temperature, humidity);

    // Check if canvas objects are properly initialized
    if (!_co2Canvas || !_tempCanvas || !_humiCanvas) {
        log_e("SCD40 canvas objects not properly initialized - CO2: %p, Temp: %p, Humi: %p",
              _co2Canvas, _tempCanvas, _humiCanvas);
        return;
    }

    // Use a larger buffer to prevent overflow
    char str[16] = { 0 };

    // Clear all canvases
    log_i("Clearing SCD40 canvases");
    _co2Canvas->clear(TFT_WHITE);
    _tempCanvas->clear(TFT_WHITE);
    _humiCanvas->clear(TFT_WHITE);

    // Update CO2 with bounds checking
    if (co2 >= 400 && co2 <= 5000) {
        snprintf(str, sizeof(str), "%d", co2);
        log_i("Updating CO2 display: %s", str);
        _co2Canvas->drawRightString(str, _co2Canvas->width(), 0, _scd40OptionFont);
        _co2Canvas->pushSprite(_co2CanvasX, _co2CanvasY);
    } else {
        log_w("CO2 value out of bounds: %d", co2);
    }

    // Update temperature with bounds checking
    if (!isnan(temperature) && temperature >= -10.0f && temperature <= 60.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", temperature);
        log_i("Updating temperature display: %s", str);
        _tempCanvas->drawRightString(str, _tempCanvas->width(), 0, _scd40OptionFont);
        _tempCanvas->pushSprite(_tempCanvasX, _tempCanvasY);
    } else {
        log_w("Temperature value out of bounds: %.2f", temperature);
    }

    // Update humidity with bounds checking
    if (!isnan(humidity) && humidity >= 0.0f && humidity <= 100.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", humidity);
        log_i("Updating humidity display: %s", str);
        _humiCanvas->drawRightString(str, _humiCanvas->width(), 0, _scd40OptionFont);
        _humiCanvas->pushSprite(_humiCanvasX, _humiCanvasY);
    } else {
        log_w("Humidity value out of bounds: %.2f", humidity);
    }

    log_i("SCD40 update completed");
}
