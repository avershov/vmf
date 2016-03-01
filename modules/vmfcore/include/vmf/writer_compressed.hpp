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

/*!
* \file writer_compressed.hpp
* \brief Serialization based on instances of Compressor and IWriter classes
*/

#pragma once

#ifndef VMF_WRITER_COMPRESSED_HPP
#define VMF_WRITER_COMPRESSED_HPP

#include "iwriter.hpp"

namespace vmf
{

/*!
 * \class WriterCompressed
 * \brief WriterCompressed class performs serialization with compression based on provided instances
 * of Compressor and IWriter
 */
class VMF_EXPORT WriterCompressed : public IWriter
{
public:
    /*!
     * \brief Constructor taking compressor and writer to be used at data writing
     * \param _writer Shared pointer to instance of IWriter
     * \param _compressorId Shared pointer to instance of Compressor
     */
    WriterCompressed(std::shared_ptr<IWriter> _writer, std::string _compressorId) :
    IWriter(), compressorId(_compressorId), writer(_writer)
    { }
    
    /*!
     * \brief Class destructor
     */
    virtual ~WriterCompressed()
    { }
    
    // IWriter implementation
    virtual std::string store(const std::vector<std::shared_ptr<MetadataSchema>>& schemas);
    virtual std::string store(const MetadataSet& set);
    virtual std::string store(const IdType& nextId,
                              const std::string& filepath,
                              const std::string& checksum,
                              const std::vector<std::shared_ptr<MetadataStream::VideoSegment>>& segments,
                              const std::vector<std::shared_ptr<MetadataSchema>>& schemas,
                              const MetadataSet& set,
                              const std::vector< Stat >& stats);
    virtual std::string store(const std::shared_ptr<MetadataStream::VideoSegment>& spSegment);
    virtual std::string store(const std::vector<std::shared_ptr<MetadataStream::VideoSegment>>& segments);
    virtual std::string store(const Stat& stat);
    virtual std::string store(const std::vector<Stat>& stats);


protected:
    /*!
     * \brief Performs compression of text data
     * \param input Input string
     * \return Compressed string
     */
    virtual std::string compress(const std::string& input);

    std::string compressorId;
    std::shared_ptr<IWriter> writer;
};


}//vmf

#endif /* VMF_WRITER_COMPRESSED_HPP */
