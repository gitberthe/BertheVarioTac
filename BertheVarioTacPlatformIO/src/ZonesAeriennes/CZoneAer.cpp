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

////////////////////////////////////////////////////////////////////////////////
/// \brief Destructeur, liberation des points
CZoneAer::~CZoneAer()
{
if ( m_PolygoneArr != NULL )
    {
    for ( int ip = 0 ; ip < m_NbPts ; ip++ )
        delete m_PolygoneArr[ip] ;
    delete [] m_PolygoneArr ;
    m_PolygoneArr = NULL ;
    m_NbPts = 0 ;
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
