////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.cpp
///
/// \brief Boutons tactiles
///
/// \date creation     : 21/09/2024
/// \date modification : 04/10/2024
///

#include "../BertheVarioTac.h"


////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi si le bouton a ete appuye et le RAZ .
bool CTouchButtons::IsButtonPressed( int ib )
{
bool ret = m_PressedArr[ib] ;
m_PressedArr[ib] = false ;
return ret ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne par CGLFX.
void CTouchButtons::SetPressed( bool pressed )
{
// anti-rebond
if ( (millis()-m_TimePressed) < ANTI_REBONDS_MS )
    return ;

m_Pressed = pressed ;
m_TimePressed = millis() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief si le centre de l'ecran est appuyé
bool CTouchButtons::IsCenterPressed()
{
bool ret = m_CenterPressed ;
m_CenterPressed = false ;
return ret ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lit la position du touch pad et active les boutons en consequence
void CTouchButtons::HandleButtons()
{
// pour tous les boutons
int ib = 0 ;
for ( int x = 0 ; x < g_GlobalVar.m_Screen.m_Largeur ; x += g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , ib++ )
    {
    // verification zone clic
    if ( g_GlobalVar.m_Screen.m_XTouch > x && g_GlobalVar.m_Screen.m_XTouch < (x + g_GlobalVar.m_Screen.m_Largeur/m_NbButtons) &&
         g_GlobalVar.m_Screen.m_YTouch > (g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons) &&
         g_GlobalVar.m_Screen.m_Pressed )
        {
        // bouton gauche centre ou droit
        m_PressedArr[ib] = true ;
        // desactivation g_tft
        m_Pressed = false ;
        // desactivation centre écran
        m_CenterPressed = false ;
        return ;
        }
    }

// si pas en vol
if ( !g_GlobalVar.m_FinDeVol.IsInFlight() )
    {
    // si toujours presse alors centre de l'ecran
    if ( m_Pressed )
        {
        // centre pressé
        m_CenterPressed = true ;
        // desactivation g_tft
        m_Pressed = false ;
        }
    }
else
    {
    // centre pressé
    m_CenterPressed = false ;
    // desactivation g_tft
    m_Pressed = false ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affiche la position du touch pad et active les boutons en consequence
void CTouchButtons::AfficheButtons()
{
const uint16_t ColorFond  = TFT_BLACK ;
const uint16_t ColorTexte = TFT_WHITE ;

// pour tous les boutons
g_tft.startWrite();
int ib = 0 ;
for ( int x = 0 ; x < g_GlobalVar.m_Screen.m_Largeur ; x += g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , ib++ )
    {
    // bord du boutons
    int ColorDouble = ColorFond ;

    // cerclage boutons
    if ( m_PressedArr[ib] )
        ColorDouble = ColorTexte ;

    g_tft.fillRect( x + 10 , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons + 10 , g_GlobalVar.m_Screen.m_Largeur/m_NbButtons -20 , m_HauteurBoutons -20 , ColorFond ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons ,
                        g_GlobalVar.m_Screen.m_Largeur/m_NbButtons ,m_Marge , ColorDouble ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons ,
                        m_Marge , m_HauteurBoutons , ColorDouble ) ;
    g_tft.fillRect( x + g_GlobalVar.m_Screen.m_Largeur/m_NbButtons - m_Marge , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons ,
                        m_Marge , m_HauteurBoutons , ColorDouble ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - m_Marge ,
                        g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , m_Marge , ColorDouble ) ;
    g_tft.drawRect( x , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons , g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , m_HauteurBoutons , ColorTexte ) ;

    // position du bouton
    g_tft.setCursor( g_GlobalVar.m_Screen.m_Largeur / m_NbButtons * ib + 23  , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons / 2 - 5 ) ;
    g_tft.setTextColor(ColorTexte) ;

    // texte du boutons
    g_tft.setTextSize(2) ;
    g_tft.print(m_Intitule[ib]);
    }
g_tft.endWrite();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Remet les boutons à zero
void CTouchButtons::RazButtons( int button )
{
if ( button == -1 )
    {
    m_CenterPressed = false ;
    for ( int ib = 0 ; ib < m_NbButtons ; ib++ )
        m_PressedArr[ib] = false ;
    m_TimePressed = millis() ;
    }
else
    m_PressedArr[button] = false ;
m_Pressed = false ;
//delay( 100 ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief positionne le texte des boutons
void CTouchButtons::SetText( const char * Txt3 , int ib )
{
int len = strlen(Txt3 ) ;
for ( int ic = 0 ; ic < 4 ; ic++ )
    {
    if ( ic < len )
        m_Intitule[ib][ic] = Txt3[ic] ;
    else
        m_Intitule[ib][ic] = 0 ;
    }
}
