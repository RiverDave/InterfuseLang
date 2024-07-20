FROM silkeh/clang:latest AS build

# Install the typical development tools for C++, and
# the base OS headers and libraries.
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        curl \
        git \
        python3 \
        linux-libc-dev \
        ninja-build \
        pkg-config \
        tar \
        unzip \
        zip \
        bison \
        libzstd-dev \
        zlib1g-dev \ 
        libcurl4-openssl-dev \
        libedit-dev \
        autoconf \
        automake \
        autoconf-archive \
        libboost-all-dev




# WORKDIR /usr/local/vcpkg
# ENV VCPKG_FORCE_SYSTEM_BINARIES=1
# RUN curl -sSL "https://github.com/Microsoft/vcpkg/archive/2024.04.26.tar.gz" | \
#     tar --strip-components=1 -zxf - \
#     && ./bootstrap-vcpkg.sh -disableMetrics

# Copy the source code to /v/source and compile it.
COPY . /v/source
WORKDIR /v/source

#RUN /usr/local/vcpkg/vcpkg install

# Run the CMake configuration step, setting the options to create
# a statically linked C++ program
RUN cmake -S/v/source -B/v/build -GNinja \
    # -DCMAKE_TOOLCHAIN_FILE=/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_BUILD_TYPE=DEBUG

# Compile the binary and strip it to reduce its size.
RUN cmake --build /v/build
RUN strip /v/build/interfuse

# Create the final deployment image, using `scratch` (the empty Docker image)
# as the starting point. Effectively we create an image that only contains
# our program.
FROM silkeh/clang:latest AS fusec
WORKDIR /r

# Copy the program from the previously created stage and the shared libraries it
# depends on.
COPY --from=build /v/build/interfuse /r


EXPOSE 8080
# Make the program the entry point.
# "/r/examples/complex.fuse", "-v", "new"
ENTRYPOINT [ "/r/interfuse"]
