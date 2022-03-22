# syntax=docker/dockerfile:1
FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -o Acquire::Check-Valid-Until=false -o Acquire::Check-Date=false update && apt-get install -y -f \
	build-essential \
	wget \
	scons \
	python3 \
	libboost-all-dev \
	libcairo2-dev \
	libnuma-dev \ 
	libproj-dev \
	git-all \
	zlib1g-dev \
	cmake \
	gdb \
	lemon \
	python3 \
	python3-pip \
    python3-dev 

RUN python3 -m pip install numpy scipy networkx OpenMatrix pandas networkx pytest pybind11 stable-baselines3 tf-agents[reverb] tensorforce gym


SHELL ["/bin/bash", "-c"]

#RUN git clone https://github.com/StanfordASL/frank-wolfe-traffic.git
COPY frank-wolfe-traffic frank-wolfe-traffic
WORKDIR frank-wolfe-traffic/External

RUN git clone https://github.com/RoutingKit/RoutingKit.git
RUN git clone https://github.com/ben-strasser/fast-cpp-csv-parser.git
RUN git clone https://github.com/jlblancoc/nanoflann.git

RUN wget http://lemon.cs.elte.hu/pub/sources/lemon-1.3.1.tar.gz
RUN tar xvzf lemon-1.3.1.tar.gz

RUN cd lemon-1.3.1 && mkdir build && cd build && cmake .. && make && make install

RUN git clone https://github.com/pybind/pybind11.git
RUN cd pybind11 && mkdir build && cd build && cmake ..

RUN cd fast-cpp-csv-parser && cp *.h /usr/local/include && cd ..
RUN cd nanoflann && cp -r include /usr/local && cd .. 
RUN cd RoutingKit && make && cp -r include lib /usr/local && cd ..
RUN cd vectorclass && mkdir /usr/local/include/vectorclass && cp *.h special/* $_ && cd .. && cd ..

WORKDIR ..

#RUN scons
#RUN scons -Q variant=Release
#RUN scons -Q variant=Debug

RUN ls

RUN g++ -o frankwolfe$(python3-config --extension-suffix) -std=c++14 -fopenmp -Werror -Wfatal-errors -Wpedantic -pedantic-errors -Wall -Wextra -Wno-missing-braces -Wno-unknown-pragmas -Wno-strict-overflow -Wno-sign-compare -O3 -shared -msse4 -fopenmp -DCSV_IO_NO_THREAD -fPIC $(python3 -m pybind11 --includes) $(python3-config --ldflags) -I. Launchers/AssignTraffic.cpp

ENV PYTHONPATH=/frank-wolfe-traffic/:${PYTHONPATH}
ENV PATH=/frank-wolfe-traffic/Build/Devel/Launchers/:${PATH}