/*This source code copyrighted by Lazy Foo' Productions 2004-2025
and may not be redistributed without written permission.*/

/* Headers */
//Using SDL, SDL_image, SDL_ttf, and STL string/stringstream
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <sstream>


/* Constants */
//Screen dimension constants
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };
constexpr int kScreenFps{ 60 };


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

    #if defined(SDL_TTF_MAJOR_VERSION)
    //Creates texture from text
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
    #endif

    //Cleans up texture
    void destroy();

    //Sets color modulation
    void setColor( Uint8 r, Uint8 g, Uint8 b);

    //Sets opacity
    void setAlpha( Uint8 alpha );

    //Sets blend mode
    void setBlending( SDL_BlendMode blendMode );

    //Draws texture
    void render( float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize, double degrees = 0.0, SDL_FPoint* center = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE );

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

class LTimer
{
    public:
        //Initializes variables
        LTimer();

        //The various clock actions
        void start();
        void stop();
        void pause();
        void unpause();

        //Gets the timer's time
        Uint64 getTicksNS();

        //Checks the status of the timer
        bool isStarted();
        bool isPaused();

    private:
        //The clock time when the timer started
        Uint64 mStartTicks;

        //The ticks stored when the timer was paused
        Uint64 mPausedTicks;

        //The timer status
        bool mPaused;
        bool mStarted;
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

//Global font
TTF_Font* gFont{ nullptr };

//The frames per second texture
LTexture gFpsTexture; 


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

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
    //Clean up existing texture
    destroy();

    //Load text surface
    if( SDL_Surface* textSurface = TTF_RenderText_Blended( gFont, textureText.c_str(), 0, textColor ); textSurface == nullptr )
    {
        SDL_Log( "Unable to render text surface! SDL_ttf Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create texture from surface
        if( mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface ); mTexture == nullptr )
        {
            SDL_Log( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Free temp surface
        SDL_DestroySurface( textSurface );
    }
    
    //Return success if texture loaded
    return mTexture != nullptr;
}
#endif

void LTexture::destroy()
{
    //Clean up texture
    SDL_DestroyTexture( mTexture );
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void LTexture::setColor( Uint8 r, Uint8 g, Uint8 b )
{
    SDL_SetTextureColorMod( mTexture, r, g, b );
}

void LTexture::setAlpha( Uint8 alpha )
{
    SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::setBlending( SDL_BlendMode blendMode )
{
    SDL_SetTextureBlendMode( mTexture, blendMode );
}

void LTexture::render( float x, float y, SDL_FRect* clip, float width, float height, double degrees, SDL_FPoint* center, SDL_FlipMode flipMode )
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
    SDL_RenderTextureRotated( gRenderer, mTexture, clip, &dstRect, degrees, center, flipMode );
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

//LTimer Implementation
LTimer::LTimer():
    mStartTicks{ 0 },
    mPausedTicks{ 0 },

    mPaused{ false },
    mStarted{ false }
{

}

void LTimer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicksNS();
    mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

    //Clear tick variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause()
{
    //If the timer is running and isn't already paused
    if( mStarted && !mPaused )
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicksNS() - mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    //If the timer is running and paused
    if( mStarted && mPaused )
    {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetTicksNS() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint64 LTimer::getTicksNS()
{
    //The actual timer time
    Uint64 time{ 0 };

    //If the timer is running
    if( mStarted )
    {
        //If the timer is paused
        if( mPaused )
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            time = SDL_GetTicksNS() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
    //Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
    //Timer is running and paused
    return mPaused && mStarted;
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
        if( SDL_CreateWindowAndRenderer( "SDL3 Tutorial: Frame Rate and VSync", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer ) == false )
        {
            SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Enable VSync
            if( SDL_SetRenderVSync( gRenderer, 1 ) == false )
            {
                SDL_Log( "Could not enable VSync! SDL error: %s\n", SDL_GetError() );
                success = false;
            }

            //Initialize font loading
            if( TTF_Init() == false )
            {
                SDL_Log( "SDL_ttf could not initialize! SDL_ttf error: %s\n", SDL_GetError() );
                success = false;
            }
        }
    }

    return success;
}

bool loadMedia()
{
    //File loading flag
    bool success{ true };

    //Load scene font
    std::string fontPath{ "12-frame-rate-and-vsync/lazy.ttf" };
    if( gFont = TTF_OpenFont( fontPath.c_str(), 28 ); gFont == nullptr )
    {
        SDL_Log( "Could not load %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError() );
        success = false;
    }
    else
    {
        //Load text
        SDL_Color textColor{ 0x00, 0x00, 0x00, 0xFF };
        if( !gFpsTexture.loadFromRenderedText( "Enter to start/stop or space to pause/unpause", textColor ) )
        {
            SDL_Log( "Could not load text texture %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError() );
            success = false;
        }
    }

    return success;
}

void close()
{
    //Clean up textures
    gFpsTexture.destroy();

    //Free font
    TTF_CloseFont( gFont );
    gFont = nullptr;

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    gRenderer = nullptr;
    SDL_DestroyWindow( gWindow );
    gWindow = nullptr;

    //Quit SDL subsystems
    TTF_Quit();
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

            //VSync toggle
            bool vsyncEnabled{ true };

            //FPS cap toggle
            bool fpsCapEnabled{ false };

            //Timer to cap frame rate
            LTimer capTimer;

            //Time spent rendering
            Uint64 renderingNS{ 0 };

            //In memory text stream
            std::stringstream timeText;

            //The main loop
            while( quit == false )
            {
                //Start frame time
                capTimer.start();

                //Get event data
                while( SDL_PollEvent( &e ) == true )
                {
                    //If event is quit type
                    if( e.type == SDL_EVENT_QUIT )
                    {
                        //End the main loop
                        quit = true;
                    }
                    //Reset start time on return keypress
                    else if( e.type == SDL_EVENT_KEY_DOWN )
                    {
                        //VSync toggle
                        if( e.key.key == SDLK_RETURN )
                        {
                            vsyncEnabled = !vsyncEnabled;
                            SDL_SetRenderVSync( gRenderer, ( vsyncEnabled ) ? 1 : SDL_RENDERER_VSYNC_DISABLED );
                        }
                        //FPS cap toggle
                        else if( e.key.key == SDLK_SPACE )
                        {
                            fpsCapEnabled = !fpsCapEnabled;
                        }
                    }
                }
                
                //Update text
                if( renderingNS != 0 )
                {
                    double framesPerSecond{ 1000000000.0 / static_cast<double>( renderingNS ) };

                    timeText.str( "" );
                    timeText << "Frames per second " << ( vsyncEnabled ? "(VSync) " : "" ) << ( fpsCapEnabled ? "(Cap) " : "" ) << framesPerSecond; 
                    SDL_Color textColor{ 0x00, 0x00, 0x00, 0xFF };
                    gFpsTexture.loadFromRenderedText( timeText.str().c_str(), textColor );
                }

                //Fill the background
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF,  0xFF );
                SDL_RenderClear( gRenderer );

                //Draw text
                gFpsTexture.render( ( kScreenWidth - gFpsTexture.getWidth() ) / 2.f,  ( kScreenHeight - gFpsTexture.getHeight() ) / 2.f );

                //Update screen
                SDL_RenderPresent( gRenderer );

                //Get time to render frame
                renderingNS = capTimer.getTicksNS();

                //If time remaining in frame
                constexpr Uint64 nsPerFrame = 1000000000 / kScreenFps; 
                if( fpsCapEnabled && renderingNS < nsPerFrame)
                {
                    //Sleep remaining frame time
                    Uint64 sleepTime = nsPerFrame - renderingNS;
                    SDL_DelayNS( nsPerFrame - renderingNS );

                    //Get frame time including sleep time
                    renderingNS = capTimer.getTicksNS();
                }
            } 
        }
    }

    //Clean up
    close();

    return exitCode;
}
