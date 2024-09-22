////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 22/09/2024
///

#include "BertheVarioTac.h"
CGlobalVar g_GlobalVar ;

////////////////////////////////////////////////////////////////////////////////
/// \brief setup ESP32-2432S028
void setup()
{
Serial.begin(115200);

// tache de mise a jour % cpu
perfmon_start() ;

// initialisation de l'ecran tactile
g_tft.InitScreen() ;

// init sdcard
g_GlobalVar.InitSDCard() ;

// Init BUS I2C
g_GlobalVar.InitI2C() ;

// lecture fichier de configuration
g_GlobalVar.m_Config.LectureFichier() ;

// lecture fichier terrains
g_GlobalVar.m_TerrainArr.LireFichierTerrains() ;

// lecture fichier zones aeriennes
g_GlobalVar.m_ZonesAerAll.LectureFichiers() ;

// init port serie GPS
g_GlobalVar.InitGps() ;

// init capteur de pression
//g_GlobalVar.m_BMP180Pression.InitBMP180() ;

// lancement tache gps
g_GlobalVar.LanceTacheGps(true) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief boucle sans fin
void loop()
{
/*while ( true )
    {
    g_tft.setTextSize(6) ;
    g_tft.setCursor( 10 , 10 ) ;
    g_tft.setTextColor(TFT_RED) ;

    if ( g_GlobalVar.m_BMP180Pression.m_InitOk )
        g_tft.print( "ok" );
    else
        g_tft.print( "fail" );

    } */
static int count = 0 ;

// traitement de touch pad
g_GlobalVar.m_Screen.HandleTouchScreen() ;

// attente ecran
g_tft.waitDisplay() ;

// affichage des boutons tactiles
g_GlobalVar.m_Screen.AfficheButtons() ;

// a 100hz
delay( 10 );

// a 3 hz
count++ ;
if ( count%33 )
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
