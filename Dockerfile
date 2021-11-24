FROM ubuntu:20.04

ARG USER_ID
ARG GROUP_ID

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
                    ssh-client \
                    ninja-build \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1

RUN mkdir -p /usr/src                                                                                              \
&& cd /usr/src                                                                                                     \
&& wget -O systemc-2.3.0a.tar.gz http://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.0a.tar.gz \
&& tar xzvf systemc-2.3.0a.tar.gz                                                                                  \
&& cd systemc-2.3.0a                                                                                               \
&& mkdir -p /usr/local/systemc-2.3.0/                                                                              \
&& mkdir objdir                                                                                                    \
&& cd objdir                                                                                                       \
&& ../configure --prefix=/usr/local/systemc-2.3.0                                                                  \
&& make                                                                                                            \
&& make install

RUN git clone --depth 1 --branch v4.202 https://github.com/verilator/verilator.git /opt/verilator \
&& cd /opt/verilator                                                                              \
&& unset VERILATOR_ROOT                                                                           \
&& autoconf                                                                                       \
&& ./configure                                                                                    \
&& make                                                                                           \
&& make install

RUN cpan install Config::YAML XML::Simple Capture::Tiny

# RUN curl --proto '=https' --tlsv1.2 https://fabianschuiki.github.io/bender/init -sSf | sh

RUN if [ ${USER_ID:-0} -ne 0 ] && [ ${GROUP_ID:-0} -ne 0 ]; then \
    groupadd -g ${GROUP_ID} nvdla &&\
    useradd -l -u ${USER_ID} -g nvdla nvdla &&\
    install -d -m 0755 -o nvdla -g nvdla /home/nvdla\
;fi
        
USER nvdla
