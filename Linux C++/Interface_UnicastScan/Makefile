CXX:=g++

TARGET1:=Interface_UnicastScan

INCFLAGS:=-I${STAPI_ROOT_PATH}/include/GenICam \
		-I${STAPI_ROOT_PATH}/include/StApi

LIBFLAGS:=-L${STAPI_ROOT_PATH}/lib \
		-L${STAPI_ROOT_PATH}/lib/GenICam \
		-lGCBase \
		-lGenApi \
		-lpthread

COMPILEFLAGS:=-Wall -Wextra --pedantic -Wno-unknown-pragmas -Wno-switch \
		-Wno-unused-but-set-variable -Wno-pointer-arith -Wno-unused-parameter \
		-std=c++11

# SDK内の間接依存ライブラリも検索 / Resolve SDK shared-library dependencies.
LIBFLAGS+=-Wl,-rpath-link,${STAPI_ROOT_PATH}/lib -Wl,-rpath-link,${STAPI_ROOT_PATH}/lib/GenICam

all: $(TARGET1)

$(TARGET1): $(TARGET1).cpp
	$(CXX) $^ -o $@ $(COMPILEFLAGS) $(INCFLAGS)	$(LIBFLAGS) -lStApi_TL

clean:
	rm -f $(TARGET1)
