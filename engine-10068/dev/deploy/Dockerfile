FROM ubuntu:14.04

RUN apt-get update && \
        apt-get install build-essential software-properties-common -y && \
        add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
        apt-get update && \
        sudo apt-get install gcc-6 g++-6 -y

RUN apt-get update && \
        apt-get install git -y

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6

RUN apt-get update && \
        apt-get install wget -y

RUN apt-get remove -yf cmake && \
        apt-get install -y libssl-dev && \
        wget "https://github.com/Kitware/CMake/releases/download/v3.16.0/cmake-3.16.0.tar.gz" && \
        tar zxf cmake-3.16.0.tar.gz && \
        cd cmake-3.16.0 && \
        ./bootstrap --parallel=4 && make && make install && \
        cd .. && \
        rm cmake-3.16.0.tar.gz && \
        rm -r cmake-3.16.0

RUN wget "https://www.samba.org/ftp/ccache/ccache-3.4.2.tar.gz" && \
        tar zxf ccache-3.4.2.tar.gz && \
        cd ccache-3.4.2 && \
        ./configure && \
        make -j4 && \
        make install && \
        cd .. && \
        rm ccache-3.4.2.tar.gz && \
        rm -r ccache-3.4.2

ENV CCACHE_DIR /ccache_data

VOLUME ["/data/code/blockmango", "/ccache_data"]
ENTRYPOINT ["/data/code/blockmango/dev/deploy/build_server.sh"]
