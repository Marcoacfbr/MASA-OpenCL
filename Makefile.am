AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = $(LIBMASA_PATH)
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = masa-opencl
masa_opencl_CXXFLAGS = $(CUDA_CFLAGS) $(COMMONFLAGS) -malign-double -fno-strict-aliasing  
masa_opencl_LDADD = $(CUDA_LIBS) $(COMMONFLAGS) ./src/OpenCLAligner.cu_o $(LIBMASA_PATH)/libmasa.a -lcuda -lcudart -lpthread -lOpenCL

masa_opencl_SOURCES = \
./src/main.cpp \
./src/cuda_util.cpp \
./src/cl_util.cpp \
./src/OpenCLAligner.cpp \
./src/OpenCLAligner.cu \
./src/OpenCLAlignerCL.cpp \
./src/OpenCLAlignerCL.cl \
./src/OpenCLAlignerParameters.cpp 

noinst_HEADERS = \
./src/cuda_util.h \
./src/cl_util.h \
./src/OpenCLAligner.hpp \
./src/OpenCLAlignerCL.hpp \
./src/OpenCLAlignerParameters.hpp 



NVCCFLAGS += -ftz=true -prec-sqrt=false -prec-div=false
NVCCFLAGS += --compiler-options -fno-strict-aliasing
#NVCCFLAGS += -Xptxas -dlcm=cg #disable L1 cache
NVCCFLAGS += $(COMMONFLAGS)


clean-local:
	rm -f ./src/*.cu_o

.cu.cu_o:
	$(NVCC) -o $@ -c $< $(NVCCFLAGS)

./src/OpenCLAligner.cu_o: ./src/OpenCLAligner.hpp
#./src/cuda_util.cu_o: ./src/cuda_util.h

dist-clone-link:
	@if test -L $(LIBMASA_PATH); then \
		mv $(LIBMASA_PATH) $(LIBMASA_PATH).link && \
		cp -r -L $(LIBMASA_PATH).link $(LIBMASA_PATH); \
	fi

dist-unclone-link:
	@if test -L $(LIBMASA_PATH).link; then \
		rm -rf $(LIBMASA_PATH) && \
		mv $(LIBMASA_PATH).link $(LIBMASA_PATH); \
	fi

dist-link: dist-clone-link dist dist-unclone-link

sw-regs:
	$(NVCC) --disable-warnings --ptxas-options=-v -o /tmp/gpu_sw.o -c $(srcdir)/src/OpenCLAligner.cu $(NVCCFLAGS)

sw-ptx:
	$(NVCC) --disable-warnings --ptx $(srcdir)/src/OpenCLAligner.cu $(NVCCFLAGS)

.PHONY: doxygen

doxygen:
	pushd . && cd $(LIBMASA_PATH)/doxygen && doxygen *.doxyfile && popd
	pushd . && cd doxygen && doxygen *.doxyfile && popd









