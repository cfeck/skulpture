project (kstyle-skulpture)

set (skulpture_SOURCES
    sk_animations.cpp
    sk_arrows.cpp
    sk_buttons.cpp
    sk_cache.cpp
    sk_color.cpp
    sk_combobox.cpp
    sk_complex.cpp
    sk_dial.cpp
    sk_dock.cpp
    sk_factory.cpp
    sk_frames.cpp
    sk_gradients.cpp
    sk_header.cpp
    sk_icons.cpp
    sk_layout.cpp
    sk_mdi.cpp
    sk_menu.cpp
    sk_misc.cpp
    sk_progressbar.cpp
    sk_scrollbar.cpp
    sk_shadows.cpp
    sk_shapes.cpp
    sk_shortcuts.cpp
    sk_slider.cpp
    sk_spinbox.cpp
    sk_tabs.cpp
    sk_text.cpp
    sk_toolbar.cpp
    skulpture.cpp
    skulpture_p.cpp
)

#if (KDE4_FOUND)
#	kde4_add_plugin (skulpture ${skulpture_SOURCES})
#	target_link_libraries (skulpture ${KDE4_KDEUI_LIBS})
#	install (TARGETS skulpture DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles)
#else (KDE4_FOUND)
	qt4_automoc (${skulpture_SOURCES})
	add_library (skulpture SHARED ${skulpture_SOURCES})
	target_link_libraries (skulpture ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
	install (TARGETS skulpture LIBRARY DESTINATION ${QT_PLUGINS_DIR}/styles)
#endif (KDE4_FOUND)

if (KDE4_FOUND)
	install (FILES skulpture.themerc DESTINATION ${DATA_INSTALL_DIR}/kstyle/themes)
	add_subdirectory (config)
endif (KDE4_FOUND)

