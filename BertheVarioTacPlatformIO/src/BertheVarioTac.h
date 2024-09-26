////////////////////////////////////////////////////////////////////////////////
/// \file BertheVarioTac.h
///
/// \brief Include global du projet de vario tactile
///
/// \date creation     : 20/09/2024
/// \date modification : 26/09/2024
///


//////////////////////
// includes globaux //
//////////////////////
#include <esp32-hal-i2c.h>
#include <esp32-hal-dac.h>
#include <Wire.h>
#include <Arduino.h>
//#include <ESP32Servo.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <FS.h>
#include <ESPFMfGK.h>
//#include <driver/dac.h>

#include <math.h>
#include <map>
#include <vector>
#include <algorithm>

///////////////////
// pour le debug //
///////////////////
//#define GPS_DEBUG

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
#include "FileMgr/FileMgr.h"

////////////////////////
// definition des pin //
////////////////////////

// GPS
//#define pinGpsRXD  (27)
//#define pinGpsRXD  (21)
//#define pinGpsRXD  (35)
#define pinGpsRXD  (1)
#define pinGpsTXD  (-1)

#define VoltageInPin (35)

// SDCARD
#define SDCARD_CS_PIN   5
#define SDCARD_MOSI_PIN 23
#define SDCARD_MISO_PIN 19
#define SDCARD_SCK_PIN  18

// SDA SCL
#define VARIO_SDA_PIN 27
#define VARIO_SCL_PIN 22

////////////////
// constantes //
////////////////

#define UnMileEnMetres         (1852.)
#define T_PI                   (3.1415926535)

///////////////////
// compatibilite //
///////////////////

#define FABS(x)     fabsf(x)
#define SQRT(x)     sqrtf(x)
#define SIN(x)      sinf(x)
#define COS(x)      cosf(x)
#define ACOS(x)     acosf(x)
#define POW(x,y)    powf(x,y)
#define FMOD(x,y)   fmodf(x,y)
#define FLOOR(x)    floorf(x)
#define MODF(x,y)   modff(x,y)

///////////////////////////
// taches plus affichage //
///////////////////////////

// tache touch, basse priorite core 0 necessaire ou plantage
#define TOUCH_NUM_TASK       0
#define TOUCH_STACK_SIZE     3000
#define TOUCH_PRIORITY       5
#define TOUCH_CORE           0

/////////////////////////////////////////////
// taches plus temps reel                  //
// FreeRtOs utilise le core 0 a 75% a vide //
/////////////////////////////////////////////

// son priorite tres haute non interruptible (bloque/bug les capteurs sans mutex)
#define VARIOBEEP_NUM_TASK   1
#define VARIOBEEP_STACK_SIZE 1000
#define VARIOBEEP_PRIORITY   20
#define VARIOBEEP_CORE       1

// pression calcul Vz, priorite tres haute non interruptible
#define MS5611_NUM_TASK   2
#define MS5611_STACK_SIZE 2000
#define MS5611_PRIORITY   20
#define MS5611_CORE       1

// acquisition cap magnetique, priorite haute non interruptible
#define MPU9250_NUM_TASK   3
#define MPU9250_STACK_SIZE 2000
#define MPU9250_PRIORITY   19
#define MPU9250_CORE       1

// acquisition gps, priorite haute non interruptible
#define SERIAL_NUM_TASK         4
#define SERIAL_GPS_STACK_SIZE   3000
#define SERIAL_GPS_PRIORITY     19
#define SERIAL_GPS_CORE         1

// ecriture igc, priorite moyenne
#define IGC_NUM_TASK    5
#define IGC_STACK_SIZE  4000
#define IGC_PRIORITY    10
#define IGC_CORE        1

// temps de vol / histo, basse priorite
#define TEMPS_NUM_TASK   6
#define TEMPS_STACK_SIZE 4000
#define TEMPS_PRIORITY   5
#define TEMPS_CORE       1

// relance pour cause faux depart de vol, tache fugitive.
#define RELANCE_IGC_NUM_TASK   7
#define RELANCE_IGC_STACK_SIZE 3000
#define RELANCE_IGC_PRIORITY   0
#define RELANCE_IGC_CORE       1

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
void WifiInit() ;
extern char g_NumVersion[] ;
