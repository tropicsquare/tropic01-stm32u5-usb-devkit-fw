#include "common.h"
#include "hardware.h"
#include "irq.h"
#include "dma.h"

#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_hal_dma.h"
#include "stm32u5xx_hal_spi.h"

// based on example
// https://github.com/STMicroelectronics/STM32CubeU5.git
// ./Projects/NUCLEO-U575ZI-Q/Examples/SPI/SPI_FullDuplex_ComDMA_Master


static DMA_HandleTypeDef handle_GPDMA1_Channel7;
static DMA_HandleTypeDef handle_GPDMA1_Channel6;

static DMA_NodeTypeDef Node_tx;
static DMA_QListTypeDef Queue_tx;
static DMA_NodeTypeDef Node_rx;
static DMA_QListTypeDef Queue_rx;

extern SPI_HandleTypeDef hspi1;

/**
  * @brief  DMA Linked-list Queue_tx configuration
  */
static void _linked_list_tx_config(void)
{
    // DMA node configuration declaration
    DMA_NodeConfTypeDef pNodeConfig;

    // Set node configuration
    pNodeConfig.NodeType = DMA_GPDMA_LINEAR_NODE;
    pNodeConfig.Init.Request = GPDMA1_REQUEST_SPI1_TX;
    pNodeConfig.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    pNodeConfig.Init.Direction = DMA_MEMORY_TO_PERIPH;
    pNodeConfig.Init.SrcInc = DMA_SINC_INCREMENTED;
    pNodeConfig.Init.DestInc = DMA_DINC_FIXED;
    pNodeConfig.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    pNodeConfig.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    pNodeConfig.Init.SrcBurstLength = 1;
    pNodeConfig.Init.DestBurstLength = 1;
    pNodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT1|DMA_DEST_ALLOCATED_PORT0;
    pNodeConfig.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    pNodeConfig.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
    pNodeConfig.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
    pNodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
    pNodeConfig.SrcAddress = 0;
    pNodeConfig.DstAddress = 0;
    pNodeConfig.DataSize = 0;

    // Build Node_tx Node
    HAL_DMAEx_List_BuildNode(&pNodeConfig, &Node_tx);

    // Insert Node_tx to Queue
    HAL_DMAEx_List_InsertNode_Tail(&Queue_tx, &Node_tx);
}
  
/**
  * @brief  DMA Linked-list Queue_rx configuration
  */
static void _linked_list_rx_config(void)
{
    // DMA node configuration declaration
    DMA_NodeConfTypeDef pNodeConfig;

    // Set node configuration
    pNodeConfig.NodeType = DMA_GPDMA_LINEAR_NODE;
    pNodeConfig.Init.Request = GPDMA1_REQUEST_SPI1_RX;
    pNodeConfig.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    pNodeConfig.Init.Direction = DMA_PERIPH_TO_MEMORY;
    pNodeConfig.Init.SrcInc = DMA_SINC_FIXED;
    pNodeConfig.Init.DestInc = DMA_DINC_INCREMENTED;
    pNodeConfig.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    pNodeConfig.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    pNodeConfig.Init.SrcBurstLength = 1;
    pNodeConfig.Init.DestBurstLength = 1;
    pNodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
    pNodeConfig.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    pNodeConfig.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
    pNodeConfig.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
    pNodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
    pNodeConfig.SrcAddress = 0;
    pNodeConfig.DstAddress = 0;
    pNodeConfig.DataSize = 0;

    // Build Node_rx Node
    HAL_DMAEx_List_BuildNode(&pNodeConfig, &Node_rx);

    // Insert Node_rx to Queue
    HAL_DMAEx_List_InsertNode_Tail(&Queue_rx, &Node_rx);
}


bool dma_init(void)
{
    // Peripheral clock enable 
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPDMA1);

    return (true);
}

void dma_init_spi_rx(void)
{   // GPDMA1 channel 6
    handle_GPDMA1_Channel6.Instance = GPDMA1_Channel6;
    handle_GPDMA1_Channel6.InitLinkedList.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
    handle_GPDMA1_Channel6.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
    handle_GPDMA1_Channel6.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT1;
    handle_GPDMA1_Channel6.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
    handle_GPDMA1_Channel6.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_NORMAL;
    if (HAL_DMAEx_List_Init(&handle_GPDMA1_Channel6) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel6, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
        Error_Handler();
    }
    irq_enable(GPDMA1_Channel6_IRQn, 0);

    _linked_list_rx_config();
    HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel6, &Queue_rx);
    __HAL_LINKDMA(&hspi1, hdmarx, handle_GPDMA1_Channel6);
}

void dma_init_spi_tx(void)
{   // GPDMA1 channel 7
    handle_GPDMA1_Channel7.Instance = GPDMA1_Channel7;
    handle_GPDMA1_Channel7.InitLinkedList.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
    handle_GPDMA1_Channel7.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
    handle_GPDMA1_Channel7.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT1;
    handle_GPDMA1_Channel7.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
    handle_GPDMA1_Channel7.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_NORMAL;
    if (HAL_DMAEx_List_Init(&handle_GPDMA1_Channel7) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel7, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
        Error_Handler();
    }
    irq_enable(GPDMA1_Channel7_IRQn, 0);

    _linked_list_tx_config();
    HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel7, &Queue_tx);
    __HAL_LINKDMA(&hspi1, hdmatx, handle_GPDMA1_Channel7);
}

void GPDMA1_Channel6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel6);
}

void GPDMA1_Channel7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel7);
}
