#include <iostream>
#include <string>

#include "circle_physics.h"


const char* WINTT = "Circles";



int main(int argc, char* argv[])
{
    const int inputWinWt = 1000, inputWinHt = 1000;
    const int logicalWt = 100, logicalHt = 100;
    /*
    std::cout << "Enter resolution:" << std::endl << "Width: ";
    std::cin >> inputWinWt;
    std::cout << "Height: ";
    std::cin >> inputWinHt;

    std::cout << "Press space to begin simulation" << std::endl << "Press ESCAPE to exit";
    */

    int initSuccess = SDL_Init(SDL_INIT_VIDEO);

    if (initSuccess != 0)
    {
        std::cout << "Init error: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Window* hWin = SDL_CreateWindow(WINTT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, inputWinWt, inputWinHt, SDL_WINDOW_SHOWN);
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



    SDL_RenderSetLogicalSize(hRend, logicalWt, logicalHt);

    SDL_Event event;
    bool quit = false;

    Environment main_env;
    main_env.bot = (float)logicalWt;
    main_env.top = 0.0f;
    main_env.left = 0.0f;
    main_env.right = (float)logicalHt;
    main_env.global_force = { 0.0f, 0.0f };

    SDL_Color circleCol = { 255, 255, 255 };
    float circleRad = 5.0f;

    std::vector<PhysicsCircle*> circleVec;
    std::vector<StaticEdge*> edgeVec;

    circleVec.push_back(new PhysicsCircle( {0, 0}, {0, 0}, 5.0f, 1, 1, false, false, main_env, SDL_Color {255, 0, 0} ));
    //edgeVec.push_back(new StaticEdge({ 500, 500 }, { 1000, 600 }, SDL_Color{ 255, 255, 255 }));

    CircleEngine engine(circleVec, edgeVec);

    Uint32 endTime, startTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    float timePassed = 0.0f;

    const char* windowCaption;

    int mouseX, mouseY, lastMouseX = 0, lastMouseY = 0;
    bool runSim = false;

    while (!quit)
    {
        SDL_GetMouseState(&mouseX, &mouseY);
        mouseX /= (inputWinWt / logicalWt); mouseY /= (inputWinHt / logicalHt);

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
                    runSim = !runSim;
                    break;

                case SDLK_r:
                    engine.m_circles.clear();
                    engine.m_circles.push_back(new PhysicsCircle({ 0, 0 }, { 0, 0 }, 5.0f, 1, 1, false, false, main_env, SDL_Color{ 255, 0, 0 }));
                }
            
                break;
            
            case SDL_MOUSEBUTTONDOWN:    
                engine.m_circles.push_back(new PhysicsCircle({ (float)mouseX + 2 * circleRad, (float)mouseY }, { 0.0f, 0.0f }, circleRad, 0.5f, 1.0f, true, false, main_env, SDL_Color{ (Uint8)rand(), (Uint8)rand(), (Uint8)rand() }));
                engine.m_circles.push_back(new PhysicsCircle({ (float)mouseX + 4 * circleRad, (float)mouseY }, { 0.0f, 0.0f }, circleRad, 0.5f, 1.0f, true, false, main_env, SDL_Color{ (Uint8)rand(), (Uint8)rand(), (Uint8)rand() }));
                engine.m_circles.push_back(new PhysicsCircle({ (float)mouseX + 6 * circleRad, (float)mouseY }, { 0.0f, 0.0f }, circleRad, 0.5f, 1.0f, true, false, main_env, SDL_Color{ (Uint8)rand(), (Uint8)rand(), (Uint8)rand() }));
                engine.m_circles.push_back(new PhysicsCircle({ (float)mouseX + 8 * circleRad, (float)mouseY }, { 0.0f, 0.0f }, circleRad, 0.5f, 1.0f, true, false, main_env, SDL_Color{ (Uint8)rand(), (Uint8)rand(), (Uint8)rand() }));

            }
            
        }

        
        //std::cout << engine.m_circles[1]->m_pos.x << ' ' << engine.m_circles[1]->m_pos.y << std::endl;

        if (runSim)
        {
            engine.phys_update(deltaTime);
        }

        
        // Instanteneous velocity calculations
        
        
        engine.m_circles[0]->m_vel.x = (mouseX - lastMouseX) / deltaTime;
        engine.m_circles[0]->m_vel.y = (mouseY - lastMouseY) / deltaTime;

        engine.m_circles[0]->m_pos.x = mouseX;
        engine.m_circles[0]->m_pos.y = mouseY;

        lastMouseX = mouseX;
        lastMouseY = mouseY;
        
        

        SDL_SetRenderDrawColor(hRend, 0, 0, 0, 255);
        SDL_RenderClear(hRend);
        if (runSim)
        {
            engine.phys_update(deltaTime);
        }

        engine.render(hRend);

        SDL_RenderPresent(hRend);

        // Manage frame rate
        endTime = SDL_GetTicks();

        deltaTime = (endTime - startTime) / 1000.0f; // Convert to seconds
        startTime = endTime;
      
        
        //SDL_SetWindowTitle(hWin, (std::string("# Circles: ") + std::to_string(engine.m_circles.size()) + std::string(" FPS: ") + std::to_string((int)roundf(1 / deltaTime))).c_str());
    }

    SDL_DestroyRenderer(hRend);
    SDL_DestroyWindow(hWin);
    SDL_Quit();

    return EXIT_SUCCESS;
}