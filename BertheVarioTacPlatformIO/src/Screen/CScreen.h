////////////////////////////////////////////////////////////////////////////////
/// \file CScreen.h
///
/// \brief Definition des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 25/09/2024
///

#ifndef _CSCREEN_
#define _CSCREEN_

// define de mode G S V
#define ATTENTE_MESSAGE_GPS     -3
#define ATTENTE_STABILITE_GPS   -2
#define ATTENTE_VITESSE_VOL     -1

// define des champs a afficher page Vz
#define PAGE_VZ_DUREE_VOL  0
#define PAGE_VZ_ALTI_BARO  1
#define PAGE_VZ_VIT_SOL    2
#define PAGE_VZ_CAP_DEG    3
#define PAGE_VZ_CAP_LET    4
#define PAGE_VZ_VZ         5
#define PAGE_VZ_FIN_TER    6
#define PAGE_VZ_RECULADE   7

// define des champs a afficher page Sys
#define PAGE_SYS_CPU0_TXT  0
#define PAGE_SYS_CPU1_TXT  1
#define PAGE_SYS_CPU0_VAL  2
#define PAGE_SYS_CPU1_VAL  3
#define PAGE_SYS_FMEM_TXT  4
#define PAGE_SYS_FMEM_VAL  5
#define PAGE_SYS_VBAT_TXT  6
#define PAGE_SYS_VBAT_VAL  7

////////////////////////////////////////////////////////////////////////////////
/// \brief /// \brief Definition des pages ecran
class CScreen : public CTouchButtons , public CAutoPages
{
public :
    CScreen() ;

    void HandleTouchScreen() ;

    const int m_Largeur = 240 ;  ///< dimensions de l'écran
    const int m_Hauteur = 320 ;  ///< dimensions de l'écran

    void ScreenRaz() ;
    void ScreenRazButtons()
        { RazButtons() ; }  ;

private :
    /// \brief iteam de la page menu
    struct StItem
        {
        int m_x , m_y ;
        std::string           m_Intitule ;
        CAutoPages::EtatsAuto m_Page ;
        } ;

    CAutoPages::EtatsAuto EcranVz() ;
    CAutoPages::EtatsAuto EcranHisto() ;
    CAutoPages::EtatsAuto EcranListeIgcFch() ;
    CAutoPages::EtatsAuto EcranTmaAll() ;
    CAutoPages::EtatsAuto EcranCfgFch() ;
    CAutoPages::EtatsAuto EcranTmaDessous() ;
    CAutoPages::EtatsAuto EcranSys() ;
    CAutoPages::EtatsAuto EcranWifi() ;
    CAutoPages::EtatsAuto EcranMenu() ;

    CAutoPages::EtatsAuto EcranTmaMod() ;
    CAutoPages::EtatsAuto EcranConfimeArchIgcFch() ;

    std::vector<CText2Screen>   m_T2SPageVzArr ;  ///< tableau des champs page Vz
    std::vector<CText2Screen>   m_T2SPageSysArr ; ///< tableau des champs page Sys
} ;

#endif
