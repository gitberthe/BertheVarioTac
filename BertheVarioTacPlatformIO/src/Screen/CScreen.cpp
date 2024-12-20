////////////////////////////////////////////////////////////////////////////////
/// \file CScreen.cpp
///
/// \brief Definition des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 03/12/2024
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
}

////////////////////////////////////////////////////////////////////////////////
/// \brief A appeler souvent pour le touch screen
void CScreen::HandleTouchScreen()
{
lv_timer_handler(); /* let the GUI do its work */
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Raz page blanche ecran
void CScreen::ScreenRaz( bool SaufBouton )
{
if ( SaufBouton )
    g_tft.fillRect( 0 , 0 , m_Largeur , m_Hauteur - 50 , TFT_BLACK ) ;
else
    g_tft.fillRect( 0 , 0 , m_Largeur , m_Hauteur , TFT_BLACK ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage de la Vz et autres parametres de navigation.
CAutoPages::EtatsAuto CScreen::EcranVz()
{
char TmpChar[50] ;
static int count = 0 ;
static int count_bat = 0 ;
static bool ZoneHautMessageErreurLast = false ;
bool ZoneHautMessageErreur ;
count++ ;

// si message d'erreur
ZoneHautMessageErreur = g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_DEDANS ||
                        g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_LIMITE_ALTI ||
                        g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_FRONTIERE ||
                        g_GlobalVar.m_Hgt2Agl.m_ErreurFichier ;

// nouvelle page
if ( IsPageChanged() || count == 1 )
    {
    ScreenRaz() ;
    // encadrements
    g_tft.drawLine( 0 , 90 , 240 , 90 , TFT_WHITE ) ;
    g_tft.drawLine( 0 ,130 , 240 ,130 , TFT_WHITE ) ;
    g_tft.drawLine( 95 , 90 , 95 ,130 , TFT_WHITE ) ;

    g_tft.drawLine( 0 ,230 , 240 ,230 , TFT_WHITE ) ;
    g_tft.drawLine( 120 ,230 , 120 ,270 , TFT_WHITE ) ;

    // reaffichage des texts
    for ( int it = 0 ; it < m_T2SPageVzArr.size() ; it++ )
        m_T2SPageVzArr[it].ForceReaffichage() ;
    ZoneHautMessageErreurLast = !ZoneHautMessageErreur ;
    count_bat = 0 ;
    }

/////////////////////////////////////
// nom/finesse du site le plus proche
float FinesseTerrainMinimum = 99. ;
const CLocTerrain * pTerrain = g_GlobalVar.m_TerrainArr.GetTerrainProche( FinesseTerrainMinimum ) ;
const int Longueur = 11  ;
char TmpCharNomSite[] = "-----------" ;
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

// reculade/derive
uint16_t color = TFT_WHITE ;
const int x1 = 68 , x2 = 240-72;
const int y1 = 50 , y2 = 90;

// si changement de mode message erreur/terrain reculade
if ( ZoneHautMessageErreurLast != ZoneHautMessageErreur )
    {
    g_tft.fillRect( 0 , 0 , g_GlobalVar.m_Screen.m_Largeur , 89  , TFT_BLACK ) ;
    if ( !ZoneHautMessageErreur )
        {
        g_tft.drawLine( x1 , y1 , x2  , y1 , TFT_WHITE ) ;
        g_tft.drawLine( x1 , y1 , x1  , y2 , TFT_WHITE ) ;
        g_tft.drawLine( x2 , y1 , x2  , y2 , TFT_WHITE ) ;
        }
    }
ZoneHautMessageErreurLast = ZoneHautMessageErreur ;

////////////
// dans zone
if ( g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_DEDANS )
    {
    g_tft.setCursor( 0 , 0 ) ;
    g_tft.setTextSize( 3 ) ;
    g_tft.setTextColor(TFT_MAGENTA) ;
    g_tft.print( g_GlobalVar.m_ZonesAerAll.m_NomZoneDansDessous.c_str() ) ;
    if ( g_GlobalVar.m_BeepAttenteGVZone )
        CGlobalVar::BeepError() ;
    }
// limite zone alti
else if ( g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_LIMITE_ALTI )
    {
    g_tft.setCursor( 0 , 0 ) ;
    g_tft.setTextSize( 3 ) ;
    g_tft.setTextColor(TFT_MAGENTA) ;
    g_tft.print( g_GlobalVar.m_ZonesAerAll.m_NomZoneDansDessous.c_str() ) ;
    }
// limite zone frontiere
else if ( g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_FRONTIERE )
    {
    g_tft.setCursor( 0 , 0 ) ;
    g_tft.setTextSize( 3 ) ;
    g_tft.setTextColor(TFT_YELLOW) ;
    g_tft.print( g_GlobalVar.m_ZonesAerAll.m_NomZoneEnLimite.c_str() ) ;
    }
// erreur hgt file
else if ( g_GlobalVar.m_Hgt2Agl.m_ErreurFichier )
    {
    g_tft.setCursor( 0 , 0 ) ;
    g_tft.setTextSize( 3 ) ;
    g_tft.setTextColor(TFT_MAGENTA) ;
    g_tft.print("** Erreur  **** Hgt File *");
    if ( g_GlobalVar.m_BeepAttenteGVZone )
        CGlobalVar::BeepError() ;
    }
else
    {
    // nom terrain
    sprintf( TmpChar , "%s%2d" , TmpCharNomSite , (int)FinesseTerrainMinimum ) ;
    m_T2SPageVzArr[PAGE_VZ_FIN_TER].Affiche(TmpChar) ;

    // reculade
    const float DeriveMilieu = 41. ;
    float DeriveAngle = g_GlobalVar.GetDiffAngle( g_GlobalVar.m_CapGpsDeg , g_GlobalVar.m_QMC5883Mag.GetCapDegres() ) ;
    if ( fabsf(DeriveAngle) >= 90. )
        {
        sprintf( TmpChar , "\\\\R//" ) ;
        m_T2SPageVzArr[PAGE_VZ_RECULADE].Affiche(TmpChar,TFT_MAGENTA) ;
        }
    else if ( DeriveAngle >= DeriveMilieu )
        {
        sprintf( TmpChar , "  %1d>>", ((int)(fabsf(DeriveAngle)/10)) ) ;
        m_T2SPageVzArr[PAGE_VZ_RECULADE].Affiche(TmpChar,TFT_YELLOW) ;
        }
    else if ( DeriveAngle <= -DeriveMilieu )
        {
        sprintf( TmpChar , "<<%1d", ((int)(fabsf(DeriveAngle)/10)) ) ;
        m_T2SPageVzArr[PAGE_VZ_RECULADE].Affiche(TmpChar,TFT_YELLOW) ;
        }
    else
        {
        sprintf( TmpChar , " ^%1d^", ((int)(fabsf(DeriveAngle)/10)) ) ;
        m_T2SPageVzArr[PAGE_VZ_RECULADE].Affiche(TmpChar) ;
        }

    // calcul pourcentage batterie
    const float VbatBas = 3.3 ;
    const float Filtrage = 0.1 ;
    float ValBat = (g_GlobalVar.GetBatteryVoltage()-VbatBas)/(4.25-VbatBas) ;
    static float Pourcentage = ValBat ;
    Pourcentage = Pourcentage * (1.-Filtrage) + Filtrage * ValBat ;
    if ( Pourcentage > 1. )
        Pourcentage = 1. ;
    if ( Pourcentage < 0. )
        Pourcentage = 0. ;

    // tout les 5 affichages batterie
    if ( !(count_bat++%5) )
        {
        // tension batterie
        const int xb = 172 ;
        const int hb = 15 ;
        const int yb = 60 ;
        const int lb = 67 ;
        /*Pourcentage += 0.1 ;
        if ( Pourcentage > 1. )
            Pourcentage = 0. ;*/
        uint16_t color_bat = TFT_GREEN ;
        if ( Pourcentage < 0.1 )
            color_bat = TFT_RED ;
        else if ( Pourcentage < 0.35 )
            color_bat = TFT_YELLOW ;
        g_tft.fillRect( xb                    , yb , lb * Pourcentage       , hb , color_bat ) ;
        g_tft.fillRect( xb + lb * Pourcentage , yb , lb * ( 1.-Pourcentage) , hb , TFT_BLACK ) ;
        g_tft.fillRect( xb                    , yb + hb , lb , 2 , color_bat ) ;
        }
    }

///////////////
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

//////////////
// cap lettres
int Cap = g_GlobalVar.m_CapGpsDeg ;
//int Cap = g_GlobalVar.m_QMC5883Mag.GetCapDegres() ;
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

/////////////
// cap degres
sprintf( TmpChar , "%3d", Cap ) ;
m_T2SPageVzArr[PAGE_VZ_CAP_DEG].Affiche(TmpChar) ;

///////////////
// affichage VZ
float VitVert = g_GlobalVar.m_VitVertMS ;
bool SigneNeg = VitVert < 0. ;
color = TFT_BLACK ;
if ( SigneNeg )
    {
    sprintf( TmpChar , "%2.1f-" , VitVert ) ;
    color = TFT_CYAN ;
    }
else
    sprintf( TmpChar , " %2.1f " , VitVert ) ;
m_T2SPageVzArr[PAGE_VZ_VZ].Affiche(TmpChar) ;
const int bordure = 5 ;
g_tft.fillRect( bordure , 210 , g_GlobalVar.m_Screen.m_Largeur - 2 * bordure , 15 , color ) ;
g_tft.fillRect( bordure , 135 , g_GlobalVar.m_Screen.m_Largeur - 2 * bordure , 15 , color ) ;
g_tft.fillRect( bordure , 135 , 15 , 75 , color ) ;
g_tft.fillRect( g_GlobalVar.m_Screen.m_Largeur - bordure -15 , 135 , 15 , 75 , color ) ;

///////////////////////////////////////////////
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
color = TFT_WHITE ;
if ( AffichageVitesse )
    {
    m_T2SPageVzArr[PAGE_VZ_VIT_SOL].ChangeUnit('k') ;
    sprintf( TmpChar , "%6.1f" , g_GlobalVar.m_VitesseKmh ) ;
    if ( g_GlobalVar.m_VitesseKmh < 5. )
        color = TFT_MAGENTA ;
    }
else
    {
    m_T2SPageVzArr[PAGE_VZ_VIT_SOL].ChangeUnit('m') ;
    sprintf( TmpChar , "%6d", (int)(g_GlobalVar.m_TerrainPosCur.m_AltiBaro-g_GlobalVar.m_AltitudeSolHgt) ) ;
    color = TFT_BROWN ;
    }
m_T2SPageVzArr[PAGE_VZ_VIT_SOL].Affiche(TmpChar,color) ;

/////////////////////
// affichage altitude
sprintf( TmpChar , "%4d", (int)g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;
m_T2SPageVzArr[PAGE_VZ_ALTI_BARO].Affiche(TmpChar) ;

// defilement autre ecran
// si activation / desactivation beep attente Gps / Vitesse
if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )
    {
    if ( g_GlobalVar.m_BeepAttenteGVZone  )
        g_GlobalVar.m_Screen.SetText( "So-" , 0 ) ;
    else
        g_GlobalVar.m_Screen.SetText( "So+" , 0 ) ;
    }
else
    g_GlobalVar.m_Screen.SetText( "" , 0 ) ;


// si attente de vol
if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
     g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )
    g_GlobalVar.m_Screen.SetText( "Vo+" , 2 ) ;
else
    g_GlobalVar.m_Screen.SetText( "" , 2 ) ;

// si vol en cours
if ( ! g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped )
    g_GlobalVar.m_Screen.SetText( "Vo-" , 1 ) ;
else
    g_GlobalVar.m_Screen.SetText( "" , 1 ) ;

// ecran menu si pas en vol
if( g_GlobalVar.m_Screen.IsCenterPressed() && !g_GlobalVar.m_FinDeVol.IsInFlight() )
    {
    ScreenRaz() ;
    return ECRAN_8_Menu ;
    }
// desactivation/activation du son
else if ( g_GlobalVar.BoutonGauche() )
    {
    /*if ( g_GlobalVar.m_DureeVolMin == ATTENTE_VITESSE_VOL ||
         g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS ||
         g_GlobalVar.m_DureeVolMin == ATTENTE_MESSAGE_GPS )*/
        g_GlobalVar.m_BeepAttenteGVZone = ! g_GlobalVar.m_BeepAttenteGVZone ;
    return ECRAN_0_Vz ;
    }
//// page suivante
//else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
//    {}
// le bouton 2 est gere par CGps
//else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
//    return ECRAN_0_Vz ;

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
    ScreenRaz() ;
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
    g_GlobalVar.m_Screen.SetText( "H-" , 0 ) ;
g_GlobalVar.m_Screen.SetText( "", 1 ) ;
if ( ivol >= (g_GlobalVar.m_HistoVol.m_HistoDir.size() - 1) )
    g_GlobalVar.m_Screen.SetText( "   " , 2 ) ;
else
    g_GlobalVar.m_Screen.SetText( "H+" , 2 ) ;

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
    //g_GlobalVar.m_HistoVol.m_HistoDir.clear() ;
    //return ECRAN_2a_ListeIgc ;
    }

return ECRAN_1_Histo ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de modifier les champs de fichier de configuration
CAutoPages::EtatsAuto CScreen::EcranListeIgcFch()
{
std::vector<std::string> VecNomIgc ;
std::vector<int> VecTempsIgc ;

g_GlobalVar.ListeIgc(VecNomIgc,VecTempsIgc) ;

// lecture de fichier
if ( IsPageChanged() )
    {
    ScreenRaz() ;

    // texte boutons
    g_GlobalVar.m_Screen.SetText( "Wif" , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "",  1 ) ;
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
    return ECRAN_7_WifiFileMgr ;
    }
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {
    //return ECRAN_3a_TmaAll ;
    }
else if ( g_GlobalVar.m_Screen.IsButtonPressed( 2 ) )
    {
    return ECRAN_2b_ConfirmArchIgc ;
    }

return ECRAN_2a_ListeIgc ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Permet de passer en mode wifi file manager
CAutoPages::EtatsAuto CScreen::EcranWifiFileMgr()
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
    g_GlobalVar.m_ModeHttpFileMgr = true ;
    ScreenRaz() ;
    return ECRAN_7_WifiFileMgr ;
    }

if ( g_GlobalVar.BoutonCentre() )
    {
    return ECRAN_2a_ListeIgc ;
    }

return ECRAN_7_WifiFileMgr ;
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
        strcpy( TmpChar , VecZonesMod[iz]->m_pNomAff ) ;
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
        sprintf( TmpChar , "-%s" ,  VecZonesMod[iz]->m_pNomAff ) ;
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
g_GlobalVar.m_Screen.SetText( "", 1 ) ;
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
    //return ECRAN_4_CfgFch ;
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

static bool s_ChampsModified = false ;
static int  s_LastiChamps = -2 ;
static bool s_CfgFileEnMod = false ;

// à l'entree dans la page on a rien modifier
if ( IsPageChanged() )
    {
    g_GlobalVar.m_Config.ConstructVect() ;
    m_CfgFileiChamps = -1 ;
    s_ChampsModified = false ;
    s_LastiChamps = -2 ;
    }

// on modifie la luminosite en direct
g_tft.setBrightness( g_GlobalVar.GetBrightness() ) ;

// texte bouton
if ( s_CfgFileEnMod )
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
        g_GlobalVar.m_Screen.SetText( "" , 1 ) ;
        g_GlobalVar.m_Screen.SetText( "Var" , 2 ) ;
        }
    }

// selection du champs
if ( m_CfgFileiChamps == -1 )
    {
    strcpy( TmpModChar , "" ) ;
    Name = "  Editeur Cfg file" ;
    s_ChampsModified = false ;
    s_LastiChamps = -2 ;
    }
else
    g_GlobalVar.m_Config.GetChar( m_CfgFileiChamps , Name , Value ) ;

// changement de champ
if ( s_LastiChamps != m_CfgFileiChamps )
    {
    s_LastiChamps = m_CfgFileiChamps ;
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

/*// sortie ecran
if ( BoutonCent && !CfgFileEnMod && m_CfgFileiChamps == -1 )
    {
    if ( ChampsModified )
        {
        g_GlobalVar.m_Config.EcritureFichier() ;
        g_GlobalVar.m_Config.LectureFichier() ;
        }
    //return ECRAN_5_TmaDessous ;
    } */

// modification du mode modif/edition
if ( BoutonCent )
    {
    if ( m_CfgFileiChamps != -1 )
        {
        ScreenRaz() ;
        s_CfgFileEnMod = !s_CfgFileEnMod ;
        if ( s_CfgFileEnMod )
            strcpy( TmpModChar , "Modification" ) ;
        else
            {
            strcpy( TmpModChar , "Edition" ) ;
            if ( s_ChampsModified )
                {
                g_GlobalVar.m_Config.EcritureFichier() ;
                g_GlobalVar.m_Config.ConstructVect() ;
                }
            s_ChampsModified = false ;
            //s_LastiChamps = -2 ;
            //m_CfgFileiChamps = -1 ;
            }
        }
    else
        s_CfgFileEnMod = false ;
    }

// decrementation de variable
if ( BoutonGau && s_CfgFileEnMod && m_CfgFileiChamps >= 0 && m_CfgFileiChamps < g_GlobalVar.m_Config.m_LinesVect.size() )
    {
    s_ChampsModified = true ;
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
if ( BoutonDroi && s_CfgFileEnMod && m_CfgFileiChamps >= 0 && m_CfgFileiChamps < g_GlobalVar.m_Config.m_LinesVect.size() )
    {
    s_ChampsModified = true ;
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

// defilement variable gauche
if (  BoutonGau && !s_CfgFileEnMod )
    {
    if ( m_CfgFileiChamps > -1 )
        m_CfgFileiChamps-- ;
    else
        m_CfgFileiChamps = g_GlobalVar.m_Config.m_LinesVect.size()-1 ;
    }

// defilement variable droite
if ( BoutonDroi && !s_CfgFileEnMod  )
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
g_GlobalVar.m_Screen.SetText( "", 1 ) ;
g_GlobalVar.m_Screen.SetText( "" , 2 ) ;
/*if ( g_GlobalVar.m_Screen.IsButtonPressed( 1 ) )
    {
    return ECRAN_6_Sys ;
    }*/

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
        if ( ! strcmp( VecAffZones[iz]->m_pNomAff , pZone->m_pNomAff ) )
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
    g_tft.print(pZone->m_pNomAff);
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
/// \brief Confirmation archivage ficier Igc
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
    {
    // defilement autre ecran
    g_GlobalVar.m_Screen.SetText( "Cal" , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "", 1 ) ;
    g_GlobalVar.m_Screen.SetText( "Fir" , 2 ) ;

    //pinMode(BrightnessPin, INPUT); //Il faut déclarer le pin en entrée
    }

//int luminosite = analogRead(BrightnessPin);
//val = 3.3 * val / 4095. * 2. ;

ScreenRaz() ;

g_tft.setTextSize(2) ;

// date
char TmpChar[20] ;
g_tft.setCursor( 20  , 0 ) ;
sprintf( TmpChar , "%04d-%02d-%02d %02d:%02d" , g_GlobalVar.m_Annee , g_GlobalVar.m_Mois , g_GlobalVar.m_Jour ,
                                                (int)((g_GlobalVar.m_HeureSec/3600)%24),
                                                (int)((g_GlobalVar.m_HeureSec/60)%60) ) ;
g_tft.print( TmpChar ) ;

const int DeltaY = 20 ;
int x = 30 ;
int y = 35 ;
// % cpu 0
g_tft.setCursor( x  , y ) ;
g_tft.print( "cpu0:" ) ;
sprintf( TmpChar , "      %3d%c" , g_GlobalVar.m_PercentCore0 , '%' ) ;
g_tft.print( TmpChar ) ;
y += DeltaY ;
// % cpu 1
g_tft.setCursor( x  , y ) ;
g_tft.print( "cpu1:" ) ;
sprintf( TmpChar , "      %3d%c" , g_GlobalVar.m_PercentCore1 , '%' ) ;
g_tft.print( TmpChar ) ;
y += DeltaY ;
// cap magnetique
g_tft.setCursor( x  , y ) ;
g_tft.print( "mag.:" ) ;
sprintf( TmpChar , "     %4dd" , g_GlobalVar.m_QMC5883Mag.GetCapDegres() ) ;
g_tft.print( TmpChar ) ;
y += DeltaY ;
// free mem
g_tft.setCursor( x  , y ) ;
g_tft.print( "fmem:" ) ;
sprintf( TmpChar , "  %7do" , (int) esp_get_free_heap_size() ) ;
g_tft.print( TmpChar ) ;
y += DeltaY ;
// vbat
g_tft.setCursor( x  , y ) ;
g_tft.print( "vbat:" ) ;
sprintf( TmpChar , "     %4.2fv" , g_GlobalVar.GetBatteryVoltage() ) ;
g_tft.print( TmpChar ) ;
y += DeltaY ;
// numero firmware
g_tft.setCursor( x  , y ) ;
g_tft.print( "fir.:" ) ;
sprintf( TmpChar , " %s" , g_NumVersion ) ;
g_tft.print( TmpChar ) ;
y += DeltaY ;
/*// numero luminosite
g_tft.setCursor( x  , y ) ;
g_tft.print( "lum.:" ) ;
sprintf( TmpChar , " %d" , luminosite ) ;
g_tft.print( TmpChar ) ;
y += DeltaY ;*/

// ecran calibration
if ( g_GlobalVar.BoutonGauche() )
    return ECRAN_6b_CalMag ;

if ( g_GlobalVar.BoutonDroit() )
    return ECRAN_6c_TelechFirm ;

return ECRAN_6_Sys ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage des pages du logiciel. Accessible depuis ecran Vz
CAutoPages::EtatsAuto CScreen::EcranMenu()
{
if ( IsPageChanged() )
    {
    ScreenRaz() ;
    g_tft.setTextSize(2) ;

    g_GlobalVar.m_Screen.SetText( "" , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "", 1 ) ;
    g_GlobalVar.m_Screen.SetText( "" , 2 ) ;
    }

StItem ItemMenu ;
std::vector<StItem> VecMenu ;

// construction menu
const int largeur = 100 ;
const int hauteur = 45 ;
ItemMenu.m_x = 20 ;
ItemMenu.m_y = 15 ;
for ( int im = 0 ; im < 8 ; im++ )
    {
    switch ( im )
        {
        case 0 :
            ItemMenu.m_Intitule = "Vz" ;
            ItemMenu.m_Page = ECRAN_0_Vz ;
            break ;
        case 1 :
            ItemMenu.m_Intitule = "His" ;
            ItemMenu.m_Page = ECRAN_1_Histo ;
            break ;
        case 2 :
            ItemMenu.m_Intitule = "Igc" ;
            ItemMenu.m_Page = ECRAN_2a_ListeIgc ;
            break ;
        case 3 :
            ItemMenu.m_Intitule = "Tma" ;
            ItemMenu.m_Page = ECRAN_3a_TmaAll ;
            break ;
        case 4 :
            ItemMenu.m_Intitule = "Cfg" ;
            ItemMenu.m_Page = ECRAN_4_CfgFch ;
            break ;
        case 5 :
            ItemMenu.m_Intitule = "Des" ;
            ItemMenu.m_Page = ECRAN_5_TmaDessous ;
            break ;
        case 6 :
            ItemMenu.m_Intitule = "Sys" ;
            ItemMenu.m_Page = ECRAN_6_Sys ;
            break ;
        case 7 :
            ItemMenu.m_Intitule = "Wif" ;
            ItemMenu.m_Page = ECRAN_7_WifiFileMgr ;
            break ;
        default :
            ItemMenu.m_Intitule = "Def" ;
            ItemMenu.m_Page = ECRAN_0_Vz ;
        }

    VecMenu.push_back( ItemMenu ) ;
    ItemMenu.m_y += hauteur + 3 ;

    // seconde colenne
    if ( im == 4 )
        {
        ItemMenu.m_x = 130 ;
        ItemMenu.m_y = 15 ;
        }
    }

// affichage menu
for ( int im = 0 ; im < VecMenu.size() ; im++ )
    {
    StItem & Item = VecMenu[im] ;
    g_tft.setCursor( Item.m_x + largeur/2 - 15 , Item.m_y + hauteur/2 - 10 ) ;
    g_tft.print( Item.m_Intitule.c_str() ) ;
    g_tft.drawRect( Item.m_x , Item.m_y , largeur , hauteur ) ;
    }

// si bouton pressé
for ( int im = 0 ; im < VecMenu.size() ; im++ )
    {
    StItem & Item = VecMenu[im] ;
    if ( GetX() > Item.m_x && GetX() < (Item.m_x+largeur) &&
         GetY() > Item.m_y && GetY() < (Item.m_y+hauteur) &&
         IsCenterPressed() )
        {
        const int bordure = 10 ;
        g_tft.fillRect( Item.m_x , Item.m_y , largeur , bordure , TFT_WHITE ) ;
        g_tft.fillRect( Item.m_x , Item.m_y + hauteur - bordure , largeur , bordure , TFT_WHITE ) ;
        g_tft.fillRect( Item.m_x , Item.m_y , bordure , hauteur , TFT_WHITE ) ;
        g_tft.fillRect( Item.m_x + largeur - bordure , Item.m_y , bordure , hauteur , TFT_WHITE ) ;
        return Item.m_Page ;
        }
    }

return ECRAN_8_Menu ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecran calibration du capteur magnetique.
CAutoPages::EtatsAuto CScreen::EcranCalibreMagnetique()
{
if ( IsPageChanged() )
    {
    ScreenRaz() ;
    g_tft.setTextSize(3) ;

    m_CalibrationEnCours = false ;

    g_tft.setCursor( 20 , 20 ) ;
    g_tft.print( "Lancer" ) ;
    g_tft.setCursor( 20 , 50 ) ;
    g_tft.print( "Calibration" ) ;
    g_tft.setCursor( 20 , 80 ) ;
    g_tft.print( "Magnetique?" ) ;

    g_GlobalVar.m_Screen.SetText( "Ok" , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "Can", 1 ) ;
    g_GlobalVar.m_Screen.SetText( "" , 2 ) ;
    }

// cancel calibration
if ( g_GlobalVar.BoutonCentre() )
    return ECRAN_6_Sys ;

// ok calibration
if ( g_GlobalVar.BoutonGauche() )
    {
    ScreenRaz() ;
    g_tft.setTextSize(3) ;

    g_tft.setCursor( 20 , 20 ) ;
    g_tft.print( "Calibration" ) ;
    g_tft.setCursor( 20 , 50 ) ;
    g_tft.print( "en cours" ) ;

    g_GlobalVar.m_Screen.SetText( "" , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "", 1 ) ;

    m_CalibrationEnCours = true ;

    g_GlobalVar.m_QMC5883Mag.CalibrationMagnetique() ;

    return ECRAN_6_Sys ;
    }

return ECRAN_6b_CalMag ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecran telechargement firmware.
CAutoPages::EtatsAuto CScreen::EcranTelechargementFirmware()
{
if ( IsPageChanged() )
    {
    ScreenRaz() ;

    // texte boutons
    g_GlobalVar.m_Screen.SetText( "Ok" , 0 ) ;
    g_GlobalVar.m_Screen.SetText( "Can",  1 ) ;
    g_GlobalVar.m_Screen.SetText( "Ok" , 2 ) ;

    g_tft.setTextSize(2) ;

    g_tft.setCursor( 20 , 20 ) ;
    g_tft.print( "Lancer" ) ;
    g_tft.setCursor( 20 , 50 ) ;
    g_tft.print( "Telechargement" ) ;
    g_tft.setCursor( 20 , 80 ) ;
    g_tft.print( "Firmware?" ) ;
    }

// si confirmation
if ( g_GlobalVar.BoutonDroit() || g_GlobalVar.BoutonGauche() )
    {
    g_GlobalVar.m_ModeHttpOta = true ;
    ScreenRaz() ;
    return ECRAN_6_Sys ;
    }

// cancel telechagement
if ( g_GlobalVar.BoutonCentre() )
    return ECRAN_6_Sys ;


return ECRAN_6c_TelechFirm ;
}
