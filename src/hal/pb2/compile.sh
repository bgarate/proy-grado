sudo ldconfig
g++ -std=c++11 -g -c pb2haltest.cpp -o pb2haltest.o -Iinclude `pkg-config opencv --cflags --libs`
g++ -std=c++11 -g -c pb2hal.cpp -o pb2hal.o -Iinclude `pkg-config opencv --cflags --libs`
g++ -std=c++11 -Llib pb2hal.o pb2haltest.o -o pb2haltest -lpthread -larcommands -larcontroller -lardiscovery -larnetwork -larnetworkal -larsal -larstream -larstream2 -larmavlink -ljson -larmedia -larutils -lcurl -lardatatransfer -lmux -lpomp -lcrypto -lssl -lz -lncurses -ltls -larupdater `pkg-config opencv --cflags --libs`
#-larcommands -larcontroller -lardiscovery -larnetwork -larnetworkal -larsal -larstream -larstream2 -larmavlink -ljson -larmedia -larutils -lcurl -lardatatransfer -lmux -lpomp -lcrypto -lssl -lz -ltls 
#-larcommands -larcontroller -lardiscovery -larnetwork -larnetworkal -larsal -larstream -larstream2 -larmavlink -ljson -larmedia -larutils -lcurl -lardatatransfer -lmux -lpomp -lcrypto -lssl -lz -lncurses -ltls -larupdater