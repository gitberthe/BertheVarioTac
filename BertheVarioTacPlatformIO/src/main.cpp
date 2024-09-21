////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#include "BertheVarioTac.h"

CGlobalVar g_GlobalVar ;

////////////////////////////////////////////////////////////////////////////////
/// \brief setup ESP32-2432S028
void setup()
{
Serial.begin(115200);

// initialisation de l'ecran tactile
g_tft.InitScreen() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief boucle sans fin
void loop()
{
static int count = 0 ;

// traitement de touch pad
g_GlobalVar.m_Screen.HandleTouchScreen() ;

// attente ecran
g_tft.waitDisplay() ;

// affichage des boutons tactiles
g_GlobalVar.m_Screen.AfficheButtons() ;

// a 20hz
delay( 5 );


// a 4 hz
count++ ;
if ( count%50 )
    return ;

//tft.sleep() ;
//tft.powerSaveOn() ;
g_tft.startWrite();
//tft.beginTransaction();

// sequencement des pages
g_GlobalVar.m_Screen.SequencementPages() ;

// raz de l'etat des boutons tactiles
g_GlobalVar.m_Screen.RazButtons() ;

g_tft.endWrite();

//tft.clearClipRect()  ;
//tft.fillRect(0, 0,240,320,TFT_WHITE);
//tft.clearDisplay() ;
//tft.powerSaveOff() ;
//tft.wakeup() ;
//tft.endTransaction();
}
