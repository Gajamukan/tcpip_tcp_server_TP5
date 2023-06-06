/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file Inputs.h
 * @brief 
 * 
 * File used to access all inputs peripherals
 * on ETML-ES PIC32 kit
 * 
 * @author Miguel Santos
 * @author Ali Zoubir
 * 
 * @date 14.03.2023
 * 
 ******************************************************************************/

#ifndef INPUTS_H
#define INPUTS_H

#include <stdbool.h>
#include <stdint.h>

#define PEC12_A_INPUT PLIB_PORTS_PinGet(PORTS_ID_0, PEC12_A_PORT, PEC12_A_BIT)
#define PEC12_B_INPUT PLIB_PORTS_PinGet(PORTS_ID_0, PEC12_B_PORT, PEC12_B_BIT)
#define PEC12_PB_INPUT PLIB_PORTS_PinGet(PORTS_ID_0, PEC12_PB_PORT, PEC12_PB_BIT)
#define BTN_SAVE_INPUT BSP_SwitchStateGet(BSP_SWITCH_1)

#define PEC12_MAX_DEBOUNCE 4
#define BTN_MAX_DEBOUNCE 3
#define INACTIVITY_TIME 5000

/* Rotary encoder PEC12 descriptor */
typedef struct {
            uint8_t Inc                 :1; // Increment event
            uint8_t Dec                 :1; // Decrement event
            uint8_t OK                  :1; // OK action event
            uint8_t ESC                 :1; // ESC action event
            uint8_t NoActivity          :1; // No activity indicator
            uint16_t PressDuration        ; // Duration of PB press
            uint16_t InactivityDuration   ; // Duration of inactivity
} S_Pec12;

// Dans le sens horaire CW:
//     __________                      ________________
// B:            |____________________|
//     ___________________                       _________
// A:                     |_____________________|                    
// Dans le sens anti-horaire CCW:
//     ____________________                      _________
// B:                      |____________________|
//     __________                       __________________
// A:            |_____________________|        


void InputsInit ( void );
void ScanAllInputs ( void );
void ScanBtnSave ( void );
bool BtnSaveValue ( void );
bool BtnSavePress ( void );
void BtnSaveClearPress ( void );
bool BtnSaveRelease ( void );
void BtnSaveClearRelease ( void );


/**
 * @param ValA
 * @param ValB
 * @param ValPB
 */
void ScanPec12 (bool ValA, bool ValB, bool ValPB);

/**
 * Initialise Pec12 descriptor
 */
void Pec12Init (void);

/**
 * Pec12 increment flag
 * @return True when increment detected
 */
bool Pec12IsInc (void);

/**
 * Pec12 decrement flag
 * @return True when decrement detected
 */
bool Pec12IsDec (void);

/**
 * Action OK flag
 * @return True when condition reached for OK action
 */
bool Pec12IsOK (void);

bool Pec12IsESC (void);

bool Pec12NoActivity(void);

void Pec12ClearInc (void);

void Pec12ClearDec (void);

void Pec12ClearOK   (void);

void Pec12ClearESC   (void);

void Pec12ClearInactivity   (void);


#endif
