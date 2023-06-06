
/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file I2C_Seeprom.c
 * @brief 
 * 
 * Used to save datas into external EEPROM by I2C
 * 
 * @author Miguel Santos
 * 
 * @date 11.05.2023
 * 
 ******************************************************************************/

#ifndef I2C_Seeprom_H
#define I2C_Seeprom_H

#include <stdint.h>
#include <stdbool.h>
#include "Mc32_I2cUtilCCS.h"

/* MCP79411 defines */
#define MCP79411_EEPROM_R   0xAF    // MCP79411 address for read
#define MCP79411_EEPROM_W   0xAE    // MCP79411 address for write
#define MCP79411_EEPROM_BEG 0x00    // Start adress for EEPROM
#define MCP79411_EEPROM_END 0x7F    // End adress for EEPROM
#define MCP79411_EEPROM_MAX 8       // Maximum of bytes to write at once

void I2C_InitMCP79411(void);
void I2C_WriteSEEPROM(void *SrcData, uint8_t EEpromAddr, uint8_t NbBytes);
void I2C_ReadSEEPROM(void *DstData, uint8_t EEpromAddr, uint8_t NbBytes);

#endif
