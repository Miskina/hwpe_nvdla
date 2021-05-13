FROM ubuntu:20.04

RUN apt-get update \
&& apt-get update --fix-missing \
&& DEBIAN_FRONTEND="noninteractive" apt-get install --no-install-recommends -y \
                    autoconf \
                    bc \
                    bison \
                    build-essential \
                    ca-certificates \
                    ccache \
                    clang \
                    cmake \
                    default-jdk \
                    flex \
                    git \
                    libattr1-dev \
                    libboost-dev \
                    libcap-dev \
                    libfl-dev \
                    libglib2.0-dev \ 
                    libgoogle-perftools-dev \
                    liblua5.2-dev \
                    libpixman-1-dev \
                    make \
                    numactl \
                    perl \
                    perl-doc \
                    python3 \
                    python3-dev \
                    python3-pip \
                    swig \ 
                    vim \
                    wget \
                    zlib1g-dev \
                    zlibc \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1

RUN mkdir -p /usr/src/systemc-2.3.0a
WORKDIR /usr/src/systemc-2.3.0a

RUN wget -O systemc-2.3.0a.tar.gz http://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.0a.tar.gz \
&& tar xzvf systemc-2.3.0a.tar.gz

RUN cd /usr/src/systemc-2.3.0a/systemc-2.3.0a     \
&& ls                                             \
&& mkdir -p /usr/local/systemc-2.3.0/             \
&& mkdir objdir                                   \
&& cd objdir                                      \
&& ../configure --prefix=/usr/local/systemc-2.3.0 \
&& make                                           \
&& make install

RUN git clone --depth 1 --branch v4.202 https://github.com/verilator/verilator.git /opt/verilator

WORKDIR /opt/verilator
RUN unset VERILATOR_ROOT \
&& autoconf              \
&& ./configure           \
&& make                  \
&& make install


RUN cpan install Config::YAML XML::Simple Capture::Tiny

WORKDIR /root
