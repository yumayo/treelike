#pragma once
#include <treelike/action/finite_time_action.h>
namespace treelike
{
namespace action
{
class fade_in : public finite_time_action
{
public:
    CREATE_H( fade_in, float duration );
    bool init( float duration );
private:
    void setup( ) override;
    void step( float t ) override;
private:
    float _opacity = 0.0F;
};
}
}