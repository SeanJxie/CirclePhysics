#include <iostream>

#include "circle.h"


const char* WINTT = "Circles";


int main(int argc, char* argv[])
{
    int initSuccess = SDL_Init(SDL_INIT_VIDEO);

    if (initSuccess != 0)
    {
        std::cout << "Init error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Window* hWin = SDL_CreateWindow(WINTT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN);
    if (hWin == NULL)
    {
        std::cout << "Window error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Renderer* hRend = SDL_CreateRenderer(hWin, -1, SDL_RENDERER_ACCELERATED);
    if (hRend == NULL)
    {
        std::cout << "Renderer error: " << SDL_GetError() << std::endl;
        if (hWin != NULL)
        {
            SDL_DestroyWindow(hWin);
        }
        SDL_Quit();
    }

    int WINWT;
    int WINHT;

    SDL_GetRendererOutputSize(hRend, &WINWT, &WINHT);

    std::cout << "Resolution: (" << WINWT << ", " << WINHT << ')' << std::endl;

    SDL_Event event;
    bool quit = false;

    Environment main_env;
    main_env.bot = (float)WINHT;
    main_env.top = 0.0f;
    main_env.left = 0.0f;
    main_env.right = (float)WINWT;
    main_env.global_force = { 0.0f, 1000.0f };

    SDL_Color circleCol = { 255, 255, 255 };
    float circleRad = 40.0f;

    std::vector<PhysicsCircle*> circleVec;
    circleVec.push_back(new PhysicsCircle{ {0, 0}, {0,0}, 150.0f, 0, 1, false, main_env, SDL_Color {255, 0, 0} });

    for (float x = 2.0f * circleRad; x < 500.0f; x+=2.0f * circleRad)
    {
        for (float y = 2.0f * circleRad; y < 500.0f; y += 2.0f * circleRad)
        {
            PhysicsCircle* cPtr = new PhysicsCircle({ x, y }, { x, x }, circleRad, 0.2, 1, true, main_env, circleCol);
            circleVec.push_back(cPtr);
        }
    }

    CircleEngine engine(circleVec);

    Uint32 endTime, startTime = SDL_GetTicks();
    float deltaTime = 0.0f;

    int mouseX, mouseY, lastMouseX = 0, lastMouseY = 0;
    bool runSim = false;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {

            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    quit = true;
                    break;

                case SDLK_SPACE:
                    runSim = true;
                    break;
                }


                break;
            }
        }

       
        SDL_GetMouseState(&mouseX, &mouseY);

        // Instanteneous velocity calculations
        engine.m_circles[0]->m_vel.x = (mouseX - lastMouseX) / deltaTime;
        engine.m_circles[0]->m_vel.y = (mouseY - lastMouseY) / deltaTime;

        engine.m_circles[0]->m_pos.x = mouseX;
        engine.m_circles[0]->m_pos.y = mouseY;

        lastMouseX = mouseX;
        lastMouseY = mouseY;
        
        
        //std::cout << engine.m_circles[1]->m_pos.x << ' ' << engine.m_circles[1]->m_pos.y << std::endl;

        if (runSim)
        {
            engine.phys_update(deltaTime);
        }
   
        SDL_SetRenderDrawColor(hRend, 0, 0, 0, 255);
        SDL_RenderClear(hRend);
         
        engine.render(hRend);

        SDL_RenderPresent(hRend);

        // Manage frame rate
        endTime = SDL_GetTicks();

        deltaTime = (endTime - startTime) / 1000.0f;
        startTime = endTime;
      

        //std::cout << 1 / deltaTime << std::endl; // fps
    }

    SDL_DestroyRenderer(hRend);
    SDL_DestroyWindow(hWin);
    SDL_Quit();

    return EXIT_SUCCESS;
}