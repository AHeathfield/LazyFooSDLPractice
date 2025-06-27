/* Headers */
//Using SDL, SDL_image, and STL string
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
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
    // Symbolic constant
    static constexpr float kOriginalSize = -1.f;
    
    //Initializes texture variables
    LTexture();

    //Cleans up texture variables
    ~LTexture();

    //Loads texture from disk
    bool loadFromFile( std::string path );

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



/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The renderer used to draw to the window
SDL_Renderer* gRenderer{ nullptr };

//The directional images
LTexture gColorsTexture;



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

    //Load scene images
    if( gColorsTexture.loadFromFile( "07-color-modulation-and-alpha-blending/colors.png" ) == false )
    {
        SDL_Log( "Unable to load foo image!\n");
        success = false;
    }

    return success;
}


void close()
{
    //Clean up texture
    gColorsTexture.destroy();

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

            //Rotation degrees
            double degrees = 0.0;

            //Flipmode
            SDL_FlipMode flipMode = SDL_FLIP_NONE;

            //Set color constants
            constexpr int kColorMagnitudeCount = 3;
            constexpr Uint8 kColorMagnitudes[ kColorMagnitudeCount ] = { 0x00, 0x7F, 0xFF };
            enum class eColorChannel
            {
                TextureRed       = 0,
                TextureGreen     = 1,
                TextureBlue      = 2,
                TextureAlpha     = 3,

                BackgroundRed    = 4,
                BackgroundGreen  = 5,
                BackgroundBlue   = 6,

                Total            = 7,
                Unknown          = 8
            };

            //Initialize colors
            Uint8 colorChannelsIndices[ static_cast<int>( eColorChannel::Total ) ];
            colorChannelsIndices[ static_cast<int>( eColorChannel::TextureRed ) ]   = 2;
            colorChannelsIndices[ static_cast<int>( eColorChannel::TextureGreen ) ] = 2;
            colorChannelsIndices[ static_cast<int>( eColorChannel::TextureBlue ) ]  = 2;
            colorChannelsIndices[ static_cast<int>( eColorChannel::TextureAlpha ) ] = 2;
            
            colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundRed ) ]   = 2;
            colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundGreen ) ] = 2;
            colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundBlue ) ]  = 2;

            //Initialize blending
            gColorsTexture.setBlending( SDL_BLENDMODE_BLEND );

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

                    //On key press
                    else if( e.type == SDL_EVENT_KEY_DOWN )
                    {
                        //Check for channel key
                        eColorChannel channelToUpdate = eColorChannel::Unknown;
                        switch( e.key.key )
                        {
                            //Update texture color 
                            case SDLK_A:
                                channelToUpdate = eColorChannel::TextureRed;
                                break;
                            case SDLK_S:
                                channelToUpdate = eColorChannel::TextureGreen;
                                break;
                            case SDLK_D:
                                channelToUpdate = eColorChannel::TextureBlue;
                                break;
                            case SDLK_F:
                                channelToUpdate = eColorChannel::TextureAlpha;
                                break;
                            
                            //Update background color
                            case SDLK_Q:
                                channelToUpdate = eColorChannel::BackgroundRed;
                                break;
                            case SDLK_W:
                                channelToUpdate = eColorChannel::BackgroundGreen;
                                break;
                            case SDLK_E:
                                channelToUpdate = eColorChannel::BackgroundBlue;
                                break;
                        }

                        //If channel key was pressed
                        if( channelToUpdate != eColorChannel::Unknown )
                        {
                            //Cycle through channel values
                            colorChannelsIndices[ static_cast<int>( channelToUpdate ) ]++;
                            if( colorChannelsIndices[ static_cast<int>( channelToUpdate ) ] >= kColorMagnitudeCount )
                            {
                                colorChannelsIndices[ static_cast<int>( channelToUpdate ) ] = 0;
                            }

                            //Write color values to console
                            SDL_Log("Texture - R:%d G:%d B:%d A:%d | Background - R:%d G:%d B:%d",
                                kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::TextureRed ) ] ],
                                kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::TextureGreen ) ] ],
                                kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::TextureBlue ) ] ],
                                kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::TextureAlpha ) ] ],
                                kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundRed ) ] ],
                                kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundGreen ) ] ],
                                kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundBlue ) ] ]
                            );
                        }
                    }
                }

                //Fill the background
                SDL_SetRenderDrawColor( gRenderer,
                    kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundRed ) ] ],
                    kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundGreen ) ] ],
                    kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::BackgroundBlue) ] ],
                    0xFF );
                SDL_RenderClear( gRenderer );

                //Set texture color and render
                gColorsTexture.setColor(
                    kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::TextureRed ) ] ],
                    kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::TextureGreen ) ] ],
                    kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::TextureBlue ) ] ]
                );
                gColorsTexture.setAlpha( kColorMagnitudes[ colorChannelsIndices[ static_cast<int>( eColorChannel::TextureAlpha ) ] ]);
                gColorsTexture.render( ( kScreenWidth - gColorsTexture.getWidth() ) / 2.f, ( kScreenHeight - gColorsTexture.getHeight() ) / 2.f );

                //Update screen
                SDL_RenderPresent( gRenderer );
            } 
        }
    }

    //Clean up
    close();

    return exitCode;
}
