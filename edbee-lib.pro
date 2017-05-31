
TEMPLATE = subdirs

#QMAKE_CXXFLAGS+=-fsanitize=address -fsanitize=bound
#QMAKE_LFLAGS+=-fsanitize=address -fsanitize=bounds

onig_lib.subdir = vendor/onig

src_lib.subdir = edbee-lib
src_lib.depends = onig_lib

src_lib_test.subdir = edbee-test
src_lib_test.depends = src_lib onig_lib


SUBDIRS = \
        onig_lib \
	src_lib \
	src_lib_test

