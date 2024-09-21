////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.cpp
///
/// \brief Boutons tactiles
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Lit la position du touch pad et active les boutons en consequence
void CTouchButtons::AfficheButtons()
{
uint16_t ColorFond  = TFT_WHITE ;
uint16_t ColorTexte = TFT_BLACK ;

const int HauteurBoutons = 40 ;
const int Marge = 5 ;

// pour tous les boutons
char TmpChar[25] ;
g_tft.startWrite();
int ib = 0 ;
for ( int x = 0 ; x < g_GlobalVar.m_Screen.m_Largeur ; x += g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , ib++ )
    {
    // bord du boutons
    int ColorDouble = ColorFond ;

    // verification zone clic
    if ( g_GlobalVar.m_Screen.m_XTouch > x && g_GlobalVar.m_Screen.m_XTouch < (x + g_GlobalVar.m_Screen.m_Largeur/m_NbButtons) &&
         g_GlobalVar.m_Screen.m_YTouch > (g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons) &&
         g_GlobalVar.m_Screen.m_Pressed )
        {
        ColorDouble = ColorTexte ;
        m_PressedArr[ib] = true ;
        }

    // cerclage boutons
    g_tft.drawRect( x + Marge , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons + Marge,
                    g_GlobalVar.m_Screen.m_Largeur/m_NbButtons - 2 * Marge , HauteurBoutons - 2 * Marge , ColorDouble ) ;
    g_tft.drawRect( x , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons , g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , HauteurBoutons , ColorTexte ) ;

    // position du bouton
    g_tft.setCursor( g_GlobalVar.m_Screen.m_Largeur / m_NbButtons * ib + 23  , g_GlobalVar.m_Screen.m_Hauteur - HauteurBoutons / 2 - 5 ) ;
    g_tft.setTextColor(ColorTexte) ;

    // texte du boutons
    g_tft.setTextSize(2) ;
    sprintf( TmpChar , "B%02d", ib ) ;
    g_tft.print(TmpChar);
    }
g_tft.endWrite();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Remet les bouons à zero
void CTouchButtons::RazButtons()
{
for ( int ib = 0 ; ib < m_NbButtons ; ib++ )
    m_PressedArr[ib] = false ;

delay( 100 ) ;
}
