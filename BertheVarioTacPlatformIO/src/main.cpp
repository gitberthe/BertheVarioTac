////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
///
/// \brief loop de l'application
///
/// \date creation     : 21/09/2024
/// \date modification : 21/09/2024
///

#include "BertheVarioTac.h"

CGlobalVar g_GlobalVar ;

////////////////////////////////////////////////////////////////////////////////
/// \brief setup ESP32-2432S028
void setup()
{
Serial.begin(115200);

// initialisation de l'ecran tactile
g_GlobalVar.InitScreen() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void loop()
{
g_GlobalVar.HandleTouchScreen() ;

/*
// berthe
static int count = 0 ;
count++ ;
static int old_x= 0 ;
static int old_y= 0 ;
*/

//if ( count % 100 )
//    return  ;

//tft.waitDMA() ;
g_tft.waitDisplay() ;

g_GlobalVar.AfficheButtons() ;

delay( 5 );

static int count = 0 ;
count++ ;

if ( count%60 )
    return ;

//tft.sleep() ;
//tft.powerSaveOn() ;
g_tft.startWrite();
//tft.beginTransaction();

if ( g_GlobalVar.IsButtonPressed( 0 ) )
    g_tft.fillCircle(100, 100,50,TFT_BLUE) ;
else if ( g_GlobalVar.IsButtonPressed( 1 ) )
    g_tft.fillCircle(100, 100,50,TFT_WHITE) ;
else if ( g_GlobalVar.IsButtonPressed( 2 ) )
    g_tft.fillCircle(100, 100,50,TFT_RED) ;

g_GlobalVar.RazButtons() ;

g_tft.endWrite();

//tft.clearClipRect()  ;
//tft.fillRect(0, 0,240,320,TFT_WHITE);
//tft.clearDisplay() ;

/*
if ( (count/50)%2 )
    g_tft.fillCircle(100, 100,50,TFT_RED);
else
    g_tft.fillCircle(100, 100,50,TFT_BLUE);

g_tft.drawLine( 0,0,100,100,TFT_BLACK) ;


// texte
char TmpChar[100] ;
if ( old_x != g_GlobalVar.m_XTouch || old_y != g_GlobalVar.m_YTouch )
    {
    g_tft.setTextSize(2) ;

    g_tft.setCursor(50, 300) ;
    g_tft.setTextColor(TFT_WHITE) ;
    sprintf( TmpChar , "x:%03d y:%03d", old_x , old_y ) ;
    g_tft.print(TmpChar);

    old_x = g_GlobalVar.m_XTouch ;
    old_y = g_GlobalVar.m_YTouch ;
    }

g_tft.setCursor(50, 300) ;
if ( g_GlobalVar.m_Pressed )
    g_tft.setTextColor(TFT_RED) ;
else
    g_tft.setTextColor(TFT_BLACK) ;
sprintf( TmpChar , "x:%03d y:%03d", g_GlobalVar.m_XTouch , g_GlobalVar.m_YTouch ) ;
g_tft.print(TmpChar);
//g_GlobalVar.m_Pressed = false ;

g_tft.endWrite();
*/
   //tft.powerSaveOff() ;
   //tft.wakeup() ;
   //tft.endTransaction();
}
