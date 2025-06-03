#include "common.h"
#include "hardware.h"

#include "spi.h"
#include "gpio.h"
#include "dma.h"
#include "irq.h"
#include "sys.h"

#include "log.h"
LOG_DEF("SPI");

#include <stm32u5xx_ll_bus.h>
#include <stm32u5xx_ll_rcc.h>
#include <stm32u5xx_ll_spi.h>

#include <stm32u5xx_hal_dma.h>
#include <stm32u5xx_hal_spi.h>

#if SPI1_ON

#define PRESCALER_APB2_MAX 16
#define PRESCALER_SPI_MAX 256

static bool _spi1_cs_state = SPI_CS_IDLE; // true == active == LOW
static volatile bool _spi_transfer_done = false;

SPI_HandleTypeDef hspi1;

void Error_Handler(void);

static void _spi1_pin_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    // SPI1 GPIO Configuration
    //   PA5   ------> SPI1_SCK
    //   PA6   ------> SPI1_MISO
    //   PA7   ------> SPI1_MOSI
    
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HW_SPI_SW_CS_INIT;
    HW_SPI_SW_CS_UP;
}

u32 spi1_get_prescaler(void)
{
    switch (SPI1->CFG1 & SPI_CFG1_MBR)
    {
    case LL_SPI_BAUDRATEPRESCALER_DIV2: return (2);
    case LL_SPI_BAUDRATEPRESCALER_DIV4: return (4);
    case LL_SPI_BAUDRATEPRESCALER_DIV8: return (8);
    case LL_SPI_BAUDRATEPRESCALER_DIV16: return (16);
    case LL_SPI_BAUDRATEPRESCALER_DIV32: return (32);
    case LL_SPI_BAUDRATEPRESCALER_DIV64: return (64);
    case LL_SPI_BAUDRATEPRESCALER_DIV128: return (128);
    case LL_SPI_BAUDRATEPRESCALER_DIV256: return (256);
    default:
        break;
    }
    return (0); // impossible value
}

bool spi1_set_prescaler(u32 value)
{
    u32 prescaler;

    switch (value)
    {
    case   2: prescaler = LL_SPI_BAUDRATEPRESCALER_DIV2;   break;
    case   4: prescaler = LL_SPI_BAUDRATEPRESCALER_DIV4;   break;
    case   8: prescaler = LL_SPI_BAUDRATEPRESCALER_DIV8;   break;
    case  16: prescaler = LL_SPI_BAUDRATEPRESCALER_DIV16;  break;
    case  32: prescaler = LL_SPI_BAUDRATEPRESCALER_DIV32;  break;
    case  64: prescaler = LL_SPI_BAUDRATEPRESCALER_DIV64;  break;
    case 128: prescaler = LL_SPI_BAUDRATEPRESCALER_DIV128; break;
    case 256: prescaler = LL_SPI_BAUDRATEPRESCALER_DIV256; break;
    default:
        return (false);
    }
    SPI1->CFG1 = (SPI1->CFG1 & ~SPI_CFG1_MBR) | prescaler;
    return (true);
}

void spi1_init(void)
{
    SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = {0};

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 0x7;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
    if (HAL_SPIEx_SetConfigAutonomousMode(&hspi1, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
    {
        Error_Handler();
    }

    dma_init_spi_rx();
    dma_init_spi_tx();
}

void spi1_data_transfer(u8 *rx, u8 *tx, size_t len)
{   // 
    _spi_transfer_done = false;
   
    // non-blocking function
    HAL_SPI_TransmitReceive_DMA(&hspi1, (u8*)tx, (u8 *)rx, len);
    
    // make it blocking, wait until done
    while (_spi_transfer_done == false)
        ;
}

void spi1_flush(void)
{
    while (SPI1->SR & SPI_SR_RXP)
    {
        u8 tmp = SPI1->RXDR;
        (void)tmp;
    }
}

u8 spi1_transfer(u8 c)
{
    u8 rx;

    spi1_data_transfer(&rx, &c, 1);
    return rx;
}

bool spi1_cs_state(void)
{
    return(_spi1_cs_state ? true : false);
}

void spi1_cs(bool state)
{
    if (state)
    {
        HW_SPI_SW_CS_DOWN;
        _spi1_cs_state = SPI_CS_ACTIVE;
    }
    else
    {
        HW_SPI_SW_CS_UP;
        _spi1_cs_state = SPI_CS_IDLE;
    }
}

void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi1);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    if(hspi->Instance == SPI1)
    {
        LL_RCC_SetSPIClockSource(LL_RCC_SPI1_CLKSOURCE_SYSCLK);
        // Peripheral clock enable
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
        dma_init();
        _spi1_pin_init();
        irq_enable(SPI1_IRQn, SPI_ISR_PRIO);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) 
    {
        _spi_transfer_done = true;
    }
}
#endif // SPI1_ON


#if SPI2_ON

#error "SPI2 not implemented"

#endif // SPI2_ON

