/**
  ******************************************************************************
  * @file       sp_dma.c
  * @author     YTom
  * @version    v0.0-alpha
  * @date       2018.Nov.24
  * @brief      DMA module driver
  ******************************************************************************
  * @license
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sp_dma.h"



spDMA_SelectorType spDMA_Mem2Mem[] = {
    /* These 2 DMA streams most likely won't be used by other peripherals in RM-TypeA board. */
    {DMA2_Stream0, DMA_Channel_1},
    {DMA2_Stream7, DMA_Channel_0},
};
/* These channels are not used in STM32, can be used for memory copy. */
//    {DMA2_Stream6, DMA_Channel_3},
//    {DMA1_Stream0, DMA_Channel_7},
//    {DMA1_Stream1, DMA_Channel_0},
//    {DMA1_Stream1, DMA_Channel_1},
//    {DMA1_Stream1, DMA_Channel_2},
//    {DMA1_Stream3, DMA_Channel_1},
//    {DMA2_Stream5, DMA_Channel_5},
//    {DMA1_Stream5, DMA_Channel_6},
//    {DMA1_Stream6, DMA_Channel_0},
//    {DMA1_Stream7, DMA_Channel_6},
//    {DMA2_Stream0, DMA_Channel_5},
//    {DMA2_Stream0, DMA_Channel_4},
//    {DMA2_Stream1, DMA_Channel_3},
//    {DMA2_Stream2, DMA_Channel_2},
//    {DMA2_Stream4, DMA_Channel_4},
//    {DMA2_Stream4, DMA_Channel_4},
//    {DMA2_Stream7, DMA_Channel_3},
//    {DMA2_Stream7, DMA_Channel_6}





void DMA_ClearStreamFlagBit(DMA_Stream_TypeDef* stream, uint32_t flag) {
    uint8_t id = 0;
    /* Clear LISR and HISR before restart DMA */
    if((uint32_t)stream<(uint32_t)DMA2_BASE) {
        id = ((uint32_t)stream-(uint32_t)DMA1_BASE)/0x18;
        /* Calculate mask */
        id = (id/2)*16+(id%2)*6;
        if(id<32) {
            DMA1->LIFCR |= flag<<id;
        } else {
            DMA1->HIFCR |= flag<<(id-32);
        }
    } else {
        id = ((uint32_t)stream-(uint32_t)DMA2_BASE)/0x18;
        /* Calculate mask */
        id = (id/2)*16+(id%2)*6;
        if(id<32) {
            DMA2->LIFCR |= flag<<id;
        } else {
            DMA2->HIFCR |= flag<<(id-32);
        }
    }
}


bool DMA_GetStreamFlagBit(DMA_Stream_TypeDef* stream, uint32_t flag) {
    uint8_t id = 0;
    uint32_t mask;
    /* Clear LISR and HISR before restart DMA */
    if((uint32_t)stream<(uint32_t)DMA2_BASE) {
        id = ((uint32_t)stream-(uint32_t)DMA1_BASE)/0x18;
        /* Calculate mask */
        id = (id/2)*16+(id%2)*6;
        if(id<32) {
            mask = DMA1->LISR & flag<<id;
        } else {
            mask = DMA1->HISR & flag<<(id-32);
        }
    } else {
        id = ((uint32_t)stream-(uint32_t)DMA2_BASE)/0x18;
        /* Calculate mask */
        id = (id/2)*16+(id%2)*6;
        if(id<32) {
            mask = DMA2->LISR & flag<<id;
        } else {
            mask = DMA2->HISR & flag<<(id-32);
        }
    }
    return mask ? true:false;
}


bool DMA_InitNull(uint8_t* addr_from, uint8_t* addr_to, uint16_t buffsize) {
    // if(!buffsize) return false;
    
    DMA_InitTypeDef     DMA_InitStructure;
    
    spRCC_Set_DMA2();
    
    for(uint8_t i=0; i<sizeof(spDMA_Mem2Mem)/sizeof(spDMA_Mem2Mem[0]); ++i) {

        DMA_DeInit(spDMA_Mem2Mem[i].stream);
        while(DMA_GetCmdStatus(spDMA_Mem2Mem[i].stream)!=DISABLE); 
        DMA_InitStructure.DMA_Channel               = spDMA_Mem2Mem[i].channel;
        DMA_InitStructure.DMA_DIR                   = DMA_DIR_MemoryToMemory;
        DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)addr_from;
        DMA_InitStructure.DMA_Memory0BaseAddr       = (uint32_t)addr_to;
        DMA_InitStructure.DMA_BufferSize            = buffsize;
        DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Enable;
        DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize        = DMA_PeripheralDataSize_Byte;  
        DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority              = DMA_Priority_Medium;
        /* Disbale FIFO mode */
        DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;
        DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_1QuarterFull;
        /* Trigger DMA transfer every single memory transfer. */
        DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
        /* Trigger DMA transfer every single peripheral transfer. */
        DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
        DMA_Init(spDMA_Mem2Mem[i].stream, &DMA_InitStructure);
        /* Enable DMA to start once transmission */
        // DMA_Cmd(spDMA_NULL_stream, ENABLE);
    }
        
    return true;
}


bool DMA_Start(DMA_Stream_TypeDef * stream, uint32_t mem_target, uint32_t periph_buffer, uint16_t len) {
    if(len==0) {
        return false;
    }
    /* DMA is transfering */
    if(DMA_GetCmdStatus(stream)==ENABLE){
        return false;
    }
//    DMA_Cmd(stream, DISABLE);
//    while(DMA_GetCmdStatus(stream)!=DISABLE);
    stream->PAR = (uint32_t)periph_buffer;
    stream->M0AR = (uint32_t)mem_target;
    stream->NDTR = len;
    if(stream->CR & DMA_SxCR_CIRC) {
        stream->CR &= ~DMA_SxCR_CIRC;       /* Disable circular mode. */
    }
    DMA_ClearStreamFlagBit(stream, DMA_CRx);
    DMA_Cmd(stream, ENABLE);
    return true;
}

bool DMA_Restart(DMA_Stream_TypeDef * stream, uint32_t mem_target, uint32_t periph_buffer, uint16_t len) {
    if(len==0) {
        return false;
    }
    /* DMA is transfering */
    uint16_t overtime = 2000;
    DMA_Cmd(stream, DISABLE);
    while(DMA_GetCmdStatus(stream)!=DISABLE && overtime>0) overtime--;
    if(overtime <= 0) {
        return  false;
    }
    
    stream->PAR = (uint32_t)periph_buffer;
    stream->M0AR = (uint32_t)mem_target;
    stream->NDTR = len;
    DMA_ClearStreamFlagBit(stream, DMA_CRx);
    DMA_Cmd(stream, ENABLE);
    return false;
}

DMA_Stream_TypeDef* DMA_CopyMem2Mem(uint32_t target, uint32_t buffer, uint16_t len) {
    if(len==0) {
        return false;
    }
    /* Select DMA stream */
    uint8_t i, size=sizeof(spDMA_Mem2Mem)/sizeof(*spDMA_Mem2Mem);
    for(i=0; i<size; i++) {
        if((spDMA_Mem2Mem[i].stream->CR&DMA_SxCR_CHSEL_Msk)==spDMA_Mem2Mem[i].channel && 
            DMA_GetCmdStatus(spDMA_Mem2Mem[i].stream)==DISABLE) {
                DMA_Start(spDMA_Mem2Mem[i].stream, target, buffer, len);
                return spDMA_Mem2Mem[i].stream;
            }
    }
    return NULL;
}

void DMA_ResetCounter(DMA_Stream_TypeDef * stream, uint32_t size) {
    /* Start RC DMA and stop USART_DELE IRQ */
    DMA_Cmd(stream, DISABLE);
    while(stream->CR & DMA_SxCR_EN);
    DMA_SetCurrDataCounter(stream, size);
    DMA_Cmd(stream, ENABLE);
}




const struct DMA_Controllers_Type spDMA =  {
    .controller = {
        .start = DMA_Start,
        .restart = DMA_Restart,
        .clear_stream_bit = DMA_ClearStreamFlagBit,
        .get_stream_bit = DMA_GetStreamFlagBit,
        .reset_counter = DMA_ResetCounter
    },
    .mem2mem = {
        .init = DMA_InitNull,
        .copy = DMA_CopyMem2Mem
    }
};

/************************ (C) COPYRIGHT Tongji Super Power *****END OF FILE****/
