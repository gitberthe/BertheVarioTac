////////////////////////////////////////////////////////////////////////////////
/// \file BertheVarioTac.h
///
/// \brief Include global du projet de vario tactile
///
/// \date creation     : 20/09/2024
/// \date modification : 19/10/2024
///


//////////////////////
// includes globaux //
//////////////////////
#include <esp32-hal-i2c.h>
#include <esp32-hal-dac.h>
#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <FS.h>
#include <ESPFMfGK.h>
#include <SFE_BMP180.h>
#include <MS5611.h>
#include <freertos/queue.h>
#include <soc/dac_channel.h>
#include <driver/dac.h>
#include <DFRobot_QMC5883.h>

#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>

//////////////////////////
// define pour le debug //
//////////////////////////
//#define GPS_DEBUG
//#define PERFMON_DEBUG
//#define NO_GPS_DEBUG
//#define MAG_DEBUG
//#define BRIGHT_DEBUG

/////////////////////////////
// define pour le materiel //
/////////////////////////////
//#define BMP180_PRESS
#define MS5611_PRESS

//////////////////////////
// includes du logiciel //
//////////////////////////
#include "GlobalVar/CMutex.h"
#include "Screen/CTouchButtons.h"
#include "Screen/CLGFX.h"
#include "Screen/CAutoPages.h"
#include "Screen/CText2Screen.h"
#include "Screen/CScreen.h"
#include "GlobalVar/CFileHistoVol.h"
#include "GlobalVar/CNbSatDelay.h"
#include "GlobalVar/CConfigFile.h"
#include "Calculateur/CSortArray.h"
#include "Calculateur/CTerrainsConnu.h"
#include "Calculateur/CFinSol.h"
#include "SDCard/CSDCard.h"
#include "Perfmon/esp32_perfmon.h"
#include "Geometrie/CVecteur2D.h"
#include "Geometrie/CDroite2D.h"
#include "Geometrie/CVecteur3D.h"
#include "Geometrie/CDroite3D.h"
#include "Geometrie/CMatrice3D.h"
#include "Geometrie/Segments.h"
#include "ZonesAeriennes/CDate.h"
#include "ZonesAeriennes/CZoneAer.h"
#include "ZonesAeriennes/CDistFront.h"
#include "ZonesAeriennes/CPolygone.h"
#include "ZonesAeriennes/CZonesAerAll.h"
#include "ZonesAeriennes/CZonesFch.h"
#include "Gps/CTrame.h"
#include "Gps/CIgc.h"
#include "Gps/CStabGps.h"
#include "Gps/CFinDeVol.h"
#include "Gps/CGps.h"
#include "Gps/CHgt2Agl.h"
#include "Gps/CPileVit.h"
#include "BMP180/CBMP180Pression.h"
#include "MS5611/CMS5611Pression.h"
#include "FileMgr/FileMgr.h"
#include "VarioCapBeep/CVarioCapBeep.h"
#include "VarioCapBeep/CSoundSvr.h"
#include "QMC5883L/CQMC5883Mag.h"

////////////////////////
// definition des pin //
////////////////////////

// GPS
//#define pinGpsRXD  (27)
//#define pinGpsRXD  (21)
//#define pinGpsRXD  (35)
#define pinGpsRXD  (1)
#define pinGpsTXD  (-1)

#define VoltageInPin  (35)
#define BrightnessPin (34)

// SDCARD
#define SDCARD_CS_PIN   5
#define SDCARD_MOSI_PIN 23
#define SDCARD_MISO_PIN 19
#define SDCARD_SCK_PIN  18

// SDA SCL
#define VARIO_SDA_PIN 27
//#define VARIO_SDA_PIN 21
//#define VARIO_SDA_PIN 35
#define VARIO_SCL_PIN 22

////////////////
// constantes //
////////////////

#define UnMileEnMetres         (1852.)
#define T_PI                   (3.1415926535)

///////////
// macro //
///////////

#define FABS(x)     fabsf(x)
#define SQRT(x)     sqrtf(x)
#define SIN(x)      sinf(x)
#define COS(x)      cosf(x)
#define ACOS(x)     acosf(x)
#define POW(x,y)    powf(x,y)
#define FMOD(x,y)   fmodf(x,y)
#define FLOOR(x)    floorf(x)
#define MODF(x,y)   modff(x,y)

#define MIN( a , b )   (((a)<(b)) ? (a) : (b))
#define MAX( a , b )   (((a)>(b)) ? (a) : (b))

///////////////////////////
// taches plus affichage //
///////////////////////////

// tache touch, basse priorite core 0 necessaire ou plantage
/*#define TOUCH_NUM_TASK       0
#define TOUCH_STACK_SIZE     3000
#define TOUCH_PRIORITY       5
#define TOUCH_CORE           0*/

/////////////////////////////////////////////
// taches plus temps reel                  //
// FreeRtOs utilise le core 0 a 75% a vide //
/////////////////////////////////////////////

// scan capteur et calcul Vz priorite tres haute non interruptible
#define VARIOCAP_NUM_TASK   0
#define VARIOCAP_STACK_SIZE 1500
#define VARIOCAP_PRIORITY   20
#define VARIOCAP_CORE       1

// son vario fonction Vz priorite moyenne
#define VARIOBEEP_NUM_TASK   1
#define VARIOBEEP_STACK_SIZE 1000
#define VARIOBEEP_PRIORITY   15
#define VARIOBEEP_CORE       1

// serveur de son, priorite moyenne
#define SOUNDSVR_NUM_TASK   2
#define SOUNDSVR_STACK_SIZE 1500
#define SOUNDSVR_PRIORITY   10
#define SOUNDSVR_CORE       1

// acquisition gps, priorite haute non interruptible
#define SERIAL_NUM_TASK         3
#define SERIAL_GPS_STACK_SIZE   2800
#define SERIAL_GPS_PRIORITY     19
#define SERIAL_GPS_CORE         1

// ecriture igc, priorite moyenne
#define IGC_NUM_TASK    4
#define IGC_STACK_SIZE  3000
#define IGC_PRIORITY    10
#define IGC_CORE        1

// temps de vol / histo, basse priorite
#define TEMPS_NUM_TASK   5
#define TEMPS_STACK_SIZE 3000
#define TEMPS_PRIORITY   5
#define TEMPS_CORE       1

// relance pour cause faux depart de vol, tache fugitive.
#define RELANCE_IGC_NUM_TASK   6
#define RELANCE_IGC_STACK_SIZE 3000
#define RELANCE_IGC_PRIORITY   0
#define RELANCE_IGC_CORE       1

// tache de mesure d'occupation des cores 0 et 1
#define PERF_MON_NUM_TASK   7
#define PERF_MON_STACK_SIZE 800
#define PERF_MON_PRIORITY   1
#define PERF_MON_CORE       1

// nombre total de taches
#define SIZE_TASK 8


//////////////////////////
// includes du logiciel //
//////////////////////////
#include "GlobalVar/CNumTaskArr.h"
#include "GlobalVar/CGlobalVar.h"


////////////////////////////
// fonctions et variables //
////////////////////////////
void AfficheEcranDebut() ;
void WifiInitFileMgr() ;
void WifiInitOta() ;
void WifiOtaHandle() ;

extern WebServer g_server_ota ;
extern char g_NumVersion[] ;
