objects = simple_video
src = simple_code_info.c video_yuv420.c
simple_video: $(src) video_yuv420.o simple_code_info.o
	 g++ simple_code_info.o video_yuv420.o -o simple_video -lx264 -ldl -pthread
video_yuv420.o:video_yuv420.c
	gcc -c video_yuv420.c
simple_code_info.o:simple_code_info.c simple_code_info.h
	gcc simple_code_info.c -c  
info:
	 g++ simple_code_info.cpp -o simple_code_info -lx264 -ldl -pthread
clean:
	rm video_yuv420.o 
	rm -f simple_code_info.o
