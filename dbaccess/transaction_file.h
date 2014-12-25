/*
 * transaction_file.h
 *
 *  Created on: 14 авг. 2014 г.
 *      Author: alexen
 */

#ifndef DBA_TRANSACTION_FILE_H_
#define DBA_TRANSACTION_FILE_H_

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

namespace dba {

using TransactionFilePtr = boost::shared_ptr< struct TransactionFile >;

struct TransactionFile
{
     static TransactionFilePtr create(
          const boost::uuids::uuid& fileId
     )
     {
          auto tf = boost::make_shared< TransactionFile >();

          tf->transaction_file_id = boost::uuids::random_generator()();
          tf->file_id = fileId;

          return tf;
     }

     boost::uuids::uuid transaction_file_id;
     boost::uuids::uuid file_id;
     long long version = 0;
};

#pragma db object( TransactionFile ) table( "transaction_file" )
#pragma db member( TransactionFile::transaction_file_id ) id

}  // namespace dba


#endif /* DBA_TRANSACTION_FILE_H_ */
