#ifndef __VMF_LOGGER_H__
#define __VMF_LOGGER_H__

#include <iostream>
#include <mutex>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace vmf
{

template< class T >
class Logger
{
public:
    explicit Logger( std::ostream& os )
        : m_stream( os )
        {}
    virtual ~Logger()
        {}
    void writeln( const std::string& text )
        {
            std::unique_lock< std::mutex > lock( m_lock );
            std::cout << text << std::endl;
        }
    void writeln( const std::string& text, const T& val )
        {
            std::unique_lock< std::mutex > lock( m_lock );
            std::cout << text << val << std::endl;
        }
private:
    std::ostream& m_stream;
    std::mutex m_lock;
};

template< class T >
Logger< T >& getLogger()
{
    static Logger< T > logger( std::cout );
    return logger;
}

} // namespace vmf

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //__VMF_LOGGER_H__

