project (kwin-skulpture)

set (kwin_skulpture_SOURCES kwin_skulpture.cpp)
kde4_add_plugin (kwin3_skulpture ${kwin_skulpture_SOURCES})
target_link_libraries (kwin3_skulpture ${KDE4_KDEUI_LIBS} ${KDE4WORKSPACE_KDECORATIONS_LIBS})
install (TARGETS kwin3_skulpture DESTINATION ${PLUGIN_INSTALL_DIR})
install (FILES skulpture.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin)

add_subdirectory (config)

