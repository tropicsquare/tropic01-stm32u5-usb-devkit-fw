#include "platform_setup.h"
#include "uart.h"
#include "irq.h"
#include "gpio.h"
#include "os.h"

#include "stm32u5xx_ll_lpuart.h"
// #include "stm32u5xx_ll_usart.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_rcc.h"

typedef struct {
    char  *rx_buf;
    volatile u32   rx_wr_index;
    volatile u32   rx_wr_cnt;
    volatile u32   rx_rd_index;
    volatile u32   rx_rd_cnt;
    volatile int   rx_run;

    char *tx_buf;
    volatile u32   tx_wr_index;
    volatile u32   tx_wr_cnt;
    volatile u32   tx_rd_index;
    volatile u32   tx_rd_cnt;
    volatile int   tx_run;
} uart_t;

#if HW_UART1_ON

static char  u1_rx_buf[UART1_RX_BUF_DEPTH];
static char  u1_tx_buf[UART1_TX_BUF_DEPTH];
static uart_t u1;

static inline void _u1_rx(const char c)
{
    if ((u32)(u1.rx_wr_cnt-u1.rx_rd_cnt) >= UART1_RX_BUF_DEPTH)
    {   //buffer full
        return;
    }

    u1.rx_buf[u1.rx_wr_index]=c;

    if (++u1.rx_wr_index >= UART1_RX_BUF_DEPTH)
        u1.rx_wr_index=0;

    u1.rx_wr_cnt++;
}

int uart1_getchar(void)
{
    int c;

    if(u1.rx_rd_cnt == u1.rx_wr_cnt)
    {   //buffer empty
        return (-1);
    }

    c = u1.rx_buf[u1.rx_rd_index];

    if (++u1.rx_rd_index >= UART1_RX_BUF_DEPTH)
        u1.rx_rd_index=0;

    u1.rx_rd_cnt++;
    return (c);
}

int uart1_putchar(const char c)
{
    if ((u32)(u1.tx_wr_cnt-u1.tx_rd_cnt) >= UART1_TX_BUF_DEPTH)
    {   //buffer full
        return (false);
    }
    u1.tx_buf[u1.tx_wr_index] = c;

    if (++u1.tx_wr_index >= UART1_TX_BUF_DEPTH)
        u1.tx_wr_index = 0;
    u1.tx_wr_cnt++;

    if (u1.tx_run == false)
    {
        u1.tx_run = true;
        LPUART1->CR1 |= USART_CR1_TXEIE_TXFNFIE; 
    }
    return (true);
}

static inline void _u1_tx(void)
{
    char c;
    c = u1.tx_buf[u1.tx_rd_index];

    if (++u1.tx_rd_index >= UART1_TX_BUF_DEPTH)
        u1.tx_rd_index=0;

    u1.tx_rd_cnt++;
    LPUART1->TDR = c;
}

#if UART1_IRQ_USED
void LPUART1_IRQHandler(void)
{
    u16 status;

    status = LPUART1->ISR;

    if ((status & USART_ISR_TXE_TXFNF) && (LPUART1->CR1 & USART_CR1_TXEIE_TXFNFIE))
    {
        if (u1.tx_wr_cnt == u1.tx_rd_cnt)//buffer empty
        {
            LPUART1->CR1 &= ~USART_CR1_TXEIE_TXFNFIE;
            u1.tx_run=false;
        }
        else
        {
            _u1_tx();
        }
    }
    if (status & USART_ISR_RXNE_RXFNE)
    {
        _u1_rx(LPUART1->RDR);
    }
    // (Optional) Handle errors if needed
    if (status & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_PE))
    {
        // Clear error flags by reading RDR and writing to ICR
        (void)LPUART1->RDR;
        LPUART1->ICR = (USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_PECF);
    }
}
#endif // UART1_IRQ_USED

void uart1_init(u32 baudrate)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_LPUART_InitTypeDef LPUART_InitStruct = {0};

    memset(&u1, 0, sizeof(u1));
    u1.rx_buf = u1_rx_buf;
    u1.tx_buf = u1_tx_buf;

    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK3);

    LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_LPUART1);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    // LPUART1 GPIO Configuration
    // PA2   ------> LPUART1_TX
    // PA3   ------> LPUART1_RX
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
    LPUART_InitStruct.BaudRate = 115200;
    LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
    LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
    LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
    LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
    LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
    LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
    LL_LPUART_SetTXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);
    LL_LPUART_SetRXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);
    LL_LPUART_DisableFIFO(LPUART1);
    LL_LPUART_Enable(LPUART1);

#if UART1_IRQ_USED
    irq_enable(LPUART1_IRQn, UART1_ISR_PRIO);
#endif
    // LPUART1->CR1 = USART_CR1_UE | USART_CR1_TXEIE_TXFNFIE | USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TE | USART_CR1_RE;
    LPUART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE;
}

void uart1_set_state(bool enabled)
{
    if (enabled)
    {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    }
    else
    {
        RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
    }

}

#endif // HW_UART1_ON

#if HW_UART2_ON

#error "UART2 not implemented"

#endif // HW_UART2_ON
