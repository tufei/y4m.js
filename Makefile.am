# Process with automake to produce Makefile.in

# NOTE:  utils and mpeg2enc MUST be built first so that libmjpegutils AND
#        libmpeg2encpp exist.  Then in the following Makefile.am files if
#        HAVE_ALTIVEC is defined libmpeg2encpp will be added to the list of
#        libraries dependencies.  All this because utils/altivec/libaltivec.la
#        contains routines which  which should have gone into the encoder
#        library libmpeg2encpp.  Sigh.  Feel free to do it differently if you
#        can come up with a better method.

EMCC = emcc

SUBDIRS = \
	utils \
	mpeg2enc \
	lavtools \
	aenc \
	mplex \
	scripts \
	docs \
	yuvcorrect \
	yuvscaler \
	yuvdenoise \
	yuvfilters \
	yuvdeinterlace \
	y4mdenoise \
	y4mscaler \
	y4munsharp \
        y4mutils \
	debian

#
# Add any non autoconf'd files here, extra readmes and other misc 
# info to be copied into the dist
#
EXTRA_DIST = \
	BUGS \
	CHANGES \
	HINTS \
	PLANS \
	README.AltiVec \
	README.DV \
	README.avilib \
	README.glav \
	README.lavpipe \
	README.transist \
	INSTALL \
	INSTALL.real \
	TODO \
	cpuinfo.sh \
	mjpeg_howto.txt \
	mjpegtools.spec \
	mjpegtools.spec.in \
	mjpegtools.pc \
	mjpegtools.pc.in \
	autogen.sh

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mjpegtools.pc

DISTCLEANFILES = \
	mjpegtools-config.h \
	mjpegtools.pc \
	confdefs.h \
	config.cache \
	config.status \
	config.log

MAINTAINERCLEANFILES = \
	compile \
	depcomp \
	install-sh \
	missing \
	mkinstalldirs \
	Makefile.in \
	aclocal.m4 \
	config.guess \
	config.h.in \
	config.sub \
	configure \
	ltmain.sh \
	stamp-h.in

EMCFLAGS = \
	-Os -Wall -MMD -I. -I./lavtools -I./utils \
	-fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections \
	-fno-math-errno -fno-signed-zeros -fno-tree-vectorize -fomit-frame-pointer \
	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_REENTRANT

# Emscripten config
EMLDFLAGS = \
	-s "EXPORTED_FUNCTIONS=['_y4m_accept_extensions', '_y4m_init_stream_info', '_y4m_init_frame_info', '_y4m_read_stream_header_cb', '_y4m_fini_stream_info', '_y4m_fini_frame_info', '_y4m_js_open_cb_reader', '_y4m_js_close_cb_reader', '_y4m_js_decode_frame', '_malloc','_free']" \
	-s NO_FILESYSTEM=1 -s NO_BROWSER=1 \
	-O3 --memory-init-file 0 --closure 1 --post-js post.js

Y4M_JS_OBJECTS = \
	utils/mjpeg_logging.js.o \
	utils/mpegconsts.js.o \
	utils/mpegtimecode.js.o \
	utils/yuv4mpeg.js.o \
	utils/yuv4mpeg_ratio.js.o \
	lavtools/colorspace.js.o \
	lavtools/subsample.js.o \
	y4m_js.js.o \
	tmalloc.js.o

y4m.js: $(Y4M_JS_OBJECTS) post.js
	$(EMCC) $(EMLDFLAGS) -s TOTAL_MEMORY=33554432 -o $@ $(Y4M_JS_OBJECTS)

SUFFIXES = .js.o

.c.js.o:
	$(EMCC) $(EMCFLAGS) -c -o $@ $<

## make rpms
rpm: Makefile
	$(MAKE) dist
	rpmbuild -ta --clean $(PACKAGE)-$(VERSION).tar.gz

## make debs
deb: Makefile dist
	-chmod -R +w $(PACKAGE)-$(VERSION)
	rm -rf $(PACKAGE)-$(VERSION)
	tar xzf $(PACKAGE)-$(VERSION).tar.gz
	cd $(PACKAGE)-$(VERSION); dpkg-buildpackage -rfakeroot
