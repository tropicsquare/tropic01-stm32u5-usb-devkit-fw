#include "common.h"
#include "tty.h"
#include "usb_device.h"

#ifndef TTY_ON_UART
    #warning "No TTY uart defined"
#endif // ! TTY_ON_UART


tty_parse_callback_t _rx_callback = NULL;

typedef struct {
    char data[TTY_BUF_SIZE];
    size_t len;
} tty_buf_t;

#ifndef USB_TTY_BUFFER_SIZE
  // default stream buffer size, minimum TTY_BUF_SIZE
  #define USB_TTY_BUFFER_SIZE (4*1024)
  // supports pipelining up to this input data size
#endif // USB_TTY_BUFFER_SIZE

#if USB_TTY_BUFFER_SIZE < TTY_BUF_SIZE
  #error "too small buffer"
#endif // 

static char _usb_stream_buffer[USB_TTY_BUFFER_SIZE];
static size_t _usb_stream_wr_ptr = 0;
static size_t _usb_stream_rd_ptr = 0;

static void _usb_send_data(u8 *data, u16 len)
{
#define USB_RETRY_NUM (100)

    int limit;

    if (! usb_device_connected())
        return;

    limit = USB_RETRY_NUM;
    while (usb_cdc_tx(data, len) == USB_RESULT_BUSY)
    {
        if (--limit == 0)
            return;
        OS_DELAY(1);
    }
    limit = 10;
    while (usb_cdc_tx_busy())
    {
        if (--limit == 0)
            return;
        OS_DELAY(1);
    }
}

int _write (int fd, const void *buf, size_t count)
{   // gcc stdout
    char *ptr = (char *)buf;
    size_t n = count;

    _usb_send_data((u8 *)buf, count);

    while (n--)
    {
        if (! TTY_UART_PUTCHAR(*ptr++))
            break;
    }
    return (count - n);
}

int _read (int fd, const void *buf, size_t count)
{   // gcc stdin
    return (0);
}

static void _usb_rx_handler(u8 *buf, u32 len)
{
    u32 i;

    size_t wr_ptr = _usb_stream_wr_ptr;

    for (i=0; i<len; i++)
    {
        if (++wr_ptr >= USB_TTY_BUFFER_SIZE)
            wr_ptr=0;

        if (wr_ptr == _usb_stream_rd_ptr)
        {
            OS_ERROR("USB RX overflow !");
            return;
        }
        _usb_stream_buffer[wr_ptr] = buf[i];
    }
    _usb_stream_wr_ptr = wr_ptr;
}

int _usb_getchar(void)
{
    size_t rd_ptr = _usb_stream_rd_ptr;

    if (rd_ptr == _usb_stream_wr_ptr)
        return (-1);

    if (++rd_ptr >= USB_TTY_BUFFER_SIZE)
        rd_ptr=0;

    _usb_stream_rd_ptr = rd_ptr;
    return (_usb_stream_buffer[rd_ptr]);
}

void _rx_feed(tty_buf_t *buf, char ch)
{
    if ((ch == '\r') || (ch == '\n'))
    {
        ch = '\0';
    }
    buf->data[buf->len] = ch;

    if (ch == '\b')
    {   // backspace (code 0x08)
        if (buf->len>0)
            buf->len--;
        return;
    }

    if (ch =='\0')
    {
        if (buf->len>0)
        {
            if (_rx_callback != NULL)
                _rx_callback(buf->data);

            buf->len = 0;
        }
    }
    else if (buf->len < (TTY_BUF_SIZE-1))
        buf->len++;
}

void tty_rx_task(void)
{
    static tty_buf_t usb_rx_buf;
    static tty_buf_t uart_rx_buf;

    int ch;

    // process USB RX data
    while ((ch = _usb_getchar()) >= 0)
    {
        _rx_feed(&usb_rx_buf, ch);
    }

    // process UART RX data
    while ((ch = TTY_UART_GETCHAR()) >= 0)
    {
        _rx_feed(&uart_rx_buf, ch);
    }
}

void tty_put_binary(u8 *data, size_t len)
{
    _usb_send_data(data, len);
    // NOTE: we dont send binary data to UART in this function
}

void tty_put_text(char *text)
{
    _usb_send_data((u8 *)text, strlen(text));

    while (*text != '\0')
    {
        TTY_UART_PUTCHAR(*text);
        text++;
    }
}

bool tty_init(tty_parse_callback_t callback)
{
    TTY_UART_INIT(115200);
    _rx_callback = callback;
    usb_cdc_rx_init(_usb_rx_handler);
    return (true);
}

