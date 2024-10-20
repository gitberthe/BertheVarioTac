////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.h
///
/// \brief Boutons tactiles
///
/// \date creation     : 21/09/2024
/// \date modification : 09/10/2024
///

#ifndef _TOUCHBUTTONS_
#define _TOUCHBUTTONS_

#define ANTI_REBONDS_MS 800

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour les boutons tactiles en modifiables
class CTouchButtons
{
public :
    void AfficheButtons() ;
    void HandleButtons() ;
    bool IsButtonPressed( int ib ) ;
    void SetText( const char * Txt3 , int ib ) ;
    void RazButtons( int button = -1 ) ;

    void SetX( int x )
        { m_XTouch = x ; } ;
    void SetY( int y )
        { m_YTouch = y ; } ;
    int GetX() const
        { return m_XTouch ; } ;
    int GetY() const
        { return m_YTouch ; } ;
    void SetPressed( bool pressed ) ;
    void SetFrozenDelaySec( int SecDelay ) ;
    bool IsCenterPressed() ;

    friend class CLGFX ;

protected :
    int   m_XTouch = -1 ;       ///< coordonnees du touch
    int   m_YTouch = -1 ;       ///< coordonnees du touch
    bool  m_Pressed = false ;   ///< si est appuyé en milieu d'ecran

private :
    unsigned long   m_TimePressed = 0 ;///< pour l' anti-rebond
    int             m_FrozenSec = 0 ;  ///< temps ou les boutons sont geles
    int             m_NbButtons = 3 ;  ///< bombre de boutons a afficher
    bool            m_PressedArr[3] ;  ///< si boutons pressés
    char            m_Intitule[3][4] ; ///< texte de bouton
    bool            m_CenterPressed = false ;

    const int m_HauteurBoutons = 50 ;
    const int m_Marge = 10 ;

} ;

#endif
