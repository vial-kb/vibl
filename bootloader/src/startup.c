extern void *_estack;

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
int Default_Handler() {
    while (1) {}
}

int Reset_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int NMI_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int HardFault_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int MemManage_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int BusFault_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int UsageFault_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int SVC_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int DebugMon_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int PendSV_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int SysTick_Handler();
__attribute__ ((weak, alias ("Default_Handler"))) int WWDG_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int PVD_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int TAMPER_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int RTC_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int FLASH_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int RCC_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int EXTI0_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int EXTI1_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int EXTI2_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int EXTI3_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int EXTI4_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int DMA1_Channel1_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int DMA1_Channel2_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int DMA1_Channel3_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int DMA1_Channel4_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int DMA1_Channel5_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int DMA1_Channel6_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int DMA1_Channel7_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int ADC1_2_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int USB_HP_CAN1_TX_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int USB_LP_CAN1_RX0_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int CAN1_RX1_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int CAN1_SCE_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int EXTI9_5_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int TIM1_BRK_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int TIM1_UP_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int TIM1_TRG_COM_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int TIM1_CC_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int TIM2_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int TIM3_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int TIM4_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int I2C1_EV_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int I2C1_ER_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int I2C2_EV_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int I2C2_ER_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int SPI1_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int SPI2_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int USART1_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int USART2_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int USART3_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int EXTI15_10_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int RTCAlarm_IRQHandler();
__attribute__ ((weak, alias ("Default_Handler"))) int USBWakeUp_IRQHandler();

/******************************************************************************
*
* The minimal vector table for a Cortex M3.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
static void* g_pfnVectors[] __attribute__((used, section (".isr_vector"))) = {
	(void*)0x20002800,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN1_TX_IRQHandler,
	USB_LP_CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler,
	TIM1_UP_IRQHandler,
	TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	USBWakeUp_IRQHandler,
	(void*)0xF108F85F          /* @0x108. This is for boot in RAM mode for
				STM32F10x Medium Density devices. */
};
