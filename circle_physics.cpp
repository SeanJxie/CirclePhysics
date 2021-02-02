#include "circle_physics.h"


void draw_circle(SDL_Renderer* renderer, float x, float y, float r, int nPoints)
{
    if (nPoints < 0) nPoints = 0;

    std::vector<SDL_FPoint> points;

    for (float theta = 0; theta < 2 * (float) M_PI; theta += 2 * (float) M_PI / nPoints)
    {
        points.push_back({r * cosf(theta) + x, r * sinf(theta) + y });
    }

    points.push_back({ r * cosf(0) + x, r * sinf(0) + y }); // Connect last point to first

    SDL_RenderDrawLinesF(renderer, &points[0], (int) points.size()); 
}


Vec2d add_v(Vec2d a, Vec2d b)
{
    return Vec2d{ a.x + b.x, a.y + b.y };
}


Vec2d sub_v(Vec2d a, Vec2d b)
{
    return Vec2d{ a.x - b.x, a.y - b.y };
}


Vec2d mul_s(float a, Vec2d b)
{
    return Vec2d{ a * b.x, a * b.y };
}

float mag_sqr(Vec2d a)
{
    return a.x * a.x + a.y * a.y;
}


float dot(Vec2d a, Vec2d b)
{
    return a.x * b.x + a.y * b.y;
}


PhysicsCircle::PhysicsCircle(Vec2d init_p_vec, Vec2d init_v_vec, float radius, float bounciness, float mass, bool global, bool isStatic, Environment e, SDL_Color rgb)
{
    m_pos = init_p_vec;
    m_vel = init_v_vec;
    m_radius = radius;
    m_bounciness = bounciness;
    m_mass = mass;
    m_global = global;
    m_env = e;
    m_col = rgb;

    if (m_global)
    {
        m_force = m_env.global_force;
    }

    m_isStatic = isStatic;
}


void PhysicsCircle::update(float dt)
{
    // --- Boundary collision ---
    if (m_pos.x - m_radius <= m_env.left)
    {
        m_pos.x = m_env.left + m_radius;
        m_vel.x *= -m_bounciness;
    }
    else if (m_pos.x + m_radius >= m_env.right)
    {
        m_pos.x = m_env.right - m_radius;
        m_vel.x *= -m_bounciness;
    }
    if (m_pos.y - m_radius <= m_env.top)
    {
        m_pos.y = m_env.top + m_radius;
        m_vel.y *= -m_bounciness;
    }
    else if (m_pos.y + m_radius >= m_env.bot)
    {
        m_pos.y = m_env.bot - m_radius;
        m_vel.y *= -m_bounciness;
    }
    
    if (!m_isStatic)  // Simply don't adhere to the laws of physics
    {
        // Apply force
        m_acc.x += m_force.x / m_mass * dt;
        m_acc.y += m_force.y / m_mass * dt;
        
        // Accelerate
        m_vel.x += m_acc.x * dt;
        m_vel.y += m_acc.y * dt;

        // --- Move ---
        m_pos.x += m_vel.x * dt;
        m_pos.y += m_vel.y * dt;
    }
    
}


void PhysicsCircle::render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, m_col.r, m_col.g, m_col.b, m_col.a);
    draw_circle(renderer, m_pos.x, m_pos.y, m_radius, CIRCLE_RESOLUTION);
}


CircleEngine::CircleEngine(std::vector<PhysicsCircle*> circles)
{
    m_circles = circles;
}


bool CircleEngine::_check_collide(PhysicsCircle c1, PhysicsCircle c2)
{
    return (sqrtf((c2.m_pos.x - c1.m_pos.x) * (c2.m_pos.x - c1.m_pos.x) + (c2.m_pos.y - c1.m_pos.y) * (c2.m_pos.y - c1.m_pos.y)) < c1.m_radius + c2.m_radius);
}


void CircleEngine::_resolve_static(PhysicsCircle* c1, PhysicsCircle* c2)
{
    float fDist = sqrtf((c2->m_pos.x - c1->m_pos.x) * (c2->m_pos.x - c1->m_pos.x) + (c2->m_pos.y - c1->m_pos.y) * (c2->m_pos.y - c1->m_pos.y));
    float fOverlap = 0.5f * (fDist - c1->m_radius - c2->m_radius);
   
    if (!c1->m_isStatic)
    {
        c1->m_pos.x -= fOverlap * (c1->m_pos.x - c2->m_pos.x) / fDist;
        c1->m_pos.y -= fOverlap * (c1->m_pos.y - c2->m_pos.y) / fDist;
    }

    if (!c2->m_isStatic)
    {
        c2->m_pos.x += fOverlap * (c1->m_pos.x - c2->m_pos.x) / fDist;
        c2->m_pos.y += fOverlap * (c1->m_pos.y - c2->m_pos.y) / fDist;
    }
   
}


void CircleEngine::_resolve_dynamic_simple(PhysicsCircle* c1, PhysicsCircle* c2)
{
    float tempVel;
    tempVel = c1->m_vel.x;
    c1->m_vel.x = c2->m_vel.x;
    c2->m_vel.x = tempVel;
  
    tempVel = c1->m_vel.y;
    c1->m_vel.y = c2->m_vel.y;
    c2->m_vel.y = tempVel;
}
 


void CircleEngine::_resolve_dynamic_elastic(PhysicsCircle* c1, PhysicsCircle* c2)
{
    // https://en.wikipedia.org/wiki/Elastic_collision
    // Angle-free representation

    Vec2d c1VelUnchanged = c1->m_vel; // This bug took me like 2 days to find

    Vec2d diff_factor = sub_v(c1->m_pos, c2->m_pos);
    float mass_factor = 2.0f * c2->m_mass / (c1->m_mass + c2->m_mass);
    float scalar_factor =  dot(sub_v(c1->m_vel, c2->m_vel), sub_v(c1->m_pos, c2->m_pos)) / mag_sqr(sub_v(c1->m_pos, c2->m_pos));
    
    c1->m_vel = sub_v(c1->m_vel, mul_s(mass_factor * scalar_factor, diff_factor));

    diff_factor = sub_v(c2->m_pos, c1->m_pos);
    mass_factor = 2.0f * c1->m_mass / (c1->m_mass + c2->m_mass);
    scalar_factor = dot(sub_v(c2->m_vel, c1VelUnchanged), sub_v(c2->m_pos, c1->m_pos)) / mag_sqr(sub_v(c2->m_pos, c1->m_pos));
    
    c2->m_vel = sub_v(c2->m_vel, mul_s(mass_factor * scalar_factor, diff_factor));
}                              



void CircleEngine::phys_update(float dt)
{
    std::vector<std::pair<PhysicsCircle*, PhysicsCircle*>> collidingPairs;
    for (int i = 0; i < m_circles.size(); i++)
    {
        for (int j = 0; j < m_circles.size(); j++)
        {
            if (i != j)
            {
                if (this->_check_collide(*m_circles[i], *m_circles[j]))
                {
                    this->_resolve_static(m_circles[i], m_circles[j]);
                    collidingPairs.push_back({ m_circles[i], m_circles[j] });
                }
            }
        }
    }

    for (auto p : collidingPairs)
    {
        this->_resolve_dynamic_elastic(p.second, p.first);
    }

    for (auto c : this->m_circles)
    {
        c->update(dt);
    }

}


void CircleEngine::render(SDL_Renderer* renderer)
{
    for (auto c : this->m_circles)
    {
        c->render(renderer);
    }
}
