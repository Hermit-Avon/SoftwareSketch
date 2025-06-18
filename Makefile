.PHONY:	all clean

PROJECT_NAME = SoftwareSketch

CXX = clang++

SKETCH = countsketch

all:
	$(MAKE) $(SKETCH)

$(SKETCH): 
	$(CXX) -g -Wall -o build/$(SKETCH)  src/$(SKETCH)/$(SKETCH).cpp -lpcap

clean:
	rm build/*