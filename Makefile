
#We try to use The Boehm-Demers-Weiser conservative garbage collector, 
#but it conflicts with the CDSChecker.

obj = src/main.o src/AbortedException.o src/Transaction.o src/BackofManager.o src/TThread.o src/SmartBackofCM.o 

#CDSCheck include path
CDSI = -I/home/yuanfan/Multicore/model-checker/include
#gclib include path
BGCI = -I/home/yuanfan/gclib-7.2/include
#CDSCheck library path
CDSLP = -L/home/yuanfan/Multicore/model-checker/
#gclib library path
BGCLP = -L/home/yuanfan/gclib-7.2/lib/
#CDSChecker
CDSL = -lmodel
#gclib
GCL = -lgc

LF = -rdynamic -lpthread -lrt

CPPFLAGS= -g -O2 -Wall -lpthread 

CMACROS = -DUSECDS

ostm : INCPATH = ${CDSI} ${BGCI}
ostm : LIBPATH = ${CDSLP} ${BGCLP}
ostm : LIBN = ${CDSL} ${GCL}
ostm : CPPFLAGS+=${CMACROS}
#ostm_p : INCPATH = ${BGCI} 
#ostm_p : LIBPATH = ${BGCLP} 
#ostm_p : LIBN = ${GCL} 
ostm_p : CPPFLAGS+= -std=c++11
#ostm_p : CPPFLAGS+= ${CMACROS} 


%.o: %.cpp src/*.h
	${CXX} -c -o $@ $<  ${INCPATH} ${CPPFLAGS} 

ostm : ${obj}
	g++-4.6 -o $@ $^ ${LIBPATH} ${LIBN} ${LF}

ostm_p : ${obj}
	${CXX} -o $@ $^ ${LIBPATH} ${LIBN} ${LF}

${obj} : src/*.h

.PHONY : clean
clean :
	-rm ostm ostm_p $(obj)







