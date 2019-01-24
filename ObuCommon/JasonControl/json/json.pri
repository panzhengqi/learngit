OTHER_FILES += \
    json/sconscript

HEADERS += \
    json/writer.h \
    json/value.h \
    json/reader.h \
    json/json.h \
    json/json_batchallocator.h \
    json/forwards.h \
    json/features.h \
    json/config.h \
    json/autolink.h

SOURCES += \
    json/json_writer.cpp \
    json/json_valueiterator.inl \
    json/json_value.cpp \
    json/json_reader.cpp \
    json/json_internalmap.inl \
    json/json_internalarray.inl
