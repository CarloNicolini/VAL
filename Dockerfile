FROM debian:bullseye-slim

RUN apt-get update -y
RUN apt-get install --no-install-recommends -y -q cmake make g++ clang-format doxygen graphviz mingw-w64 zip unzip
RUN apt-get clean && rm /var/lib/apt/lists/*_*

WORKDIR /val
COPY . .
RUN mkdir build && cd build && cmake .. && make -j
ENV PATH="/val/build/bin:${PATH}"
ENV LD_LIBRARY_PATH="/val/build/bin"

CMD ["/bin/bash"]