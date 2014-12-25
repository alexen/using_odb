/*
 * transaction_file_signature.h
 *
 *  Created on: 15 авг. 2014 г.
 *      Author: alexen
 */

#ifndef DBA_TRANSACTION_FILE_SIGNATURE_H_
#define DBA_TRANSACTION_FILE_SIGNATURE_H_

#include <iosfwd>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

namespace dba {

using TransactionFileSignaturePtr = boost::shared_ptr< struct TransactionFileSignature >;

struct TransactionFileSignature
{
     static TransactionFileSignaturePtr create(
          const boost::uuids::uuid& transactionFileId,
          const std::string& signature
     )
     {
          auto tfs = boost::make_shared< TransactionFileSignature >();

          tfs->transaction_file_signature_id = boost::uuids::random_generator()();
          tfs->transaction_file_id = transactionFileId;
          tfs->signature = signature;

          return tfs;
     }

     boost::uuids::uuid transaction_file_signature_id;
     boost::uuids::uuid transaction_file_id;
     std::string signature;
     long long version = 0;
};

#pragma db object( TransactionFileSignature ) table( "transaction_file_signature" )
#pragma db member( TransactionFileSignature::transaction_file_signature_id ) id

}  // namespace dba


#endif /* DBA_TRANSACTION_FILE_SIGNATURE_H_ */
