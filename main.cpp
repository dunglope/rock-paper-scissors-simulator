#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

// prefix
int objSize = 20;
int numberOfRocks = 20;
int numberOfPapers = 20;
int numberOfScissors = 20;

// define objective
enum ObjectType { ROCK, PAPER, SCISSORS };

// Object class
class Object {
public:
    ObjectType type;
    int x, y;
    int dx, dy;

    // Create object with random position
    Object(ObjectType t, int xPos, int yPos) : type(t), x(xPos), y(yPos) {
        dx = (rand() % 2 + 1) * (rand() % 2 == 0 ? 1 : -1);
        dy = (rand() % 2 + 1) * (rand() % 2 == 0 ? 1 : -1); 
    }

    // Check for border
    void move(int width, int height) {
        x += dx;
        y += dy;
        if (x < 0 || x >= width) dx = -dx;  // Rotate if collised with edge
        if (y < 0 || y >= height) dy = -dy;
    }

    // Check for rules
    void interact(Object& other) {
        if (type == SCISSORS && other.type == PAPER) {
            other.type = SCISSORS;  // Scissors > Paper
        } else if (type == PAPER && other.type == ROCK) {
            other.type = PAPER;     // Paper > Rock
        } else if (type == ROCK && other.type == SCISSORS) {
            other.type = ROCK;      // Rock > Scissors
        }
    }

    // Render object
    void render(SDL_Renderer* renderer, SDL_Texture* rockTex, SDL_Texture* paperTex, SDL_Texture* scissorsTex) const {
        SDL_Texture* texture = nullptr;
        if (type == ROCK) texture = rockTex;
        else if (type == PAPER) texture = paperTex;
        else texture = scissorsTex;

        SDL_Rect dstRect = {x, y, objSize, objSize};  // Draw with objSize
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    }
};

// Check for collision
bool checkCollision(const Object& obj1, const Object& obj2) {
    int distanceX = obj1.x - obj2.x;
    int distanceY = obj1.y - obj2.y;
    return (distanceX * distanceX + distanceY * distanceY) < (50 * 50); // Va chạm nếu khoảng cách nhỏ hơn 50
}

int main(int argc, char* argv[]) {
    srand(time(0));

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    const int windowWidth = 800;
    const int windowHeight = 600;
    SDL_Window* window = SDL_CreateWindow("Rock-Paper-Scissors with Emojis", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Load textures
    SDL_Surface* rockSurface = SDL_LoadBMP("assets/rock.bmp");
    SDL_Surface* paperSurface = SDL_LoadBMP("assets/paper.bmp");
    SDL_Surface* scissorsSurface = SDL_LoadBMP("assets/scissors.bmp");
    SDL_Texture* rockTexture = SDL_CreateTextureFromSurface(renderer, rockSurface);
    SDL_Texture* paperTexture = SDL_CreateTextureFromSurface(renderer, paperSurface);
    SDL_Texture* scissorsTexture = SDL_CreateTextureFromSurface(renderer, scissorsSurface);
    SDL_FreeSurface(rockSurface);
    SDL_FreeSurface(paperSurface);
    SDL_FreeSurface(scissorsSurface);

    std::vector<Object> objects;

    // Rocks position (left)
    for (int i = 0; i < numberOfRocks; ++i) {
        objects.push_back(Object(ROCK, rand() % (windowWidth / 3), rand() % windowHeight));
    }
    // Papers position (middle)
    for (int i = 0; i < numberOfPapers; ++i) {
        objects.push_back(Object(PAPER, windowWidth / 3 + rand() % (windowWidth / 3), rand() % windowHeight));
    }
    // Scissors position (right)
    for (int i = 0; i < numberOfScissors; ++i) {
        objects.push_back(Object(SCISSORS, 2 * windowWidth / 3 + rand() % (windowWidth / 3), rand() % windowHeight));
    }

    bool quit = false;
    SDL_Event e;

    const int FPS = 60;  
    const int frameDelay = 1000 / FPS;  

    Uint32 frameStart;
    int frameTime;

    // Main loop
    while (!quit) {
        frameStart = SDL_GetTicks();  

        // Check events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Check for collision
        for (auto& obj : objects) {
            obj.move(windowWidth, windowHeight);
        }
        for (size_t i = 0; i < objects.size(); ++i) {
            for (size_t j = i + 1; j < objects.size(); ++j) {
                if (checkCollision(objects[i], objects[j])) {
                    objects[i].interact(objects[j]);
                    objects[j].interact(objects[i]);
                }
            }
        }

        // Draw objects to screen
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);  // Colors are always RGBA
        SDL_RenderClear(renderer);

        for (const auto& obj : objects) {
            obj.render(renderer, rockTexture, paperTexture, scissorsTexture);
        }

        SDL_RenderPresent(renderer);

        // FPS cap
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Destroy all textures
    SDL_DestroyTexture(rockTexture);
    SDL_DestroyTexture(paperTexture);
    SDL_DestroyTexture(scissorsTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
