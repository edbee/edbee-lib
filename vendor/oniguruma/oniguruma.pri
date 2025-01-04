# !win32:system($$PWD/oniguruma/configure)
!win32:system("cp -Rp $$PWD/patch/* $$PWD/oniguruma/")
## The line below doesn't work because autoreconf is not in the default path
# !win32:system("cd $$PWD/oniguruma; autoreconf -vfi; ./configure; cd -")
!win32:system("cd $$PWD/oniguruma; ./configure; cd -")

win32 {
    edbee_xcopy_command.target = edbee_xcopy_files
    edbee_xcopy_command.commands = xcopy /s /e /Y /I $$shell_quote($$shell_path($$PWD/patch/*)) $$shell_quote($$shell_path($$PWD/oniguruma/)) 
    edbee_xcopy_command.commands += && copy $$shell_quote($$shell_path($$PWD/oniguruma/src/config.h.windows.in)) $$shell_quote($$shell_path($$PWD/oniguruma/src/config.h))
    PRE_TARGETDEPS += edbee_xcopy_files
    QMAKE_EXTRA_TARGETS += edbee_xcopy_command
}

#win32-g++:contains(QMAKE_HOST.arch, x86_64) {
#    # 64-bit Windows-specific code
#    message("Building for 64-bit Windows")
#    edbee_xcopy_command.commands += && copy $$shell_quote($$shell_path($$PWD/config/config.h.win64)) $$shell_quote($$shell_path($$PWD/oniguruma/src/config.h))
#}
#else:win32-g++:contains(QMAKE_HOST.arch, x86) {
#    # 32-bit Windows-specific code
#    message("Building for 32-bit Windows")
#    edbee_xcopy_command.commands += && copy $$shell_quote($$shell_path($$PWD/config/config.h.win32)) $$shell_quote($$shell_path($$PWD/oniguruma/src/config.h))
#}

INCLUDEPATH += $$PWD/oniguruma/src/

SOURCES += $$PWD/oniguruma/src/regint.h \
           $$PWD/oniguruma/src/regparse.h \
           $$PWD/oniguruma/src/regenc.h \
           $$PWD/oniguruma/src/st.h \
           $$PWD/oniguruma/src/regerror.c \
           $$PWD/oniguruma/src/regparse.c \
           $$PWD/oniguruma/src/regext.c \
           $$PWD/oniguruma/src/regcomp.c \
           $$PWD/oniguruma/src/regexec.c \
           $$PWD/oniguruma/src/reggnu.c \
           $$PWD/oniguruma/src/regenc.c \
           $$PWD/oniguruma/src/regsyntax.c \
           $$PWD/oniguruma/src/regtrav.c \
           $$PWD/oniguruma/src/regversion.c \
           $$PWD/oniguruma/src/st.c \
           $$PWD/oniguruma/src/onig_init.c \
           $$PWD/oniguruma/src/unicode.c \
           $$PWD/oniguruma/src/ascii.c \
           $$PWD/oniguruma/src/utf8.c \
           $$PWD/oniguruma/src/utf16_be.c \
           $$PWD/oniguruma/src/utf16_le.c \
           $$PWD/oniguruma/src/utf32_be.c \
           $$PWD/oniguruma/src/utf32_le.c \
           $$PWD/oniguruma/src/euc_jp.c \
           $$PWD/oniguruma/src/sjis.c \
           $$PWD/oniguruma/src/iso8859_1.c \
           $$PWD/oniguruma/src/iso8859_2.c \
           $$PWD/oniguruma/src/iso8859_3.c \
           $$PWD/oniguruma/src/iso8859_4.c \
           $$PWD/oniguruma/src/iso8859_5.c \
           $$PWD/oniguruma/src/iso8859_6.c \
           $$PWD/oniguruma/src/iso8859_7.c \
           $$PWD/oniguruma/src/iso8859_8.c \
           $$PWD/oniguruma/src/iso8859_9.c \
           $$PWD/oniguruma/src/iso8859_10.c \
           $$PWD/oniguruma/src/iso8859_11.c \
           $$PWD/oniguruma/src/iso8859_13.c \
           $$PWD/oniguruma/src/iso8859_14.c \
           $$PWD/oniguruma/src/iso8859_15.c \
           $$PWD/oniguruma/src/iso8859_16.c \
           $$PWD/oniguruma/src/euc_tw.c \
           $$PWD/oniguruma/src/euc_kr.c \
           $$PWD/oniguruma/src/big5.c \
           $$PWD/oniguruma/src/gb18030.c \
           $$PWD/oniguruma/src/koi8_r.c \
           $$PWD/oniguruma/src/cp1251.c \
           $$PWD/oniguruma/src/euc_jp_prop.c \
           $$PWD/oniguruma/src/sjis_prop.c \
           $$PWD/oniguruma/src/unicode_unfold_key.c \
           $$PWD/oniguruma/src/unicode_fold1_key.c \
           $$PWD/oniguruma/src/unicode_fold2_key.c \
           $$PWD/oniguruma/src/unicode_fold3_key.c

HEADERS += $$PWD/oniguruma/src/oniguruma.h \
           $$PWD/oniguruma/src/oniggnu.h \
           $$PWD/oniguruma/src/config-oniguruma-edbee.h
