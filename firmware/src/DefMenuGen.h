/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file DefMenuGen.h
 * @brief 
 * 
 * Definition used for the generator menu
 * 
 * @author Miguel Santos
 * @author Ali Zoubir
 * 
 * @date 14.03.2023
 * 
 ******************************************************************************/

#ifndef DEFMENUGEN_H
#define DEFMENUGEN_H

#include <stdint.h>

/* Abracadabra ! */
/* Used to verify integrity of memory messages */
#define MAGIC 0x123455AA

/* Parameters extreme values*/
#define FREQ_MIN 20
#define FREQ_TYP 1000
#define FREQ_MAX 2000

#define AMP_MIN 0
#define AMP_TYP 5000
#define AMP_MAX 10000
#define AMP_DELTA (AMP_MAX - AMP_MIN)

#define OFFSET_MIN -5000
#define OFFSET_MAX 5000
#define OFFSET_DELTA (OFFSET_MAX - OFFSET_MIN)

/* Display space used by parameters */
#define NAME_LENGTH 11
#define HEAD_LENGTH 7
#define VALUE_LENGTH 6
#define UNIT_LENGTH 3

/* X Position of strings on the LCD */
#define HEAD_POS 2
#define EQUAL_POS (HEAD_POS + HEAD_LENGTH)
#define VALUE_POS (EQUAL_POS + 2)
#define UNIT_POS (VALUE_POS + VALUE_LENGTH + 1)

/**/
typedef enum  {
    SignalSinus,
    SignalTriangle,
    SignalDentDeScie,
    SignalCarre
} E_SignalShapes;

/* Generator parameters structure */
typedef struct {
    uint32_t Magic;
    E_SignalShapes shape;
    union{
        struct{
            uint8_t name;
            int16_t frequency;
            int16_t amplitude;
            int16_t offset;
        }data;    
        int16_t key[4];        
    }parameters;
} S_ParamGen;

#endif
