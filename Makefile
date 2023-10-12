
CXX=g++
CFLAGS = -std=gnu++17 -lgfortran -Wall -O3 -w -mavx
INC=-I /scratch1/jwy4/faiss -I include/
LFLAGS=/scratch1/jwy4/faiss/build/faiss/libfaiss.a /scratch1/jwy4/OpenBLAS/libopenblas.a -lpthread -lm -ldl -lgfortran -fopenmp

index: clean_index
	$(CXX) $(INC) $(CFLAGS) src/readfile.cpp \
							src/utils.cpp \
							src/FilterIndexHamming.cpp \
							src/index.cpp \
	-o index $(LFLAGS)

query: clean_query
	$(CXX) $(INC) $(CFLAGS) src/readfile.cpp \
							src/utils.cpp \
							src/FilterIndexHamming.cpp \
							src/query.cpp \
	-o query $(LFLAGS)
	
clean_index:
	rm -f index
clean_query:
	rm -f query

.PHONY: clean all

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all
