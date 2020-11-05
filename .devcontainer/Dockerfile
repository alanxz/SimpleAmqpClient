
# [Choice] Debian / Ubuntu version: debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT=buster
FROM mcr.microsoft.com/vscode/devcontainers/cpp:dev-${VARIANT}

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends lsb-release wget software-properties-common \
    && bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" \
    && apt-get -y install --no-install-recommends clang-format-11 clang-tidy-11 ninja-build rabbitmq-server \
      libssl-dev librabbitmq-dev libboost-dev libboost-chrono-dev libboost-system-dev
