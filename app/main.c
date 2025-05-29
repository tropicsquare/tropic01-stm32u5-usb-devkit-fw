#include "os.h"
#include "main.h"
#include "gpio.h"
#include "time.h"
#include "reset.h"
#include "tty.h"
#include "wd.h"
#include "gpreg.h"
#include "led.h"
#include "sys.h"
#include "usb_device.h"
#include "spi.h"
#include "cmd.h"
#include "log.h"

LOG_DEF("main");

#include "stm32u5xx_ll_utils.h"

bool main_spi_auto = false;
u8 main_spi_get_resp = 0;
u8 main_spi_no_resp = 0;

static bool _spi_cs_active = false;
#define _SPI_BUF_SIZE (512)

#define MAIN_LED_INIT  HW_LED1_INIT
#define MAIN_LED_ON    HW_LED1_ON
#define MAIN_LED_OFF   HW_LED1_OFF

static led_t led1;

#define _LED_MODE_IDLE  0x01,4,0
#define _LED_MODE_READY 0x0F,4,0

static u8 reset_type = RESET_POWER_ON;

static void main_gpio_init(void)
{
    gpio_init();

    MAIN_LED_INIT;
    MAIN_LED_ON;

#ifdef HW_TS13
    HW_GPO_IN_INIT;
    HW_BUTTON_INIT;
    HW_CHIP_PWR_OFF;
    HW_CHIP_PWR_INIT;
#else // HW_TS13
    HW_SPI_OE_DISABLE;
    HW_SPI_OE_INIT;

    HW_GPO0_IN_INIT;
    HW_GPO1_IN_INIT;
    HW_GPO2_IN_INIT;
    HW_GPO3_IN_INIT;
    HW_GPO4_IN_INIT;

    HW_MODE0_IN_INIT;
    HW_MODE1_IN_INIT;
    HW_MODE2_IN_INIT;
    HW_MODE3_IN_INIT;
#endif // ! HW_TS13
    
    // put USB to reset state
    GPIO_BIT_CLR(HW_USB_DP_PORT, HW_USB_DP_BIT);
    GPIO_PIN_INIT(HW_USB_DP_PORT, HW_USB_DP_BIT, GPIO_MODE_OUTPUT);

    HW_CHIP_PWR_INIT;
    HW_CHIP_PWR_ON;
}

void Error_Handler(void)
{
    // User can add his own implementation to report the HAL error return state
    __disable_irq();
    while (1)
    {
        MAIN_LED_ON;
        OS_DELAY(100);
        MAIN_LED_OFF;
        OS_DELAY(100);
    }
}

u32 HAL_GetTick(void)
{
	return (os_timer_get_time());
}

static bool _get_hex(u8 *dest, const char *src)
{   // read one byte of HEX value
    if (hex_to_bin(dest, src, 1) != 1)
        return (false);

    return (true);
}

static void _led1_on (void)
{
    MAIN_LED_ON;
}

static void _led1_off (void)
{
    MAIN_LED_OFF;
}

static void _spi_cs_enable(void)
{
    spi1_cs(true);
    _spi_cs_active = true;
}

static void _spi_cs_disable(void)
{
    spi1_cs(false);
    _spi_cs_active = false;
}

static void _spi_auto_task(void)
{   //
    char spi_buf[_SPI_BUF_SIZE];
    int i, l;

    spi1_flush();

    _spi_cs_enable();

    spi_buf[0] = spi1_transfer(main_spi_get_resp);
    // TODO: we can check busy bit here
    spi_buf[0] = spi1_transfer(0); // header

    if (spi_buf[0] == main_spi_no_resp)
    {   // no response to read
        _spi_cs_disable();
        // TODO: automatic read MSG_HDR_DEBUG ?
        return;
    }

    l = spi1_transfer(0);
    spi_buf[1] = l; // data length

    for (i=2; i<(2+l+2); i++)
    {
        spi_buf[i] = spi1_transfer(0);
    }
    
    _spi_cs_disable();

    // print result
    for (i=0; i<(2+l+2); i++)
    {
        OS_PRINTF("%02X", spi_buf[i]);
    }
    OS_PRINTF(NL);
}

static bool _skip_cs_char(char ch)
{   // following characters can be used for skip CS manipulation
    // only usable as first or last character (or both) in SPI data stream
    return (((ch == 'x') || (ch == '\\')) ? true : false);
}

static bool _parse_hex(char *data)
{
    char spi_tx_buf[_SPI_BUF_SIZE];
    char spi_rx_buf[_SPI_BUF_SIZE];
    int buf_size = 0;
    int i;
    u8 tmp;
    bool skip_cs = false;

    if (_skip_cs_char(*data))
    {   // 
        data++;
        skip_cs = true;
    }

    if (! _get_hex(&tmp, data))
        return (false);

    // convert HEX to binary data to buffer
    do
    {
        data+=2;
        while (*data == ' ')
            data++; // skip spaces

        spi_tx_buf[buf_size] = tmp;

        if (buf_size < (_SPI_BUF_SIZE-1))
            buf_size++;
    }
    while (_get_hex(&tmp, data));

    _led1_off();

    if (skip_cs == false)
    {
        _spi_cs_enable();
    }

    // transfer buffer to spi
    spi1_data_transfer((u8 *)spi_rx_buf, (u8 *)spi_tx_buf, buf_size);

    if (! _skip_cs_char(*data))
        _spi_cs_disable();

    // print result
    for (i=0; i<buf_size; i++)
    {
        OS_PRINTF("%02X", spi_rx_buf[i]);
    }
    OS_PRINTF(NL);
    return (true);
}

static void _tty_rx_parser(char *data)
{
    while (*data == ' ')
        data++; // skip spaces

    if (! _parse_hex(data))
    {
        cmd_parse(data);
    }
    OS_FLUSH();
}

static void _usb_update_state(void)
{
    static bool prev_state = false;
    
    bool state = usb_device_connected();

    if (prev_state == state)
        return;

    if (state)
    {
        // OS_PUTTEXT("READY" NL);
        led_cyclic_sequence(&led1, _LED_MODE_READY);
        HW_SPI_OE_ENABLE;
    }
    else
    {
        led_cyclic_sequence(&led1, _LED_MODE_IDLE);
        HW_SPI_OE_DISABLE;
    }

    prev_state = state;
}

static void _main_task(void)
{
    os_timer_t now, timer_100ms = 0;
    os_timer_t timer_10s = 0;

    reset_clear();
    
    led_init(&led1);
    led1.on = _led1_on;
    led1.off = _led1_off;
    led_cyclic_sequence(&led1, _LED_MODE_IDLE);
   
    usb_device_init();
    spi1_init();

    timer_100ms = timer_get_time();
    timer_10s = timer_100ms;

    while (1)
    {
        now = timer_get_time();
        tty_rx_task();
        usb_device_task();

        if (now > timer_100ms)
        {
            _usb_update_state();
            timer_100ms += 100*TIMER_MS;
            led_tick(&led1);
            wd_feed();
            if (main_spi_auto)
            {
                if (_spi_cs_active == false)
                {
                    _spi_auto_task();
                }
            }
        }
        if (now > timer_10s)
        {
            timer_10s += 10*1000*TIMER_MS;
            // 
        }
        // __WFI(); // at least 1ms timer IRQ running
    }
}

int main(void)
{
    sys_init();
    sys_clock_config();
    
    main_gpio_init();

    wd_init();
    wd_run();

    reset_type = reset_get_type();

    timer_init();

    OS_DELAY(10);
    tty_init(_tty_rx_parser);
    OS_DELAY(10);

    OS_PUTTEXT(NL);
    OS_PUTTEXT("APP START" NL);
    MAIN_LED_OFF;
    
    OS_PUTTEXT("# BUILD DATE: " __DATE__ NL);

    OS_PUTTEXT("# RESET TYPE: ");
    switch (reset_type)
    {
    case RESET_POWER_ON: OS_PUTTEXT("POWER_ON"); break;
    case RESET_USER_RQ:  OS_PUTTEXT("USER_RQ");  break;
    case RESET_WDT:      OS_PUTTEXT("WDT");      break;
    case RESET_BOD:      OS_PUTTEXT("BOD");      break;
    default:             OS_PUTTEXT("UNKNOWN");  break;
    }
    OS_PUTTEXT(NL);
    OS_PUTTEXT(NL);

    _main_task();

    // should never continue here
    while (1)
        ;
}


