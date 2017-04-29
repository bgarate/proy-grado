sudo ldconfig
g++ -std=c++11 -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux -c vrephaltest.cpp -o vrephaltest.o `pkg-config opencv --cflags --libs`
g++ -std=c++11 -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux -c vrephal.cpp -o vrephal.o `pkg-config opencv --cflags --libs`
gcc -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux -c extApi.c -o extApi.o
gcc -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux -c extApiPlatform.c -o extApiPlatform.o
g++ -std=c++11 extApi.o extApiPlatform.o vrephal.o vrephaltest.o -o vrephaltest -lpthread `pkg-config opencv --cflags --libs`
