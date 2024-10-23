////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 16/10/2024
///

#include "BertheVarioTac.h"

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

// affaichage vbat
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

// reglage luminosite
g_tft.setBrightness( g_GlobalVar.GetBrightness() );

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
//g_GlobalVar.m_Screen.LancerTacheTouch() ;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief boucle sans fin
void loop()
{
//g_GlobalVar.m_QMC5883Mag.ScanI2C() ;

// variables
static int count_calcul = 0 ;
static int count_10hz = 0 ;
static int count_5hz = 0 ;
static int count_2_5hz = 0 ;
static bool WifiSetupFileMgr = true ;
static bool WifiSetupOta = true ;

// compteur 10 hz
count_10hz++ ;

////////////////////////
// si mode wifi file mgr
if ( g_GlobalVar.m_ModeHttpFileMgr )
    {
    // si init wifi
    if ( WifiSetupFileMgr )
        {
        WifiSetupFileMgr = false ;
        WifiInitFileMgr() ;
        }

    g_pfilemgr->handleClient();

    // si ecran pressé on reboot
    if ( !(count_10hz%1000) )
        {
        // traitement de touch pad
        g_GlobalVar.m_Screen.HandleTouchScreen() ;

        // scan les boutons tactiles
        g_GlobalVar.m_Screen.HandleButtons() ;

        if ( g_GlobalVar.m_Screen.IsCenterPressed() )
            g_GlobalVar.Reboot() ;
        }

    return ;
    }

////////////////////
// si mode wifi ota
if ( g_GlobalVar.m_ModeHttpOta )
    {
    // si init wifi
    if ( WifiSetupOta )
        {
        WifiSetupOta = false ;
        WifiInitOta() ;
        }

    WifiOtaHandle() ;

    // si ecran pressé on reboot
    if ( !(count_10hz%1000) )
        {
        // traitement de touch pad
        g_GlobalVar.m_Screen.HandleTouchScreen() ;

        // scan les boutons tactiles
        g_GlobalVar.m_Screen.HandleButtons() ;

        if ( g_GlobalVar.m_Screen.IsCenterPressed() )
            g_GlobalVar.Reboot() ;
        }

    return ;
    }

/////////////////
// ecran de debut
if( (millis()-g_GlobalVar.m_temps_debut)/1000 < 7 )
    {
    AfficheEcranDebut() ;
    delay( 1000 ) ;
    return ;
    }

////////
// 10 hz
delay( 100 ) ;

// traitement de touch pad
g_GlobalVar.m_Screen.HandleTouchScreen() ;

// scan les boutons tactiles
g_GlobalVar.m_Screen.HandleButtons() ;

///////
// 5hz
if ( count_10hz%2 )
    return ;

// compteur 5hz
count_5hz++ ;

// gestion des boutons
g_GlobalVar.m_Screen.AfficheButtons() ;

////////
// 2.5hz
if ( count_5hz%2 )
    return ;

// compteur 2.5 hz
count_2_5hz++ ;

// gestion des pages
g_GlobalVar.m_Screen.SequencementPages() ;

/////////
// 1.25hz
if ( count_2_5hz%2 )
    return ;

// reglage luminosite
g_tft.setBrightness( g_GlobalVar.GetBrightness() );

#ifdef NO_GPS_DEBUG
 static int count_debug = 0 ;
 Serial.print( count_debug++) ;
 Serial.println(" mode debug") ;
#endif

// une fois sur 2
if ( (count_calcul++)%2 )
    // calcul des zones aeriennes
    g_GlobalVar.m_ZonesAerAll.CalcZone() ;
else
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
