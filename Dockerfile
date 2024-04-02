FROM ubuntu:22.04 AS builder

RUN apt-get update \
        && apt-get install -y g++-11 \
        && apt-get install -y cmake \
        && apt-get install -y git \
        && apt-get install -y autoconf \
        && apt-get install -y libtool \
        && apt-get install -y zlib1g-dev

WORKDIR /workspace

ARG SSH_KEY

RUN mkdir -p /root/.ssh/ && \
        echo "$SSH_KEY" > /root/.ssh/id_rsa && \
        chmod -R 600 /root/.ssh/ && \
        ssh-keyscan -t rsa github.com >> ~/.ssh/known_hosts

ENV CXX=/usr/bin/g++-11

ENV LD_LIBRARY_PATH /usr/local/lib:${LD_LIBRARY_PATH}

COPY . .

RUN mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo && make -j && make install

FROM ubuntu:22.04

WORKDIR /app

RUN mkdir logs

ENV LD_LIBRARY_PATH /app:${LD_LIBRARY_PATH}

COPY --from=0 /usr/local/bin/descartes_server .
COPY --from=0 /usr/local/lib/libgflags.so* .
COPY --from=0 /usr/local/lib/libglog.so* .
COPY --from=0 /usr/local/lib/libprotobuf.so* .
COPY --from=0 /workspace/lib/libdescartes.so .

ENTRYPOINT ["./descartes_server"]

