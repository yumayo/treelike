﻿#include <treelike/network/tcp_client.h>
#include <treelike/network/tcp_client_member.h>
namespace treelike
{
namespace network
{
CREATE_CPP( tcp_client, std::string const& ip_address, int port )
{
    CREATE( tcp_client, ip_address, port );
}
bool tcp_client::init( std::string const& ip_address, int port )
{
    _m.reset( );
    _m = std::make_shared<_member>( *this, ip_address, port );

    set_schedule_update( );

    _m->connect( );

    return true;
}
tcp_client::~tcp_client( )
{
    _m->close( );
    _m->io.stop( );
}
void tcp_client::update( float delta )
{
    _m->io.reset( );
    _m->io.poll( );
    _m->update( );
}
void tcp_client::write( std::string const & message, std::function<void( )> on_send )
{
    write( message.c_str( ), message.size( ), on_send );
}
void tcp_client::write( char const * message, size_t size, std::function<void( )> on_send )
{
    _m->write( message, size, on_send );
}
int tcp_client::get_port( )
{
    return _m->get_port( );
}
void tcp_client::on( std::string const& name, std::function<void( Json::Value root )> const& func )
{
    on_received_named_json.insert( std::make_pair( name, func ) );
}
}
}