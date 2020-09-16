// Copyright (c) 2009 Rowley Associates Limited.
//
// This file may be distributed under the terms of the License Agreement
// provided with this software.
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#ifndef liblpc1000_h
#define liblpc1000_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <libmem.h>

#define IAP_CMD_PREPARE_SECTORS_FOR_WRITE_OPERATION 50
#define IAP_CMD_COPY_RAM_TO_FLASH 51
#define IAP_CMD_ERASE_SECTORS 52
#define IAP_CMD_BLANK_CHECK_SECTORS 53
#define IAP_CMD_READ_PART_ID 54
#define IAP_CMD_READ_BOOT_CODE_VERSION 55
#define IAP_CMD_COMPARE 56

#define IAP_CMD_SUCCESS 0
#define IAP_INVALID_COMMAND 1
#define IAP_SRC_ADDR_ERROR 2
#define IAP_DST_ADDR_ERROR 3
#define IAP_SRC_ADDR_NOT_MAPPED 4
#define IAP_DST_ADDR_NOT_MAPPED 5
#define IAP_COUNT_ERROR 6
#define IAP_INVALID_SECTOR 7
#define IAP_SECTOR_NOT_BLANK 8
#define IAP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION 9
#define IAP_COMPARE_ERROR 10
#define IAP_BUSY 11 

/*! \brief Call an LPC1000 IAP command.
 *  \param cmd The IAP command to call.
 *  \param p0 IAP function parameter 0.
 *  \param p1 IAP function parameter 1.
 *  \param p2 IAP function parameter 2.
 *  \param p3 IAP function parameter 3.
 *  \param r0 Pointer to location to store result0 or NULL if not required.
 *  \param r1 Pointer to location to store result1 or NULL if not required.
 *  \param r2 Pointer to location to store result2 or NULL if not required.
 *  \param r3 Pointer to location to store result3 or NULL if not required.
 *  \return Result0
 */
uint32_t liblpc1000_iap_command(uint32_t cmd, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t *r0, uint32_t *r1, uint32_t *r2, uint32_t *r3);

/*  \brief Get the internal FLASH geometry information.
 *  \return A pointer to the internal FLASH geometry.
 */
const libmem_geometry_t *liblpc1000_get_internal_flash_geometry(void);

/*  \brief Get the size of the internal FLASH memory.
 *  \return The FLASH memory size in bytes.
 */
size_t liblpc1000_get_internal_flash_size(void);

/* \brief Get the size of the local internal SRAM memory.
 * \return The SRAM memory size in bytes.
 */
size_t liblpc1000_get_local_sram_size(void);

/* \brief Get the size of the AHB SRAM memory.
 * \return The SRAM memory size in bytes.
 */
size_t liblpc1000_get_ahb_sram_size(void);

/*! \brief Return the current CPU clock frequency on an LPC17xx device.
    \deprecated Use \a liblpc1000_lpc17xx_get_cclk
    \param fosc The oscillator frequency in Hz.
    \return The CPU clock (CCLK) frequency in Hz.
 */
uint32_t liblpc1000_get_cclk(uint32_t fosc);

/*! \brief Return the current main clock frequency on an LPC11xx device.
    \param fosc The oscillator frequency in Hz.
    \return The main clock frequency in Hz.
 */
uint32_t liblpc1000_lpc11xx_get_main_clk(uint32_t fosc);

/*! \brief Return the current CPU clock frequency on an LPC11xx device.
    \param fosc The oscillator frequency in Hz.
    \return The CPU clock (CCLK) frequency in Hz.
 */
uint32_t liblpc1000_lpc11xx_get_cclk(uint32_t fosc);

/*! \brief Return the current main clock frequency on an LPC13xx device.
    \param fosc The oscillator frequency in Hz.
    \return The main clock frequency in Hz.
 */
uint32_t liblpc1000_lpc13xx_get_main_clk(uint32_t fosc);

/*! \brief Return the current CPU clock frequency on an LPC13xx device.
    \param fosc The oscillator frequency in Hz.
    \return The CPU clock (CCLK) frequency in Hz.
 */
uint32_t liblpc1000_lpc13xx_get_cclk(uint32_t fosc);

/*! \brief Return the current CPU clock frequency on an LPC17xx device.
    \param fosc The oscillator frequency in Hz.
    \return The CPU clock (CCLK) frequency in Hz.
 */
uint32_t liblpc1000_lpc17xx_get_cclk(uint32_t fosc);

/*! \brief Return the current CPU clock frequency on an LPC177x_8x device.
    \param fosc The oscillator frequency in Hz.
    \return The CPU clock (CCLK) frequency in Hz.
 */
uint32_t liblpc1000_lpc177x_8x_get_cclk(uint32_t fosc);

/*! \brief Configure the PLL so that CCLK is running at a specific frequency on an LPC17xx device.
    \deprecated Use \a liblpc1000_lpc17xx_configure_pll
    \param fosc The oscillator frequency.
    \param cclk The desired CCLK frequency.
    \return The achieved CCLK frequency or zero if the configuration is not possible.
 */
uint32_t liblpc1000_configure_pll(uint32_t fosc, uint32_t cclk);

/*! \brief Configure the PLL so that CCLK is running at a specific frequency on an LPC13xx device.
    \param fosc The oscillator frequency.
    \param cclk The desired CCLK frequency.
    \return The achieved CCLK frequency or zero if the configuration is not possible.
 */
uint32_t liblpc1000_lpc13xx_configure_pll(uint32_t fosc, uint32_t cclk);

/*! \brief Configure the PLL so that CCLK is running at a specific frequency on an LPC17xx device.
    \param fosc The oscillator frequency.
    \param cclk The desired CCLK frequency.
    \return The achieved CCLK frequency or zero if the configuration is not possible.
 */
uint32_t liblpc1000_lpc17xx_configure_pll(uint32_t fosc, uint32_t cclk);

/*! \brief Register a driver for the LPC1000 internal FLASH with the libmem library.
 *  \param h Pointer to libmem driver handle.
 *  \param cclk_frequency The the freqency of CCLK in Hz.
 *  \return The LIBMEM status result.
 */
int liblpc1000_register_libmem_driver(libmem_driver_handle_t *h, uint32_t cclk_frequency_hz);

/*! \brief Register a driver for the LPC1000 internal FLASH with the libmem library.
 *  \deprecated Use \a liblpc1000_register_libmem_driver_ex2
 *  \param h Pointer to libmem driver handle.
 *  \param size The size of the FLASH.
 *  \param geometry The FLASH geomeatry
 *  \param write_buffer Pointer to the write buffer.
 *  \param write_buffer_size The size in bytes of the write buffer pointed to by \a write_buffer. This can be 256, 512, 1024 or 4096 bytes.
 *  \param cclk_frequency The the freqency of CCLK in Hz.
 *  \return The LIBMEM status result.
 */
#define liblpc1000_register_libmem_driver_ex(h, size, geometry, write_buffer, write_buffer_size, cclk_frequency_hz) liblpc1000_register_libmem_driver_ex2(h, size, geometry, write_buffer, write_buffer_size, (cclk_frequency_hz) / 1000)

/*! \brief Register a driver for the LPC1000 internal FLASH with the libmem library.
 *  You should use this version of the driver register function when memory is at a premium,
 *  it allows you to specify the FLASH size and geometry and therefore saves the overhead
 *  of the FLASH detection code. It also allows you to specify the location and size of
 *  the write buffer to use when writing to FLASH.
 *  \param h Pointer to libmem driver handle.
 *  \param size The size of the FLASH.
 *  \param geometry The FLASH geomeatry
 *  \param write_buffer Pointer to the write buffer.
 *  \param write_buffer_size The size in bytes of the write buffer pointed to by \a write_buffer. This can be 256, 512, 1024 or 4096 bytes.
 *  \param cclk_frequency The the freqency of CCLK in kHz.
 *  \return The LIBMEM status result.
 */
int liblpc1000_register_libmem_driver_ex2(libmem_driver_handle_t *h, size_t size, const libmem_geometry_t *geometry, uint8_t *write_buffer, uint32_t write_buffer_size, uint32_t cclk_frequency_khz);

#ifdef __cplusplus
}
#endif

#endif

