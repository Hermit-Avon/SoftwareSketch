.PHONY:	all clean

PROJECT_NAME = SoftwareSketch

CXX = clang++

SKETCH = elasticsketch

all:
	$(MAKE) $(SKETCH)

$(SKETCH): 
	$(CXX) -g -Wall -o build/$(SKETCH)  src/$(SKETCH)/main.cpp -lpcap

clean:
	rm -rf build/*