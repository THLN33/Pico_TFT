//#include "portable.h"
#include "def.h"

#if (INTERFACE_LCD == INTERFACE_LCD_SPI)

#include "hardware/dma.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"

#include "Pinout.h"
#include "Chipset.h"
#include "Interface.h"
#include "Chipsets/Generics_Regs.h"



#if (TFT_COLOR == TFT_COLOR_16BITS)   
//#define DMA_LCD true
#endif

#define CS_L        gpio_put(TFT_CS, LOW)
#define CS_H        gpio_put(TFT_CS, HIGH)

#define DC_C        gpio_put(TFT_DC, LOW)
#define DC_D        gpio_put(TFT_DC, HIGH)

#define TFT_SPI_PORT    spi1
#define TFT_SPI_MODE    0
#define PIN_SCLK        TFT_SCLK
#define PIN_MOSI        TFT_MOSI

#define SPI_BUSY_CHECK          while(spi_is_busy(TFT_SPI_PORT))

#define SPI_WRITABLE_CHECK      while (!spi_is_writable(TFT_SPI_PORT))

#define SPI_WRITTE_BYTE         spi_get_hw(TFT_SPI_PORT)->dr        

void begin_tft_write(void) {
    //#warning '->'
    //spi_init(TFT_SPI_PORT, SPI_FREQUENCY>>1);    
    //sleep_us(10);   
    CS_L;
}

void end_tft_write(void) {
    CS_H;
    //spi_deinit(TFT_SPI_PORT);    
}


inline void tft_Write_8(uint8_t data) {
    spi_write_blocking(TFT_SPI_PORT, &data, 1);
}

inline void tft_Write_16(uint16_t data) {
    //spi_write16_blocking(TFT_SPI_PORT, &data, 2);
    uint8_t h = data>>8;
    uint8_t l = data;
    spi_write_blocking(TFT_SPI_PORT, &h, 1);
    spi_write_blocking(TFT_SPI_PORT, &l, 1);
}

#define tft_Write_32C(C,D) \
    tft_Write_16((uint16_t) (C)); \
    tft_Write_16((uint16_t) (D))



void writecommand(uint8_t c) {
    begin_tft_write();
    DC_C;
    tft_Write_8(c);
    DC_D;
    end_tft_write();
}


void writedata(uint8_t d) {
    begin_tft_write();
    DC_D;        // Play safe, but should already be in data mode
    tft_Write_8(d);
    //CS_L;        // Allow more hold time for low VDI rail
    end_tft_write();
}

/*
                            TFT_COLOR==TFT_COLOR_16BITS         TFT_COLOR==TFT_COLOR_18BITS

    _MODE_Index_8bits               _index_to_16                         _index_to_24 

    _MODE_RGB332_8bits              _8_to_16                             _8_to_24

    _MODE_RGB565_16bits             _direct_16                           _16_to_24

    _MODE_RGB666_18bits             not supported                       direct_24      ( not used )   

    _MODE_RGB666_24bits             not supported                       direct_24      ( not used )   
*/

#if (TFT_COLOR == TFT_COLOR_16BITS) 

#if (MODE_RGB == _MODE_Index_8bits)
    #define _index_to_16         1
#elif (MODE_RGB == _MODE_RGB332_8bits)
    #define _8_to_16            2
#elif (MODE_RGB == _MODE_RGB565_16bits)
    #define _direct_16           3
#else
#error mode not supported !
#endif  /* MODE_RGB */

#elif (TFT_COLOR == TFT_COLOR_18BITS) || (TFT_COLOR == TFT_COLOR_24BITS) 

#if (MODE_RGB == _MODE_Index_8bits)
    #define _index_to_24         4
#elif (MODE_RGB == _MODE_RGB332_8bits)
    #define _8_to_24            5
#elif (MODE_RGB == _MODE_RGB565_16bits)
    #define _16_to_24           6
#elif (MODE_RGB == _MODE_RGB666_18bits) || (MODE_RGB == _MODE_RGB666_24bits)
    #define _direct_24          7
    #warning Not recommanded : This mode use lot of memory usage!    
//#error mode not used, DMA support only 8 or 16 bits transfert !
#else
#error mode not supported !
#endif  /* MODE_RGB */

#endif  /* TFT_COLOR */




#if defined(_8_to_16) || defined(_index_to_16)
uint16_t color_8_to_16(uint32_t color8) {
    register uint16_t color16;
    
    color16 = (color8 & 0x03)<<3;
    if (color8 & 0x01) color16 |= 0x0007;
    color16 |= (color8 & 0x1C)<<6;
    if (color8 & 0x04) color16 |= 0x00E0;
    color16 |= (color8 & 0xE0)<<8;     
    if (color8 & 0x20) color16 |= 0x1800;
    return color16;
}
#endif

#ifdef _8_to_24
uint32_t color_8_to_24(uint32_t color8) {
    register uint32_t color24;
    
    color24 = (color8 & 0x03)<<6;
    if (color8 & 0x01) color24 |= 0x00003F;
    color24 |= (color8 & 0x1C)<<11;
    if (color8 & 0x04) color24 |= 0x001F00;
    color24 |= (color8 & 0xE0)<<16;     
    if (color8 & 0x20) color24 |= 0x1F0000;
    return color24;
}
#endif

#if defined(_16_to_24) || defined(_index_to_24)
uint32_t color_16_to_24(uint16_t color16) {
    register uint32_t color24;

    color24 = (color16 & 0x001F)<<3;
    if (color16 & 0x0001) color24 |= 0x000007;
    color24 |= (color16 & 0x07E0)<<5;
    if (color16 & 0x0020) color24 |= 0x000300;    
    color24 |= (color16 & 0xF800)<<8;
    if (color16 & 0x0800) color24 |= 0x030000;    
    return color24;
}
#endif

#ifdef _index_to_16
static uint16_t Palette[256];	

uint16_t index_to_16(uint8_t index) {
    register uint16_t color16 = Palette[index];
    return color16;
}
#endif

#ifdef _index_to_24
static uint16_t Palette[256];	

uint32_t index_to_24(uint8_t index) {
    register uint32_t color24 = color_16_to_24(Palette[index]);
    return color24;
}
#endif


static inline uint32_t cnvt_color(uint32_t color) {
   
#ifdef _index_to_16
        color = index_to_16(color);
#elif _8_to_16
        color = color_8_to_16(color);
#elif _direct_16

#elif _index_to_24
        color = index_to_24(color);  
#elif _8_to_24
        color = color_8_to_24(color);
#elif _16_to_24
        color = color_16_to_24(color);     
#elif _direct_24

#else
#error !!!
#endif
    return color;
}  


static inline void send_pixel(uint32_t color) {

#ifdef _index_to_16
        uint8_t c[2];   
        c[0] = color>>8;
        c[1] = color;        
        spi_write_blocking(TFT_SPI_PORT, c, 2);
#elif _8_to_16
        uint8_t c[2];   
        c[0] = color>>8;
        c[1] = color;        
        spi_write_blocking(TFT_SPI_PORT, c, 2);
#elif _direct_16
        uint8_t c[2];   
        c[0] = color>>8;
        c[1] = color;        
        spi_write_blocking(TFT_SPI_PORT, c, 2);
#elif _index_to_24        
        uint8_t c[3];
        c[0] = color>>16;
        c[1] = color>>8;
        c[2] = color;  
        spi_write_blocking(TFT_SPI_PORT, c, 3);    
#elif _8_to_24
        uint8_t c[3];
        c[0] = color>>16;
        c[1] = color>>8;
        c[2] = color;  
        spi_write_blocking(TFT_SPI_PORT, c, 3);    
#elif _16_to_24
        uint8_t c[3];
        c[0] = color>>16;
        c[1] = color>>8;
        c[2] = color;  
        spi_write_blocking(TFT_SPI_PORT, c, 3);      
#elif _direct_24
        uint8_t c[3];
        c[0] = color>>16;
        c[1] = color>>8;
        c[2] = color;  
        spi_write_blocking(TFT_SPI_PORT, c, 3);    
#else
#error !!!
#endif
}  


#if (MODE_RGB == _MODE_Index_8bits)  
int8_t Load_Palette(const uint8_t *_palette, uint32_t len) {
    uint16_t i=0;
    uint16_t color;
    if (len>768)
        return -1;
    while (len) {
#ifdef BNW        
        uint r = (*_palette++)>>3;
        uint g = (*_palette++)>>2;
        uint b = (*_palette++)>>3; 

        uint16_t y = (r + g + b) >> 2;
        //(r*0.299 + g*0.587 + b*0.114);
        color =  (y&0x1F)<<11 | (2*y&0x3F)<<5 | (y&0x1F);  
#else
        // 888 to 565
        uint r = (*_palette++)>>3;
        uint g = (*_palette++)>>2;
        uint b = (*_palette++)>>3;         
        color =  (r&0x1F)<<11 | (g&0x3F)<<5 | (b&0x1F); 
#endif        
        Palette[i++] = color;
        len-=3;
    }
    return 0;
}
#endif



#ifdef DMA_LCD

int32_t             dma_tx_channel;
dma_channel_config  dma_tx_config;
static bool         DMA_Enabled = false;
static bool         _dma_completed = false;

void dma_handler(void) {
    dma_hw->ints0 = 1u << dma_tx_channel;
    // reconfigure GPIO for PIO0
    _dma_completed = true;

    // flush SPI Buffer
    SPI_BUSY_CHECK;

    gpio_put(TFT_CS, 1);
}

bool init_DMA(void) {

    if (DMA_Enabled) 
        return false;

    dma_tx_channel = dma_claim_unused_channel(false);

    if (dma_tx_channel < 0) 
        return false;

    dma_tx_config = dma_channel_get_default_config(dma_tx_channel);

    channel_config_set_transfer_data_size(&dma_tx_config, DMA_SIZE_16);

    channel_config_set_dreq(&dma_tx_config, spi_get_index(TFT_SPI_PORT) ? DREQ_SPI1_TX : DREQ_SPI0_TX);

    // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq0_enabled(dma_tx_channel, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);

    irq_set_enabled(DMA_IRQ_0, true);

    DMA_Enabled = true;
    return true;
}

void dmaWait(void) {
    while (dma_channel_is_busy(dma_tx_channel));
}
#endif  /* DMA_LCD */

static uint8_t TFT_Interface_Init(void) {

    gpio_set_function(PIN_SCLK, GPIO_FUNC_SPI);
#ifdef PIN_MISO    
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
#endif
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    //gpio_set_function(TFT_CS, GPIO_FUNC_SIO);

    //spi_init(TFT_SPI_PORT, SPI_FREQUENCY);  
    spi_set_format(TFT_SPI_PORT, 8, (spi_cpol_t)(TFT_SPI_MODE >> 1), (spi_cpha_t)(TFT_SPI_MODE & 0x1), SPI_MSB_FIRST);
    //#warning '->'
    spi_init(TFT_SPI_PORT, SPI_FREQUENCY>>1); 

#ifdef DMA_LCD
    init_DMA();
#endif  /* DMA_LCD */


#if (MODE_RGB == _MODE_Index_8bits)  
    uint r, g, b;
    for (uint i=0;i<256;i++) {
        // gray levels
        r = (i * 32)/256;
        g = (i * 64)/256;
        b = (i * 32)/256;

        Palette[i] = r<<11 | g<<5 | b;
    }
#endif

    return NO_ERR;
}

void TFT_HardReset(void) {

    writecommand(0x00);
    gpio_put(TFT_RST, HIGH);
    sleep_ms(5);
    gpio_put(TFT_RST, LOW);
    sleep_ms(20);
    gpio_put(TFT_RST, HIGH);
}


uint8_t TFT_Hardware_Init(void) {
    gpio_init(TFT_CS);
    gpio_set_dir(TFT_CS, GPIO_OUT);
    gpio_put(TFT_CS, HIGH);

    gpio_init(TFT_DC);
    gpio_set_dir(TFT_DC, GPIO_OUT);
    gpio_put(TFT_DC, HIGH);

    gpio_init(TFT_RST);
    gpio_set_dir(TFT_RST, GPIO_OUT);
    gpio_put(TFT_RST, HIGH);

    gpio_init(TFT_LED);    
    gpio_set_dir(TFT_LED, GPIO_OUT);    
    gpio_put(TFT_LED, LOW);

    TFT_Interface_Init();

    TFT_HardReset();

    return NO_ERR;
}

void TFT_Blacklight(bool enable) {
    gpio_put(TFT_LED, (enable==true?HIGH:LOW));
}

void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    SPI_BUSY_CHECK;
    DC_C; 
    tft_Write_8(TFT_CASET);
    DC_D;
    tft_Write_32C(x0, x1);

    DC_C; 
    tft_Write_8(TFT_PASET);
    DC_D; 
    tft_Write_32C(y0, y1);

    DC_C; 
    tft_Write_8(TFT_RAMWR);
    DC_D;
}


void setPixel(uint16_t x, uint16_t y, uint32_t color) {
    setWindow(x, y, x, y);

    send_pixel(cnvt_color(color));
}


#ifdef DMA_LCD
void pushBlockDMA(uint32_t _color, uint32_t _len) {
    static uint16_t color;

    if ((_len == 0) || (!DMA_Enabled)) 
        return;

    dmaWait();
    color = _color;
    _dma_completed = false;

    channel_config_set_read_increment(&dma_tx_config, false);
    dma_channel_configure(dma_tx_channel, &dma_tx_config, &spi_get_hw(TFT_SPI_PORT)->dr, &color, _len, true);
}
#endif

void pushBlock(uint32_t color, uint32_t len) {

    color = cnvt_color(color);

#ifdef DMA_LCD
    if (len>16) {
        pushBlockDMA(color, len);
        return;
    }
#endif    
    while (len--) {
        send_pixel(color);
    }
}



#ifdef DMA_LCD
void pushImageDMA(const void *_image, uint32_t _len) {

    if ((_len == 0) || (!DMA_Enabled)) 
        return;

    dmaWait();

    _dma_completed = false;
   
    channel_config_set_read_increment(&dma_tx_config, true);    
    dma_channel_configure(dma_tx_channel, &dma_tx_config, &spi_get_hw(TFT_SPI_PORT)->dr, _image, _len, true);
}
#endif


void pushImage(const void *buffer, uint32_t len) {

#if (MODE_RGB == _MODE_RGB332_8bits) || defined(_index_to_16) || defined(_index_to_24) 
    const uint8_t *image = buffer;
#else
    const uint16_t *image = buffer;
#endif

#ifdef DMA_LCD
    if (len>16) {
        pushImageDMA(image, len);
        return;
    }
#endif
    while (len--) {
        uint32_t color = *image++;

        send_pixel(cnvt_color(color));
    }
}


/******/
#ifdef _ZZ

#if (TFT_COLOR == TFT_COLOR_16BITS)   
    color = b<<3;
    color += g<<8;
    color += r<<13;    
    if (b&1) color |= 0x0007;
    if (g&1) color |= 0x00E0;
    if (r&1) color |= 0x1800;
    uint8_t c[] = {color>>8, color};
    while (len--) {
        //tft_Write_16(0xFFFF);
        spi_write_blocking(TFT_SPI_PORT, c, 2);
    }
#elif (TFT_COLOR == TFT_COLOR_18BITS)
    color = b<<6;
    if (b&1) color |= 0x00003F;
    color += g<<13;
    if (g&1) color |= 0x001F00;
    color += r<<21;    
    if (r&1) color |= 0x1F0000;  

    uint8_t c[3];
    c[0] = color>>16;
    c[1] = color>>8;
    c[2] = color;
    while (len--) {
        /*
        for (uint i=0;i<3;i++) {
            SPI_BUSY_CHECK;
            //SPI_WRITABLE_CHECK;
            SPI_WRITTE_BYTE = c[i];
        }     
        */    
        spi_write_blocking(TFT_SPI_PORT, c, 3);
    } 
#elif (TFT_COLOR == TFT_COLOR_24BITS)

#endif

#endif

#ifdef _ZZ
    uint8_t r, g, b;

    while (len--) {
        uint32_t color = *image++;

#if (MODE_RGB == _MODE_RGB332_8bits)  
        b = color & 0x03;
        g = (color>>2) & 0x07;
        r = (color>>5) & 0x07;

#elif (MODE_RGB == _MODE_RGB565_16bits)
/*
        b = color & 0x001F;

        if (b&1) {
            b<<=3;
            b |= 0x0007;            
        } else {
            b<<=3;
        }

        g = (color>>5) & 0x003F;

        if (g&1) {
            g<<=2;
            g |= 0x0003;            
        } else {
            g<<=2;

        }

        r = (color>>11) & 0x001F;

        if (r&1) {
            r<<=3;
            r |= 0x0007;            
        } else {
            r<<=3;
        }    
*/
#else
#error mode not supported!
#endif


#if (TFT_COLOR == TFT_COLOR_16BITS)   
        color = b<<3;
        color += g<<8;
        color += r<<13;   


        uint8_t c[2];   
        c[0] = color>>8;
        c[1] = color;        
        spi_write_blocking(TFT_SPI_PORT, c, 2);
#elif (TFT_COLOR == TFT_COLOR_18BITS)

#if (MODE_RGB == _MODE_RGB332_8bits)  
        color = b<<6;
        color += g<<13;
        color += r<<21; 

        uint8_t c[3];
        c[0] = color>>16;
        c[1] = color>>8;
        c[2] = color;   
#elif (MODE_RGB == _MODE_RGB565_16bits)
/*
        color = b<<3;
        color += g<<10;
        color += r<<19;     
*/        
#else
#error mode not supported!
#endif
        uint8_t c[3];
        /*        
        c[0] = r;
        c[1] = g;
        c[2] = b;
        */
        color = color_16_to_24(color);
        c[0] = color>>16;
        c[1] = color>>8;
        c[2] = color;  
        spi_write_blocking(TFT_SPI_PORT, c, 3);    
#elif (TFT_COLOR == TFT_COLOR_24BITS)
#error mode not supported!
#endif
    }

#endif



void test_spi_init(void) {

    //begin_tft_write();
    //TFT_Init_Registers();
    //end_tft_write();

/*
    writecommand(0x00);
    writedata(0x01);
    writecommand(0xFF);    
    writedata(0x02);    
    writedata(0x03);
*/
}


void test_spi_rgb(void) {

    begin_tft_write();


    uint16_t x = 80;
    uint16_t w = 80;    
    uint16_t y = 80;
    uint16_t h = 80;

    setWindow(x, y, x+w-1, y+h-1);
    uint len = w * h;



#if (TFT_COLOR == TFT_COLOR_16BITS)   
    uint8_t c[] = {0xFF, 0xFF};
    while (len--) {
        //tft_Write_16(0xFFFF);
        spi_write_blocking(TFT_SPI_PORT, c, 2);
    }
#elif (TFT_COLOR == TFT_COLOR_18BITS)  
    uint8_t c[] = {0x0F, 0x0F, 0x0F};
    while (len--) {
        spi_write_blocking(TFT_SPI_PORT, c, 3);
    } 
#elif (TFT_COLOR == TFT_COLOR_24BITS)
    uint8_t c[] = {0xFF, 0xFF, 0xFF};
    while (len--) {
        spi_write_blocking(TFT_SPI_PORT, c, 3);
    }  
#endif

    end_tft_write();
}

#endif /* INTERFACE_LCD == INTERFACE_LCD_SPI */
