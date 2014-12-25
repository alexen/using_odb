/*
 * person.h
 *
 *  Created on: 14 авг. 2014 г.
 *      Author: alexen
 */

#ifndef DBA_PERSON_H_
#define DBA_PERSON_H_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace dba {

using PersonPtr = boost::shared_ptr< struct Person >;

struct Person
{
     static PersonPtr create(
          const boost::uuids::uuid& companyId,
          const std::string& lastName,
          const std::string& firstName,
          const std::string& middleName = ""
     )
     {
          auto person = boost::make_shared< Person >();

          person->person_id = boost::uuids::random_generator()();
          person->last_name = lastName;
          person->first_name = firstName;
          person->middle_name = boost::make_optional( !middleName.empty(), middleName );
          person->company_id = companyId;

          return person;
     }

     boost::uuids::uuid person_id;
     std::string last_name;
     std::string first_name;
     boost::optional< std::string > middle_name;
     boost::uuids::uuid company_id;
};

#pragma db object( Person ) table( "person" )
#pragma db member( Person::person_id ) id

}  // namespace dba



#endif /* DBA_PERSON_H_ */
