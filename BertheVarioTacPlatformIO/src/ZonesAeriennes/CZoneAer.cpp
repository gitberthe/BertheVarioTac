////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 25/11/2024
///

#include "../BertheVarioTac.h"

bool CZoneAer::ms_TriParNom = false ;

int     CZoneAer::ms_MaxNombrePtsZone = -1 ;        // nombre de pts maximum d'une zone
char*   CZoneAer::ms_compressed_data_lz4 = NULL ;   // buffer de compression lz4
int     CZoneAer::ms_max_dst_size ;                 // taille du buffer lz4

////////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur, liberation des points
CZoneAer::~CZoneAer()
{
if ( m_PolyStLaLoArr != NULL )
    {
    for ( int ip = 0 ; ip < m_NbStLaLoPts ; ip++ )
        delete m_PolyStLaLoArr[ip] ;
    delete [] m_PolyStLaLoArr ;
    m_PolyStLaLoArr = NULL ;
    m_NbStLaLoPts = 0 ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou altitude
bool CZoneAer::operator > ( const CZoneAer & Zone ) const
{
if ( ms_TriParNom )
    return (strcmp( m_NomAff.c_str() , Zone.m_NomAff.c_str() ) > 0 ) ;

//return m_Area > Zone.m_Area ;
return GetAltiBasse() > Zone.GetAltiBasse() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou surface
bool CZoneAer::operator < ( const CZoneAer & Zone ) const
{
if ( ms_TriParNom )
    return ( strcmp( m_NomAff.c_str() , Zone.m_NomAff.c_str() ) < 0 ) ;

//return m_Area < Zone.m_Area ;
return GetAltiBasse() < Zone.GetAltiBasse() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Altitude en cours fonction de la date
int CZoneAer::GetAltiBasse() const
{
if ( ! HavePeriod() )
    return m_AltiBasse ;
else if ( m_pDerogFfvl->m_AltiAPrendreEnCompte == ALTI_BASSE )
    return m_AltiBasse ;
else if ( m_pDerogFfvl->m_AltiAPrendreEnCompte == ALTI_PERIODE_SEMAINE )
    return m_pDerogFfvl->m_AltiBassePeriodeSemaine ;
else if ( m_pDerogFfvl->m_AltiAPrendreEnCompte == ALTI_PERIODE_WEEKEND )
    return m_pDerogFfvl->m_AltiBassePeriodeWeekEnd ;
return -1 ;
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Compresse la zone en tableau de int de resolution 18m et maximum +-589km.
/// Puis compression des short en char lz4.
void CZoneAer::CompressZone()
{
// si pas de points a compresser
if ( m_PolyStLaLoArr == NULL )
    return ;

// si deja compresse en short/lz4
if ( m_CharLz4Arr != NULL )
    return ;

// allocation du tableau de short
short short_size = m_NbStLaLoPts*2 ;
short * LowResShortArr = new short [short_size] ;

// verification que la zone n'est pas trop grande
if ( m_RayonMetre >= 589.*1000. )
    {
    Serial.println( "zone aerienne trop grande") ;
    g_GlobalVar.Reboot() ;
    }

// passage en short
int ipshort = 0 ;
for ( int ipstruct = 0 ; ipstruct < m_NbStLaLoPts ; ipstruct++ )
    {
    const CVecZoneReduce::st_coord_poly * pStPts = m_PolyStLaLoArr[ipstruct] ;

    LowResShortArr[ipshort++] = (short)((float)((pStPts->m_Lat - m_Barycentre.m_Lat) * MilesParDegres * UnMileEnMetres / ResolCompress)) ;
    LowResShortArr[ipshort++] = (short)((float)((pStPts->m_Lon - m_Barycentre.m_Lon) * MilesParDegres * UnMileEnMetres / ResolCompress)) ;
    }

// compression lz4
m_Lz4BuffSize = LZ4_compress_default( (const char*) LowResShortArr , ms_compressed_data_lz4, short_size*sizeof(short) , ms_max_dst_size );
// allocation buffer lz4 local
m_CharLz4Arr = new char [ m_Lz4BuffSize ] ;
// recopie locale
memcpy( m_CharLz4Arr , ms_compressed_data_lz4 , m_Lz4BuffSize ) ;

// supression des float
FreeFloat() ;

// destruction buffer temporaire
delete [] LowResShortArr ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief  Supression du tableau des float et des short
/// ne reste que le lz4
void CZoneAer::FreeFloat()
{
// destruction buffer structure float lat/lon
if ( m_PolyStLaLoArr != NULL )
    {
    for ( int ip = 0 ; ip < m_NbStLaLoPts ; ip++ )
        delete m_PolyStLaLoArr[ip] ;
    delete [] m_PolyStLaLoArr ;
    m_PolyStLaLoArr = NULL ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief De-compresse une zone en float
void CZoneAer::UnCompressZone()
{
// si deja decompressee en float/short
FreeFloat() ;

// allocation buffer short
short short_size = m_NbStLaLoPts*2 ;
short * LowResShortArr = new short [short_size] ;

// decompression lz4
LZ4_decompress_safe( m_CharLz4Arr , (char*) LowResShortArr , m_Lz4BuffSize , short_size*sizeof(short) ) ;

// allocation du tableau st *
m_PolyStLaLoArr = new CVecZoneReduce::st_coord_poly * [m_NbStLaLoPts] ;

// passage en float
int ipshort = 0 ;
for ( int ipstruct = 0 ; ipstruct < m_NbStLaLoPts ; ipstruct++ )
    {
    // ajout du points
    CVecZoneReduce::st_coord_poly * pStPts = new CVecZoneReduce::st_coord_poly ;
    m_PolyStLaLoArr[ipstruct] = pStPts ;

    pStPts->m_Lat = ((float)LowResShortArr[ipshort++]) / (MilesParDegres * UnMileEnMetres / ResolCompress) + m_Barycentre.m_Lat ;
    pStPts->m_Lon = ((float)LowResShortArr[ipshort++]) / (MilesParDegres * UnMileEnMetres / ResolCompress) + m_Barycentre.m_Lon ;
    }

// destruction buffer short
delete [] LowResShortArr ;
}
