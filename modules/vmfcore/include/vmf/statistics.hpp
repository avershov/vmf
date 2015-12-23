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
#include "metadata.hpp"
#include <memory>
#include <string>

namespace vmf
{

class VMF_EXPORT IOperation
{
protected:
    IOperation() {};
    virtual ~IOperation() {};
public:
    enum OperationId { UserId=0, MinId, MaxId, AverageId, MedianId, CountId, SumId, LastValueId };
    enum OperationFlags { Add=0x01, Remove=0x02, Change=0x04, All=(Add|Remove|Change) };
public:
    static std::shared_ptr<IOperation> create( int id, const std::string& name = "" );
public:
    virtual int getId() const = 0;
    virtual bool canFast( unsigned flags ) const = 0;
    virtual Variant getValue() const = 0;
};

class VMF_EXPORT MinOp: public IOperation
{
public:
    MinOp(): IOperation() {};
    virtual ~MinOp() {};
public:
    virtual int getId() const { return MinId; };
    virtual bool canFast( unsigned flags ) const { return bool((flags & Add) != 0); };
    virtual Variant getValue() const { return value; };
private:
    Variant value;
};

class VMF_EXPORT MaxOp: public IOperation
{
public:
    MaxOp(): IOperation() {};
    virtual ~MaxOp() {};
public:
    virtual int getId() const { return MaxId; };
    virtual bool canFast( unsigned flags ) const { return bool((flags & Add) != 0); };
    virtual Variant getValue() const { return value; };
private:
    Variant value;
};

class VMF_EXPORT AverageOp: public IOperation
{
public:
    AverageOp(): IOperation() {};
    virtual ~AverageOp() {};
public:
    virtual int getId() const { return AverageId; };
    virtual bool canFast( unsigned flags ) const { return bool((flags & All) != 0); };
    virtual Variant getValue() const { return value; };
private:
    Variant value;
};

class VMF_EXPORT MedianOp: public IOperation
{
public:
    MedianOp(): IOperation() {};
    virtual ~MedianOp() {};
public:
    virtual int getId() const { return MedianId; };
    virtual bool canFast( unsigned flags ) const { return false; };
    virtual Variant getValue() const { return value; };
private:
    Variant value;
};

class VMF_EXPORT CountOp: public IOperation
{
public:
    CountOp(): IOperation() {};
    virtual ~CountOp() {};
public:
    virtual int getId() const { return CountId; };
    virtual bool canFast( unsigned flags ) const { return bool((flags & All) != 0); };
    virtual Variant getValue() const { return value; };
private:
    Variant value;
};

class VMF_EXPORT SumOp: public IOperation
{
public:
    SumOp(): IOperation() {};
    virtual ~SumOp() {};
public:
    virtual int getId() const { return SumId; };
    virtual bool canFast( unsigned flags ) const { return bool((flags & All) != 0); };
    virtual Variant getValue() const { return value; };
private:
    Variant value;
};

class VMF_EXPORT LastValueOp: public IOperation
{
public:
    LastValueOp(): IOperation() {};
    virtual ~LastValueOp() {};
public:
    virtual int getId() const { return LastValueId; };
    virtual bool canFast( unsigned flags ) const { return bool((flags & Add) != 0); };
    virtual Variant getValue() const { return value; };
private:
    Variant value;
};

struct StatisticsItem
{
    std::string name;
    std::string metadata;
    std::string field;
    std::shared_ptr<IOperation> operation;
};

class VMF_EXPORT Statistics
{
public:
    enum UpdateMode { Time, Auto, Manual };
public:
    Statistics() {};
    virtual ~Statistics() {};
    void add( const std::string& name, const std::string& metadata, const std::string& field, std::shared_ptr<IOperation>& operation );
    void setUpdateMode( UpdateMode mode );
    void setUpdateTime( unsigned ms );

    /*!
    * \brief Functions used to notify statistics about events
    * \param spMetadata [in] pointer to metadata object
    * \param fieldName [in] name of changed field
    */
    void metadataAdded( const std::shared_ptr< Metadata >& spMetadata );
    void metadataRemoved( const std::shared_ptr< Metadata >& spMetadata );
    void metadataChanged( const std::shared_ptr< Metadata >& spMetadata, const std::string& fieldName );

private:
    std::vector<StatisticsItem> m_items;
};

} // namespace vmf

#endif /* __VMF_STATISTICS_H__ */
