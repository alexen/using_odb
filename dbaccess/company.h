/*
 * company.h
 *
 *  Created on: 13 авг. 2014 г.
 *      Author: alexen
 */

#ifndef DBA_COMPANY_H_
#define DBA_COMPANY_H_

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace dba
{

using CompanyPtr = boost::shared_ptr< struct Company >;

struct Company
{
     static CompanyPtr create(
          const std::string& name,
          const std::string& inn,
          const std::string& kpp = ""
     )
     {
          auto company = boost::make_shared< Company >();

          company->company_id = boost::uuids::random_generator()();
          company->name = name;
          company->inn = inn;
          company->kpp = boost::make_optional( !kpp.empty(), kpp );
          company->entrepreneur = kpp.empty();

          return company;
     }

     boost::uuids::uuid company_id;
     std::string name;
     std::string inn;
     boost::optional< std::string > kpp;
     bool entrepreneur = false;
};

#pragma db object( Company ) table( "company" )
#pragma db member( Company::company_id ) id

}


#endif /* DBA_COMPANY_H_ */
