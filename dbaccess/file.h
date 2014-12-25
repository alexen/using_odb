/*
 * file.h
 *
 *  Created on: 14 авг. 2014 г.
 *      Author: alexen
 */

#ifndef DBA_FILE_H_
#define DBA_FILE_H_


#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace dba {

using FilePtr = boost::shared_ptr< struct File >;

struct File
{
     static FilePtr create(
          const std::string& name,
          const size_t size
     )
     {
          auto file = boost::make_shared< File >();

          file->file_id = boost::uuids::random_generator()();
          file->name = name;
          file->size = size;

          return file;
     }

     boost::uuids::uuid file_id;
     std::string name;
     size_t size = 0;
     long long version = 0;
};

#pragma db object( File ) table( "file" )
#pragma db member( File::file_id ) id

}  // namespace dba

#endif /* DBA_FILE_H_ */
