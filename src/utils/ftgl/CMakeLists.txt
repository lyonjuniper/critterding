
#include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )
include_directories( ${bengine_INCLUDES} )

########### next target ###############

set(ftgl_STAT_SRCS
    FTBuffer.cpp
    FTCharmap.cpp
    FTCharmap.h
    FTCharToGlyphIndexMap.h
    FTContour.cpp
    FTContour.h
    FTFace.cpp
    FTFace.h
    FTGlyphContainer.cpp
    FTGlyphContainer.h
    FTInternals.h
    FTLibrary.cpp
    FTLibrary.h
    FTList.h
    FTPoint.cpp
    FTSize.cpp
    FTSize.h
    FTVector.h
    FTVectoriser.cpp
    FTVectoriser.h
    FTUnicode.h
   FTGlyph/FTGlyph.cpp
   FTGlyph/FTGlyphImpl.h
   FTGlyph/FTGlyphGlue.cpp
   FTGlyph/FTBitmapGlyph.cpp
   FTGlyph/FTBitmapGlyphImpl.h
   FTGlyph/FTBufferGlyph.cpp
   FTGlyph/FTBufferGlyphImpl.h
   FTGlyph/FTExtrudeGlyph.cpp
   FTGlyph/FTExtrudeGlyphImpl.h
   FTGlyph/FTOutlineGlyph.cpp
   FTGlyph/FTOutlineGlyphImpl.h
   FTGlyph/FTPixmapGlyph.cpp
   FTGlyph/FTPixmapGlyphImpl.h
   FTGlyph/FTPolygonGlyph.cpp
   FTGlyph/FTPolygonGlyphImpl.h
   FTGlyph/FTTextureGlyph.cpp
   FTGlyph/FTTextureGlyphImpl.h
   FTFont/FTFont.cpp
   FTFont/FTFontImpl.h
   FTFont/FTFontGlue.cpp
   FTFont/FTBitmapFont.cpp
   FTFont/FTBitmapFontImpl.h
   FTFont/FTBufferFont.cpp
   FTFont/FTBufferFontImpl.h
   FTFont/FTExtrudeFont.cpp
   FTFont/FTExtrudeFontImpl.h
   FTFont/FTOutlineFont.cpp
   FTFont/FTOutlineFontImpl.h
   FTFont/FTPixmapFont.cpp
   FTFont/FTPixmapFontImpl.h
   FTFont/FTPolygonFont.cpp
   FTFont/FTPolygonFontImpl.h
   FTFont/FTTextureFont.cpp
   FTFont/FTTextureFontImpl.h
   FTLayout/FTLayout.cpp
   FTLayout/FTLayoutImpl.h
   FTLayout/FTLayoutGlue.cpp
   FTLayout/FTSimpleLayout.cpp
   FTLayout/FTSimpleLayoutImpl.h
   FTGL/ftgl.h
   FTGL/FTBBox.h
   FTGL/FTBuffer.h
   FTGL/FTPoint.h
   FTGL/FTGlyph.h
   FTGL/FTBitmapGlyph.h
   FTGL/FTBufferGlyph.h
   FTGL/FTExtrdGlyph.h
   FTGL/FTOutlineGlyph.h
   FTGL/FTPixmapGlyph.h
   FTGL/FTPolyGlyph.h
   FTGL/FTTextureGlyph.h
   FTGL/FTFont.h
   FTGL/FTGLBitmapFont.h
   FTGL/FTBufferFont.h
   FTGL/FTGLExtrdFont.h
   FTGL/FTGLOutlineFont.h
   FTGL/FTGLPixmapFont.h
   FTGL/FTGLPolygonFont.h
   FTGL/FTGLTextureFont.h
   FTGL/FTLayout.h
   FTGL/FTSimpleLayout.h )

add_library(ftgl STATIC ${ftgl_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#noinst_LTLIBRARIES = libftgl.la
#noinst_HEADERS = $(ftgl_headers)
#
#libftgl_la_CPPFLAGS = -IFTGlyph -IFTFont -IFTLayout
#libftgl_la_LDFLAGS = -avoid-version -no-undefined
#
##libftgl_la_LIBADD = -l
#
#libftgl_la_SOURCES = \
#    FTBuffer.cpp \
#    FTCharmap.cpp \
#    FTCharmap.h \
#    FTCharToGlyphIndexMap.h \
#    FTContour.cpp \
#    FTContour.h \
#    FTFace.cpp \
#    FTFace.h \
#    FTGlyphContainer.cpp \
#    FTGlyphContainer.h \
#    FTInternals.h \
#    FTLibrary.cpp \
#    FTLibrary.h \
#    FTList.h \
#    FTPoint.cpp \
#    FTSize.cpp \
#    FTSize.h \
#    FTVector.h \
#    FTVectoriser.cpp \
#    FTVectoriser.h \
#    FTUnicode.h \
#    $(ftglyph_sources) \
#    $(ftfont_sources) \
#    $(ftlayout_sources) \
#    $(ftgl_headers) \
#    $(NULL)
#
#ftgl_headers = \
#    FTGL/ftgl.h \
#    FTGL/FTBBox.h \
#    FTGL/FTBuffer.h \
#    FTGL/FTPoint.h \
#    FTGL/FTGlyph.h \
#    FTGL/FTBitmapGlyph.h \
#    FTGL/FTBufferGlyph.h \
#    FTGL/FTExtrdGlyph.h \
#    FTGL/FTOutlineGlyph.h \
#    FTGL/FTPixmapGlyph.h \
#    FTGL/FTPolyGlyph.h \
#    FTGL/FTTextureGlyph.h \
#    FTGL/FTFont.h \
#    FTGL/FTGLBitmapFont.h \
#    FTGL/FTBufferFont.h \
#    FTGL/FTGLExtrdFont.h \
#    FTGL/FTGLOutlineFont.h \
#    FTGL/FTGLPixmapFont.h \
#    FTGL/FTGLPolygonFont.h \
#    FTGL/FTGLTextureFont.h \
#    FTGL/FTLayout.h \
#    FTGL/FTSimpleLayout.h \
#    ${NULL}
#
#ftglyph_sources = \
#    FTGlyph/FTGlyph.cpp \
#    FTGlyph/FTGlyphImpl.h \
#    FTGlyph/FTGlyphGlue.cpp \
#    FTGlyph/FTBitmapGlyph.cpp \
#    FTGlyph/FTBitmapGlyphImpl.h \
#    FTGlyph/FTBufferGlyph.cpp \
#    FTGlyph/FTBufferGlyphImpl.h \
#    FTGlyph/FTExtrudeGlyph.cpp \
#    FTGlyph/FTExtrudeGlyphImpl.h \
#    FTGlyph/FTOutlineGlyph.cpp \
#    FTGlyph/FTOutlineGlyphImpl.h \
#    FTGlyph/FTPixmapGlyph.cpp \
#    FTGlyph/FTPixmapGlyphImpl.h \
#    FTGlyph/FTPolygonGlyph.cpp \
#    FTGlyph/FTPolygonGlyphImpl.h \
#    FTGlyph/FTTextureGlyph.cpp \
#    FTGlyph/FTTextureGlyphImpl.h \
#    $(NULL)
#
#ftfont_sources = \
#    FTFont/FTFont.cpp \
#    FTFont/FTFontImpl.h \
#    FTFont/FTFontGlue.cpp \
#    FTFont/FTBitmapFont.cpp \
#    FTFont/FTBitmapFontImpl.h \
#    FTFont/FTBufferFont.cpp \
#    FTFont/FTBufferFontImpl.h \
#    FTFont/FTExtrudeFont.cpp \
#    FTFont/FTExtrudeFontImpl.h \
#    FTFont/FTOutlineFont.cpp \
#    FTFont/FTOutlineFontImpl.h \
#    FTFont/FTPixmapFont.cpp \
#    FTFont/FTPixmapFontImpl.h \
#    FTFont/FTPolygonFont.cpp \
#    FTFont/FTPolygonFontImpl.h \
#    FTFont/FTTextureFont.cpp \
#    FTFont/FTTextureFontImpl.h \
#    $(NULL)
#
#ftlayout_sources = \
#    FTLayout/FTLayout.cpp \
#    FTLayout/FTLayoutImpl.h \
#    FTLayout/FTLayoutGlue.cpp \
#    FTLayout/FTSimpleLayout.cpp \
#    FTLayout/FTSimpleLayoutImpl.h \
#    $(NULL)
#
#NULL =