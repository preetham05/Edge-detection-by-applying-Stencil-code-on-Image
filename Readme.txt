Project Assignment on Scalable Computing (Edge detection by applying Stencil code on Image)
By Preetham Kannan -Matriculation Number : 217205505

On Cluster:
1.Login to linux cluster (venus.uni-rostock.de) with appropriate credentials. 
2.Load the test-image.png file in the directory where the code resides.
3.Load all the 5 classes,Makefile and mpi_job.slurm to the directory.
  List of 6 file names:

	a.EdgeDetectionMain.c
	b.EdgeDetectionUtil.h
	c.MPI_Edge_Detection.c
	d.ParallelEdgeDetection.h
	e.SerialEdgeDetection.h
	f.Makefile
 	
4.Load intel-mpi compiler using command "module load intel-mpi".
5.Type "make" in the terminal.
6. The Parallel and serial implementation should be executed using ./executable file name in the terminal
	
	-./output_file_name parallel test-image.png test_p.png -lpng
	-./output_file_name serial test-image.png test_s.png -lpng
        -./output_file_name -np 2 outputfilename
	
6.To execute the MPI programs, type "sbatch mpi_job.slurm" on the terminal.
7.A message appears on the terminal 'a job has been submitted', wait for a while after that.
8.Type 'ls'  on the terminal.
9.Now could notice the last slurm-XXX with unique ids and check the time and date with available codes.
10.To read the file "cat slurm-xxxxxxx.out". The execution run time for each of the code is dispayed in the file.
11.output png files will be saved for each of the codes
12.Output files can be opened by tranferring it to the base computer. WinSCP tool can be used for this purpose.




On Local System:
