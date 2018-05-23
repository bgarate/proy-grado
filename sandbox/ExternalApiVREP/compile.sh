g++ -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux -c simular.cpp -o simular.o
gcc -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux -c extApi.c -o extApi.o
gcc -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -D__linux -c extApiPlatform.c -o extApiPlatform.o
g++ extApi.o extApiPlatform.o simular.o -o simular -lpthread
