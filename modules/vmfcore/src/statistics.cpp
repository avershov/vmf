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
#include "vmf/statistics.hpp"
#include "vmf/metadata.hpp"
#include "vmf/metadatastream.hpp"
//#include <cstring>
//#include <string>
//#include <memory>
//#include <sstream>
//#include <cmath>
//#include <limits>
//#include <iomanip>

namespace vmf
{

// standard operation classes

class MinOperation: public IStatisticsOperation
{
public:
    MinOperation( unsigned dataType ) {};
    virtual ~MinOperation() {};

public:
    virtual std::string getName() const { return std::string("MinOperation"); };
    virtual bool canImmediate( Mode mode ) const { return true; };
    virtual void handleValue( Mode mode, const FieldValue& fieldValue ) const {};
    virtual void reset() {};
    virtual FieldValue getValue() const { return m_value; };

private:
    FieldValue m_value;
};

class MaxOperation: public IStatisticsOperation
{
public:
    MaxOperation( unsigned dataType ) {};
    virtual ~MaxOperation() {};

public:
    virtual std::string getName() const { return std::string("MaxOperation"); };
    virtual bool canImmediate( Mode mode ) const { return true; };
    virtual void handleValue( Mode mode, const FieldValue& fieldValue ) const {};
    virtual void reset() {};
    virtual FieldValue getValue() const { return m_value; };

private:
    FieldValue m_value;
};

class AverageOperation: public IStatisticsOperation
{
public:
    AverageOperation( unsigned dataType ) {};
    virtual ~AverageOperation() {};

public:
    virtual std::string getName() const { return std::string("AverageOperation"); };
    virtual bool canImmediate( Mode mode ) const { return true; };
    virtual void handleValue( Mode mode, const FieldValue& fieldValue ) const {};
    virtual void reset() {};
    virtual FieldValue getValue() const { return m_value; };

private:
    FieldValue m_value;
};

class MedianOperation: public IStatisticsOperation
{
public:
    MedianOperation( unsigned dataType ) {};
    virtual ~MedianOperation() {};

public:
    virtual std::string getName() const { return std::string("MedianOperation"); };
    virtual bool canImmediate( Mode mode ) const { return true; };
    virtual void handleValue( Mode mode, const FieldValue& fieldValue ) const {};
    virtual void reset() {};
    virtual FieldValue getValue() const { return m_value; };

private:
    FieldValue m_value;
};

class CountOperation: public IStatisticsOperation
{
public:
    CountOperation( unsigned dataType ) {};
    virtual ~CountOperation() {};

public:
    virtual std::string getName() const { return std::string("CountOperation"); };
    virtual bool canImmediate( Mode mode ) const { return true; };
    virtual void handleValue( Mode mode, const FieldValue& fieldValue ) const {};
    virtual void reset() {};
    virtual FieldValue getValue() const { return m_value; };

private:
    FieldValue m_value;
};

class SumOperation: public IStatisticsOperation
{
public:
    SumOperation( unsigned dataType ) {};
    virtual ~SumOperation() {};

public:
    virtual std::string getName() const { return std::string("SumOperation"); };
    virtual bool canImmediate( Mode mode ) const { return true; };
    virtual void handleValue( Mode mode, const FieldValue& fieldValue ) const {};
    virtual void reset() {};
    virtual FieldValue getValue() const { return m_value; };

private:
    FieldValue m_value;
};

class LastValueOperation: public IStatisticsOperation
{
public:
    LastValueOperation( unsigned dataType ) {};
    virtual ~LastValueOperation() {};

public:
    virtual std::string getName() const { return std::string("LastValueOperation"); };
    virtual bool canImmediate( Mode mode ) const { return true; };
    virtual void handleValue( Mode mode, const FieldValue& fieldValue ) const {};
    virtual void reset() {};
    virtual FieldValue getValue() const { return m_value; };

private:
    FieldValue m_value;
};

// class IStatisticsOperation

IStatisticsOperation::IStatisticsOperation()
{
}

IStatisticsOperation::~IStatisticsOperation()
{
}

std::shared_ptr< IStatisticsOperation > IStatisticsOperation::create( Type operationType, Variant::Type dataType )
{
    std::shared_ptr< IStatisticsOperation > operation( nullptr );
    bool standard = true;
    switch( operationType )
    {
    case Min:
        operation = std::make_shared< MinOperation >( dataType );
        break;
    case Max:
        operation = std::make_shared< MaxOperation >( dataType );
        break;
    case Average:
        operation = std::make_shared< AverageOperation >( dataType );
        break;
    case Median:
        operation = std::make_shared< MedianOperation >( dataType );
        break;
    case Count:
        operation = std::make_shared< CountOperation >( dataType );
        break;
    case Sum:
        operation = std::make_shared< SumOperation >( dataType );
        break;
    case LastValue:
        operation = std::make_shared< LastValueOperation >( dataType );
        break;
    default:
        operation = MetadataStream::findUserOperation( operationType /*, dataType*/ );
        standard = false;
        break;
    }
    return validate( operation, standard );
}

std::shared_ptr< IStatisticsOperation > IStatisticsOperation::validate(
        std::shared_ptr< IStatisticsOperation > operation,
        bool standard )
{
    if( operation == nullptr )
    {
        VMF_EXCEPTION(NullPointerException, "Description pointer is empty!" );
    }
    if( !operation->canImmediate(Add) )
    {
        if( standard )
        {
            VMF_EXCEPTION(InternalErrorException, "Standard operation doesn't provide an Add/Immediate capability!" );
        }
        else
        {
            VMF_EXCEPTION(NotImplementedException, "User operation doesn't provide an Add/Immediate capability!" );
        }
    }
    return operation;
}

// class StatisticsDesc

StatisticsDesc::StatisticsDesc(
        const std::string& statisticsName,
        std::shared_ptr< MetadataDesc > metadataDesc,
        const std::string& fieldName )
    : m_statisticsName( statisticsName )
    , m_metadataDesc( metadataDesc )
    , m_fieldName( fieldName )
{
}

StatisticsDesc::~StatisticsDesc()
{
}

const std::string& StatisticsDesc::getStatisticsName() const
{
    return m_statisticsName;
}

std::shared_ptr< MetadataDesc > StatisticsDesc::getMetadataDesc() const
{
    return m_metadataDesc;
}

const std::string& StatisticsDesc::getFieldName() const
{
    return m_fieldName;
}

// class StatisticsField

StatisticsField::StatisticsField(
        std::shared_ptr< StatisticsDesc > desc,
        std::shared_ptr< IStatisticsOperation > operation )
    : m_desc( desc )
    , m_operation( operation )
    , m_dirty( false )
{
}

StatisticsField::~StatisticsField()
{
}

std::shared_ptr< StatisticsDesc > StatisticsField::getDesc() const
{
    return m_desc;
}

std::shared_ptr< IStatisticsOperation > StatisticsField::getOperation() const
{
    return m_operation;
}

bool StatisticsField::isDirty() const
{
    return m_dirty;
}

void StatisticsField::setDirty( bool dirtyState )
{
    m_dirty = dirtyState;
}

FieldValue StatisticsField::getValue() const
{
    return m_operation->getValue();
}

// class Statistics

Statistics::Statistics()
    : m_fields( std::vector< std::shared_ptr< StatisticsField >>() )
    , m_pStream( nullptr )
    , m_schemaName( "" )
    , m_dirty( false )
{
}

Statistics::~Statistics()
{
    m_pStream = nullptr;
}

void Statistics::handleMetadata(
        IStatisticsOperation::Mode mode,
        const std::shared_ptr< Metadata > spMetadata )
{
    for( auto spStatisticsField : m_fields )
    {
        const std::shared_ptr< StatisticsDesc > spStatisticsDesc = spStatisticsField->getDesc();
        const std::shared_ptr< MetadataDesc > spMetadataDesc = spMetadata->getDesc();
        if( spStatisticsDesc->getMetadataDesc() == spMetadataDesc )
        {
            const std::string& fieldName = spStatisticsDesc->getFieldName();
            Metadata::iterator it = spMetadata->findField( fieldName );
            if( it != spMetadata->end() )
            {
                FieldDesc fd;
                if( spMetadataDesc->getFieldDesc( fd, fieldName ) == true )
                {
                    // TODO: we can/must add also fd checks - e.g. value type
                    const FieldValue& fv = *it;
                    std::shared_ptr< IStatisticsOperation > operation =
                            spStatisticsField->getOperation();
                    if( operation->canImmediate( mode ))
                    {
                        operation->handleValue( mode, fv );
                    }
                    else
                    {
                        spStatisticsField->setDirty( true );
                        setDirty( true );
                    }
                }
            }
        }

    }
}

void Statistics::handleMetadataField(
        IStatisticsOperation::Mode mode,
        const std::shared_ptr< Metadata > spMetadata,
        const std::string& fieldName )
{
    for( auto spStatisticsField : m_fields )
    {
        const std::shared_ptr< StatisticsDesc > spStatisticsDesc = spStatisticsField->getDesc();
        const std::shared_ptr< MetadataDesc > spMetadataDesc = spMetadata->getDesc();
        if( (spStatisticsDesc->getMetadataDesc() == spMetadataDesc) &&
            (spStatisticsDesc->getFieldName() == fieldName) )
        {
            Metadata::iterator it = spMetadata->findField( fieldName );
            if( it != spMetadata->end() )
            {
                FieldDesc fd;
                if( spMetadataDesc->getFieldDesc( fd, fieldName ) == true )
                {
                    // TODO: we can/must add also fd checks - e.g. value type
                    const FieldValue& fv = *it;
                    std::shared_ptr< IStatisticsOperation > operation =
                            spStatisticsField->getOperation();
                    if( operation->canImmediate( mode ))
                    {
                        operation->handleValue( mode, fv );
                    }
                    else
                    {
                        spStatisticsField->setDirty( true );
                        setDirty( true );
                    }
                }
            }
        }

    }
}

void Statistics::rescan()
{
    if( isDirty() )
    {
        for( auto spStatisticsField : m_fields )
        {
            if( spStatisticsField->isDirty() )
            {
                std::shared_ptr< IStatisticsOperation > operation =
                        spStatisticsField->getOperation();
                operation->reset();
            }
        }
        MetadataSet ms = m_pStream->getAll();
        for( auto spMetadata : ms )
        {
            std::shared_ptr< MetadataDesc > spMetadataDesc = spMetadata->getDesc();
            for( auto spStatisticsField : m_fields )
            {
                if( spStatisticsField->isDirty() )
                {
                    std::shared_ptr< StatisticsDesc > spStatisticsDesc = spStatisticsField->getDesc();
                    if( (spMetadataDesc->getSchemaName() == m_schemaName) &&
                        (spStatisticsDesc->getMetadataDesc() == spMetadataDesc) )
                    {
                        const std::string& fieldName = spStatisticsDesc->getFieldName();
                        Metadata::iterator it = spMetadata->findField( fieldName );
                        if( it != spMetadata->end() )
                        {
                            FieldDesc fd;
                            if( spMetadataDesc->getFieldDesc( fd, fieldName ) == true )
                            {
                                // TODO: we can/must add also fd checks - e.g. value type
                                const FieldValue& fv = *it;
                                std::shared_ptr< IStatisticsOperation > operation =
                                        spStatisticsField->getOperation();
                                /* assert( operation->canImmediate( IStatisticsOperation::Add ) == true ); */
                                operation->handleValue( IStatisticsOperation::Add, fv );
                            }
                        }
                    }
                }
            }
        }
        for( auto spStatisticsField : m_fields )
        {
            if( spStatisticsField->isDirty() )
            {
                spStatisticsField->setDirty( false );
            }
        }
        setDirty( false );
    }
}

void Statistics::reset()
{
    for( auto spStatisticsField : m_fields )
    {
        std::shared_ptr< IStatisticsOperation > operation =
                spStatisticsField->getOperation();
        operation->reset();
        spStatisticsField->setDirty( false );
    }
    setDirty( false );
}

bool Statistics::isDirty() const
{
    return m_dirty;
}

void Statistics::setDirty( bool dirtyState )
{
    m_dirty = dirtyState;
}

void Statistics::setSchemaName( std::string schemaName )
{
    m_schemaName = schemaName;
}

void Statistics::setMetadataStream( const MetadataStream* pStream )
{
    m_pStream = pStream;
}

void Statistics::addField(
        const std::string& statisticsName,
        const std::string& metadataName,
        const std::string& fieldName,
        unsigned operationType )
{
    if( findStatisticsField( statisticsName ) != nullptr )
    {
        VMF_EXCEPTION( IncorrectParamException, "Statistics field with same name already exists!" );
    }

    std::shared_ptr< MetadataSchema > spSchema = getSchema();

    std::shared_ptr< MetadataDesc > spMetadataDesc = spSchema->findMetadataDesc( metadataName );
    if( spMetadataDesc == nullptr )
    {
        VMF_EXCEPTION( IncorrectParamException, "Unknown metadata name!" );
    }

    FieldDesc fd;
    if( spMetadataDesc->getFieldDesc( fd, fieldName ) != true )
    {
        VMF_EXCEPTION( IncorrectParamException, "Unknown metadata field name!" );
    }

    std::shared_ptr< StatisticsDesc > spStatisticsDesc =
            std::make_shared< StatisticsDesc >( statisticsName, spMetadataDesc, fieldName );
    std::shared_ptr< IStatisticsOperation > spOperation =
            IStatisticsOperation::create( IStatisticsOperation::Type(operationType), fd.type );

    std::shared_ptr< StatisticsField > spStatisticsField =
            std::make_shared< StatisticsField >( spStatisticsDesc, spOperation );

    m_fields.push_back( spStatisticsField );
}

const std::shared_ptr< StatisticsField > Statistics::findStatisticsField( const std::string& statisticsName ) const
{
    auto it = std::find_if( m_fields.begin(), m_fields.end(), [&]( const std::shared_ptr< StatisticsField >& spField )->bool
    {
        return spField->getDesc()->getStatisticsName() == statisticsName;
    });

    if( it != m_fields.end() )
        return *it;

    return nullptr;
}

std::shared_ptr< MetadataSchema > Statistics::getSchema() const
{
    if( (m_pStream != nullptr) && !m_schemaName.empty() )
    {
        std::shared_ptr< MetadataSchema > spSchema = m_pStream->getSchema( m_schemaName );
        if( spSchema != nullptr )
        {
            return spSchema;
        }
    }
    VMF_EXCEPTION( IncorrectParamException, "Unknown schema for statistics object" );
}

} // namespace vmf
