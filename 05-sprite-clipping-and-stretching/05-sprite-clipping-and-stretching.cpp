/*This source code copyrighted by Lazy Foo' Productions 2004-2025
and may not be redistributed without written permission.*/

/* Headers */
//Using SDL, SDL_image, and STL string
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>


/* Constants */
//Screen dimension constants
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };


/* Class Prototypes */
class LTexture
{
public:
    //Symbolic constant
    static constexpr float kOriginalSize = -1.f;

    //Initializes texture variables
    LTexture();

    //Cleans up texture variables
    ~LTexture();

    //Loads texture from disk
    bool loadFromFile( std::string path );

    //Cleans up texture
    void destroy();

    //Draws texture
    void render( float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize );

    //Gets texture attributes
    int getWidth();
    int getHeight();
    bool isLoaded();

private:
    //Contains texture data
    SDL_Texture* mTexture;

    //Texture dimensions
    int mWidth;
    int mHeight;
};


/* Function Prototypes */
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();


/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The renderer used to draw to the window
SDL_Renderer* gRenderer{ nullptr };

//The sprite sheet texture
LTexture gSpriteSheetTexture; 


/* Class Implementations */
//LTexture Implementation
LTexture::LTexture():
    //Initialize texture variables
    mTexture{ nullptr },
    mWidth{ 0 },
    mHeight{ 0 }
{

}

LTexture::~LTexture()
{
    //Clean up texture
    destroy();
}

bool LTexture::loadFromFile( std::string path )
{
    //Clean up texture if it already exists
    destroy();

    //Load surface
    if( SDL_Surface* loadedSurface = IMG_Load( path.c_str() ); loadedSurface == nullptr )
    {
        SDL_Log( "Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError() );
    }
    else
    {
        //Color key image
        if( SDL_SetSurfaceColorKey( loadedSurface, true, SDL_MapSurfaceRGB( loadedSurface, 0x00, 0xFF, 0xFF ) ) == false )
        {
            SDL_Log( "Unable to color key! SDL error: %s", SDL_GetError() );
        }
        else
        {
            //Create texture from surface
            if( mTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface ); mTexture == nullptr )
            {
                SDL_Log( "Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError() );
            }
            else
            {
                //Get image dimensions
                mWidth = loadedSurface->w;
                mHeight = loadedSurface->h;
            }
        }
        
        //Clean up loaded surface
        SDL_DestroySurface( loadedSurface );
    }

    //Return success if texture loaded
    return mTexture != nullptr;
}

void LTexture::destroy()
{
    //Clean up texture
    SDL_DestroyTexture( mTexture );
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void LTexture::render( float x, float y, SDL_FRect* clip, float width, float height )
{
    //Set texture position
    SDL_FRect dstRect{ x, y, static_cast<float>( mWidth ), static_cast<float>( mHeight ) };

    //Default to clip dimensions if clip is given
    if( clip != nullptr )
    {
        dstRect.w = clip->w;
        dstRect.h = clip->h;
    }

    //Resize if new dimensions are given
    if( width > 0 )
    {
        dstRect.w = width;
    }
    if( height > 0 )
    {
        dstRect.h = height;
    }

    //Render texture
    SDL_RenderTexture( gRenderer, mTexture, clip, &dstRect );
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

bool LTexture::isLoaded()
{
    return mTexture != nullptr;
}


/* Function Implementations */
bool init()
{
    //Initialization flag
    bool success{ true };

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) == false )
    {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window with renderer
        if( SDL_CreateWindowAndRenderer( "SDL3 Tutorial: Sprite Clipping and Stretching", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer ) == false )
        {
            SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
            success = false;
        }
    }

    return success;
}

bool loadMedia()
{
    //File loading flag
    bool success{ true };

    //Load scene images
    if( gSpriteSheetTexture.loadFromFile( "05-sprite-clipping-and-stretching/dots.png" ) == false )
    {
        SDL_Log( "Unable to load sprite sheet image!\n");
        success = false;
    }

    return success;
}

void close()
{
    //Clean up textures
    gSpriteSheetTexture.destroy();
    
    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    gRenderer = nullptr;
    SDL_DestroyWindow( gWindow );
    gWindow = nullptr;

    //Quit SDL subsystems
    SDL_Quit();
}


int main( int argc, char* args[] )
{
    //Final exit code
    int exitCode{ 0 };

    //Initialize
    if( init() == false )
    {
        SDL_Log( "Unable to initialize program!\n" );
        exitCode = 1;
    }
    else
    {
        //Load media
        if( loadMedia() == false )
        {
            SDL_Log( "Unable to load media!\n" );
            exitCode = 2;
        }
        else
        {
            //The quit flag
            bool quit{ false };

            //The event data
            SDL_Event e;
            SDL_zero( e );

            //The main loop
            while( quit == false )
            {
                //Get event data
                while( SDL_PollEvent( &e ) == true )
                {
                    //If event is quit type
                    if( e.type == SDL_EVENT_QUIT )
                    {
                        //End the main loop
                        quit = true;
                    }
                }

                //Fill the background white
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );
            
                //Init sprite clip
                constexpr float kSpriteSize = 100.f;
                SDL_FRect spriteClip{ 0.f, 0.f, kSpriteSize, kSpriteSize };
                
                //Init sprite size
                SDL_FRect spriteSize{ 0.f, 0.f, kSpriteSize, kSpriteSize };

                
                //Use top left sprite
                spriteClip.x =         0.f;
                spriteClip.y =         0.f;

                //Set sprite size to original size
                spriteSize.w = kSpriteSize;
                spriteSize.h = kSpriteSize;

                //Draw original sized sprite
                gSpriteSheetTexture.render(                         0.f,                          0.f, &spriteClip, spriteSize.w, spriteSize.h );
                

                //Use top right sprite
                spriteClip.x = kSpriteSize;
                spriteClip.y =         0.f;

                //Set sprite to half size
                spriteSize.w = kSpriteSize * 0.5f;
                spriteSize.h = kSpriteSize * 0.5f;

                //Draw half size sprite
                gSpriteSheetTexture.render( kScreenWidth - spriteSize.w,                          0.f, &spriteClip, spriteSize.w, spriteSize.h );
                

                //Use bottom left sprite
                spriteClip.x =         0.f;
                spriteClip.y = kSpriteSize;

                //Set sprite to double size
                spriteSize.w = kSpriteSize * 2.f;
                spriteSize.h = kSpriteSize * 2.f;
                
                //Draw double size sprite
                gSpriteSheetTexture.render(                         0.f, kScreenHeight - spriteSize.h, &spriteClip, spriteSize.w, spriteSize.h );


                //Use bottom right sprite
                spriteClip.x = kSpriteSize;
                spriteClip.y = kSpriteSize;

                //Squish the sprite vertically
                spriteSize.w = kSpriteSize;
                spriteSize.h = kSpriteSize * 0.5f;
                
                //Draw squished sprite
                gSpriteSheetTexture.render( kScreenWidth - spriteSize.w, kScreenHeight - spriteSize.h, &spriteClip, spriteSize.w, spriteSize.h );
                

                //Update screen
                SDL_RenderPresent( gRenderer );
            } 
        }
    }

    //Clean up
    close();

    return exitCode;
}
