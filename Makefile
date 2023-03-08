all: h264Splitter4
	
h264Splitter4: h264Splitter.cpp
	g++ h264Splitter.cpp -DSTARTER264NAL4 -o h264Splitter4

clean:
	rm -f h264Splitter4

