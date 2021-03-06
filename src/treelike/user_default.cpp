﻿#include <treelike/user_default.h>
#include <treelike/utility/assert_log.h>
#include <cinder/app/App.h>
#include <cinder/ImageIo.h>
#include <fstream>
#include <boost/filesystem.hpp>
using namespace cinder;
namespace treelike
{
user_default* user_default::_instans = nullptr;
Json::Value & user_default::get_root( )
{
    return _root;
}
void user_default::save( )
{
    Json::StyledWriter writer;
    auto data = writer.write( _root );
    auto dataRef = writeFile( _target_full_path );
    dataRef->getStream( )->writeData( data.c_str( ), data.size( ) );
}
user_default* user_default::get_instans( )
{
    if ( !_instans ) _instans = new user_default;
    return _instans;
}
void user_default::remove_instans( )
{
    get_instans( )->save( );
    delete _instans;
    _instans = nullptr;
}
user_default::user_default( )
{
    _target_full_path = app::getAssetDirectories( ).front( ).string( ) + "\\" + _target_file_name;

    if ( !boost::filesystem::exists( _target_full_path ) )
    {
        std::ofstream output( _target_full_path );
        Json::Value root;
        output << Json::StyledWriter( ).write( root );
    }

    std::stringstream buffer;
    buffer << std::ifstream( _target_full_path ).rdbuf( ) << std::flush;

    Json::Reader reader;
    assert_log( reader.parse( buffer, _root ), "無効なJsonファイルです。", return );
}

}