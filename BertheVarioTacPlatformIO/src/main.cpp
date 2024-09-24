////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 24/09/2024
///

#include "BertheVarioTac.h"
CGlobalVar g_GlobalVar ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Affiche le nom et le firmware
void AfficheEcranDebut()
{
g_GlobalVar.m_Screen.ScreenRaz() ;
g_tft.setTextColor(TFT_RED) ;
g_tft.setTextSize(2) ;
g_tft.setCursor( 35 , 50 ) ;
g_tft.print("BertheVarioTac");
g_tft.setTextColor(TFT_WHITE) ;
g_tft.setTextSize(3) ;
g_tft.setCursor( 40 , 100 ) ;
g_tft.print(g_NumVersion);
}

//#include "AudioOutputI2S.h"
//#include "WiFiMulti.h"
//#include "Audio.h"
#define BUZZER_PIN 26
#define BUZZER_CHANNEL 0

////////////////////////////////////////////////////////////////////////////////
/// \brief setup ESP32-2432S028
void setup()
{
Serial.begin(115200);

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
/// \brief init du Wifi
void WifiInit()
{
// creation fichier de validation zones
g_GlobalVar.m_ZonesAerAll.Valid() ;

// liberation memoire
g_GlobalVar.m_ZonesAerAll.DeleteAll() ;

// affichage
g_GlobalVar.m_Screen.ScreenRaz() ;
g_tft.setCursor( 10 , 50 ) ;
g_tft.print( "Connect to wifi" ) ;

// connection wifi
WiFi.begin( g_GlobalVar.m_Config.m_Ssid.c_str(), g_GlobalVar.m_Config.m_Passwd.c_str() );
while (WiFi.status() != WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
    }

#ifdef HTTP_DEBUG
Serial.println("");
Serial.print("Connected to ");
Serial.println(WIFI_SSID);
Serial.print("IP address: ");
Serial.println(WiFi.localIP());
#endif

// adresse wifi
char buf[50];
sprintf(buf, "IP: %d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
g_GlobalVar.m_Screen.ScreenRaz() ;
g_tft.setCursor( 10 , 50 ) ;
g_tft.print( buf ) ;
g_tft.setCursor( 50 , 100 ) ;
g_tft.print( "touch/reboot" ) ;

// creation init file manager
g_pfilemgr = new ESPFMfGK( 8080 ) ;

addFileSystems();
setupFilemanager();

// arret des autres taches
g_GlobalVar.StopAll() ;
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
    if ( !(count%100) )
        {
        g_GlobalVar.m_Screen.HandleTouchScreen() ;
        if ( g_GlobalVar.m_Screen.IsPressed() )
            g_GlobalVar.Reboot() ;
        }

    return ;
    }

// traitement de touch pad
g_GlobalVar.m_Screen.HandleTouchScreen() ;

// attente ecran
g_tft.waitDisplay() ;

// affichage des boutons tactiles
g_GlobalVar.m_Screen.AfficheButtons() ;

// a 10hz, 50hz ça plante l'affichage avec le Gps
delay( 100 );

// a 2 hz
count++ ;
if ( count%5 )
    return ;

//tft.sleep() ;
//tft.powerSaveOn() ;
//g_tft.startWrite(); // bug avec carte SD si utilisation
//tft.beginTransaction();

// sequencement des pages
g_GlobalVar.m_Screen.SequencementPages() ;

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
