FROM ann-benchmarks

RUN apt update
RUN apt install -y git cmake g++ python3 python3-setuptools python3-pip libblas-dev liblapack-dev
RUN pip3 install wheel pybind11==2.5.0

WORKDIR /home/app
RUN git clone https://github.com/01-ai/Descartes.git
RUN pip3 install Descartes/lib/descartes-*-linux_x86_64.whl
