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
class Metadata;
class MetadataStream;

class VMF_EXPORT IOperation
{
public:
    IOperation();
    virtual ~IOperation();

public:
    enum OperationId { UserId=0, MinId, MaxId, AverageId, MedianId, CountId, SumId, LastValueId };
    enum OperationFlags { Add=0x01, Remove=0x02, Change=0x04, All=(Add|Remove|Change) };

public:
    static std::shared_ptr<IOperation> create( int id, const std::string& name = "" );

public:
    virtual int getId() const = 0;
    virtual bool canInc( unsigned flags ) const = 0;
    virtual Variant getValue() const = 0;
    virtual void initValue() = 0;
    virtual void addValue( const Variant& in ) = 0;
    virtual void removeValue( const Variant& in ) = 0;
    virtual void changeValue( const Variant& in ) = 0;
};

class VMF_EXPORT MinOp: public IOperation
{
public:
    MinOp();
    virtual ~MinOp();

public:
    virtual int getId() const;
    virtual bool canInc( unsigned flags ) const;
    virtual Variant getValue() const;
    virtual void initValue();
    virtual void addValue( const Variant& in );
    virtual void removeValue( const Variant& in );
    virtual void changeValue( const Variant& in );

private:
    Variant value;
};

class VMF_EXPORT MaxOp: public IOperation
{
public:
    MaxOp();
    virtual ~MaxOp();

public:
    virtual int getId() const;
    virtual bool canInc( unsigned flags ) const;
    virtual Variant getValue() const;
    virtual void initValue();
    virtual void addValue( const Variant& in );
    virtual void removeValue( const Variant& in );
    virtual void changeValue( const Variant& in );

private:
    Variant value;
};

class VMF_EXPORT AverageOp: public IOperation
{
public:
    AverageOp();
    virtual ~AverageOp();

public:
    virtual int getId() const;
    virtual bool canInc( unsigned flags ) const;
    virtual Variant getValue() const;
    virtual void initValue();
    virtual void addValue( const Variant& in );
    virtual void removeValue( const Variant& in );
    virtual void changeValue( const Variant& in );

private:
    Variant value;
};

class VMF_EXPORT MedianOp: public IOperation
{
public:
    MedianOp();
    virtual ~MedianOp();

public:
    virtual int getId() const;
    virtual bool canInc( unsigned flags ) const;
    virtual Variant getValue() const;
    virtual void initValue();
    virtual void addValue( const Variant& in );
    virtual void removeValue( const Variant& in );
    virtual void changeValue( const Variant& in );

private:
    Variant value;
};

class VMF_EXPORT CountOp: public IOperation
{
public:
    CountOp();
    virtual ~CountOp();

public:
    virtual int getId() const;
    virtual bool canInc( unsigned flags ) const;
    virtual Variant getValue() const;
    virtual void initValue();
    virtual void addValue( const Variant& in );
    virtual void removeValue( const Variant& in );
    virtual void changeValue( const Variant& in );

private:
    Variant value;
};

class VMF_EXPORT SumOp: public IOperation
{
public:
    SumOp();
    virtual ~SumOp();

public:
    virtual int getId() const;
    virtual bool canInc( unsigned flags ) const;
    virtual Variant getValue() const;
    virtual void initValue();
    virtual void addValue( const Variant& in );
    virtual void removeValue( const Variant& in );
    virtual void changeValue( const Variant& in );

private:
    Variant value;
};

class VMF_EXPORT LastValueOp: public IOperation
{
public:
    LastValueOp();
    virtual ~LastValueOp();

public:
    virtual int getId() const;
    virtual bool canInc( unsigned flags ) const;
    virtual Variant getValue() const;
    virtual void initValue();
    virtual void addValue( const Variant& in );
    virtual void removeValue( const Variant& in );
    virtual void changeValue( const Variant& in );

private:
    Variant value;
};

struct VMF_EXPORT StatisticsItem
{
    StatisticsItem( const std::string& n, const std::string& m, const std::string& f, std::shared_ptr<IOperation> op )
        : name( n ), metadata( m ), field( f ), operation( op ), dirty( false ) {};
    ~StatisticsItem() {};

    std::string name;
    std::string metadata;
    std::string field;
    std::shared_ptr<IOperation> operation;
    bool dirty;
};

class VMF_EXPORT Statistics
{
    friend class MetadataSchema;
    friend class MetadataStream;

public:
    enum UpdateMode { Time, Auto, Manual };

public:
    Statistics();
    virtual ~Statistics();

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

    bool isDirty() const;
    void rescan();
    void reset();

    void addStatisticsItem( const StatisticsItem& item );
    std::vector<StatisticsItem> getStatisticsItems() const;

protected:
    void setSchemaName( std::string schema );
    void setMetadataStream( const MetadataStream* pStream );

private:
    void validate();

    std::vector<StatisticsItem> m_items;
    const MetadataStream* m_pStream;
    std::string m_schema;
    bool m_dirty;
};

} // namespace vmf

#endif /* __VMF_STATISTICS_H__ */
