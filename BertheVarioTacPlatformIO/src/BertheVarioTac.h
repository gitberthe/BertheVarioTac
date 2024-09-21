////////////////////////////////////////////////////////////////////////////////
/// \file BertheVarioTac.h
///
/// \brief Include global du projet de vario tactile
///
/// \date creation     : 20/09/2024
/// \date modification : 21/09/2024
///


//////////////////////
// includes globaux //
//////////////////////
#include <Arduino.h>
#include <vector>


//////////////////////////
// includes du logiciel //
//////////////////////////
#include "Screen/CTouchButtons.h"
#include "Screen/CLGFX.h"
#include "Screen/CAutoPages.h"
#include "Screen/CText2Screen.h"
#include "Screen/CScreen.h"
#include "GlobalVar/CMutex.h"
#include "GlobalVar/CGlobalVar.h"
#include "Perfmon/esp32_perfmon.h"

////////////////////////
// definition des pin //
////////////////////////

// GPS
#define pinGpsRXD  (27)
#define pinGpsTXD  (-1)
