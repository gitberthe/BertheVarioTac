////////////////////////////////////////////////////////////////////////////////
/// \file CScreen.cpp
///
/// \brief Definition des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 25/09/2024
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
m_T2SPageVzArr[PAGE_VZ_VZ].SetPos       (  35 , 160 , 6 ) ;
m_T2SPageVzArr[PAGE_VZ_FIN_TER].SetPos  (   5 ,  5  , 3 ) ;
m_T2SPageVzArr[PAGE_VZ_RECULADE].SetPos (  75 , 60  , 3 ) ;
m_T2SPageVzArr[PAGE_VZ_VIT_SOL].SetPos  ( 110 , 240 , 3 , 'k') ;
m_T2SPageVzArr[PAGE_VZ_ALTI_BARO].SetPos(  15 , 240 , 3 , 'm' ) ;

// page sys
m_T2SPageSysArr.resize(8) ;
const int DeltaY = 20 ;
int y = 35 ;
// cpu
m_T2SPageSysArr[PAGE_SYS_CPU0_TXT].SetPos( 10  , y , 2 , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_CPU0_VAL].SetPos( 170 , y , 2 , '%' ) ;
y += DeltaY ;
m_T2SPageSysArr[PAGE_SYS_CPU1_TXT].SetPos( 10  , y , 2 , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_CPU1_VAL].SetPos( 170 , y , 2 , '%' ) ;
// free memory
y += DeltaY ;
m_T2SPageSysArr[PAGE_SYS_FMEM_TXT].SetPos( 10  , y , 2 , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_FMEM_VAL].SetPos( 120 , y , 2 , 'o' ) ;
// vbat
y += DeltaY ;
m_T2SPageSysArr[PAGE_SYS_VBAT_TXT].SetPos( 10  , y , 2 , ' ' , true ) ;
m_T2SPageSysArr[PAGE_SYS_VBAT_VAL].SetPos( 155 , y , 2 , 'v' ) ;
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
g_tft.fillRect( 0 , 0 , m_Largeur , m_Hauteur , TFT_BLACK ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de la Vz et autres parametres de navigation.
CAutoPages::EtatsAuto CScreen::EcranVz()
{
char TmpChar[50] ;
static int count = 0 ;
count++ ;

// nouvelle page
if ( IsPageChanged() )
    ScreenRaz() ;

// nom/finesse du site le plus proche
float FinesseTerrainMinimum = 99. ;
const CLocTerrain * pTerrain = g_GlobalVar.m_TerrainArr.GetTerrainProche( FinesseTerrainMinimum ) ;
const int Longueur = 11  ;
char TmpCharNomSite[Longueur+2] = "-----------" ;
if ( pTerrain != NULL )
    {
    int inspp = 0 ;
    // recopie nom de terrain 11 maximum
    for ( ; inspp < Longueur && inspp < pTerrain->m_Nom.length() ; inspp++ )
        TmpCharNomSite[inspp] = pTerrain->m_Nom[inspp] ;
    TmpCharNomSite[inspp] = 0 ;
    // 11 minimum
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

// reculade/derive
const float DeriveMilieu = 41. ;
float DeriveAngle = g_GlobalVar.GetDiffAngle( g_GlobalVar.m_CapGpsDeg , 0 /*m_Mpu9250.m_CapMagnetique*/ ) ;
if ( fabsf(DeriveAngle) >= 90. )
    sprintf( TmpChar , " \\R/" ) ;
else if ( DeriveAngle >= DeriveMilieu )
    sprintf( TmpChar , "  %1d>>", ((int)(fabsf(DeriveAngle)/10)) ) ;
else if ( DeriveAngle <= -DeriveMilieu )
    sprintf( TmpChar , "<<%1d", ((int)(fabsf(DeriveAngle)/10)) ) ;
else
    sprintf( TmpChar , " ^%1d^", ((int)(fabsf(DeriveAngle)/10)) ) ;
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

// cap lettres
int Cap = g_GlobalVar.m_CapGpsDeg ;
int CapMarge = 45/2 + 1 ;
char TmpCharNomCap[] = "  " ;
if ( Cap < CapMarge || Cap > (360-CapMarge) )
    strcpy( TmpCharNomCap, "N " ) ;
else if ( labs(Cap-45) < CapMarge )
    strcpy( TmpCharNomCap, "NE" ) ;
else if ( labs(Cap-90) < CapMarge )
    strcpy( TmpCharNomCap, "E " ) ;
else if ( labs(Cap-135) < CapMarge )
    strcpy( TmpCharNomCap, "SE" ) ;
else if ( labs(Cap-180) < CapMarge )
    strcpy( TmpCharNomCap, "S " ) ;
else if ( labs(Cap-225) < CapMarge )
    strcpy( TmpCharNomCap, "SW" ) ;
else if ( labs(Cap-270) < CapMarge )
    strcpy( TmpCharNomCap, "W " ) ;
else if ( labs(Cap-315) < CapMarge )
    strcpy( TmpCharNomCap, "NW" ) ;
m_T2SPageVzArr[PAGE_VZ_CAP_LET].Affiche(TmpCharNomCap) ;

// cap degres
sprintf( TmpChar , "%3d", Cap ) ;
m_T2SPageVzArr[PAGE_VZ_CAP_DEG].Affiche(TmpChar) ;

// affichage VZ
float VitVert = g_GlobalVar.m_VitVertMS ;
if ( count%2 )
    VitVert = -0.5 ;
else
    VitVert = 0.3 ;
bool SigneNeg = VitVert < 0. ;
uint16_t color = TFT_BLACK ;
if ( SigneNeg )
    {
    sprintf( TmpChar , "%2.1f-" , VitVert ) ;
    color = TFT_RED ;
    }
else
    sprintf( TmpChar , " %2.1f " , VitVert ) ;
m_T2SPageVzArr[PAGE_VZ_VZ].Affiche(TmpChar) ;
const int bordure = 5 ;
g_tft.fillRect( bordure , 210 , g_GlobalVar.m_Screen.m_Largeur - 2 * bordure , 15 , color ) ;
g_tft.fillRect( bordure , 135 , g_GlobalVar.m_Screen.m_Largeur - 2 * bordure , 15 , color ) ;
g_tft.fillRect( bordure , 135 , 15 , 75 , color ) ;
g_tft.fillRect( g_GlobalVar.m_Screen.m_Largeur - bordure -15 , 135 , 15 , 75 , color ) ;

// affichage pour affichage vitesse/hauteur sol
static bool AffichageVitesse = false ;
static unsigned long TempsHauteurSol = millis() ;
// si 5 secondes depassée
if ( AffichageVitesse && ((millis()-TempsHauteurSol)/1000) >= 5 )
    {
    AffichageVitesse = !AffichageVitesse ;
    TempsHauteurSol = millis() ;
    }
else if ( !AffichageVitesse && ((millis()-TempsHauteurSol)/1000) >= 2 )
    {
    AffichageVitesse = !AffichageVitesse ;
    TempsHauteurSol = millis() ;
    }
if ( AffichageVitesse )
    {
    m_T2SPageVzArr[PAGE_VZ_VIT_SOL].ChangeUnit('k') ;
    sprintf( TmpChar , "%6.1f" , g_GlobalVar.m_VitesseKmh ) ;
    }
else
    {
    m_T2SPageVzArr[PAGE_VZ_VIT_SOL].ChangeUnit('m') ;
    sprintf( TmpChar , "%6d", (int)(g_GlobalVar.m_TerrainPosCur.m_AltiBaro-g_GlobalVar.m_AltitudeSolHgt) ) ;
    }
m_T2SPageVzArr[PAGE_VZ_VIT_SOL].Affiche(TmpChar) ;

// affichage altitude
sprintf( TmpChar , "%4d", (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;
m_T2SPageVzArr[PAGE_VZ_ALTI_BARO].Affiche(TmpChar) ;

// defilement autre ecran
// si activation / desactivation beep attente Gps / Vitesse
if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )
    g_GlobalVar.m_Screen.SetText( "Son" , 0 ) ;
else
    g_GlobalVar.m_Screen.SetText( "" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "His", 1 ) ;
// si attente de vol
if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )
    g_GlobalVar.m_Screen.SetText( "Vol" , 2 ) ;
// si vol en cours
else if ( ! g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped )
    g_GlobalVar.m_Screen.SetText( "Vol" , 2 ) ;
else
    g_GlobalVar.m_Screen.SetText( "" , 2 ) ;

// desactivation son
if ( g_GlobalVar.m_Screen.IsButtonPressed( 0 ) )
    {
    if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL ||
         g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
         g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )
        g_GlobalVar.m_BeepAttenteGVZone = ! g_GlobalVar.m_BeepAttenteGVZone ;
    return ECRAN_0_Vz ;
    }
// page suivante
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {
    return ECRAN_1_Histo ;
    }
//else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
//    return ECRAN_0_Vz ;

// encadrements
g_tft.drawLine( 0 , 90 , 240 , 90 , TFT_WHITE ) ;
g_tft.drawLine( 0 ,130 , 240 ,130 , TFT_WHITE ) ;
g_tft.drawLine( 70 , 50 , 240-70  , 50 , TFT_WHITE ) ;
g_tft.drawLine( 70 , 50 , 70  , 90 , TFT_WHITE ) ;
g_tft.drawLine( 240-70 , 50 , 240-70  , 90 , TFT_WHITE ) ;
g_tft.drawLine( 95 , 90 , 95 ,130 , TFT_WHITE ) ;

g_tft.drawLine( 0 ,230 , 240 ,230 , TFT_WHITE ) ;
g_tft.drawLine( 120 ,230 , 120 ,270 , TFT_WHITE ) ;

return ECRAN_0_Vz ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Historique des fichier Igc
CAutoPages::EtatsAuto CScreen::EcranHisto()
{
static int ivol = 0 ;
static int lastivol = -1 ;
int y = 20 ;
const int x1 = 30 ;
const int x2 = 140 ;

// lecture des histo
if ( IsPageChanged() )
    {
    g_GlobalVar.m_HistoVol.LectureFichiers() ;
    lastivol = -1 ;
    }

if ( lastivol != ivol )
    {
    lastivol = ivol ;
    ScreenRaz() ;
    }

g_tft.setTextSize(2) ;

// si pas de fichiers histo
if ( g_GlobalVar.m_HistoVol.m_HistoDir.size() == 0 )
    {
    g_tft.setCursor( 40 , 10 ) ;
    g_tft.print("0 histo");
    goto fin_histo ;
    }

char TmpCharNomFchIgc[20] ;
sprintf( TmpCharNomFchIgc , "%s", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_NomIgc ) ;

char TmpCharAltiDeco[20] ;
sprintf( TmpCharAltiDeco , "%4dm", (int)g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_ZDeco ) ;

char TmpCharAltiMax[20] ;
sprintf( TmpCharAltiMax , "%4dm", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_ZMax ) ;

char TmpCharVzMax[20] ;
sprintf( TmpCharVzMax , "%5.1f", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_VzMax ) ;

char TmpCharVzMin[20] ;
sprintf( TmpCharVzMin , "%5.1f", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_VzMin ) ;

char TmpCharVsMax[20] ;
sprintf( TmpCharVsMax , "%5.1f", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_VsMax ) ;

char TmpCharDistMax[20] ;
sprintf( TmpCharDistMax , "%5.1f", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_DistanceKm ) ;

// temps de vol
char TmpCharTV[20] ;
sprintf( TmpCharTV , " %3d'", g_GlobalVar.m_HistoVol.m_HistoDir[ivol].m_TempsDeVol ) ;


// nom fch igc
g_tft.setCursor(x1, y);
g_tft.print(TmpCharNomFchIgc);

// alti decollage
y += 40 ;
g_tft.setCursor(x1, y);
g_tft.print("Z deco:");
g_tft.setCursor(x2, y);
g_tft.print(TmpCharAltiDeco);

// alti max
y += 20 ;
g_tft.setCursor(x1, y);
g_tft.print("Z max :");
g_tft.setCursor(x2, y);
g_tft.print(TmpCharAltiMax);

// Vz max
y += 20 ;
g_tft.setCursor(x1, y);
g_tft.print("Vz max:");
g_tft.setCursor(x2, y);
g_tft.print(TmpCharVzMax);

// Vz min
y += 20 ;
g_tft.setCursor(x1, y);
g_tft.print("Vz min:");
g_tft.setCursor(x2, y);
g_tft.print(TmpCharVzMin);

// distance max
y += 20 ;
g_tft.setCursor(x1, y);
g_tft.print("Dist. :");
g_tft.setCursor(x2, y);
g_tft.print(TmpCharDistMax);

// Vs max
y += 20 ;
g_tft.setCursor(x1, y);
g_tft.print("Vs max:");
g_tft.setCursor(x2, y);
g_tft.print(TmpCharVsMax);

// Dure vol
y += 20 ;
g_tft.setCursor(x1,y);
g_tft.print("t vol :");
g_tft.setCursor(x2, y);
g_tft.print(TmpCharTV);

// fin de la fonction
fin_histo :

// defilement autre ecran
if ( ivol <= 0 )
    g_GlobalVar.m_Screen.SetText( "   " , 0 ) ;
else
    g_GlobalVar.m_Screen.SetText( "M-" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "Igc", 1 ) ;
if ( ivol >= (g_GlobalVar.m_HistoVol.m_HistoDir.size() - 1) )
    g_GlobalVar.m_Screen.SetText( "   " , 2 ) ;
else
    g_GlobalVar.m_Screen.SetText( "M+" , 2 ) ;

// si changement de numero histo vol
if ( g_GlobalVar.BoutonDroit() )
    {
    ResetTimeOut() ;
    ivol++ ;
    if ( ivol >= g_GlobalVar.m_HistoVol.m_HistoDir.size() )
        ivol = g_GlobalVar.m_HistoVol.m_HistoDir.size() - 1 ;
    return ECRAN_1_Histo ;
    }

// si changement d'ecran
if ( g_GlobalVar.BoutonGauche() )
    {
    ResetTimeOut() ;
    ivol-- ;
    if ( ivol < 0 )
        ivol = 0 ;
    return ECRAN_1_Histo ;
    }

// si changement d'ecran
if ( g_GlobalVar.BoutonCentre() )
    {
    g_GlobalVar.m_HistoVol.m_HistoDir.clear() ;
    return ECRAN_2a_ListeIgc ;
    }

return ECRAN_1_Histo ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de modifier les champs de fichier de configuration
CAutoPages::EtatsAuto CScreen::EcranListeIgcFch()
{
static std::vector<std::string> VecNomIgc ;
static std::vector<int> VecTempsIgc ;

// lecture de fichier
if ( IsPageChanged() )
    {
    ScreenRaz() ;
    g_GlobalVar.ListeIgc(VecNomIgc,VecTempsIgc) ;

    // texte boutons
    g_GlobalVar.m_Screen.SetText( "Wif" , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "TMo",  1 ) ;
    g_GlobalVar.m_Screen.SetText( "Arc" , 2 ) ;


    int TotalMin = 0 ;
    int y_cursor ;
    for ( int ifch = 0 ; ifch < VecNomIgc.size() ; ifch++ )
        TotalMin += VecTempsIgc[ifch] ;

    char TmpChar[25] ;

    g_tft.setTextSize(2) ;

    int ivec = 0 ;
    y_cursor = 10 ;
    for ( ; ivec < VecNomIgc.size() ; ivec++ )
        {
        sprintf( TmpChar , "%s %03d", (const char*)VecNomIgc[ivec].c_str() , VecTempsIgc[ivec] ) ;
        y_cursor += 16 ;
        g_tft.setCursor( 10, y_cursor );
        g_tft.print( TmpChar ) ;
        }

    sprintf( TmpChar , "tot. igc:%03dm", TotalMin ) ;
    g_tft.setCursor( 10, y_cursor + 25 );
    g_tft.print( TmpChar ) ;
    }

// defilement autre ecran
if ( g_GlobalVar.m_Screen.IsButtonPressed( 0 ) )
    {
    unsigned long time = millis() ;
    while( (millis()-time) < 1000 )
        g_GlobalVar.m_Screen.RazButtons() ;
    return ECRAN_7_Wifi ;
    }
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {
    return ECRAN_3a_TmaAll ;
    }
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
    {
    return ECRAN_2b_ConfirmArchIgc ;
    }

return ECRAN_2a_ListeIgc ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de passer en mode wifi
CAutoPages::EtatsAuto CScreen::EcranWifi()
{
if ( IsPageChanged() )
    {
    ScreenRaz() ;

    // texte boutons
    g_GlobalVar.m_Screen.SetText( "Ok" , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "Can",  1 ) ;
    g_GlobalVar.m_Screen.SetText( "Ok" , 2 ) ;

    g_tft.setTextSize(2) ;
    g_tft.setCursor( 10 , 50 ) ;
    g_tft.print("Confirmer le Wifi ?") ;
    }

// si confirmation
if ( g_GlobalVar.BoutonDroit() || g_GlobalVar.BoutonGauche() )
    {
    g_GlobalVar.m_ModeHttp = true ;
    ScreenRaz() ;
    return ECRAN_7_Wifi ;
    }

if ( g_GlobalVar.BoutonCentre() )
    {
    return ECRAN_2a_ListeIgc ;
    }

return ECRAN_7_Wifi ;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Affiche les zones modifiables
CAutoPages::EtatsAuto CScreen::EcranTmaAll()
{
// tri par nom
g_GlobalVar.m_ZonesAerAll.TriZonesNom() ;

if ( IsPageChanged() )
    ScreenRaz() ;

// construction du tableau des zones
std::vector<CZoneAer*> VecZonesMod ;
const int NbZones = g_GlobalVar.m_ZonesAerAll.GetNbZones() ;
CZoneAer ** pZoneArr = g_GlobalVar.m_ZonesAerAll.GetZoneArr() ;

// ajout vecteur des zones activables
for ( int iz = 0 ; iz < NbZones ; iz++ )
    {
    CZoneAer * pZone = pZoneArr[iz] ;
    if ( pZone->m_DansFchActivation )
        VecZonesMod.push_back( pZone ) ;
    }
// ajout vecteur des zones activables
for ( int iz = 0 ; iz < NbZones ; iz++ )
    {
    CZoneAer * pZone = pZoneArr[iz] ;
    if ( !pZone->m_DansFchActivation )
        VecZonesMod.push_back( pZone ) ;
    }

// titre
char TmpTitre[25] ;
sprintf( TmpTitre , "%2d Zones memoire" , g_GlobalVar.m_ZonesAerAll.GetNbZones() ) ;

// taille texte et placement
g_tft.setTextSize(2) ;
g_tft.setCursor( 10, 20 );
g_tft.print( TmpTitre ) ;

// zones active
int xcol = 0 ;
int yligne = 15 ;
char TmpChar[25] ;
for ( int iz = 0 ; iz < VecZonesMod.size() ; iz++ )
    {
    if ( !VecZonesMod[iz]->m_DansFchActivation )
        continue ;
    g_tft.setCursor(10+xcol, 40 + yligne );

    // nom des zones
    if ( VecZonesMod[iz]->m_Activee )
        {
        strcpy( TmpChar , VecZonesMod[iz]->m_NomAff.c_str() ) ;
        // remplacement des espaces
        int ic = 0 ;
        while ( TmpChar[ic] != 0 )
            {
            if ( TmpChar[ic] == ' ' )
                TmpChar[ic] = '_' ;
            ic++ ;
            }
        }
    else
        {
        sprintf( TmpChar , "-%s" ,  VecZonesMod[iz]->m_NomAff.c_str() ) ;
        TmpChar[9] = 0 ;
        // remplacement des espaces
        int ic = 0 ;
        while ( TmpChar[ic] != 0 )
            {
            if ( TmpChar[ic] == ' ' )
                TmpChar[ic] = '_' ;
            ic++ ;
            }
        }
    g_tft.print( TmpChar ) ;
    yligne += 19 ;
    if ( iz == 10 )
        {
        xcol = 110 ;
        yligne = 15 ;
        }
    }

g_GlobalVar.m_Screen.SetText( "Mod" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "Cfg", 1 ) ;
g_GlobalVar.m_Screen.SetText( "Mod" , 2 ) ;

// si changement d'ecran
if ( g_GlobalVar.BoutonDroit() )
    {
    return ECRAN_3b_TmaMod ;
    }

// si changement d'ecran
if ( g_GlobalVar.BoutonGauche() )
    {
    return ECRAN_3b_TmaMod ;
    }

// si changement modification zone
if ( g_GlobalVar.BoutonCentre() )
    {
    return ECRAN_4_CfgFch ;
    }

return ECRAN_3a_TmaAll ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Modification du fichier configuration
CAutoPages::EtatsAuto CScreen::EcranCfgFch()
{
static char TmpModChar[100] = {0} ;
std::string Name ;
std::string Value ; ;

static bool ChampsModified = false ;
static int m_LastiChamps = -2 ;
static bool CfgFileEnMod = false ;

// à l'entree dans la page on a rien modifier
if ( IsPageChanged() )
    {
    m_CfgFileiChamps = -1 ;
    ChampsModified = false ;
    m_LastiChamps = -2 ;
    }

// on modifie la luminosite en direct
g_tft.setBrightness( g_GlobalVar.m_Config.m_luminosite ) ;

// texte bouton
if ( CfgFileEnMod )
    {
    g_GlobalVar.m_Screen.SetText( " - " , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "Val", 1 ) ;
    g_GlobalVar.m_Screen.SetText( " + " , 2 ) ;
    }
else
    {
    if ( m_CfgFileiChamps != -1 )
        {
        g_GlobalVar.m_Screen.SetText( "Mv-" , 0 ) ;
        g_GlobalVar.m_Screen.SetText( "Mod" , 1 ) ;
        g_GlobalVar.m_Screen.SetText( "Mv+" , 2 ) ;
        }
    else
        {
        g_GlobalVar.m_Screen.SetText( "Var" , 0 ) ;
        g_GlobalVar.m_Screen.SetText( "Tma" , 1 ) ;
        g_GlobalVar.m_Screen.SetText( "Var" , 2 ) ;
        }
    }

// selection du champs
if ( m_CfgFileiChamps == -1 )
    {
    strcpy( TmpModChar , "" ) ;
    Name = "  Editeur Cfg file" ;
    }
else
    g_GlobalVar.m_Config.GetChar( m_CfgFileiChamps , Name , Value ) ;

// changement de champ
if ( m_LastiChamps != m_CfgFileiChamps )
    {
    m_LastiChamps = m_CfgFileiChamps ;
    ScreenRaz() ;

    g_tft.setTextSize(2) ;
    }

// Mod
g_tft.setCursor(0, 20);
g_tft.print(TmpModChar) ;
// NomVar
g_tft.setCursor(0, 60);
g_tft.print(Name.c_str()) ;
// ValNomVar
g_tft.setCursor(0, 140);
g_tft.print(Value.c_str()) ;

bool BoutonCent = g_GlobalVar.BoutonCentre() ;
bool BoutonGau  = g_GlobalVar.BoutonGauche() ;
bool BoutonDroi = g_GlobalVar.BoutonDroit() ;
// sortie ecran
if ( BoutonCent && !CfgFileEnMod && m_CfgFileiChamps == -1 )
    {
    if ( ChampsModified )
        {
        g_GlobalVar.m_Config.EcritureFichier() ;
        g_GlobalVar.m_Config.LectureFichier() ;
        }
    return ECRAN_5_TmaDessous ;
    }

// modification du mode modif/edition
if ( BoutonCent && m_CfgFileiChamps != -1 )
    {
    ScreenRaz() ;
    CfgFileEnMod = !CfgFileEnMod ;
    if ( CfgFileEnMod )
        strcpy( TmpModChar , "Modification" ) ;
    else
        strcpy( TmpModChar , "Edition" ) ;
    }

// decrementation de variable
if ( BoutonGau && CfgFileEnMod )
    {
    ChampsModified = true ;
    CConfigFile::st_line * pLine = g_GlobalVar.m_Config.m_LinesVect[m_CfgFileiChamps] ;
    if ( pLine->m_Type == TYPE_VAR_FLOAT )
        {
        float * pVal = (float*) pLine->m_pVar ;
        *pVal -= 0.05 ;
        }
    else if ( pLine->m_Type == TYPE_VAR_INT )
        {
        int *pVal = (int*) pLine->m_pVar ;
        int inc = 10 ;
        if ( abs(*pVal) <= 60 )
            inc = 1 ;
        *pVal -= inc ;
        }
    else if ( pLine->m_Type == TYPE_VAR_BOOL )
        {
        bool *pVal = (bool*) pLine->m_pVar ;
        *pVal = !*pVal ;
        }
    g_GlobalVar.m_Screen.ScreenRaz() ;
    }

// incrementation de variable
if ( BoutonDroi && CfgFileEnMod )
    {
    ChampsModified = true ;
    CConfigFile::st_line * pLine = g_GlobalVar.m_Config.m_LinesVect[m_CfgFileiChamps] ;
    if ( pLine->m_Type == TYPE_VAR_FLOAT )
        {
        float * pVal = (float*) pLine->m_pVar ;
        *pVal += 0.05 ;
        }
    else if ( pLine->m_Type == TYPE_VAR_INT )
        {
        int *pVal = (int*) pLine->m_pVar ;
        int inc = 10 ;
        if ( abs(*pVal) <= 60 )
            inc = 1 ;
        *pVal += inc ;
        }
    else if ( pLine->m_Type == TYPE_VAR_BOOL )
        {
        bool *pVal = (bool*) pLine->m_pVar ;
        *pVal = !*pVal ;
        }
    g_GlobalVar.m_Screen.ScreenRaz() ;
    }

// defilement
if (  BoutonGau && !CfgFileEnMod )
    {
    if ( m_CfgFileiChamps > -1 )
        m_CfgFileiChamps-- ;
    else
        m_CfgFileiChamps = g_GlobalVar.m_Config.m_LinesVect.size()-1 ;
    }

if ( BoutonDroi && !CfgFileEnMod  )
    {
    int size = (g_GlobalVar.m_Config.m_LinesVect.size()-1) ;
    if ( m_CfgFileiChamps < size )
        m_CfgFileiChamps++ ;
    else
        m_CfgFileiChamps = -1 ;
    }

return ECRAN_4_CfgFch ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Indique la Tma desoous laquelle on est.
CAutoPages::EtatsAuto CScreen::EcranTmaDessous()
{
std::string NomZone = "" ;

// zone au dessus
g_GlobalVar.m_ZonesAerAll.m_Mutex.PrendreMutex() ;
 if ( g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone )
    NomZone = g_GlobalVar.m_ZonesAerAll.m_NomZoneDansDessous ;
g_GlobalVar.m_ZonesAerAll.m_Mutex.RelacherMutex() ;

g_tft.setTextSize(2) ;

if ( IsPageChanged() )
    ScreenRaz() ;

// nom zone
g_tft.setCursor(20,50);
g_tft.print( "Tma Dessus:\n\n" );
g_tft.print( NomZone.c_str() );

g_GlobalVar.m_Screen.SetText( "" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "Sys", 1 ) ;
g_GlobalVar.m_Screen.SetText( "" , 2 ) ;
if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {
    return ECRAN_6_Sys ;
    }

return ECRAN_5_TmaDessous ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
CAutoPages::EtatsAuto CScreen::EcranTmaMod()
{
static int NumTma = -1 ;
static int LastNumTma = -2 ;

// tri par nom
g_GlobalVar.m_ZonesAerAll.TriZonesNom() ;

if ( IsPageChanged() )
    {
    LastNumTma = -2 ;
    }

// construction du tableau des zones
std::vector<CZoneAer*> VecAffZones ;
const int NbZones = g_GlobalVar.m_ZonesAerAll.GetNbZones() ;
CZoneAer ** pZoneArr = g_GlobalVar.m_ZonesAerAll.GetZoneArr() ;

// construction vecteur des zones a afficher
for ( int iz = 0 ; iz < NbZones ; iz++ )
    {
    if ( pZoneArr[iz]->m_DansFchActivation )
        VecAffZones.push_back( pZoneArr[iz] ) ;
    }
for ( int iz = 0 ; iz < NbZones ; iz++ )
    {
    if ( !pZoneArr[iz]->m_DansFchActivation )
        VecAffZones.push_back( pZoneArr[iz] ) ;
    }

// selection des zones de meme nom que selectionnee
std::vector<CZoneAer *> VecZone2Mod ;
if ( NumTma >= 0 && NumTma < VecAffZones.size() )
    {
    CZoneAer * pZone = VecAffZones[NumTma] ;
    for ( int iz = 0 ; iz < VecAffZones.size() ; iz++ )
        if ( VecAffZones[iz]->m_NomAff == pZone->m_NomAff )
            VecZone2Mod.push_back( VecAffZones[iz] ) ;
    }

g_tft.setTextSize(2) ;
if ( LastNumTma != NumTma )
    {
    LastNumTma = NumTma ;
    ScreenRaz() ;
    }

// titre
if ( VecZone2Mod.size() == 0 )
    {
    g_tft.setCursor(10, 70);
    g_tft.print("Ret Bou. Cent.");
    g_tft.setCursor(5, 90);
    g_tft.print("Mod. Bou. G.D.");
    }
else
    {
    // num tma/ctr
    g_tft.setCursor(0, 15);
    g_tft.print(NumTma);
    g_tft.print(":");
    // nom
    //g_tft.setCursor(0, 35);
    CZoneAer * pZone = VecAffZones[NumTma] ;
    g_tft.print(pZone->m_NomAff.c_str());
    // activation
    g_tft.setCursor(0, 60);
    g_tft.print( "Active:");
    if ( pZone->m_Activee )
        g_tft.print( "1");
    else
        g_tft.print( "0");
    if ( pZone->m_DansFchActivation )
        g_tft.print( " mod.");
    else
        g_tft.print( " const");
    // altibasse
    g_tft.setCursor(0, 80);
    g_tft.print( "A bas : ");
    g_tft.print( pZone->m_AltiBasse );

    // periode
    g_tft.setCursor(0,100);
    g_tft.print( "p.de. : ");
    g_tft.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_PeriodeDebutJour : -1 );
    g_tft.print( "-" );
    g_tft.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_PeriodeDebutMois : -1 );
    g_tft.setCursor(0,120);
    g_tft.print( "p.fi. : ");
    g_tft.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_PeriodeFinJour : -1 );
    g_tft.print( "-" );
    g_tft.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_PeriodeFinMois : -1 );

    // altisemaine
    g_tft.setCursor(0,140);
    g_tft.print( "A sem. : ");
    g_tft.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_AltiBassePeriodeSemaine : -1 );
    // altiweekend
    g_tft.setCursor(0,160);
    g_tft.print( "A week : ");
    g_tft.print( (pZone->HavePeriod()) ? pZone->m_pDerogFfvl->m_AltiBassePeriodeWeekEnd : -1 );
    }


g_GlobalVar.m_Screen.SetText( "Mo-" , 0 ) ;
if ( VecZone2Mod.size() == 0 )
    g_GlobalVar.m_Screen.SetText( "Ret", 1 ) ;
else
    g_GlobalVar.m_Screen.SetText( "Inv", 1 ) ;

g_GlobalVar.m_Screen.SetText( "Mo+" , 2 ) ;

// si changement d'ecran
bool BCentre = g_GlobalVar.BoutonCentre() ;
if ( BCentre && VecZone2Mod.size() == 0 )
    {
    return ECRAN_3a_TmaAll ;
    }

// si modification activation
if ( BCentre && VecZone2Mod.size() != 0 )
    {
    for ( int iz = 0 ; iz < VecZone2Mod.size() ; iz++ )
        {
        CZoneAer * pZone = VecZone2Mod[iz] ;
        if ( pZone->m_DansFchActivation )
            {
            pZone->m_Activee = !pZone->m_Activee ;
            g_GlobalVar.m_ZonesAerAll.EcritureFichierZonesActive() ;
            }
        }
    return ECRAN_3a_TmaAll ;
    }

// decrementation numero de zone
if ( g_GlobalVar.BoutonGauche() )
    {
    NumTma-- ;
    if ( NumTma < -1 )
        NumTma = NbZones - 1 ;
    }

// incrementation numero de zone
if ( g_GlobalVar.BoutonDroit() )
    {
    int Size = VecAffZones.size()-1 ;
    NumTma++ ;
    if ( NumTma > Size )
        NumTma = 0 ;
    }

return ECRAN_3b_TmaMod ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
CAutoPages::EtatsAuto CScreen::EcranConfimeArchIgcFch()
{
// titre
char TmpChar[] = "\n\n   Confirme\n   Archivage\n     Igc\n  Bouton GD" ;

if ( IsPageChanged() )
    ScreenRaz() ;

g_tft.setTextSize(2) ;

// titre
g_tft.setCursor( 0, 20 );
g_tft.print( TmpChar ) ;

// si changement d'ecran
g_GlobalVar.m_Screen.SetText( "Con" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "Can", 1 ) ;
g_GlobalVar.m_Screen.SetText( "Con" , 2 ) ;

if ( g_GlobalVar.BoutonCentre() )
    {
    return ECRAN_2a_ListeIgc ;
    }

// si changement d'ecran
if ( g_GlobalVar.BoutonGauche() )
    {
    g_GlobalVar.ArchiveIgc() ;
    return ECRAN_2a_ListeIgc ;
    }

// si confirme delete igc
if ( g_GlobalVar.BoutonDroit() )
    {
    g_GlobalVar.ArchiveIgc() ;
    return ECRAN_2a_ListeIgc ;
    }

return ECRAN_2b_ConfirmArchIgc ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage des parametres systemes.
CAutoPages::EtatsAuto CScreen::EcranSys()
{
if ( IsPageChanged() )
    ScreenRaz() ;

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
// vbat
m_T2SPageSysArr[PAGE_SYS_VBAT_TXT].Affiche( "vbat:" ) ;
sprintf( TmpChar , "%4.2f" , g_GlobalVar.GetBatteryVoltage() ) ;
m_T2SPageSysArr[PAGE_SYS_VBAT_VAL].Affiche( TmpChar ) ;

// defilement autre ecran
g_GlobalVar.m_Screen.SetText( "" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "Vz", 1 ) ;
g_GlobalVar.m_Screen.SetText( "" , 2 ) ;
if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {
    return ECRAN_0_Vz ;
    }

return ECRAN_6_Sys ;
}
