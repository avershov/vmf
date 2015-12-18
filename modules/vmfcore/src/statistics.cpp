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
//#include <cstring>
//#include <string>
//#include <memory>
//#include <sstream>
//#include <cmath>
//#include <limits>
//#include <iomanip>


namespace vmf
{

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

} // namespace vmf
