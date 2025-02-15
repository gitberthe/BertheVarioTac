////////////////////////////////////////////////////////////////////////////////
/// \file CGlobalVar.h
///
/// \brief Variable globale
///
/// \date creation     : 21/09/2024
/// \date modification : 09/02/2025
///

#ifndef _GLOBALVAR_
#define _GLOBALVAR_

////////////////////////////////////////////////////////////////////////////////
/// \brief Variable globale du projet
class CGlobalVar : public CSDCard, public CNumTaskArr, public CGps , public CVarioCapBeep , public CSoundSvr , public CRandoVol
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
    void PurgeBoutons( int DelaySec ) ;
    void GainMemoire() ;

    unsigned long   m_temps_debut ; ///< temps d'affichage de debut
    CScreen         m_Screen ;      ///< ecran tactile
    CConfigFile     m_Config ;      ///< variables issues du fichier de config
    CFilesHistoVols m_HistoVol ;    ///< historique du vol precedent
    CHgt2Agl        m_Hgt2Agl ;     ///< pour determiner la hauteur sol
    CPileVit        m_PileVit ;     ///< pour lancement vol suivant la vitesse gps
    CZonesFchValid  m_ZonesAerAll ; ///< tableau des zones aeriennes pris en compte
    CVirtCaptPress* m_pCapteurPression ;    ///< capteur de pression BMP180 ou MS5611
    CQMC5883Mag     m_QMC5883Mag ;   ///< capteur magnetique
    CBleXct         m_BleXct ;      ///< bluetooth Xc-Track

    bool m_ModeHttpFileMgr = false ;   ///< si mode http pour file manager
    bool m_ModeHttpOta = false ;       ///< si mode telechargement de firmware http
    bool m_BeepAttenteGVZone = true ;  ///< beep d'attente Gps/Vitesse/Zone

    int         m_PercentCore0 ;    ///< pourcentage utilisation core 0
    int         m_PercentCore1 ;    ///< pourcentage utilisation core 1

    // pour affichage
    float m_VitVertMS ;     ///< vitesse verticale
    float m_DureeVolMin ;   ///< duree vol en minutes

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
    float       GetBrightness() ;
    static void BeepError(bool small = false) ;
    static void Reboot() ;
    static void BeepOk() ;
    static void beeper( int frequence , int DurationMs ) ;

    static void  RelancerEnregistrementFichier() ;

private :
    CSoundSvr::StSoundRequest m_Req ;   ///< pour le son

    static void  TacheRelanceIgc(void *param) ;
} ;

extern CGlobalVar g_GlobalVar ;

#endif
