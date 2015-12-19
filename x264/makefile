objects = simple_code
src = simple_code.cpp
simple_code: $(src)
	 g++ simple_code.cpp -o simple_code -lx264 -ldl -pthread
info:
	 g++ simple_code_info.cpp -o simple_code_info -lx264 -ldl -pthread
clean:
	rm simple_code
	rm -f simple_code_info
