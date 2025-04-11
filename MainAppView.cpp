#include "MainAppView.hpp"

StatusView::StatusView() {
    log_i("Default constructor called");
}

StatusView::StatusView(LGFX_Device *lcd, M5Canvas *canvas) {
    log_i("Constructor called with lcd and canvas");
    _lcd = lcd;
    _canvas = canvas;

    // Initialize column width and cursor positions with fixed LCD dimensions
    _columnWidth = 96;  // (200 - 4) / 2 = 98 for each column
    log_i("Initialized column width: %ld", _columnWidth);

    // Set right column positions
    _bme688BaseCursorX = 2;    // Start at left column
    _bme688BaseCursorY = 2 + 65 + 2;  // Position below SCD40 section
    _powerBaseCursorX = 100;   // Start at right column (200/2)
    _powerBaseCursorY = 2 + 135 + 2;  // Position below SEN55 section
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
    log_i("  Power - Voltage: %p",
          _voltageCanvas);
}

void StatusView::begin() {
    log_i("begin() called");

    // Display splash screen first
    displaySplashScreen();

    // Create sprite with full LCD dimensions
    log_i("Creating main canvas sprite with dimensions: %dx%d", _lcd->width(), _lcd->height());
    _canvas->createSprite(_lcd->width(), _lcd->height());
    _canvas->setBaseColor(TFT_BLACK);
    _canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _canvas->fillSprite(TFT_WHITE);

    // No vertical divider line in the new design
    log_i("No vertical divider line in the new design");

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
    log_i("UI initialization complete");
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

    // Add height for battery percentage
    tempH = _border
            + _padding
            + _canvas->fontHeight(_powerTitleFont)
            + _padding
            + _canvas->fontHeight(_poweroptionFont)
            + _padding
            + _canvas->fontHeight(_poweroptionFont)  // Added for battery percentage
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

    // Battery voltage label
    tempX = _powerBaseCursorX + _border + _padding;
    tempY = tempY + _canvas->fontHeight(_powerTitleFont) + _padding;
    _canvas->drawString("BAT V:", tempX, tempY, _poweroptionFont);

    // Calculate right edge of the power section for right-aligned values
    int32_t rightEdge = _powerBaseCursorX + tempW - _border - _padding;

    // Battery voltage value position (right-aligned)
    _voltageCanvasX = rightEdge - _canvas->textWidth("0.00V", _poweroptionFont);
    _voltageCanvasY = tempY;

    // Create voltage canvas with proper dimensions
    _voltageCanvas = new M5Canvas(_canvas);
    _voltageCanvas->createSprite(
        _voltageCanvas->textWidth("0.00V", _poweroptionFont),
        _voltageCanvas->fontHeight(_poweroptionFont)
    );
    _voltageCanvas->setBaseColor(TFT_WHITE);
    _voltageCanvas->setTextColor(TFT_BLACK, TFT_WHITE);

    // Battery percentage label
    tempY = tempY + _canvas->fontHeight(_poweroptionFont) + _padding;
    _canvas->drawString("BAT %:", tempX, tempY, _poweroptionFont);

    // Battery percentage value position (right-aligned)
    _percentageCanvasX = rightEdge - _canvas->textWidth("100%", _poweroptionFont);
    _percentageCanvasY = tempY;

    // Create percentage canvas with proper dimensions
    _percentageCanvas = new M5Canvas(_canvas);
    _percentageCanvas->createSprite(
        _percentageCanvas->textWidth("100%", _poweroptionFont),
        _percentageCanvas->fontHeight(_poweroptionFont)
    );
    _percentageCanvas->setBaseColor(TFT_WHITE);
    _percentageCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
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

    // Calculate right edge of the section for right-aligned values
    int32_t rightEdge = _sen55BaseCursorX + tempW - _border - _padding;

    // Draw labels and create canvases for each value
    int32_t labelX = tempX;
    int32_t labelY = tempY + _canvas->fontHeight(_sen55TitleFont) + _padding;

    // PM1.0
    _canvas->drawString("PM1.0:", labelX, labelY);
    _pm1p0CanvasX = rightEdge - _canvas->textWidth("000.00", _sen55OptionFont);
    _pm1p0CanvasY = labelY;

    // PM2.5
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM2.5:", labelX, labelY);
    _pm2p5CanvasX = rightEdge - _canvas->textWidth("000.00", _sen55OptionFont);
    _pm2p5CanvasY = labelY;

    // PM4.0
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM4.0:", labelX, labelY);
    _pm4p0CanvasX = rightEdge - _canvas->textWidth("000.00", _sen55OptionFont);
    _pm4p0CanvasY = labelY;

    // PM10.0
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("PM10:", labelX, labelY);
    _pm10p0CanvasX = rightEdge - _canvas->textWidth("000.00", _sen55OptionFont);
    _pm10p0CanvasY = labelY;

    // Temperature
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("Temp:", labelX, labelY);
    _sen55TempCanvasX = rightEdge - _canvas->textWidth("000.00", _sen55OptionFont);
    _sen55TempCanvasY = labelY;

    // Humidity
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("RH:", labelX, labelY);
    _sen55HumiCanvasX = rightEdge - _canvas->textWidth("000.00", _sen55OptionFont);
    _sen55HumiCanvasY = labelY;

    // VOC
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("VOC:", labelX, labelY);
    _vocCanvasX = rightEdge - _canvas->textWidth("000.00", _sen55OptionFont);
    _vocCanvasY = labelY;

    // NOx
    labelY += _canvas->fontHeight(_sen55OptionFont) + _padding;
    _canvas->drawString("NOx:", labelX, labelY);
    _noxCanvasX = rightEdge - _canvas->textWidth("000.00", _sen55OptionFont);
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

    // Initialize each canvas with dynamic dimensions based on maximum expected text width
    _pm1p0Canvas->createSprite(
        _pm1p0Canvas->textWidth("000.00", _sen55OptionFont),
        _pm1p0Canvas->fontHeight(_sen55OptionFont)
    );
    _pm2p5Canvas->createSprite(
        _pm2p5Canvas->textWidth("000.00", _sen55OptionFont),
        _pm2p5Canvas->fontHeight(_sen55OptionFont)
    );
    _pm4p0Canvas->createSprite(
        _pm4p0Canvas->textWidth("000.00", _sen55OptionFont),
        _pm4p0Canvas->fontHeight(_sen55OptionFont)
    );
    _pm10p0Canvas->createSprite(
        _pm10p0Canvas->textWidth("000.00", _sen55OptionFont),
        _pm10p0Canvas->fontHeight(_sen55OptionFont)
    );
    _sen55TempCanvas->createSprite(
        _sen55TempCanvas->textWidth("000.00", _sen55OptionFont),
        _sen55TempCanvas->fontHeight(_sen55OptionFont)
    );
    _sen55HumiCanvas->createSprite(
        _sen55HumiCanvas->textWidth("000.00", _sen55OptionFont),
        _sen55HumiCanvas->fontHeight(_sen55OptionFont)
    );
    _vocCanvas->createSprite(
        _vocCanvas->textWidth("000.00", _sen55OptionFont),
        _vocCanvas->fontHeight(_sen55OptionFont)
    );
    _noxCanvas->createSprite(
        _noxCanvas->textWidth("000.00", _sen55OptionFont),
        _noxCanvas->fontHeight(_sen55OptionFont)
    );

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
    _pm1p0Canvas->clear();
    _pm1p0Canvas->drawRightString("0.0", _pm1p0Canvas->width(), 0, _sen55OptionFont);
    _updateImpl(_pm1p0Canvas, _pm1p0CanvasX, _pm1p0CanvasY);

    _pm2p5Canvas->clear();
    _pm2p5Canvas->drawRightString("0.0", _pm2p5Canvas->width(), 0, _sen55OptionFont);
    _updateImpl(_pm2p5Canvas, _pm2p5CanvasX, _pm2p5CanvasY);

    _pm4p0Canvas->clear();
    _pm4p0Canvas->drawRightString("0.0", _pm4p0Canvas->width(), 0, _sen55OptionFont);
    _updateImpl(_pm4p0Canvas, _pm4p0CanvasX, _pm4p0CanvasY);

    _pm10p0Canvas->clear();
    _pm10p0Canvas->drawRightString("0.0", _pm10p0Canvas->width(), 0, _sen55OptionFont);
    _updateImpl(_pm10p0Canvas, _pm10p0CanvasX, _pm10p0CanvasY);

    _sen55TempCanvas->clear();
    _sen55TempCanvas->drawRightString("0.0", _sen55TempCanvas->width(), 0, _sen55OptionFont);
    _updateImpl(_sen55TempCanvas, _sen55TempCanvasX, _sen55TempCanvasY);

    _sen55HumiCanvas->clear();
    _sen55HumiCanvas->drawRightString("0.0", _sen55HumiCanvas->width(), 0, _sen55OptionFont);
    _updateImpl(_sen55HumiCanvas, _sen55HumiCanvasX, _sen55HumiCanvasY);

    _vocCanvas->clear();
    _vocCanvas->drawRightString("0.0", _vocCanvas->width(), 0, _sen55OptionFont);
    _updateImpl(_vocCanvas, _vocCanvasX, _vocCanvasY);

    _noxCanvas->clear();
    _noxCanvas->drawRightString("0.0", _noxCanvas->width(), 0, _sen55OptionFont);
    _updateImpl(_noxCanvas, _noxCanvasX, _noxCanvasY);

    log_i("SEN55 section initialized successfully");
}

void StatusView::initBME688()
{
    log_i("Initializing BME688 section");
    int32_t tempW = _columnWidth;
    int32_t tempH = 110; // Increased height to accommodate two more values
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
    _bme688IaqCanvas = new M5Canvas(_canvas);
    _bme688Co2EqCanvas = new M5Canvas(_canvas);

    log_i("BME688 canvas objects created - Temp: %p, Humi: %p, Press: %p, Gas: %p, IAQ: %p, CO2Eq: %p",
          _bme688TempCanvas, _bme688HumiCanvas, _bme688PressCanvas, _bme688GasCanvas, 
          _bme688IaqCanvas, _bme688Co2EqCanvas);

    // Initialize each canvas with dynamic dimensions based on maximum expected text width
    log_i("Creating BME688 sprites");
    _bme688TempCanvas->createSprite(
        _bme688TempCanvas->textWidth("000.0", _bme688OptionFont),
        _bme688TempCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688HumiCanvas->createSprite(
        _bme688HumiCanvas->textWidth("000.0", _bme688OptionFont),
        _bme688HumiCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688PressCanvas->createSprite(
        _bme688PressCanvas->textWidth("0000.0", _bme688OptionFont),  // Larger for pressure values
        _bme688PressCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688GasCanvas->createSprite(
        _bme688GasCanvas->textWidth("100000.0", _bme688OptionFont),  // Much larger for gas resistance values
        _bme688GasCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688IaqCanvas->createSprite(
        _bme688IaqCanvas->textWidth("000.0", _bme688OptionFont),
        _bme688IaqCanvas->fontHeight(_bme688OptionFont)
    );
    _bme688Co2EqCanvas->createSprite(
        _bme688Co2EqCanvas->textWidth("10000.0", _bme688OptionFont),  // Larger for CO2 equivalent values
        _bme688Co2EqCanvas->fontHeight(_bme688OptionFont)
    );

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

    _bme688IaqCanvas->setBaseColor(TFT_WHITE);
    _bme688IaqCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _bme688IaqCanvas->setTextDatum(TL_DATUM);
    _bme688IaqCanvas->setTextSize(1);

    _bme688Co2EqCanvas->setBaseColor(TFT_WHITE);
    _bme688Co2EqCanvas->setTextColor(TFT_BLACK, TFT_WHITE);
    _bme688Co2EqCanvas->setTextDatum(TL_DATUM);
    _bme688Co2EqCanvas->setTextSize(1);

    log_i("BME688 canvas properties set successfully");

    // Calculate right edge of the section for right-aligned values
    int32_t rightEdge = _bme688BaseCursorX + tempW - _border - _padding;

    // Draw labels on the main canvas
    int32_t labelX = tempX + _border + _padding;
    int32_t labelY = headerY + _canvas->fontHeight(_bme688TitleFont) + _padding;

    // Calculate dynamic Y increments based on font height and padding
    int32_t yIncrement = _canvas->fontHeight(_bme688OptionFont) + _padding;

    _canvas->drawString("Temp:", labelX, labelY);

    labelY += yIncrement;
    _canvas->drawString("Humi:", labelX, labelY);

    labelY += yIncrement;
    _canvas->drawString("Press:", labelX, labelY);

    labelY += yIncrement;
    _canvas->drawString("Gas:", labelX, labelY);

    labelY += yIncrement;
    _canvas->drawString("IAQ:", labelX, labelY);

    labelY += yIncrement;
    _canvas->drawString("CO2eq:", labelX, labelY);

    // Reset labelY to the initial value for canvas positioning
    labelY = headerY + _canvas->fontHeight(_bme688TitleFont) + _padding;

    // Store positions for value updates - right-aligned
    _bme688TempCanvasX = rightEdge - _canvas->textWidth("000.0", _bme688OptionFont);
    _bme688TempCanvasY = labelY;

    labelY += yIncrement;
    _bme688HumiCanvasX = rightEdge - _canvas->textWidth("000.0", _bme688OptionFont);
    _bme688HumiCanvasY = labelY;

    labelY += yIncrement;
    _bme688PressCanvasX = rightEdge - _canvas->textWidth("0000.0", _bme688OptionFont);
    _bme688PressCanvasY = labelY;

    labelY += yIncrement;
    _bme688GasCanvasX = rightEdge - _canvas->textWidth("100000.0", _bme688OptionFont);
    _bme688GasCanvasY = labelY;

    labelY += yIncrement;
    _bme688IaqCanvasX = rightEdge - _canvas->textWidth("000.0", _bme688OptionFont);
    _bme688IaqCanvasY = labelY;

    labelY += yIncrement;
    _bme688Co2EqCanvasX = rightEdge - _canvas->textWidth("10000.0", _bme688OptionFont);
    _bme688Co2EqCanvasY = labelY;

    // Draw initial values
    _bme688TempCanvas->clear();
    _bme688TempCanvas->drawRightString("0.0", _bme688TempCanvas->width(), 0, _bme688OptionFont);
    _updateImpl(_bme688TempCanvas, _bme688TempCanvasX, _bme688TempCanvasY);

    _bme688HumiCanvas->clear();
    _bme688HumiCanvas->drawRightString("0.0", _bme688HumiCanvas->width(), 0, _bme688OptionFont);
    _updateImpl(_bme688HumiCanvas, _bme688HumiCanvasX, _bme688HumiCanvasY);

    _bme688PressCanvas->clear();
    _bme688PressCanvas->drawRightString("0.0", _bme688PressCanvas->width(), 0, _bme688OptionFont);
    _updateImpl(_bme688PressCanvas, _bme688PressCanvasX, _bme688PressCanvasY);

    _bme688GasCanvas->clear();
    _bme688GasCanvas->drawRightString("0.0", _bme688GasCanvas->width(), 0, _bme688OptionFont);
    _updateImpl(_bme688GasCanvas, _bme688GasCanvasX, _bme688GasCanvasY);

    _bme688IaqCanvas->clear();
    _bme688IaqCanvas->drawRightString("0.0", _bme688IaqCanvas->width(), 0, _bme688OptionFont);
    _updateImpl(_bme688IaqCanvas, _bme688IaqCanvasX, _bme688IaqCanvasY);

    _bme688Co2EqCanvas->clear();
    _bme688Co2EqCanvas->drawRightString("0.0", _bme688Co2EqCanvas->width(), 0, _bme688OptionFont);
    _updateImpl(_bme688Co2EqCanvas, _bme688Co2EqCanvasX, _bme688Co2EqCanvasY);

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
    if (!_voltageCanvas || !_percentageCanvas) {
        log_e("Power canvases not initialized - Voltage: %p, Percentage: %p", 
              _voltageCanvas, _percentageCanvas);
        return;
    }

    char str[13] = { 0 };

    // Clear canvases
    _voltageCanvas->clear();
    _percentageCanvas->clear();

    // Calculate voltage
    float v = (((float)voltage / 1000) * 2);
    v = v > 4.2 ? 4.2 : v;

    // Calculate battery percentage (simple linear mapping)
    // 3.0V = 0%, 4.2V = 100%
    int percentage = (int)((v - 3.0) / 1.2 * 100);
    percentage = percentage < 0 ? 0 : percentage;
    percentage = percentage > 100 ? 100 : percentage;

    // Format and display voltage
    sprintf(str, "%.2fV", v);
    log_i("Drawing voltage value: %s", str);
    _voltageCanvas->clear();
    _voltageCanvas->drawRightString(str, _voltageCanvas->width(), 0, _poweroptionFont);
    _updateImpl(_voltageCanvas, _voltageCanvasX, _voltageCanvasY);

    // Format and display percentage
    sprintf(str, "%d%%", percentage);
    log_i("Drawing percentage value: %s", str);
    _percentageCanvas->clear();
    _percentageCanvas->drawRightString(str, _percentageCanvas->width(), 0, _poweroptionFont);
    _updateImpl(_percentageCanvas, _percentageCanvasX, _percentageCanvasY);

    lastVoltage = voltage;
    log_i("Power display updated - Voltage: %.2fV, Percentage: %d%%", v, percentage);
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
    _pm1p0Canvas->clear();
    _pm2p5Canvas->clear();
    _pm4p0Canvas->clear();
    _pm10p0Canvas->clear();
    _sen55TempCanvas->clear();
    _sen55HumiCanvas->clear();
    _vocCanvas->clear();
    _noxCanvas->clear();

    // Update each value
    sprintf(str, "%.2f", massConcentrationPm1p0);
    log_i("Updating PM1.0: %s", str);
    _pm1p0Canvas->drawRightString(str, _pm1p0Canvas->width(), 0, _sen55OptionFont);
    _updateImpl(_pm1p0Canvas, _pm1p0CanvasX, _pm1p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm2p5);
    log_i("Updating PM2.5: %s", str);
    _pm2p5Canvas->drawRightString(str, _pm2p5Canvas->width(), 0, _sen55OptionFont);
    _updateImpl(_pm2p5Canvas, _pm2p5CanvasX, _pm2p5CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm4p0);
    log_i("Updating PM4.0: %s", str);
    _pm4p0Canvas->drawRightString(str, _pm4p0Canvas->width(), 0, _sen55OptionFont);
    _updateImpl(_pm4p0Canvas, _pm4p0CanvasX, _pm4p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", massConcentrationPm10p0);
    log_i("Updating PM10: %s", str);
    _pm10p0Canvas->drawRightString(str, _pm10p0Canvas->width(), 0, _sen55OptionFont);
    _updateImpl(_pm10p0Canvas, _pm10p0CanvasX, _pm10p0CanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", ambientTemperature);
    log_i("Updating Temperature: %s", str);
    _sen55TempCanvas->drawRightString(str, _sen55TempCanvas->width(), 0, _sen55OptionFont);
    _updateImpl(_sen55TempCanvas, _sen55TempCanvasX, _sen55TempCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", ambientHumidity);
    log_i("Updating Humidity: %s", str);
    _sen55HumiCanvas->drawRightString(str, _sen55HumiCanvas->width(), 0, _sen55OptionFont);
    _updateImpl(_sen55HumiCanvas, _sen55HumiCanvasX, _sen55HumiCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", vocIndex);
    log_i("Updating VOC: %s", str);
    _vocCanvas->drawRightString(str, _vocCanvas->width(), 0, _sen55OptionFont);
    _updateImpl(_vocCanvas, _vocCanvasX, _vocCanvasY);

    memset(str, 0, sizeof(str));
    sprintf(str, "%.2f", noxIndex);
    log_i("Updating NOx: %s", str);
    _noxCanvas->drawRightString(str, _noxCanvas->width(), 0, _sen55OptionFont);
    _updateImpl(_noxCanvas, _noxCanvasX, _noxCanvasY);

    // Wait for display to complete refresh
    _lcd->waitDisplay();

    log_i("SEN55 update completed successfully");
}



void StatusView::displaySplashScreen() {
    log_i("Displaying splash screen");

    // Clear the screen
    _lcd->clear(TFT_WHITE);
    _lcd->waitDisplay();

    // Create a temporary canvas for the splash screen
    M5Canvas splashCanvas(_lcd);
    splashCanvas.createSprite(_lcd->width(), _lcd->height());
    splashCanvas.setBaseColor(TFT_WHITE);
    splashCanvas.setTextColor(TFT_BLACK, TFT_WHITE);
    splashCanvas.fillSprite(TFT_WHITE);

    // Draw the logo (simplified version of the SVG)
    int centerX = _lcd->width() / 2;
    int centerY = _lcd->height() / 2 - 20;
    int radius = 60;

    // Draw outer circle
    splashCanvas.drawCircle(centerX, centerY, radius, TFT_BLACK);

    // Draw inner design (simplified representation of the logo)
    for (int i = 0; i < 5; i++) {
        float angle = i * 2 * PI / 5;
        int x1 = centerX + cos(angle) * radius * 0.5;
        int y1 = centerY + sin(angle) * radius * 0.5;
        int x2 = centerX + cos(angle + PI/5) * radius * 0.8;
        int y2 = centerY + sin(angle + PI/5) * radius * 0.8;
        splashCanvas.drawLine(x1, y1, x2, y2, TFT_BLACK);
    }

    // Draw "Biozen, LLC" text
    splashCanvas.setTextSize(1);
    splashCanvas.setTextDatum(TC_DATUM);
    splashCanvas.drawString("Biozen, LLC", centerX, centerY + radius + 20, &fonts::FreeSansBold12pt7b);

    // Push the canvas to the display using partial update
    _updateImpl(&splashCanvas, 0, 0);
    _lcd->waitDisplay();

    // Delay to show the splash screen
    delay(2000);

    log_i("Splash screen displayed");
}

void StatusView::load() {
    log_i("Loading view");
    _lcd->clear();
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

    // Get the dimensions of the canvas for partial update
    int32_t w = canvas->width();
    int32_t h = canvas->height();

    // Begin transaction for partial update
    _lcd->startWrite();

    // Push the sprite to the display
    canvas->pushSprite(x, y);

    // End transaction
    _lcd->endWrite();

    _lcd->waitDisplay();

    log_d("Canvas update complete with partial update (x=%ld, y=%ld, w=%ld, h=%ld)", x, y, w, h);
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

void StatusView::updateBME688(float temperature, float humidity, float pressure, float gasResistance, float iaq, float co2Equivalent) {
    log_i("Starting BME688 update with values - Temp: %.2f, Humi: %.2f, Press: %.2f, Gas: %.2f, IAQ: %.2f, CO2eq: %.2f",
          temperature, humidity, pressure, gasResistance, iaq, co2Equivalent);

    // Check if canvas objects are properly initialized
    if (!_bme688TempCanvas || !_bme688HumiCanvas || !_bme688PressCanvas || !_bme688GasCanvas || 
        !_bme688IaqCanvas || !_bme688Co2EqCanvas) {
        log_e("BME688 canvas objects not initialized - Temp: %p, Humi: %p, Press: %p, Gas: %p, IAQ: %p, CO2eq: %p",
              _bme688TempCanvas, _bme688HumiCanvas, _bme688PressCanvas, _bme688GasCanvas,
              _bme688IaqCanvas, _bme688Co2EqCanvas);
        return;
    }

    // Check if base cursor position is valid
    if (_bme688BaseCursorX < 0 || _bme688BaseCursorY < 0) {
        log_e("Invalid BME688 base cursor position - X: %ld, Y: %ld", _bme688BaseCursorX, _bme688BaseCursorY);
        return;
    }

    // Bounds checking for temperature - log warning but don't return
    if (temperature < -40.0f || temperature > 85.0f) {
        log_w("Temperature out of bounds: %.2f", temperature);
    }

    // Bounds checking for humidity - log warning but don't return
    if (humidity < 0.0f || humidity > 100.0f) {
        log_w("Humidity out of bounds: %.2f", humidity);
    }

    // Bounds checking for pressure - log warning but don't return
    if (pressure < 300.0f || pressure > 1100.0f) {
        log_w("Pressure out of bounds: %.2f", pressure);
    }

    // Bounds checking for gas resistance - log warning but don't return
    if (gasResistance < 0.0f || gasResistance > 100000.0f) {
        log_w("Gas resistance out of bounds: %.2f", gasResistance);
    }

    // Bounds checking for IAQ - log warning but don't return
    if (iaq < 0.0f || iaq > 500.0f) {
        log_w("IAQ out of bounds: %.2f", iaq);
    }

    // Bounds checking for CO2 equivalent - log warning but don't return
    if (co2Equivalent < 0.0f || co2Equivalent > 10000.0f) {
        log_w("CO2 equivalent out of bounds: %.2f", co2Equivalent);
    }

    char str[16] = { 0 };

    // Update temperature display
    log_i("Updating temperature display");
    _bme688TempCanvas->clear();
    snprintf(str, sizeof(str), "%.1f", temperature);
    _bme688TempCanvas->drawRightString(str, _bme688TempCanvas->width(), 0, _bme688OptionFont);
    log_i("Pushing temperature canvas to display at (%ld, %ld)", _bme688TempCanvasX, _bme688TempCanvasY);
    _updateImpl(_bme688TempCanvas, _bme688TempCanvasX, _bme688TempCanvasY);

    // Update humidity display
    log_i("Updating humidity display");
    _bme688HumiCanvas->clear();
    snprintf(str, sizeof(str), "%.1f", humidity);
    _bme688HumiCanvas->drawRightString(str, _bme688HumiCanvas->width(), 0, _bme688OptionFont);
    log_i("Pushing humidity canvas to display at (%ld, %ld)", _bme688HumiCanvasX, _bme688HumiCanvasY);
    _updateImpl(_bme688HumiCanvas, _bme688HumiCanvasX, _bme688HumiCanvasY);

    // Update pressure display
    log_i("Updating pressure display");
    _bme688PressCanvas->clear();
    snprintf(str, sizeof(str), "%.1f", pressure);
    _bme688PressCanvas->drawRightString(str, _bme688PressCanvas->width(), 0, _bme688OptionFont);
    log_i("Pushing pressure canvas to display at (%ld, %ld)", _bme688PressCanvasX, _bme688PressCanvasY);
    _updateImpl(_bme688PressCanvas, _bme688PressCanvasX, _bme688PressCanvasY);

    // Update gas resistance display
    log_i("Updating gas resistance display");
    _bme688GasCanvas->clear();
    snprintf(str, sizeof(str), "%.1f", gasResistance);
    _bme688GasCanvas->drawRightString(str, _bme688GasCanvas->width(), 0, _bme688OptionFont);
    log_i("Pushing gas resistance canvas to display at (%ld, %ld)", _bme688GasCanvasX, _bme688GasCanvasY);
    _updateImpl(_bme688GasCanvas, _bme688GasCanvasX, _bme688GasCanvasY);

    // Update IAQ display
    log_i("Updating IAQ display");
    _bme688IaqCanvas->clear();
    snprintf(str, sizeof(str), "%.1f", iaq);
    _bme688IaqCanvas->drawRightString(str, _bme688IaqCanvas->width(), 0, _bme688OptionFont);
    log_i("Pushing IAQ canvas to display at (%ld, %ld)", _bme688IaqCanvasX, _bme688IaqCanvasY);
    _updateImpl(_bme688IaqCanvas, _bme688IaqCanvasX, _bme688IaqCanvasY);

    // Update CO2 equivalent display
    log_i("Updating CO2 equivalent display");
    _bme688Co2EqCanvas->clear();
    snprintf(str, sizeof(str), "%.1f", co2Equivalent);
    _bme688Co2EqCanvas->drawRightString(str, _bme688Co2EqCanvas->width(), 0, _bme688OptionFont);
    log_i("Pushing CO2 equivalent canvas to display at (%ld, %ld)", _bme688Co2EqCanvasX, _bme688Co2EqCanvasY);
    _updateImpl(_bme688Co2EqCanvas, _bme688Co2EqCanvasX, _bme688Co2EqCanvasY);

    // Wait for display to complete refresh
    _lcd->waitDisplay();

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
    _co2Canvas->clear();
    _tempCanvas->clear();
    _humiCanvas->clear();

    // Update CO2 with bounds checking
    if (co2 >= 400 && co2 <= 5000) {
        snprintf(str, sizeof(str), "%d", co2);
        log_i("Updating CO2 display: %s", str);
        _co2Canvas->drawRightString(str, _co2Canvas->width(), 0, _scd40OptionFont);
        _updateImpl(_co2Canvas, _co2CanvasX, _co2CanvasY);
    } else {
        log_w("CO2 value out of bounds: %d", co2);
    }

    // Update temperature with bounds checking
    if (!isnan(temperature) && temperature >= -10.0f && temperature <= 60.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", temperature);
        log_i("Updating temperature display: %s", str);
        _tempCanvas->drawRightString(str, _tempCanvas->width(), 0, _scd40OptionFont);
        _updateImpl(_tempCanvas, _tempCanvasX, _tempCanvasY);
    } else {
        log_w("Temperature value out of bounds: %.2f", temperature);
    }

    // Update humidity with bounds checking
    if (!isnan(humidity) && humidity >= 0.0f && humidity <= 100.0f) {
        memset(str, 0, sizeof(str));
        snprintf(str, sizeof(str), "%.2f", humidity);
        log_i("Updating humidity display: %s", str);
        _humiCanvas->drawRightString(str, _humiCanvas->width(), 0, _scd40OptionFont);
        _updateImpl(_humiCanvas, _humiCanvasX, _humiCanvasY);
    } else {
        log_w("Humidity value out of bounds: %.2f", humidity);
    }

    // Wait for display to complete refresh
    _lcd->waitDisplay();

    log_i("SCD40 update completed");
}
