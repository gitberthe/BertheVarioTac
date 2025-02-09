////////////////////////////////////////////////////////////////////////////////
/// \file CGps.cpp
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 09/02/2025
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation du port serie avec les pin qui vont bien.
void CGps::InitGps()
{
Serial1.begin(9600 ,SERIAL_8N1 , pinGpsRXD , pinGpsTXD );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance les tache de gestion du Gps : acquisition serie et gestion du
/// temps de vol / fichier igc.
void CGps::LanceTacheGps( bool AvecPortSerie )
{
#ifdef _LG_DEBUG_
 Serial.println("lancement tache gps");
#endif

// tache de port serie
if ( AvecPortSerie )
    {
    g_GlobalVar.m_TaskArr[SERIAL_NUM_TASK].m_Run = true ;
    g_GlobalVar.m_TaskArr[SERIAL_NUM_TASK].m_Stopped = false ;
    xTaskCreatePinnedToCore(TacheGpsSerial, "GpsSerial", SERIAL_GPS_STACK_SIZE, this, SERIAL_GPS_PRIORITY,NULL, SERIAL_GPS_CORE);
    }

// tache de calcul du temps de vol
g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Stopped = false ;
xTaskCreatePinnedToCore(TacheGpsTempsVol, "GpsTempsVol", TEMPS_STACK_SIZE , this, TEMPS_PRIORITY, NULL, TEMPS_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui lit les caracteres du port serie Gps et traite
/// la trame.
void CGps::TacheGpsSerial(void *param)
{
while (g_GlobalVar.m_TaskArr[SERIAL_NUM_TASK].m_Run)
    {
    while (Serial1.available())
        {
        char c = Serial1.read() ;
        #ifdef GPS_DEBUG
         Serial.print(c);
        #endif
        g_GlobalVar.Push( c ) ;
        }
    delay(2) ;
    }
g_GlobalVar.m_TaskArr[SERIAL_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui calcul le temps de vol. Elle assure aussi une
/// surveillance de la position pour detection de faux vol.
/// Lancement tache enregistrement IGC.
void CGps::TacheGpsTempsVol(void *param)
{
#ifdef _LG_DEBUG_
 Serial.println("tache temps de vol lancee");
#endif

g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Stopped = false ;

// raz temps premier GGA
CTrame::m_MillisPremierGGA = 0 ;
g_GlobalVar.m_VitesseKmh = 0 ;

// affichage gps non pret
g_GlobalVar.m_DureeVolMin = ATTENTE_MESSAGE_GPS ;

// boucle d'attente premier GGA
int iboucle = 0 ;
while ( g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run )
    {
    // toutes les 1 secondes a 1hz
    delay( 1000 ) ;
    iboucle++ ;

    // temps pour zones periode aeriennes
    g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;

    // toutes les 7s beep d'attente
    bool beep = !(iboucle%m_BeepSecondes) ;

    // beep attente gps 'G'
    if ( beep && g_GlobalVar.m_BeepAttenteGVZone )
        CGlobalVar::beeper( 1100 , 100 ) ;

    // si pas de GGA attente
    if ( CTrame::m_MillisPremierGGA == 0 )
        {
        // pas de memorisation du depart de vol par bouton droit
        if ( g_GlobalVar.m_Screen.GetEtatAuto() == CAutoPages::ECRAN_0_Vz )
            g_GlobalVar.m_Screen.RazButtons(2) ;
        // pour avoir une altitude sol à -1 au début
        g_GlobalVar.m_AltitudeSolHgt = g_GlobalVar.m_TerrainPosCur.m_AltiBaro + 2 ;
        continue ;
        }
    else
        break ;
    }

// etat du GPS
g_GlobalVar.m_DureeVolMin = ATTENTE_STABILITE_GPS ;

// boucle d'attente vitesse minimale
//int iVz = 0 ;
while ( g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run )
    {
    // toutes les 1 secondes a 1hz
    delay( 1000 ) ;
    iboucle++ ;

    // toutes les 7s beep d'attente
    bool beep = !(iboucle%m_BeepSecondes) ;

    // temps pour zones periode aeriennes
    g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;

    // declenchement du vol par bouton centre si ecran 0_Vz
    if ( g_GlobalVar.m_Screen.GetEtatAuto() == CAutoPages::ECRAN_0_Vz && g_GlobalVar.BoutonCentre() )
        {
        // raz difference altitude presion/wgs84 = altitude affichée est barometrique pure
        if ( ! g_GlobalVar.m_StabGps.IsGpsStable() )
            g_GlobalVar.m_pCapteurPression->SetAltiSolUndef() ;

        // purge boutons pour eviter un arret vol dans la foulée
        g_GlobalVar.PurgeBoutons( 3 ) ;
        // on passe au vol
        break ;
        }

    // si le gps nest pas stable au moins une fois (10 secondes)
    g_GlobalVar.m_StabGps.PushGpPos4Stab() ;
    // si pas attente vitesse
    if ( g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS && ! g_GlobalVar.m_StabGps.IsAlwaysStable() )
        {
        // beep attente gps 'S'
        if ( beep && g_GlobalVar.m_BeepAttenteGVZone )
            CGlobalVar::beeper( 1500 , 100 ) ;

        // recalage altibaro
        g_GlobalVar.m_MutexVariable.PrendreMutex() ;
         g_GlobalVar.m_pCapteurPression->SetAltiSolMetres( g_GlobalVar.m_AltitudeSolHgt ) ;
        g_GlobalVar.m_MutexVariable.RelacherMutex() ;

        // si le gps n'est pas stable
        if ( ! g_GlobalVar.m_StabGps.IsGpsStable() )
            {
            g_GlobalVar.m_PileVit.ResetVit() ;
            // on reste en stabilisation gps
            continue ;
            }
        }

    // affichage gps pret
    g_GlobalVar.m_DureeVolMin = ATTENTE_VITESSE_VOL ;

    // beep attente vitesse
    g_GlobalVar.m_PileVit.PusGpsVit() ;
    if ( beep && g_GlobalVar.m_BeepAttenteGVZone )
        {
        CGlobalVar::beeper( 1500 , 100 ) ;
        CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 ) ;
        CGlobalVar::beeper( 2000 , 100 ) ;
        }

    // si debut de vol cause vitesse
    if ( g_GlobalVar.m_PileVit.IsStartFlight() )
        break ;

    // si vitesse verticale depassee pendant 3s pour declenchement igc
    if ( (g_GlobalVar.m_Screen.GetEtatAuto() == CAutoPages::ECRAN_0_Vz) && (fabs(g_GlobalVar.m_VitVertMS) >= g_GlobalVar.m_Config.m_vz_igc_ms) )
        {
        //iVz++ ;
        //if ( iVz >= g_GlobalVar.m_Config.m_temps_igc_sec )
            break ;
        }
    //else
    //    iVz = 0 ;
    }

// pour gain memoire
g_GlobalVar.GainMemoire() ;

// derniere configuration des zones
g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;

// mesure de temps de debut du vol
g_GlobalVar.m_MillisDebutVol = millis() ;

// point de vol de decollage
g_GlobalVar.m_HistoVol.m_ZDeco =
g_GlobalVar.m_HistoVol.m_ZMax = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
g_GlobalVar.m_TerrainPosDeco = g_GlobalVar.m_TerrainPosCur ;
g_GlobalVar.m_TerrainPosDeco.m_Nom = "decollage" ;

// pour calcul de distance et init histo vol
g_GlobalVar.m_HistoVol.m_LastLat = g_GlobalVar.m_TerrainPosDeco.m_Lat ;
g_GlobalVar.m_HistoVol.m_LastLon = g_GlobalVar.m_TerrainPosDeco.m_Lon ;
g_GlobalVar.m_HistoVol.m_DistanceKm = 0. ;
g_GlobalVar.m_HistoVol.m_VsMax = 0. ;
g_GlobalVar.m_HistoVol.m_VzMax = -10 ;
g_GlobalVar.m_HistoVol.m_VzMin =  10 ;

// lancement tache fichier igc
if ( g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run )
    {
    g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = true ;
    xTaskCreatePinnedToCore(TacheGpsIgc, "IgcTask", IGC_STACK_SIZE , & g_GlobalVar , IGC_PRIORITY , NULL, IGC_CORE);
    }

if ( g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run )
    {
    // bip debut enregistrement
    CGlobalVar::BeepOk() ;
    CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 ) ;
    CGlobalVar::BeepOk() ;
    CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 ) ;
    CGlobalVar::BeepOk() ;

    // pos et temps debut de stationnarite
    g_GlobalVar.m_FinDeVol.InitFinDeVol() ;
    }

// mise a jour du temps de vol toutes les secondes
while (g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run)
    {
    // a 1 hz
    delay( 1000 ) ;

    // temps de vol en minutes
    unsigned long TempsCourant = millis() ;
    TempsCourant -= g_GlobalVar.m_MillisDebutVol ;

    float DureeVolMinutes = ((float)TempsCourant) / (1000 * 60) ;
    g_GlobalVar.m_DureeVolMin = DureeVolMinutes ;

    // histo vol
    //float Distance =
    g_GlobalVar.m_HistoVol.PushHistoVol() ;

    // pour la finesse sol
    //g_GlobalVar.PushDistAlti( Distance , g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;

    // historique du vol
    g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
     g_GlobalVar.m_HistoVol.EcritureFichier( g_GlobalVar.GetIgcFileName() ) ;
    g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
    }

g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui enregistre le fichier IGC, l'altitude max , la Vs
/// max et la Vz Min/Max.
/// toutes les secondes
void CGps::TacheGpsIgc(void *param)
{
#ifdef _LG_DEBUG_
 Serial.println("tache igc lancee");
#endif

 // remise a true des beep zone aerienne si desactive lors attente gps
g_GlobalVar.m_BeepAttenteGVZone = true ;

g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped = false ;

// init fichier IGC
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
 g_GlobalVar.InitCurentIgc() ;
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;

while (g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run)
    {
    // enregistrement position dans igc
    g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
     g_GlobalVar.PushLoc2Igc() ;
    g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
    delay( 500 ) ;

    g_GlobalVar.m_FinDeVol.PushPos4FlihgtEnd() ;
    delay( 500 ) ;

    // arret du vol par bouton droit
    if ( g_GlobalVar.m_Screen.GetEtatAuto() == CAutoPages::ECRAN_0_Vz && g_GlobalVar.BoutonDroit()
         && g_GlobalVar.m_VitesseKmh < 5. && fabsf(g_GlobalVar.m_VitVertMS) < 0.4 )
        {
        // beep prise en compte relance
        CGlobalVar::beeper( 6000 , 300 ) ;
        // relance igc
        CGlobalVar::RelancerEnregistrementFichier() ;
        // purge boutons pour eviter un relance vol dans la fouléé
        g_GlobalVar.PurgeBoutons( 4 ) ;
        break ;
        }

    // si fin de vol
    if ( g_GlobalVar.m_FinDeVol.IsFlightEnd() )
        {
        CGlobalVar::RelancerEnregistrementFichier() ;
        break ;
        }
    }

g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = false ;
g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped = true ;

while( true )
    vTaskDelete(NULL) ;
}
