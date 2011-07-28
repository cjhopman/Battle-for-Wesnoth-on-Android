
        #include <SDL.h> 
        
#if SDL_VERSIONNUM(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL) < SDL_VERSIONNUM(1, 2, 7)
#error Library is too old!
#endif

        int main(int argc, char** argv)
        {
        }
        
