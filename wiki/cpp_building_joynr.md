# Building joynr C++
The following scripts found within the joynr repository can be used
to build joynr inside a docker container without having to install
anything on the build machine.

## Prerequisites
See [Building joynr Java and common components](java_building_joynr.md#Prerequisites).

## Building with Docker
joynr C++ can be built by executing the following commands:

```bash
sudo docker run --rm --sig-proxy -e DEV_UID="$(id -u)" \
    -v <FULL_PATH_TO_JOYNR_SOURCES>:/data/src \
    -v <FULL_PATH_TO_MAVEN_DIRECTORY>:/home/joynr/.m2 \
    -v <FULL_PATH_TO_BUILD_DIRECTORY>:/data/build \
    joynr-cpp-gcc \
    /data/src/docker/joynr-cpp-base/scripts/build/cpp-generate.sh

sudo docker run --rm --sig-proxy -e DEV_UID="$(id -u)" \
    -v <FULL_PATH_TO_JOYNR_SOURCES>:/data/src \
    -v <FULL_PATH_TO_MAVEN_DIRECTORY>:/home/joynr/.m2 \
    -v <FULL_PATH_TO_BUILD_DIRECTORY>:/data/build \
    joynr-cpp-gcc \
    /data/src/docker/joynr-cpp-base/scripts/build/cpp-clean-build.sh --jobs 4

sudo docker run --rm --sig-proxy -e DEV_UID="$(id -u)" \
    -e JOYNR_INSTALL_DIR=/data/build/joynr \
    -v <FULL_PATH_TO_JOYNR_SOURCES>:/data/src \
    -v <FULL_PATH_TO_MAVEN_DIRECTORY>:/home/joynr/.m2 \
    -v <FULL_PATH_TO_BUILD_DIRECTORY>:/data/build \
    joynr-cpp-gcc \
    /data/src/docker/joynr-cpp-base/scripts/build/cpp-radio-app.sh --jobs 4
```

As an alternative you can also use the joynr-cpp-clang docker image.

This will start the docker container **joynr-cpp-gcc** or **joynr-cpp-clang** and execute the scripts **docker/joynr-cpp-base/scripts/build/cpp-generate.sh** and **docker/joynr-cpp-base/scripts/build/cpp-clean-build.sh**. Optionally you can also run **docker/joynr-cpp-base/scripts/build/cpp-radio-app.sh** to build the C++ radio app example.

joynr uses Maven for generating C++ code. You can use your own local Maven repository within the docker container by replacing **&lt;FULL_PATH_TO_HOST_MAVEN_DIRECTORY&gt;** with the absolute path to your Maven repo. This will then be made available to the build job, mounted at **/home/joynr/.m2** in the docker container. As for building joynr Java, the path to the joynr sources **&lt;FULL_PATH_TO_JOYNR_SOURCES&gt;** has to be provided to be accessible from the docker container at **/data/src**. Additionally, a directory for the build results **&lt;FULL_PATH_TO_BUILD_DIRECTORY&gt;** has to be provided.

```--jobs 4``` restricts the number of jobs of the executed make calls to 4 (default is 8, which may lead to errors on too weak machines)

```-e JOYNR_INSTALL_DIR=/data/build/joynr``` in the second command makes the build results of the first execution available for the second script since they are necessary to build the radio app.

## Building natively in Mac OS X
Please consult the wiki: [Building joynr C++ in Mac OS X](cpp_building_joynr_mac.md).
