FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# Yocto mandatory packages for Ubuntu, list taken from:
# https://docs.yoctoproject.org/brief-yoctoprojectqs/index.html
# and added 'tmux' to allow 'devshell' tasks
RUN apt-get update && apt-get install -y --no-install-recommends \
	bash \
	build-essential \
	chrpath \
	cpio \
	debianutils \
	diffstat \
	file \
	gawk \
	gcc \
	git \
	iputils-ping \
	libacl1 \
	libegl1-mesa \
	liblz4-tool \
	libsdl1.2-dev \
	locales \
	mesa-common-dev \
	python3 \
	python3-git \
	python3-jinja2 \
	python3-pexpect \
	python3-pip \
	python3-subunit \
	socat \
	texinfo \
	tmux \
	unzip \
	wget \
	xz-utils \
	zstd \
	&& rm -rf /var/lib/apt/lists/*

# Other useful packages
RUN apt-get update && apt-get install -y --no-install-recommends \
	asciidoc \
	bc \
	bison \
	ca-certificates \
	curl \
	flex \
	gir1.2-gtk-3.0 \
	libncurses5-dev \
	libssl-dev \
	poppler-utils \
	python-is-python3 \
	python3-gi \
	rsync \
	ruby \
	ssh \
	sshpass \
	subversion \
	sudo \
	swig \
	xterm \
	xxd \
	zip \
	&& rm -rf /var/lib/apt/lists/*

RUN sed -i "s/# en_US\.UTF-8 UTF-8/en_US\.UTF-8 UTF-8/" /etc/locale.gen \
	&& locale-gen \
	&& dpkg-reconfigure locales
