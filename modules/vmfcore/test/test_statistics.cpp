/*
 * Copyright 2016 Intel(r) Corporation
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
#include "test_precomp.hpp"

#include <fstream>

#if defined(WIN32)
  const char delim = '\\';
#else
  const char delim = '/';
#endif

class TestStat : public ::testing::Test
{
protected:
    void SetUp()
    {
        statName = "SimpleStat";
        defUpdateMode = vmf::StatUpdateMode::Manual;
        stat = std::make_shared< vmf::Stat >( statName, emptyFields, defUpdateMode );

        defUpdateTimeout = 0;
        defStatState = vmf::StatState::UpToDate;
        inexistingFieldName = "AnyNameYouWish";
    }

    std::string statName;
    vmf::StatUpdateMode::Type defUpdateMode;
    unsigned defUpdateTimeout;
    vmf::StatState::Type defStatState;
    std::vector< vmf::StatField > emptyFields;
    std::string inexistingFieldName;
    std::shared_ptr< vmf::Stat > stat;
};

TEST_F( TestStat, Creation )
{
    ASSERT_EQ( stat->getName(), statName );
    ASSERT_EQ( stat->getState(), defStatState );
}

TEST_F( TestStat, UpdateMode )
{
    vmf::StatUpdateMode::Type updateMode = vmf::StatUpdateMode::OnAdd;
    ASSERT_NE( updateMode, defUpdateMode );

    ASSERT_EQ( stat->getUpdateMode(), defUpdateMode );

    EXPECT_NO_THROW( stat->setUpdateMode( updateMode ));
    ASSERT_EQ( stat->getUpdateMode(), updateMode );

    EXPECT_NO_THROW( stat->setUpdateMode( defUpdateMode ));
    ASSERT_EQ( stat->getUpdateMode(), defUpdateMode );
}

#define CHECK_TO_STRING( _x ) \
    { std::string _s_; EXPECT_NO_THROW( _s_ = vmf::StatUpdateMode::toString( _x )); ASSERT_EQ( _s_, #_x ); }
#define CHECK_FROM_STRING( _x ) \
    { vmf::StatUpdateMode::Type _v_; EXPECT_NO_THROW( _v_ = vmf::StatUpdateMode::fromString( #_x )); ASSERT_EQ( _v_, _x ); }
TEST_F( TestStat, UpdateModeStrings )
{
    vmf::StatUpdateMode::Type val;
    std::string str;

    CHECK_TO_STRING( vmf::StatUpdateMode::Disabled );
    CHECK_TO_STRING( vmf::StatUpdateMode::Manual );
    CHECK_TO_STRING( vmf::StatUpdateMode::OnAdd );
    CHECK_TO_STRING( vmf::StatUpdateMode::OnTimer );

    val = vmf::StatUpdateMode::Type( int( vmf::StatUpdateMode::Disabled ) - 1 );
    EXPECT_THROW( str = vmf::StatUpdateMode::toString( val ), vmf::IncorrectParamException );
    val = vmf::StatUpdateMode::Type( int( vmf::StatUpdateMode::OnTimer ) + 1 );
    EXPECT_THROW( str = vmf::StatUpdateMode::toString( val ), vmf::IncorrectParamException );

    CHECK_FROM_STRING( vmf::StatUpdateMode::Disabled );
    CHECK_FROM_STRING( vmf::StatUpdateMode::Manual );
    CHECK_FROM_STRING( vmf::StatUpdateMode::OnAdd );
    CHECK_FROM_STRING( vmf::StatUpdateMode::OnTimer );

    str = "AnyUnknownStringYouWant";
    EXPECT_THROW( val = vmf::StatUpdateMode::fromString( str ), vmf::IncorrectParamException );
    str = "YetAnotherUnknownString";
    EXPECT_THROW( val = vmf::StatUpdateMode::fromString( str ), vmf::IncorrectParamException );
}
#undef CHECK_TO_STRING
#undef CHECK_FROM_STRING

TEST_F( TestStat, UpdateTimeout )
{
    unsigned updateTimeout = defUpdateTimeout + 100;
    ASSERT_NE( updateTimeout, defUpdateTimeout );

    ASSERT_EQ( stat->getUpdateTimeout(), defUpdateTimeout );

    EXPECT_NO_THROW( stat->setUpdateTimeout( updateTimeout ));
    ASSERT_EQ( stat->getUpdateTimeout(), updateTimeout );

    EXPECT_NO_THROW( stat->setUpdateTimeout( defUpdateTimeout ));
    ASSERT_EQ( stat->getUpdateTimeout(), defUpdateTimeout );
}

TEST_F( TestStat, EmptyFields )
{
    std::vector< std::string > fieldNames;

    EXPECT_NO_THROW( fieldNames = stat->getAllFieldNames() );
    ASSERT_EQ( fieldNames.size(), emptyFields.size() );

    vmf::StatField field;
    EXPECT_THROW( field = stat->getField( inexistingFieldName ), vmf::NotFoundException );

    vmf::Variant value;
    EXPECT_THROW( value = (*stat)[ inexistingFieldName ], vmf::NotFoundException );
}

class TestStatFields : public ::testing::Test
{
protected:
    struct NameData
    {
        NameData( const std::string& n, const std::string& sn, const std::string& mn,
                  const std::string& fn, const std::string& on )
            : name( n ), schemaName( sn ), metadataName( mn ), fieldName( fn ), opName ( on ) {}
        NameData() {}
        ~NameData() {}
        const std::string name, schemaName, metadataName, fieldName, opName;
    };

    void initNameData()
    {
        nameData.emplace_back( "MinField", "MetadataSchema", "MetadataName", "FieldName", vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Min ));
        nameData.emplace_back( "MaxField", "MetadataSchema", "MetadataName", "FieldName", vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Max ));
        nameData.emplace_back( "AverageField", "MetadataSchema", "MetadataName", "FieldName", vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Average ));
        nameData.emplace_back( "CountField", "MetadataSchema", "MetadataName", "FieldName", vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Count ));
        nameData.emplace_back( "SumField", "MetadataSchema", "MetadataName", "FieldName", vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Sum ));
        nameData.emplace_back( "LastField", "MetadataSchema", "MetadataName", "FieldName", vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Last ));
    }

    void SetUp()
    {
        statName = "SimpleStat";
        defUpdateMode = vmf::StatUpdateMode::Manual;

        initNameData();
        for( auto& n : nameData )
            statFields.emplace_back( n.name, n.schemaName, n.metadataName, n.fieldName, n.opName );

        stat = std::make_shared< vmf::Stat >( statName, statFields, defUpdateMode );
    }

    std::string statName;
    vmf::StatUpdateMode::Type defUpdateMode;
    std::vector< NameData > nameData;
    std::vector< vmf::StatField > statFields;
    std::shared_ptr< vmf::Stat > stat;
};

TEST_F( TestStatFields, Creation )
{
    std::vector< std::string > fieldNames;

    EXPECT_NO_THROW( fieldNames = stat->getAllFieldNames() );
    ASSERT_EQ( fieldNames.size(), statFields.size() );

    auto it = nameData.begin();
    for( auto& fieldName : fieldNames )
    {
        const NameData& n = *it;

        const vmf::StatField* testField = 0;
        EXPECT_NO_THROW( testField = &(stat->getField( fieldName )));

        ASSERT_EQ( testField->getName(), n.name );
        ASSERT_EQ( testField->getSchemaName(), n.schemaName );
        ASSERT_EQ( testField->getMetadataName(), n.metadataName );
        ASSERT_EQ( testField->getFieldName(), n.fieldName );
        ASSERT_EQ( testField->getOpName(), n.opName );

        ASSERT_EQ( testField->getMetadataDesc(), nullptr );
        vmf::FieldDesc emptyDesc;
        ASSERT_EQ( testField->getFieldDesc(), emptyDesc );

        ASSERT_NE( it, nameData.end() );
        ++it;
    }
}

class TestStatOperations : public ::testing::Test
{
protected:
    enum Flags // operation capability flags
    {
        InputMask  = 0x000f, // input mask
        InputInt   = 0x0001, //   accepts integer
        InputReal  = 0x0002, //   accepts real
        InputAny   = 0x0004, //   accepts any type (types can be mixed)
        OutputMask = 0x00f0, // output mask
        OutputInt  = 0x0010, //   produces integer
        OutputReal = 0x0020, //   produces real
        OutputSame = 0x0040, //   produces the same type as input
        ResetMask  = 0x0f00, // reset mask
        ResetInt   = 0x0100, //   reset to integer 0
        ResetReal  = 0x0200, //   reset to real 0
        ResetEmpty = 0x0400, //   reset to empty value (type_unknown)
        CanMask    = 0xf000, // can mask
        CanAdd     = 0x1000, //   can add value
        CanRemove  = 0x2000  //   can remove value
    };

    // Min      i:i r:r A:t R:f
    // Max
    // Average  i:r r:r A:t R:t
    // Count    any:i   A:t R:t
    // Sum      i:i r:r A:t R:t
    // Last     any:id  A:t R:f

    void testBuiltin( const std::string& name, unsigned flags )
    {
        bool status;
        std::string str;
        vmf::Variant val1,val2,val3,bad,res;
        vmf::StatOpBase* op = nullptr;

        // created op by fabric call
        EXPECT_NO_THROW( op = vmf::StatOpFactory::create( name ));
        ASSERT_NE( op, nullptr );

        // check name consistency
        EXPECT_NO_THROW( str = op->name() );
        ASSERT_EQ( str, name );

        // check reset result
        EXPECT_NO_THROW( op->reset() );
        EXPECT_NO_THROW( res = op->value() );

        ASSERT_NE( flags & ResetMask, 0 );
        vmf::Variant::Type resetType = res.getType();
        if( flags & ResetInt )
            ASSERT_EQ( resetType, vmf::Variant::type_integer );
        else if( flags & ResetReal )
            ASSERT_EQ( resetType, vmf::Variant::type_real );
        else /*if( flags & ResetEmpty )*/
            ASSERT_EQ( resetType, vmf::Variant::type_unknown );

        // provide right output type
        ASSERT_NE( flags & OutputMask, 0 );
        vmf::Variant::Type outputType = vmf::Variant::type_unknown;
        if( flags & OutputInt )
            outputType = vmf::Variant::type_integer;
        else if( flags & OutputReal )
            outputType = vmf::Variant::type_real;
        else /*if( flags & OutputSame )*/
            outputType = vmf::Variant::type_unknown; // depends on input type, see below

        // provide consistent test inputs
        ASSERT_NE( flags & InputMask, 0 );
        if( flags & InputInt )
        {
            val1 = vmf::Variant( (vmf::vmf_integer)131 );
            val2 = vmf::Variant( (vmf::vmf_integer)-13 );
            val3 = vmf::Variant( (vmf::vmf_integer) 75 );
            bad = vmf::Variant( (vmf::vmf_real)77.13 );
            if( flags & OutputSame )
                outputType = vmf::Variant::type_integer; // depends on input type, fixed
        }
        else if( flags & InputReal )
        {
            val1 = vmf::Variant( (vmf::vmf_real) 36.6 );
            val2 = vmf::Variant( (vmf::vmf_real)307.1 );
            val3 = vmf::Variant( (vmf::vmf_real)-3.14 );
            bad = vmf::Variant( (vmf::vmf_integer)77 );
            if( flags & OutputSame )
                outputType = vmf::Variant::type_real; // depends on input type, fixed
        }
        else /*if( flags & InputAny )*/
        {
            val1 = vmf::Variant( (vmf::vmf_integer) 352 );
            val2 = vmf::Variant( (vmf::vmf_real)   13.7 );
            val3 = vmf::Variant( (vmf::vmf_string)"any" );
            bad = vmf::Variant(); // op accepts any type, so there's no bad
            // output depends on input type of individual value, must be checked individually
        }

        // try to remove anything on empty state
        if( flags & CanRemove )
        {
            EXPECT_THROW( status = op->handle( vmf::StatAction::Remove, val1 ), vmf::NotImplementedException );
        }

        // must always support Add
        ASSERT_NE( flags & CanAdd, 0 );

        // Add first value
        EXPECT_NO_THROW( status = op->handle( vmf::StatAction::Add, val1 ));
        ASSERT_EQ( status, true );

        EXPECT_NO_THROW( res = op->value() );
        if( outputType == vmf::Variant::type_unknown )
            ASSERT_EQ( res.getType(), val1.getType() );
        else
            ASSERT_EQ( res.getType(), outputType );

        // Add second value
        EXPECT_NO_THROW( status = op->handle( vmf::StatAction::Add, val2 ));
        ASSERT_EQ( status, true );

        EXPECT_NO_THROW( res = op->value() );
        if( outputType == vmf::Variant::type_unknown )
            ASSERT_EQ( res.getType(), val2.getType() );
        else
            ASSERT_EQ( res.getType(), outputType );

        // Add third value
        EXPECT_NO_THROW( status = op->handle( vmf::StatAction::Add, val3 ));
        ASSERT_EQ( status, true );

        EXPECT_NO_THROW( res = op->value() );
        if( outputType == vmf::Variant::type_unknown )
            ASSERT_EQ( res.getType(), val3.getType() );
        else
            ASSERT_EQ( res.getType(), outputType );

        // try to handle bad input
        if( bad.getType() == vmf::Variant::type_unknown )
        {
            EXPECT_NO_THROW( status = op->handle( vmf::StatAction::Add, bad ));
            ASSERT_EQ( status, true );
        }
        else
        {
            EXPECT_THROW( status = op->handle( vmf::StatAction::Add, bad ), vmf::TypeCastException );
        }

        // try to remove anything on non-empty state
        if( flags & CanRemove )
        {
            EXPECT_NO_THROW( status = op->handle( vmf::StatAction::Remove, val1 ));
            ASSERT_EQ( status, true );
        }

        delete op;
    }

    void testUserOperation()
    {
        std::string name,str;
        vmf::StatOpBase* op = nullptr;

        str = "AnyUnknownStringYouWant";
        EXPECT_THROW( op = vmf::StatOpFactory::create( str ), vmf::NotFoundException );
        ASSERT_EQ( op, nullptr );

        str = "YetAnotherUnknownString";
        EXPECT_THROW( op = vmf::StatOpFactory::create( str ), vmf::NotFoundException );
        ASSERT_EQ( op, nullptr );

        EXPECT_NO_THROW( op = UserOp::createInstance() );
        ASSERT_NE( op, nullptr );
        EXPECT_NO_THROW( name = op->name() );
        ASSERT_EQ( name, UserOp::userOpName );
        delete op; op = nullptr;

        EXPECT_THROW( vmf::StatOpFactory::registerUserOp( nullptr ), vmf::NullPointerException );
        EXPECT_NO_THROW( vmf::StatOpFactory::registerUserOp( UserOp::createInstance ));
        EXPECT_THROW( vmf::StatOpFactory::registerUserOp( UserOp::createInstance2 ), vmf::IncorrectParamException );

        ASSERT_EQ( op, nullptr );
        EXPECT_NO_THROW( op = vmf::StatOpFactory::create( name ));
        ASSERT_NE( op, nullptr );
        EXPECT_NO_THROW( str = op->name() );
        ASSERT_EQ( str, name );
        delete op; op = nullptr;
    }

    class UserOp: public vmf::StatOpBase
    {
    public:
        UserOp()
            {}
        virtual ~UserOp()
            {}

    public:
        virtual std::string name() const
            { return userOpName; }
        virtual void reset()
            { m_value = vmf::Variant(); }
        virtual bool handle( vmf::StatAction::Type action, const vmf::Variant& fieldValue )
            {
                switch( action )
                {
                case vmf::StatAction::Add:
                    m_value = fieldValue;
                    break;
                case vmf::StatAction::Remove:
                    return false;
                }
                return true;
            }
        virtual vmf::Variant value() const
            { return m_value; }

    private:
        vmf::Variant m_value;

    public:
        static const std::string userOpName;
        static vmf::StatOpBase* createInstance()
            { return new UserOp(); }
        static vmf::StatOpBase* createInstance2()
            { return new UserOp(); }
    };
};

/*static*/ const std::string TestStatOperations::UserOp::userOpName = "***TestStatOperations::UserOp::userOpName***";

TEST_F( TestStatOperations, BuiltinMin )
{
    testBuiltin( vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Min ),
                 InputInt | InputReal | OutputSame | ResetEmpty | CanAdd );
}

TEST_F( TestStatOperations, BuiltinMax )
{
    testBuiltin( vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Max ),
                 InputInt | InputReal | OutputSame | ResetEmpty | CanAdd );
}

TEST_F( TestStatOperations, BuiltinAverage )
{
    testBuiltin( vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Average ),
                 InputInt | InputReal | OutputReal | ResetEmpty | CanAdd | CanRemove );
}

TEST_F( TestStatOperations, BuiltinCount )
{
    testBuiltin( vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Count ),
                 InputAny | OutputInt | ResetInt | CanAdd | CanRemove );
}

TEST_F( TestStatOperations, BuiltinSum )
{
    testBuiltin( vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Sum ),
                 InputInt | InputReal | OutputSame | ResetEmpty | CanAdd | CanRemove );
}

TEST_F( TestStatOperations, BuiltinLast )
{
    testBuiltin( vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Last ),
                 InputAny | OutputSame | ResetEmpty | CanAdd );
}

TEST_F( TestStatOperations, UserOperation )
{
    testUserOperation();
}

class TestStatistics : public ::testing::TestWithParam< vmf::StatUpdateMode::Type >
{
protected:
    void SetUp()
    {
        vmf::initialize();

        fnWorkingPath = getWorkingPath();
        fnInputName   = "BlueSquare.avi";

        mcSchemaName = "PersonSchema";
        mcDescName   = "Person";
        mcPersonName = "Name";
        mcAgeName    = "Age";
        mcGrowthName = "Growth";
        mcSalaryName = "Salary";

        scStatName            = "PersonStatistics";
        scPersonNameCount     = "PersonNameCount";
        scPersonNameLast      = "PersonNameLast";
        scPersonAgeMin        = "PersonAgeMin";
        scPersonAgeMax        = "PersonAgeMax";
        scPersonGrowthAverage = "PersonGrowthAverage";
        scPersonSalarySum     = "PersonSalarySum";
    }

    void TearDown()
    {
        vmf::terminate();
    }

    std::string getWorkingPath() const
    {
        std::string path = ::testing::internal::g_argvs[0];
        size_t pos = path.find_last_of( delim );
        if( pos != std::string::npos )
            path = path.substr( 0, pos + 1 );
		else
			path = "";
        return path;
    }

    void createFile( const std::string& dstName )
    {
        std::string srcName = fnWorkingPath + fnInputName;

        std::ifstream source( srcName, std::ios::binary );
        if( !source )
            VMF_EXCEPTION( vmf::IncorrectParamException, "Error opening input file: "  + srcName );

        std::ofstream dest( dstName, std::ios::binary );
        if( !dest )
            VMF_EXCEPTION( vmf::IncorrectParamException, "Error opening output file: " + dstName );

        dest << source.rdbuf();
    }

    void configureSchema( vmf::MetadataStream& stream )
    {
        scFieldDesc.emplace_back( mcPersonName, vmf::Variant::type_string );
        scFieldDesc.emplace_back( mcAgeName, vmf::Variant::type_integer );
        scFieldDesc.emplace_back( mcGrowthName, vmf::Variant::type_integer );
        scFieldDesc.emplace_back( mcSalaryName, vmf::Variant::type_integer );

        scMetadataDesc = std::make_shared< vmf::MetadataDesc >( mcDescName, scFieldDesc );
        scMetadataSchema = std::make_shared< vmf::MetadataSchema >( mcSchemaName );

        scMetadataSchema->add( scMetadataDesc );
        stream.addSchema( scMetadataSchema );
    }

    void configureStatistics( vmf::MetadataStream& stream )
    {
        std::vector< vmf::StatField > fields;
        fields.emplace_back( scPersonNameCount, mcSchemaName, mcDescName, mcPersonName, vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Count ));
        fields.emplace_back( scPersonNameLast, mcSchemaName, mcDescName, mcPersonName, vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Last ));
        fields.emplace_back( scPersonAgeMin, mcSchemaName, mcDescName, mcAgeName, vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Min ));
        fields.emplace_back( scPersonAgeMax, mcSchemaName, mcDescName, mcAgeName, vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Max ));
        fields.emplace_back( scPersonGrowthAverage, mcSchemaName, mcDescName, mcGrowthName, vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Average ));
        fields.emplace_back( scPersonSalarySum, mcSchemaName, mcDescName, mcSalaryName, vmf::StatOpFactory::builtinName( vmf::StatOpFactory::BuiltinOp::Sum ));
        stream.addStat( scStatName, fields, vmf::StatUpdateMode::Disabled );
    }

    void initStatistics()
    {
        stNameCount = 0;
        stNameLast = "";
        stAgeMin = 0;
        stAgeMax = 0;
        stGrowthAverage = 0.0; stGrowthAverageSum = 0; stGrowthAverageCount = 0;
        stSalarySum = 0;
        stFirstTimeOnce = true;
    }

    void finalizeStatistics()
    {
        if( stGrowthAverageCount != 0 )
            stGrowthAverage = (vmf::vmf_real)stGrowthAverageSum / (vmf::vmf_real)stGrowthAverageCount;
        else
            stGrowthAverage = 0.0;
    }

    void addMetadata( vmf::MetadataStream& stream, const vmf::vmf_string& name, vmf::vmf_integer age, vmf::vmf_integer growth, vmf::vmf_integer salary, bool doStatistics )
    {
        std::shared_ptr<vmf::Metadata> metadata = std::make_shared< vmf::Metadata >( scMetadataDesc );

        metadata->emplace_back( mcPersonName, name );
        if( doStatistics )
        {
            ++stNameCount;
            stNameLast = name;
        }

        metadata->emplace_back( mcAgeName, age );
        if( doStatistics )
        {
            stAgeMin = (stFirstTimeOnce ? age : std::min( stAgeMin, age ));
            stAgeMax = (stFirstTimeOnce ? age : std::max( stAgeMax, age ));
        }

        metadata->emplace_back( mcGrowthName, growth );
        if( doStatistics )
        {
            stGrowthAverageSum += growth;
            ++stGrowthAverageCount;
        }

        metadata->emplace_back( mcSalaryName, salary );
        if( doStatistics )
        {
            stSalarySum += salary;
        }

        stream.add( metadata );

        if( doStatistics )
        {
            stFirstTimeOnce = false;
        }
    }

    void putMetadata( vmf::MetadataStream& stream, bool doStatistics )
    {
        if( doStatistics )
            initStatistics();

        addMetadata( stream, "Peter", 53, 185, 5000, doStatistics );
        addMetadata( stream, "Jessica", 31, 176, 3000, doStatistics );
        addMetadata( stream, "Matthias", 41, 192, 7000, doStatistics );
        addMetadata( stream, "John", 29, 180, 5500, doStatistics );

        if( doStatistics )
            finalizeStatistics();
    }

    void checkStatistics( const vmf::Stat& stat, vmf::StatUpdateMode::Type updateMode, bool doCompareValues )
    {
        vmf::Variant nameCount     = stat[scPersonNameCount];
        vmf::Variant nameLast      = stat[scPersonNameLast];
        vmf::Variant ageMin        = stat[scPersonAgeMin];
        vmf::Variant ageMax        = stat[scPersonAgeMax];
        vmf::Variant growthAverage = stat[scPersonGrowthAverage];
        vmf::Variant salarySum     = stat[scPersonSalarySum];

        if( updateMode != vmf::StatUpdateMode::Disabled )
        {
            ASSERT_EQ( nameCount.getType(), vmf::Variant::type_integer );
            ASSERT_EQ( nameLast.getType(), vmf::Variant::type_string );
            ASSERT_EQ( ageMin.getType(), vmf::Variant::type_integer );
            ASSERT_EQ( ageMax.getType(), vmf::Variant::type_integer );
            ASSERT_EQ( growthAverage.getType(), vmf::Variant::type_real );
            ASSERT_EQ( salarySum.getType(), vmf::Variant::type_integer );

            if( doCompareValues )
            {
                ASSERT_EQ( nameCount.get_integer(), stNameCount );
                ASSERT_EQ( nameLast.get_string(), stNameLast );
                ASSERT_EQ( ageMin.get_integer(), stAgeMin );
                ASSERT_EQ( ageMax.get_integer(), stAgeMax );
                ASSERT_EQ( growthAverage.get_real(), stGrowthAverage );
                ASSERT_EQ( salarySum.get_integer(), stSalarySum );
            }
        }
    }

    std::string fnWorkingPath;
    std::string fnInputName;

    std::string mcSchemaName;
    std::string mcDescName;
    std::string mcPersonName;
    std::string mcAgeName;
    std::string mcGrowthName;
    std::string mcSalaryName;

    std::vector< vmf::FieldDesc > scFieldDesc;
    std::shared_ptr< vmf::MetadataDesc > scMetadataDesc;
    std::shared_ptr< vmf::MetadataSchema > scMetadataSchema;

    std::string scStatName;
    std::string scPersonNameCount;
    std::string scPersonNameLast;
    std::string scPersonAgeMin;
    std::string scPersonAgeMax;
    std::string scPersonGrowthAverage;
    std::string scPersonSalarySum;

    vmf::vmf_integer stNameCount;
    vmf::vmf_string stNameLast;
    vmf::vmf_integer stAgeMin;
    vmf::vmf_integer stAgeMax;
    vmf::vmf_real stGrowthAverage; vmf::vmf_integer stGrowthAverageSum,stGrowthAverageCount;
    vmf::vmf_integer stSalarySum;
    bool stFirstTimeOnce;
};

TEST_P( TestStatistics, Gathering )
{
    vmf::StatUpdateMode::Type updateMode = GetParam();
    unsigned updateTimeout = 100;
    const bool doCompareValues = true;

    std::string fileName = "test_statistics.avi";
    createFile( fileName );

    vmf::MetadataStream stream;
    ASSERT_EQ( stream.open( fileName, vmf::MetadataStream::Update ), true );

    configureSchema( stream );
    configureStatistics( stream );

    vmf::Stat& stat = stream.getStat( scStatName );
    stat.setUpdateTimeout( updateTimeout );
    stat.setUpdateMode( updateMode );
    putMetadata( stream, doCompareValues );
    stat.update( true, true );

    checkStatistics( stat, updateMode, doCompareValues );

    stream.save();
    stream.close();
}

TEST_P( TestStatistics, SaveLoad )
{
    vmf::StatUpdateMode::Type updateMode = GetParam();
    unsigned updateTimeout = 100;
    const bool doCompareValues = true;

    std::string fileName = "test_statistics.avi";
    createFile( fileName );

    vmf::MetadataStream saveStream;
    ASSERT_EQ( saveStream.open( fileName, vmf::MetadataStream::Update ), true );

    configureSchema( saveStream );
    configureStatistics( saveStream );
    putMetadata( saveStream, doCompareValues );

    saveStream.save();
    saveStream.close();

    vmf::MetadataStream loadStream;
    ASSERT_EQ( loadStream.open( fileName, vmf::MetadataStream::ReadOnly ), true );
    ASSERT_EQ( loadStream.load( mcSchemaName ), true );

    vmf::Stat& stat = loadStream.getStat( scStatName );
    stat.setUpdateTimeout( updateTimeout );
    stat.setUpdateMode( updateMode );

    stat.update( true, true );

    checkStatistics( stat, updateMode, doCompareValues );

    loadStream.close();
}

TEST_P( TestStatistics, ExportImportXML )
{
    vmf::StatUpdateMode::Type updateMode = GetParam();
    unsigned updateTimeout = 100;
    const bool doCompareValues = true;

    vmf::MetadataStream saveStream;

    configureSchema( saveStream );
    configureStatistics( saveStream );
    putMetadata( saveStream, doCompareValues );

    vmf::XMLWriter writer;
    std::string data = saveStream.serialize( writer );

    saveStream.close();

    vmf::MetadataStream loadStream;

    vmf::XMLReader reader;
    loadStream.deserialize( data, reader );

    vmf::Stat& stat = loadStream.getStat( scStatName );
    stat.setUpdateTimeout( updateTimeout );
    stat.setUpdateMode( updateMode );

    stat.update( true, true );

    checkStatistics( stat, updateMode, doCompareValues );

    loadStream.close();
}

TEST_P( TestStatistics, ExportImportJSON )
{
    vmf::StatUpdateMode::Type updateMode = GetParam();
    unsigned updateTimeout = 100;
    const bool doCompareValues = true;

    vmf::MetadataStream saveStream;

    configureSchema( saveStream );
    configureStatistics( saveStream );
    putMetadata( saveStream, doCompareValues );

    vmf::JSONWriter writer;
    std::string data = saveStream.serialize( writer );

    saveStream.close();

    vmf::MetadataStream loadStream;
    vmf::JSONReader reader;
    loadStream.deserialize( data, reader );

    vmf::Stat& stat = loadStream.getStat( scStatName );
    stat.setUpdateTimeout( updateTimeout );
    stat.setUpdateMode( updateMode );

    stat.update( true, true );

    checkStatistics( stat, updateMode, doCompareValues );

    loadStream.close();
}

INSTANTIATE_TEST_CASE_P(UnitTest, TestStatistics,
                        ::testing::Values(
                            vmf::StatUpdateMode::Disabled ,vmf::StatUpdateMode::Manual
                            //,vmf::StatUpdateMode::OnAdd, vmf::StatUpdateMode::OnTimer
                            ));

//        std::cout << "nameCount     = " << nameCount.get_integer() << std::endl;
//        std::cout << "nameLast      = " << nameLast.get_string() << std::endl;
//        std::cout << "ageMin        = " << ageMin.get_integer() << std::endl;
//        std::cout << "ageMax        = " << ageMax.get_integer() << std::endl;
//        std::cout << "growthAverage = " << growthAverage.get_real() << std::endl;
//        std::cout << "salarySum     = " << salarySum.get_integer() << std::endl;

