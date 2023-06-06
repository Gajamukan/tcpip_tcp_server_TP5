/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file Debounce.h
 * @brief 
 * 
 * Used to handle multiple kinds of switch (buttons, etc...)
 * Should be called cyclically fast enough (for every switch)
 * 
 * @author Miguel Santos
 * @author Ali Zoubir
 * 
 * @date 14.03.2023
 * 
 ******************************************************************************/

#ifndef MC32DEBOUNCE2_H
#define MC32DEBOUNCE2_H

#include <stdbool.h>
#include <stdint.h>

// définitions des types qui seront utilisés dans cette application

// Etats du système antirebond
typedef enum {
    DebounceWaitChange = 0,
    DebounceWaitStable = 1,
} E_DebounceState;

typedef struct {
   bool  Rise        :1; // Pressed flag
   bool  Fall        :1; // Released flag
   bool  ValueNow    :1; // Final value
   bool  ValueOld    :1; // Previous value
 }S_Bits;

typedef struct {    
           uint8_t          DebounceCounter;    // Count number of debounce
           uint8_t          DebounceMaximal;    // Maximal value of counter
           E_DebounceState  DebounceState;      // Actual state
           S_Bits           Bits;               // Flags and values
}S_Switch;

/**
 * Initialize structure of a signal to debounce
 * 
 * @param Descriptor :
 * Adress of the structure to init
 * 
 * @param DebounceMaximal :
 * Number of time to read signal for debounce
 * 
 */
void DebounceInit (S_Switch *Descriptor, bool InitValue, uint8_t DebounceMax);

void Debounce (S_Switch *Descriptor, bool InputValue);

void DebounceReset(S_Switch *Descriptor);

/**
 * @param pDescriptor :
 * Descriptor of signal to debounce
 * 
 * @return bool :
 *  
 */
bool DebounceGetInput (S_Switch *pDescriptor);

//  DebounceIsPressed    true indique que l'on vient de presser la touche

/**
 * @param pDescriptor
 * @return 
 */
bool DebounceRised (S_Switch *pDescriptor);

//  DebounceIsReleased   true indique que l'on vient de relacher la touche
bool DebounceFalled (S_Switch *pDescriptor);

//  DebounceClearPressed    annule indication de pression sur la touche
void DebounceClearRised  (S_Switch *pDescriptor);

//  DebounceClearReleased   annule indication de relachement de la touche
void DebounceClearFalled  (S_Switch *pDescriptor);

#endif