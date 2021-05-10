# Create a virtual environment with all tools installed
# ref: https://hub.docker.com/_/ubuntu
FROM ubuntu:20.04 AS env

#############
##  SETUP  ##
#############
RUN apt update -qq \
&& DEBIAN_FRONTEND=noninteractive apt install -yq \
 git pkg-config wget make cmake autoconf libtool zlib1g-dev gawk g++ curl subversion \
 lsb-release \
&& apt clean \
&& rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Swig Install
RUN apt-get update -qq \
&& apt-get install -yq swig \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

ENV TZ=America/Sao_Paulo
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

################
##  OR-TOOLS  ##
################
FROM env AS devel

ARG SRC_GIT_BRANCH
ENV SRC_GIT_BRANCH ${SRC_GIT_BRANCH:-master}
ARG SRC_GIT_SHA1
ENV SRC_GIT_SHA1 ${SRC_GIT_SHA1:-unknown}

# Download sources
# use SRC_GIT_SHA1 to modify the command
# i.e. avoid docker reusing the cache when new commit is pushed
WORKDIR /root
RUN git clone -b "${SRC_GIT_BRANCH}" --single-branch https://github.com/google/or-tools \
&& echo "sha1: $(cd or-tools && git rev-parse --verify HEAD)" \
&& echo "expected sha1: ${SRC_GIT_SHA1}"

# Build third parties
FROM devel AS third_party
WORKDIR /root/or-tools
RUN make detect && make third_party

# Build project
FROM third_party AS build
RUN make detect_cc && make cc && make install_cc

# Build Heuristic to be called by the API
COPY ./services /root/app/services
WORKDIR /root/app/services/heuristica
RUN make rebuild && make

# install python and pip
RUN apt-get update -qq \
&& apt-get install -yq python3 python3-pip \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# install fastapi and uvicorn
RUN pip3 install fastapi uvicorn

EXPOSE 3030

COPY ./app /root/app/app

WORKDIR /root/app

# start API on docker run
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "3030"]
