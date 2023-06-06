/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file Debounce.c
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

#include "Debounce.h"
#include "appgen.h"

void DebounceInit (S_Switch *Descriptor, bool InitValue, uint8_t DebounceMax)
{
    Descriptor->DebounceState    = DebounceWaitChange;
    Descriptor->DebounceMaximal  = DebounceMax;
    Descriptor->DebounceCounter  = 1;
    Descriptor->Bits.Rise        = false;
    Descriptor->Bits.Fall        = false;
    Descriptor->Bits.ValueNow    = InitValue;
    Descriptor->Bits.ValueOld    = InitValue;
}

/**
 * @param Descriptor : Switch structure to apply debounce
 * @param InputValue : Input value to debounce
 */
void Debounce(S_Switch *Descriptor, bool InputValue)
{   
    /* DEBUG LED for measurements */
    BSP_LEDToggle(DEBUG_LED_DEBOUNCE);
    
   /* Depending on current state */
   switch ( Descriptor->DebounceState )
   {
        case DebounceWaitChange :
            if ( InputValue != Descriptor->Bits.ValueOld )
            {
                Descriptor->Bits.ValueOld = InputValue;
                Descriptor->DebounceState = DebounceWaitStable;
            }
            break;

        case DebounceWaitStable :
            if( InputValue == Descriptor->Bits.ValueOld )
            {               
                if( Descriptor->DebounceCounter >= Descriptor->DebounceMaximal )
                {
                    Descriptor->Bits.ValueNow = InputValue;
                    Descriptor->Bits.Rise = Descriptor->Bits.ValueNow;
                    Descriptor->Bits.Fall = !Descriptor->Bits.ValueNow;
                    
                    Descriptor->DebounceCounter = 0;
                    Descriptor->DebounceState = DebounceWaitChange;
                }
                else
                {
                    Descriptor->DebounceCounter++;
                }
            }            
            else
            {                
                Descriptor->DebounceCounter = 0;
            }
            break;
    }
 }

//  DebounceGetInput  fourni l'état du switch aprés anti-rebond
bool DebounceGetInput (S_Switch *Descriptor) {
   return Descriptor->Bits.ValueNow;
}

//  DebounceIsPressed    true indique que l'on vient de presser la touche
bool DebounceRised (S_Switch *Descriptor) {
   return Descriptor->Bits.Rise;
}

//  DebounceIsReleased   true indique que l'on vient de relacher la touche
bool DebounceFalled (S_Switch *Descriptor) {
   return Descriptor->Bits.Fall;
}

//  DebounceClearPressed    annule indication de pression sur la touche
void DebounceClearRised  (S_Switch *Descriptor) {
   Descriptor->Bits.Rise = 0;
}

//  DebounceClearReleased   annule indication de relachement de la touche
void DebounceClearFalled  (S_Switch *Descriptor) {
   Descriptor->Bits.Fall = 0;
}