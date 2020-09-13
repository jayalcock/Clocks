
/****************************************************************************************************//**
 * @file     LPC11U6x.h
 *
 * @brief    CMSIS Cortex-M0 Peripheral Access Layer Header File for
 *           LPC11U6x from <unknown Vendor>.
 *
 * @version  V0.1
 * @date     14. May 2013
 *
 * @note     Generated with SVDConv V2.77e 
 *           from CMSIS SVD File 'Kylin_v0.1.svd' Version 0.1,
 *******************************************************************************************************/



/** @addtogroup (null)
  * @{
  */

/** @addtogroup LPC11U6x
  * @{
  */

#ifndef __LPC11U6X_H__
#define __LPC11U6X_H__

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
/* -------------------  Cortex-M0 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn                    = -15,              /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn           = -14,              /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                = -13,              /*!<   3  Hard Fault, all classes of Fault                                 */
  SVCall_IRQn                   =  -5,              /*!<  11  System Service Call via SVC instruction                          */
  DebugMonitor_IRQn             =  -4,              /*!<  12  Debug Monitor                                                    */
  PendSV_IRQn                   =  -2,              /*!<  14  Pendable request for system service                              */
  SysTick_IRQn                  =  -1,              /*!<  15  System Tick Timer                                                */
/* ----------------------  LPC11U6x Specific Interrupt Numbers  ---------------------- */
  PIN_INT0_IRQn                 =   0,              /*!<   0  PIN_INT0                                                         */
  PIN_INT1_IRQn                 =   1,              /*!<   1  PIN_INT1                                                         */
  PIN_INT2_IRQn                 =   2,              /*!<   2  PIN_INT2                                                         */
  PIN_INT3_IRQn                 =   3,              /*!<   3  PIN_INT3                                                         */
  PIN_INT4_IRQn                 =   4,              /*!<   4  PIN_INT4                                                         */
  PIN_INT5_IRQn                 =   5,              /*!<   5  PIN_INT5                                                         */
  PIN_INT6_IRQn                 =   6,              /*!<   6  PIN_INT6                                                         */
  PIN_INT7_IRQn                 =   7,              /*!<   7  PIN_INT7                                                         */
  GINT0_IRQn                    =   8,              /*!<   8  GINT0                                                            */
  GINT1_IRQn                    =   9,              /*!<   9  GINT1                                                            */
  I2C1_IRQn                     =  10,              /*!<  10  I2C1                                                             */
  USART1_4_IRQn                 =  11,              /*!<  11  USART1_4                                                         */
  USART2_3_IRQn                 =  12,              /*!<  12  USART2_3                                                         */
  SCT0_1_IRQn                   =  13,              /*!<  13  SCT0_1                                                           */
  SSP1_IRQn                     =  14,              /*!<  14  SSP1                                                             */
  I2C0_IRQn                     =  15,              /*!<  15  I2C0                                                             */
  CT16B0_IRQn                   =  16,              /*!<  16  CT16B0                                                           */
  CT16B1_IRQn                   =  17,              /*!<  17  CT16B1                                                           */
  CT32B0_IRQn                   =  18,              /*!<  18  CT32B0                                                           */
  CT32B1_IRQn                   =  19,              /*!<  19  CT32B1                                                           */
  SSP0_IRQn                     =  20,              /*!<  20  SSP0                                                             */
  USART_IRQn                    =  21,              /*!<  21  USART                                                            */
  USB_IRQ_IRQn                  =  22,              /*!<  22  USB_IRQ                                                          */
  USB_FIQ_IRQn                  =  23,              /*!<  23  USB_FIQ                                                          */
  ADC_A_IRQn                    =  24,              /*!<  24  ADC_A                                                            */
  RTC_IRQn                      =  25,              /*!<  25  RTC                                                              */
  BOD_WDT_IRQn                  =  26,              /*!<  26  BOD_WDT                                                          */
  FLASH_IRQ_IRQn                =  27,              /*!<  27  FLASH_IRQ                                                        */
  DMA_IRQn                      =  28,              /*!<  28  DMA                                                              */
  ADC_B_IRQn                    =  29,              /*!<  29  ADC_B                                                            */
  USBWAKEUP_IRQn                =  30               /*!<  30  USBWAKEUP                                                        */
} IRQn_Type;


/** @addtogroup Configuration_of_CMSIS
  * @{
  */


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the cm0 Processor and Core Peripherals---------------- */
#define __CM0_REV                 0x0000            /*!< Cortex-M0 Core Revision                                               */
#define __MPU_PRESENT                  0            /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS               2            /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig         0            /*!< Set to 1 if different SysTick Config is used                          */
/** @} */ /* End of group Configuration_of_CMSIS */

#include <core_cm0.h>                               /*!< Cortex-M0 processor and core peripherals                              */
#include "system_LPC11U6x.h"                        /*!< LPC11U6x System                                                       */


/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */


/** @addtogroup Device_Peripheral_Registers
  * @{
  */


/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning 586
#else
  #warning Not supported compiler type
#endif



/* ================================================================================ */
/* ================                      I2C0                      ================ */
/* ================================================================================ */


/**
  * @brief I2C-bus controller (I2C0)
  */

typedef struct {                                    /*!< (@ 0x40000000) I2C0 Structure                                         */
  __IO uint32_t  CONSET;                            /*!< (@ 0x40000000) I2C Control Set Register. When a one is written
                                                         to a bit of this register, the corresponding bit in the I2C
                                                          control register is set. Writing a zero has no effect on the
                                                          corresponding bit in the I2C control register.                       */
  __I  uint32_t  STAT;                              /*!< (@ 0x40000004) I2C Status Register. During I2C operation, this
                                                         register provides detailed status codes that allow software
                                                          to determine the next action needed.                                 */
  __IO uint32_t  DAT;                               /*!< (@ 0x40000008) I2C Data Register. During master or slave transmit
                                                         mode, data to be transmitted is written to this register. During
                                                          master or slave receive mode, data that has been received may
                                                          be read from this register.                                          */
  __IO uint32_t  ADR0;                              /*!< (@ 0x4000000C) I2C Slave Address Register 0. Contains the 7-bit
                                                         slave address for operation of the I2C interface in slave mode,
                                                          and is not used in master mode. The least significant bit determines
                                                          whether a slave responds to the General Call address.                */
  __IO uint32_t  SCLH;                              /*!< (@ 0x40000010) SCH Duty Cycle Register High Half Word. Determines
                                                         the high time of the I2C clock.                                       */
  __IO uint32_t  SCLL;                              /*!< (@ 0x40000014) SCL Duty Cycle Register Low Half Word. Determines
                                                         the low time of the I2C clock. I2nSCLL and I2nSCLH together
                                                          determine the clock frequency generated by an I2C master and
                                                          certain times used in slave mode.                                    */
  __O  uint32_t  CONCLR;                            /*!< (@ 0x40000018) I2C Control Clear Register. When a one is written
                                                         to a bit of this register, the corresponding bit in the I2C
                                                          control register is cleared. Writing a zero has no effect on
                                                          the corresponding bit in the I2C control register.                   */
  __IO uint32_t  MMCTRL;                            /*!< (@ 0x4000001C) Monitor mode control register.                         */
  __IO uint32_t  ADR1;                              /*!< (@ 0x40000020) I2C Slave Address Register. Contains the 7-bit
                                                         slave address for operation of the I2C interface in slave mode,
                                                          and is not used in master mode. The least significant bit determines
                                                          whether a slave responds to the General Call address.                */
  __IO uint32_t  ADR2;                              /*!< (@ 0x40000024) I2C Slave Address Register. Contains the 7-bit
                                                         slave address for operation of the I2C interface in slave mode,
                                                          and is not used in master mode. The least significant bit determines
                                                          whether a slave responds to the General Call address.                */
  __IO uint32_t  ADR3;                              /*!< (@ 0x40000028) I2C Slave Address Register. Contains the 7-bit
                                                         slave address for operation of the I2C interface in slave mode,
                                                          and is not used in master mode. The least significant bit determines
                                                          whether a slave responds to the General Call address.                */
  __I  uint32_t  DATA_BUFFER;                       /*!< (@ 0x4000002C) Data buffer register. The contents of the 8 MSBs
                                                         of the I2DAT shift register will be transferred to the DATA_BUFFER
                                                          automatically after every nine bits (8 bits of data plus ACK
                                                          or NACK) has been received on the bus.                               */
  __IO uint32_t  MASK0;                             /*!< (@ 0x40000030) I2C Slave address mask register. This mask register
                                                         is associated with I2ADR0 to determine an address match. The
                                                          mask register has no effect when comparing to the General Call
                                                          address (0000000).                                                   */
  __IO uint32_t  MASK1;                             /*!< (@ 0x40000034) I2C Slave address mask register. This mask register
                                                         is associated with I2ADR0 to determine an address match. The
                                                          mask register has no effect when comparing to the General Call
                                                          address (0000000).                                                   */
  __IO uint32_t  MASK2;                             /*!< (@ 0x40000038) I2C Slave address mask register. This mask register
                                                         is associated with I2ADR0 to determine an address match. The
                                                          mask register has no effect when comparing to the General Call
                                                          address (0000000).                                                   */
  __IO uint32_t  MASK3;                             /*!< (@ 0x4000003C) I2C Slave address mask register. This mask register
                                                         is associated with I2ADR0 to determine an address match. The
                                                          mask register has no effect when comparing to the General Call
                                                          address (0000000).                                                   */
} LPC_I2C0_Type;


/* ================================================================================ */
/* ================                      WWDT                      ================ */
/* ================================================================================ */


/**
  * @brief Windowed Watchdog Timer (WWDT) (WWDT)
  */

typedef struct {                                    /*!< (@ 0x40004000) WWDT Structure                                         */
  __IO uint32_t  MOD;                               /*!< (@ 0x40004000) Watchdog mode register. This register contains
                                                         the basic mode and status of the Watchdog Timer.                      */
  __IO uint32_t  TC;                                /*!< (@ 0x40004004) Watchdog timer constant register. This 24-bit
                                                         register determines the time-out value.                               */
  __O  uint32_t  FEED;                              /*!< (@ 0x40004008) Watchdog feed sequence register. Writing 0xAA
                                                         followed by 0x55 to this register reloads the Watchdog timer
                                                          with the value contained in WDTC.                                    */
  __I  uint32_t  TV;                                /*!< (@ 0x4000400C) Watchdog timer value register. This 24-bit register
                                                         reads out the current value of the Watchdog timer.                    */
  __IO uint32_t  CLKSEL;                            /*!< (@ 0x40004010) Watchdog clock select register.                        */
  __IO uint32_t  WARNINT;                           /*!< (@ 0x40004014) Watchdog Warning Interrupt compare value.              */
  __IO uint32_t  WINDOW;                            /*!< (@ 0x40004018) Watchdog Window compare value.                         */
} LPC_WWDT_Type;


/* ================================================================================ */
/* ================                     USART0                     ================ */
/* ================================================================================ */


/**
  * @brief USART0 (USART0)
  */

typedef struct {                                    /*!< (@ 0x40008000) USART0 Structure                                       */
  
  union {
    __IO uint32_t  DLL;                             /*!< (@ 0x40008000) Divisor Latch LSB. Least significant byte of
                                                         the baud rate divisor value. The full divisor is used to generate
                                                          a baud rate from the fractional rate divider. (DLAB=1)               */
    __O  uint32_t  THR;                             /*!< (@ 0x40008000) Transmit Holding Register. The next character
                                                         to be transmitted is written here. (DLAB=0)                           */
    __I  uint32_t  RBR;                             /*!< (@ 0x40008000) Receiver Buffer Register. Contains the next received
                                                         character to be read. (DLAB=0)                                        */
  } ;
  
  union {
    __IO uint32_t  IER;                             /*!< (@ 0x40008004) Interrupt Enable Register. Contains individual
                                                         interrupt enable bits for the 7 potential USART interrupts.
                                                          (DLAB=0)                                                             */
    __IO uint32_t  DLM;                             /*!< (@ 0x40008004) Divisor Latch MSB. Most significant byte of the
                                                         baud rate divisor value. The full divisor is used to generate
                                                          a baud rate from the fractional rate divider. (DLAB=1)               */
  } ;
  
  union {
    __O  uint32_t  FCR;                             /*!< (@ 0x40008008) FIFO Control Register. Controls USART FIFO usage
                                                         and modes.                                                            */
    __I  uint32_t  IIR;                             /*!< (@ 0x40008008) Interrupt ID Register. Identifies which interrupt(s)
                                                         are pending.                                                          */
  } ;
  __IO uint32_t  LCR;                               /*!< (@ 0x4000800C) Line Control Register. Contains controls for
                                                         frame formatting and break generation.                                */
  __IO uint32_t  MCR;                               /*!< (@ 0x40008010) Modem Control Register.                                */
  __I  uint32_t  LSR;                               /*!< (@ 0x40008014) Line Status Register. Contains flags for transmit
                                                         and receive status, including line errors.                            */
  __I  uint32_t  MSR;                               /*!< (@ 0x40008018) Modem Status Register.                                 */
  __IO uint32_t  SCR;                               /*!< (@ 0x4000801C) Scratch Pad Register. Eight-bit temporary storage
                                                         for software.                                                         */
  __IO uint32_t  ACR;                               /*!< (@ 0x40008020) Auto-baud Control Register. Contains controls
                                                         for the auto-baud feature.                                            */
  __IO uint32_t  ICR;                               /*!< (@ 0x40008024) IrDA Control Register. Enables and configures
                                                         the IrDA (remote control) mode.                                       */
  __IO uint32_t  FDR;                               /*!< (@ 0x40008028) Fractional Divider Register. Generates a clock
                                                         input for the baud rate divider.                                      */
  __IO uint32_t  OSR;                               /*!< (@ 0x4000802C) Oversampling Register. Controls the degree of
                                                         oversampling during each bit time.                                    */
  __IO uint32_t  TER;                               /*!< (@ 0x40008030) Transmit Enable Register. Turns off USART transmitter
                                                         for use with software flow control.                                   */
  __I  uint32_t  RESERVED0[3];
  __IO uint32_t  HDEN;                              /*!< (@ 0x40008040) Half duplex enable register.                           */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  SCICTRL;                           /*!< (@ 0x40008048) Smart Card Interface Control register. Enables
                                                         and configures the Smart Card Interface feature.                      */
  __IO uint32_t  RS485CTRL;                         /*!< (@ 0x4000804C) RS-485/EIA-485 Control. Contains controls to
                                                         configure various aspects of RS-485/EIA-485 modes.                    */
  __IO uint32_t  RS485ADRMATCH;                     /*!< (@ 0x40008050) RS-485/EIA-485 address match. Contains the address
                                                         match value for RS-485/EIA-485 mode.                                  */
  __IO uint32_t  RS485DLY;                          /*!< (@ 0x40008054) RS-485/EIA-485 direction control delay.                */
  __IO uint32_t  SYNCCTRL;                          /*!< (@ 0x40008058) Synchronous mode control register.                     */
} LPC_USART0_Type;


/* ================================================================================ */
/* ================                     CT16B0                     ================ */
/* ================================================================================ */


/**
  * @brief 16-bit counter/timers CT16B0 (CT16B0)
  */

typedef struct {                                    /*!< (@ 0x4000C000) CT16B0 Structure                                       */
  __IO uint32_t  IR;                                /*!< (@ 0x4000C000) Interrupt Register. The IR can be written to
                                                         clear interrupts. The IR can be read to identify which of eight
                                                          possible interrupt sources are pending.                              */
  __IO uint32_t  TCR;                               /*!< (@ 0x4000C004) Timer Control Register. The TCR is used to control
                                                         the Timer Counter functions. The Timer Counter can be disabled
                                                          or reset through the TCR.                                            */
  __IO uint32_t  TC;                                /*!< (@ 0x4000C008) Timer Counter. The 16-bit TC is incremented every
                                                         PR+1 cycles of PCLK. The TC is controlled through the TCR.            */
  __IO uint32_t  PR;                                /*!< (@ 0x4000C00C) Prescale Register. When the Prescale Counter
                                                         (below) is equal to this value, the next clock increments the
                                                          TC and clears the PC.                                                */
  __IO uint32_t  PC;                                /*!< (@ 0x4000C010) Prescale Counter. The 16-bit PC is a counter
                                                         which is incremented to the value stored in PR. When the value
                                                          in PR is reached, the TC is incremented and the PC is cleared.
                                                          The PC is observable and controllable through the bus interface.     */
  __IO uint32_t  MCR;                               /*!< (@ 0x4000C014) Match Control Register. The MCR is used to control
                                                         if an interrupt is generated and if the TC is reset when a Match
                                                          occurs.                                                              */
  __IO uint32_t  MR0;                               /*!< (@ 0x4000C018) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR1;                               /*!< (@ 0x4000C01C) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR2;                               /*!< (@ 0x4000C020) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR3;                               /*!< (@ 0x4000C024) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  CCR;                               /*!< (@ 0x4000C028) Capture Control Register. The CCR controls which
                                                         edges of the capture inputs are used to load the Capture Registers
                                                          and whether or not an interrupt is generated when a capture
                                                          takes place.                                                         */
  __I  uint32_t  CR0;                               /*!< (@ 0x4000C02C) Capture Register 0. CR0 is loaded with the value
                                                         of TC when there is an event on the CT16B0_CAP0 input.                */
  __I  uint32_t  RESERVED0;
  __I  uint32_t  CR1;                               /*!< (@ 0x4000C034) Capture Register 1. CR1 is loaded with the value
                                                         of TC when there is an event on the CT16B0_CAP1 input.                */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  EMR;                               /*!< (@ 0x4000C03C) External Match Register. The EMR controls the
                                                         match function and the external match pins CT16B0_MAT[1:0] and
                                                          CT16B1_MAT[1:0].                                                     */
  __I  uint32_t  RESERVED2[12];
  __IO uint32_t  CTCR;                              /*!< (@ 0x4000C070) Count Control Register. The CTCR selects between
                                                         Timer and Counter mode, and in Counter mode selects the signal
                                                          and edge(s) for counting.                                            */
  __IO uint32_t  PWMC;                              /*!< (@ 0x4000C074) PWM Control Register. The PWMCON enables PWM
                                                         mode for the external match pins CT16B0_MAT[1:0] and CT16B1_MAT[1:0]. */
} LPC_CT16B0_Type;


/* ================================================================================ */
/* ================                     CT16B1                     ================ */
/* ================================================================================ */


/**
  * @brief 16-bit counter/timers CT16B1 (CT16B1)
  */

typedef struct {                                    /*!< (@ 0x40010000) CT16B1 Structure                                       */
  __IO uint32_t  IR;                                /*!< (@ 0x40010000) Interrupt Register. The IR can be written to
                                                         clear interrupts. The IR can be read to identify which of eight
                                                          possible interrupt sources are pending.                              */
  __IO uint32_t  TCR;                               /*!< (@ 0x40010004) Timer Control Register. The TCR is used to control
                                                         the Timer Counter functions. The Timer Counter can be disabled
                                                          or reset through the TCR.                                            */
  __IO uint32_t  TC;                                /*!< (@ 0x40010008) Timer Counter. The 16-bit TC is incremented every
                                                         PR+1 cycles of PCLK. The TC is controlled through the TCR.            */
  __IO uint32_t  PR;                                /*!< (@ 0x4001000C) Prescale Register. When the Prescale Counter
                                                         (below) is equal to this value, the next clock increments the
                                                          TC and clears the PC.                                                */
  __IO uint32_t  PC;                                /*!< (@ 0x40010010) Prescale Counter. The 16-bit PC is a counter
                                                         which is incremented to the value stored in PR. When the value
                                                          in PR is reached, the TC is incremented and the PC is cleared.
                                                          The PC is observable and controllable through the bus interface.     */
  __IO uint32_t  MCR;                               /*!< (@ 0x40010014) Match Control Register. The MCR is used to control
                                                         if an interrupt is generated and if the TC is reset when a Match
                                                          occurs.                                                              */
  __IO uint32_t  MR0;                               /*!< (@ 0x40010018) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR1;                               /*!< (@ 0x4001001C) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR2;                               /*!< (@ 0x40010020) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR3;                               /*!< (@ 0x40010024) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  CCR;                               /*!< (@ 0x40010028) Capture Control Register. The CCR controls which
                                                         edges of the capture inputs are used to load the Capture Registers
                                                          and whether or not an interrupt is generated when a capture
                                                          takes place.                                                         */
  __I  uint32_t  CR0;                               /*!< (@ 0x4001002C) Capture Register 0. CR0 is loaded with the value
                                                         of TC when there is an event on the CT16B0_CAP0 input.                */
  __I  uint32_t  CR1;                               /*!< (@ 0x40010030) Capture Register 1. CR1 is loaded with the value
                                                         of TC when there is an event on the CT16B1_CAP1 input.                */
  __I  uint32_t  RESERVED0[2];
  __IO uint32_t  EMR;                               /*!< (@ 0x4001003C) External Match Register. The EMR controls the
                                                         match function and the external match pins CT16B0_MAT[1:0] and
                                                          CT16B1_MAT[1:0].                                                     */
  __I  uint32_t  RESERVED1[12];
  __IO uint32_t  CTCR;                              /*!< (@ 0x40010070) Count Control Register. The CTCR selects between
                                                         Timer and Counter mode, and in Counter mode selects the signal
                                                          and edge(s) for counting.                                            */
  __IO uint32_t  PWMC;                              /*!< (@ 0x40010074) PWM Control Register. The PWMCON enables PWM
                                                         mode for the external match pins CT16B0_MAT[1:0] and CT16B1_MAT[1:0]. */
} LPC_CT16B1_Type;


/* ================================================================================ */
/* ================                     CT32B0                     ================ */
/* ================================================================================ */


/**
  * @brief 32-bit counter/timers CT32B0 (CT32B0)
  */

typedef struct {                                    /*!< (@ 0x40014000) CT32B0 Structure                                       */
  __IO uint32_t  IR;                                /*!< (@ 0x40014000) Interrupt Register. The IR can be written to
                                                         clear interrupts. The IR can be read to identify which of eight
                                                          possible interrupt sources are pending.                              */
  __IO uint32_t  TCR;                               /*!< (@ 0x40014004) Timer Control Register. The TCR is used to control
                                                         the Timer Counter functions. The Timer Counter can be disabled
                                                          or reset through the TCR.                                            */
  __IO uint32_t  TC;                                /*!< (@ 0x40014008) Timer Counter. The 32-bit TC is incremented every
                                                         PR+1 cycles of PCLK. The TC is controlled through the TCR.            */
  __IO uint32_t  PR;                                /*!< (@ 0x4001400C) Prescale Register. When the Prescale Counter
                                                         (below) is equal to this value, the next clock increments the
                                                          TC and clears the PC.                                                */
  __IO uint32_t  PC;                                /*!< (@ 0x40014010) Prescale Counter. The 32-bit PC is a counter
                                                         which is incremented to the value stored in PR. When the value
                                                          in PR is reached, the TC is incremented and the PC is cleared.
                                                          The PC is observable and controllable through the bus interface.     */
  __IO uint32_t  MCR;                               /*!< (@ 0x40014014) Match Control Register. The MCR is used to control
                                                         if an interrupt is generated and if the TC is reset when a Match
                                                          occurs.                                                              */
  __IO uint32_t  MR0;                               /*!< (@ 0x40014018) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR1;                               /*!< (@ 0x4001401C) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR2;                               /*!< (@ 0x40014020) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR3;                               /*!< (@ 0x40014024) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  CCR;                               /*!< (@ 0x40014028) Capture Control Register. The CCR controls which
                                                         edges of the capture inputs are used to load the Capture Registers
                                                          and whether or not an interrupt is generated when a capture
                                                          takes place.                                                         */
  __I  uint32_t  CR0;                               /*!< (@ 0x4001402C) Capture Register 0. CR0 is loaded with the value
                                                         of TC when there is an event on the CT32B0_CAP0 input.                */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  CR1;                               /*!< (@ 0x40014034) Capture Register 1. CR1 is loaded with the value
                                                         of TC when there is an event on the CT32B0_CAP1 input.                */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  EMR;                               /*!< (@ 0x4001403C) External Match Register. The EMR controls the
                                                         match function and the external match pins CT32Bn_MAT[3:0].           */
  __I  uint32_t  RESERVED2[12];
  __IO uint32_t  CTCR;                              /*!< (@ 0x40014070) Count Control Register. The CTCR selects between
                                                         Timer and Counter mode, and in Counter mode selects the signal
                                                          and edge(s) for counting.                                            */
  __IO uint32_t  PWMC;                              /*!< (@ 0x40014074) PWM Control Register. The PWMCON enables PWM
                                                         mode for the external match pins CT32Bn_MAT[3:0].                     */
} LPC_CT32B0_Type;


/* ================================================================================ */
/* ================                     CT32B1                     ================ */
/* ================================================================================ */


/**
  * @brief 32-bit counter/timers CT32B1 (CT32B1)
  */

typedef struct {                                    /*!< (@ 0x40018000) CT32B1 Structure                                       */
  __IO uint32_t  IR;                                /*!< (@ 0x40018000) Interrupt Register. The IR can be written to
                                                         clear interrupts. The IR can be read to identify which of eight
                                                          possible interrupt sources are pending.                              */
  __IO uint32_t  TCR;                               /*!< (@ 0x40018004) Timer Control Register. The TCR is used to control
                                                         the Timer Counter functions. The Timer Counter can be disabled
                                                          or reset through the TCR.                                            */
  __IO uint32_t  TC;                                /*!< (@ 0x40018008) Timer Counter. The 32-bit TC is incremented every
                                                         PR+1 cycles of PCLK. The TC is controlled through the TCR.            */
  __IO uint32_t  PR;                                /*!< (@ 0x4001800C) Prescale Register. When the Prescale Counter
                                                         (below) is equal to this value, the next clock increments the
                                                          TC and clears the PC.                                                */
  __IO uint32_t  PC;                                /*!< (@ 0x40018010) Prescale Counter. The 32-bit PC is a counter
                                                         which is incremented to the value stored in PR. When the value
                                                          in PR is reached, the TC is incremented and the PC is cleared.
                                                          The PC is observable and controllable through the bus interface.     */
  __IO uint32_t  MCR;                               /*!< (@ 0x40018014) Match Control Register. The MCR is used to control
                                                         if an interrupt is generated and if the TC is reset when a Match
                                                          occurs.                                                              */
  __IO uint32_t  MR0;                               /*!< (@ 0x40018018) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR1;                               /*!< (@ 0x4001801C) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR2;                               /*!< (@ 0x40018020) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  MR3;                               /*!< (@ 0x40018024) Match Register 0. MR0 can be enabled through
                                                         the MCR to reset the TC, stop both the TC and PC, and/or generate
                                                          an interrupt every time MR0 matches the TC.                          */
  __IO uint32_t  CCR;                               /*!< (@ 0x40018028) Capture Control Register. The CCR controls which
                                                         edges of the capture inputs are used to load the Capture Registers
                                                          and whether or not an interrupt is generated when a capture
                                                          takes place.                                                         */
  __I  uint32_t  CR0;                               /*!< (@ 0x4001802C) Capture Register 0. CR0 is loaded with the value
                                                         of TC when there is an event on the CT32B0_CAP0 input.                */
  __IO uint32_t  CR1;                               /*!< (@ 0x40018030) Capture Register 1. CR1 is loaded with the value
                                                         of TC when there is an event on the CT32B1_CAP1 input.                */
  __I  uint32_t  RESERVED0[2];
  __IO uint32_t  EMR;                               /*!< (@ 0x4001803C) External Match Register. The EMR controls the
                                                         match function and the external match pins CT32Bn_MAT[3:0].           */
  __I  uint32_t  RESERVED1[12];
  __IO uint32_t  CTCR;                              /*!< (@ 0x40018070) Count Control Register. The CTCR selects between
                                                         Timer and Counter mode, and in Counter mode selects the signal
                                                          and edge(s) for counting.                                            */
  __IO uint32_t  PWMC;                              /*!< (@ 0x40018074) PWM Control Register. The PWMCON enables PWM
                                                         mode for the external match pins CT32Bn_MAT[3:0].                     */
} LPC_CT32B1_Type;


/* ================================================================================ */
/* ================                       ADC                      ================ */
/* ================================================================================ */


/**
  * @brief Product name title=Kylin UM Chapter title=Kylin12-bit Analog-to-Digital Converter (ADC) Modification date=5/13/2013 Major revision=0 Minor revision=1  (ADC)
  */

typedef struct {                                    /*!< (@ 0x4001C000) ADC Structure                                          */
  __IO uint32_t  CTRL;                              /*!< (@ 0x4001C000) A/D Control Register. Contains the clock divide
                                                         value, enable bits for each sequence and the A/D power-down
                                                          bit.                                                                 */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  SEQA_CTRL;                         /*!< (@ 0x4001C008) A/D Conversion Sequence-A control Register: Controls
                                                         triggering and channel selection for conversion sequence-A.
                                                          Also specifies interrupt mode for sequence-A.                        */
  __IO uint32_t  SEQB_CTRL;                         /*!< (@ 0x4001C00C) A/D Conversion Sequence-B Control Register: Controls
                                                         triggering and channel selection for conversion sequence-B.
                                                          Also specifies interrupt mode for sequence-B.                        */
  __IO uint32_t  SEQA_GDAT;                         /*!< (@ 0x4001C010) A/D Sequence-A Global Data Register. This register
                                                         contains the result of the most recent A/D conversion performed
                                                          under sequence-A                                                     */
  __IO uint32_t  SEQB_GDAT;                         /*!< (@ 0x4001C014) A/D Sequence-B Global Data Register. This register
                                                         contains the result of the most recent A/D conversion performed
                                                          under sequence-B                                                     */
  __I  uint32_t  RESERVED1[2];
  __I  uint32_t  DAT[12];                           /*!< (@ 0x4001C020) A/D Channel 0 Data Register. This register contains
                                                         the result of the most recent conversion completed on channel
                                                          0.                                                                   */
  __IO uint32_t  THR0_LOW;                          /*!< (@ 0x4001C050) A/D Low Compare Threshold Register 0 : Contains
                                                         the lower threshold level for automatic threshold comparison
                                                          for any channels linked to threshold pair 0.                         */
  __IO uint32_t  THR1_LOW;                          /*!< (@ 0x4001C054) A/D Low Compare Threshold Register 1: Contains
                                                         the lower threshold level for automatic threshold comparison
                                                          for any channels linked to threshold pair 1.                         */
  __IO uint32_t  THR0_HIGH;                         /*!< (@ 0x4001C058) A/D High Compare Threshold Register 0: Contains
                                                         the upper threshold level for automatic threshold comparison
                                                          for any channels linked to threshold pair 0.                         */
  __IO uint32_t  THR1_HIGH;                         /*!< (@ 0x4001C05C) A/D High Compare Threshold Register 1: Contains
                                                         the upper threshold level for automatic threshold comparison
                                                          for any channels linked to threshold pair 1.                         */
  __IO uint32_t  CHAN_THRSEL;                       /*!< (@ 0x4001C060) A/D Channel-Threshold Select Register. Specifies
                                                         which set of threshold compare registers are to be used for
                                                          each channel                                                         */
  __IO uint32_t  INTEN;                             /*!< (@ 0x4001C064) A/D Interrupt Enable Register. This register
                                                         contains enable bits that enable the sequence-A, sequence-B,
                                                          threshold compare and data overrun interrupts to be generated.       */
  __IO uint32_t  FLAGS;                             /*!< (@ 0x4001C068) A/D Flags Register. Contains the four interrupt
                                                         request flags and the individual component overrun and threshold-compare
                                                          flags. (The overrun bits replicate information stored in the
                                                          result registers).                                                   */
  __IO uint32_t  TRM;                               /*!< (@ 0x4001C06C) ADC trim register.                                     */
} LPC_ADC_Type;


/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */


/**
  * @brief Real-Time Clock (RTC) (RTC)
  */

typedef struct {                                    /*!< (@ 0x40024000) RTC Structure                                          */
  __IO uint32_t  CTRL;                              /*!< (@ 0x40024000) RTC control register                                   */
  __IO uint32_t  MATCH;                             /*!< (@ 0x40024004) RTC match register                                     */
  __IO uint32_t  COUNT;                             /*!< (@ 0x40024008) RTC counter register                                   */
  __IO uint32_t  WAKE;                              /*!< (@ 0x4002400C) RTC high-resolution/wake-up timer control register     */
} LPC_RTC_Type;


/* ================================================================================ */
/* ================                   DMATRIGMUX                   ================ */
/* ================================================================================ */


/**
  * @brief Product name title=Kylin UM Chapter title=KylinDMA controller Modification date=5/13/2013 Major revision=0 Minor revision=1  (DMATRIGMUX)
  */

typedef struct {                                    /*!< (@ 0x40028000) DMATRIGMUX Structure                                   */
  __IO uint32_t  DMA_ITRIG_PINMUX[16];              /*!< (@ 0x40028000) Trigger input select register for DMA channel
                                                         0.                                                                    */
} LPC_DMATRIGMUX_Type;


/* ================================================================================ */
/* ================                       PMU                      ================ */
/* ================================================================================ */


/**
  * @brief Product name title=Kylin UM Chapter title=KylinPower Management Unit (PMU) Modification date=5/13/2013 Major revision=0 Minor revision=1  (PMU)
  */

typedef struct {                                    /*!< (@ 0x40038000) PMU Structure                                          */
  __IO uint32_t  PCON;                              /*!< (@ 0x40038000) Power control register                                 */
  __IO uint32_t  GPREG0;                            /*!< (@ 0x40038004) General purpose register 0                             */
  __IO uint32_t  GPREG1;                            /*!< (@ 0x40038008) General purpose register 0                             */
  __IO uint32_t  GPREG2;                            /*!< (@ 0x4003800C) General purpose register 0                             */
  __IO uint32_t  GPREG3;                            /*!< (@ 0x40038010) General purpose register 0                             */
  __IO uint32_t  DPDCTRL;                           /*!< (@ 0x40038014) Deep power down control register                       */
} LPC_PMU_Type;


/* ================================================================================ */
/* ================                    FLASHCTRL                   ================ */
/* ================================================================================ */


/**
  * @brief Flash controller  (FLASHCTRL)
  */

typedef struct {                                    /*!< (@ 0x4003C000) FLASHCTRL Structure                                    */
  __I  uint32_t  RESERVED0[4];
  __IO uint32_t  FLASHCFG;                          /*!< (@ 0x4003C010) Flash configuration register                           */
  __I  uint32_t  RESERVED1[3];
  __IO uint32_t  FMSSTART;                          /*!< (@ 0x4003C020) Signature start address register                       */
  __IO uint32_t  FMSSTOP;                           /*!< (@ 0x4003C024) Signature stop-address register                        */
  __I  uint32_t  RESERVED2;
  __I  uint32_t  FMSW0;                             /*!< (@ 0x4003C02C) Signature Word                                         */
} LPC_FLASHCTRL_Type;


/* ================================================================================ */
/* ================                      SSP0                      ================ */
/* ================================================================================ */


/**
  * @brief SSP/SPI  (SSP0)
  */

typedef struct {                                    /*!< (@ 0x40040000) SSP0 Structure                                         */
  __IO uint32_t  CR0;                               /*!< (@ 0x40040000) Control Register 0. Selects the serial clock
                                                         rate, bus type, and data size.                                        */
  __IO uint32_t  CR1;                               /*!< (@ 0x40040004) Control Register 1. Selects master/slave and
                                                         other modes.                                                          */
  __IO uint32_t  DR;                                /*!< (@ 0x40040008) Data Register. Writes fill the transmit FIFO,
                                                         and reads empty the receive FIFO.                                     */
  __I  uint32_t  SR;                                /*!< (@ 0x4004000C) Status Register                                        */
  __IO uint32_t  CPSR;                              /*!< (@ 0x40040010) Clock Prescale Register                                */
  __IO uint32_t  IMSC;                              /*!< (@ 0x40040014) Interrupt Mask Set and Clear Register                  */
  __I  uint32_t  RIS;                               /*!< (@ 0x40040018) Raw Interrupt Status Register                          */
  __I  uint32_t  MIS;                               /*!< (@ 0x4004001C) Masked Interrupt Status Register                       */
  __O  uint32_t  ICR;                               /*!< (@ 0x40040020) SSPICR Interrupt Clear Register                        */
  __IO uint32_t  DMACR;                             /*!< (@ 0x40040024) DMA Control Register                                   */
} LPC_SSP0_Type;


/* ================================================================================ */
/* ================                      IOCON                     ================ */
/* ================================================================================ */


/**
  * @brief Product name title=Kylin UM Chapter title=KylinI/O control (IOCON) Modification date=5/13/2013 Major revision=0 Minor revision=1  (IOCON)
  */

typedef struct {                                    /*!< (@ 0x40044000) IOCON Structure                                        */
  __IO uint32_t  PIO0_0;                            /*!< (@ 0x40044000) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_1;                            /*!< (@ 0x40044004) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_2;                            /*!< (@ 0x40044008) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_3;                            /*!< (@ 0x4004400C) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_4;                            /*!< (@ 0x40044010) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_5;                            /*!< (@ 0x40044014) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_6;                            /*!< (@ 0x40044018) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_7;                            /*!< (@ 0x4004401C) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_8;                            /*!< (@ 0x40044020) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_9;                            /*!< (@ 0x40044024) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_10;                           /*!< (@ 0x40044028) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_11;                           /*!< (@ 0x4004402C) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_12;                           /*!< (@ 0x40044030) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_13;                           /*!< (@ 0x40044034) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_14;                           /*!< (@ 0x40044038) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_15;                           /*!< (@ 0x4004403C) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_16;                           /*!< (@ 0x40044040) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_17;                           /*!< (@ 0x40044044) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_18;                           /*!< (@ 0x40044048) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_19;                           /*!< (@ 0x4004404C) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_20;                           /*!< (@ 0x40044050) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_21;                           /*!< (@ 0x40044054) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_22;                           /*!< (@ 0x40044058) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO0_23;                           /*!< (@ 0x4004405C) I/O configuration for port PIO0                        */
  __IO uint32_t  PIO1_0;                            /*!< (@ 0x40044060) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_1;                            /*!< (@ 0x40044064) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_2;                            /*!< (@ 0x40044068) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_3;                            /*!< (@ 0x4004406C) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_4;                            /*!< (@ 0x40044070) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_5;                            /*!< (@ 0x40044074) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_6;                            /*!< (@ 0x40044078) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_7;                            /*!< (@ 0x4004407C) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_8;                            /*!< (@ 0x40044080) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_9;                            /*!< (@ 0x40044084) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_10;                           /*!< (@ 0x40044088) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_11;                           /*!< (@ 0x4004408C) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_12;                           /*!< (@ 0x40044090) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_13;                           /*!< (@ 0x40044094) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_14;                           /*!< (@ 0x40044098) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_15;                           /*!< (@ 0x4004409C) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_16;                           /*!< (@ 0x400440A0) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_17;                           /*!< (@ 0x400440A4) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_18;                           /*!< (@ 0x400440A8) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_19;                           /*!< (@ 0x400440AC) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_20;                           /*!< (@ 0x400440B0) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_21;                           /*!< (@ 0x400440B4) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_22;                           /*!< (@ 0x400440B8) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_23;                           /*!< (@ 0x400440BC) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_24;                           /*!< (@ 0x400440C0) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_25;                           /*!< (@ 0x400440C4) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_26;                           /*!< (@ 0x400440C8) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_27;                           /*!< (@ 0x400440CC) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_28;                           /*!< (@ 0x400440D0) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_29;                           /*!< (@ 0x400440D4) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_30;                           /*!< (@ 0x400440D8) I/O configuration for port PIO1                        */
  __IO uint32_t  PIO1_31;                           /*!< (@ 0x400440DC) I/O configuration for port PIO1                        */
  __I  uint32_t  RESERVED0[4];
  __IO uint32_t  PIO2_0;                            /*!< (@ 0x400440F0) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_1;                            /*!< (@ 0x400440F4) I/O configuration for port PIO2                        */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  PIO2_2;                            /*!< (@ 0x400440FC) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_3;                            /*!< (@ 0x40044100) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_4;                            /*!< (@ 0x40044104) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_5;                            /*!< (@ 0x40044108) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_6;                            /*!< (@ 0x4004410C) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_7;                            /*!< (@ 0x40044110) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_8;                            /*!< (@ 0x40044114) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_9;                            /*!< (@ 0x40044118) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_10;                           /*!< (@ 0x4004411C) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_11;                           /*!< (@ 0x40044120) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_12;                           /*!< (@ 0x40044124) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_13;                           /*!< (@ 0x40044128) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_14;                           /*!< (@ 0x4004412C) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_15;                           /*!< (@ 0x40044130) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_16;                           /*!< (@ 0x40044134) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_17;                           /*!< (@ 0x40044138) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_18;                           /*!< (@ 0x4004413C) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_19;                           /*!< (@ 0x40044140) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_20;                           /*!< (@ 0x40044144) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_21;                           /*!< (@ 0x40044148) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_22;                           /*!< (@ 0x4004414C) I/O configuration for port PIO2                        */
  __IO uint32_t  PIO2_23;                           /*!< (@ 0x40044150) I/O configuration for port PIO2                        */
} LPC_IOCON_Type;


/* ================================================================================ */
/* ================                     SYSCON                     ================ */
/* ================================================================================ */


/**
  * @brief Product name title=Kylin UM Chapter title=KylinSystem configuration (SYSCON) Modification date=5/13/2013 Major revision=0 Minor revision=1  (SYSCON)
  */

typedef struct {                                    /*!< (@ 0x40048000) SYSCON Structure                                       */
  __IO uint32_t  SYSMEMREMAP;                       /*!< (@ 0x40048000) System memory remap                                    */
  __IO uint32_t  PRESETCTRL;                        /*!< (@ 0x40048004) Peripheral reset control                               */
  __IO uint32_t  SYSPLLCTRL;                        /*!< (@ 0x40048008) System PLL control                                     */
  __I  uint32_t  SYSPLLSTAT;                        /*!< (@ 0x4004800C) System PLL status                                      */
  __IO uint32_t  USBPLLCTRL;                        /*!< (@ 0x40048010) USB PLL control                                        */
  __I  uint32_t  USBPLLSTAT;                        /*!< (@ 0x40048014) USB PLL status                                         */
  __I  uint32_t  RESERVED0;
  __IO uint32_t  RTCOSCCTRL;                        /*!< (@ 0x4004801C) Ultra-low power 32 kHz oscillator output control       */
  __IO uint32_t  SYSOSCCTRL;                        /*!< (@ 0x40048020) System oscillator control                              */
  __IO uint32_t  WDTOSCCTRL;                        /*!< (@ 0x40048024) Watchdog oscillator control                            */
  __I  uint32_t  RESERVED1[2];
  __IO uint32_t  SYSRSTSTAT;                        /*!< (@ 0x40048030) System reset status register                           */
  __I  uint32_t  RESERVED2[3];
  __IO uint32_t  SYSPLLCLKSEL;                      /*!< (@ 0x40048040) System PLL clock source select                         */
  __IO uint32_t  SYSPLLCLKUEN;                      /*!< (@ 0x40048044) System PLL clock source update enable                  */
  __IO uint32_t  USBPLLCLKSEL;                      /*!< (@ 0x40048048) USB PLL clock source select                            */
  __IO uint32_t  USBPLLCLKUEN;                      /*!< (@ 0x4004804C) USB PLL clock source update enable                     */
  __I  uint32_t  RESERVED3[8];
  __IO uint32_t  MAINCLKSEL;                        /*!< (@ 0x40048070) Main clock source select                               */
  __IO uint32_t  MAINCLKUEN;                        /*!< (@ 0x40048074) Main clock source update enable                        */
  __IO uint32_t  SYSAHBCLKDIV;                      /*!< (@ 0x40048078) System clock divider                                   */
  __I  uint32_t  RESERVED4;
  __IO uint32_t  SYSAHBCLKCTRL;                     /*!< (@ 0x40048080) System clock control                                   */
  __I  uint32_t  RESERVED5[4];
  __IO uint32_t  SSP0CLKDIV;                        /*!< (@ 0x40048094) SSP0 clock divider                                     */
  __IO uint32_t  USART0CLKDIV;                      /*!< (@ 0x40048098) USART0 clock divider                                   */
  __IO uint32_t  SSP1CLKDIV;                        /*!< (@ 0x4004809C) SSP1 clock divider                                     */
  __IO uint32_t  FRGCLKDIV;                         /*!< (@ 0x400480A0) Clock divider for the common fractional baud
                                                         rate generator of USART1 to USART4                                    */
  __I  uint32_t  RESERVED6[7];
  __IO uint32_t  USBCLKSEL;                         /*!< (@ 0x400480C0) USB clock source select                                */
  __IO uint32_t  USBCLKUEN;                         /*!< (@ 0x400480C4) USB clock source update enable                         */
  __IO uint32_t  USBCLKDIV;                         /*!< (@ 0x400480C8) USB clock source divider                               */
  __I  uint32_t  RESERVED7[5];
  __IO uint32_t  CLKOUTSEL;                         /*!< (@ 0x400480E0) CLKOUT clock source select                             */
  __IO uint32_t  CLKOUTUEN;                         /*!< (@ 0x400480E4) CLKOUT clock source update enable                      */
  __IO uint32_t  CLKOUTDIV;                         /*!< (@ 0x400480E8) CLKOUT clock divider                                   */
  __I  uint32_t  RESERVED8;
  __IO uint32_t  UARTFRGDIV;                        /*!< (@ 0x400480F0) USART fractional generator divider value               */
  __IO uint32_t  UARTFRGMULT;                       /*!< (@ 0x400480F4) USART fractional generator multiplier value            */
  __I  uint32_t  RESERVED9;
  __IO uint32_t  EXTTRACECMD;                       /*!< (@ 0x400480FC) External trace buffer command register                 */
  __I  uint32_t  PIOPORCAP0;                        /*!< (@ 0x40048100) POR captured PIO status 0                              */
  __I  uint32_t  PIOPORCAP1;                        /*!< (@ 0x40048104) POR captured PIO status 1                              */
  __I  uint32_t  PIOPORCAP2;                        /*!< (@ 0x40048108) POR captured PIO status 1                              */
  __I  uint32_t  RESERVED10[10];
  __IO uint32_t  IOCONCLKDIV6;                      /*!< (@ 0x40048134) Peripheral clock 6 to the IOCON block for programmable
                                                         glitch filter                                                         */
  __IO uint32_t  IOCONCLKDIV5;                      /*!< (@ 0x40048138) Peripheral clock 5 to the IOCON block for programmable
                                                         glitch filter                                                         */
  __IO uint32_t  IOCONCLKDIV4;                      /*!< (@ 0x4004813C) Peripheral clock 4 to the IOCON block for programmable
                                                         glitch filter                                                         */
  __IO uint32_t  IOCONCLKDIV3;                      /*!< (@ 0x40048140) Peripheral clock 3 to the IOCON block for programmable
                                                         glitch filter                                                         */
  __IO uint32_t  IOCONCLKDIV2;                      /*!< (@ 0x40048144) Peripheral clock 2 to the IOCON block for programmable
                                                         glitch filter                                                         */
  __IO uint32_t  IOCONCLKDIV1;                      /*!< (@ 0x40048148) Peripheral clock 1 to the IOCON block for programmable
                                                         glitch filter                                                         */
  __IO uint32_t  IOCONCLKDIV0;                      /*!< (@ 0x4004814C) Peripheral clock 0 to the IOCON block for programmable
                                                         glitch filter                                                         */
  __IO uint32_t  BODCTRL;                           /*!< (@ 0x40048150) Brown-Out Detect                                       */
  __IO uint32_t  SYSTCKCAL;                         /*!< (@ 0x40048154) System tick counter calibration                        */
  __IO uint32_t  AHBMATRIXPRIO;                     /*!< (@ 0x40048158) AHB matrix priority configuration                      */
  __I  uint32_t  RESERVED11[5];
  __IO uint32_t  IRQLATENCY;                        /*!< (@ 0x40048170) IRQ delay. Allows trade-off between interrupt
                                                         latency and determinism.                                              */
  __IO uint32_t  NMISRC;                            /*!< (@ 0x40048174) NMI Source Control                                     */
  __IO uint32_t  PINTSEL[8];                        /*!< (@ 0x40048178) GPIO Pin Interrupt Select register 0                   */
//  __IO uint32_t  PINTSEL0;                          /*!< (@ 0x40048178) GPIO Pin Interrupt Select register 0                   */
//  __IO uint32_t  PINTSEL1;                          /*!< (@ 0x4004817C) GPIO Pin Interrupt Select register 0                   */
//  __IO uint32_t  PINTSEL2;                          /*!< (@ 0x40048180) GPIO Pin Interrupt Select register 0                   */
//  __IO uint32_t  PINTSEL3;                          /*!< (@ 0x40048184) GPIO Pin Interrupt Select register 0                   */
//  __IO uint32_t  PINTSEL4;                          /*!< (@ 0x40048188) GPIO Pin Interrupt Select register 0                   */
//  __IO uint32_t  PINTSEL5;                          /*!< (@ 0x4004818C) GPIO Pin Interrupt Select register 0                   */
//  __IO uint32_t  PINTSEL6;                          /*!< (@ 0x40048190) GPIO Pin Interrupt Select register 0                   */
//  __IO uint32_t  PINTSEL7;                          /*!< (@ 0x40048194) GPIO Pin Interrupt Select register 0                   */
  __IO uint32_t  USBCLKCTRL;                        /*!< (@ 0x40048198) USB clock control                                      */
  __I  uint32_t  USBCLKST;                          /*!< (@ 0x4004819C) USB clock status                                       */
  __I  uint32_t  RESERVED12[25];
  __IO uint32_t  STARTERP0;                         /*!< (@ 0x40048204) Start logic 0 interrupt wake-up enable register
                                                         0                                                                     */
  __I  uint32_t  RESERVED13[3];
  __IO uint32_t  STARTERP1;                         /*!< (@ 0x40048214) Start logic 1 interrupt wake-up enable register
                                                         1                                                                     */
  __I  uint32_t  RESERVED14[6];
  __IO uint32_t  PDSLEEPCFG;                        /*!< (@ 0x40048230) Power-down states in deep-sleep mode                   */
  __IO uint32_t  PDAWAKECFG;                        /*!< (@ 0x40048234) Power-down states for wake-up from deep-sleep          */
  __IO uint32_t  PDRUNCFG;                          /*!< (@ 0x40048238) Power configuration register                           */
  __I  uint32_t  RESERVED15[110];
  __I  uint32_t  DEVICE_ID;                         /*!< (@ 0x400483F4) Device ID                                              */
} LPC_SYSCON_Type;


/* ================================================================================ */
/* ================                     USART4                     ================ */
/* ================================================================================ */


/**
  * @brief USART4  (USART4)
  */

typedef struct {                                    /*!< (@ 0x4004C000) USART4 Structure                                       */
  __IO uint32_t  CFG;                               /*!< (@ 0x4004C000) USART Configuration register. Basic USART configuration
                                                         settings that typically are not changed during operation.             */
  __IO uint32_t  CTRL;                              /*!< (@ 0x4004C004) USART Control register. USART control settings
                                                         that are more likely to change during operation.                      */
  __IO uint32_t  STAT;                              /*!< (@ 0x4004C008) USART Status register. The complete status value
                                                         can be read here. Writing ones clears some bits in the register.
                                                          Some bits can be cleared by writing a 1 to them.                     */
  __IO uint32_t  INTENSET;                          /*!< (@ 0x4004C00C) Interrupt Enable read and Set register. Contains
                                                         an individual interrupt enable bit for each potential USART
                                                          interrupt. A complete value may be read from this register.
                                                          Writing a 1 to any implemented bit position causes that bit
                                                          to be set.                                                           */
  __O  uint32_t  INTENCLR;                          /*!< (@ 0x4004C010) Interrupt Enable Clear register. Allows clearing
                                                         any combination of bits in the INTENSET register. Writing a
                                                          1 to any implemented bit position causes the corresponding bit
                                                          to be cleared.                                                       */
  __I  uint32_t  RXDATA;                            /*!< (@ 0x4004C014) Receiver Data register. Contains the last character
                                                         received.                                                             */
  __I  uint32_t  RXDATASTAT;                        /*!< (@ 0x4004C018) Receiver Data with Status register. Combines
                                                         the last character received with the current USART receive status.
                                                          Allows DMA or software to recover incoming data and status together. */
  __IO uint32_t  TXDATA;                            /*!< (@ 0x4004C01C) Transmit Data register. Data to be transmitted
                                                         is written here.                                                      */
  __IO uint32_t  BRG;                               /*!< (@ 0x4004C020) Baud Rate Generator register. 16-bit integer
                                                         baud rate divisor value.                                              */
  __I  uint32_t  INTSTAT;                           /*!< (@ 0x4004C024) Interrupt status register. Reflects interrupts
                                                         that are currently enabled.                                           */
} LPC_USART4_Type;


/* ================================================================================ */
/* ================                      GINT0                     ================ */
/* ================================================================================ */


/**
  * @brief GPIO group interrupt 0 (GINT0)
  */

typedef struct {                                    /*!< (@ 0x4005C000) GINT0 Structure                                        */
  __IO uint32_t  CTRL;                              /*!< (@ 0x4005C000) GPIO grouped interrupt control register                */
  __I  uint32_t  RESERVED0[7];
  __IO uint32_t  PORT_POL[3];                       /*!< (@ 0x4005C020) GPIO grouped interrupt port 0 polarity register        */
  __I  uint32_t  RESERVED1[5];
  __IO uint32_t  PORT_ENA[3];                       /*!< (@ 0x4005C040) GPIO grouped interrupt port enable register            */
} LPC_GINT0_Type;


/* ================================================================================ */
/* ================                       USB                      ================ */
/* ================================================================================ */


/**
  * @brief USB device controller (USB)
  */

typedef struct {                                    /*!< (@ 0x40080000) USB Structure                                          */
  __IO uint32_t  DEVCMDSTAT;                        /*!< (@ 0x40080000) USB Device Command/Status register                     */
  __IO uint32_t  INFO;                              /*!< (@ 0x40080004) USB Info register                                      */
  __IO uint32_t  EPLISTSTART;                       /*!< (@ 0x40080008) USB EP Command/Status List start address               */
  __IO uint32_t  DATABUFSTART;                      /*!< (@ 0x4008000C) USB Data buffer start address                          */
  __IO uint32_t  LPM;                               /*!< (@ 0x40080010) Link Power Management register                         */
  __IO uint32_t  EPSKIP;                            /*!< (@ 0x40080014) USB Endpoint skip                                      */
  __IO uint32_t  EPINUSE;                           /*!< (@ 0x40080018) USB Endpoint Buffer in use                             */
  __IO uint32_t  EPBUFCFG;                          /*!< (@ 0x4008001C) USB Endpoint Buffer Configuration register             */
  __IO uint32_t  INTSTAT;                           /*!< (@ 0x40080020) USB interrupt status register                          */
  __IO uint32_t  INTEN;                             /*!< (@ 0x40080024) USB interrupt enable register                          */
  __IO uint32_t  INTSETSTAT;                        /*!< (@ 0x40080028) USB set interrupt status register                      */
  __IO uint32_t  INTROUTING;                        /*!< (@ 0x4008002C) USB interrupt routing register                         */
  __I  uint32_t  RESERVED0;
  __I  uint32_t  EPTOGGLE;                          /*!< (@ 0x40080034) USB Endpoint toggle register                           */
} LPC_USB_Type;


/* ================================================================================ */
/* ================                       CRC                      ================ */
/* ================================================================================ */


/**
  * @brief Cyclic Redundancy Check (CRC) engine (CRC)
  */

typedef struct {                                    /*!< (@ 0x50000000) CRC Structure                                          */
  __IO uint32_t  MODE;                              /*!< (@ 0x50000000) CRC mode register                                      */
  __IO uint32_t  SEED;                              /*!< (@ 0x50000004) CRC seed register                                      */
  
  union {
    __I  uint32_t  SUM;                             /*!< (@ 0x50000008) CRC checksum register                                  */
    __O  uint32_t  WR_DATA_DWORD;                   /*!< (@ 0x50000008) CRC data register                                      */
    __O  uint16_t  WR_DATA_WORD;                    /*!< (@ 0x50000008) CRC data register                                      */
         uint16_t  RESERVED_WORD;                   /*!< (@ 0x5000000A) CRC data register                                      */
    __O  uint8_t   WR_DATA_BYTE;                    /*!< (@ 0x50000008) CRC data register                                      */
         uint8_t   RESERVED_BYTE[3];                /*!< (@ 0x50000009) CRC data register                                      */
  } ;
} LPC_CRC_Type;


/* ================================================================================ */
/* ================                       DMA                      ================ */
/* ================================================================================ */


/**
  * @brief Product name title=Kylin UM Chapter title=KylinDMA controller Modification date=5/13/2013 Major revision=0 Minor revision=1  (DMA)
  */

typedef struct {                                    /*!< (@ 0x50004000) DMA Structure                                          */
  __IO uint32_t  CTRL;                              /*!< (@ 0x50004000) DMA control.                                           */
  __I  uint32_t  INTSTAT;                           /*!< (@ 0x50004004) Interrupt status.                                      */
  __IO uint32_t  SRAMBASE;                          /*!< (@ 0x50004008) SRAM address of the channel configuration table.       */
  __I  uint32_t  RESERVED0[5];
  __IO uint32_t  ENABLESET0;                        /*!< (@ 0x50004020) Channel Enable read and Set for all DMA channels.      */
  __I  uint32_t  RESERVED1;
  __O  uint32_t  ENABLECLR0;                        /*!< (@ 0x50004028) Channel Enable Clear for all DMA channels.             */
  __I  uint32_t  RESERVED2;
  __I  uint32_t  ACTIVE0;                           /*!< (@ 0x50004030) Channel Active status for all DMA channels.            */
  __I  uint32_t  RESERVED3;
  __I  uint32_t  BUSY0;                             /*!< (@ 0x50004038) Channel Busy status for all DMA channels.              */
  __I  uint32_t  RESERVED4;
  __IO uint32_t  ERRINT0;                           /*!< (@ 0x50004040) Error Interrupt status for all DMA channels.           */
  __I  uint32_t  RESERVED5;
  __IO uint32_t  INTENSET0;                         /*!< (@ 0x50004048) Interrupt Enable read and Set for all DMA channels.    */
  __I  uint32_t  RESERVED6;
  __O  uint32_t  INTENCLR0;                         /*!< (@ 0x50004050) Interrupt Enable Clear for all DMA channels.           */
  __I  uint32_t  RESERVED7;
  __IO uint32_t  INTA0;                             /*!< (@ 0x50004058) Interrupt A status for all DMA channels.               */
  __I  uint32_t  RESERVED8;
  __IO uint32_t  INTB0;                             /*!< (@ 0x50004060) Interrupt B status for all DMA channels.               */
  __I  uint32_t  RESERVED9;
  __O  uint32_t  SETVALID0;                         /*!< (@ 0x50004068) Set ValidPending control bits for all DMA channels.    */
  __I  uint32_t  RESERVED10;
  __O  uint32_t  SETTRIG0;                          /*!< (@ 0x50004070) Set Trigger control bits for all DMA channels.         */
  __I  uint32_t  RESERVED11;
  __O  uint32_t  ABORT0;                            /*!< (@ 0x50004078) Channel Abort control for all DMA channels.            */
#if 0
  __I  uint32_t  RESERVED12[225];
  __IO uint32_t  CFG0;                              /*!< (@ 0x50004400) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT0;                          /*!< (@ 0x50004404) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG0;                          /*!< (@ 0x50004408) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED13;
  __IO uint32_t  CFG1;                              /*!< (@ 0x50004410) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT1;                          /*!< (@ 0x50004414) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG1;                          /*!< (@ 0x50004418) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED14;
  __IO uint32_t  CFG2;                              /*!< (@ 0x50004420) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT2;                          /*!< (@ 0x50004424) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG2;                          /*!< (@ 0x50004428) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED15;
  __IO uint32_t  CFG3;                              /*!< (@ 0x50004430) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT3;                          /*!< (@ 0x50004434) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG3;                          /*!< (@ 0x50004438) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED16;
  __IO uint32_t  CFG4;                              /*!< (@ 0x50004440) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT4;                          /*!< (@ 0x50004444) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG4;                          /*!< (@ 0x50004448) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED17;
  __IO uint32_t  CFG5;                              /*!< (@ 0x50004450) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT5;                          /*!< (@ 0x50004454) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG5;                          /*!< (@ 0x50004458) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED18;
  __IO uint32_t  CFG6;                              /*!< (@ 0x50004460) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT6;                          /*!< (@ 0x50004464) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG6;                          /*!< (@ 0x50004468) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED19;
  __IO uint32_t  CFG7;                              /*!< (@ 0x50004470) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT7;                          /*!< (@ 0x50004474) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG7;                          /*!< (@ 0x50004478) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED20;
  __IO uint32_t  CFG8;                              /*!< (@ 0x50004480) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT8;                          /*!< (@ 0x50004484) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG8;                          /*!< (@ 0x50004488) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED21;
  __IO uint32_t  CFG9;                              /*!< (@ 0x50004490) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT9;                          /*!< (@ 0x50004494) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG9;                          /*!< (@ 0x50004498) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED22;
  __IO uint32_t  CFG10;                             /*!< (@ 0x500044A0) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT10;                         /*!< (@ 0x500044A4) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG10;                         /*!< (@ 0x500044A8) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED23;
  __IO uint32_t  CFG11;                             /*!< (@ 0x500044B0) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT11;                         /*!< (@ 0x500044B4) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG11;                         /*!< (@ 0x500044B8) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED24;
  __IO uint32_t  CFG12;                             /*!< (@ 0x500044C0) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT12;                         /*!< (@ 0x500044C4) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG12;                         /*!< (@ 0x500044C8) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED25;
  __IO uint32_t  CFG13;                             /*!< (@ 0x500044D0) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT13;                         /*!< (@ 0x500044D4) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG13;                         /*!< (@ 0x500044D8) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED26;
  __IO uint32_t  CFG14;                             /*!< (@ 0x500044E0) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT14;                         /*!< (@ 0x500044E4) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG14;                         /*!< (@ 0x500044E8) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED27;
  __IO uint32_t  CFG15;                             /*!< (@ 0x500044F0) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT15;                         /*!< (@ 0x500044F4) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG15;                         /*!< (@ 0x500044F8) Transfer configuration register for DMA channel
                                                         0.                                                                    */
#endif
} LPC_DMA_Type;

#if 1
typedef struct
{
  __IO uint32_t  CFG;                               /*!< (@ 0x50004400) Configuration register for DMA channel 0.              */
  __I  uint32_t  CTLSTAT;                           /*!< (@ 0x50004404) Control and status register for DMA channel 0.         */
  __IO uint32_t  XFERCFG;                           /*!< (@ 0x50004408) Transfer configuration register for DMA channel
                                                         0.                                                                    */
  __I  uint32_t  RESERVED;
} LPC_DMA_Channel;
#endif


/* ================================================================================ */
/* ================                      SCT0                      ================ */
/* ================================================================================ */


/**
  * @brief Product name title=Kylin UM Chapter title=KylinState Configurable Timers (SCT0/1) Modification date=5/14/2013 Major revision=0 Minor revision=1  (SCT0)
  */

typedef struct {                                    /*!< (@ 0x5000C000) SCT0 Structure                                         */
  __IO uint32_t  CONFIG;                            /*!< (@ 0x5000C000) SCT configuration register                             */
    union {
      __IO uint32_t CTRL_U;                         /*!< (@ 0x5000C004) SCT control register                                   */
      struct {
        __IO uint16_t CTRL_L;                       /*!< (@ 0x5000C004) SCT control low register                               */
        __IO uint16_t CTRL_H;                       /*!< (@ 0x5000C006) SCT control high register                              */
      };
    };
    union {
      __IO uint32_t LIMIT_U;                        /*!< (@ 0x5000C008) SCT limit register                                     */
      struct {
        __IO uint16_t LIMIT_L;                      /*!< (@ 0x5000C008) SCT limit low register                                 */
        __IO uint16_t LIMIT_H;                      /*!< (@ 0x5000C00A) SCT limit high register                                */
      };
    };
    union {
      __IO uint32_t HALT_U;                         /*!< (@ 0x5000C00C) SCT halt condition register                            */
      struct {
        __IO uint16_t HALT_L;                       /*!< (@ 0x5000C00C) SCT halt condition low register                        */
        __IO uint16_t HALT_H;                       /*!< (@ 0x5000C00E) SCT halt condition high register                       */
      };
    };
    union {
      __IO uint32_t STOP_U;                         /*!< (@ 0x5000C010) SCT stop condition register                            */
      struct {
        __IO uint16_t STOP_L;                       /*!< (@ 0x5000C010) SCT stop condition low register                        */
        __IO uint16_t STOP_H;                       /*!< (@ 0x5000C012) SCT stop condition high register                       */
      };
    };
    union {
      __IO uint32_t START_U;                        /*!< (@ 0x5000C014) SCT start condition register                           */
      struct {
        __IO uint16_t START_L;                      /*!< (@ 0x5000C014) SCT start condition low register                       */
        __IO uint16_t START_H;                      /*!< (@ 0x5000C016) SCT start condition high register                      */
      };
    };
  __I  uint32_t  RESERVED0[10];
    union {
      __IO uint32_t COUNT_U;                        /*!< (@ 0x5000C040) SCT counter register                                   */
      struct {
        __IO uint16_t COUNT_L;                      /*!< (@ 0x5000C040) SCT counter register                                   */
        __IO uint16_t COUNT_H;                      /*!< (@ 0x5000C042) SCT counter register                                   */
      };
    };
    union {
      __IO uint32_t STATE_U;                        /*!< (@ 0x5000C044) SCT state register                                     */
      struct {
        __IO uint16_t STATE_L;                      /*!< (@ 0x5000C044) SCT state low register                                 */
        __IO uint16_t STATE_H;                      /*!< (@ 0x5000C046) SCT state high register                                */
      };
    };
  __I  uint32_t  INPUT;                             /*!< (@ 0x5000C048) SCT input register                                     */
    union {
      __IO uint32_t REGMODE_U;                      /*!< (@ 0x5000C04C) SCT match/capture registers mode register              */
      struct {
        __IO uint16_t REGMODE_L;                    /*!< (@ 0x5000C04C) SCT match/capture registers mode low register          */
        __IO uint16_t REGMODE_H;                    /*!< (@ 0x5000C04E) SCT match/capture registers mode high register         */
      };
    };
  __IO uint32_t  OUTPUT;                            /*!< (@ 0x5000C050) SCT output register                                    */
  __IO uint32_t  OUTPUTDIRCTRL;                     /*!< (@ 0x5000C054) SCT output counter direction control register          */
  __IO uint32_t  RES;                               /*!< (@ 0x5000C058) SCT conflict resolution register                       */
  __IO uint32_t  DMAREQ0;                           /*!< (@ 0x5000C05C) SCT DMA request 0 register                             */
  __IO uint32_t  DMAREQ1;                           /*!< (@ 0x5000C060) SCT DMA request 1 register                             */
  __I  uint32_t  RESERVED1[35];
  __IO uint32_t  EVEN;                              /*!< (@ 0x5000C0F0) SCT event enable register                              */
  __IO uint32_t  EVFLAG;                            /*!< (@ 0x5000C0F4) SCT event flag register                                */
  __IO uint32_t  CONEN;                             /*!< (@ 0x5000C0F8) SCT conflict enable register                           */
  __IO uint32_t  CONFLAG;                           /*!< (@ 0x5000C0FC) SCT conflict flag register                             */
    union {
      __IO union {                                  /*!< (@ 0x5000C100) SCT match value register of match channels 0 to 4      */
        uint32_t U;                                 /*       SCTMATCH[i].U  Unified 32-bit register                            */
        struct {
          uint16_t L;                               /*       SCTMATCH[i].L  Access to L value                                  */
          uint16_t H;                               /*       SCTMATCH[i].H  Access to H value                                  */
        };
      } MATCH[5];
      __I union {                                   /*!< (@ 0x5000C100) SCT capture register of capture channel 0 to 4         */
        uint32_t U;                                 /*       SCTCAP[i].U  Unified 32-bit register                              */
        struct {
          uint16_t L;                               /*       SCTCAP[i].L  Access to H value                                    */
          uint16_t H;                               /*       SCTCAP[i].H  Access to H value                                    */
        };
      } CAP[5];
    };
  __I  uint32_t  RESERVED2[59];
    union {
      __IO union {                                  /*!< (@ 0x5000C200) SCT match reload value register 0 to 4                 */
        uint32_t U;                                 /*       SCTMATCHREL[i].U  Unified 32-bit register                         */
        struct {
          uint16_t L;                               /*       SCTMATCHREL[i].L  Access to L value                               */
          uint16_t H;                               /*       SCTMATCHREL[i].H  Access to H value                               */
        };
      } MATCHREL[5];
      __IO union {                                  /*!< (@ 0x5000C200) SCT capture control register 0 to 4                    */
        uint32_t U;                                 /*       SCTCAPCTRL[i].U  Unified 32-bit register                          */
        struct {
          uint16_t L;                               /*       SCTCAPCTRL[i].L  Access to H value                                */
          uint16_t H;                               /*       SCTCAPCTRL[i].H  Access to H value                                */
        };
      } CAPCTRL[5];
    };
  __I  uint32_t  RESERVED3[59];
  __IO struct {
    uint32_t STATE;                                 /*!< (@ 0x5000C300) SCT event state register 0 to 5                        */
    uint32_t CTRL;                                  /*!< (@ 0x5000C304) SCT event control register 0 to 5                      */
  } EVENT[6];
  __I  uint32_t  RESERVED4[116];
  __IO struct {
    uint32_t SET;                                   /*!< (@ 0x5000C500) SCT output set register 0 to 3                         */
    uint32_t CLR;                                   /*!< (@ 0x5000C504) SCT output clear register 0 to 3                       */
  } OUT[4];
} LPC_SCT0_Type;


/* ================================================================================ */
/* ================                    GPIO_PORT                   ================ */
/* ================================================================================ */


/**
  * @brief General Purpose I/O (GPIO)  (GPIO_PORT)
  */

typedef struct {                                    /*!< (@ 0xA0000000) GPIO_PORT Structure                                    */
  __IO uint8_t   B[88];                             /*!< (@ 0xA0000000) Byte pin registers                                     */
  __I  uint32_t  RESERVED0[42];
  __IO uint32_t  W[88];                             /*!< (@ 0xA0000100) Word pin registers                                     */
  __I  uint32_t  RESERVED1[1896];
  __IO uint32_t  DIR[3];                            /*!< (@ 0xA0002000) Port Direction registers                               */
  __I  uint32_t  RESERVED2[29];
  __IO uint32_t  MASK[3];                           /*!< (@ 0xA0002080) Port Mask register                                     */
  __I  uint32_t  RESERVED3[29];
  __IO uint32_t  PIN[3];                            /*!< (@ 0xA0002100) Port pin register                                      */
  __I  uint32_t  RESERVED4[29];
  __IO uint32_t  MPIN[3];                           /*!< (@ 0xA0002180) Masked port register                                   */
  __I  uint32_t  RESERVED5[29];
  __IO uint32_t  SET[3];                            /*!< (@ 0xA0002200) Write: Set port register Read: port output bits
                                                                                                                               */
  __I  uint32_t  RESERVED6[29];
  __O  uint32_t  CLR[3];                            /*!< (@ 0xA0002280) Clear port                                             */
  __I  uint32_t  RESERVED7[29];
  __O  uint32_t  NOT[3];                            /*!< (@ 0xA0002300) Toggle port                                            */
} LPC_GPIO_PORT_Type;


/* ================================================================================ */
/* ================                      PINT                      ================ */
/* ================================================================================ */


/**
  * @brief Pin interruptand pattern match (PINT)  (PINT)
  */

typedef struct {                                    /*!< (@ 0xA0004000) PINT Structure                                         */
  __IO uint32_t  ISEL;                              /*!< (@ 0xA0004000) Pin Interrupt Mode register                            */
  __IO uint32_t  IENR;                              /*!< (@ 0xA0004004) Pin interrupt level or rising edge interrupt
                                                         enable register                                                       */
  __O  uint32_t  SIENR;                             /*!< (@ 0xA0004008) Pin interrupt level or rising edge interrupt
                                                         set register                                                          */
  __O  uint32_t  CIENR;                             /*!< (@ 0xA000400C) Pin interrupt level (rising edge interrupt) clear
                                                         register                                                              */
  __IO uint32_t  IENF;                              /*!< (@ 0xA0004010) Pin interrupt active level or falling edge interrupt
                                                         enable register                                                       */
  __O  uint32_t  SIENF;                             /*!< (@ 0xA0004014) Pin interrupt active level or falling edge interrupt
                                                         set register                                                          */
  __O  uint32_t  CIENF;                             /*!< (@ 0xA0004018) Pin interrupt active level or falling edge interrupt
                                                         clear register                                                        */
  __IO uint32_t  RISE;                              /*!< (@ 0xA000401C) Pin interrupt rising edge register                     */
  __IO uint32_t  FALL;                              /*!< (@ 0xA0004020) Pin interrupt falling edge register                    */
  __IO uint32_t  IST;                               /*!< (@ 0xA0004024) Pin interrupt status register                          */
  __IO uint32_t  PMCTRL;                            /*!< (@ 0xA0004028) Pattern match interrupt control register               */
  __IO uint32_t  PMSRC;                             /*!< (@ 0xA000402C) Pattern match interrupt bit-slice source register      */
  __IO uint32_t  PMCFG;                             /*!< (@ 0xA0004030) Pattern match interrupt bit slice configuration
                                                         register                                                              */
} LPC_PINT_Type;


/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined(__TASKING__)
  #pragma warning restore
#else
  #warning Not supported compiler type
#endif




/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define LPC_I2C0_BASE                   0x40000000UL
#define LPC_WWDT_BASE                   0x40004000UL
#define LPC_USART0_BASE                 0x40008000UL
#define LPC_CT16B0_BASE                 0x4000C000UL
#define LPC_CT16B1_BASE                 0x40010000UL
#define LPC_CT32B0_BASE                 0x40014000UL
#define LPC_CT32B1_BASE                 0x40018000UL
#define LPC_ADC_BASE                    0x4001C000UL
#define LPC_I2C1_BASE                   0x40020000UL
#define LPC_RTC_BASE                    0x40024000UL
#define LPC_DMATRIGMUX_BASE             0x40028000UL
#define LPC_PMU_BASE                    0x40038000UL
#define LPC_FLASHCTRL_BASE              0x4003C000UL
#define LPC_SSP0_BASE                   0x40040000UL
#define LPC_IOCON_BASE                  0x40044000UL
#define LPC_SYSCON_BASE                 0x40048000UL
#define LPC_USART4_BASE                 0x4004C000UL
#define LPC_SSP1_BASE                   0x40058000UL
#define LPC_GINT0_BASE                  0x4005C000UL
#define LPC_GINT1_BASE                  0x40060000UL
#define LPC_USART1_BASE                 0x4006C000UL
#define LPC_USART2_BASE                 0x40070000UL
#define LPC_USART3_BASE                 0x40074000UL
#define LPC_USB_BASE                    0x40080000UL
#define LPC_CRC_BASE                    0x50000000UL
#define LPC_DMA_BASE                    0x50004000UL
#if 1
#define LPC_DMA_CH_BASE                 0x50004400UL
#endif
#define LPC_SCT0_BASE                   0x5000C000UL
#define LPC_SCT1_BASE                   0x5000E000UL
#define LPC_GPIO_PORT_BASE              0xA0000000UL
#define LPC_PINT_BASE                   0xA0004000UL


/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define LPC_I2C0                        ((LPC_I2C0_Type           *) LPC_I2C0_BASE)
#define LPC_WWDT                        ((LPC_WWDT_Type           *) LPC_WWDT_BASE)
#define LPC_USART0                      ((LPC_USART0_Type         *) LPC_USART0_BASE)
#define LPC_CT16B0                      ((LPC_CT16B0_Type         *) LPC_CT16B0_BASE)
#define LPC_CT16B1                      ((LPC_CT16B1_Type         *) LPC_CT16B1_BASE)
#define LPC_CT32B0                      ((LPC_CT32B0_Type         *) LPC_CT32B0_BASE)
#define LPC_CT32B1                      ((LPC_CT32B1_Type         *) LPC_CT32B1_BASE)
#define LPC_ADC                         ((LPC_ADC_Type            *) LPC_ADC_BASE)
#define LPC_I2C1                        ((LPC_I2C0_Type           *) LPC_I2C1_BASE)
#define LPC_RTC                         ((LPC_RTC_Type            *) LPC_RTC_BASE)
#define LPC_DMATRIGMUX                  ((LPC_DMATRIGMUX_Type     *) LPC_DMATRIGMUX_BASE)
#define LPC_PMU                         ((LPC_PMU_Type            *) LPC_PMU_BASE)
#define LPC_FLASHCTRL                   ((LPC_FLASHCTRL_Type      *) LPC_FLASHCTRL_BASE)
#define LPC_SSP0                        ((LPC_SSP0_Type           *) LPC_SSP0_BASE)
#define LPC_IOCON                       ((LPC_IOCON_Type          *) LPC_IOCON_BASE)
#define LPC_SYSCON                      ((LPC_SYSCON_Type         *) LPC_SYSCON_BASE)
#define LPC_USART4                      ((LPC_USART4_Type         *) LPC_USART4_BASE)
#define LPC_SSP1                        ((LPC_SSP0_Type           *) LPC_SSP1_BASE)
#define LPC_GINT0                       ((LPC_GINT0_Type          *) LPC_GINT0_BASE)
#define LPC_GINT1                       ((LPC_GINT0_Type          *) LPC_GINT1_BASE)
#define LPC_USART1                      ((LPC_USART4_Type         *) LPC_USART1_BASE)
#define LPC_USART2                      ((LPC_USART4_Type         *) LPC_USART2_BASE)
#define LPC_USART3                      ((LPC_USART4_Type         *) LPC_USART3_BASE)
#define LPC_USB                         ((LPC_USB_Type            *) LPC_USB_BASE)
#define LPC_CRC                         ((LPC_CRC_Type            *) LPC_CRC_BASE)
#define LPC_DMA                         ((LPC_DMA_Type            *) LPC_DMA_BASE)
#if 1
#define LPC_DMA_CH(i)                   ((LPC_DMA_Channel         *) (LPC_DMA_CH_BASE + ((i)<<4)))
#endif
#define LPC_SCT0                        ((LPC_SCT0_Type           *) LPC_SCT0_BASE)
#define LPC_SCT1                        ((LPC_SCT0_Type           *) LPC_SCT1_BASE)
#define LPC_GPIO_PORT                   ((LPC_GPIO_PORT_Type      *) LPC_GPIO_PORT_BASE)
#define LPC_PINT                        ((LPC_PINT_Type           *) LPC_PINT_BASE)


/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group LPC11U6x */
/** @} */ /* End of group (null) */

#ifdef __cplusplus
}
#endif


#endif  /* __LPC11U6X_H__ */

