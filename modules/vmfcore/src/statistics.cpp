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

// TODO: std op methods need to be implemented

IOperation::IOperation() {}
IOperation::~IOperation() {}

std::shared_ptr<IOperation> IOperation::create( int id, const std::string& name )
{
    switch( id )
    {
    case UserId:
	throw std::runtime_error( "[INTERNAL] vmf::IOperation::create() : User operation currently not supported" );
    case MinId:
	return std::make_shared<MinOp>();
    case MaxId:
	return std::make_shared<MaxOp>();
    case AverageId:
	return std::make_shared<AverageOp>();
    case MedianId:
	return std::make_shared<MedianOp>();
    case CountId:
	return std::make_shared<CountOp>();
    case SumId:
	return std::make_shared<SumOp>();
    case LastValueId:
	return std::make_shared<LastValueOp>();
    default:
	throw std::runtime_error( "[FATAL] vmf::IOperation::create() : Undefined operation Id specified: " + std::to_string( id ));
    }
}

MinOp::MinOp(): IOperation() {}
MinOp::~MinOp() {}
int MinOp::getId() const { return MinId; }
bool MinOp::canInc( unsigned flags ) const { return bool((flags & Add) != 0); }
Variant MinOp::getValue() const { return value; }
void MinOp::initValue() {}
void MinOp::addValue( const Variant& in ) {}
void MinOp::removeValue( const Variant& in ) {}
void MinOp::changeValue( const Variant& in ) {}

MaxOp::MaxOp(): IOperation() {}
MaxOp::~MaxOp() {}
int MaxOp::getId() const { return MaxId; }
bool MaxOp::canInc( unsigned flags ) const { return bool((flags & Add) != 0); }
Variant MaxOp::getValue() const { return value; }
void MaxOp::initValue() {}
void MaxOp::addValue( const Variant& in ) {}
void MaxOp::removeValue( const Variant& in ) {}
void MaxOp::changeValue( const Variant& in ) {}

AverageOp::AverageOp(): IOperation() {}
AverageOp::~AverageOp() {}
int AverageOp::getId() const { return AverageId; }
bool AverageOp::canInc( unsigned flags ) const { return bool((flags & All) != 0); }
Variant AverageOp::getValue() const { return value; }
void AverageOp::initValue() {}
void AverageOp::addValue( const Variant& in ) {}
void AverageOp::removeValue( const Variant& in ) {}
void AverageOp::changeValue( const Variant& in ) {}

MedianOp::MedianOp(): IOperation() {}
MedianOp::~MedianOp() {}
int MedianOp::getId() const { return MedianId; }
bool MedianOp::canInc( unsigned flags ) const { return false; }
Variant MedianOp::getValue() const { return value; }
void MedianOp::initValue() {}
void MedianOp::addValue( const Variant& in ) {}
void MedianOp::removeValue( const Variant& in ) {}
void MedianOp::changeValue( const Variant& in ) {}

CountOp::CountOp(): IOperation() {}
CountOp::~CountOp() {}
int CountOp::getId() const { return CountId; }
bool CountOp::canInc( unsigned flags ) const { return bool((flags & All) != 0); }
Variant CountOp::getValue() const { return value; }
void CountOp::initValue() { value = Variant( vmf_integer( 0 )); }
void CountOp::addValue( const Variant& /*in*/ ) { value = Variant( value.get_integer()+1 ); }
void CountOp::removeValue( const Variant& /*in*/ ) { value = Variant( value.get_integer()-1 ); }
void CountOp::changeValue( const Variant& /*in*/ ) { /* changing of metadata value doesn't change metadata count */ }

SumOp::SumOp(): IOperation() {}
SumOp::~SumOp() {}
int SumOp::getId() const { return SumId; }
bool SumOp::canInc( unsigned flags ) const { return bool((flags & All) != 0); }
Variant SumOp::getValue() const { return value; }
void SumOp::initValue() {}
void SumOp::addValue( const Variant& in ) {}
void SumOp::removeValue( const Variant& in ) {}
void SumOp::changeValue( const Variant& in ) {}

LastValueOp::LastValueOp(): IOperation() {}
LastValueOp::~LastValueOp() {}
int LastValueOp::getId() const { return LastValueId; }
bool LastValueOp::canInc( unsigned flags ) const { return bool((flags & Add) != 0); }
Variant LastValueOp::getValue() const { return value; }
void LastValueOp::initValue() {}
void LastValueOp::addValue( const Variant& in ) {}
void LastValueOp::removeValue( const Variant& in ) {}
void LastValueOp::changeValue( const Variant& in ) {}

Statistics::Statistics()
    : m_dirty( false )
{
}

Statistics::~Statistics()
{
}

void Statistics::metadataAdded( const std::shared_ptr< Metadata >& spMetadata )
{
    std::shared_ptr< MetadataDesc > desc = spMetadata->getDesc();
    for( int is = 0;  is < (int)m_items.size();  ++is )
    {
        StatisticsItem& si = m_items[is];
        if( si.metadata == desc->getMetadataName() )
        {
            FieldDesc fd;
            Metadata::iterator fit = spMetadata->findField( si.field );
            if( (fit != spMetadata->end()) && (true == desc->getFieldDesc( fd, si.field )))
            {
                // TODO: we can/must add also fd checks - e.g. value type
                vmf::FieldValue& fv = *fit;
/*
                if( si.operation->canInc( IOperation::Add ))
                {
                    si.operation->addValue( fv );
                }
                else
                {
                    si.dirty = true;
                    m_dirty = true;
                }
*/
                // TODO: we assume that all operations can add incrementally, check this at configuration stage
                si.operation->addValue( fv );
            }
        }
    }
}

void Statistics::metadataRemoved( const std::shared_ptr< Metadata >& spMetadata )
{
    std::shared_ptr< MetadataDesc > desc = spMetadata->getDesc();
    for( int is = 0;  is < (int)m_items.size();  ++is )
    {
        StatisticsItem& si = m_items[is];
        if( si.metadata == desc->getMetadataName() )
        {
            FieldDesc fd;
            Metadata::iterator fit = spMetadata->findField( si.field );
            if( (fit != spMetadata->end()) && (true == desc->getFieldDesc( fd, si.field )))
            {
                // TODO: we can/must add also fd checks - e.g. value type
                vmf::FieldValue& fv = *fit;
                if( si.operation->canInc( IOperation::Remove ))
                {
                    si.operation->removeValue( fv );
                }
                else
                {
                    si.dirty = true;
                    m_dirty = true;
                }
            }
        }
    }
}

void Statistics::metadataChanged( const std::shared_ptr< Metadata >& spMetadata, const std::string& fieldName )
{
    std::shared_ptr< MetadataDesc > desc = spMetadata->getDesc();
    for( int is = 0;  is < (int)m_items.size();  ++is )
    {
        StatisticsItem& si = m_items[is];
        if( (si.metadata == desc->getMetadataName()) && (si.field == fieldName) )
        {
            FieldDesc fd;
            Metadata::iterator fit = spMetadata->findField( si.field );
            if( (fit != spMetadata->end()) && (true == desc->getFieldDesc( fd, si.field )))
            {
                // TODO: we can/must add also fd checks - e.g. value type
                vmf::FieldValue& fv = *fit;
                if( si.operation->canInc( IOperation::Change ))
                {
                    si.operation->changeValue( fv );
                }
                else
                {
                    si.dirty = true;
                    m_dirty = true;
                }
            }
        }
    }
}

bool Statistics::isDirty() const
{
    return m_dirty;
}

void Statistics::rescan()
{
    if( m_dirty )
    {
        for( int is = 0;  is < (int)m_items.size();  ++is )
        {
            StatisticsItem& si = m_items[is];
            if( si.dirty )
                si.operation->initValue();
        }
        MetadataSet ms = m_pStream->getAll();
        for( int im = 0;  im < (int)ms.size();  ++im )
        {
            std::shared_ptr< Metadata > spMetadata = ms[im];
            std::shared_ptr< MetadataDesc > desc = spMetadata->getDesc();
            for( int is = 0;  is < (int)m_items.size();  ++is )
            {
                StatisticsItem& si = m_items[is];
                if( si.dirty && (desc->getSchemaName() == m_schema) && (desc->getMetadataName() == si.metadata) )
                {
                    FieldDesc fd;
                    Metadata::iterator fit = spMetadata->findField( si.field );
                    if( (fit != spMetadata->end()) && (true == desc->getFieldDesc( fd, si.field )))
                    {
                        // TODO: we can/must add also fd checks - e.g. value type
                        vmf::FieldValue& fv = *fit;
                        // TODO: we assume that all operations can add incrementally, check this at configuration stage
                        si.operation->addValue( fv );
                    }
                }
            }
        }
        for( int is = 0;  is < (int)m_items.size();  ++is )
        {
            StatisticsItem& si = m_items[is];
            si.dirty = false;
        }
        m_dirty = false;
    }
}

void Statistics::reset()
{
    for( int is = 0;  is < (int)m_items.size();  ++is )
    {
        StatisticsItem& si = m_items[is];
        si.operation->initValue();
        si.dirty = false;
    }
    m_dirty = false;
}

void Statistics::addStatisticsItem( const StatisticsItem& item )
{
    for( int is = 0;  is < (int)m_items.size();  ++is )
    {
        StatisticsItem& si = m_items[is];
        if( si.name == item.name )
        {
            VMF_EXCEPTION(IncorrectParamException, "Duplicate statistics name for statistics object" );
        }
    }
    m_items.push_back( item );
    m_items.back().dirty = false;
    validate();
}

void Statistics::setSchemaName( std::string schema )
{
    m_schema = schema;
    validate();
}

void Statistics::setMetadataStream( const MetadataStream* pStream )
{
    m_pStream = pStream;
    validate();
}

void Statistics::validate()
{
    if( (m_pStream != nullptr) && !m_schema.empty() )
    {
        if( m_pStream->getSchema( m_schema ) == nullptr )
        {
            VMF_EXCEPTION(IncorrectParamException, "Unknown schema for statistics object" );
        }
        // TODO: for m_items vector:
        //       - validate std::string metadata
        //       - validate std::string field;
        //       - validate operation type against metadata/field value type
        reset();
    }
}

std::vector<StatisticsItem> Statistics::getStatisticsItems() const
{
    return m_items;
}

} // namespace vmf
