include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

set(cdentities_STAT_SRCS
    entity.cpp
    genotype.cpp
    genotypes.cpp
    critterb.cpp
    food.cpp
    archbodypart.cpp
    archconstraint.cpp
    archmouth.cpp
    bodypart.cpp
    constraint.cpp
    mouth.cpp
    bodyarch.cpp
    body.cpp
    wall.cpp
    worldb.cpp
    critterselection.cpp
    dirlayout.cpp
    displaylists.cpp
    statsbuffer.cpp
)

add_library(cdentities STATIC ${cdentities_STAT_SRCS})
target_link_libraries( cdentities filesystem )
