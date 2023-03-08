# How to use 
# To make tests: make tests 
# To make all experiment binaries: make binaries

CXX=g++
CFLAGS = -std=gnu++11 -lgfortran -Wall -O3
INC=-I ./../faiss -I include/
# LFLAGS=./../faiss/build/faiss/libfaiss.so /home/gg29/.conda/envs/graphSage/lib/libgomp.so -lpthread ./faiss/tools/lib/libopenblas.a -lpthread ./faiss/tools/lib/lib/libopenblas.a -lpthread -lm -ldl -lgfortran
LFLAGS=./../faiss/build/faiss/libfaiss.a /home/gg29/OpenBLAS/libopenblas.a -lpthread -lm -ldl -lgfortran -fopenmp

index: clean_index
	$(CXX) $(INC) $(CFLAGS) src/readfile.cpp \
							src/MurmurHash3.cpp \
							src/cbloomfilter.cpp \
							src/crambo.cpp \
							src/bitArray.cpp \
							src/utils.cpp \
							src/FilterIndex.cpp \
	-o index $(LFLAGS)

preFilterIndex: clean_preFilterIndex
	$(CXX) $(INC) $(CFLAGS) src/readfile.cpp \
							src/MurmurHash3.cpp \
							src/cbloomfilter.cpp \
							src/crambo.cpp \
							src/bitArray.cpp \
							src/utils.cpp \
							src/PreFilterIndex.cpp \
	-o preFilterIndex $(LFLAGS)

searchFilter: clean_searchFilter
	$(CXX) $(INC) $(CFLAGS) src/readfile.cpp \
							src/MurmurHash3.cpp \
							src/cbloomfilter.cpp \
							src/crambo.cpp \
							src/bitArray.cpp \
							src/utils.cpp \
							src/searchFilter.cpp \
	-o searchFilter $(LFLAGS)
	
query: query.cpp
	$(CXX) $(INC) $(CFLAGS) query.cpp -o query $(LFLAGS)

clean_index:
	rm -f index
clean_preFilterIndex:
	rm -f preFilterIndex
clean_searchFilter:
	rm -f searchFilter

.PHONY: clean all

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all
