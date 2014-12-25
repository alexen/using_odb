/*
 * transaction.h
 *
 *  Created on: 14 авг. 2014 г.
 *      Author: alexen
 */

#ifndef DBA_TRANSACTION_H_
#define DBA_TRANSACTION_H_

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

#include <odb/core.hxx>

namespace dba {

using TransactionPtr = boost::shared_ptr< struct Transaction >;
using TransactionPtrVector = std::vector< TransactionPtr >;

struct Transaction
{
     static TransactionPtr create(
          const boost::uuids::uuid& workflowId,
          const boost::uuids::uuid& senderId,
          const std::string& name
     )
     {
          auto tr = boost::make_shared< Transaction >();

          tr->transaction_id = boost::uuids::random_generator()();
          tr->name = name;
          tr->workflow_id = workflowId;
          tr->sender_id = senderId;

          return tr;
     }

     boost::uuids::uuid transaction_id;
     std::string name;
     boost::uuids::uuid workflow_id;
     long long version = 0;
     boost::uuids::uuid sender_id;
};

#pragma db object( Transaction ) table( "transaction" )
#pragma db member( Transaction::transaction_id ) id

}  // namespace dba



#endif /* DBA_TRANSACTION_H_ */
