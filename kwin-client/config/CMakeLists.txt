set (kwin_skulpture_config_SOURCES kwin_skulpture_config.cpp)
kde4_add_ui_files(kwin_skulpture_config_SOURCES kwin_skulpture_dialog.ui)

kde4_add_plugin (kwin_skulpture_config ${kwin_skulpture_config_SOURCES})
target_link_libraries (kwin_skulpture_config ${KDE4_KDEUI_LIBS})
install (TARGETS kwin_skulpture_config DESTINATION ${PLUGIN_INSTALL_DIR})

