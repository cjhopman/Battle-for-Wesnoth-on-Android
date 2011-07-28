
        #include <SDL_image.h> 
        
#if SDL_VERSIONNUM(SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_PATCHLEVEL) < SDL_VERSIONNUM(1, 2, 0)
#error Library is too old!
#endif

        int main(int argc, char** argv)
        {
        }
        
