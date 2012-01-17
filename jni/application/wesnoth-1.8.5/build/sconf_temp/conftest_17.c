
        #include <SDL_mixer.h> 
        
#if SDL_VERSIONNUM(SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION, SDL_MIXER_PATCHLEVEL) < SDL_VERSIONNUM(1, 2, 0)
#error Library is too old!
#endif

        int main(int argc, char** argv)
        {
        }
        
