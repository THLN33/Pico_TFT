


#include "Graphics.h"

#include "Pinout.h"
#include "Chipset.h"
#include "Interface.h"

/**
 * inspired by TFT_eSPI code
 */



uint16_t _width = 0;
uint16_t _height = 0;
uint8_t _rotation = 0;



static uint16_t addr_col, addr_row;


bool _vpDatum, _vpOoB;
uint16_t _xDatum = 0, _yDatum = 0;
uint16_t _vpX = 0, _vpY = 0;
uint16_t _xWidth, _yHeight, _vpW, _vpH, _width, _height;
uint8_t rotation = 0;



/***************************************************************************************
** Function name:           width
** Description:             Return the pixel width of display (per current rotation)
***************************************************************************************/
// Return the size of the display (per current rotation)
int16_t width(void) {
	if (_vpDatum)
		return _xWidth;
	return _width;
}


/***************************************************************************************
** Function name:           height
** Description:             Return the pixel height of display (per current rotation)
***************************************************************************************/
int16_t height(void) {
	if (_vpDatum)
		return _yHeight;
	return _height;
}



/***************************************************************************************
** Function name:           resetViewport
** Description:             Reset viewport to whole TFT screen, datum at 0,0
***************************************************************************************/
void resetViewport(void) {
  // Reset viewport to the whole screen (or sprite) area
  _vpDatum = false;
  _vpOoB   = false;
  _xDatum = 0;
  _yDatum = 0;
  _vpX = 0;
  _vpY = 0;
  _vpW = width();
  _vpH = height();
  _xWidth  = width();
  _yHeight = height();
}



/***************************************************************************************
** Function name:           setRotation
** Description:             rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
***************************************************************************************/
void setRotation(uint8_t m) {

    begin_tft_write();
    _rotation = m % 4;
    TFT_Rotation(m, &_width, &_height);

    sleep_us(10);
	end_tft_write();

	// Reset the viewport to the whole screen
    resetViewport();
}

void test_init(void);

uint8_t Graphics_Init(void) {
    
    uint8_t err = TFT_Hardware_Init();
    if (err!=NO_ERR)
        return err;

    sleep_ms(150); // Wait for reset to complete

    TFT_Init_Registers();

    // Clear screen (black!)
    begin_tft_write();
    setWindow(0, 0, TFT_WIDTH-1, TFT_HEIGHT-1);    
    pushBlock(0x00000, TFT_WIDTH * TFT_HEIGHT);
    end_tft_write(); 

    setRotation(3);
    
    TFT_Blacklight(true);

    return err;
}


void DrawRectFilled(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    begin_tft_write();

    setWindow(x, y, x+width-1, y+height-1);
    uint len = width * height;

    pushBlock(color, len);

    end_tft_write();    
}


void DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const void *image) {
    begin_tft_write();

    setWindow(x, y, x+width-1, y+height-1);
    uint len = width * height;

    pushImage(image, len);

    end_tft_write();    
}


/***************************************************************************************
** Function name:           DrawPixel
** Description:             push a single pixel at an arbitrary position
***************************************************************************************/
void DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
    if (_vpOoB) return;

    x+= _xDatum;
    y+= _yDatum;

    // Range checking
    if ((x < _vpX) || (y < _vpY) ||(x >= _vpW) || (y >= _vpH)) return;

    begin_tft_write();

    setPixel(x, y, color);

    end_tft_write();
}


/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
***************************************************************************************/
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint32_t color) {
    if (_vpOoB) return;

    x+= _xDatum;
    y+= _yDatum;

    // Clipping
    if ((y < _vpY) || (x >= _vpW) || (y >= _vpH)) return;

    if (x < _vpX) { w += x - _vpX; x = _vpX; }

    if ((x + w) > _vpW) w = _vpW - x;

    if (w < 1) return;

    begin_tft_write();

    setWindow(x, y, x + w - 1, y);

    pushBlock(color, w);

    end_tft_write();
}

/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
***************************************************************************************/
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint32_t color) {
    if (_vpOoB) return;

    x+= _xDatum;
    y+= _yDatum;

    // Clipping
    if ((x < _vpX) || (x >= _vpW) || (y >= _vpH)) return;

    if (y < _vpY) { h += y - _vpY; y = _vpY; }

    if ((y + h) > _vpH) h = _vpH - y;

    if (h < 1) return;

    begin_tft_write();

    setWindow(x, y, x, y + h - 1);

    pushBlock(color, h);

    end_tft_write();
}

/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle outline
***************************************************************************************/
// @see : https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_cercle_d%27Andres
void DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint32_t color) {

    if ( r <= 0 ) return;

    int32_t x = 0;
    int32_t y = r;
    int32_t d = r - 1;

    while (y>=x) {
        DrawPixel(x0 + x, y0 + y, color);
        DrawPixel(x0 + y, y0 + x, color);

        DrawPixel(x0 - x, y0 + y, color);
        DrawPixel(x0 - y, y0 + x, color);

        DrawPixel(x0 + x, y0 - y, color);
        DrawPixel(x0 + y, y0 - x, color);

        DrawPixel(x0 - x, y0 - y, color);
        DrawPixel(x0 - y, y0 - x, color);

        if (d>=2*x) {
            d -= (2*x)-1;
		    x ++;
        } else if (d < 2*(r-y)) {
            d += (2*y)-1;
		    y --;	
        } else {
            d += 2*(y-x-1);
		    y --;
		    x ++;
        }
    }
}




void DrawCircle2(uint16_t x0, uint16_t y0, uint16_t outer, uint16_t inner, uint32_t color) {

    int16_t xo = outer;
    int16_t xi = inner;
    int16_t y = 0;
    int16_t erro = 1 - xo;
    int16_t erri = 1 - xi;

    while(xo >= y) {
        drawFastHLine(x0 + xi, x0 + xo, y0 + y,  color);
        drawFastVLine(x0 + y,  y0 + xi, y0 + xo, color);
        drawFastHLine(x0 - xo, x0 - xi, y0 + y,  color);
        drawFastVLine(x0 - y,  y0 + xi, y0 + xo, color);
        drawFastHLine(x0 - xo, x0 - xi, y0 - y,  color);
        drawFastVLine(x0 - y,  y0 - xo, y0 - xi, color);
        drawFastHLine(x0 + xi, x0 + xo, y0 - y,  color);
        drawFastVLine(x0 + y,  y0 - xo, y0 - xi, color);

        y++;

        if (erro < 0) {
            erro += 2 * y + 1;
        } else {
            xo--;
            erro += 2 * (y - xo + 1);
        }

        if (y > inner) {
            xi = y;
        } else {
            if (erri < 0) {
                erri += 2 * y + 1;
            } else {
                xi--;
                erri += 2 * (y - xi + 1);
            }
        }
    }
}


void xLine(int x1, int x2, int y, int colour) {
    drawFastHLine(x1, y, x2-x1, colour);
}

void yLine(int x, int y1, int y2, int colour) {
    drawFastVLine(x, y1, y2-y1, colour);
}


void circle2(int xc, int yc, int inner, int outer, int colour) {
    int xo = outer;
    int xi = inner;
    int y = 0;
    int erro = 1 - xo;
    int erri = 1 - xi;

    while(xo >= y) {
        xLine(xc + xi, xc + xo, yc + y,  colour);
        yLine(xc + y,  yc + xi, yc + xo, colour);
        xLine(xc - xo, xc - xi, yc + y,  colour);
        yLine(xc - y,  yc + xi, yc + xo, colour);
        xLine(xc - xo, xc - xi, yc - y,  colour);
        yLine(xc - y,  yc - xo, yc - xi, colour);
        xLine(xc + xi, xc + xo, yc - y,  colour);
        yLine(xc + y,  yc - xo, yc - xi, colour);

        y++;

        if (erro < 0) {
            erro += 2 * y + 1;
        } else {
            xo--;
            erro += 2 * (y - xo + 1);
        }

        if (y > inner) {
            xi = y;
        } else {
            if (erri < 0) {
                erri += 2 * y + 1;
            } else {
                xi--;
                erri += 2 * (y - xi + 1);
            }
        }
    }
}