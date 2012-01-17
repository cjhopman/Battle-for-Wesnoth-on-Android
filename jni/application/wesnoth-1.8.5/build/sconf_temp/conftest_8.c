
        #include <libintl.h>

        int main()
        {
            textdomain("test");
            char* text = gettext("foo");
        }
        
