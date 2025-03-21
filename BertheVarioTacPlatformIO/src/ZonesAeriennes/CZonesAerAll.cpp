////////////////////////////////////////////////////////////////////////////////
/// \file CZonesAerAll.cpp
///
/// \brief
///
/// \date creation   : 23/03/2024
/// \date 25/09/2024 : refonte calcul des zones Tma...
/// \date 25/11/2024 : la compression du nombre de points de zone ce fait dans
///                    BVTZoneAerienne.
/// \date 25/11/2024 : ajout de la compression des float en short et lz4.
/// \date 20/03/2025 : modification
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
CZonesAerAll::CZonesAerAll()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Detruit toutes les zones; libere de la place pour le file manager qui
/// bloque autrement.
void CZonesAerAll::DeleteAll()
{
// zones en tableaux
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer * pZoneAer = m_ZonesArr[iz] ;
    delete pZoneAer ;
    }

// tableau de zones
m_NbZones = 0 ;
delete [] m_ZonesArr ;
m_ZonesArr = NULL ;

// buffer de compression
if ( CZoneAer::ms_compressed_data_lz4 != NULL )
    {
    delete [] CZoneAer::ms_compressed_data_lz4 ;
    CZoneAer::ms_compressed_data_lz4 = NULL ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Positionne l'altitude des zones suivant leur periode et la date courante.
void CZonesAerAll::SetDatePeriode()
{
SetDateTU( g_GlobalVar.m_Annee, g_GlobalVar.m_Mois, g_GlobalVar.m_Jour, 0. ) ;
m_IsWeekEnd = IsWeekEnd() ;
float JDCur = GetJD_TU() ;

/*Serial.print( g_GlobalVar.m_Annee ) ;
Serial.print( g_GlobalVar.m_Mois ) ;
Serial.println( "CZonesAerAll::SetDatePeriode()" ) ;*/

// pour toutes les zones
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer & ZoneAer = *m_ZonesArr[iz] ;

    // type de zone
    ZoneAer.SetTypeZone() ;

    // si zone sans periode
    if ( ZoneAer.m_pDerogFfvl == NULL )
        continue ;

/*Serial.print( g_GlobalVar.m_Annee ) ;
Serial.print( g_GlobalVar.m_Mois ) ;
Serial.println( "CZonesAerAll::SetDatePeriode()" ) ;*/

    // test si dans periode
    ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_BASSE ;
    CDate DateDebut( g_GlobalVar.m_Annee , ZoneAer.m_pDerogFfvl->m_PeriodeDebutMois , ZoneAer.m_pDerogFfvl->m_PeriodeDebutJour , 0. ) ;
    CDate DateDefin( g_GlobalVar.m_Annee , ZoneAer.m_pDerogFfvl->m_PeriodeFinMois ,   ZoneAer.m_pDerogFfvl->m_PeriodeFinJour , 24. ) ;
    if ( (DateDebut.GetJD_TU() <= JDCur) && (JDCur <= DateDefin.GetJD_TU()) )
        {
        // positionnement alti fonction periode
        if ( m_IsWeekEnd )
            ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_PERIODE_WEEKEND ;
        else
            ZoneAer.m_pDerogFfvl->m_AltiAPrendreEnCompte = ALTI_PERIODE_SEMAINE ;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des 3 fichier, zones, activation et periode.
void CZonesAerAll::LectureFichiers()
{
LectureFichierZonesAer() ;
LectureFichierZonesActive() ;
LectureFichierZonesPeriode() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture du fichier zonesaer.txt
void CZonesAerAll::LectureFichierZonesAer()
{
//return ;
const int TaillMaxChar = 20000 ; // taille buffer
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// allocation pour 1000 points maximum
if ( CZoneAer::ms_compressed_data_lz4 != NULL )
    delete [] CZoneAer::ms_compressed_data_lz4 ;
CZoneAer::ms_max_dst_size = LZ4_compressBound( MAX_POINTS_ZONE * 2 * sizeof(short) );
CZoneAer::ms_compressed_data_lz4 = new char [CZoneAer::ms_max_dst_size] ;

// ouverture fichier
m_File = SD.open(NOM_FCH_ZONE_AER);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(m_File.available())
    {
    char c = m_File.read() ;
    if ( c != '\n' )
        {
        // si taille chaine maximum on empile plus dans le buffer
        if ( ic < TaillMaxChar )
            TmpChar[ic++] = c ;
        }
    else
        {
        // si fin de ligne on traite le buffer
        TmpChar[ic++] = 0 ;

        // traitement du buffer si memoire 30k free mem
        if ( esp_get_free_heap_size() > 30*1024 )
            {
            CZoneAer * pZone = TraiteBufferZoneAer( TmpChar ) ;
            if ( pZone != NULL )
                {
                pZone->CompressZone() ;
                pZone->FreeFloat() ;
                }
            }
        else
            g_GlobalVar.BeepError() ;

        ic = 0 ;
        }
    }

// pour la derniere zone
CZoneAer * pZone = TraiteBufferZoneAer( TmpChar ) ;
if ( pZone != NULL )
    {
    pZone->CompressZone() ;
    pZone->FreeFloat() ;
    }

m_File.close();

delete [] TmpChar ;

// reallocation du buffer de compression de zone
delete [] CZoneAer::ms_compressed_data_lz4 ;
CZoneAer::ms_max_dst_size = LZ4_compressBound( CZoneAer::ms_MaxNombrePtsZone * 2 * sizeof(short) );
CZoneAer::ms_compressed_data_lz4 = new char [CZoneAer::ms_max_dst_size] ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cette fonction traite une ligne texte de zone et la met sous forme binaire
/// dans le tableau m_ZonesArr.
/// Calcul aussi le barycentre et le rayon max de la zone pour une recheche rapide.
CZoneAer * CZonesAerAll::TraiteBufferZoneAer( char * buff )
{
// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' )
    {
    *buff = 0 ;
    return NULL ;
    }

/*// on ne prend pas en compte les zone 1.20, 5.20, 4.20...
//if ( strstr( pChar , ".20" ) )
//    return NULL ;
if ( strstr( pChar , " 5 App" ) )
    return NULL ;
if ( strstr( pChar , " 4 App" ) )
    return NULL ;
if ( strstr( pChar , " 2 App" ) )
    return NULL ;*/

// nouvelle zone
CZoneAer * pZone = new CZoneAer ;

// allocation nouveau tableau de zones plus grand et recopie de l'ancien contenu
if ( m_ZonesArr == NULL )
    {
    m_ZonesArr = new CZoneAer * [1] ;
    m_NbZones = 1 ;
    }
else
    {
    CZoneAer ** pOldArr = m_ZonesArr ;
    m_ZonesArr = new CZoneAer * [++m_NbZones] ;
    memcpy( m_ZonesArr , pOldArr , (m_NbZones-1) * sizeof(CZoneAer *) ) ;
    delete [] pOldArr ;
    }
// ajout de la nouvelle zone au tableau
m_ZonesArr[m_NbZones-1] = pZone ;

// zone protege PROTECT ou FFVL-Prot dans chaine
bool IsProtect = ( (*pChar == 'P') && (strstr( pChar , "PROTECT" ) != NULL)) ||
                 ( (*pChar == 'F') && (strstr( pChar , "FFVL-Prot" ) != NULL)) ;
if ( IsProtect )
    {
    // si m/sol trouve
    char * pCharFin = strstr( pChar , "m/sol" ) ;
    if ( pCharFin != NULL )
        {
        char TmpBuff[500] ;
        strcpy( TmpBuff , pChar ) ;
        *pCharFin = 0 ;
        // on recule jusqu'a l'espace
        while( *pCharFin != ' ' && *pCharFin != '(' && pCharFin != pChar )
            pCharFin-- ;
        // on ravance d'un coup
        pCharFin++ ;
        // hauteur sol
        pZone->m_HauteurSolZoneProtege = atoi( pCharFin ) ;
        // si probleme de conversion hauteur max
        if ( pZone->m_HauteurSolZoneProtege == 0 )
            pZone->m_HauteurSolZoneProtege = 1000 ;
        }
    // 1000 metres par defaut
    else
        pZone->m_HauteurSolZoneProtege = 1000 ;
    }

// nom de zone
std::string NomOri = pChar ;

// zone protegee on enleve des chaines
int iespacemax = 3 ;
char NomRND[]  = "Reserve naturelle nationale de " ;
char NomRNDV[] = "Reserve naturelle nationale de la Vallee de " ;
char NomProtect[] = "PROTECT " ;
char NomFFVL[] = "FFVL-Prot " ;
// zone "FFVL"
if ( (pChar[0] == 'F') && strstr( pChar , NomFFVL ) )
    pChar += strlen( NomFFVL ) ;
// zone "PROTECT"
if ( (pChar[0] == 'P') && strstr( pChar , NomProtect ) )
    pChar += strlen( NomProtect ) ;
// reserve naturelle nationale de la vallee
char * pTmpChar = strstr( pChar , NomRNDV ) ;
if ( pTmpChar != NULL )
    {
    pChar += strlen( NomRNDV ) ;
    iespacemax = 1 ;
    }
// reserve naturelle nationale de
pTmpChar = strstr( pChar , NomRND ) ;
if ( pTmpChar != NULL )
    {
    pChar += strlen( NomRND ) ;
    iespacemax = 1 ;
    }
std::string NomAff = pChar ;

// formattage nom de zone 3 champs
int iespace = 0 ;
for ( int ic = 0 ; ic < NomAff.size() ; ic++ )
    {
    // premier (
    if ( NomAff[ic] == '(' )
        {
        NomAff.resize( ic ) ;
        break ;
        }
    // trois champs max
    else if ( (NomAff[ic] == ' ' || NomAff[ic] == '-') && ++iespace >= iespacemax )
        {
        NomAff.resize( ic ) ;
        break ;
        }
    }
// on enleve le dernier blanc
if ( NomAff[NomAff.size()-1] == ' ' )
    NomAff.resize(NomAff.size()-1) ;

pZone->m_pNomAff = new char [NomAff.size() + 1 ] ;
strcpy( pZone->m_pNomAff , NomAff.c_str() ) ;

// altitude de basse zone
pChar = strtok( NULL , ";" ) ;
pZone->m_AltiBasse = atoi( pChar ) ;

// ajout des points du polygone
pChar = strtok( NULL , ";," ) ;
std::vector<CZoneAer::st_coord_poly> VecPoly ;
// reserve pour une grande zone
VecPoly.reserve( MAX_POINTS_ZONE ) ;
while ( pChar != NULL )
    {
    // nouveau point
    CZoneAer::st_coord_poly pts ;

    // longitude
    pts.m_Lon = atof( pChar ) ;
    pChar = strtok( NULL , ";," ) ;

    // latitude
    pts.m_Lat = atof( pChar ) ;
    pChar = strtok( NULL , ";," ) ;

    // ajout au vecteur
    VecPoly.push_back( pts ) ;

    //Serial.println( m_NbZones ) ;
    //Serial.println( esp_get_free_heap_size() ) ;
    //if ( VecPoly.size() > 200 )
    //    break ;
    }

// recopie du vecteur de points vers le tableau de points
pZone->m_PolyStLaLoArr = new CZoneAer::st_coord_poly [ VecPoly.size() ] ;
memcpy( pZone->m_PolyStLaLoArr , VecPoly.data() , VecPoly.size() * sizeof( CZoneAer::st_coord_poly ) ) ;
pZone->m_NbStLaLoPts = VecPoly.size() ;

// calcul du barycentre
CPolygone::CalcBarycentre( pZone->m_PolyStLaLoArr , pZone->m_NbStLaLoPts , pZone->m_Barycentre ) ;

// calcul du rayon max
pZone->m_RayonMetre = 0. ;
for ( int is = 0 ; is < pZone->m_NbStLaLoPts ; is++ )
    {
    const CZoneAer::st_coord_poly & PtsCour = pZone->m_PolyStLaLoArr[is] ;
    float dist = sqrtf( powf(pZone->m_Barycentre.m_Lat-PtsCour.m_Lat,2) + powf(pZone->m_Barycentre.m_Lon-PtsCour.m_Lon,2) ) * 60. * UnMileEnMetres  ;
    if ( dist > pZone->m_RayonMetre )
        pZone->m_RayonMetre = dist ;
    }

// pour prochain appel, buffer vide
*buff = 0 ;

// comptage du nombre de points maximum
if ( CZoneAer::ms_MaxNombrePtsZone < pZone->m_NbStLaLoPts )
    CZoneAer::ms_MaxNombrePtsZone = pZone->m_NbStLaLoPts ;

// compression / decompression
//pZone->CompressZone() ;
//pZone->FreeFloat() ;
return pZone ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture fichier periode de zone
void CZonesAerAll::LectureFichierZonesPeriode()
{
const int TaillMaxChar = 1000 ;
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
m_File = SD.open(NOM_FCH_ZONE_PER);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(m_File.available())
    {
    char c = m_File.read() ;
    if ( c != '\n' )
        {
        // si taille chaine maximum on empile plus dans le buffer
        if ( ic < TaillMaxChar )
            TmpChar[ic++] = c ;
        }
    else
        {
        // si fin de ligne on traite le buffer
        TmpChar[ic++] = 0 ;
        TraiteBufferZonePeriode( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZonePeriode( TmpChar ) ;

m_File.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite ligne periode de zone
void CZonesAerAll::TraiteBufferZonePeriode( char * buff )
{
// comptage des champs pour le plantage strtok
int countpv = 0 ;
int len = strlen( buff ) ;
for ( int ic = 0 ; buff != NULL && ic < len ; ic++ )
    {
    if ( buff[ic] == ';' )
        countpv++ ;
    }

// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' || *pChar == '\n' || countpv < 5 )
    {
    *buff = 0 ;
    return ;
    }

std::string NomOri = pChar ;
//NomOri += ' ' ;     // caractere de fin de champ pour recherche sans bug
std::string NomAff = strtok( NULL ,";");
std::string PeriodeDeb = strtok( NULL ,";");
std::string PeriodeFin = strtok( NULL ,";");
std::string AltiSemaine = strtok( NULL ,";");
std::string AltiWeekEnd = strtok( NULL ,";");

std::string JourDeb = "" ;
std::string MoisDeb = "" ;
if ( PeriodeDeb != "-" && PeriodeDeb != "" )
    {
    JourDeb = strtok( & PeriodeDeb[0] , "-" ) ;
    MoisDeb = strtok( NULL , "-" ) ;
    }

std::string JourFin = "" ;
std::string MoisFin = "" ;
std::string AltiSemaineStr = "" ;
std::string AltiWeekEndStr = "" ;
if ( PeriodeFin != "-" && PeriodeFin != "" )
    {
    JourFin = strtok( & PeriodeFin[0] , "-" ) ;
    MoisFin = strtok( NULL , "-" ) ;
    AltiSemaineStr = strtok( & AltiSemaine[0] , "-" ) ;
    AltiWeekEndStr = strtok( & AltiWeekEnd[0] , "-" ) ;
    }


for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer * pZone = m_ZonesArr[iz] ;

    // si pas de zone
    if ( pZone == NULL )
        {
        Serial.println("zone aerienne non trouvée periode") ;
        *buff = 0 ;
        return ;
        }

    // si pas bon nom de zone
    if ( strcmp( pZone->m_pNomAff , NomOri.c_str() ) )
        continue ;

    // recopie nom pour affiche
    if ( NomAff != "-" && NomAff != "" )
        {
        delete [] pZone->m_pNomAff ;
        pZone->m_pNomAff = new char [ NomAff.size() + 1 ] ;
        strcpy( pZone->m_pNomAff , NomAff.c_str() ) ;
        }
    /*Serial.print(pZone->m_NomOri.c_str()) ;
    Serial.print("|") ;
    Serial.println(pZone->m_pNomAff) ;*/

    // periode debut
    if ( JourDeb != "" )
        {
        if ( pZone->m_pDerogFfvl == NULL )
            pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
        pZone->m_pDerogFfvl->m_PeriodeDebutJour = atoi( JourDeb.c_str() ) ;
        pZone->m_pDerogFfvl->m_PeriodeDebutMois = atoi( MoisDeb.c_str() ) ;
        }

    // periode fin
    if ( JourFin != "" )
        {
        if ( pZone->m_pDerogFfvl == NULL )
            pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
        pZone->m_pDerogFfvl->m_PeriodeFinJour = atoi( JourFin.c_str() ) ;
        pZone->m_pDerogFfvl->m_PeriodeFinMois = atoi( MoisFin.c_str() ) ;
        }

    // alti semaine
    if ( AltiSemaineStr != "" )
        {
        if ( pZone->m_pDerogFfvl == NULL )
            pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
        pZone->m_pDerogFfvl->m_AltiBassePeriodeSemaine = atoi( AltiSemaineStr.c_str() ) ;
        }

    // alti week-end
    if ( AltiWeekEndStr != "" )
        {
        if ( pZone->m_pDerogFfvl == NULL )
            pZone->m_pDerogFfvl = new CZoneAerDerogFfvl ;
        pZone->m_pDerogFfvl->m_AltiBassePeriodeWeekEnd = atoi( AltiWeekEnd.c_str() ) ;
        }
    }

// pour prochain appel
*buff = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture fichier activation de zone
void CZonesAerAll::EcritureFichierZonesActive()
{

File Fch ;

// ouverture fichier
Fch = SD.open(NOM_FCH_ZONE_ACT, FILE_WRITE , true );
if (!Fch)
    {
    Serial.print( "erreur creation fichier zone actice" ) ;
    return ;
    }

// debut de fichier
Fch.seek(0) ;

std::vector<std::string> VecNomDejaEcrit ;

// ecriture des zone
for ( int iz = 0 ; iz < m_NbZones ; iz++ )
    {
    // zone en memoire
    const CZoneAer * pZone = m_ZonesArr[iz] ;
    if ( !pZone->m_DansFchActivation )
        continue ;

    // verif si pas deja fait
    bool DejaFait = false ;
    for ( long iv = 0 ; iv < VecNomDejaEcrit.size() ; iv++ )
        {
        if ( VecNomDejaEcrit[iv] == pZone->m_pNomAff )
            {
            DejaFait = true ;
            break ;
            }
        }


    // si deja ecrit dans le fichier
    if ( DejaFait )
        continue ;

    // empilement pour suivit deja fait
    VecNomDejaEcrit.push_back( pZone->m_pNomAff ) ;

    // ecriture fichier
    Fch.print( pZone->m_pNomAff ) ;
    Fch.print( ";" ) ;
    if ( pZone->m_Activee )
        Fch.print( "1" ) ;
    else
        Fch.print( "0" ) ;
    Fch.println( ";" ) ;
    }

Fch.close() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture fichier activation de zone
void CZonesAerAll::LectureFichierZonesActive()
{
const int TaillMaxChar = 1000 ;
char * TmpChar = new char [TaillMaxChar+1] ;
int ic = 0 ;

// ouverture fichier
m_File = SD.open(NOM_FCH_ZONE_ACT);
if ( !m_File )
    {
    Serial.println("Failed to open file for reading");
    delete [] TmpChar ;
    return;
    }

// lecture fichier
while(m_File.available())
    {
    char c = m_File.read() ;
    if ( c != '\n' )
        {
        // si taille chaine maximum on empile plus dans le buffer
        if ( ic < TaillMaxChar )
            TmpChar[ic++] = c ;
        }
    else
        {
        // si fin de ligne on traite le buffer
        TmpChar[ic++] = 0 ;
        TraiteBufferZoneActive( TmpChar ) ;
        ic = 0 ;
        }
    }

// pour la derniere zone
TraiteBufferZoneActive( TmpChar ) ;

m_File.close();

delete [] TmpChar ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Traite ligne activation zone.
void CZonesAerAll::TraiteBufferZoneActive( char * buff )
{
// test fin de ligne ou commentaire
char * pChar = strtok( buff , ";" ) ;
if ( pChar == NULL || *pChar == 0 || *pChar == '#' || *pChar == '\n' )
    return ;

// nom de zone
std::string NomOri = pChar ;

// activation de zone
char * pActivation = strtok( NULL ,";") ;
if ( pActivation == NULL )
    return ;
std::string Activation = pActivation ;

// pour toutes les zones
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;
    if ( strstr( Zone.m_pNomAff , NomOri.c_str() ) )
        {
        CZoneAer * pZone = m_ZonesArr[iz] ;

        // activation
        if ( Activation == "0" )
            pZone->m_Activee = false ;
        else
            pZone->m_Activee = true ;

        pZone->m_DansFchActivation = true ;
        }
    }

*buff = 0 ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Cherche la zone dans le tableau par rapport a son nom d'origine 3 champs.
CZoneAer * CZonesAerAll::Find( const char * NomOri )
{
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    const CZoneAer & Zone = *m_ZonesArr[iz] ;
    if ( ! strcmp( Zone.m_pNomAff , NomOri ) )
        return m_ZonesArr[iz] ;
    }

return NULL ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri croissant de nom pour ecran gestion zone
void CZonesAerAll::TriZonesNom()
{
CSortArray::m_pArr = m_ZonesArr ;
CSortArray::m_Size = m_NbZones ;
CZoneAer::TrierPar( CZoneAer::TriParNom ) ;
TriSchellMetzner() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction de tri de vector.
bool MySortFunction (const CZoneAer *p1,const CZoneAer *p2)
{
return ((*p1)<(*p2));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri altitude croissante pour determination
void CZonesAerAll::TriZonesParAltitude( std::vector<const CZoneAer *> & VecZones )
{
/*CSortArray::m_pArr = & VecZones[0] ;
CSortArray::m_Size = VecZones.size() ;
CZoneAer::TriParNom( false ) ;
TriSchellMetzner(true) ; */
CZoneAer::TrierPar( CZoneAer::TriParAltitude ) ;
std::sort(VecZones.begin(), VecZones.end(),MySortFunction);
//std::reverse(VecZones.begin(),VecZones.end());
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur aux zones necessaires en prenant en compte la derogation
/// et l'activabilite de la zone
void CZonesAerAll::ClasseZoneRetenueAltitude( std::vector<const CZoneAer *> & VecZonesIn )
{
std::vector<const CZoneAer *> VecZonesRet ;

// tri par altitude
TriZonesParAltitude( VecZonesIn ) ;

// tri suivant la derogation
for ( size_t iz = 0 ; iz < VecZonesIn.size()  ; iz++ )
    {
    // zone activable pris en compte apres
    if ( VecZonesIn[iz]->m_DansFchActivation )
        continue ;
    // si zone avec derogation, on efface les autres avant
    if ( VecZonesIn[iz]->HavePeriod() )
        VecZonesRet.clear() ;
    // ajout de la zone
    VecZonesRet.push_back( VecZonesIn[iz] ) ;
    }

// rajout des zones activables
for ( size_t iz = 0 ; iz < VecZonesIn.size()  ; iz++ )
    if ( VecZonesIn[iz]->m_DansFchActivation )
        VecZonesRet.push_back( VecZonesIn[iz] ) ;

// recopie des zones
VecZonesIn = VecZonesRet ;

// tri par altitude
TriZonesParAltitude( VecZonesIn ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul si l'on est dans une zone avec le nom de la zone et son altitude
/// \param m_NomZone [out] : nom de la zone en presence ou proche
/// \param m_DansDessousUneZone [out] : indique si on est dans ou au dessous d'une zone
/// \param m_NomZoneDansDessous [out] :  nom de la zone de laquelle on est dedans ou endessous
/// \param m_LimiteZone [out] : si en limite de zone
/// \param m_NomZoneEnLimite [out] : nom de la zone en limite
void CZonesAerAll::CalcZone()
{
m_Plafond4Valid = 100000 ;
std::string RetStrIn = "" ;
int         RetNbrIn = ZONE_EN_DEHORS ;
std::string RetStrLimite = "" ;
int         RetNbrLimite = ZONE_EN_DEHORS ;
char        TmpChar[100] ;

// test dans la zone, position courante
CZoneAer::st_coord_poly PtsEnCours ;
PtsEnCours.m_Lat = g_GlobalVar.m_TerrainPosCur.m_Lat ;
PtsEnCours.m_Lon = g_GlobalVar.m_TerrainPosCur.m_Lon ;

#ifdef TMA_DEBUG
SetDate( 2024 , 3, 31 ) ;
//SetDate( 2024 , 2, 25 ) ;
g_GlobalVar.m_TerrainPosCur.m_AltiBaro = 1500 ;
g_GlobalVar.m_Config.m_XYMargin = 3000. ;

//PtsEnCours.m_Lat = 45.773785;   // pdd
//PtsEnCours.m_Lon = 2.96898;   // pdd
//PtsEnCours.m_Lat = 45.537966;   // saurier
//PtsEnCours.m_Lon = 3.04627;   // saurier
PtsEnCours.m_Lat = 45.331671;   // anzat-le-luguet
PtsEnCours.m_Lon = 3.041811;   // anzat-le-luguet

//PtsEnCours.m_Lat = 45.52730;   // sancy zone protege
//PtsEnCours.m_Lon = 2.81738;   // sancy zone protege

#endif

// si test wifi positionnement altitude sol
if ( g_GlobalVar.m_ModeHttpFileMgr )
    g_GlobalVar.m_AltitudeSolHgt = g_GlobalVar.m_Hgt2Agl.GetGroundZ( g_GlobalVar.m_TerrainPosCur.m_Lon , g_GlobalVar.m_TerrainPosCur.m_Lat ) ;

// pour toutes les zones, determination si dans surface
std::vector<const CZoneAer *> VecZoneInArea ;   // zones normale comme TMA ou RTBA
const CZoneAer * pZoneProtegee = NULL ;             // zone protege
for ( long iz = 0 ; iz < m_NbZones ; iz++ )
    {
    CZoneAer & Zone = *m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !Zone.m_Activee )
        continue ;

    // dans le du rayon
    float DistBaryMetres = sqrtf( powf(Zone.m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(Zone.m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayon = DistBaryMetres < Zone.m_RayonMetre ;

    // si pas dans le rayon
    if ( ! DansLeRayon )
        continue ;

    // decompression de la zone
    Zone.UnCompressZone() ;

    // dedans la surface
    bool IsInArea = CPolygone::IsIn( Zone.m_PolyStLaLoArr , Zone.m_NbStLaLoPts , PtsEnCours ) ;

    // liberation des float
    Zone.FreeFloat() ;

    // si dedans la surface
    if ( IsInArea )
        {
        // zone protege
        if ( Zone.IsProtect() )
            pZoneProtegee = & Zone ;
        // zone normale
        else
            VecZoneInArea.push_back( & Zone ) ;
        }
    }

// classement reduction zone
ClasseZoneRetenueAltitude( VecZoneInArea ) ;

// cas des zones normales
const CZoneAer * pZoneIn = NULL ;
if ( VecZoneInArea.size() == 0 )
    {
    RetStrIn = "aucune_zone" ;
    RetNbrIn = ZONE_EN_DEHORS ;
    }
else
    {
    // zone la plus basse
    pZoneIn = VecZoneInArea[0] ;

    // variables de zone
    int  PlafondZone = pZoneIn->GetAltiBasse() ;

    ////////////
    // si corent
    if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneCorent )
        {
        m_Plafond4Valid = PlafondZone ;

        // si dedans
        if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaro > m_Plafond4Valid) &&
             (g_GlobalVar.m_TerrainPosCur.m_AltiBaro > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone())) )
            {
            sprintf( TmpChar , "In %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            }
        // dessous
        else
            {
            sprintf( TmpChar , "Be %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DESSOUS ;
            RetStrIn = TmpChar ;
            }

        // si limite alti
        if ( ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > m_Plafond4Valid) &&
             ((g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone())) )
            {
            sprintf( TmpChar , "Al %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrLimite = ZONE_LIMITE_ALTI ;
            RetStrLimite = TmpChar ;
            }
        }
    ///////////////
    // si saint yan
    else if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneStYan )
        {
        m_Plafond4Valid = PlafondZone ;

        // si l'on est en pas en week end
        if ( ! m_IsWeekEnd )
            {
            // si dedans
            if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone()) )
                {
                sprintf( TmpChar , "In %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
                RetNbrIn = ZONE_DEDANS ;
                RetStrIn = TmpChar ;
                }
            // dessous
            else
                {
                sprintf( TmpChar , "Be %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
                RetNbrIn = ZONE_DESSOUS ;
                RetStrIn = TmpChar ;
                }

            // si limite alti
            if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > (g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone()) )
                {
                sprintf( TmpChar , "Al %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
                RetNbrLimite = ZONE_LIMITE_ALTI ;
                RetStrLimite = TmpChar ;
                }
            }
        }
    ////////////////////
    // si zone generique
    else
        {
        // si dedans
        if ( PlafondZone < g_GlobalVar.m_TerrainPosCur.m_AltiBaro )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "In %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DEDANS ;
            RetStrIn = TmpChar ;
            }
        // en dessous
        else
            {
            m_Plafond4Valid = pZoneIn->GetAltiBasse() ;
            sprintf( TmpChar , "Be %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrIn = ZONE_DESSOUS ;
            RetStrIn = TmpChar ;
            }

        // si limite altitude
        if ( (g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > PlafondZone )
            {
            m_Plafond4Valid = PlafondZone ;
            sprintf( TmpChar , "Al %s al:%4dm" , pZoneIn->m_pNomAff , m_Plafond4Valid ) ;
            RetNbrLimite = ZONE_LIMITE_ALTI ;
            RetStrLimite = TmpChar ;
            }
        }
    }

// liberation memoire
VecZoneInArea.clear() ;
VecZoneInArea.shrink_to_fit() ;

// si zone protege et pas deja dedans une comme TMA ou activable
int HauteurSolZoneProtegee = 1000 ;
int PlafondZoneProtegee = g_GlobalVar.m_AltitudeSolHgt+HauteurSolZoneProtegee ;
if ( pZoneProtegee != NULL && RetNbrIn != ZONE_DEDANS )
    {
    // hauteur zone protegee
    HauteurSolZoneProtegee = pZoneProtegee->GetHauteurSolZoneProtect() ;

    // actualisation plafond zone protege
    PlafondZoneProtegee = g_GlobalVar.m_AltitudeSolHgt+HauteurSolZoneProtegee ;

    // si dedans
    if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro < PlafondZoneProtegee )
        {
        m_Plafond4Valid = HauteurSolZoneProtegee ;
        RetNbrIn = ZONE_DEDANS ;
        sprintf( TmpChar , "Zp %s al:%4dm" , (*pZoneProtegee).m_pNomAff , m_Plafond4Valid ) ;
        RetStrIn = TmpChar ;
        }
    // si limite
    else if ( g_GlobalVar.m_TerrainPosCur.m_AltiBaro < (PlafondZoneProtegee + g_GlobalVar.m_Config.m_AltiMargin) )
        {
        m_Plafond4Valid = HauteurSolZoneProtegee ;
        RetNbrLimite = ZONE_LIMITE_ALTI ;
        sprintf( TmpChar , "Al %s al:%4dm" , (*pZoneProtegee).m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        }
    }

// variables de sortie zone en cours
m_Mutex.PrendreMutex() ;
 m_DansDessousUneZone = RetNbrIn ;
 m_NomZoneDansDessous = RetStrIn ;
m_Mutex.RelacherMutex() ;

// raz des variables distance prochaine zone et mise a jour distance altitude
if ( pZoneIn != NULL )
    {
    int PlafondZone = pZoneIn->GetAltiBasse() ;
    int DistAltCurZone = PlafondZone - g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;

    //////////////////////////////////////////////
    // si corent prise en compte de l'altitude sol
    if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneCorent )
        {
        int AltitudeSolPlus300 = g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone() ;
        int DistAltSol = AltitudeSolPlus300 - g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
        m_DistAltCurZone = std::max( DistAltSol , DistAltCurZone ) ;
        }
    //////////////////////////////////////////////
    // si st yan prise en compte de l'altitude sol
    else if ( pZoneIn->GetTypeZone() == CZoneAer::ZoneStYan )
        {
        int AltitudeSolPlus610 = g_GlobalVar.m_AltitudeSolHgt + pZoneIn->GetAltiSolZone() ;
        int DistAltSol = AltitudeSolPlus610 - g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
        m_DistAltCurZone = DistAltSol ;
        }
    else
        m_DistAltCurZone = DistAltCurZone ;
    }
else
    m_DistAltCurZone = 9999 ;

// determination des zones proches et à l'altitude de croisement dans les 1.5 km
std::vector<CZoneAer *> VecZoneProchesXY ;   // zones proches
for ( int iz = 0 ; iz < m_NbZones; iz++ )
    {
    CZoneAer * pZoneXY = m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !pZoneXY->m_Activee )
        continue ;

    // pas la zone en cours
    if ( pZoneIn == pZoneXY )
        continue ;

    // si zone st yan le week end
    if ( pZoneXY->GetTypeZone() == CZoneAer::ZoneStYan && m_IsWeekEnd )
        continue ;

    // dans le rayon de 1.5 km
    float DistBaryMetres = sqrtf( powf(pZoneXY->m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(pZoneXY->m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayonProche = DistBaryMetres < ( pZoneXY->m_RayonMetre + DISTANCE_PROCHE_XY ) ;

    // si pas dans le rayon proche
    if ( !DansLeRayonProche )
        continue ;

    // prise en compte de l'altitude
    bool IsNearFrontAlti = false ;
    if ( pZoneXY->IsProtect() )
        // zone proche protege
        IsNearFrontAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro <= (PlafondZoneProtegee+g_GlobalVar.m_Config.m_AltiMargin) ;
    else
        {
        // zone proche generique
        IsNearFrontAlti = g_GlobalVar.m_TerrainPosCur.m_AltiBaro >= (pZoneXY->GetAltiBasse()-g_GlobalVar.m_Config.m_AltiMargin) ;
        // si saint sandoux proche CTR Corent alors prise en compte hauteur sol
        if ( pZoneXY->GetTypeZone() == CZoneAer::ZoneCorent )
            // altitude majoree marge alti > altitude sol plus 300m
            IsNearFrontAlti &= (g_GlobalVar.m_TerrainPosCur.m_AltiBaro+g_GlobalVar.m_Config.m_AltiMargin) > (g_GlobalVar.m_AltitudeSolHgt + pZoneXY->GetAltiSolZone())  ;
        }

    // si a l'altitude de croisement
    if ( IsNearFrontAlti )
        {
        VecZoneProchesXY.push_back( pZoneXY ) ;
        pZoneXY->UnCompressZone() ;
        // distance et point frontiere proche
        pZoneXY->m_DistanceFrontiere = CDistFront::IsNearFront( pZoneXY->m_PolyStLaLoArr , pZoneXY->m_NbStLaLoPts , PtsEnCours , pZoneXY->m_CapFrontProche ) ;
        pZoneXY->FreeFloat() ;
        }
    }

// tri distance minimale des zones
CZoneAer::TrierPar( CZoneAer::TriParDistance ) ;
std::sort(VecZoneProchesXY.begin(), VecZoneProchesXY.end(),MySortFunction);

// si zone la plus proche
m_DistXYNextZone = 9999 ;
m_CapFrontProche = -1 ;
if ( VecZoneProchesXY.size() )
    {
    const CZoneAer * pZoneProche = VecZoneProchesXY[0] ;

    // distance et point prochaine zone
    m_DistXYNextZone = pZoneProche->m_DistanceFrontiere ;
    m_CapFrontProche = pZoneProche->m_CapFrontProche ;

    // si marge distance atteinte
    if ( pZoneProche->m_DistanceFrontiere <= g_GlobalVar.m_Config.m_XYMargin )
        {
        RetNbrLimite = ZONE_LIMITE_FRONTIERE ;
        // construction nom + altitude
        if ( pZoneProche->IsProtect() )
            m_Plafond4Valid = HauteurSolZoneProtegee ;
        else
            m_Plafond4Valid = pZoneProche->GetAltiBasse() ;
        sprintf( TmpChar , "Bo %s al:%4dm" , pZoneProche->m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        }
    }

/*
// pour toutes les zones recherche frontiere XY
for ( int iz = 0 ; iz < m_NbZones && RetNbrLimite != ZONE_LIMITE_ALTI && RetNbrIn != ZONE_DEDANS && g_GlobalVar.m_Config.m_XYMargin > 0 ; iz++ )
    {
    CZoneAer & Zone = *m_ZonesArr[iz] ;

    // si la zone n'est pas active
    if ( !Zone.m_Activee )
        continue ;

    // dans le du rayon + marge
    float DistBaryMetres = sqrtf( powf(Zone.m_Barycentre.m_Lat-PtsEnCours.m_Lat,2) + powf(Zone.m_Barycentre.m_Lon-PtsEnCours.m_Lon,2) )  * 60. * UnMileEnMetres ;
    bool DansLeRayonMarge = DistBaryMetres < (Zone.m_RayonMetre+g_GlobalVar.m_Config.m_XYMargin) ;

    // si pas dans le rayon + marge
    if ( !DansLeRayonMarge )
        continue ;

    // decompression de la zone
    Zone.UnCompressZone() ;

    // dedans la surface
    bool IsInArea = CPolygone::IsIn( Zone.m_PolyStLaLoArr , Zone.m_NbStLaLoPts , PtsEnCours ) ;
    // proche de la frontiere de zone
    bool IsNearFront = !IsInArea && CDistFront::IsNearFront( Zone.m_PolyStLaLoArr , Zone.m_NbStLaLoPts , PtsEnCours ) ;
    // determionation zone protege
    bool ZoneProtege = IsNearFront && Zone.IsProtect() ;

    // liberation des float
    Zone.FreeFloat() ;

    // prise en compte de l'altitude
    bool IsNearFrontAlti ;
    if ( ZoneProtege )
        IsNearFrontAlti = IsNearFront && (g_GlobalVar.m_TerrainPosCur.m_AltiBaro <= (PlafondZoneProtegee+g_GlobalVar.m_Config.m_AltiMargin)) ;
    else
        IsNearFrontAlti = IsNearFront && (g_GlobalVar.m_TerrainPosCur.m_AltiBaro >= (Zone.GetAltiBasse()-g_GlobalVar.m_Config.m_AltiMargin)) ;

    // positionnement retour
    if ( IsNearFrontAlti )
        {
        RetNbrLimite = ZONE_LIMITE_FRONTIERE ;
        // construction nom
        if ( ZoneProtege )
            m_Plafond4Valid = HauteurSolZoneProtegee ;
        else
            m_Plafond4Valid = Zone.GetAltiBasse() ;
        sprintf( TmpChar , "Bo %s al:%4dm" , Zone.m_pNomAff , m_Plafond4Valid ) ;
        RetStrLimite = TmpChar ;
        break ;
        }
    }
*/

// variables de sortie fontiere
m_Mutex.PrendreMutex() ;
 m_LimiteZone      = RetNbrLimite ;
 m_NomZoneEnLimite = RetStrLimite ;
m_Mutex.RelacherMutex() ;
}
