/*
 * workflow.h
 *
 *  Created on: 14 авг. 2014 г.
 *      Author: alexen
 */

#ifndef DBA_WORKFLOW_H_
#define DBA_WORKFLOW_H_

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/optional.hpp>

#include <odb/core.hxx>

namespace dba {

using WorkflowPtr = boost::shared_ptr< struct Workflow >;
using WorkflowPtrVector = std::vector< WorkflowPtr >;

struct Workflow
{
     static WorkflowPtr create(
          const std::string& name,
          const boost::optional< boost::uuids::uuid >& initialTransactionId = boost::none
     )
     {
          auto workflow = boost::make_shared< Workflow >();

          workflow->workflow_id = boost::uuids::random_generator()();
          workflow->name = name;
          workflow->initial_transaction_id = initialTransactionId;

          return workflow;
     }

     boost::uuids::uuid workflow_id;
     std::string name;
     long long version = 0;
     boost::optional< boost::uuids::uuid > initial_transaction_id;
};

#pragma db object( Workflow ) table( "workflow" )
#pragma db member( Workflow::workflow_id ) id

}  // namespace dba


#endif /* DBA_WORKFLOW_H_ */
