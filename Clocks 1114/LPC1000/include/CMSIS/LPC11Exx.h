
/****************************************************************************************************//**
 * @file     LPC11Exx.h
 *
 *
 * @brief    CMSIS Cortex-M0 Core Peripheral Access Layer Header File for
 *           default LPC11Exx Device Series
 *
 * @version  V0.1
 * @date     31. January 2012
 *
 * @note     Generated with SFDGen V2.6 Build 3j (beta) on Tuesday, 31.01.2012 13:19:45
 *
 *           from CMSIS SVD File 'LPC11E1x_svd.xml' Version 0.1,
 *           created on Tuesday, 31.01.2012 20:30:42, last modified on Tuesday, 31.01.2012 20:19:40
 *
 *******************************************************************************************************/

/** @addtogroup NXP
  * @{
  */

/** @addtogroup LPC11Exx
  * @{
  */

#ifndef __LPC11EXX_H__
#define __LPC11EXX_H__

#ifdef __cplusplus
extern "C" {
#endif 


#if defined ( __CC_ARM   )
  #pragma anon_unions
#endif

 /* Interrupt Number Definition */

typedef enum {
// -------------------------  Cortex-M0 Processor Exceptions Numbers  -----------------------------
  Reset_IRQn                        = -15,  /*!<   1  Reset Vector, invoked on Power up and warm reset */
  NonMaskableInt_IRQn               = -14,  /*!<   2  Non maskable Interrupt, cannot be stopped or preempted */
  HardFault_IRQn                    = -13,  /*!<   3  Hard Fault, all classes of Fault */
  SVCall_IRQn                       = -5,   /*!<  11  System Service Call via SVC instruction */
  DebugMonitor_IRQn                 = -4,   /*!<  12  Debug Monitor                    */
  PendSV_IRQn                       = -2,   /*!<  14  Pendable request for system service */
  SysTick_IRQn                      = -1,   /*!<  15  System Tick Timer                */
// ---------------------------  LPC11Exx Specific Interrupt Numbers  ------------------------------
  FLEX_INT0_IRQn                = 0,        /*!< All I/O pins can be routed to below 8 interrupts. */
  FLEX_INT1_IRQn                = 1,
  FLEX_INT2_IRQn                = 2,
  FLEX_INT3_IRQn                = 3,
  FLEX_INT4_IRQn                = 4,   
  FLEX_INT5_IRQn                = 5,        
  FLEX_INT6_IRQn                = 6,        
  FLEX_INT7_IRQn                = 7,        
  GINT0_IRQn                    = 8,        /*!< Grouped Interrupt 0                              */
  GINT1_IRQn                    = 9,        /*!< Grouped Interrupt 1                              */
  Reserved0_IRQn                = 10,       /*!< Reserved Interrupt                               */
  Reserved1_IRQn                = 11,       
  Reserved2_IRQn                = 12,       
  Reserved3_IRQn                = 13,       
  SSP1_IRQn                     = 14,       /*!< SSP1 Interrupt                                   */
  I2C_IRQn                      = 15,       /*!< I2C Interrupt                                    */
  TIMER_16_0_IRQn               = 16,       /*!< 16-bit Timer0 Interrupt                          */
  TIMER_16_1_IRQn               = 17,       /*!< 16-bit Timer1 Interrupt                          */
  TIMER_32_0_IRQn               = 18,       /*!< 32-bit Timer0 Interrupt                          */
  TIMER_32_1_IRQn               = 19,       /*!< 32-bit Timer1 Interrupt                          */
  SSP0_IRQn                     = 20,       /*!< SSP0 Interrupt                                   */
  UART_IRQn                     = 21,       /*!< UART Interrupt                                   */
  Reserved4_IRQn                = 22,       /*!< Reserved Interrupt                               */
  Reserved5_IRQn                = 23,       /*!< Reserved Interrupt                               */
  ADC_IRQn                      = 24,       /*!< A/D Converter Interrupt                          */
  WDT_IRQn                      = 25,       /*!< Watchdog timer Interrupt                         */  
  BOD_IRQn                      = 26,       /*!< Brown Out Detect(BOD) Interrupt                  */
  FMC_IRQn                      = 27,       /*!< Flash Memory Controller Interrupt                */
  Reserved6_IRQn                = 28,       /*!< Reserved Interrupt                               */
  Reserved7_IRQn                = 29,       /*!< Reserved Interrupt                               */
  Reserved8_IRQn                = 30,       /*!< Reserved Interrupt   				       */
  Reserved9_IRQn                = 31,       /*!< Reserved Interrupt                               */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */

/* Processor and Core Peripheral Section */ /* Configuration of the Cortex-M0 Processor and Core Peripherals */

#define __MPU_PRESENT             0         /*!< MPU present or not                    */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used */
/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm0.h"                       /*!< Cortex-M0 processor and core peripherals */
#include "system_LPC11Exx.h"                /*!< LPC11Exx System                       */

/** @addtogroup Device_Peripheral_Registers
  * @{
  */


// ------------------------------------------------------------------------------------------------
// -----                                          I2C                                         -----
// ------------------------------------------------------------------------------------------------


typedef struct {                            /*!< (@ 0x40000000) I2C Structure          */
  __IO uint32_t CONSET;                     /*!< (@ 0x40000000) I2C Control Set Register */
  __I  uint32_t STAT;                       /*!< (@ 0x40000004) I2C Status Register */
  __IO uint32_t DAT;                        /*!< (@ 0x40000008) I2C Data Register.  */
  __IO uint32_t ADR0;                       /*!< (@ 0x4000000C) I2C Slave Address Register 0 */
  __IO uint32_t SCLH;                       /*!< (@ 0x40000010) SCH Duty Cycle Register High Half Word */
  __IO uint32_t SCLL;                       /*!< (@ 0x40000014) SCL Duty Cycle Register Low Half Word */
  __IO uint32_t CONCLR;                     /*!< (@ 0x40000018) I2C Control Clear Register*/
  __IO uint32_t MMCTRL;                     /*!< (@ 0x4000001C) Monitor mode control register*/
  __IO uint32_t ADR1;                       /*!< (@ 0x40000020) I2C Slave Address Register 1*/
  __IO uint32_t ADR2;                       /*!< (@ 0x40000024) I2C Slave Address Register 2*/
  __IO uint32_t ADR3;                       /*!< (@ 0x40000028) I2C Slave Address Register 3*/
  __I  uint32_t DATA_BUFFER;                /*!< (@ 0x4000002C) Data buffer register */
union{
  __IO uint32_t MASK[4];                    /*!< (@ 0x40000030) I2C Slave address mask register */
  struct{
  __IO uint32_t MASK0;
  __IO uint32_t MASK1;
  __IO uint32_t MASK2;
  __IO uint32_t MASK3;
  };
  };
} LPC_I2C_Type;


// ------------------------------------------------------------------------------------------------
// -----                                         WWDT                                         -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x40004000) WWDT Structure         */
  __IO uint32_t MOD;                        /*!< (@ 0x40004000) Watchdog mode register*/
  __IO uint32_t TC;                         /*!< (@ 0x40004004) Watchdog timer constant register */
  __IO uint32_t FEED;                       /*!< (@ 0x40004008) Watchdog feed sequence register */
  __I  uint32_t TV;                         /*!< (@ 0x4000400C) Watchdog timer value register */
  __IO uint32_t CLKSEL;                     /*!< (@ 0x40004010) Watchdog clock select register. */
  __IO uint32_t WARNINT;                    /*!< (@ 0x40004014) Watchdog Warning Interrupt compare value. */
  __IO uint32_t WINDOW;                     /*!< (@ 0x40004018) Watchdog Window compare value. */
} LPC_WWDT_Type;


// ------------------------------------------------------------------------------------------------
// -----                                         USART                                        -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x40008000) USART Structure        */
  
  union {
    __IO uint32_t DLL;                      /*!< (@ 0x40008000) Divisor Latch LSB. Least significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider. (DLAB=1) */
    __O  uint32_t THR;                      /*!< (@ 0x40008000) Transmit Holding Register. The next character to be transmitted is written here. (DLAB=0) */
    __I  uint32_t RBR;                      /*!< (@ 0x40008000) Receiver Buffer Register. Contains the next received character to be read. (DLAB=0) */
  };
  
  union {
    __IO uint32_t IER;                      /*!< (@ 0x40008004) Interrupt Enable Register. Contains individual interrupt enable bits for the 7 potential USART interrupts. (DLAB=0) */
    __IO uint32_t DLM;                      /*!< (@ 0x40008004) Divisor Latch MSB. Most significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider. (DLAB=1) */
  };
  
  union {
    __O  uint32_t FCR;                      /*!< (@ 0x40008008) FIFO Control Register. Controls USART FIFO usage and modes. */
    __I  uint32_t IIR;                      /*!< (@ 0x40008008) Interrupt ID Register. Identifies which interrupt(s) are pending. */
  };
  __IO uint32_t LCR;                        /*!< (@ 0x4000800C) Line Control Register. Contains controls for frame formatting and break generation. */
  __IO uint32_t MCR;                        /*!< (@ 0x40008010) Modem Control Register. */
  __I  uint32_t LSR;                        /*!< (@ 0x40008014) Line Status Register. Contains flags for transmit and receive status, including line errors. */
  __I  uint32_t MSR;                        /*!< (@ 0x40008018) Modem Status Register. */
  __IO uint32_t SCR;                        /*!< (@ 0x4000801C) Scratch Pad Register. Eight-bit temporary storage for software. */
  __IO uint32_t ACR;                        /*!< (@ 0x40008020) Auto-baud Control Register. Contains controls for the auto-baud feature. */
  __IO uint32_t ICR;                        /*!< (@ 0x40008024) IrDA Control Register. Enables and configures the IrDA (remote control) mode. */
  __IO uint32_t FDR;                        /*!< (@ 0x40008028) Fractional Divider Register. Generates a clock input for the baud rate divider. */
  __IO uint32_t OSR;                        /*!< (@ 0x4000802C) Oversampling Register. Controls the degree of oversampling during each bit time. */
  __IO uint32_t TER;                        /*!< (@ 0x40008030) Transmit Enable Register. Turns off USART transmitter for use with software flow control. */
  __I  uint32_t RESERVED0[3];
  __IO uint32_t HDEN;                       /*!< (@ 0x40008040) Half duplex enable register. */
  __I  uint32_t RESERVED1;
  __IO uint32_t SCICTRL;                    /*!< (@ 0x40008048) Smart Card Interface Control register. Enables and configures the Smart Card Interface feature. */
  __IO uint32_t RS485CTRL;                  /*!< (@ 0x4000804C) RS-485/EIA-485 Control. Contains controls to configure various aspects of RS-485/EIA-485 modes. */
  __IO uint32_t RS485ADRMATCH;              /*!< (@ 0x40008050) RS-485/EIA-485 address match. Contains the address match value for RS-485/EIA-485 mode. */
  __IO uint32_t RS485DLY;                   /*!< (@ 0x40008054) RS-485/EIA-485 direction control delay. */
  __IO uint32_t SYNCCTRL; 
} LPC_USART_Type;


// ------------------------------------------------------------------------------------------------
// -----                                        Timer                                       -----
// ------------------------------------------------------------------------------------------------






/* *************************Timer 0 Base Type ***************************************** */ 
typedef struct {                            /*!< (@ 0x4000C000) CT16B0 Structure        */
  __IO uint32_t IR;                         /*!< (@ 0x4000C000) Interrupt Register      */
  __IO uint32_t TCR;                        /*!< (@ 0x4000C004) Timer Control Register  */
  __IO uint32_t TC;                         /*!< (@ 0x4000C008) Timer Counter 		*/
  __IO uint32_t PR;                         /*!< (@ 0x4000C010) Prescale Register  	*/
  __IO uint32_t PC;                         /*!< (@ 0x4000C014) Prescale Counter	 */
  __IO uint32_t MCR;                        /*!< (@ 0x4000C014) Match Control Register */
  union {
  __IO uint32_t MR[4];                      /*!< (@ 0x4000C018) Match Register */
  struct{
  __IO uint32_t MR0;                        /*!< (@ 0x4000C018) Match Register. MR0 */
  __IO uint32_t MR1;                        /*!< (@ 0x4000C01C) Match Register. MR1 */
  __IO uint32_t MR2;                        /*!< (@ 0x4000C020) Match Register. MR2 */
  __IO uint32_t MR3;                        /*!< (@ 0x4000C024) Match Register. MR3 */
  };
  };
  __IO uint32_t CCR;                        /*!< (@ 0x4000C028) Capture Control Register */
  union{
  __I  uint32_t CR[4];                      /*!< (@ 0x4000C02C) Capture Register  */
    struct{
  __I  uint32_t CR0;			     							/*!< (@ 0x4000C02C) Capture Register. CR 0 */
  __I  uint32_t RESERVED1;		   							/*!< (@ 0x4000C030) Reserved */ 
	__I  uint32_t CR1;			   		  					/*!< (@ 0x4000C034) Capture Register. CR 1 */
  __I  uint32_t RESERVED2;		  							/*!< (@ 0x4000C038) Reserved */
	  };
  };
__IO uint32_t EMR;                        /*!< (@ 0x4000C03C) External Match Register */
  __I  uint32_t RESERVED0[12];
  __IO uint32_t CTCR;                       /*!< (@ 0x4000C070) Count Control Register */
  __IO uint32_t PWMC;                       /*!< (@ 0x4000C074) PWM Control Register */
} LPC_CTxxB0_Type;


/* *************************Timer 1 Base Type ***************************************** */ 
typedef struct {                            /*!< (@ 0x40010000) CT16B1 Structure        */
  __IO uint32_t IR;                         /*!< (@ 0x40010000) Interrupt Register      */
  __IO uint32_t TCR;                        /*!< (@ 0x40010004) Timer Control Register  */
  __IO uint32_t TC;                         /*!< (@ 0x40010008) Timer Counter 		*/
  __IO uint32_t PR;                         /*!< (@ 0x4001000C) Prescale Register  	*/
  __IO uint32_t PC;                         /*!< (@ 0x40010010) Prescale Counter	 */
  __IO uint32_t MCR;                        /*!< (@ 0x40010014) Match Control Register */
  union {
  __IO uint32_t MR[4];                      /*!< (@ 0x40010018) Match Register */
  struct{
  __IO uint32_t MR0;                        /*!< (@ 0x40010018) Match Register. MR0 */
  __IO uint32_t MR1;                        /*!< (@ 0x4001001C) Match Register. MR1 */
  __IO uint32_t MR2;                        /*!< (@ 0x40010020) Match Register. MR2 */
  __IO uint32_t MR3;                        /*!< (@ 0x40010024) Match Register. MR3 */
  };
  };
  __IO uint32_t CCR;                        /*!< (@ 0x40010028) Capture Control Register */
  union{
  __I  uint32_t CR[4];                      /*!< (@ 0x4001002C) Capture Register  */
    struct{
  __I  uint32_t CR0;			   								/*!< (@ 0x4001002C) Capture Register. CR 0 */
  __I  uint32_t CR1;			    							/*!< (@ 0x40010030) Capture Register. CR 1 */
  __I  uint32_t RESERVED1;			    					/*!< (@ 0x40010034) Capture Register. CR 2 */
  __I  uint32_t RESERVED2;			    					/*!< (@ 0x40010038) Capture Register. CR 3 */
  };
  };
__IO uint32_t EMR;                        	/*!< (@ 0x4001003C) External Match Register */
  __I  uint32_t RESERVED0[12];
  __IO uint32_t CTCR;                       /*!< (@ 0x40010070) Count Control Register */
  __IO uint32_t PWMC;                       /*!< (@ 0x40010074) PWM Control Register */
} LPC_CTxxB1_Type;


// ------------------------------------------------------------------------------------------------
// -----                                          ADC                                         -----
// ------------------------------------------------------------------------------------------------
typedef struct {                             /*!< (@ 0x4001C000) ADC Structure          */
  __IO uint32_t CR;                         /*!< (@ 0x4001C000) A/D Control Register */
  __IO uint32_t GDR;                        /*!< (@ 0x4001C004) A/D Global Data Register */
  __I  uint32_t RESERVED0[1];
  __IO uint32_t INTEN;                      /*!< (@ 0x4001C00C) A/D Interrupt Enable Register */
  union{
  __I  uint32_t DR[8];                      /*!< (@ 0x4001C010) A/D Channel Data Register*/
    struct{
  __IO uint32_t DR0;                      	/*!< (@ 0x40020010) A/D Channel Data Register 0*/
  __IO uint32_t DR1;                      	/*!< (@ 0x40020014) A/D Channel Data Register 1*/
  __IO uint32_t DR2;                      	/*!< (@ 0x40020018) A/D Channel Data Register 2*/
  __IO uint32_t DR3;                      	/*!< (@ 0x4002001C) A/D Channel Data Register 3*/
  __IO uint32_t DR4;                      	/*!< (@ 0x40020020) A/D Channel Data Register 4*/
  __IO uint32_t DR5;                      	/*!< (@ 0x40020024) A/D Channel Data Register 5*/
  __IO uint32_t DR6;                      	/*!< (@ 0x40020028) A/D Channel Data Register 6*/
  __IO uint32_t DR7;                      	/*!< (@ 0x4002002C) A/D Channel Data Register 7*/
  };
  };
  __I  uint32_t STAT;                       /*!< (@ 0x4001C030) A/D Status Register.  */
} LPC_ADC_Type;


// ------------------------------------------------------------------------------------------------
// -----                                          PMU                                         -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x40038000) PMU Structure          */
  __IO uint32_t PCON;                       /*!< (@ 0x40038000) Power control register */
  union{
  __IO uint32_t GPREG[4];                   /*!< (@ 0x40038004) General purpose register 0 */
  struct{
  __IO uint32_t GPREG0;                   	/*!< (@ 0x40038004) General purpose register 0 */
  __IO uint32_t GPREG1;                   	/*!< (@ 0x40038008) General purpose register 1 */
  __IO uint32_t GPREG2;                   	/*!< (@ 0x4003800C) General purpose register 2 */
  __IO uint32_t GPREG3;                   	/*!< (@ 0x40038010) General purpose register 3 */
  };
  };
} LPC_PMU_Type;


// ------------------------------------------------------------------------------------------------
// -----                                       FLASHCTRL                                      -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x4003C000) FLASHCTRL Structure    */
  __I  uint32_t RESERVED0[4];
  __IO uint32_t FLASHCFG;                   /*!< (@ 0x4003C010) Flash memory access time configuration register */
  __I  uint32_t RESERVED1[3];
  __IO uint32_t FMSSTART;                   /*!< (@ 0x4003C020) Signature start address register */
  __IO uint32_t FMSSTOP;                    /*!< (@ 0x4003C024) Signature stop-address register */
  __I  uint32_t RESERVED2[1];
  __I  uint32_t FMSW0;                      /*!< (@ 0x4003C02C) Word 0 [31:0]          */
  __I  uint32_t FMSW1;                      /*!< (@ 0x4003C030) Word 1 [63:32]         */
  __I  uint32_t FMSW2;                      /*!< (@ 0x4003C034) Word 2 [95:64]         */
  __I  uint32_t FMSW3;                      /*!< (@ 0x4003C038) Word 3 [127:96]        */
  __I  uint32_t RESERVED3[1001];
  __I  uint32_t FMSTAT;                     /*!< (@ 0x4003CFE0) Signature generation status register */
  __I  uint32_t RESERVED4[1];
  __IO uint32_t FMSTATCLR;                  /*!< (@ 0x4003CFE8) Signature generation status clear register */
} LPC_FLASHCTRL_Type;


// ------------------------------------------------------------------------------------------------
// -----                                         SSP0/1                                         -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x40040000) SSP0 Structure         */
  __IO uint32_t CR0;                        /*!< (@ 0x40040000) Control Register 0. Selects the serial clock rate, bus type, and data size. */
  __IO uint32_t CR1;                        /*!< (@ 0x40040004) Control Register 1. Selects master/slave and other modes. */
  __IO uint32_t DR;                         /*!< (@ 0x40040008) Data Register. Writes fill the transmit FIFO, and reads empty the receive FIFO. */
  __I  uint32_t SR;                         /*!< (@ 0x4004000C) Status Register        */
  __IO uint32_t CPSR;                       /*!< (@ 0x40040010) Clock Prescale Register */
  __IO uint32_t IMSC;                       /*!< (@ 0x40040014) Interrupt Mask Set and Clear Register */
  __I  uint32_t RIS;                        /*!< (@ 0x40040018) Raw Interrupt Status Register */
  __I  uint32_t MIS;                        /*!< (@ 0x4004001C) Masked Interrupt Status Register */
  __IO uint32_t ICR;                        /*!< (@ 0x40040020) SSPICR Interrupt Clear Register */
} LPC_SSPx_Type;



// ------------------------------------------------------------------------------------------------
// -----                                       IOCONFIG                                       -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x40044000) IOCONFIG Structure     */
  __IO uint32_t RESET_PIO0_0;               /*!< (@ 0x40044000) I/O configuration for pin RESET/PIO0_0 */
  __IO uint32_t PIO0_1;                     /*!< (@ 0x40044004) I/O configuration for pin PIO0_1/CLKOUT/CT32B0_MAT2 */
  __IO uint32_t PIO0_2;                     /*!< (@ 0x40044008) I/O configuration for pin PIO0_2/SSEL0/CT16B0_CAP0 */
  __IO uint32_t PIO0_3;                     /*!< (@ 0x4004400C) I/O configuration for pin PIO0_3 */
  __IO uint32_t PIO0_4;                     /*!< (@ 0x40044010) I/O configuration for pin PIO0_4/SCL */
  __IO uint32_t PIO0_5;                     /*!< (@ 0x40044014) I/O configuration for pin PIO0_5/SDA */
  __IO uint32_t PIO0_6;                     /*!< (@ 0x40044018) I/O configuration for pin PIO0_6/SCK0 */
  __IO uint32_t PIO0_7;                     /*!< (@ 0x4004401C) I/O configuration for pin PIO0_7/CTS */
  __IO uint32_t PIO0_8;                     /*!< (@ 0x40044020) I/O configuration for pin PIO0_8/MISO0/CT16B0_MAT0 */
  __IO uint32_t PIO0_9;                     /*!< (@ 0x40044024) I/O configuration for pin PIO0_9/MOSI0/CT16B0_MAT1 */
  __IO uint32_t SWCLK_PIO0_10;              /*!< (@ 0x40044028) I/O configuration for pin SWCLK/PIO0_10/ SCK0/CT16B0_MAT2 */
  __IO uint32_t TDI_PIO0_11;                /*!< (@ 0x4004402C) I/O configuration for pin TDI/PIO0_11/AD0/CT32B0_MAT3 */
  __IO uint32_t TMS_PIO0_12;                /*!< (@ 0x40044030) I/O configuration for pin TMS/PIO0_12/AD1/CT32B1_CAP0 */
  __IO uint32_t TDO_PIO0_13;                /*!< (@ 0x40044034) I/O configuration for pin TDO/PIO0_13/AD2/CT32B1_MAT0 */
  __IO uint32_t TRST_PIO0_14;               /*!< (@ 0x40044038) I/O configuration for pin TRST/PIO0_14/AD3/CT32B1_MAT1 */
  __IO uint32_t SWDIO_PIO0_15;              /*!< (@ 0x4004403C) I/O configuration for pin SWDIO/PIO0_15/AD4/CT32B1_MAT2 */
  __IO uint32_t PIO0_16;                    /*!< (@ 0x40044040) I/O configuration for pin PIO0_16/AD5/CT32B1_MAT3/ WAKEUP */
  __IO uint32_t PIO0_17;                    /*!< (@ 0x40044044) I/O configuration for pin PIO0_17/RTS/CT32B0_CAP0/SCLK */
  __IO uint32_t PIO0_18;                    /*!< (@ 0x40044048) I/O configuration for pin PIO0_18/RXD/CT32B0_MAT0 */
  __IO uint32_t PIO0_19;                    /*!< (@ 0x4004404C) I/O configuration for pin PIO0_19/TXD/CT32B0_MAT1 */
  __IO uint32_t PIO0_20;                    /*!< (@ 0x40044050) I/O configuration for pin PIO0_20/CT16B1_CAP0 */
  __IO uint32_t PIO0_21;                    /*!< (@ 0x40044054) I/O configuration for pin PIO0_21/CT16B1_MAT0/MOSI1 */
  __IO uint32_t PIO0_22;                    /*!< (@ 0x40044058) I/O configuration for pin PIO0_22/AD6/CT16B1_MAT1/MISO1 */
  __IO uint32_t PIO0_23;                    /*!< (@ 0x4004405C) I/O configuration for pin PIO0_23/AD7 */
  __IO uint32_t PIO1_0;                 /*!< Offset: 0x060 */
  __IO uint32_t PIO1_1;         
  __IO uint32_t PIO1_2;       
  __IO uint32_t PIO1_3;      
  __IO uint32_t PIO1_4;                 /*!< Offset: 0x070 */
  __IO uint32_t PIO1_5;                     /*!< (@ 0x40044074) I/O configuration for pin PIO1_5/CT32B1_CAP1 */
  __IO uint32_t PIO1_6;     
  __IO uint32_t PIO1_7;       
  __IO uint32_t PIO1_8;                 /*!< Offset: 0x080 */
  __IO uint32_t PIO1_9;        
  __IO uint32_t PIO1_10;        
  __IO uint32_t PIO1_11;       
  __IO uint32_t PIO1_12;                /*!< Offset: 0x090 */
  __IO uint32_t PIO1_13;                    /*!< (@ 0x40044094) I/O configuration for pin PIO1_13/DTR/CT16B0_MAT0/TXD */
  __IO uint32_t PIO1_14;                    /*!< (@ 0x40044098) I/O configuration for pin PIO1_14/DSR/CT16B0_MAT1/RXD */
  __IO uint32_t PIO1_15;                    /*!< (@ 0x4004409C) I/O configuration for pin PIO1_15/DCD/ CT16B0_MAT2/SCK1 */
  __IO uint32_t PIO1_16;                    /*!< (@ 0x400440A0) I/O configuration for pin PIO1_16/RI/CT16B0_CAP0 */
  __IO uint32_t PIO1_17;
  __IO uint32_t PIO1_18;
  __IO uint32_t PIO1_19;                    /*!< (@ 0x400440AC) I/O configuration for pin PIO1_19/DTR/SSEL1 */
  __IO uint32_t PIO1_20;                    /*!< (@ 0x400440B0) I/O configuration for pin PIO1_20/DSR/SCK1 */
  __IO uint32_t PIO1_21;                    /*!< (@ 0x400440B4) I/O configuration for pin PIO1_21/DCD/MISO1 */
  __IO uint32_t PIO1_22;                    /*!< (@ 0x400440B8) I/O configuration for pin PIO1_22/RI/MOSI1 */
  __IO uint32_t PIO1_23;                    /*!< (@ 0x400440BC) I/O configuration for pin PIO1_23/CT16B1_MAT1/SSEL1 */
  __IO uint32_t PIO1_24;                    /*!< (@ 0x400440C0) I/O configuration for pin PIO1_24/ CT32B0_MAT0 */
  __IO uint32_t PIO1_25;                    /*!< (@ 0x400440C4) I/O configuration for pin PIO1_25/CT32B0_MAT1 */
  __IO uint32_t PIO1_26;                    /*!< (@ 0x400440C8) I/O configuration for pin PIO1_26/CT32B0_MAT2/ RXD */
  __IO uint32_t PIO1_27;                    /*!< (@ 0x400440CC) I/O configuration for pin PIO1_27/CT32B0_MAT3/ TXD */
  __IO uint32_t PIO1_28;                    /*!< (@ 0x400440D0) I/O configuration for pin PIO1_28/CT32B0_CAP0/ SCLK */
  __IO uint32_t PIO1_29;                    /*!< (@ 0x400440D4) I/O configuration for pin PIO1_29/SCK0/ CT32B0_CAP1 */
  __IO uint32_t PIO1_30;
  __IO uint32_t PIO1_31;                    /*!< (@ 0x400440DC) I/O configuration for pin PIO1_31 */
} LPC_IOCON_Type;


// ------------------------------------------------------------------------------------------------
// -----                                        SYSCON                                        -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x40048000) SYSCON Structure       */
  __IO uint32_t SYSMEMREMAP;                /*!< (@ 0x40048000) System memory remap    */
  __IO uint32_t PRESETCTRL;                 /*!< (@ 0x40048004) Peripheral reset control */
  __IO uint32_t SYSPLLCTRL;                 /*!< (@ 0x40048008) System PLL control     */
  __I  uint32_t SYSPLLSTAT;                 /*!< (@ 0x4004800C) System PLL status      */
  __I  uint32_t RESERVED0[4];
  __IO uint32_t SYSOSCCTRL;                 /*!< (@ 0x40048020) System oscillator control */
  __IO uint32_t WDTOSCCTRL;                 /*!< (@ 0x40048024) Watchdog oscillator control */
  __I  uint32_t RESERVED1[2];
  __IO uint32_t SYSRSTSTAT;                 /*!< (@ 0x40048030) System reset status register */
  __I  uint32_t RESERVED2[3];
  __IO uint32_t SYSPLLCLKSEL;               /*!< (@ 0x40048040) System PLL clock source select */
  __IO uint32_t SYSPLLCLKUEN;               /*!< (@ 0x40048044) System PLL clock source update enable */
  __I  uint32_t RESERVED3[10];
  __IO uint32_t MAINCLKSEL;                 /*!< (@ 0x40048070) Main clock source select */
  __IO uint32_t MAINCLKUEN;                 /*!< (@ 0x40048074) Main clock source update enable */
  __IO uint32_t SYSAHBCLKDIV;               /*!< (@ 0x40048078) System clock divider   */
  __I  uint32_t RESERVED4[1];
  __IO uint32_t SYSAHBCLKCTRL;              /*!< (@ 0x40048080) System clock control   */
  __I  uint32_t RESERVED5[4];
  __IO uint32_t SSP0CLKDIV;                 /*!< (@ 0x40048094) SSP0 clock divider     */
  __IO uint32_t UARTCLKDIV;                 /*!< (@ 0x40048098) UART clock divider     */
  __IO uint32_t SSP1CLKDIV;                 /*!< (@ 0x4004809C) SSP1 clock divider     */
  __I  uint32_t RESERVED6[16];
  __IO uint32_t CLKOUTSEL;                  /*!< (@ 0x400480E0) CLKOUT clock source select */
  __IO uint32_t CLKOUTUEN;                  /*!< (@ 0x400480E4) CLKOUT clock source update enable */
  __IO uint32_t CLKOUTDIV;                  /*!< (@ 0x400480E8) CLKOUT clock divider   */
  __I  uint32_t RESERVED7[5];
  __I  uint32_t PIOPORCAP0;                 /*!< (@ 0x40048100) POR captured PIO status 0 */
  __I  uint32_t PIOPORCAP1;                 /*!< (@ 0x40048104) POR captured PIO status 1 */
  __I  uint32_t RESERVED8[18];
  __IO uint32_t BODCTRL;                    /*!< (@ 0x40048150) Brown-Out Detect       */
  __IO uint32_t SYSTCKCAL;                  /*!< (@ 0x40048154) System tick counter calibration */
  __I  uint32_t RESERVED9[6];
  __IO uint32_t IRQLATENCY;                 /*!< (@ 0x40048170) IQR delay */
  __IO uint32_t NMISRC;                     /*!< (@ 0x40048174) NMI Source Control     */
  __IO uint32_t PINTSEL[8];                 /*!< (@ 0x40048178) GPIO Pin Interrupt Select register 0 */
  __I  uint32_t RESERVED10[27];
  __IO uint32_t STARTERP0;                  /*!< (@ 0x40048204) Start logic 0 interrupt wake-up enable register 0 */
  __I  uint32_t RESERVED11[3];
  __IO uint32_t STARTERP1;                  /*!< (@ 0x40048214) Start logic 1 interrupt wake-up enable register 1 */
  __I  uint32_t RESERVED12[6];
  __IO uint32_t PDSLEEPCFG;                 /*!< (@ 0x40048230) Power-down states in deep-sleep mode */
  __IO uint32_t PDAWAKECFG;                 /*!< (@ 0x40048234) Power-down states for wake-up from deep-sleep */
  __IO uint32_t PDRUNCFG;                   /*!< (@ 0x40048238) Power configuration register */
  __I  uint32_t RESERVED13[110];
  __I  uint32_t DEVICE_ID;                  /*!< (@ 0x400483F4) Device ID              */
} LPC_SYSCON_Type;


// ------------------------------------------------------------------------------------------------
// -----                                     GPIO_PIN_INT                                     -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x4004C000) GPIO_PIN_INT Structure */
  __IO uint32_t ISEL;                       /*!< (@ 0x4004C000) Pin Interrupt Mode register */
  __IO uint32_t IENR;                       /*!< (@ 0x4004C004) Pin Interrupt Enable (Rising) register */
  __IO uint32_t SIENR;                      /*!< (@ 0x4004C008) Set Pin Interrupt Enable (Rising) register */
  __IO uint32_t CIENR;                      /*!< (@ 0x4004C00C) Clear Pin Interrupt Enable (Rising) register */
  __IO uint32_t IENF;                       /*!< (@ 0x4004C010) Pin Interrupt Enable Falling Edge / Active Level register */
  __IO uint32_t SIENF;                      /*!< (@ 0x4004C014) Set Pin Interrupt Enable Falling Edge / Active Level register */
  __IO uint32_t CIENF;                      /*!< (@ 0x4004C018) Clear Pin Interrupt Enable Falling Edge / Active Level address */
  __IO uint32_t RISE;                       /*!< (@ 0x4004C01C) Pin Interrupt Rising Edge register */
  __IO uint32_t FALL;                       /*!< (@ 0x4004C020) Pin Interrupt Falling Edge register */
  __IO uint32_t IST;                        /*!< (@ 0x4004C024) Pin Interrupt Status register */
} LPC_GPIO_PIN_INT_Type;


// ------------------------------------------------------------------------------------------------
// -----                                    GPIO_GROUP_INT0/1                                   -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            /*!< (@ 0x4005C000) GPIO_GROUP_INT0 Structure */
  __IO uint32_t CTRL;                       /*!< (@ 0x4005C000) GPIO grouped interrupt control register */
  __I  uint32_t RESERVED0[7];
  __IO uint32_t PORT_POL[2];                /*!< (@ 0x4005C020) GPIO grouped interrupt port 0 polarity register */
  __I  uint32_t RESERVED1[6];
  __IO uint32_t PORT_ENA[2];                /*!< (@ 0x4005C040) GPIO grouped interrupt port 0/1 enable register */
} LPC_GPIO_GROUP_INTx_Type;

// ------------------------------------------------------------------------------------------------
// -----                                       GPIO_PORT                                      -----
// ------------------------------------------------------------------------------------------------

typedef struct {                            
  union {
    struct {
      __IO uint8_t B0[32];                       /*!< (@ 0x50000000) Byte pin registers port 0; pins PIO0_0 to PIO0_31 */
      __IO uint8_t B1[32];                       /*!< (@ 0x50000020) Byte pin registers port 1 */
    };
    __IO uint8_t B[64];                       /*!< (@ 0x50000000) Byte pin registers port 0/1 */
  };
  __I  uint32_t RESERVED0[1008];
  union {
    struct {
      __IO uint32_t W0[32];                      /*!< (@ 0x50001000) Word pin registers port 0 */
      __IO uint32_t W1[32];                      /*!< (@ 0x50001080) Word pin registers port 1 */
    };
    __IO uint32_t W[64];                       /*!< (@ 0x50001000) Word pin registers port 0/1 */
  };
       uint32_t RESERVED1[960];
  __IO uint32_t DIR[2];			/* 0x2000 */
       uint32_t RESERVED2[30];
  __IO uint32_t MASK[2];		/* 0x2080 */
       uint32_t RESERVED3[30];
  __IO uint32_t PIN[2];			/* 0x2100 */
       uint32_t RESERVED4[30];
  __IO uint32_t MPIN[2];		/* 0x2180 */
       uint32_t RESERVED5[30];
  __IO uint32_t SET[2];			/* 0x2200 */
       uint32_t RESERVED6[30];
  __O  uint32_t CLR[2];			/* 0x2280 */
       uint32_t RESERVED7[30];
  __O  uint32_t NOT[2];			/* 0x2300 */
} LPC_GPIO_Type;


#if defined ( __CC_ARM   )
  #pragma no_anon_unions
#endif

/*	The value at this address is the entry to ROM Division API.
 	Once it is dereferenced, individual API functions can be used*/
#define LPC_1XXX_ROM_LOC          (0x1FFF1FF8)


// ------------------------------------------------------------------------------------------------
// -----                                 Peripheral memory map                                -----
// ------------------------------------------------------------------------------------------------

#define LPC_I2C_BASE              (0x40000000)
#define LPC_WWDT_BASE             (0x40004000)
#define LPC_USART_BASE            (0x40008000)
#define LPC_CT16B0_BASE           (0x4000C000)
#define LPC_CT16B1_BASE           (0x40010000)
#define LPC_CT32B0_BASE           (0x40014000)
#define LPC_CT32B1_BASE           (0x40018000)
#define LPC_ADC_BASE              (0x4001C000)
#define LPC_PMU_BASE              (0x40038000)
#define LPC_FLASHCTRL_BASE        (0x4003C000)
#define LPC_SSP0_BASE             (0x40040000)
#define LPC_SSP1_BASE             (0x40058000)
#define LPC_IOCON_BASE            (0x40044000)
#define LPC_SYSCON_BASE           (0x40048000)
#define LPC_GPIO_PIN_INT_BASE     (0x4004C000)
#define LPC_GPIO_GROUP_INT0_BASE  (0x4005C000)
#define LPC_GPIO_GROUP_INT1_BASE  (0x40060000)
#define LPC_GPIO_BASE             (0x50000000)


// ------------------------------------------------------------------------------------------------
// -----                                Peripheral declaration                                -----
// ------------------------------------------------------------------------------------------------

#define LPC_I2C                   ((LPC_I2C_Type            *) LPC_I2C_BASE)
#define LPC_WWDT                  ((LPC_WWDT_Type           *) LPC_WWDT_BASE)
#define LPC_USART                 ((LPC_USART_Type          *) LPC_USART_BASE)
#define LPC_CT16B0                ((LPC_CTxxB0_Type         *) LPC_CT16B0_BASE)
#define LPC_CT16B1                ((LPC_CTxxB1_Type         *) LPC_CT16B1_BASE)
#define LPC_CT32B0                ((LPC_CTxxB0_Type         *) LPC_CT32B0_BASE)
#define LPC_CT32B1                ((LPC_CTxxB1_Type         *) LPC_CT32B1_BASE)
#define LPC_ADC                   ((LPC_ADC_Type            *) LPC_ADC_BASE)
#define LPC_PMU                   ((LPC_PMU_Type            *) LPC_PMU_BASE)
#define LPC_FLASHCTRL             ((LPC_FLASHCTRL_Type      *) LPC_FLASHCTRL_BASE)
#define LPC_SSP0                  ((LPC_SSPx_Type           *) LPC_SSP0_BASE)
#define LPC_SSP1                  ((LPC_SSPx_Type           *) LPC_SSP1_BASE)
#define LPC_IOCON                 ((LPC_IOCON_Type          *) LPC_IOCON_BASE)
#define LPC_SYSCON                ((LPC_SYSCON_Type         *) LPC_SYSCON_BASE)
#define LPC_GPIO_PIN_INT          ((LPC_GPIO_PIN_INT_Type   *) LPC_GPIO_PIN_INT_BASE)
#define LPC_GPIO_GROUP_INT0       ((LPC_GPIO_GROUP_INTx_Type*) LPC_GPIO_GROUP_INT0_BASE)
#define LPC_GPIO_GROUP_INT1       ((LPC_GPIO_GROUP_INTx_Type*) LPC_GPIO_GROUP_INT1_BASE)
#define LPC_GPIO                  ((LPC_GPIO_Type           *) LPC_GPIO_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group (null) */
/** @} */ /* End of group LPC11Exx */

#ifdef __cplusplus
}
#endif 


#endif  // __LPC11EXX_H__
