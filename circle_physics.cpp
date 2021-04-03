#include "circle_physics.h"




void _draw_line_bresenham(SDL_Renderer* h, int sx, int sy, int ex, int ey)
{
    /*
    SDL_RenderSetLogicalSize seems to not scale SDL_RenderDrawLine.
    Here's a custom line function.
    */
    // bresenham line
    int x1 = sx, y1 = sy, x2 = ex, y2 = ey;
    int steep = abs(y2 - y1) > abs(x2 - x1), inc = -1;

    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    if (y1 < y2) {
        inc = 1;
    }

    int dx = abs(x2 - x1),
        dy = abs(y2 - y1),
        y = y1, x = x1,
        e = 0;

    for (x; x <= x2; x++) {
        if (steep) {
            SDL_RenderDrawPoint(h, y, x);
        }
        else {
            SDL_RenderDrawPoint(h, x, y);
        }

        if ((e + dy) << 1 < dx) {
            e = e + dy;
        }
        else {
            y += inc;
            e = e + dy - dx;
        }
    }
}


void _draw_lines_bresenham(SDL_Renderer* h, SDL_FPoint* points, int nPoints)
{
    
    for (int i = 0; i < nPoints - 1; i++)
    {
        _draw_line_bresenham(h, points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
    }
}




void draw_circle_arr(SDL_Renderer* renderer, float x, float y, float r, int nPoints)
{
    if (nPoints <= 2) return;

    SDL_FPoint* pArr = (SDL_FPoint*)malloc((nPoints + 1ull) * sizeof(SDL_FPoint)); // +1 for the "end to start" connection 
    int nIdx = 0;

    const float F_DOUBLEPI = 2.0f * (float)M_PI;

    if (pArr != NULL)
    {
        for (float theta = 0.0f; theta < F_DOUBLEPI; theta += F_DOUBLEPI / nPoints)
        {
            pArr[nIdx] = { r * cosf(theta) + x, r * sinf(theta) + y };
            nIdx++;
        }

        pArr[nIdx] = { r * cosf(0.0f) + x, r * sinf(0.0f) + y }; // "end to start" connection 
        _draw_lines_bresenham(renderer, pArr, nPoints + 1); // +1 for the "end to start" connection 
    }

    else return;
}

/*
Array version is faster

void draw_circle_vec(SDL_Renderer* renderer, float x, float y, float r, int nPoints)
{
    if (nPoints < 0) nPoints = 0;

    std::vector<SDL_FPoint> points;

    for (float theta = 0; theta < 2 * (float)M_PI; theta += 2 * (float)M_PI / nPoints)
    {
        points.push_back({ r * cosf(theta) + x, r * sinf(theta) + y });
    }

    points.push_back({ r * cosf(0) + x, r * sinf(0) + y }); // Connect last point to first

    SDL_RenderDrawLinesF(renderer, &points[0], (int)points.size());
}
*/

v2d add_v(v2d a, v2d b)
{
    return v2d{ a.x + b.x, a.y + b.y };
}


v2d sub_v(v2d a, v2d b)
{
    return v2d{ a.x - b.x, a.y - b.y };
}


v2d mul_s(float a, v2d b)
{
    return v2d{ a * b.x, a * b.y };
}


float mag_sqr(v2d a)
{
    return a.x * a.x + a.y * a.y;
}


float dot(v2d a, v2d b)
{
    return a.x * b.x + a.y * b.y;
}


PhysicsCircle::PhysicsCircle(v2d init_p_vec, v2d init_v_vec, float radius, float bounciness, float mass, bool global, bool isStatic, Environment e, SDL_Color rgba)
{
    this->m_pos = init_p_vec;
    this->m_vel = init_v_vec;
    this->m_radius = radius;
    this->m_bounciness = bounciness;
    this->m_mass = mass;
    this->m_global = global;
    this->m_env = e;
    this->m_rgba = rgba;

    if (this->m_global)
    {
        this->m_force = this->m_env.global_force;
    }

    this->m_isStatic = isStatic;
}


void PhysicsCircle::update(float dt)
{    
    // Remove when StaticEdge has been implemented
    if (this->m_pos.x - this->m_radius <= this->m_env.left)
    {
        this->m_pos.x = this->m_env.left + this->m_radius;
        this->m_vel.x *= -this->m_bounciness;
    }
    else if (this->m_pos.x + this->m_radius >= this->m_env.right)
    {
        this->m_pos.x = this->m_env.right - this->m_radius;
        this->m_vel.x *= -this->m_bounciness;
    }
    if (this->m_pos.y - this->m_radius <= this->m_env.top)
    {
        this->m_pos.y = this->m_env.top + this->m_radius;
        this->m_vel.y *= -this->m_bounciness;
    }
    else if (this->m_pos.y + this->m_radius >= this->m_env.bot)
    {
        this->m_pos.y = this->m_env.bot - this->m_radius;
        this->m_vel.y *= -this->m_bounciness;
    }

    if (!this->m_isStatic)  // Simply don't adhere to the laws of physics
    {
        // Apply force
        this->m_acc.x += this->m_force.x / this->m_mass * dt;
        this->m_acc.y += this->m_force.y / this->m_mass * dt;
        
        // Accelerate
        this->m_vel.x += this->m_acc.x * dt;
        this->m_vel.y += this->m_acc.y * dt;

        // --- Move ---
        this->m_pos.x += this->m_vel.x * dt;
        this->m_pos.y += this->m_vel.y * dt;
    }
}


void PhysicsCircle::render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, this->m_rgba.r, this->m_rgba.g, this->m_rgba.b, this->m_rgba.a);
    draw_circle_arr(renderer, this->m_pos.x, this->m_pos.y, this->m_radius, CIRCLE_RESOLUTION);
}


void PhysicsCircle::render_point(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, this->m_rgba.r, this->m_rgba.g, this->m_rgba.b, this->m_rgba.a);
    SDL_RenderDrawPointF(renderer, this->m_pos.x, this->m_pos.y);
}


bool PhysicsCircle::operator!=(PhysicsCircle c)
{
    return this->m_pos.x != c.m_pos.x || this->m_pos.y != c.m_pos.y;
}


StaticEdge::StaticEdge(v2d start, v2d end, SDL_Color rgba)
{
    this->m_start = start;
    this->m_end = end;
    this->m_rgba = rgba;
}


void StaticEdge::render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, this->m_rgba.r, this->m_rgba.g, this->m_rgba.b, this->m_rgba.a);
    SDL_RenderDrawLineF(renderer, this->m_start.x, this->m_start.y, this->m_end.x, this->m_end.y);
}



CircleEngine::CircleEngine(std::vector<PhysicsCircle*> circles, std::vector<StaticEdge*> edges)
{
    this->m_circles = circles;
    this->m_edges = edges;
}


bool CircleEngine::_cc_check_collide(PhysicsCircle c1, PhysicsCircle c2)
{
    return (sqrtf((c2.m_pos.x - c1.m_pos.x) * (c2.m_pos.x - c1.m_pos.x) + (c2.m_pos.y - c1.m_pos.y) * (c2.m_pos.y - c1.m_pos.y)) <= c1.m_radius + c2.m_radius);
}


void CircleEngine::_cc_resolve_static(PhysicsCircle* c1, PhysicsCircle* c2)
{
    float fDist = sqrtf((c2->m_pos.x - c1->m_pos.x) * (c2->m_pos.x - c1->m_pos.x) + (c2->m_pos.y - c1->m_pos.y) * (c2->m_pos.y - c1->m_pos.y));
    float fOverlap = 0.5f * (fDist - c1->m_radius - c2->m_radius);
   
    float fRadSum = c1->m_radius + c2->m_radius; // The maximum distance circles should be displaced.

    float xDisplace = fOverlap * (c1->m_pos.x - c2->m_pos.x) / fDist;
    float yDisplace = fOverlap * (c1->m_pos.y - c2->m_pos.y) / fDist;

    
    if (fabs(xDisplace) > fRadSum)
    {
        xDisplace = fOverlap * (c1->m_pos.x - c2->m_pos.x) / fRadSum;
    }

    if (fabs(yDisplace) > fRadSum)
    {
        yDisplace = fOverlap * (c1->m_pos.y - c2->m_pos.y) / fRadSum;
    }
    

    if (!c1->m_isStatic)
    {
        c1->m_pos.x -= xDisplace;
        c1->m_pos.y -= yDisplace;
    }                           
                                
    if (!c2->m_isStatic)        
    {                           
        c2->m_pos.x += xDisplace;
        c2->m_pos.y += yDisplace;
    }
   
}


void CircleEngine::_cc_resolve_dynamic_simple(PhysicsCircle* c1, PhysicsCircle* c2)
{
    float tempVel;
    tempVel = c1->m_vel.x;
    c1->m_vel.x = c2->m_vel.x;
    c2->m_vel.x = tempVel;
  
    tempVel = c1->m_vel.y;
    c1->m_vel.y = c2->m_vel.y;
    c2->m_vel.y = tempVel;
}
 

void CircleEngine::_cc_resolve_dynamic_elastic(PhysicsCircle* c1, PhysicsCircle* c2)
{
    // https://en.wikipedia.org/wiki/Elastic_collision
    // Angle-free representation

    v2d c1VelUnchanged = c1->m_vel;

    v2d diff_factor = sub_v(c1->m_pos, c2->m_pos);
    float mass_factor = 2.0f * c2->m_mass / (c1->m_mass + c2->m_mass);
    float scalar_factor =  dot(sub_v(c1->m_vel, c2->m_vel), sub_v(c1->m_pos, c2->m_pos)) / mag_sqr(sub_v(c1->m_pos, c2->m_pos));
    
    c1->m_vel = sub_v(c1->m_vel, mul_s(mass_factor * scalar_factor, diff_factor));

    diff_factor = sub_v(c2->m_pos, c1->m_pos);
    mass_factor = 2.0f * c1->m_mass / (c1->m_mass + c2->m_mass);
    scalar_factor = dot(sub_v(c2->m_vel, c1VelUnchanged), sub_v(c2->m_pos, c1->m_pos)) / mag_sqr(sub_v(c2->m_pos, c1->m_pos));
    
    c2->m_vel = sub_v(c2->m_vel, mul_s(mass_factor * scalar_factor, diff_factor));
}                              


bool CircleEngine::_ce_check_collide(PhysicsCircle* c, StaticEdge e)
{
    /*
    Naive version -> Uses analytical geometry
    */
    

    // Line
    float m = (e.m_start.y - e.m_end.y) / (e.m_start.x - e.m_end.x);
    float b = e.m_start.y - m * e.m_start.x;

    // Circle
    float p = c->m_pos.x;
    float q = c->m_pos.y;
    float r = c->m_radius;

    // Quadratic
    float A = m * m + 1.0f;
    float B = -2.0f * (p - b * m + m * q);
    float C = p * p - 2.0f * b * q + b * b + q * q - r * r;

    float D = B * B - 4.0f * A * C;

    if (D >= 0.0f)
    {
        if (p - r <= MAX(e.m_start.x, e.m_end.x) && p + r >= MIN(e.m_start.x, e.m_end.x)) // Check domain of collision
        {
            float xSol1 = (-B + sqrtf(B * B - 4.0f * A * C)) / (2.0f * A);
            float ySol1 = m * xSol1 + b;
            float xSol2 = (-B - sqrtf(B * B - 4.0f * A * C)) / (2.0f * A);
            float ySol2 = m * xSol1 + b;
            c->m_vel.y *= -1.0f;
            return true;
        }
    }

    return false;
}



void CircleEngine::phys_update(float dt)
{
    // Circle circle 
    std::vector<std::pair<PhysicsCircle*, PhysicsCircle*>> ccCollidingPairs;
    for (int i = 0; i < this->m_circles.size(); i++)
    {
        for (int j = 0; j < this->m_circles.size(); j++)
        {
            if (i != j)
            {
                if (this->_cc_check_collide(*this->m_circles[i], *this->m_circles[j]))
                {
                    this->_cc_resolve_static(this->m_circles[i], this->m_circles[j]);
                    ccCollidingPairs.push_back({ this->m_circles[i], this->m_circles[j] });
                }
            }
        }
    }

    for (int i = 0; i < this->m_circles.size(); i++)
    {
        for (int j = 0; j < this->m_edges.size(); j++)
        {
            this->_ce_check_collide(this->m_circles[i], *this->m_edges[j]);
        }
    }

   

    for (auto p : ccCollidingPairs)
    {
        this->_cc_resolve_dynamic_elastic(p.first, p.second);
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

    for (auto e : this->m_edges)
    {
        e->render(renderer);
    }
}



