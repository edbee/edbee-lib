!win32:system($$PWD/configure)
INCLUDEPATH += $$PWD/src

SOURCES += $$PWD/src/regint.h \
           $$PWD/src/regparse.h \
           $$PWD/src/regenc.h \
           $$PWD/src/st.h \
           $$PWD/src/regerror.c \
           $$PWD/src/regparse.c \
           $$PWD/src/regext.c \
           $$PWD/src/regcomp.c \
           $$PWD/src/regexec.c \
           $$PWD/src/reggnu.c \
           $$PWD/src/regenc.c \
           $$PWD/src/regsyntax.c \
           $$PWD/src/regtrav.c \
           $$PWD/src/regversion.c \
           $$PWD/src/st.c \
           $$PWD/src/onig_init.c \
           $$PWD/src/unicode.c \
           $$PWD/src/ascii.c \
           $$PWD/src/utf8.c \
           $$PWD/src/utf16_be.c \
           $$PWD/src/utf16_le.c \
           $$PWD/src/utf32_be.c \
           $$PWD/src/utf32_le.c \
           $$PWD/src/euc_jp.c \
           $$PWD/src/sjis.c \
           $$PWD/src/iso8859_1.c \
           $$PWD/src/iso8859_2.c \
           $$PWD/src/iso8859_3.c \
           $$PWD/src/iso8859_4.c \
           $$PWD/src/iso8859_5.c \
           $$PWD/src/iso8859_6.c \
           $$PWD/src/iso8859_7.c \
           $$PWD/src/iso8859_8.c \
           $$PWD/src/iso8859_9.c \
           $$PWD/src/iso8859_10.c \
           $$PWD/src/iso8859_11.c \
           $$PWD/src/iso8859_13.c \
           $$PWD/src/iso8859_14.c \
           $$PWD/src/iso8859_15.c \
           $$PWD/src/iso8859_16.c \
           $$PWD/src/euc_tw.c \
           $$PWD/src/euc_kr.c \
           $$PWD/src/big5.c \
           $$PWD/src/gb18030.c \
           $$PWD/src/koi8_r.c \
           $$PWD/src/cp1251.c \
           $$PWD/src/euc_jp_prop.c \
           $$PWD/src/sjis_prop.c \
           $$PWD/src/unicode_unfold_key.c \
           $$PWD/src/unicode_fold1_key.c \
           $$PWD/src/unicode_fold2_key.c \
           $$PWD/src/unicode_fold3_key.c



HEADERS += $$PWD/src/oniguruma.h \
           $$PWD/src/oniggnu.h

