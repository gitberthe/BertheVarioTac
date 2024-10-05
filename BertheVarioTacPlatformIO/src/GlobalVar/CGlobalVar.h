////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.h
///
/// \brief Variable globale
///
/// \date creation     : 21/09/2024
/// \date modification : 05/10/2024
///

#ifndef _GLOBALVAR_
#define _GLOBALVAR_

////////////////////////////////////////////////////////////////////////////////
/// \brief Variable globale du projet
class CGlobalVar : public CSDCard, public CNumTaskArr, public CGps , public CVarioCapBeep , public CSoundSvr // public CFinSol ,
{
public :
    CGlobalVar() ;

//    CMutex      m_MutexI2c ; ///< mutex d'accet au bus car trop de plantage nan alti baro / beeper / cap magnetique
    CMutex      m_MutexVariable ; ///< mutex de la variable globale pour lat/lon gps sous forme char entre tache gps / igc

    bool BoutonGauche()
        { return m_Screen.IsButtonPressed(0) ; } ;
    bool BoutonCentre()
        { return m_Screen.IsButtonPressed(1) ; } ;
    bool BoutonDroit()
        { return m_Screen.IsButtonPressed(2) ; } ;
    void PurgeBoutons( unsigned long DelayMs ) ;

    CScreen         m_Screen ;
    CConfigFile     m_Config ;      ///< variables issues du fichier de config
    CFilesHistoVols m_HistoVol ;    ///< historique du vol precedent
    CHgt2Agl        m_Hgt2Agl ;     ///< pour determiner la hauteur sol
    CPileVit        m_PileVit ;     ///< pour lancement vol suivant la vitesse gps
    CZonesFch       m_ZonesAerAll ; ///< tableau des zones aeriennes pris en compte
    #ifdef BMP180_PRESS
     CBMP180Pression m_BMP180Pression ;///< capteur de pression
    #endif
    CQMC5883Mag      m_QMC5883Mag ;   ///< capteur magnetique

    bool m_ModeHttp = false ;          ///< si mode http
    bool m_BeepAttenteGVZone = true ;  ///< beep d'attente Gps/Vitesse/Zone

    int         m_PercentCore0 ;    ///< pourcentage utilisation core 0
    int         m_PercentCore1 ;    ///< pourcentage utilisation core 1

    // pour affichage
    float m_VitVertMS ;     ///< vitesse verticale
    int   m_DureeVolMin ;   ///< duree vol en minutes

    // pour elaboration finesse
    CTerrainsConnu m_TerrainArr ;     ///< fichier de terrains connus pour elaboration finesse
    CLocTerrain    m_TerrainPosCur ;  ///< position courante
    CLocTerrain    m_TerrainPosDeco ; ///< position de decollage

    // parametres Gps
    float m_AltiGps = 0. ;      ///< alti gps en metres
    int   m_AltitudeSolHgt = 0 ;///< altitude sol hgt
    int   m_CapGpsDeg = 0 ;     ///< cap gps en degres
    float m_VitesseKmh = 0. ;   ///< vitesse gps en km/h
    int   m_Annee = 1 ;         ///< Annee gps
    int   m_Mois = 1 ;          ///< Mois gps
    int   m_Jour = 1 ;          ///< Jour gps
    int   m_HeureSec = 0 ;      ///< heure gps en seconde
    char  m_LonChar[10] ;       ///< pour fichier igc
    char  m_LatChar[10] ;       ///< pour fichier igc

    void        InitI2C() ;
    void        InitBattery() ;
    float       GetDiffAngle( float AngleA , float AngleB ) ;
    float       GetBatteryVoltage() const ;
    static void BeepError(bool small = false) ;
    static void Reboot() ;
    static void BeepOk() ;
    static void beeper( int frequence , int DurationMs ) ;

    static void  RelancerEnregistrementFichier() ;

private :

    static void  TacheRelanceIgc(void *param) ;
} ;

extern CGlobalVar g_GlobalVar ;

#endif
