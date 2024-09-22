////////////////////////////////////////////////////////////////////////////////
/// \file CScreen.cpp
///
/// \brief Definition des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 22/09/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Constructeur
CScreen::CScreen()
{
// page Vz
m_T2SPageVzArr.resize(8) ;
m_T2SPageVzArr[PAGE_VZ_DUREE_VOL].SetPos(  15 , 100 , 3 , '\'' ) ;
m_T2SPageVzArr[PAGE_VZ_CAP_DEG].SetPos  ( 110 , 100 , 3 , 'd') ;
m_T2SPageVzArr[PAGE_VZ_CAP_LET].SetPos  ( 200 , 100 , 3 ) ;
m_T2SPageVzArr[PAGE_VZ_VZ].SetPos       ( 35 , 160 , 6 ) ;
m_T2SPageVzArr[PAGE_VZ_FIN_TER].SetPos  ( 5 ,    5 , 3 ) ;
m_T2SPageVzArr[PAGE_VZ_RECULADE].SetPos ( 80 ,  65 , 3 ) ;
m_T2SPageVzArr[PAGE_VZ_VIT_SOL].SetPos  ( 140 , 240, 3 , 'k') ;
m_T2SPageVzArr[PAGE_VZ_ALTI_BARO].SetPos(  15 , 240, 3 , 'm' ) ;

// page sys
m_T2SPageSysArr.resize(6) ;
// cpu
m_T2SPageSysArr[PAGE_SYS_CPU0_TXT].SetPos( 4   , 35 , 3 , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_CPU1_TXT].SetPos( 4   , 60 , 3 , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_CPU0_VAL].SetPos( 160 , 35 , 3 , '%' ) ;
m_T2SPageSysArr[PAGE_SYS_CPU1_VAL].SetPos( 160 , 60 , 3 , '%' ) ;
// free memory
m_T2SPageSysArr[PAGE_SYS_FMEM_TXT].SetPos( 4   , 85 , 3 , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_FMEM_VAL].SetPos(  90 , 85 , 3 , 'o' ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief A appeler souvent pour le touch screen
void CScreen::HandleTouchScreen()
{
lv_timer_handler(); /* let the GUI do its work */
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Raz page blanche ecran
void CScreen::ScreenRaz()
{
g_tft.fillRect( 0 , 0 , m_Largeur , m_Hauteur , TFT_WHITE ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de la Vz et autres parametres de navigation.
CAutoPages::EtatsAuto CScreen::EcranVz()
{
char TmpChar[50] ;
static int count = 0 ;
count++ ;

// nom/finesse du site le plus proche
float FinesseTerrainMinimum = 99. ;
const CLocTerrain * pTerrain = g_GlobalVar.m_TerrainArr.GetTerrainProche( FinesseTerrainMinimum ) ;
const int Longueur = 11  ;
char TmpCharNomSite[Longueur+2] = "-----------" ;
if ( pTerrain != NULL )
    {
    int inspp = 0 ;
    // recopie nom de terrain 10 maximum
    for ( ; inspp < Longueur && inspp < pTerrain->m_Nom.length() ; inspp++ )
        TmpCharNomSite[inspp] = pTerrain->m_Nom[inspp] ;
    TmpCharNomSite[inspp] = 0 ;
    // 10 minimum
    int len = strlen(TmpCharNomSite) ;
    while( len < Longueur )
        {
        TmpCharNomSite[len] = ' ' ;
        TmpCharNomSite[len+1] = 0 ;
        len++ ;
        }
    }
else
    FinesseTerrainMinimum = 99 ;
sprintf( TmpChar , "%s%2d" , TmpCharNomSite , (int)FinesseTerrainMinimum ) ;
m_T2SPageVzArr[PAGE_VZ_FIN_TER].Affiche(TmpChar) ;

// reculade
m_T2SPageVzArr[PAGE_VZ_RECULADE].Affiche("<<5>>") ;

// duree du vol
if ( g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )
    sprintf( TmpChar , "%2dG" , g_GlobalVar.GetNbSat() ) ;
else if ( g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS )
    sprintf( TmpChar , "%2dS" , g_GlobalVar.GetNbSat()) ;
else if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL )
    sprintf( TmpChar , "%2dV" , g_GlobalVar.GetNbSat()) ;
else
    sprintf( TmpChar , "%3d", g_GlobalVar.m_DureeVolMin ) ;
m_T2SPageVzArr[PAGE_VZ_DUREE_VOL].Affiche(TmpChar) ;

// cap dizaines
m_T2SPageVzArr[PAGE_VZ_CAP_DEG].Affiche("360") ;

// cap nom
m_T2SPageVzArr[PAGE_VZ_CAP_LET].Affiche("SW") ;

// affichage VZ
if ( count%2 )
    m_T2SPageVzArr[PAGE_VZ_VZ].Affiche("-0.5-") ;
else
    m_T2SPageVzArr[PAGE_VZ_VZ].Affiche(" 0.3 ") ;

// affichage vitesse sol
sprintf( TmpChar , "%4.1f" , g_GlobalVar.m_VitesseKmh ) ;
m_T2SPageVzArr[PAGE_VZ_VIT_SOL].Affiche(TmpChar) ;

// affichage altitdue
m_T2SPageVzArr[PAGE_VZ_ALTI_BARO].Affiche("9999") ;

// defilement autre ecran
g_GlobalVar.m_Screen.SetText( "" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "Sys", 1 ) ;
g_GlobalVar.m_Screen.SetText( "" , 2 ) ;
if ( g_GlobalVar.m_Screen.IsButtonPressed( 0 ) )
    return ECRAN_0_Vz ;
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    return ECRAN_6_Sys ;
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
    return ECRAN_0_Vz ;

return ECRAN_0_Vz ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage des parametres systemes.
CAutoPages::EtatsAuto CScreen::EcranSys()
{

// % cpu 0
char TmpChar[10] ;
m_T2SPageSysArr[PAGE_SYS_CPU0_TXT].Affiche( "cpu0:" ) ;
sprintf( TmpChar , "%3d" , g_GlobalVar.m_PercentCore0 ) ;
m_T2SPageSysArr[PAGE_SYS_CPU0_VAL].Affiche( TmpChar ) ;
// % cpu 1
m_T2SPageSysArr[PAGE_SYS_CPU1_TXT].Affiche( "cpu1:" ) ;
sprintf( TmpChar , "%3d" , g_GlobalVar.m_PercentCore1 ) ;
m_T2SPageSysArr[PAGE_SYS_CPU1_VAL].Affiche( TmpChar ) ;
// free mem
m_T2SPageSysArr[PAGE_SYS_FMEM_TXT].Affiche( "fmem:" ) ;
sprintf( TmpChar , "%7d" , (int) esp_get_free_heap_size() ) ;
m_T2SPageSysArr[PAGE_SYS_FMEM_VAL].Affiche( TmpChar ) ;

// defilement autre ecran
g_GlobalVar.m_Screen.SetText( "" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "Vz", 1 ) ;
g_GlobalVar.m_Screen.SetText( "" , 2 ) ;
if ( g_GlobalVar.m_Screen.IsButtonPressed( 0 ) )
    return ECRAN_6_Sys ;
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    return ECRAN_0_Vz ;
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
    return ECRAN_6_Sys ;

return ECRAN_6_Sys ;
}
