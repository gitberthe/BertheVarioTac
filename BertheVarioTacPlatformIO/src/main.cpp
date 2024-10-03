////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 03/10/2024
///

#include "BertheVarioTac.h"
CGlobalVar g_GlobalVar ;

//#include "AudioOutputI2S.h"
//#include "WiFiMulti.h"
//#include "Audio.h"
#define BUZZER_PIN 26
#define BUZZER_CHANNEL 0

//#include "SoundSrv/DacESP32.h"

//DacESP32 dac1(GPIO_NUM_26);
//#define DAC_CH2 26

/*#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include "soc/rtc.h"*/
#include "soc/dac_channel.h"
#include "driver/dac.h"

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
//#ifdef BMP180_PRESS
 g_GlobalVar.m_BMP180Pression.InitBMP180() ;
//#endif

#ifndef NO_GPS_DEBUG
 // init port serie GPS
 g_GlobalVar.InitGps() ;

 // lancement tache gps
 g_GlobalVar.LanceTacheGps(true) ;
#endif // NO_GPS_DEBUG

// lancement tache de calcul du cap magnetique
//g_GlobalVar.m_Mpu9250.LancerTacheCalculCapMag() ;

// lancement tache beep
g_GlobalVar.LanceTacheVarioCapBeep() ;

// raz de l'ecran
g_GlobalVar.m_Screen.ScreenRaz(false) ;

// lancement tache touch
g_GlobalVar.m_Screen.LancerTacheTouch() ;

/*//fin :
//pinMode(22, OUTPUT);
int freq = 2000; // 3000 Hz
const int ledChannel = 0;
const int resolution = 8; // Résolution de 8 bits
const int ledPin = 26;
int dutyCycle = 255/2 ;

//ledcWrite(ledChannel, dutyCycle);
//ledcSetup(ledChannel, freq, resolution);
ledcAttachPin(ledPin, ledChannel);
ledcWriteTone( ledChannel, freq ) ;
//pinMode(22, OUTPUT);

delay( 300 ) ;

freq = 8000 ;
//ledcSetup(ledChannel, freq, resolution);
ledcWriteTone( ledChannel, freq ) ;

delay( 300 ) ;

ledcDetachPin( ledPin ) ;*/
//#define EXAMPLE_ADC_ATTEN                   ADC_ATTEN_DB_12
/*int8_t offset = 0 ;
dac_cw_scale_t scale = DAC_CW_SCALE_8 ;
dac_output_enable(DAC_CHANNEL_2);


dac_cw_config_t config ;
config.en_ch = DAC_GPIO26_CHANNEL ;
config.scale = DAC_CW_SCALE_8 ;
config.freq = 2000 ;
dac_cw_generator_config( & config ) ;
dac_cw_generator_enable() ;


delay( 2000 ) ;
dac_cw_generator_disable() ;*/
}


////////////////////////////////////////////////////////////////////////////////
/// \brief boucle sans fin
void loop()
{
  /*for (int deg = 0; deg < 360; deg = deg + 1) {
    // Calculate sine and write to DAC
    dacWrite(26, int(128 + 64 * sin(deg * PI / 180)));
    }*/
/*int A = 2 ;
dacWrite(26,A);
delay(1) ;
dacWrite(26,0);
delay(1) ;
return ;*/
/*float freq = 2000 ;
int A = 10 ;
int count_d = 0 ;
while ( true ) ;
    {
    count_d++ ;
    if ( count_d%2  )
        {
        dacWrite(26, A ) ;
        //delay( 1000*(int)(1./freq)/2 ) ;
        }
    else
        {
        dacWrite(26, 0 ) ;
        //delay( 1000*(int)(1./freq)/2 ) ;
        }
    }*/

/*
//tone(BUZZER_PIN, NOTE_C4, 500, BUZZER_CHANNEL);
CText2Screen Aff ;
Aff.SetPos(  15 , 15 , 3 , 'm' ) ;
float Alti_0 = 0 ;
float Alti_1 = 0 ;
float Vz = 0 ;
while ( true )
    {
    //g_tft.setCursor(20,50);

    if ( g_GlobalVar.m_BMP180Pression.m_InitOk )
        {
        //g_tft.print( "ok" );
        Serial.print("ok");
        }
    else
        {
        //g_tft.print( "fail" );
        Serial.print("fail");
        }
    float coef = 0.4 ;
    Alti_0 = g_GlobalVar.m_BMP180Pression.GetAltiMetres() ;
    Vz = (1.-coef) * Vz + coef * (Alti_0 - Alti_1) ;
    Alti_1 = Alti_0 ;
    //float VzRound = ((int)(Vz*100.)) / 100. ;
    char Tmp[25] ;
    sprintf(Tmp,"%5.2f %4.1f", Alti_0 , Vz ) ;
    Aff.Affiche(Tmp) ;
//    if ( etat%2 )
//        digitalWrite(22, HIGH) ;
//    else
//        digitalWrite(22, LOW) ;

    delay ( 1000 ) ;
    }
return ; */

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

// si on n'est pas en ecran 0
//if ( g_GlobalVar.m_EtatAuto != ECRAN_0_Vz )
//    continue ;

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
