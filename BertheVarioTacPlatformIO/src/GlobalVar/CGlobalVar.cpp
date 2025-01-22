////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.cpp
///
/// \brief Variable globale
///
/// \date creation     : 20/09/2024
/// \date modification : 22/01/2025
///

#include "../BertheVarioTac.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
CGlobalVar::CGlobalVar()
{
strcpy(m_LatChar, "0000000") ;
strcpy(m_LonChar, "00000000") ;
m_TerrainPosCur.m_Nom = "pos-cur" ;

m_pCapteurPression = NULL ;
#ifdef BMP180_PRESS
 m_pCapteurPression = new CBMP180Pression ;///< capteur de pression
#endif
#ifdef MS5611_PRESS
 m_pCapteurPression = new CMS5611Pression ;///< capteur de pression
#endif

m_temps_debut = millis() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Pour un gain memoire
void CGlobalVar::GainMemoire()
{
m_PileVit.ResetVit() ;
m_HistoVol.m_HistoDir.clear() ;
m_Config.FreeVect() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoie l'angle signe en degres entre A et B.
float CGlobalVar::GetDiffAngle( float AngleA , float AngleB )
{
// coordonnes  cartersiennes vecteur angle
float u1 = cosf(AngleA*T_PI/180.) ;
float u2 = sinf(AngleA*T_PI/180.) ;
float v1 = cosf(AngleB*T_PI/180.) ;
float v2 = sinf(AngleB*T_PI/180.) ;

// produit vectoriel
float z3 = u1*v2 - u2*v1 ;

// produit scalaire
float & x1 = u1 ;
float & x2 = u2 ;
float & y1 = v1 ;
float & y2 = v2 ;
float produit_scalaire = x1*y1 + x2*y2 ;

// angle entre les 2 vecteurs
float Angle = acosf( produit_scalaire ) * 180./T_PI ;

// signe de l'angle
if ( z3 < 0. )
    return Angle ;
return -Angle ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Son d'erreur
void CGlobalVar::BeepError(bool small)
{
beeper(7000, 300) ;
beeper( SOUND_DELAY_ONLY , 500 ) ;
beeper(7000, 300) ;
if ( small )
    return ;
beeper( SOUND_DELAY_ONLY , 500 ) ;
beeper(7000, 300) ;
beeper( SOUND_DELAY_ONLY , 500 ) ;
beeper(7000, 300) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Son Ok
void CGlobalVar::BeepOk()
{
beeper( 7000 , 100 ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de demander un son au serveur de son
void CGlobalVar::beeper( int frequence , int DurationMs , bool VolumeFort )
{
//g_GlobalVar.m_MutexI2c.PrendreMutex() ;
CSoundSvr::StSoundRequest Req ;
Req.m_Frequence = frequence ;
Req.m_DelayMs = DurationMs ;
if ( VolumeFort )
    Req.m_Volume = DAC_CW_SCALE_1 ;
else
    Req.m_Volume = DAC_CW_SCALE_8 ;
g_GlobalVar.PostSoundRequest( & Req ) ;
//g_GlobalVar.m_MutexI2c.RelacherMutex() ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief fonction qui permet de rebooter.
/// Attention la abort() est bugguee et ne redemarre pas defois
void CGlobalVar::Reboot()
{
g_GlobalVar.BeepError(true) ;

g_GlobalVar.StopAll() ;

ESP.restart();

/*while ( true )
    {
    g_GlobalVar.m_MutexCore.RelacherMutex() ;
    g_GlobalVar.m_MutexVariable.RelacherMutex() ;
    g_GlobalVar.m_MutexI2c.RelacherMutex() ;
    g_TermicMap.m_MutexMap.RelacherMutex() ;
    g_GlobalVar.m_ZonesAerAll.m_Mutex.RelacherMutex() ;
    Serial.print("CGlobalVar::Reboot()") ;
    delay( 100 ) ;
    abort() ;
    }*/

/*float * p = NULL ;
*p = 1 ;*/
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Init GPIO35 en mesure de tension. Et GPIO34 pour luminosite
void  CGlobalVar::InitBattery()
{
analogReadResolution( 12 ) ;
pinMode(VoltageInPin, INPUT_PULLUP); //Il faut déclarer le pin en entrée

pinMode(BrightnessPin, INPUT); //Il faut déclarer le pin en entrée
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Permet de lire la tension sur 4095 valeur, avec VMax à 3.3v,
/// par l'intermediare d'un potentiometre diviseur de tension par 2.
float CGlobalVar::GetBatteryVoltage() const
{
float val = analogRead(VoltageInPin);
val = 3.3 * val / 4095. * 2. ;

return val ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Permet de lire la tension sur 4095 valeur de luminosite, avec VMax
/// à 3.3v. Resistance GT36516 : 10lux = 5k, dark = 0.3M.
/// ValCap semble varier de 0 en pleine lumiere a 130 dans le noir.
int CGlobalVar::GetBrightness()
{
float ValCap = analogRead(BrightnessPin);
float ValCap01 = ValCap/130. ; // varie alors de 0 pleine lumiere a 1 dans le noir
if ( m_Config.m_luminosite < 1 )
    m_Config.m_luminosite = 1 ;
const float MaxBright = 255./((float)m_Config.m_luminosite) ;
float Brightness = MaxBright - MaxBright * ValCap01 ;
if ( Brightness < 1. )
    Brightness = 1. ;
if ( Brightness > MaxBright )
    Brightness = MaxBright ;

#ifdef BRIGHT_DEBUG
 Serial.print( "lum:" ) ;
 Serial.print( Brightness ) ;
 Serial.print( " cap:" ) ;
 Serial.println( ValCap ) ;
#endif
return Brightness ;
//return 255 ;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Permet de purger les boutons pour ne pas redeclancher un vol
void CGlobalVar::PurgeBoutons( int DelaySec )
{
m_Screen.SetFrozenDelaySec( DelaySec ) ;
}

//TwoWire I2C1 = TwoWire(1);

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction qui initialise le "Wire" avec les pin qui vont bien
/// voir aussi CSDCard::InitSDCard() pour les pin et le "SPI"
void CGlobalVar::InitI2C()
{
#ifdef _LG_DEBUG_
 Serial.println("Init I2C" ) ;
#endif

//Wire.begin();

delay(100);
//I2C1.begin(VARIO_SDA_PIN, VARIO_SCL_PIN,400000);
//Wire.flush();
Wire.begin(VARIO_SDA_PIN, VARIO_SCL_PIN);
//Wire.begin(VARIO_SCL_PIN, VARIO_SDA_PIN);
//Wire.setClock(400000); //Increase I2C data rate to 400kHz
 // d'apres la FAQ 100khz evite un plantage du MS5611 capteur de pression
 // mais je pense que c'est plus le mutex qui fait que ca plante plus
//Wire.setClock(100000); //Increase I2C data rate to 100kHz
delay (250);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Relance la sequence d'enregistrement pour un nouveau fichier
void CGlobalVar::RelancerEnregistrementFichier()
{
Serial.println("CGlobalVar::RelancerEnregistrementFichier()") ;
beeper(7000, 300) ;
g_GlobalVar.m_TaskArr[RELANCE_IGC_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[RELANCE_IGC_NUM_TASK].m_Stopped = false ;
xTaskCreatePinnedToCore( TacheRelanceIgc, "RelanceIgc", RELANCE_IGC_STACK_SIZE, NULL  , RELANCE_IGC_PRIORITY , NULL, RELANCE_IGC_CORE );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui relance un nouveau fichier igc
void CGlobalVar::TacheRelanceIgc( void * param )
{
// reset pour reboot avec boutons G/D.
g_GlobalVar.m_FinDeVol.InitFinDeVol() ;

// reset debut de vol vitesse
g_GlobalVar.m_PileVit.ResetVit() ;

// on ne refait pas la stabilisation gps
g_GlobalVar.m_StabGps.SetAlwaysStable() ;

// raz deco
g_GlobalVar.m_TerrainPosDeco.m_Lat =
g_GlobalVar.m_TerrainPosDeco.m_Lon =
g_GlobalVar.m_TerrainPosDeco.m_AltiBaro = 0 ;

// demande d'arret des taches
g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = false ;
g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run = false ;

// attente arret des taches
while( !g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped || !g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Stopped )
    delay( 100 ) ;

// reinit variables de boucle de taches
g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run = true ;

// relance tache
g_GlobalVar.LanceTacheGps(false) ;

g_GlobalVar.m_TaskArr[RELANCE_IGC_NUM_TASK].m_Stopped = true ;

while( true )
    vTaskDelete(NULL) ;
}
