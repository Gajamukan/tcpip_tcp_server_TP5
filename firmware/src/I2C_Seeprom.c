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

#include "I2C_Seeprom.h"

// Definitions du bus (pour mesures)
// #define I2C-SCK  SCL2/RA2      PORTAbits.RA2   pin 58
// #define I2C-SDA  SDa2/RA3      PORTAbits.RA3   pin 59

/**
 * I2C_InitMCP79411
 * Init communication with I2C to MCP79411
 */
void I2C_InitMCP79411(void)
{
   bool Fast = true;
   i2c_init( Fast );
}

/**
 * I2C_WriteSEEPROM
 * 
 * Write an array of bytes by I2C to MCP79411 EEPROM
 * 
 * @param SrcData : Bytes array to be transmitted by I2C
 * @param EEpromAddr : Starting adress to store datas
 * @param NbBytes : Number of bytes to be stored
 */
void I2C_WriteSEEPROM(void *SrcData, uint8_t EEpromAddr, uint8_t NbBytes)
{
    uint8_t i_data;
    bool ack; 
    
    /* A maximal of 8 bytes can be transmitted at once
     * If more, a stop and a new start are needed */
    
    for(i_data = 0; i_data < NbBytes; i_data++)
    {
        if( ( i_data % MCP79411_EEPROM_MAX ) == 0 )
        {
            do{
                i2c_start();                        // START
                ack = i2c_write(MCP79411_EEPROM_W); // Control byte
            }while(!ack);
            
            i2c_write(EEpromAddr + i_data); // Adress
        }
        
        i2c_write( *((uint8_t*)SrcData + i_data ) );    // Data
        
        if( ( i_data % MCP79411_EEPROM_MAX ) == (MCP79411_EEPROM_MAX - 1) )
        {
            i2c_stop();                     // STOP
        }
    }
}

// Lecture d'un bloc dans l'EEPROM du MCP79411
void I2C_ReadSEEPROM(void *DstData, uint8_t EEpromAddr, uint8_t NbBytes)
{
    uint8_t i_data;
    bool ack = false;
    
    do{
        i2c_start();                        // START
        ack = i2c_write(MCP79411_EEPROM_W); // Control byte
    }while(!ack);
    
    i2c_write(EEpromAddr);          // Adress
    
    ack = false;
    
    do{
        i2c_start();                        // START
        ack = i2c_write(MCP79411_EEPROM_R); // Control byte
    }while(!ack);
    
    for(i_data = 0; i_data < NbBytes - 1; i_data++)
    {
        *((uint8_t*)DstData + i_data ) = i2c_read(true);
    }
    
    *((uint8_t*)DstData + i_data ) = i2c_read(false);
    
    i2c_stop();
}
   





 



