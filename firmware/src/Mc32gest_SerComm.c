// Mc32Gest_SerComm.C
// fonction d'émission et de réception des message
// transmis en USB CDC
// Canevas TP4 SLO2 2015-2015


#include "app.h"
#include "Mc32gest_SerComm.h"
#include "MenuGen.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define APP_READ_BUFFER_SIZE 64

//uint8_t APP_MAKE_BUFFER_DMA_READY readBuffer [APP_READ_BUFFER_SIZE];

//char valSignal;
APP_DATA appData;

// Fonction de reception  d'un  message
// Met à jour les paramètres du generateur a partir du message recu
// Format du message
//  !S=TF=2000A=10000O=+5000W=0#
//  !S=TF=2000A=10000O=-5000W=1#


bool GetMessage(uint8_t *USBReadBuffer, S_ParamGen *pParam, bool *SaveTodo)
{
     //Déclaration des varaible
     char Signal;
     char F[5], A[6], O[6], W[2];
 
     //Lecture du buffer et stockage des valeurs en string
     sscanf((char*)USBReadBuffer, "!S=%1s"
                                   "F=%4s"
                                   "A=%5s"
                                   "O=%5s"
                                   "W=%1s" , &Signal, F, A, O, W); 
    
     switch (Signal)
     {
        case 'T':
            pParam->Forme = SignalTriangle;   
            break;
        case 'S':
            pParam->Forme = SignalSinus;   
            break;
        case 'C':
            pParam->Forme = SignalCarre;   
            break;
        case 'D':
            pParam->Forme = SignalDentDeScie;   
            break;
        default:
            break;   
     }
     
    //Conversion string => int 
    pParam->Frequence = atoi(F);
    pParam->Amplitude = atoi(A);
    pParam->Offset = atoi(O);
    
    *SaveTodo = atoi(W);
    
     return *SaveTodo;
     
} // GetMessage


// Fonction d'envoi d'un  message
// Rempli le tampon d'émission pour USB en fonction des paramètres du générateur
// Format du message
// !S=TF=2000A=10000O=+5000WP=0#
// !S=TF=2000A=10000O=+5000WP=1#    // ack sauvegarde
    
void SendMessage(uint8_t *USBSendBuffer, S_ParamGen *pParam, bool Saved)
{
    //Déclaration des variables
    char forme = 'S';
    char valWP[2];

    switch (pParam->Forme)
    {
        case SignalTriangle:
            forme = 'T';
            break;
        case SignalSinus:
            forme = 'S';
            break;
        case SignalCarre:
            forme = 'C';
            break;
        case SignalDentDeScie:
            forme = 'D';
            break;
        default:
            break;
    }
    //Sauvegarde
    if (Saved)
    {
        sprintf(valWP, "%d", 1);
    }
    else
    {
        sprintf(valWP, "%d", 0);
    }

    //Ecriture et stockage en chaîne de caractère
    sprintf((char*)USBSendBuffer, "!S=%cF=%04dA=%05dO=%04dWP=%s#", forme, pParam->Frequence, pParam->Amplitude, pParam->Offset, valWP);
    
} // SendMessage
