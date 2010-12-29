set (kstyle_skulpture_config_SOURCES sk_config.cpp)
kde4_add_ui_files (kstyle_skulpture_config_SOURCES sk_dialog.ui sk_preview1.ui sk_preview2.ui)

kde4_add_plugin (kstyle_skulpture_config ${kstyle_skulpture_config_SOURCES})
target_link_libraries (kstyle_skulpture_config ${KDE4_KDEUI_LIBS})
install (FILES skulpture.png DESTINATION ${DATA_INSTALL_DIR}/skulpture/pics)
install( FILES skulptureui.rc DESTINATION ${DATA_INSTALL_DIR}/skulpture)
install (TARGETS kstyle_skulpture_config DESTINATION ${PLUGIN_INSTALL_DIR})

