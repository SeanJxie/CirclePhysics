#ifndef CIRCLE_H
#define CIRCLE_H

#include <SDL.h>
#include <vector>
#include <iostream>

/*

A 2D circles physics engine inspired by javidx9's tutorial: https://www.youtube.com/watch?v=LPzyNOHY3A4

*/


const int CIRCLE_RESOLUTION = 5;
const float MIN_VELOCITY = 0.0f; // Uh... I'll use this soon


void draw_circle(SDL_Renderer* renderer, float x, float y, float r, int nPoints);


struct Vec2d
{
    float x, y;
};

// Vector operations
Vec2d add_v(Vec2d a, Vec2d b);
Vec2d sub_v(Vec2d a, Vec2d b);
Vec2d mul_s(float a, Vec2d b);
float mag_sqr(Vec2d a);
float dot(Vec2d a, Vec2d b);


struct Environment
{
    Vec2d global_force;
    float friction;
    float top, bot, left, right;
};



class PhysicsCircle
{
public:
    PhysicsCircle(Vec2d init_p_vec, Vec2d init_v_vec, float radius, float bounciness, float mass, bool global, Environment e, SDL_Color rgb);
    void update(float dt);
    void render(SDL_Renderer* renderer);

    SDL_Color m_col = { 0, 0, 0, 0 };

    Environment m_env = { 0, 0, 0, 0, 0, 0 };

    float m_radius = 0;
    float m_mass = 0;
    float m_bounciness = 0;

    Vec2d m_pos = { 0, 0 };
    Vec2d m_vel = { 0, 0 };
    Vec2d m_acc = { 0, 0 };
    Vec2d m_force = { 0, 0 };

    bool m_global = true;
};




class CircleEngine
{
public:
    CircleEngine(std::vector<PhysicsCircle*> circles);
    void phys_update(float dt);
    void render(SDL_Renderer* renderer);

    std::vector<PhysicsCircle*> m_circles;

private:
    static bool _check_collide(PhysicsCircle c1, PhysicsCircle c2);
    static void _resolve_static(PhysicsCircle* c1, PhysicsCircle* c2);
    static void _resolve_dynamic_simple(PhysicsCircle* c1, PhysicsCircle* c2);
    static void _resolve_dynamic_elastic(PhysicsCircle* c1, PhysicsCircle* c2);
};


#endif