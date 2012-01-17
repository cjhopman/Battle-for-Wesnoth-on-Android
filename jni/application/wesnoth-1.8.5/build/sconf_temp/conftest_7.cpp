
    #include <iostream>
    int main()
    {
    std::cout << "Hello, world\n";
    }
    
        #define GCC_VERSION (__GNUC__ * 10000 \
                           + __GNUC_MINOR__ * 100 \
                           + __GNUC_PATCHLEVEL__)
        #if GCC_VERSION < 30300
        #error Compiler version is too old!
        #endif
        
