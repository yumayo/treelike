﻿#include <treelike/node.h>
#include <cinder/gl/gl.h>
#include <treelike/utility/assert_log.h>
#include <treelike/utility/collision.h>
namespace treelike
{
using namespace cinder;
using namespace treelike::utility;
CREATE_CPP( node )
{
    CREATE( node );
}
node::~node( )
{
    if ( !_name.empty( ) ) log( "Destroy node: [%s]", _name.c_str( ) );
}
bool node::_mouse_began( cinder::app::MouseEvent event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            // 子供がモーダルオブジェクトだった場合
            if ( ( *itr )->_mouse_began( event ) )
            {
                return true;
            }
        }
    if ( _schedule_mouse_event )
    {
        if ( mouse_began( event ) )
        {
            _swallow = true;
            return true;
        }
    }
    return false;
}
bool node::_mouse_moved( cinder::app::MouseEvent event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            if ( ( *itr )->_mouse_moved( event ) )
            {
                return true;
            }
        }
    if ( _schedule_mouse_event )
    {
        if ( _swallow )
        {
            mouse_moved( event );
            return true;
        }
    }
    return false;
}
bool node::_mouse_ended( cinder::app::MouseEvent event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            if ( ( *itr )->_mouse_ended( event ) )
            {
                return true;
            }
        }
    if ( _schedule_mouse_event )
    {
        if ( _swallow )
        {
            mouse_ended( event );
            _swallow = false;
            return true;
        }
    }
    return false;
}
bool node::_touch_began( cinder::app::TouchEvent::Touch event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            // 子供がモーダルオブジェクトだった場合
            if ( ( *itr )->_touch_began( event ) )
            {
                return true;
            }
        }
    if ( _schedule_touch_event )
    {
        if ( touch_began( event ) )
        {
            _swallow = true;
            return true;
        }
    }
    return false;
}
bool node::_touch_moved( cinder::app::TouchEvent::Touch event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            if ( ( *itr )->_touch_moved( event ) )
            {
                return true;
            }
        }
    if ( _schedule_touch_event )
    {
        if ( _swallow )
        {
            touch_moved( event );
            return true;
        }
    }
    return false;
}
bool node::_touch_ended( cinder::app::TouchEvent::Touch event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            if ( ( *itr )->_touch_ended( event ) )
            {
                return true;
            }
        }
    if ( _schedule_touch_event )
    {
        if ( _swallow )
        {
            touch_ended( event );
            _swallow = false;
            return true;
        }
    }
    return false;
}
void node::_touches_began( cinder::app::TouchEvent event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            ( *itr )->_touches_began( event );
        }
    if ( _schedule_touches_event )
    {
        touches_began( event );
    }
}
void node::_touches_moved( cinder::app::TouchEvent event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            ( *itr )->_touches_moved( event );
        }
    if ( _schedule_touches_event )
    {
        touches_moved( event );
    }
}
void node::_touches_ended( cinder::app::TouchEvent event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            ( *itr )->_touches_ended( event );
        }
    if ( _schedule_touches_event )
    {
        touches_ended( event );
    }
}
void node::_key_down( cinder::app::KeyEvent event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            ( *itr )->_key_down( event );
        }
    if ( _schedule_key_event )
    {
        key_down( event );
    }
}
void node::_key_up( cinder::app::KeyEvent event )
{
    if ( !_block_schedule_event )
        for ( auto itr = _children.rbegin( ); itr != _children.rend( ); ++itr )
        {
            ( *itr )->_key_up( event );
        }
    if ( _schedule_key_event )
    {
        key_up( event );
    }
}
void node::_update( float delta )
{
    // 途中でaddがあるため、コンテナをバックアップします。
    std::vector<std::list<hardptr<node>>::iterator> update_objects;
    if ( !_block_schedule_update )
        for ( auto itr = std::begin( _children ); itr != std::end( _children ); ++itr )
        {
            update_objects.emplace_back( itr );
        }

    for ( auto const& c : update_objects )
    {
        ( *c )->_update( delta );
    }
    _action_manager.update( delta );
    if ( _schedule_update ) update( delta );

    for ( auto const& signal : _update_end_signal ) signal( );
    _update_end_signal.clear( );
}
void node::_render( cinder::mat3 model_view_matrix )
{
    if ( !_visible ) return;

    model_view_matrix = translate( model_view_matrix, _position );
    model_view_matrix = scale( model_view_matrix, _scale );
    model_view_matrix = rotate( model_view_matrix, _rotation );
    model_view_matrix = translate( model_view_matrix, -_content_size * _anchor_point );
    auto& m = model_view_matrix;
    // 2次元空間を扱うマトリックスを3次元空間に引き上げる場合、
    // 回転や、スケーリングを行う行列位置は変わりません。
    // しかし、平行移動の部分は2次元であれば3次元目に、3次元であれば4次元目のマトリックス位置に来ます。
    // 2次元空間マトリックスにおいて3次元目は必ず0になるのでm[*][2]は0で決め打ちしています。
    gl::setModelMatrix( mat4( m[0][0], m[0][1], 0, 0,
                              m[1][0], m[1][1], 0, 0,
                              0, 0, 1, 0,
                              m[2][0], m[2][1], 0, 1 ) );

    gl::color( _color );

    if ( utility::hit_window_aabb( model_view_matrix, shared_from_this( ) ) )
    {
        this->render( );
    }

    model_view_matrix = translate( model_view_matrix, _content_size * _pivot );
    for ( auto const& c : _children )
    {
        c->_render( model_view_matrix );
    }
}
bool node::init( )
{
    return true;
}
void node::set_schedule_update( bool value )
{
    _schedule_update = value;
}
bool node::get_schedule_update( ) const
{
    return _schedule_update;
}
void node::set_block_schedule_update( bool value )
{
    _block_schedule_update = value;
}
bool node::get_block_schedule_update( ) const
{
    return _block_schedule_update;
}
void node::set_schedule_mouse_event( bool value )
{
    _schedule_mouse_event = value;
}
bool node::get_schedule_mouse_event( ) const
{
    return _schedule_mouse_event;
}
void node::set_schedule_touch_event( bool value )
{
    _schedule_touch_event = value;
}
bool node::get_schedule_touch_event( ) const
{
    return _schedule_touch_event;
}
void node::set_schedule_touches_event( bool value )
{
    _schedule_touches_event = value;
}
bool node::get_schedule_touches_event( ) const
{
    return _schedule_touches_event;
}
void node::set_schedule_key_event( bool value )
{
    _schedule_key_event = value;
}
bool node::get_schedule_key_event( ) const
{
    return _schedule_key_event;
}
void node::set_schedule_all( bool value )
{
    _schedule_update = value;
    _schedule_mouse_event = value;
    _schedule_touch_event = value;
    _schedule_touches_event = value;
    _schedule_key_event = value;
}
void node::set_block_schedule_event( bool value )
{
    _block_schedule_event = value;
}
bool node::get_block_schedule_event( ) const
{
    return _block_schedule_event;
}
void node::set_position( cinder::vec2 value )
{
    _position = value;
}
cinder::vec2 node::get_position( ) const
{
    return _position;
}
void node::set_scale( cinder::vec2 value )
{
    _scale = value;
}
cinder::vec2 node::get_scale( ) const
{
    return _scale;
}
void node::set_content_size( cinder::vec2 value )
{
    _content_size = value;
}
cinder::vec2 node::get_content_size( ) const
{
    return _content_size;
}
void node::set_rotation( float value )
{
    _rotation = value;
}
float node::get_rotation( ) const
{
    return _rotation;
}
void node::set_anchor_point( cinder::vec2 value )
{
    _anchor_point = value;
}
cinder::vec2 node::get_anchor_point( ) const
{
    return _anchor_point;
}
void node::set_pivot( cinder::vec2 value )
{
    _pivot = value;
}
cinder::vec2 node::get_pivot( ) const
{
    return _pivot;
}
void node::set_color( cinder::ColorA value )
{
    _color = value;
}
cinder::ColorA node::get_color( ) const
{
    return _color;
}
void node::set_opacity( float alpha )
{
    _color.a = alpha;
}
float node::get_opacity( ) const
{
    return _color.a;
}
std::list<hardptr<node>> const& node::get_children( ) const
{
    return _children;
}
void node::set_parent( hardptr<node> const& value )
{
    softptr<node> prev_parent = _parent;
    value->add_child( shared_from_this( ) );

    if ( prev_parent )
    {
        prev_parent->remove_child( shared_from_this( ) );
    }
}
softptr<node> node::get_parent( ) const
{
    return _parent;
}
void node::set_tag( int value )
{
    _tag = value;
}
int node::get_tag( ) const
{
    return _tag;
}
void node::set_order( int value )
{
    _order = value;
    _children.sort( [ ] ( hardptr<node>& a, hardptr<node>& b )
    {
        return a->_order < b->_order;
    } );
}
int node::get_order( ) const
{
    return _order;
}
void node::set_name( std::string const& value )
{
    _name = value;

    std::hash<std::string> h;
    _hash = h( value );
}
std::string const& node::get_name( ) const
{
    return _name;
}
void node::set_running( bool value )
{
    _running = value;
}
bool node::get_running( ) const
{
    return _running;
}
void node::set_visible( bool value )
{
    _visible = value;
}
bool node::get_visible( ) const
{
    return _visible;
}
softptr<node> node::get_child_by_name( std::string const & name ) const
{
    assert_log( !name.empty( ), "無効な名前です。", return nullptr );

    std::hash<std::string> h;
    size_t hash = h( name );

    auto itr = std::find_if( std::cbegin( _children ), std::cend( _children ), [ this, hash, name ] ( hardptr<node> const& child )
    {
        return child->_hash == hash && child->_name.compare( name ) == 0;
    } );

    if ( itr != std::cend( _children ) )
    {
        return *itr;
    }
    return nullptr;
}
softptr<node> node::get_child_by_tag( int tag ) const
{
    assert_log( tag != node::INVALID_TAG, "無効なタグです。", return nullptr );

    auto itr = std::find_if( std::cbegin( _children ), std::cend( _children ), [ this, tag ] ( hardptr<node> const& n )
    {
        return n->_tag == tag;
    } );

    if ( itr != std::cend( _children ) )
    {
        return *itr;
    }
    return nullptr;
}
void node::remove_child( hardptr<node> const& child )
{
    if ( _children.empty( ) ) return;

    auto erase = std::remove_if( std::begin( _children ), std::end( _children ), [ this, child ] ( hardptr<node>& n )
    {
        return n == child;
    } );

    _children.erase( erase, std::end( _children ) );
}
void node::remove_child_by_name( std::string const & name )
{
    assert_log( !name.empty( ), "無効な名前です。", return );

    auto child = this->get_child_by_name( name );

    if ( child )
    {
        this->remove_child( child );
    }
    else
    {
        log( "remove_child_by_name(name = %s): 子供が見つかりませんでした。", name.c_str( ) );
    }
}
void node::remove_child_by_tag( int tag )
{
    assert_log( tag != node::INVALID_TAG, "無効なタグです。", return );

    auto child = this->get_child_by_tag( tag );

    if ( child )
    {
        this->remove_child( child );
    }
    else
    {
        log( "remove_child_by_tag(tag = %d): 子供が見つかりませんでした。", tag );
    }
}
void node::remove_all_children( )
{
    _update_end_signal.emplace_back( [ this ]
    {
        _children.clear( );
    } );
}
void node::remove_from_parent( )
{
    if ( !_own_removing )
    {
        _own_removing = true;
        if ( _parent )
        {
            _parent->_update_end_signal.emplace_back( [ this ]
            {
                _parent->remove_child( shared_from_this( ) );
            } );
        }
    }
}
softptr<node> node::get_root( )
{
    return _get_root( );
}

softptr<node> node::_get_root( )
{
    if ( _parent )
    {
        return _parent->_get_root( );
    }
    else
    {
        return shared_from_this( );
    }
}

void node::run_action( hardptr<action::action> const & action )
{
    _action_manager.add_action( action, shared_from_this( ), !_running );
    action->setup( );
}

softptr<action::action> node::get_action_by_name( std::string const & name ) const
{
    return _action_manager.get_action_by_name( name );
}

softptr<action::action> node::get_action_by_tag( int tag ) const
{
    return _action_manager.get_action_by_tag( tag );
}

void node::remove_all_actions( )
{
    _action_manager.remove_all_actions( );
}

void node::remove_action( hardptr<action::action> const & action )
{
    _action_manager.remove_action( action );
}

void node::remove_action_by_tag( int tag )
{
    _action_manager.remove_action_by_tag( tag );
}

void node::remove_action_by_name( std::string const & name )
{
    _action_manager.remove_action_by_name( name );
}

bool node::is_running_action( ) const
{
    return _action_manager.is_running( );
}

cinder::mat3 node::get_world_matrix( ) const
{
    std::vector<mat3> mats;
    auto p = _parent;
    while ( p )
    {
        auto m = translate( mat3( ), p->_position );
        m = scale( m, p->_scale );
        m = rotate( m, p->_rotation );
        m = translate( m, -p->_content_size * p->_anchor_point );
        m = translate( m, p->_content_size * p->_pivot );
        mats.emplace_back( std::move( m ) );
        p = p->_parent;
    }

    mat3 result;
    for ( auto itr = mats.rbegin( ); itr != mats.rend( ); ++itr )
    {
        result *= *itr;
    }

    return result;
}
}