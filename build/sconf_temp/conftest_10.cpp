
        #include <boost/iostreams/filtering_stream.hpp>
        #include <boost/iostreams/filter/gzip.hpp>

        int main()
        {
            boost::iostreams::filtering_stream<boost::iostreams::output> filter;
            filter.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params()));
        }
        
