
TEMPLATE = subdirs

EDBEE_SANITIZE = $$(EDBEE_SANITIZE)
!isEmpty( EDBEE_SANITIZE ) {
  QMAKE_CXXFLAGS+=-fsanitize=address -fsanitize=bound -fsanitize-undefined-trap-on-error
  QMAKE_LFLAGS+=-fsanitize=address -fsanitize=bounds -fsanitize-undefined-trap-on-error
}

src_lib.subdir = edbee-lib

src_lib_test.subdir = edbee-test
src_lib_test.depends = src_lib


SUBDIRS = \
	src_lib \
	src_lib_test

