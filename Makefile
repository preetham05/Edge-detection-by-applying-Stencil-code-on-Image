EXECS1=MPI_Edge_Detection
EXECS2=EdgeDetectionMain
MPICC=mpicc
GCC=gcc


all: ${EXECS1} {EXECS2}
MPI_Edge_Detection: MPI_Edge_Detection.c
	${MPICC} -o MPI_Edge_Detection MPI_Edge_Detection.c
EdgeDetectionMain: EdgeDetectionMain.c
	${GCC} -o EdgeDetectionMain EdgeDetectionMain.c -lpng -fopenmp
clean:
	 rm ${EXECS}