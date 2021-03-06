﻿#include <treelike/utility/string.h>
#include <stdarg.h>
#include <time.h>
#include <treelike/utility/scoped_mutex.h>
namespace treelike
{
namespace utility
{
static const int max_string_length = 65536;
static recursion_usable_mutex gloval_app_console_mutex;
std::string format( char const * str, ... )
{
    scoped_mutex console( gloval_app_console_mutex );

    static char buf[max_string_length];
    va_list args;
    va_start( args, str );
    vsnprintf( buf, max_string_length, str, args );
    va_end( args );

    return buf;
}
void log( char const * str, ... )
{
    scoped_mutex console( gloval_app_console_mutex );

    static char buf[max_string_length];
    va_list args;
    va_start( args, str );
    vsnprintf( buf, max_string_length, str, args );
    va_end( args );

    cinder::app::console( ) << buf << std::endl;
}
void log_with_time_stamp( char const * str, ... )
{
    scoped_mutex console( gloval_app_console_mutex );

    static char buf[max_string_length];
    va_list args;
    va_start( args, str );
    vsnprintf( buf, max_string_length, str, args );
    va_end( args );

    cinder::app::console( ) << "[" << cinder::app::getSystemTimeNamed( ) << "]" << buf << std::endl;
}
void log_network( std::string const & ip_address, int const & port, char const * str, ... )
{
    scoped_mutex console( gloval_app_console_mutex );

    static char buf[max_string_length];
    va_list args;
    va_start( args, str );
    vsnprintf( buf, max_string_length, str, args );
    va_end( args );

    cinder::app::console( ) << "[" << cinder::app::getSystemTimeNamed( ) << "] " <<
        ip_address << " : " << port << std::endl << buf << std::endl;
}
void log_data( char const * data, size_t size )
{
    std::string output = ">[ ";
    size_t i;
    for ( i = 0; i < size - 1; ++i )
    {
        if ( ( i % 16 ) == 15 )
        {
            output += format( "%02X", data[i] & 0x000000FF );
            output += " ]\n>[ ";
        }
        else
        {
            if ( ( i % 4 ) == 3 )
            {
                output += format( "%02X | ", data[i] & 0x000000FF );
            }
            else
            {
                output += format( "%02X", data[i] & 0x000000FF );
            }
        }
    }
    output += format( "%02X", data[i] & 0x000000FF );
    log( "%s ]", output.c_str( ) );
}
}
}
namespace cinder
{
namespace app
{
std::string getWritablePath( std::string const& file_name )
{
    #if defined(CINDER_COCOA_TOUCH)
    // iOS:はアプリごとに用意された場所
    return ci::getDocumentsDirectory( ).string( ) + file_name;
    #elif defined (CINDER_MAC)
    #if defined (DEBUG)
    // OSX:Debug時はプロジェクトの場所へ書き出す
    return ci::fs::path( PREPRO_TO_STR( SRCROOT ) ).string( ) + file_name;
    #else
    // OSX:Release時はアプリコンテンツ内
    return ci::app::Platform::get( )->getResourceDirectory( ).string( ) + file_name;
    #endif
    #else
    // Widnows:実行ファイルと同じ場所
    return ci::app::getAppPath( ).string( ) + file_name;
    #endif
}
std::string loadString( std::string const & relative_path )
{
    return static_cast<char*>( loadAsset( relative_path )->getBuffer( )->getData( ) );
}
std::string getSystemTimeNamed( )
{
    // http://blog.livedoor.jp/live_mu/archives/1015582783.html
    std::time_t now = std::time( nullptr );
    struct tm *pnow = std::localtime( &now );
    return treelike::utility::format( "%04d/%02d/%02d %02d:%02d:%02d",
                                      pnow->tm_year + 1900,
                                      pnow->tm_mon + 1,
                                      pnow->tm_mday,
                                      pnow->tm_hour,
                                      pnow->tm_min,
                                      pnow->tm_sec );
}
}
}