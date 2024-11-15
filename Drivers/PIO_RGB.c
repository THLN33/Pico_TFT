

#include "def.h"


#if (INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO) || (INTERFACE_LCD == INTERFACE_LCD_PARALLEL_PIO)

#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "Pinout.h"
#include "Chipset.h"
#include "Interface.h"
#include "Chipsets/Generics_Regs.h"


//#include "build/PIO_RGB.pio.h"
#include "PIO_RGB.pio.h"

#if (INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO) 
/*
                            TFT_COLOR==TFT_COLOR_16BITS         TFT_COLOR==TFT_COLOR_18BITS

    _MODE_Index_8bits               direct_565                          custom_565_666 

    _MODE_RGB332_8bits              custom_332_565                      custom_332_666

    _MODE_RGB565_16bits             direct_565                          custom_565_666

    _MODE_RGB666_18bits             not supported                       direct_666      ( not used )   

    _MODE_RGB666_24bits             not supported                       direct_888      ( not used )   
*/

#if (TFT_COLOR == TFT_COLOR_16BITS) 

#if (MODE_RGB == _MODE_Index_8bits)
    #define direct_565          2
#elif (MODE_RGB == _MODE_RGB332_8bits)
    #define custom_332_565      1
#elif (MODE_RGB == _MODE_RGB565_16bits)
    #define direct_565          2
#else
#error mode not supported !
#endif  /* MODE_RGB */

#elif (TFT_COLOR == TFT_COLOR_18BITS) || (TFT_COLOR == TFT_COLOR_24BITS) 

#if (MODE_RGB == _MODE_Index_8bits)
    #define custom_565_666      4
#elif (MODE_RGB == _MODE_RGB332_8bits)
    #define custom_332_666      3
#elif (MODE_RGB == _MODE_RGB565_16bits)
    #define custom_565_666      4
#elif (MODE_RGB == _MODE_RGB666_18bits) || (MODE_RGB == _MODE_RGB666_24bits)
    #define direct_666          5
    #warning Not recommanded : This mode use lot of memory usage!    
//#error mode not used, DMA support only 8 or 16 bits transfert !
#else
#error mode not supported !
#endif  /* MODE_RGB */
#endif  /* TFT_COLOR */
#endif  /* INTERFACE_LCD_SERIAL_PIO */


#if (INTERFACE_LCD == INTERFACE_LCD_PARALLEL_PIO)

/*
                            TFT_COLOR==TFT_COLOR_16BITS         TFT_COLOR==TFT_COLOR_18BITS

    _MODE_Index_8bits               _index_to_16                         _index_to_24 

    _MODE_RGB332_8bits              _index_to_16(1)                       not supported

    _MODE_RGB565_16bits             _direct_16                          not supported

    _MODE_RGB666_18bits             not supported                       direct_24      ( not used )   

    _MODE_RGB666_24bits             not supported                       direct_24      ( not used )   
*/

#if (TFT_COLOR == TFT_COLOR_16BITS) 

#if (MODE_RGB == _MODE_Index_8bits)
    #define _index_to_16         1
#elif (MODE_RGB == _MODE_RGB332_8bits)
    #define _index_to_16_         2
#elif (MODE_RGB == _MODE_RGB565_16bits)
    #define _direct_16          3
#else
#error mode not supported !
#endif  /* MODE_RGB */

#elif (TFT_COLOR == TFT_COLOR_18BITS) || (TFT_COLOR == TFT_COLOR_24BITS) 

#if (MODE_RGB == _MODE_Index_8bits)
    #define _index_to_24        4
#elif (MODE_RGB == _MODE_RGB332_8bits)
#error mode not supported !
#elif (MODE_RGB == _MODE_RGB565_16bits)
#error mode not supported !
#elif (MODE_RGB == _MODE_RGB666_18bits) || (MODE_RGB == _MODE_RGB666_24bits)
    #define direct_24          5
    #warning Not recommanded : This mode use lot of memory usage!    
//#error mode not used, DMA support only 8 or 16 bits transfert !
#else
#error mode not supported !
#endif  /* MODE_RGB */
#endif  /* TFT_COLOR */ 
#endif  /* INTERFACE_LCD_PARALLEL_PIO */


#if (TFT_COLOR==TFT_COLOR_24BITS) && (MODE_RGB==_MODE_RGB666_18bits)
//#define DMA_LCD false
#else
#define DMA_LCD true
#endif


#define CS_L        gpio_put(TFT_CS, LOW)
#define CS_H        gpio_put(TFT_CS, HIGH)


PIO pio_lcd;
int8_t pio_lcd_sm;
uint32_t pio_lcd_pull_stall_mask;

#define PIO_LCD_NB      pio_lcd 
#define PIO_LCD_SM      pio_lcd_sm 
#define PIO_LCD_PSP     pio_lcd_pull_stall_mask

// Wait for the PIO to stall (SM pull request finds no data in TX FIFO)
// This is used to detect when the SM is idle and hence ready for a jump instruction
#define WAIT_FOR_LCD_STALL  PIO_LCD_NB->fdebug = PIO_LCD_PSP; while (!(PIO_LCD_NB->fdebug & PIO_LCD_PSP))

// Wait until at least "S" locations free
#define WAIT_FOR_LCD_FIFO_FREE(S) while (((PIO_LCD_NB->flevel >> (PIO_LCD_SM * 8)) & 0x000F) > (8-S)){}

// The write register of the TX FIFO.
#define TX_LCD_FIFO  PIO_LCD_NB->txf[PIO_LCD_SM]

uint32_t pio_instr_start_Data;
uint32_t pio_instr_start_Command;
uint32_t pio_instr_start_32;
uint32_t pio_instr_start_RGB;

//uint32_t pio_instr_set_dc;
//uint32_t pio_instr_clr_dc;

#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)

PIO pio_indexer;
int8_t pio_indexer_sm;
uint32_t pio_indexer_pull_stall_mask;

#define PIO_IXR_NB              pio_indexer 
#define PIO_IXR_SM              pio_indexer_sm 
#define PIO_IXR_PULL_TEST       pio_indexer_pull_stall_mask

// The write register of the TX FIFO.
#define TX_IXR_FIFO  PIO_IXR_NB->txf[PIO_IXR_SM]

// The read register of the RX FIFO.
#define RX_IXR_FIFO  PIO_IXR_NB->rxf[PIO_IXR_SM]

#define WAIT_FOR_IXR_STALL  PIO_IXR_NB->fdebug = PIO_IXR_PULL_TEST; while (!(PIO_IXR_NB->fdebug & PIO_IXR_PULL_TEST))

static uint16_t __attribute__((aligned(512))) Palette[256];		//MUST be 512 bit aligned
#endif

void begin_tft_write(void) {
    CS_L;
}

void end_tft_write(void) {
    // wait TXFIFO is empty
    WAIT_FOR_LCD_STALL;
    //CS_H;
}

inline void tft_Write_8(uint8_t data) {
    WAIT_FOR_LCD_STALL;    
    TX_LCD_FIFO = data;
}


void writecommand(uint8_t c) {    
       
    PIO_LCD_NB->sm[PIO_LCD_SM].instr = pio_instr_start_Command;
    begin_tft_write();   
    tft_Write_8(c);
    end_tft_write();
}


void writedata(uint8_t d) {
  
    PIO_LCD_NB->sm[PIO_LCD_SM].instr = pio_instr_start_Data;
    begin_tft_write();   
    tft_Write_8(d);
    end_tft_write();
}




#pragma GCC push_options
#pragma GCC optimize ("O0")


// Code will try both pio's to find a free SM
static int8_t pio_check_available_pio(PIO *_pio, const pio_program_t *program) {
    int8_t pio_nb = 0;
     *_pio = pio0;
    // Find enough free space on one of the PIO's
    if (!pio_can_add_program(pio0, program)) {
        *_pio = pio1;
        pio_nb = 1;
        if (!pio_can_add_program(pio1, program)) {
            // Serial.println("No room for PIO program!");
            return -1;
        }
    }
    return pio_nb;
}

#if (INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO)

typedef struct {
    uint8_t         RED_cycles;
    uint8_t         RED_padding_cycles;
    uint8_t         GREEN_cycles;
    uint8_t         GREEN_padding_cycles;
    uint8_t         BLUE_cycles;
    uint8_t         BLUE_padding_cycles;                
} PIO_COLOR_Mode_def;

 
static const PIO_COLOR_Mode_def PIO_COLOR_Mode_RGB332_to_RGB565 = {
    3-1-1, 2-1-1,
    3-1, 3-1-1,
    2-1, 3-1 
};

static const PIO_COLOR_Mode_def PIO_COLOR_Mode_RGB332_to_RGB666 = {
    3-1-1, 5-1-1,
    3-1, 5-1-1,
    2-1, 6-1 
};

static const PIO_COLOR_Mode_def PIO_COLOR_Mode_RGB565_to_RGB666 = {
    5-1-1, 3-1-1,
    6-1, 2-1-1,
    5-1, 3-1
};


static uint8_t pio_update_instr_custom_RGB_mode(PIO pio, uint8_t _program_offset, const PIO_COLOR_Mode_def *pPIO_COLOR_Mode) {
    pio_set_instr_RED_cycles(pio, _program_offset, pPIO_COLOR_Mode->RED_cycles, pPIO_COLOR_Mode->RED_padding_cycles);
    pio_set_instr_GREEN_cycles(pio, _program_offset, pPIO_COLOR_Mode->GREEN_cycles, pPIO_COLOR_Mode->GREEN_padding_cycles);
    pio_set_instr_BLUE_cycles(pio, _program_offset, pPIO_COLOR_Mode->BLUE_cycles, pPIO_COLOR_Mode->BLUE_padding_cycles);    

    // Create the assembler instruction for the jump to byte send routine
    pio_instr_start_Data = pio_encode_jmp(_program_offset + pio_send_serial_custom_RGB_offset_start_Data);    
    pio_instr_start_Command = pio_encode_jmp(_program_offset + pio_send_serial_custom_RGB_offset_start_Command);
    pio_instr_start_32 = pio_encode_jmp(_program_offset + pio_send_serial_custom_RGB_offset_start_32);    
    pio_instr_start_RGB = pio_encode_jmp(_program_offset + pio_send_serial_custom_RGB_offset_start_RGB);

    return _program_offset + pio_send_serial_custom_RGB_wrap_target;  
}


static uint8_t pio_update_instr_direct_RGB_mode(PIO pio, uint8_t _program_offset) {

    // Create the assembler instruction for the jump to byte send routine
    pio_instr_start_Data = pio_encode_jmp(_program_offset + pio_send_serial_direct_RGB_offset_start_Data);    
    pio_instr_start_Command = pio_encode_jmp(_program_offset + pio_send_serial_direct_RGB_offset_start_Command);
    pio_instr_start_32 = pio_encode_jmp(_program_offset + pio_send_serial_direct_RGB_offset_start_32);    
    pio_instr_start_RGB = pio_encode_jmp(_program_offset + pio_send_serial_direct_RGB_offset_start_RGB);

    return _program_offset + pio_send_serial_direct_RGB_wrap_target;  
}

#elif (INTERFACE_LCD == INTERFACE_LCD_PARALLEL_PIO)

static uint8_t pio_update_instr_parallel_RGB_mode(PIO pio, uint8_t _program_offset) {

    // Create the assembler instruction for the jump to byte send routine
    pio_instr_start_Data = pio_encode_jmp(_program_offset + pio_send_parallel_8bits_RGB_offset_start_Data);    
    pio_instr_start_Command = pio_encode_jmp(_program_offset + pio_send_parallel_8bits_RGB_offset_start_Command);
    pio_instr_start_32 = pio_encode_jmp(_program_offset + pio_send_parallel_8bits_RGB_offset_start_32);    
    pio_instr_start_RGB = pio_encode_jmp(_program_offset + pio_send_parallel_8bits_RGB_offset_start_RGB);

    return _program_offset + pio_send_parallel_8bits_RGB_wrap_target;  
}

#endif  /* INTERFACE_LCD */

static uint8_t pioA_init(uint16_t clock_div) {
    const pio_program_t *program;
    uint8_t wrap_target;
    pio_sm_config c;

#if (INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO)
#if defined(custom_332_565) || defined(custom_332_666) || defined(custom_565_666)
    program = &pio_send_serial_custom_RGB_program;   
#else
    program = &pio_send_serial_direct_RGB_program;
#endif
#elif (INTERFACE_LCD == INTERFACE_LCD_PARALLEL_PIO)
    program = &pio_send_parallel_8bits_RGB_program; 
#endif  /* INTERFACE_LCD */

    if (pio_check_available_pio(&PIO_LCD_NB, program) < 0) {
        return ERR_PIO_NO_AVAILABLE;
    }

    // pioinit will claim a free one
    PIO_LCD_SM = pio_claim_unused_sm(PIO_LCD_NB, false);

    // Load the PIO program
    int _program_offset = pio_add_program(PIO_LCD_NB, program);

#if defined(custom_332_565)
    wrap_target = pio_update_instr_custom_RGB_mode(PIO_LCD_NB, _program_offset, &PIO_COLOR_Mode_RGB332_to_RGB565);    
    pio_set_instr_out_custom_RGB(PIO_LCD_NB, _program_offset, 8);      
#elif defined(custom_332_666)
    wrap_target = pio_update_instr_custom_RGB_mode(PIO_LCD_NB, _program_offset, &PIO_COLOR_Mode_RGB332_to_RGB666); 
    pio_set_instr_out_custom_RGB(PIO_LCD_NB, _program_offset, 8);          
#elif defined(custom_565_666)
     wrap_target = pio_update_instr_custom_RGB_mode(PIO_LCD_NB, _program_offset, &PIO_COLOR_Mode_RGB565_to_RGB666); 
    pio_set_instr_out_custom_RGB(PIO_LCD_NB, _program_offset, 16);         
#elif defined(direct_565)
    wrap_target = pio_update_instr_direct_RGB_mode(PIO_LCD_NB, _program_offset);
    pio_set_instr_out_direct_RGB(PIO_LCD_NB, _program_offset, 16);         
#elif defined(direct_666)
    wrap_target = pio_update_instr_direct_RGB_mode(PIO_LCD_NB, _program_offset);
    pio_set_instr_out_direct_RGB(PIO_LCD_NB, _program_offset, 24);    
#else

#endif

    // Configure the state machine
#if (INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO)
#if defined(custom_332_565) || defined(custom_332_666) || defined(custom_565_666)
    c = pio_send_serial_custom_RGB_program_get_default_config(_program_offset);
#else
    c = pio_send_serial_direct_RGB_program_get_default_config(_program_offset);
#endif

    pio_gpio_init(PIO_LCD_NB, TFT_SCLK);
    pio_gpio_init(PIO_LCD_NB, TFT_MOSI);
#if defined(TFT_MISO) && (TFT_MISO>0)    
    pio_gpio_init(PIO_LCD_NB, TFT_MISO);    
#endif
    // Configure the pins to be outputs
    pio_sm_set_consecutive_pindirs(PIO_LCD_NB, PIO_LCD_SM, TFT_SCLK, 1, true);
    pio_sm_set_consecutive_pindirs(PIO_LCD_NB, PIO_LCD_SM, TFT_MOSI, 1, true);
#if defined(TFT_MISO) && (TFT_MISO>0) 
    pio_sm_set_consecutive_pindirs(PIO_LCD_NB, PIO_LCD_SM, TFT_MISO, 1, false);
#endif

    // Define the single side-set pin
    sm_config_set_sideset_pins(&c, TFT_SCLK);

    // Define the pin used for data output
    sm_config_set_out_pins(&c, TFT_MOSI, 1);

#if defined(TFT_MISO) && (TFT_MISO>0) 
    // Define the pin used for data input
    sm_config_set_in_pins(&c, TFT_MOSI);
#endif

#elif (INTERFACE_LCD == INTERFACE_LCD_PARALLEL_PIO)
    c = pio_send_parallel_8bits_RGB_program_get_default_config(_program_offset);

    wrap_target = pio_update_instr_parallel_RGB_mode(PIO_LCD_NB, _program_offset);

#if (MODE_RGB==_MODE_Index_8bits) || defined(_index_to_16_) || (MODE_RGB==_MODE_RGB565_16bits)
    pio_set_instr_parallel_RGB(PIO_LCD_NB, _program_offset, 16);
#elif (MODE_RGB==_MODE_RGB666_18bits)
    pio_set_instr_parallel_RGB(PIO_LCD_NB, _program_offset, 24);
#endif
    // Define the consecutive pins that are used for data output
    for (int i=0;i<8;i++) {
        pio_gpio_init(PIO_LCD_NB, TFT_D0 + i);
    }
    pio_sm_set_consecutive_pindirs(PIO_LCD_NB, PIO_LCD_SM, TFT_D0, 8, true);    
    sm_config_set_out_pins(&c, TFT_D0, 8);

    pio_gpio_init(PIO_LCD_NB, TFT_WR);
    pio_sm_set_consecutive_pindirs(PIO_LCD_NB, PIO_LCD_SM, TFT_WR, 1, true);
    sm_config_set_sideset_pins(&c, TFT_WR);

#endif  /* INTERFACE_LCD */

    // Associate pins with the PIO
    pio_gpio_init(PIO_LCD_NB, TFT_DC);
    pio_sm_set_consecutive_pindirs(PIO_LCD_NB, PIO_LCD_SM, TFT_DC, 1, true);    
    // Define set pin
    sm_config_set_set_pins(&c, TFT_DC, 1);

    //pio_instr_set_dc = pio_encode_set(pio_pins, 1);
    //pio_instr_clr_dc = pio_encode_set(pio_pins, 0);

    // Set clock division
    sm_config_set_clkdiv(&c, clock_div);

    // Make a single 8 words FIFO from the 4 words TX and RX FIFOs
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    // The OSR register shifts to the left, sm designed to send MS byte of a colour first, autopull off
    sm_config_set_out_shift(&c, false, false, 0);

    // Create the pull stall bit mask
    PIO_LCD_PSP = 1u << (PIO_FDEBUG_TXSTALL_LSB + PIO_LCD_SM);

    // Now load the configuration
    pio_sm_init(PIO_LCD_NB, PIO_LCD_SM, wrap_target, &c);

    // Start the state machine.
    pio_sm_set_enabled(PIO_LCD_NB, PIO_LCD_SM, true);   

    return NO_ERR;
}

#ifdef DMA_LCD

static int32_t              dma_toLCD_channel;
static dma_channel_config   dma_toLCD_config;
static bool                 DMA_Enabled = false;
static bool                 _dma_completed = false;

void dma_handler(void) {
    dma_hw->ints0 = 1u << dma_toLCD_channel;

    _dma_completed = true;

    WAIT_FOR_LCD_STALL; // wait TXFIFO is empty
    CS_H; 
}

static bool init_DMA(void) {

    if (DMA_Enabled) 
        return false;

    dma_toLCD_channel = dma_claim_unused_channel(false);

    if (dma_toLCD_channel < 0) 
        return false;

    dma_toLCD_config = dma_channel_get_default_config(dma_toLCD_channel);


#if (MODE_RGB == _MODE_RGB332_8bits) || (MODE_RGB == _MODE_Index_8bits) || defined(index_to_16_) 
    channel_config_set_transfer_data_size(&dma_toLCD_config, DMA_SIZE_8);    
#elif (MODE_RGB == _MODE_RGB565_16bits)    
    channel_config_set_transfer_data_size(&dma_toLCD_config, DMA_SIZE_16);
#else
    channel_config_set_transfer_data_size(&dma_toLCD_config, DMA_SIZE_32);
#endif   
    //channel_config_set_bswap(&dma_toLCD_config, false);
#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)
    channel_config_set_dreq(&dma_toLCD_config, pio_get_dreq(PIO_IXR_NB, PIO_IXR_SM, true));
#else
    channel_config_set_dreq(&dma_toLCD_config, pio_get_dreq(PIO_LCD_NB, PIO_LCD_SM, true));
#endif

    // Tell the DMA to raise IRQ line 0 when the channel finishes a block
    dma_channel_set_irq0_enabled(dma_toLCD_channel, true);

    // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);

    irq_set_enabled(DMA_IRQ_0, true);

    DMA_Enabled = true;
    return true;
}

#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)

static int32_t              dma_channel_B = 1;
static dma_channel_config   dma_config_B;

static int32_t              dma_channel_C = 2;
static dma_channel_config   dma_config_C;


static uint8_t pioB_init(uint16_t clock_div) {
    const pio_program_t *program = &pio_indexer_program;
    pio_sm_config c;

    if (pio_check_available_pio(&PIO_IXR_NB, program) < 0) {
        return -1;
    }
    //PIO_IXR_NB = pio1;  // @to remove

    // pioinit will claim a free one
    PIO_IXR_SM = pio_claim_unused_sm(PIO_IXR_NB, false);

    // Load the PIO program
    int _program_offset = pio_add_program(PIO_IXR_NB, program);
    
    c = pio_indexer_program_get_default_config(_program_offset);

    // Set clock division
    sm_config_set_clkdiv(&c, clock_div);

    //sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE);

    // The OSR register shifts to the right, autopull off
    sm_config_set_out_shift(&c, true, false, 0);

    // The ISR register shifts to the right, autopull onn
    sm_config_set_in_shift(&c, true, true, 0);


    // Create the pull stall bit mask
    PIO_IXR_PULL_TEST = 1u << (PIO_FDEBUG_TXSTALL_LSB + PIO_IXR_SM);

    // Now load the configuration
    pio_sm_init(PIO_IXR_NB, PIO_IXR_SM, _program_offset + pio_indexer_wrap_target, &c);

    // Init X register with Palette address shifted 9 bits equ (512)
    pio_sm_put(PIO_IXR_NB, PIO_IXR_SM, ((uint32_t)Palette) >> 9);
    pio_sm_exec(PIO_IXR_NB, PIO_IXR_SM, pio_encode_pull(false, true));
    pio_sm_exec(PIO_IXR_NB, PIO_IXR_SM, pio_encode_mov(pio_x, pio_osr));

    // Start the state machine.
    pio_sm_set_enabled(PIO_IXR_NB, PIO_IXR_SM, true);  

    return 0;
}



static bool init_DMA_SM8to16_DMA_src(void) {
    
    dma_config_B = dma_channel_get_default_config(dma_channel_B);
    channel_config_set_read_increment(&dma_config_B, false);    
    channel_config_set_write_increment(&dma_config_B, false);

    channel_config_set_transfer_data_size(&dma_config_B, DMA_SIZE_32);

    channel_config_set_dreq(&dma_config_B, pio_get_dreq(PIO_IXR_NB, PIO_IXR_SM, false));    

    // DMA channel B will copy addresses from PIO_IXR to DMA channel C read address register.
    // It is triggered by the PIO RX IXR FIFO request
    dma_channel_configure(dma_channel_B,
        &dma_config_B,
        //&test_src_DMAC,
        &dma_hw->ch[dma_channel_C].al3_read_addr_trig,
        &RX_IXR_FIFO,
        1,
        true);
    return true;
}
   

static bool init_DMA_DMA_src_LCD(void) {

    dma_config_C = dma_channel_get_default_config(dma_channel_C);
    channel_config_set_transfer_data_size(&dma_config_C, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_config_C, false);    
    channel_config_set_write_increment(&dma_config_C, false);

    channel_config_set_dreq(&dma_config_C, pio_get_dreq(PIO_LCD_NB, PIO_LCD_SM, true));

    // DMA channel C will read Palette[color_ix] to copy data to LCD write state machine.
    // It is triggered by LCD write machine TX FIFO request and chains to re-enable channel B.
    channel_config_set_chain_to(&dma_config_C, dma_channel_B);

    dma_channel_configure(dma_channel_C,
        &dma_config_C,
        //&test_dst_DMAC,
        &PIO_LCD_NB->txf[PIO_LCD_SM],
        NULL,
        1,
        false);
    return true;
}

#endif

void dmaWait(void) {
    while (dma_channel_is_busy(dma_toLCD_channel));
}
#endif  /* DMA_LCD */

static uint8_t TFT_Interface_Init(void) {

    // Set clock divider
    uint32_t clock_freq = PIO_FREQUENCY;
    uint16_t clock_div = clock_get_hz(clk_sys) / (clock_freq * 2.0); // 2 cycles per bit
    if (clock_div==0) {
        clock_div = 1;
    }

    uint8_t err = pioA_init(clock_div);
    if (err!=NO_ERR)
        return err;

#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)
    err = pioB_init(clock_div);
    if (err!=NO_ERR)
        return err;

    uint r, g, b;
    for (uint i=0;i<256;i++) {
#if defined(_index_to_16_)
    uint16_t color16 = (i & 0x03)<<3;
    if (i & 0x01) color16 |= 0x0007;
    color16 |= (i & 0x1C)<<6;
    if (i & 0x04) color16 |= 0x00E0;
    color16 |= (i & 0xE0)<<8;     
    if (i & 0x20) color16 |= 0x1800;
    Palette[i] = color16;

#else
        // gray levels
        r = (i * 32)/256;
        g = (i * 64)/256;
        b = (i * 32)/256;
        Palette[i] = r<<11 | g<<5 | b;
#endif
        
    }
#endif

#ifdef DMA_LCD
    init_DMA();
#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)  
    init_DMA_SM8to16_DMA_src();
    init_DMA_DMA_src_LCD();
#endif    
#endif  /* DMA_LCD */

    return err;
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

#if defined(TFT_LED)
    gpio_init(TFT_LED);    
    gpio_set_dir(TFT_LED, GPIO_OUT);    
    gpio_put(TFT_LED, LOW);
#endif

    uint8_t err = TFT_Interface_Init();
    if (err!=NO_ERR)
        return err;

    TFT_HardReset();
    return err;    
}

void TFT_Blacklight(bool enable) {
#if defined(TFT_LED)    
    gpio_put(TFT_LED, (enable==true?HIGH:LOW));
#endif
}

#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_) 

int8_t Load_Palette(const uint8_t *_palette, uint32_t len) {
    uint16_t i=0;
    uint16_t color;
    if (len>768)
        return -1;
    while (len) {
#ifdef GRAY        
        uint r = (*_palette++)>>3;
        uint g = (*_palette++)>>2;
        uint b = (*_palette++)>>3; 
        uint16_t y = (r + g + b) >> 2;

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

#else

int8_t Load_Palette(const uint8_t *_palette, uint32_t len) {
    return -1;
}

#endif

void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    register uint32_t x0x1 = (x0<<16) | x1; 
    register uint32_t y0y1 = (y0<<16) | y1;  

    //pio_sm_set_enabled(PIO_NB, PIO_SM, false); 
    // clr  autopull
    //PIO_NB->sm[PIO_SM].shiftctrl &= ~PIO_SM0_SHIFTCTRL_AUTOPULL_BITS;
    //pio_sm_set_enabled(PIO_NB, PIO_SM, true); 

    WAIT_FOR_LCD_STALL; // wait TXFIFO is empty

    PIO_LCD_NB->sm[PIO_LCD_SM].instr = pio_instr_start_Command;
    TX_LCD_FIFO = TFT_CASET;
    TX_LCD_FIFO = x0x1;     
#if (INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO)    
    WAIT_FOR_LCD_STALL; // wait TXFIFO is empty
    PIO_LCD_NB->sm[PIO_LCD_SM].instr = pio_instr_start_Command;
#endif
    TX_LCD_FIFO = TFT_PASET;
    TX_LCD_FIFO = y0y1;   
#if (INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO)    
    WAIT_FOR_LCD_STALL; // wait TXFIFO is empty
    PIO_LCD_NB->sm[PIO_LCD_SM].instr = pio_instr_start_Command;
#endif
    TX_LCD_FIFO = TFT_RAMWR;
    WAIT_FOR_LCD_STALL; // wait TXFIFO is empty

    // set  autopull
    //PIO_NB->sm[PIO_SM].shiftctrl |= PIO_SM0_SHIFTCTRL_AUTOPULL_BITS;
    
    PIO_LCD_NB->sm[PIO_LCD_SM].instr = pio_instr_start_RGB;
}


void setPixel(uint16_t x, uint16_t y, uint32_t color) {
    setWindow(x, y, x, y);
    //WAIT_FOR_STALL; // wait TXFIFO is empty    
#if (MODE_RGB == _MODE_Index_8bits)|| defined(_index_to_16_)
    TX_IXR_FIFO = color;
#else    
    TX_LCD_FIFO = color;
#endif
}



#ifdef DMA_LCD


void pushBlockDMA(uint32_t _color, uint32_t _len) {
    static uint32_t color;

    if ((_len == 0) || (!DMA_Enabled)) 
        return;

    dmaWait();
    color = _color;
    _dma_completed = false;

    channel_config_set_read_increment(&dma_toLCD_config, false);

#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)
    dma_channel_configure(dma_toLCD_channel, &dma_toLCD_config, &PIO_IXR_NB->txf[PIO_IXR_SM], &color, _len, true);  
#else
    dma_channel_configure(dma_toLCD_channel, &dma_toLCD_config, &PIO_LCD_NB->txf[PIO_LCD_SM], &color, _len, true);
#endif

}
#endif

void pushBlock(uint32_t color, uint32_t len) {

#ifdef DMA_LCD
    if (len>16) {
        pushBlockDMA(color, len);
        return;
    }
#endif
    while (len) { 
#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)
        WAIT_FOR_IXR_STALL; // wait TXFIFO is empty
        TX_IXR_FIFO = color;
        len-=1;
#else
       
        if (len>4) {
            WAIT_FOR_LCD_FIFO_FREE(4);
            TX_LCD_FIFO = color;
            TX_LCD_FIFO = color;
            TX_LCD_FIFO = color;
            TX_LCD_FIFO = color;
            len-=4;
        } else {
            WAIT_FOR_LCD_STALL; // wait TXFIFO is empty
            TX_LCD_FIFO = color;
            len-=1;
        } 

#endif
    }
    WAIT_FOR_LCD_STALL;
    CS_H; 
}

#ifdef DMA_LCD
 
void pushImageDMA(const void *_image, uint32_t _len) {

    if ((_len == 0) || (!DMA_Enabled)) 
        return;

    dmaWait();

    _dma_completed = false;
   
    channel_config_set_read_increment(&dma_toLCD_config, true);    
#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)
    dma_channel_configure(dma_toLCD_channel, &dma_toLCD_config, &PIO_IXR_NB->txf[PIO_IXR_SM], _image, _len, true);  
#else    
    dma_channel_configure(dma_toLCD_channel, &dma_toLCD_config, &PIO_LCD_NB->txf[PIO_LCD_SM], _image, _len, true);
#endif
}
#endif

  
void pushImage(const void *buffer, uint32_t len) {

#if (MODE_RGB == _MODE_RGB332_8bits) || defined(_index_to_16_) 
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
    while (len) {

#if (MODE_RGB == _MODE_Index_8bits) || defined(_index_to_16_)
        WAIT_FOR_IXR_STALL; // wait TXFIFO is empty
        TX_IXR_FIFO = *image++;
        len-=1;
#else
        if (len>4) {        
            WAIT_FOR_LCD_FIFO_FREE(4);
            TX_LCD_FIFO = *image++;
            TX_LCD_FIFO = *image++;
            TX_LCD_FIFO = *image++;
            TX_LCD_FIFO = *image++;      
            len-=4;              
        } else {
            WAIT_FOR_LCD_STALL; // wait TXFIFO is empty
            TX_LCD_FIFO = *image++;
            len-=1;
        }    
#endif                        
    }
    WAIT_FOR_LCD_STALL;
    CS_H;    
}

#ifdef DMA_LCD
#endif


#pragma GCC pop_options


#endif /* INTERFACE_LCD == INTERFACE_LCD_SERIAL_PIO || INTERFACE_LCD == INTERFACE_LCD_PARALLEL_PIO */




