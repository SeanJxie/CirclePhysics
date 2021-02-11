#ifndef CIRCLE_PHYSICS_H
#define CIRCLE_PHYSICS_H

#include <SDL.h>
#include <vector>
#include <iostream>

/*

A 2D circles physics engine inspired by javidx9's tutorial: https://www.youtube.com/watch?v=LPzyNOHY3A4

*/

#define MAX(A, B) ((A > B) ? A : B)
#define MIN(A, B) ((A < B) ? A : B)


const int CIRCLE_RESOLUTION = 10;
const float MIN_VELOCITY = 0.0f; // Uh... I'll use this soon


void draw_circle_arr(SDL_Renderer* renderer, float x, float y, float r, int nPoints);

struct v2d
{
    float x, y;
};


// Vector operations
v2d add_v(v2d a, v2d b);
v2d sub_v(v2d a, v2d b);
v2d mul_s(float a, v2d b);
float mag_sqr(v2d a);
float dot(v2d a, v2d b);


struct Environment
{
    v2d global_force;
    float friction; // Not physically accurate yet. Not used.
    float top, bot, left, right;
};


class PhysicsCircle
{
public:
    PhysicsCircle(v2d init_p_vec, v2d init_v_vec, float radius, float bounciness, float mass, bool global, bool isStatic, Environment e, SDL_Color rgba);
    void update(float dt);
    void render(SDL_Renderer* renderer);
    void render_point(SDL_Renderer* renderer);

    bool operator !=(PhysicsCircle c);

    SDL_Color m_rgba;

    Environment m_env;

    float m_radius;
    float m_mass;
    float m_bounciness;

    v2d m_acc   = { 0.0f, 0.0f };
    v2d m_pos   = { 0.0f, 0.0f };
    v2d m_vel   = { 0.0f, 0.0f };
    v2d m_force = { 0.0f, 0.0f };
     
    bool m_global = true;
    bool m_isStatic = false;
};


class StaticEdge  // It's static, so no physics
{
public:
    StaticEdge(v2d start, v2d end, SDL_Color rgba);
    void render(SDL_Renderer* renderer);

    v2d m_start;
    v2d m_end;
    SDL_Color m_rgba;
};


class CircleEngine
{
public:
    CircleEngine(std::vector<PhysicsCircle*> circles, std::vector<StaticEdge*> edges);
    void phys_update(float dt);
    void render(SDL_Renderer* renderer);

    std::vector<PhysicsCircle*> m_circles;
    std::vector<StaticEdge*> m_edges;

private:
    static bool _cc_check_collide(PhysicsCircle c1, PhysicsCircle c2);
    static void _cc_resolve_static(PhysicsCircle* c1, PhysicsCircle* c2);
    static void _cc_resolve_dynamic_simple(PhysicsCircle* c1, PhysicsCircle* c2);
    static void _cc_resolve_dynamic_elastic(PhysicsCircle* c1, PhysicsCircle* c2);
    bool _ce_check_collide(PhysicsCircle* c, StaticEdge e);
};



#endif