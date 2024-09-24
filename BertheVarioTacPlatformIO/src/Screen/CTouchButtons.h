////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.h
///
/// \brief Boutons tactiles
///
/// \date creation     : 21/09/2024
/// \date modification : 23/09/2024
///

#ifndef _TOUCHBUTTONS_
#define _TOUCHBUTTONS_

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour les boutons tactiles en modifiables
class CTouchButtons
{
public :
    void AfficheButtons() ;
    bool IsButtonPressed( int ib ) ;
    void SetText( const char * Txt3 , int ib ) ;
    void RazButtons( int button = -1 ) ;

    void SetX( int x )
        { m_XTouch = x ; } ;
    void SetY( int y )
        { m_YTouch = y ; } ;
    void SetPressed( bool pressed )
        { m_Pressed = pressed ; } ;
    bool IsPressed() const
        { return m_Pressed ; } ;

    friend class CLGFX ;

protected :
    int   m_XTouch = -1 ;       ///< coordonnees du touch
    int   m_YTouch = -1 ;       ///< coordonnees du touch
    bool  m_Pressed = false ;   ///< si est appuyé

private :
    unsigned long   m_TimePressed = 0 ;///< pour le rebond
    int             m_NbButtons = 3 ;  ///< bombre de boutons a afficher
    bool            m_PressedArr[3] ;  ///< si boutons pressés
    char            m_Intitule[3][4] ; ///< texte de bouton
} ;

#endif
