/* 
 * Copyright 2015 Intel(r) Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http ://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __VMF_STATISTICS_H__
#define __VMF_STATISTICS_H__

#include "variant.hpp"
#include "global.hpp"
#include <memory>
#include <string>

namespace vmf
{
namespace stats
{

enum OperationId { UserId=0, MinId, MaxId, AverageId, MedianId, CountId, SumId, LastValueId };
enum UpdateMode { Time, Auto, Manual };

class VMF_EXPORT IOperation
{
protected:
    IOperation() {};
    virtual ~IOperation() {};
public:
    virtual int getId() const = 0;
//    virtual void apply() = 0; // TODO temp stub, will be revealed later
    static std::shared_ptr<IOperation> create( int id, const std::string& name = "" );
};

class VMF_EXPORT MinOp: public IOperation
{
public:
    MinOp(): IOperation() {};
    virtual ~MinOp() {};
public:
    virtual int getId() const { return MinId; };
};

class VMF_EXPORT MaxOp: public IOperation
{
public:
    MaxOp(): IOperation() {};
    virtual ~MaxOp() {};
public:
    virtual int getId() const { return MaxId; };
};

class VMF_EXPORT AverageOp: public IOperation
{
public:
    AverageOp(): IOperation() {};
    virtual ~AverageOp() {};
public:
    virtual int getId() const { return AverageId; };
};

class VMF_EXPORT MedianOp: public IOperation
{
public:
    MedianOp(): IOperation() {};
    virtual ~MedianOp() {};
public:
    virtual int getId() const { return MedianId; };
};

class VMF_EXPORT CountOp: public IOperation
{
public:
    CountOp(): IOperation() {};
    virtual ~CountOp() {};
public:
    virtual int getId() const { return CountId; };
};

class VMF_EXPORT SumOp: public IOperation
{
public:
    SumOp(): IOperation() {};
    virtual ~SumOp() {};
public:
    virtual int getId() const { return SumId; };
};

class VMF_EXPORT LastValueOp: public IOperation
{
public:
    LastValueOp(): IOperation() {};
    virtual ~LastValueOp() {};
public:
    virtual int getId() const { return LastValueId; };
};

// TODO perhaps it could be possible to set up and use Metadata object instead of StatsSet
typedef std::pair<std::string, vmf::Variant> Value; // name,value
typedef std::vector<Value> StatsSet;

class VMF_EXPORT Config
{
public:
    Config() {}
    virtual ~Config() {}
    Config( const Config& other ) { *this = other; }
    void addStats( const std::string& statsName, const std::string& metadataName,
		   const std::string& fieldName, std::shared_ptr<IOperation>& operation );
    void setUpdateMode( UpdateMode mode );
    void setUpdateTime( unsigned ms ); // which units?
private:
};

// TODO perhaps it would be better to use functor instead of (*) function, but it leads to use templated methods
typedef void (*UpdateHandler)( const std::string& schemaName, StatsSet& stats );

// TODO perhaps it should be moved to corresponding classes (e.g. MetadataSchema)
void getStats( const std::string& schemaName, StatsSet& stats ); // for manual get stats
UpdateHandler registerHandler( UpdateHandler newHandler ); // for auto/time get stats

} // namespace vmf::stats
} // namespace vmf

#endif /* __VMF_STATISTICS_H__ */
