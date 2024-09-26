////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 26/09/2024
///

#include "BertheVarioTac.h"
CGlobalVar g_GlobalVar ;

//#include "AudioOutputI2S.h"
//#include "WiFiMulti.h"
//#include "Audio.h"
#define BUZZER_PIN 26
#define BUZZER_CHANNEL 0

////////////////////////////////////////////////////////////////////////////////
/// \brief setup ESP32-2432S028
void setup()
{
//Serial.begin(115200);

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

// init port serie GPS
g_GlobalVar.InitGps() ;

// init capteur de pression
//g_GlobalVar.m_BMP180Pression.InitBMP180() ;

// lancement tache gps
g_GlobalVar.LanceTacheGps(true) ;

// lancement tache de calcul du cap magnetique
//g_GlobalVar.m_Mpu9250.LancerTacheCalculCapMag() ;

// lancement tache beep
//g_GlobalVar.LanceTacheVarioBeep() ;

g_GlobalVar.m_Screen.ScreenRaz() ;

// lancement tache touch
g_GlobalVar.m_Screen.LancerTacheTouch() ;

/*const int freq = 4000; // 5000 Hz
const int ledChannel = 0;
const int resolution = 8; // Résolution de 8 bits
const int ledPin = 26;
int dutyCycle = 255/2 ;

ledcWrite(ledChannel, dutyCycle);
ledcSetup(ledChannel, freq, resolution);
ledcAttachPin(ledPin, ledChannel);*/
}

////////////////////////////////////////////////////////////////////////////////
/// \brief boucle sans fin
void loop()
{
//tone(BUZZER_PIN, NOTE_C4, 500, BUZZER_CHANNEL);
/* while ( true )
    {
    g_GlobalVar.m_Screen.ScreenRaz() ;

    if ( g_GlobalVar.m_BMP180Pression.m_InitOk )
        g_tft.print( "ok" );
    else
        g_tft.print( "fail" );

    } // */

static int count = 0 ;
static bool WifiSetup = true ;

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
    count++ ;
    if ( !(count%1000) )
        {
        if ( g_GlobalVar.m_Screen.IsCenterPressed() )
            g_GlobalVar.Reboot() ;
        }

    return ;
    }


// 2hz
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
 g_GlobalVar.m_Screen.AfficheButtons() ;
 g_GlobalVar.m_Screen.SequencementPages() ;
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
delay( 500 ) ;

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
