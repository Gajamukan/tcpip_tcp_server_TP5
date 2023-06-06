// Mc32Gest_SerComm.C
// fonction d'émission et de réception des message
// transmis en USB CDC
// Canevas TP4 SLO2 2015-2015


#include "app.h"
#include "Mc32gest_SerComm.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Fonction de reception  d'un  message
// Met à jour les paramètres du generateur a partir du message recu
// Format du message
//  !S=TF=1000A=10000O=+5000W=0#
//  !S=PF=2000A=10000O=-5000W=1#


bool GetMessage(uint8_t *USBReadBuffer, S_ParamGen *pParam, bool *SaveTodo)
{
    char signal[2];
    char frequency[5];
    char amplitude[6];
    char offset[6];
    char write[2];
    bool valid_data = true;
    S_ParamGen checkParam = *pParam;
    
    sscanf((char*)USBReadBuffer,    "!S=%s"
                                    "F=%s"
                                    "A=%s"
                                    "O=%s"
                                    "W=%s#"
    ,signal ,frequency ,amplitude ,offset ,write );
    
    switch(signal[0])
    {
        case 'S':
            checkParam.parameters.key[0] = 0;
            break;
        case 'T':
            checkParam.parameters.key[0] = 1;
            break;
        case 'D':
            checkParam.parameters.key[0] = 2;
            break;
        case 'C':
            checkParam.parameters.key[0] = 3;
            break;
        default:
            valid_data = false;
            break;
    }
    
    checkParam.parameters.key[1] = atoi(frequency);
    if(checkParam.parameters.key[1] < FREQ_MIN || checkParam.parameters.key[1] > FREQ_MAX)
        valid_data = false;
    
    checkParam.parameters.key[2] = atoi(amplitude);
    if(checkParam.parameters.key[2] < AMP_MIN || checkParam.parameters.key[2] > AMP_MAX)
        valid_data = false;
    
    checkParam.parameters.key[3] = atoi(offset);
    if(checkParam.parameters.key[3] < OFFSET_MIN || checkParam.parameters.key[3] > OFFSET_MAX)
        valid_data = false;
    
    if(valid_data)
    {
        *SaveTodo = atoi(write);
        *pParam = checkParam;
    }

    return valid_data;
}

// Fonction d'envoi d'un  message
// Rempli le tampon d'émission pour USB en fonction des paramètres du générateur
// Format du message
// !S=TF=2000A=10000O=+5000WP=0#
// !S=TF=2000A=10000O=+5000WP=1#    // ack sauvegarde
void SendMessage(uint8_t *USBSendBuffer, S_ParamGen *pParam, bool Saved )
{
    char signal;
    int16_t frequency;
    int16_t amplitude;
    int16_t offset;
    
    switch(pParam->parameters.key[0])
    {
        case 0:
            signal = 'S';
            break;
        case 1:
            signal = 'T';
            break;
        case 2:
            signal = 'D';
            break;
        case 3:
            signal = 'C';
            break;
        default:
            break;
    }
    frequency = pParam->parameters.key[1];
    amplitude = pParam->parameters.key[2];
    offset = pParam->parameters.key[3];
    
    sprintf((char*)USBSendBuffer,   "!S=%c"
                                    "F=%d"
                                    "A=%d"
                                    "O=%d"
                                    "WP=%d#"
    ,signal ,frequency ,amplitude ,offset ,Saved );
   
} // SendMessage
