
SET(ONIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/oniguruma")

SET(ONIG_INCLUDE_DIRS
  ${ONIG_DIR}/src/

  PARENT_SCOPE
)

SET(ONIG_SOURCES
  ${ONIG_DIR}/src/regint.h
  ${ONIG_DIR}/src/regparse.h
  ${ONIG_DIR}/src/regenc.h
  ${ONIG_DIR}/src/st.h
  ${ONIG_DIR}/src/regerror.c
  ${ONIG_DIR}/src/regparse.c
  ${ONIG_DIR}/src/regext.c
  ${ONIG_DIR}/src/regcomp.c
  ${ONIG_DIR}/src/regexec.c
  ${ONIG_DIR}/src/reggnu.c
  ${ONIG_DIR}/src/regenc.c
  ${ONIG_DIR}/src/regsyntax.c
  ${ONIG_DIR}/src/regtrav.c
  ${ONIG_DIR}/src/regversion.c
  ${ONIG_DIR}/src/st.c
  ${ONIG_DIR}/src/onig_init.c
  ${ONIG_DIR}/src/unicode.c
  ${ONIG_DIR}/src/ascii.c
  ${ONIG_DIR}/src/utf8.c
  ${ONIG_DIR}/src/utf16_be.c
  ${ONIG_DIR}/src/utf16_le.c
  ${ONIG_DIR}/src/utf32_be.c
  ${ONIG_DIR}/src/utf32_le.c
  ${ONIG_DIR}/src/euc_jp.c
  ${ONIG_DIR}/src/sjis.c
  ${ONIG_DIR}/src/iso8859_1.c
  ${ONIG_DIR}/src/iso8859_2.c
  ${ONIG_DIR}/src/iso8859_3.c
  ${ONIG_DIR}/src/iso8859_4.c
  ${ONIG_DIR}/src/iso8859_5.c
  ${ONIG_DIR}/src/iso8859_6.c
  ${ONIG_DIR}/src/iso8859_7.c
  ${ONIG_DIR}/src/iso8859_8.c
  ${ONIG_DIR}/src/iso8859_9.c
  ${ONIG_DIR}/src/iso8859_10.c
  ${ONIG_DIR}/src/iso8859_11.c
  ${ONIG_DIR}/src/iso8859_13.c
  ${ONIG_DIR}/src/iso8859_14.c
  ${ONIG_DIR}/src/iso8859_15.c
  ${ONIG_DIR}/src/iso8859_16.c
  ${ONIG_DIR}/src/euc_tw.c
  ${ONIG_DIR}/src/euc_kr.c
  ${ONIG_DIR}/src/big5.c
  ${ONIG_DIR}/src/gb18030.c
  ${ONIG_DIR}/src/koi8_r.c
  ${ONIG_DIR}/src/cp1251.c
  ${ONIG_DIR}/src/euc_jp_prop.c
  ${ONIG_DIR}/src/sjis_prop.c
  ${ONIG_DIR}/src/unicode_unfold_key.c
  ${ONIG_DIR}/src/unicode_fold1_key.c
  ${ONIG_DIR}/src/unicode_fold2_key.c
  ${ONIG_DIR}/src/unicode_fold3_key.c

  PARENT_SCOPE
)

SET(ONIG_HEADERS
  ${ONIG_DIR}/src/oniguruma.h
  ${ONIG_DIR}/src/oniggnu.h

  PARENT_SCOPE
)
