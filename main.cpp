/// @file
/// @brief
///
/// @copyright Copyright (c) InfoTeCS. All Rights Reserved.

#include <iostream>
#include <vector>
#include <sstream>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

#include <odb/pgsql/database.hxx>
#include <odb/query.hxx>
#include <odb/traits.hxx>

#include "dbaccess/company.odb.hpp"
#include "dbaccess/person.odb.hpp"
#include "dbaccess/workflow.odb.hpp"
#include "dbaccess/transaction.odb.hpp"
#include "dbaccess/transaction_recipient.odb.hpp"
#include "dbaccess/file.h"
#include "dbaccess/transaction_file.h"
#include "dbaccess/transaction_file_signature.h"


class ServerContext
{
public:
     explicit ServerContext( odb::transaction& tr )
          : tr_( tr )
     {}

     template< typename OdbT >
     void insert( const boost::shared_ptr< OdbT >& obj )
     {
          tr_.database().persist( *obj );
     }

     template< typename OdbT >
     void update( const boost::shared_ptr< OdbT >& obj )
     {
          tr_.database().update( *obj );
     }

     template< typename OdbT, typename IdT >
     boost::shared_ptr< OdbT > loadById( const IdT& id )
     {
          try
          {
               return tr_.database().load< OdbT >( id );
          }
          catch( const odb::object_not_persistent& )
          {}

          return nullptr;
     }

     template< typename OdbT >
     boost::shared_ptr< OdbT > loadActual( const boost::shared_ptr< OdbT >& obj )
     {
          return loadById< OdbT >( odb::object_traits< OdbT >::id( *obj ) );
     }

     odb::transaction& transaction()
     {
          return tr_;
     }

private:
     odb::transaction& tr_;
};


template<>
dba::TransactionRecipientPtr ServerContext::loadActual< dba::TransactionRecipient >( const dba::TransactionRecipientPtr& recipient )
{
     using Query = odb::query< dba::TransactionRecipient >;
     const Query query =
          Query::transaction_id == recipient->transaction_id
          && Query::recipient_id == recipient->recipient_id;

     auto res = tr_.database().query( query );

     return res.empty() ? nullptr : res.begin().load();
}


template<>
void ServerContext::update< dba::TransactionRecipient >( const dba::TransactionRecipientPtr& )
{}


class WorkflowProcessingException : public std::runtime_error
{
public:
     explicit WorkflowProcessingException( const std::string& what )
          : std::runtime_error( what )
     {}
};


template< typename OdbT >
void processWorkflowEntity( ServerContext& ctx, const boost::shared_ptr< OdbT >& changed )
{
     const auto actual = ctx.loadActual( changed );

     if( !actual )
     {
          ctx.insert( changed );
     }
     else
     {
          ctx.update( changed );
     }
}


void upSyncWorkflow(
     ServerContext& ctx,
     const dba::WorkflowPtr& w,
     const dba::TransactionPtr& t,
     const dba::TransactionRecipientPtr& tr
)
{
     std::cout
          << "Processing TransactionRecipient for recipient "
          << tr->recipient_id << " -> "
          << tr->transaction_id << '\n';

     processWorkflowEntity( ctx, tr );
}


void upSyncWorkflow(
     ServerContext& ctx,
     const dba::WorkflowPtr& w,
     const dba::TransactionPtr& t,
     const dba::TransactionRecipientPtrVector& recipients
)
{
     std::cout
          << "Processing Transaction "
          << t->transaction_id << " -> "
          << t->workflow_id << '\n';

     processWorkflowEntity( ctx, t );

     for( const auto& tr: recipients )
          if( t->transaction_id == tr->transaction_id )
               upSyncWorkflow( ctx, w, t, tr );
}


void upSyncWorkflow(
     ServerContext& ctx,
     const dba::WorkflowPtr& w,
     const dba::TransactionPtrVector& transactions,
     const dba::TransactionRecipientPtrVector& recipients
)
{
     std::cout
          << "Processing workflow "
          << w->workflow_id << '\n';

     const auto initTransactionId = w->initial_transaction_id;
     w->initial_transaction_id.reset();

     processWorkflowEntity( ctx, w );

     for( const auto& t: transactions )
          if( w->workflow_id == t->workflow_id )
               upSyncWorkflow( ctx, w, t, recipients );

     if( initTransactionId )
     {
          w->initial_transaction_id = initTransactionId;
          ctx.update( w );
     }
}


dba::TransactionPtr transactionWithoutWorkflow( const dba::TransactionPtr& transaction, const dba::WorkflowPtrVector& workflows )
{
     const auto foundIter =
          std::find_if( workflows.begin(), workflows.end(),
               [ &transaction ]( const dba::WorkflowPtr& workflow )
               {
                    return transaction->workflow_id == workflow->workflow_id;
               }
          );

     return foundIter == workflows.end() ? transaction : nullptr;
}


void upSyncWorkflow(
     ServerContext& ctx,
     const dba::WorkflowPtrVector& workflows,
     const dba::TransactionPtrVector& transactions,
     const dba::TransactionRecipientPtrVector& recipients
)
{
     std::cout
          << "Processing workflow tree\n";

     for( const auto& w: workflows )
          upSyncWorkflow( ctx, w, transactions, recipients );

     std::cout
          << "Processing remain transactions\n";

     for( const auto& t: transactions )
     {
          if( const auto& transaction = transactionWithoutWorkflow( t, workflows ) )
          {
               if( const auto workflow = ctx.loadById< dba::Workflow >( transaction->workflow_id ) )
               {
                    upSyncWorkflow( ctx, workflow, transaction, recipients );
               }
               else
                    BOOST_THROW_EXCEPTION(
                         WorkflowProcessingException(
                              "no workflow found by id "
                              + boost::uuids::to_string( t->workflow_id )
                              + " referenced from transaction "
                              + boost::uuids::to_string( t->transaction_id ) ) );
          }
     }
}


dba::PersonPtr loadRandomPerson( ServerContext& ctx )
{
     auto res =
          ctx.transaction().database().query< dba::Person >();

     if( res.empty() )
          BOOST_THROW_EXCEPTION( std::runtime_error( "no persons found" ) );

     auto n = rand() % res.size();

     for( odb::result< dba::Person >::iterator it = res.begin(); it != res.end(); ++it )
          if( n-- == 0 )
               return it.load();

     return nullptr;
}


int main( int ac, char** av )
{
     try
     {
          srand( time( 0 ) );

          odb::pgsql::database db( "alexen", "alexen", "test" );
          odb::transaction tr( db.begin() );

          ServerContext ctx( tr );

          const auto sender = loadRandomPerson( ctx );
          const auto recipient = loadRandomPerson( ctx );

          const dba::WorkflowPtrVector workflows{
               dba::Workflow::create( "Newsletter" )
          };

          const dba::TransactionPtrVector transactions{
               dba::Transaction::create( workflows[ 0 ]->workflow_id, sender->person_id, "Document" ),
               dba::Transaction::create( boost::uuids::random_generator()(), sender->person_id, "Response" )
          };

          workflows[ 0 ]->initial_transaction_id = transactions[ 0 ]->transaction_id;

          const dba::TransactionRecipientPtrVector recipients{
               dba::TransactionRecipient::create( transactions[ 0 ]->transaction_id, recipient->person_id ),
               dba::TransactionRecipient::create( transactions[ 1 ]->transaction_id, recipient->person_id )
          };

          upSyncWorkflow( ctx, workflows, transactions, recipients );

          tr.commit();
     }
     catch( const odb::exception& e )
     {
          std::cerr << "odb exception: " << boost::diagnostic_information( e ) << '\n';
          return 1;
     }
     catch( const std::exception& e )
     {
          std::cerr << "std exception: " << boost::diagnostic_information( e ) << '\n';
          return 2;
     }

     return 0;
}
