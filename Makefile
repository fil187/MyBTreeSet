CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

tests: tests.cpp
	$(CXX) $(CXXFLAGS) ./tests.cpp -o tests.exe

run: tests
	./tests.exe

clean:
	rm tests.exe