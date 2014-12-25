/*
 * transaction_recipient.h
 *
 *  Created on: 14 авг. 2014 г.
 *      Author: alexen
 */

#ifndef DBA_TRANSACTION_RECIPIENT_H_
#define DBA_TRANSACTION_RECIPIENT_H_

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>

namespace dba {

using TransactionRecipientPtr = boost::shared_ptr< struct TransactionRecipient >;
using TransactionRecipientPtrVector = std::vector< TransactionRecipientPtr >;

struct TransactionRecipient
{
     static TransactionRecipientPtr create(
          const boost::uuids::uuid& transactionId,
          const boost::uuids::uuid& recipientId
     )
     {
          auto trRec = boost::make_shared< TransactionRecipient >();

          trRec->transaction_id = transactionId;
          trRec->recipient_id = recipientId;

          return trRec;
     }

     boost::uuids::uuid transaction_id;
     boost::uuids::uuid recipient_id;
};

#pragma db object( TransactionRecipient ) table( "transaction_recipient" ) no_id

}  // namespace dba



#endif /* DBA_TRANSACTION_RECIPIENT_H_ */
