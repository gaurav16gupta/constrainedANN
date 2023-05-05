
CXX=g++
CFLAGS = -std=gnu++17 -lgfortran -Wall -O3 -w -mavx
INC=-I /scratch/gg29/faiss -I include/
LFLAGS=/scratch/gg29/faiss/build/faiss/libfaiss.a /home/gg29/OpenBLAS/libopenblas.a -lpthread -lm -ldl -lgfortran -fopenmp

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
	
clean_index:
	rm -f index
clean_query:
	rm -f query
clean_preFilterIndex:
	rm -f preFilterIndex
clean_searchFilter:
	rm -f searchFilter

.PHONY: clean all

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all
