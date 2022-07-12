FROM debian:9

RUN apt-get update && apt-get install -y \
        vim \
        emacs \
        kate \
        wget \
        gcc \
        dpkg-dev \
        cmake \
        g++ \
        binutils \
        libx11-dev \
        libxpm-dev \
        libxft-dev \
        libxext-dev \
        libpng-dev \
        libjpeg-dev \
        python \
        libssl-dev \
        gfortran \
        libpcre3-dev \
        xlibmesa-glu-dev \
        libglew1.5-dev \
        libftgl-dev \
        libmariadb-dev \
        libfftw3-dev \
        libcfitsio-dev \
        graphviz-dev \
        libavahi-compat-libdnssd-dev \
        libldap2-dev \
        python-dev \
        python-numpy-dev \
        libxml2-dev \
        libkrb5-dev \
        libgsl0-dev \
        r-base \
        qtwebengine5-dev \
        libexpat1-dev \
        libxerces-c-dev \
        qtbase5-dev \
        qt3d5-dev \
        libgl1-mesa-dev \
        libglu1-mesa-dev \
        libglw1-mesa-dev \
        libexpat1-dev \
        libxerces-c-dev \
        qtbase5-dev \
        qt3d5-dev \
        libgl1-mesa-dev \
        libglu1-mesa-dev \
        libglw1-mesa-dev \
        libxmu-dev

WORKDIR /opt

RUN useradd -m -d /home/user user
RUN chown user:user /opt
USER user

SHELL ["/bin/bash", "--login", "-c"]

RUN cd /opt &&\
        wget https://root.cern/download/root_v6.14.06.source.tar.gz &&\
        tar -xzvf root_v6.14.06.source.tar.gz &&\
        rm /opt/root_v6.14.06.source.tar.gz &&\
        mkdir root-6.14.06-build root-6.14.06-install &&\
        cd /opt/root-6.14.06-build/ &&\
        cmake -DCMAKE_INSTALL_PREFIX=/opt/root-6.14.06-install ../root-6.14.06 &&\
        make -j8 &&\
        make install &&\
        cd /opt &&\
        rm -rf /opt/root-6.14.06 &&\
        rm -rf /opt/root-6.14.06-build &&\
        cd /opt &&\
        wget https://geant4-data.web.cern.ch/releases/geant4.10.05.p01.tar.gz &&\
        tar -xzvf geant4.10.05.p01.tar.gz &&\
        rm /opt/geant4.10.05.p01.tar.gz &&\
        mkdir geant4.10.05.p01-build &&\
        cd geant4.10.05.p01-build &&\
        cmake \
            -DCMAKE_INSTALL_PREFIX=/opt/geant4.10.05.p01-install \
            -DGEANT4_INSTALL_DATA=ON \
            -DGEANT4_USE_GDML=ON \
            -DGEANT4_USE_OPENGL_X11=ON \
            -DGEANT4_USE_QT=ON \
            -DGEANT4_USE_RAYTRACER_X11=ON \
            ../geant4.10.05.p01 &&\
        make -j8 &&\
        make install &&\
        cd /opt &&\
        rm -rf /opt/geant4.10.05.p01-build &&\
        rm -rf /opt/geant4.10.05.p01 &&\
        echo "source /opt/root-6.14.06-install/bin/thisroot.sh" >> $HOME/.bashrc &&\
        echo "source /opt/geant4.10.05.p01-install/bin/geant4.sh" >> $HOME/.bashrc
