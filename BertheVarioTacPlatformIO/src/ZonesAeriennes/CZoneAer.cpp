////////////////////////////////////////////////////////////////////////////////
/// \file CZoneAer.cpp
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 08/02/2025
///

#include "../BertheVarioTac.h"

CZoneAer::TriPar CZoneAer::ms_TriPar = CZoneAer::TriParAltitude ;

int     CZoneAer::ms_MaxNombrePtsZone = -1 ;        // nombre de pts maximum d'une zone
char*   CZoneAer::ms_compressed_data_lz4 = NULL ;   // buffer de compression lz4
int     CZoneAer::ms_max_dst_size ;                 // taille du buffer lz4

////////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur, liberation des points
CZoneAer::~CZoneAer()
{
// points non compressés
if ( m_PolyStLaLoArr != NULL )
    {
    delete [] m_PolyStLaLoArr ;
    m_PolyStLaLoArr = NULL ;
    m_NbStLaLoPts = 0 ;
    }
// points compresse
if ( m_CharLz4Arr != NULL )
    {
    delete [] m_CharLz4Arr ;
    m_CharLz4Arr = NULL ;
    m_Lz4BuffSize = 0 ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou altitude
bool CZoneAer::operator > ( const CZoneAer & Zone ) const
{
if ( ms_TriPar == TriParNom )
    return (strcmp( m_pNomAff , Zone.m_pNomAff ) > 0 ) ;
else if ( ms_TriPar == TriParDistance )
    return m_DistanceFrontiere > Zone.m_DistanceFrontiere ;

//return m_Area > Zone.m_Area ;
return GetAltiBasse() > Zone.GetAltiBasse() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tri par nom ou surface
bool CZoneAer::operator < ( const CZoneAer & Zone ) const
{
if ( ms_TriPar == TriParNom )
    return (strcmp( m_pNomAff , Zone.m_pNomAff ) < 0 ) ;
else if ( ms_TriPar == TriParDistance )
    return m_DistanceFrontiere < Zone.m_DistanceFrontiere ;

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
/// \brief Compresse la zone en tableau de short de resolution variable.
/// (de 1 a 7 metres, 2 metres en moyenne)
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

// short maximum en float
const float ShortMax = 32767. ;

// resolution entiere minimum
m_ResolutionMetre = ceilf( m_RayonMetre / ShortMax ) ;

// verification que la zone n'est pas trop grande
const float RayonMaxZoneEnMetre = ShortMax * m_ResolutionMetre ;
if ( m_RayonMetre >= RayonMaxZoneEnMetre )
    {
    Serial.println( "zone aerienne trop grande") ;
    g_GlobalVar.BeepError() ;
    g_GlobalVar.Reboot() ;
    }

// passage en short
int ipshort = 0 ;
for ( int ipstruct = 0 ; ipstruct < m_NbStLaLoPts ; ipstruct++ )
    {
    const CZoneAer::st_coord_poly * pStPts = & m_PolyStLaLoArr[ipstruct] ;

    LowResShortArr[ipshort++] = (short)((float)((pStPts->m_Lat - m_Barycentre.m_Lat) * MilesParDegres * UnMileEnMetres / ((float)m_ResolutionMetre))) ;
    LowResShortArr[ipshort++] = (short)((float)((pStPts->m_Lon - m_Barycentre.m_Lon) * MilesParDegres * UnMileEnMetres / ((float)m_ResolutionMetre))) ;
    }

// compression lz4
m_Lz4BuffSize = LZ4_compress_default( (const char*) LowResShortArr , ms_compressed_data_lz4, short_size*sizeof(short) , ms_max_dst_size );

// verification si compression efficace
if ( m_Lz4BuffSize >= short_size*sizeof(short) )
    {
    // taille du buffer et indicateur compression
    m_CompressLz4 = false ;
    m_Lz4BuffSize = short_size*sizeof(short) ;
    // deplacement buffer short non compresse
    m_CharLz4Arr = (char*) LowResShortArr ;
    }
else
    {
    // indicateur compression
    m_CompressLz4 = true ;
    // allocation buffer lz4 local
    m_CharLz4Arr = new char [ m_Lz4BuffSize ] ;
    // recopie compressee
    memcpy( m_CharLz4Arr , ms_compressed_data_lz4 , m_Lz4BuffSize ) ;
    // destruction buffer short
    delete [] LowResShortArr ;
    }

// supression des float
FreeFloat() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief  Supression du tableau des float et des short
/// ne reste que le lz4
void CZoneAer::FreeFloat()
{
// destruction buffer structure float lat/lon
if ( m_PolyStLaLoArr != NULL )
    {
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
if ( m_CompressLz4 )
    LZ4_decompress_safe( m_CharLz4Arr , (char*) LowResShortArr , m_Lz4BuffSize , short_size*sizeof(short) ) ;
else
    memcpy( LowResShortArr , m_CharLz4Arr , short_size*sizeof(short) ) ;

// allocation du tableau st *
m_PolyStLaLoArr = new CZoneAer::st_coord_poly [m_NbStLaLoPts] ;

// passage en float
int ipshort = 0 ;
for ( int ipstruct = 0 ; ipstruct < m_NbStLaLoPts ; ipstruct++ )
    {
    // ajout du points
    CZoneAer::st_coord_poly * pStPts = & m_PolyStLaLoArr[ipstruct] ;

    pStPts->m_Lat = ((float)LowResShortArr[ipshort++]) / (MilesParDegres * UnMileEnMetres / ((float)m_ResolutionMetre)) + m_Barycentre.m_Lat ;
    pStPts->m_Lon = ((float)LowResShortArr[ipshort++]) / (MilesParDegres * UnMileEnMetres / ((float)m_ResolutionMetre)) + m_Barycentre.m_Lon ;
    }

// destruction buffer short
delete [] LowResShortArr ;
}
