////////////////////////////////////////////////////////////////////////////////
/// \file CScreen.h
///
/// \brief Definition des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 22/09/2024
///

#ifndef _CSCREEN_
#define _CSCREEN_

// define de mode G S V
#define ATTENTE_MESSAGE_GPS     -3
#define ATTENTE_STABILITE_GPS   -2
#define ATTENTE_VITESSE_VOL     -1

// define des champs a afficher par page
#define PAGE_VZ_VZ  0

#define PAGE_SYS_CPU0_TXT  0
#define PAGE_SYS_CPU1_TXT  1
#define PAGE_SYS_CPU0_VAL  2
#define PAGE_SYS_CPU1_VAL  3
#define PAGE_SYS_FMEM_TXT  4
#define PAGE_SYS_FMEM_VAL  5

////////////////////////////////////////////////////////////////////////////////
/// \brief /// \brief Definition des pages ecran
class CScreen : public CTouchButtons , public CAutoPages
{
public :
    CScreen() ;

    void HandleTouchScreen() ;

    int   m_XTouch = -1 ;       ///< coordonnees du touch
    int   m_YTouch = -1 ;       ///< coordonnees du touch
    bool  m_Pressed = false ;   ///< si est appuyé

    const int m_Largeur = 240 ;  ///< dimensions de l'écran
    const int m_Hauteur = 320 ;  ///< dimensions de l'écran

    void ScreenRaz() ;

private :
    CAutoPages::EtatsAuto EcranVz() ;
    CAutoPages::EtatsAuto EcranSys() ;


    std::vector<CText2Screen>   m_T2SPageVzArr ;    ///< tableau des champs page Vz
    std::vector<CText2Screen>   m_T2SPageSysArr ; ///< tableau des champs page Sys
} ;

#endif
