////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 07/10/2024
///

#include "BertheVarioTac.h"
CGlobalVar g_GlobalVar ;

////////////////////////////////////////////////////////////////////////////////
/// \brief setup ESP32-2432S028
void setup()
{
//Serial.begin(115200);
#ifdef NO_GPS_DEBUG
 Serial.begin(9600);
 Serial.println("Debugage on" ) ;
#endif
//goto fin ;

// lancement tache son
g_GlobalVar.LanceTacheSound() ;
delay(500) ;
g_GlobalVar.BeepOk() ;
g_GlobalVar.beeper( SOUND_DELAY_ONLY , 100 ) ;
g_GlobalVar.BeepOk() ;

// tache de mise a jour % cpu
perfmon_start() ;

// init pin d'entree de tension
g_GlobalVar.InitBattery() ;

// initialisation de l'ecran tactile
g_tft.InitScreen() ;

// affichage numero de firmware
AfficheEcranDebut() ;

// init sdcard
g_GlobalVar.InitSDCard() ;

// Init BUS I2C
g_GlobalVar.InitI2C() ;

// lecture fichier terrains
g_GlobalVar.m_TerrainArr.LireFichierTerrains() ;

// lecture fichier zones aeriennes
g_GlobalVar.m_ZonesAerAll.LectureFichiers() ;

// lecture fichier de configuration
g_GlobalVar.m_Config.LectureFichier() ;

// reglage luminosite car initscreen en premier obligatoire pour SDCard
g_tft.setBrightness( g_GlobalVar.m_Config.m_luminosite );

// init capteur de pression
g_GlobalVar.m_pCapteurPression->InitCapteur() ;

// init capteur magnetique
g_GlobalVar.m_QMC5883Mag.InitMagnetique() ;

#ifndef NO_GPS_DEBUG
 // init port serie GPS
 g_GlobalVar.InitGps() ;

 // lancement tache gps
 g_GlobalVar.LanceTacheGps(true) ;
#else
 g_GlobalVar.m_DureeVolMin = ATTENTE_MESSAGE_GPS ;
#endif // NO_GPS_DEBUG

// lancement tache beep
g_GlobalVar.LanceTacheVarioCapBeep() ;

// raz de l'ecran
g_GlobalVar.m_Screen.ScreenRaz(false) ;

// lancement tache touch
g_GlobalVar.m_Screen.LancerTacheTouch() ;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief boucle sans fin
void loop()
{
//g_GlobalVar.m_QMC5883Mag.ScanI2C() ;

// variables
static int count = 0 ;
static bool WifiSetup = true ;

count++ ;

// si mode wifi
if ( g_GlobalVar.m_ModeHttp )
    {
    // si init wifi
    if ( WifiSetup )
        {
        WifiSetup = false ;
        WifiInit() ;
        }

    g_pfilemgr->handleClient();

    // si ecran pressé on reboot
    if ( !(count%1000) )
        {
        if ( g_GlobalVar.m_Screen.IsCenterPressed() )
            g_GlobalVar.Reboot() ;
        }

    return ;
    }

// 4hz
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
 g_GlobalVar.m_Screen.AfficheButtons() ;
 g_GlobalVar.m_Screen.SequencementPages() ;
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
delay( 250 ) ;

// 1hz
if ( count%4 )
    return ;

#ifdef NO_GPS_DEBUG
 Serial.println("mode debug") ;
#endif

// calcul des zones aeriennes
g_GlobalVar.m_ZonesAerAll.CalcZone() ;

// calcul terrain le plus proche
g_GlobalVar.m_TerrainArr.CalcTerrainPlusProche() ;


//tft.sleep() ;
//tft.powerSaveOn() ;
//g_tft.startWrite(); // bug avec carte SD si utilisation
//tft.beginTransaction();

// sequencement des pages
//g_GlobalVar.m_Screen.SequencementPages() ;

// raz de l'etat des boutons tactiles
//g_GlobalVar.m_Screen.RazButtons() ;

//g_tft.endWrite(); // bug avec carte SD si utilisation

//tft.clearClipRect()  ;
//tft.fillRect(0, 0,240,320,TFT_WHITE);
//tft.clearDisplay() ;
//tft.powerSaveOff() ;
//tft.wakeup() ;
//tft.endTransaction();
}
