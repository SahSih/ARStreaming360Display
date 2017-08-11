#!/bin/bash
#########################################
#
# Install OpenCV Latest from git
#
#########################################

# Install Dependencies for OpenCV
apt-get update && apt-get upgrade -y
apt-get install -y build-essential
apt-get install -y build-dep
apt-get install -y opencv
apt-get install -y git
apt-get install -y cmake
apt-get install -y g++
apt-get install -y nano
apt-get install -y vim
apt-get install -y libopencv-dev
apt-get install -y checkinstall
apt-get install -y pkg-config
apt-get install -y libgtk2.0-dev
apt-get install -y libjpeg-dev
apt-get install -y libpng12-dev
apt-get install -y libtiff4-dev
apt-get install -y libtiff5-dev
apt-get install -y libxine-dev
apt-get install -y libjasper-dev
apt-get install -y libopenexr-dev
apt-get install -y libtbb2
apt-get install -y libtbb-dev
apt-get install -y libeigen3-dev
apt-get install -y yasm
apt-get install -y libfaac-dev
apt-get install -y libopencore-amrnb-dev
apt-get install -y libopencore-amrwb-dev
apt-get install -y libtheora-dev
apt-get install -y libvorbis-dev
apt-get install -y libxvidcore-dev
apt-get install -y libx264-dev
apt-get install -y libqt4-dev
apt-get install -y libqt4-opengl-dev
apt-get install -y libpng-dev
apt-get install -y libtiff-dev
apt-get install -y sphinx-common
apt-get install -y texlive-latex-extra
apt-get install -y libdc1394-22-dev
apt-get install -y libxine2-dev
apt-get install -y libgstreamer0.10-dev
apt-get install -y libgstreamer-plugins-base0.10-dev
apt-get install -y libqt5x11extras5
apt-get install -y libqt5opengl5
apt-get install -y libqt5opengl5-dev
apt-get install -y libavcodec-dev
apt-get install -y libavformat-dev
apt-get install -y libswscale-dev
apt-get install -y libswresample-dev
apt-get install -y libmp3lame-dev
apt-get install -y v4l-utils
apt-get install -y unzip
apt-get install -y ffmpeg
apt-get install -y default-jdk
apt-get install -y python-dev python-numpy python-tk
apt-get install -y python3-dev python3-pip python3-tk python3-lxml python3-six
apt-get install -y python-opencv
apt-get install -y libjpeg8-dev
apt-get install -y libatlas-base-dev
apt-get install -y gfortran
apt-get install -y gstreamer1.0*
apt-get install -y libgstreamer1.0*
apt-get install -y libgstreamer-plugins-base1.0-dev
apt-get install -y libavresample*
apt-get install -y libgphoto2*
apt-get install -y libopenblas-dev
apt-get install -y liblapacke-dev
apt-get install -y doxygen
apt-get install -y libvtk6-dev
apt-get install -y mencoder
add-apt-repository --yes ppa:xqms/opencv-nonfree && apt-get update
apt-get install libopencv-nonfree-dev

apt-get install -y ccache
/bin/sh -c "echo 'export PATH=/usr/lib/ccache:$PATH' >> ~/.bashrc"
source ~/.bashrc && echo $PATH

apt-get install -y libv4l-dev
cd /usr/include/linux
ln -s ../libv4l1-videodev.h videodev.h

cd ~

# Install OpenCV and OpenCV_Contrib Latest from Github Repo
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
cd opencv && mkdir build && cd build

cmake -D CMAKE_BUILD_TYPE=RELEASE \
-D CMAKE_INSTALL_PREFIX=/usr/local \
-D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules \
-D WITH_TBB=ON \
-D BUILD_NEW_PYTHON_SUPPORT=ON \
-D WITH_V4L=ON \
-D WITH_LIBV4L=OFF \
-D WITH_V4L2=ON \
-D WITH_LIBV4L2=OFF \
-D WITH_MATLAB=OFF \
-D INSTALL_C_EXAMPLES=ON \
-D INSTALL_PYTHON_EXAMPLES=ON \
-D BUILD_EXAMPLES=ON \
-D WITH_FFMPEG=ON \
-D WITH_QT=ON \
-D WITH_OPENGL=ON ..

make -j7
make install
/bin/sh -c "echo '/usr/local/lib' >> /etc/ld.so.conf.d/opencv.conf"
ldconfig

/bin/sh -c "echo 'PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig' >> /etc/bash.bashrc"
/bin/sh -c "echo 'export PKG_CONFIG_PATH' >> /etc/bash.bashrc"
pkg-config --cflags opencv
pkg-config --libs opencv
