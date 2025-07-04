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



/* Function Prototypes */
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();



/* Class Prototypes */
class LTexture
{
public:
    //Initializes texture variables
    LTexture();

    //Cleans up texture variables
    ~LTexture();

    //Loads texture from disk
    bool loadFromFile( std::string path );

    //Cleans up texture
    void destroy();

    //Draws texture
    void render( float x, float y );

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



/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The renderer used to draw to the window
SDL_Renderer* gRenderer{ nullptr };

//The directional images
LTexture gUpTexture, gDownTexture, gLeftTexture, gRightTexture;



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

        //Clean up loaded surface
        SDL_DestroySurface( loadedSurface );
    }

    //Return success if texture loaded
    return mTexture != nullptr;
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

void LTexture::destroy()
{
    //Clean up texture
    SDL_DestroyTexture( mTexture );
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void LTexture::render( float x, float y )
{
    //Set texture position
    SDL_FRect dstRect{ x, y, static_cast<float>( mWidth ), static_cast<float>( mHeight ) };

    //Render texture
    SDL_RenderTexture( gRenderer, mTexture, nullptr, &dstRect );
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
        if( SDL_CreateWindowAndRenderer( "SDL3 Tutorial: Textures and Extension Libraries", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer ) == false )
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

    //Load directional images
    if( gUpTexture.loadFromFile( "03-key-presses-and-key-states/up.png" ) == false )
    {
        SDL_Log( "Unable to load up image!\n");
        success = false;
    }
    if( gDownTexture.loadFromFile( "03-key-presses-and-key-states/down.png" ) == false )
    {
        SDL_Log( "Unable to load down image!\n");
        success = false;
    }
    if( gLeftTexture.loadFromFile( "03-key-presses-and-key-states/left.png" ) == false )
    {
        SDL_Log( "Unable to load left image!\n");
        success = false;
    }
    if( gRightTexture.loadFromFile( "03-key-presses-and-key-states/right.png" ) == false )
    {
        SDL_Log( "Unable to load right image!\n");
        success = false;
    }

    return success;
}


void close()
{
    //Clean up texture
    gUpTexture.destroy();
    gDownTexture.destroy();
    gLeftTexture.destroy();
    gRightTexture.destroy();
    
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
            
            //The currently rendered texture (default texture is up)
            LTexture* currentTexture = &gUpTexture;

            //Background color defaults to white
            SDL_Color bgColor{ 0xFF, 0xFF, 0xFF, 0xFF };
            
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
                    //On keyboard key press
                    else if( e.type == SDL_EVENT_KEY_DOWN )
                    {
                        //Set texture
                        if( e.key.key == SDLK_UP )
                        {
                            currentTexture = &gUpTexture;
                        }
                        else if( e.key.key == SDLK_DOWN )
                        {
                            currentTexture = &gDownTexture;
                        }
                        else if( e.key.key == SDLK_LEFT )
                        {
                            currentTexture = &gLeftTexture;
                        }
                        else if( e.key.key == SDLK_RIGHT )
                        {
                            currentTexture = &gRightTexture;
                        }
                    }
                }

                 //Reset background color to white
                bgColor.r = 0xFF;
                bgColor.g = 0xFF;
                bgColor.b = 0xFF;

                //Set background color based on key state
                const bool* keyStates = SDL_GetKeyboardState( nullptr );
                if( keyStates[ SDL_SCANCODE_UP ] == true )
                {
                    //Red
                    bgColor.r = 0xFF;
                    bgColor.g = 0x00;
                    bgColor.b = 0x00;
                }
                else if( keyStates[ SDL_SCANCODE_DOWN ] == true )
                {
                    //Green
                    bgColor.r = 0x00;
                    bgColor.g = 0xFF;
                    bgColor.b = 0x00;
                }
                else if( keyStates[ SDL_SCANCODE_LEFT ] == true )
                {
                    //Yellow
                    bgColor.r = 0xFF;
                    bgColor.g = 0xFF;
                    bgColor.b = 0x00;
                }
                else if( keyStates[ SDL_SCANCODE_RIGHT ] == true )
                {
                    //Blue
                    bgColor.r = 0x00;
                    bgColor.g = 0x00;
                    bgColor.b = 0xFF;
                }

                //Fill the background 
                SDL_SetRenderDrawColor( gRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF );
                SDL_RenderClear( gRenderer );
            
                //Render image on screen (calculation is how to center image)
                currentTexture->render( ( kScreenWidth - currentTexture->getWidth() ) / 2.f, ( kScreenHeight - currentTexture->getHeight() ) / 2.f );

                //Update screen
                SDL_RenderPresent( gRenderer );
            } 
        }
    }

    //Clean up
    close();

    return exitCode;
}
