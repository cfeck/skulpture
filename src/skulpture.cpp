/*
 * Skulpture - Classical Three-Dimensional Artwork for Qt 4
 *
 * Copyright (c) 2007-2010 Christoph Feck <christoph@maxiom.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

// ### This file is a mess

#include "skulpture_p.h"
#include <QtGui/QLayout>
#include <QtGui/QLCDNumber>
#include <QtGui/QPainter>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QScrollBar>
#include <QtGui/QIcon>
#include <QtGui/QStatusBar>
#include <QtGui/QAbstractScrollArea>
#include <QtGui/QScrollArea>
#if (QT_VERSION >= QT_VERSION_CHECK(4, 3, 0))
#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>
#endif
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QWorkspace>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>
#include <QtGui/QTreeView>
#include <QtGui/QTableView>
#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtGui/QPainterPath>
#include <QtGui/QGroupBox>
#include <QtGui/QDockWidget>
#include <QtGui/QToolButton>
#include <QtGui/QTextEdit>
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
#include <QtGui/QPlainTextEdit>
#include <QtGui/QFormLayout>
#endif
#include <QtGui/QStackedLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QDial>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QToolBox>
#include <QtGui/QToolBar>
#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QDialog>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QKeyEvent>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <cstdio>
#include <QtCore/QDebug>

/*-----------------------------------------------------------------------*/

#include <QtGui/QStylePlugin>

class SkulptureStylePlugin : public QStylePlugin
{
	public:
		QStringList keys() const {
			return QStringList(QLatin1String("Skulpture"));
		}

		QStyle *create(const QString &key) {
			if (key.toLower() == QLatin1String("skulpture")) {
				return new SkulptureStyle;
			}
			return 0;
		}
};


Q_EXPORT_PLUGIN2(skulpture, SkulptureStylePlugin)


/*-----------------------------------------------------------------------*/

SkulptureStyle::SkulptureStyle()
	: d(new Private)
{
	d->q = this;
}


SkulptureStyle::~SkulptureStyle()
{
	delete d;
}


/*-----------------------------------------------------------------------*/

void SkulptureStyle::polish(QApplication *application)
{
//	ParentStyle::polish(application);
//	return;
#if (QT_VERSION >= QT_VERSION_CHECK(4, 2, 0))
    QString recursionCheck = QLatin1String("\n/* -skulpture-recursion-check- */\n");
    if (!d->styleSheetFileName.isEmpty()) {
        QString oldStyle = application->styleSheet();
        if (!oldStyle.contains(recursionCheck)) {
            QFile file(d->styleSheetFileName);
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream stream(&file);
                QString newStyle = stream.readAll();
                application->setStyleSheet(newStyle + recursionCheck + oldStyle);
            }
        }
    }
#endif
	ParentStyle::polish(application);
	application->installEventFilter(d->shortcut_handler);
#if 0
	QPalette palette;
	polish(palette);
	application->setPalette(palette);
#endif
//	if (application->inherits("KApplication")) {
//		qDebug() << "KApplication is a" << application->metaObject()->className() << "(" << "object name:" << application->objectName() << ")";
//	}
//        QFontMetrics fm = QFontMetrics(QFont());
//        printf("h:%d, s:%d, xh:%d, xb:%d, Xb:%d, Xyb: %d\n", fm.height(), fm.lineSpacing(), fm.xHeight(), fm.boundingRect(QChar('x', 0)).height(), fm.boundingRect(QChar('X', 0)).height(), fm.boundingRect(QLatin1String("Xy")).height());
}


void SkulptureStyle::unpolish(QApplication *application)
{
	application->removeEventFilter(d->shortcut_handler);
	ParentStyle::unpolish(application);
}


/*-----------------------------------------------------------------------*/

enum SidebarViewMode {
    DefaultSidebar,
    TransparentSidebar
};


static void polishSidebarView(QAbstractItemView *view, SidebarViewMode viewMode)
{
    QWidget *viewport = view->viewport();
    QPalette palette = view->palette();

    if (viewMode == TransparentSidebar) {
        if (viewport->autoFillBackground()) {
            viewport->setAutoFillBackground(false);
            QPalette::ColorRole textRole = viewport->foregroundRole();
            if (textRole != QPalette::WindowText) {
                palette.setBrush(QPalette::Active, textRole, palette.brush(QPalette::Active, QPalette::WindowText));
                palette.setBrush(QPalette::Inactive, textRole, palette.brush(QPalette::Inactive, QPalette::WindowText));
                palette.setBrush(QPalette::Disabled, textRole, palette.brush(QPalette::Disabled, QPalette::WindowText));
                viewport->setPalette(palette);
            }
        }
        view->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    } else {
        if (viewport->autoFillBackground()) {
            palette.setBrush(QPalette::Active, QPalette::Window, palette.brush(QPalette::Active, QPalette::Base));
            palette.setBrush(QPalette::Inactive, QPalette::Window, palette.brush(QPalette::Inactive, QPalette::Base));
            palette.setBrush(QPalette::Disabled, QPalette::Window, palette.brush(QPalette::Disabled, QPalette::Base));
        } else {
            viewport->setAutoFillBackground(true);
        }
        view->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        viewport->setPalette(palette);
    }
}


/*-----------------------------------------------------------------------*/

#if (QT_VERSION >= QT_VERSION_CHECK(4, 3, 0))
static WidgetShadow *findShadow(QWidget *widget)
{
	QWidget *parent = widget->parentWidget();
	if (parent) {
		QList<WidgetShadow *> shadows = parent->findChildren<WidgetShadow *>();

		Q_FOREACH (WidgetShadow *shadow, shadows) {
			if (shadow->widget() == widget) {
				return shadow;
			}
		}
	}
	return 0;
}
#endif

void SkulptureStyle::polish(QWidget *widget)
{
//	ParentStyle::polish(widget);
//	return;
#if 1
	//printf("polishing a \"%s\" (which is a \"%s\")\n", widget->metaObject()->className(), widget->metaObject()->superClass()->className());
#if 0
	QPalette palette = widget->palette();
	polish(palette);
	widget->setPalette(palette);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(4, 3, 0))
	if (QMdiArea *area = qobject_cast<QMdiArea *>(widget)) {
		area->installEventFilter(d);
	}
	if (qobject_cast<QMdiSubWindow *>(widget)) {
		WidgetShadow *shadow = findShadow(widget);
		if (!shadow) {
			widget->installEventFilter(d);
			if (widget->parentWidget()) {
				WidgetShadow *shadow = new WidgetShadow(widget->parentWidget());
				shadow->setWidget(widget);
				shadow->updateZOrder();
				shadow->show();
			}
		}
	}
#endif
#if 1
	if (QLCDNumber *lcd = qobject_cast<QLCDNumber *>(widget)) {
		QPalette palette;
		palette.setColor(QPalette::Base, QColor(220, 230, 210));
		palette.setColor(QPalette::WindowText, QColor(60, 60, 60));
	//	lcd->setPalette(palette);
	//	lcd->installEventFilter(d);
	//	lcd->setContentsMargins(8, 8, 8, 8);
		lcd->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
		lcd->setSegmentStyle(QLCDNumber::Flat);
	}
#endif
#if 1
	if (QDialog *dialog = qobject_cast<QDialog *>(widget)) {
		dialog->installEventFilter(d);
	}
#endif
#if 0
	if (QMainWindow *window = qobject_cast<QMainWindow *>(widget)) {
		window->setBackgroundRole(QPalette::Dark);
	}
	if (QDockWidget *dock = qobject_cast<QDockWidget *>(widget)) {
		dock->installEventFilter(d);
	}
	if (QStatusBar *bar = qobject_cast<QStatusBar *>(widget)) {
		bar->installEventFilter(d);
	}
#endif
	if (QToolBox *toolBox = qobject_cast<QToolBox *>(widget)) {
		toolBox->setBackgroundRole(QPalette::Window);
	//	toolBox->setContentsMargins(2, 2, 2, 2);
	//	toolBox->installEventFilter(d);
		toolBox->layout()->setSpacing(0);
	}
	if (widget->inherits("KTitleWidget")) {
//            widget->setMaximumHeight(0);
#if 0
		QPalette palette = widget->palette();
		palette.setColor(QPalette::Base, palette.color(QPalette::Window));
		palette.setColor(QPalette::Text, palette.color(QPalette::WindowText));
		widget->setPalette(palette);
#endif
        }
        if (qobject_cast<QScrollBar *>(widget)) {
                widget->installEventFilter(d);
                widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
        }
        if (QFrame *frame = qobject_cast<QFrame *>(widget)) {
		switch (frame->frameShape()) {
			case QFrame::Panel:
			case QFrame::WinPanel:
			case QFrame::Box:
				frame->setFrameShape(QFrame::StyledPanel);
				break;
			case QFrame::HLine:
			case QFrame::VLine:
				frame->setEnabled(false);
				break;
			default:
				break;
		}
		if (frame->frameShadow() == QFrame::Plain && frame->backgroundRole() == QPalette::Base) {
			if (frame->parentWidget() && frame->parentWidget()->inherits("KTitleWidget")) {
				frame->setBackgroundRole(QPalette::Window);
			}
		}
#if 1
                if (!strcmp(widget->metaObject()->className(), "QListWidget")
                    && widget->parentWidget()
                    && !strcmp(widget->parentWidget()->metaObject()->className(), "Sidebar")) {
                    //(static_cast<QAbstractItemView *>(widget))->setFrameStyle(QFrame::Plain | QFrame::StyledPanel);
                    (static_cast<QAbstractItemView *>(widget))->setFrameStyle(QFrame::NoFrame);
                }
                if (!strcmp(widget->metaObject()->className(), "Kontact::Navigator")) {
                    // (static_cast<QAbstractItemView *>(widget))->viewport()->setBackgroundRole(QPalette::Base);
                    // (static_cast<QAbstractItemView *>(widget))->viewport()->setAutoFillBackground(true);
                    (static_cast<QAbstractItemView *>(widget))->setFrameStyle(QFrame::Plain | QFrame::StyledPanel);
                    // polishSidebarView(static_cast<QAbstractItemView *>(widget), d->transparentPlacesPanel ? TransparentSidebar : DefaultSidebar);
                }
		if (widget->inherits("SidebarTreeView")) {
                    polishSidebarView(static_cast<QAbstractItemView *>(widget), DefaultSidebar);
                }
		if (widget->inherits("KHTMLView")) {
		//	QPalette palette = widget->palette();
		//	palette.setColor(QPalette::Window, palette.color(QPalette::Base));
		//	((QAbstractScrollArea *) widget)->viewport()->setPalette(palette);
		//	printf("frame style is 0x%08x\n", ((QFrame *) widget)->frameStyle());
                        if (d->expensiveShadows) {
                            ((QFrame *) widget)->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
                        }
		}
#endif
#if 1
		if (widget->inherits("KFilePlacesView")) {
                    polishSidebarView(static_cast<QAbstractItemView *>(widget), d->transparentPlacesPanel ? TransparentSidebar : DefaultSidebar);
		}
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
		if (widget->inherits("QPlainTextEdit")) {
			QPlainTextEdit *edit = static_cast<QPlainTextEdit *>(widget);
		//	QPalette palette = edit->palette();
		//	palette.setColor(QPalette::Window, QColor(245, 245, 245));
		//	edit->setPalette(palette);
		//	edit->viewport()->setPalette(palette);
			edit->setBackgroundVisible(false);
			edit->viewport()->installEventFilter(d);
                        edit->installEventFilter(d);
                        widget->setAttribute(Qt::WA_Hover, true);
		}
#endif
#if 1
		if (QTextEdit *edit = qobject_cast<QTextEdit *>(widget)) {
#if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
			if (!qstrcmp(widget->metaObject()->className(), "SampleEdit")) {
				QWidget *bg = new QWidget(widget);
				bg->lower();
				bg->setObjectName(QLatin1String("sample_background"));
				bg->setGeometry(2, 2, widget->width() - 4, widget->height() - 4);
				bg->setAutoFillBackground(true);
				bg->show();
			} else
#endif
                        {
				d->mapper.setMapping(edit, edit);
				connect(edit, SIGNAL(textChanged()), &d->mapper, SLOT(map()));
				connect(&d->mapper, SIGNAL(mapped(QWidget *)), d, SLOT(textEditSourceChanged(QWidget *)));
				d->updateTextEditMargins(edit);
			}
			edit->viewport()->installEventFilter(d);
                        edit->installEventFilter(d);
                        widget->setAttribute(Qt::WA_Hover, true);
			if (qstrcmp(widget->metaObject()->className(), "IRCInput")) {
                            if (!d->allowTabulations) {
                                edit->setTabChangesFocus(true);
                            }
                        }
#if 0
			if (QTextBrowser *browser = qobject_cast<QTextBrowser *>(widget)) {
				connect(browser, SIGNAL(sourceChanged()), &d->mapper, SLOT(map()));
			}
#endif
		}
#endif
	}
#if 0
	if (QComboBox *combo = qobject_cast<QComboBox *>(widget)) {
		if (!combo->isEditable()) {
			combo->setBackgroundRole(QPalette::Button);
			combo->setForegroundRole(QPalette::ButtonText);
		}
	}
	if (qobject_cast<QCheckBox *>(widget)
	 || qobject_cast<QRadioButton *>(widget)) {
		widget->setBackgroundRole(QPalette::Window);
		widget->setForegroundRole(QPalette::WindowText);
	}
#endif
	if (qobject_cast<QScrollBar *>(widget)
	 || qobject_cast<QSlider *>(widget)
	 || qobject_cast<QDial *>(widget)
	 || qobject_cast<QLineEdit *>(widget)
	 || qobject_cast<QAbstractSpinBox *>(widget)
	 || qobject_cast<QHeaderView*>(widget)
	 || qobject_cast<QTabBar *>(widget)
	 || qobject_cast<QSplitterHandle *>(widget)
	 || qobject_cast<QPushButton *>(widget)
	 || qobject_cast<QComboBox *>(widget)
	 || qobject_cast<QCheckBox *>(widget)
	 || qobject_cast<QRadioButton *>(widget)
	 || qobject_cast<QGroupBox *>(widget)
	 || qobject_cast<QToolButton *>(widget)) {
		widget->setAttribute(Qt::WA_Hover, true);
	}
#if 0
	if (d->allowScrollBarSliderToCoverArrows && qobject_cast<QScrollBar *>(widget)) {
		widget->installEventFilter(d);
	}
#endif
#if 0
        if (widget->inherits("Q3ProgressBar")) {
            widget->installEventFilter(d);
            if (widget->isVisible()) {
                d->setAnimated(widget, true);
            }
        }
#endif
	if (QProgressBar *pbar = qobject_cast<QProgressBar *>(widget)) {
		pbar->installEventFilter(d);
		if (pbar->isVisible() && !widget->inherits("StatusBarSpaceInfo")) {
			d->setAnimated(pbar, true);
		}
	}
#if 1
        if (qobject_cast<QMenu *>(widget)) {
            widget->installEventFilter(d);
        }
	if (QToolBar *toolbar = qobject_cast<QToolBar *>(widget)) {
		QFont font;
		font.setPointSizeF(font.pointSizeF() / (1.19));
		QList<QToolButton *> children = toolbar->findChildren<QToolButton *>();
		Q_FOREACH (QToolButton *child, children) {
			if (!child->icon().isNull()) {
				child->setFont(font);
			}
		}
		connect(toolbar, SIGNAL(orientationChanged(Qt::Orientation)), d, SLOT(updateToolBarOrientation(Qt::Orientation)));
                toolbar->setBackgroundRole(QPalette::Window);
        }
        if (widget->inherits("Q3ToolBar")) {
            widget->setBackgroundRole(QPalette::Window);
        }
#endif
#if 0
	// FIXME does not work
	if (QMenu *menu = qobject_cast<QMenu *>(widget)) {
#if 1
		QFont font;
		QFont oldfont;
		oldfont.setPointSizeF(oldfont.pointSizeF() * 1.0001);
		font.setPointSizeF(font.pointSizeF() / (1.19 /* * 1.19*/));
		font.setBold(true);
		menu->setFont(font);
	/*	QAction *action = menu->menuAction();
		action->setFont(oldfont);
		QList<QAction *> children = action->findChildren<QAction *>();
		Q_FOREACH (QAction *child, children) {
			child->setFont(oldfont);
		}*/
#else
		menu->setStyleSheet(QLatin1String("font-size: 6.5")/*.arg(menu->font().pointSizeF() / (1.19 * 1.19))*/);
#endif
	}
#endif
#if 0
	// FIXME does not work
	if (QGroupBox *group = qobject_cast<QGroupBox *>(widget)) {
		QFont oldfont;
#if 0
		if (group->testAttribute(Qt::WA_SetFont)) {
			QFont oldfont = group->fontInfo();
		}
#endif
		QFont font = oldfont;
		font.setPointSizeF(font.pointSizeF() * 1.19);
		font.setBold(true);
		group->setFont(font);
		QList<QWidget *> children = group->findChildren<QWidget *>();
		Q_FOREACH (QWidget *child, children) {
			if (1 || !(child->testAttribute(Qt::WA_SetFont))) {
				printf("reset\n");
				child->setFont(oldfont);
			}
		}
	}
#endif
#if 1
	if (widget->inherits("Q3Header")) {
		QFont font;
		font.setPointSizeF(font.pointSizeF() / (1.19 /* 1.19*/));
		font.setBold(true);
		widget->setFont(font);
	}
#endif
	if (QAbstractScrollArea *area = qobject_cast<QAbstractScrollArea *>(widget)) {
		if (QAbstractItemView *iv = qobject_cast<QAbstractItemView *>(widget)) {
#if (QT_VERSION >= QT_VERSION_CHECK(4, 2, 0))
			// ### Qt issue
		//	iv->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		//	iv->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
		//	QApplication::setWheelScrollLines(64);
			iv = iv;
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
			iv->viewport()->setAttribute(Qt::WA_Hover);
                        if (QTreeView *tree = qobject_cast<QTreeView *>(widget)) {
                            iv->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                            if (tree->uniformRowHeights()) {
                                iv->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
                            }
                        } else if (QListView *list = qobject_cast<QListView *>(widget)) {
                            if (list->uniformItemSizes()) {
                                iv->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                                iv->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
                            }
                        } else if (qobject_cast<QTableView *>(widget)) {
                            iv->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                            iv->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
                        }
#endif

#if 1
			if (QHeaderView *header = qobject_cast<QHeaderView *>(widget)) {
				QFont font;
				font.setPointSizeF(font.pointSizeF() / (1.19 /* 1.19*/));
				font.setBold(true);
				header->setFont(font);
				// FIXME workaround for Qt 4.3
				header->headerDataChanged(header->orientation(), 0, 0);
				header->updateGeometry();
			}
#endif
		}
		if (area->frameStyle() == (QFrame::StyledPanel | QFrame::Sunken)) {
			d->installFrameShadow(area);
		}
	}
        if (d->expensiveShadows) {
            if (widget->inherits("Konsole::TerminalDisplay")
             || widget->inherits("KTextEditor::View")
             || widget->inherits("KHTMLView")) {
            //	((QFrame *) widget)->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
                    d->installFrameShadow(widget);
            }
        }
#if 0
	if (widget->inherits("KTextEditor::View")) {
		QWidget *parent = widget->parentWidget();
		if (parent) {
			QFrame *frame = new QFrame(parent);
			if (frame) {
				frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
				widget->setParent(frame);
			}
		}
	}
#endif
#if 1
	if (widget->inherits("KCharSelectTable")) {
		QPalette palette;
		widget->setPalette(palette);
	}
#endif
#if 1
	if (widget->inherits("KFadeWidgetEffect")) {
		widget->installEventFilter(d);
	}
#endif
	if (widget->inherits("Q3ScrollView")) {
		QFrame *frame = qobject_cast<QFrame *>(widget);
		if (frame && frame->frameStyle() == (QFrame::StyledPanel | QFrame::Sunken)) {
			d->installFrameShadow(widget);
		}
	}
#endif
#if 0
	if (QTabWidget *tab = qobject_cast<QTabWidget *>(widget)) {
		if (QToolButton *button = qobject_cast<QToolButton *>(tab->cornerWidget(Qt::TopRightCorner))) {
			button->setAutoRaise(true);
		}
		if (QToolButton *button = qobject_cast<QToolButton *>(tab->cornerWidget(Qt::TopLeftCorner))) {
			button->setAutoRaise(true);
		}
	}
#endif
#if 1
	if (QToolButton *button = qobject_cast<QToolButton *>(widget)) {
		if (qobject_cast<QTabWidget *>(button->parentWidget())) {
			button->setAutoRaise(true);
		}
	}
#endif
	if (!qstrcmp(widget->metaObject()->className(), "QToolBoxButton")) {
		widget->setAttribute(Qt::WA_Hover, true);
	}
	if (!qstrcmp(widget->metaObject()->className(), "KLineEditButton")) {
		widget->installEventFilter(d);
		widget->setAttribute(Qt::WA_Hover, true);
	}
        if (qobject_cast<QLineEdit *>(widget)) {
            widget->unsetCursor();
            widget->installEventFilter(d);
            widget->setMouseTracking(true);
        }
        if (QLayout *layout = widget->layout()) {
            // explicitely check public layout classes, QMainWindowLayout doesn't work here
            if (qobject_cast<QBoxLayout *>(layout)
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
             || qobject_cast<QFormLayout *>(layout)
#endif
             || qobject_cast<QGridLayout *>(layout)
             || qobject_cast<QStackedLayout *>(layout)) {
                d->polishLayout(layout);
             }
        }
        if (!qstrcmp(widget->metaObject()->className(), "InfoSidebarPage")
        || !qstrcmp(widget->metaObject()->className(), "InformationPanel")) {
            widget->installEventFilter(d);
        }
#if 0//(QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
        if (widget->inherits("KTabWidget") && widget->property("closeButtonEnabled").toBool()) {
            widget->setProperty("tabsClosable", true);
            widget->setProperty("closeButtonEnabled", false);
            connect(widget, SIGNAL(tabCloseRequested(int)), widget, SIGNAL(closeRequest(int)));
        }
        if (widget->inherits("KTabBar")) {
            widget->setProperty("tabsClosable", true);
            connect(widget, SIGNAL(tabCloseRequested(int)), widget, SIGNAL(closeRequest(int)));
        }
#endif
        if (d->visualizeLayouts) {
            widget->installEventFilter(d);
        }
        ParentStyle::polish(widget);
}


void SkulptureStyle::unpolish(QWidget *widget)
{
	ParentStyle::unpolish(widget);
        if (d->visualizeLayouts) {
            widget->removeEventFilter(d);
        }
//	return;
	if (qobject_cast<QScrollBar *>(widget)
	 || qobject_cast<QSlider *>(widget)
	 || qobject_cast<QDial *>(widget)
//	 || qobject_cast<QLineEdit *>(widget)
	 || qobject_cast<QAbstractSpinBox *>(widget)
	 || qobject_cast<QHeaderView*>(widget)
	 || qobject_cast<QTabBar *>(widget)
	 || qobject_cast<QSplitterHandle *>(widget)
	 || qobject_cast<QPushButton *>(widget)
	 || qobject_cast<QComboBox *>(widget)
	 || qobject_cast<QCheckBox *>(widget)
	 || qobject_cast<QRadioButton *>(widget)
	 || qobject_cast<QGroupBox *>(widget)
	 || qobject_cast<QToolButton *>(widget)) {
		widget->setAttribute(Qt::WA_Hover, false);
	}
#if (QT_VERSION >= QT_VERSION_CHECK(4, 3, 0))
	if (qobject_cast<QMdiArea *>(widget)) {
		widget->removeEventFilter(d);
	}
	if (QMdiSubWindow *win = qobject_cast<QMdiSubWindow *>(widget)) {
		win->removeEventFilter(d);
		WidgetShadow *shadow = findShadow(win);
		if (shadow) {
			shadow->hide();
			shadow->setParent(0);
			shadow->deleteLater();
		}
	}
#endif
#if 1
	if (QDialog *dialog = qobject_cast<QDialog *>(widget)) {
		dialog->removeEventFilter(d);
	}
#endif
#if 0
	if (QLCDNumber *lcd = qobject_cast<QLCDNumber *>(widget)) {
		lcd->removeEventFilter(d);
	}
	if (QToolBox *toolBox = qobject_cast<QToolBox *>(widget)) {
		toolBox->removeEventFilter(d);
	}
	if (QDockWidget *dock = qobject_cast<QDockWidget *>(widget)) {
		dock->removeEventFilter(d);
	}
	if (QStatusBar *status = qobject_cast<QStatusBar *>(widget)) {
		status->removeEventFilter(d);
	}
#endif
#if 0
	if (/*d->allowScrollBarSliderToCoverArrows &&*/ qobject_cast<QScrollBar *>(widget)) {
		widget->installEventFilter(d);
	}
#endif
	if (QProgressBar *pbar = qobject_cast<QProgressBar *>(widget)) {
		pbar->removeEventFilter(d);
		d->setAnimated(pbar, false);
		return;
	}
	if (QAbstractScrollArea *area = qobject_cast<QAbstractScrollArea *>(widget)) {
		area->removeEventFilter(d);
		if (/*QAbstractItemView *iv =*/qobject_cast<QAbstractItemView *>(widget)) {
#if 1
			if (QHeaderView *header = qobject_cast<QHeaderView *>(widget)) {
				QFont font;
			//	font.setPointSizeF(font.pointSizeF() / (1.19 * 1.19));
			//	font.setBold(true);
				header->setFont(font);
				// FIXME workaround for Qt 4.3
				header->headerDataChanged(header->orientation(), 0, 0);
				header->updateGeometry();
			}
#endif
		}
	/*	if (QMdiArea *area = qobject_cast<QMdiArea *>(widget)) {
			area->viewport()->removeEventFilter(d);
		}
	*/
		d->removeFrameShadow(area);
	}
        if (d->expensiveShadows) {
            if (widget->inherits("Konsole::TerminalDisplay")
             || widget->inherits("KTextEditor::View")
             || widget->inherits("KHTMLView")) {
                    widget->removeEventFilter(d);
                    d->removeFrameShadow(widget);
            }
        }
	if (widget->inherits("Q3ScrollView")) {
		widget->removeEventFilter(d);
		d->removeFrameShadow(widget);
	}
#if 1
	if (widget->inherits("KFadeWidgetEffect")) {
		widget->removeEventFilter(d);
	}
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
	if (widget->inherits("QPlainTextEdit")) {
		QPlainTextEdit *edit = static_cast<QPlainTextEdit *>(widget);
		edit->viewport()->removeEventFilter(d);
                edit->removeEventFilter(d);
        }
#endif
        if (qobject_cast<QScrollBar *>(widget)) {
            widget->removeEventFilter(d);
            widget->setAttribute(Qt::WA_OpaquePaintEvent, true);
        }
	if (QTextEdit *edit = qobject_cast<QTextEdit *>(widget)) {
#if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
		if (!qstrcmp(widget->metaObject()->className(), "SampleEdit")) {
			QList<QObject *> children = widget->children();
			Q_FOREACH (QObject *child, children) {
				if (child->objectName() == QLatin1String("sample_background")) {
					child->setParent(0);
					child->deleteLater();
				}
			}
		} else
#endif
                {
			d->mapper.removeMappings(edit);
		}
		edit->viewport()->removeEventFilter(d);
                edit->removeEventFilter(d);
        }
	if (QToolBar *toolbar = qobject_cast<QToolBar *>(widget)) {
		QFont font;
	//	font.setPointSizeF(font.pointSizeF() / (1.19));
		QList<QToolButton *> children = toolbar->findChildren<QToolButton *>();
		Q_FOREACH (QToolButton *child, children) {
			if (!child->icon().isNull()) {
				child->setFont(font);
			}
		}
		disconnect(toolbar, SIGNAL(orientationChanged(Qt::Orientation)), d, SLOT(updateToolBarOrientation(Qt::Orientation)));
	}
	if (!qstrcmp(widget->metaObject()->className(), "KLineEditButton")) {
		widget->removeEventFilter(d);
	}
        if (qobject_cast<QLineEdit *>(widget)) {
            widget->setMouseTracking(false);
            widget->removeEventFilter(d);
            widget->setCursor(Qt::IBeamCursor);
        }
        if (!d->postEventWidgets.isEmpty()) {
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
            d->postEventWidgets.removeOne(widget);
#else
            d->postEventWidgets.removeAll(widget);
#endif
        }
        if ((QWidget *) d->oldEdit == widget) {
            d->oldEdit = 0;
        }
        if (!qstrcmp(widget->metaObject()->className(), "InfoSidebarPage")
        || !qstrcmp(widget->metaObject()->className(), "InformationPanel")) {
            widget->removeEventFilter(d);
        }
        if (qobject_cast<QMenu *>(widget)) {
            widget->removeEventFilter(d);
        }
}


/*-----------------------------------------------------------------------*/

extern void lineEditMouseMoved(QLineEdit *lineEdit, QMouseEvent *event);

void SkulptureStyle::Private::processPostEventWidgets()
{
    QWidget *widget;

    while (!postEventWidgets.isEmpty() && (widget = postEventWidgets.takeFirst())) {
        if (QTextEdit *edit = qobject_cast<QTextEdit *>(widget)) {
            handleCursor(edit);
        }
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
        else if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit *>(widget)) {
            handleCursor(edit);
        }
#endif
    }
}


void SkulptureStyle::Private::addPostEventWidget(QWidget *widget)
{
    if (qobject_cast<QTextEdit *>(widget)
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
        || qobject_cast<QPlainTextEdit *>(widget)
#endif
    ) {
        if (!postEventWidgets.contains(widget)) {
            bool signal = postEventWidgets.isEmpty();
            postEventWidgets.append(widget);
            if (signal) {
                QTimer::singleShot(0, this, SLOT(processPostEventWidgets()));
            }
        }
    }
}


static void visualizeLayoutSpacing(QLayout *layout, QPainter *painter)
{
    layout->activate();
    QColor color;
    int spacing = -1;
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
    if (QFormLayout *formLayout = qobject_cast<QFormLayout *>(layout)) {
        spacing = formLayout->spacing();
    } else
#endif
    if (QGridLayout *gridLayout = qobject_cast<QGridLayout *>(layout)) {
        spacing = gridLayout->spacing();
    } else if (QBoxLayout *boxLayout = qobject_cast<QBoxLayout *>(layout)) {
        spacing = boxLayout->spacing();
    } else {
        spacing = layout->spacing();
    }
    if (spacing == 0) {
        color = Qt::green;
    } else if (spacing > 0) {
        color = Qt::red;
    }
    if (color.isValid()) {
        color.setAlpha(30);
        painter->fillRect(layout->geometry(), color);
    } else {
        painter->setPen(QColor(0, 150, 0, 50));
        painter->drawRect(layout->geometry().adjusted(0, 0, -1, -1));
    }
    if (qobject_cast<QGridLayout *>(layout)) {
        painter->setPen(QColor(200, 0, 0, 150));
        painter->drawRect(layout->geometry().adjusted(0, 0, -1, -1));
    }
    for (int i = 0; i < layout->count(); ++i) {
        if (QLayout *childLayout = layout->itemAt(i)->layout()) {
            visualizeLayoutSpacing(childLayout, painter);
        }
    }
}


bool SkulptureStyle::Private::eventFilter(QObject *watched, QEvent *event)
{
#if 0
	// can't happen, because widgets are the only ones to install it
	if (!watched->isWidgetType()) {
		return QObject::eventFilter(watched, event);
	}
#endif
	QWidget *widget = reinterpret_cast<QWidget *>(watched);
#if 0
        if (event->type() != QEvent::UpdateRequest && event->type() != QEvent::Paint) {
            qDebug() << "handling" << event->type() << "for object" << widget->objectName() << "which is a" << widget->metaObject()->className() << " which is a" << widget->metaObject()->superClass()->className();
        }
#endif
#if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
        if (QMenu *menu = qobject_cast<QMenu *>(widget)) {
            if (runtimeQtVersion() < QT_VERSION_CHECK(4, 6, 0)) {
                return menuEventFilter(menu, event);
            }
        }
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(4, 3, 0))
	if (qobject_cast<QMdiSubWindow *>(widget)) {
		WidgetShadow *shadow = findShadow(widget);
		switch (event->type()) {
			case QEvent::Move:
			case QEvent::Resize:
				if (shadow) {
					shadow->updateGeometry();
				}
				break;
			case QEvent::ZOrderChange:
				if (shadow) {
					shadow->updateZOrder();
				}
				break;
			case QEvent::Hide:
            case QEvent::Destroy:
                if (shadow) {
					shadow->setParent(0);
					shadow->hide();
					shadow->deleteLater();
				}
				break;
			case QEvent::Show:
				if (!shadow) {
					if (widget->parentWidget()) {
						shadow = new WidgetShadow(widget->parentWidget());
						shadow->setWidget(widget);
						shadow->updateZOrder();
					}
				} else {
					shadow->updateZOrder();
				}
			default:
				break;
		}
	}
#endif
        if (event->type() == QEvent::Hide || event->type() == QEvent::Destroy) {
            if (!postEventWidgets.isEmpty()) {
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
                postEventWidgets.removeOne(widget);
#else
                postEventWidgets.removeAll(widget);
#endif
            }
            if ((QWidget *) oldEdit == widget) {
                oldEdit = 0;
            }
        } else if (event->type() != QEvent::Paint) {
            addPostEventWidget(widget);
            if (QWidget *parent = widget->parentWidget()) {
                addPostEventWidget(parent);
                if ((parent = parent->parentWidget())) {
                    addPostEventWidget(parent);
                }
            }
        }
        switch (event->type()) {
		case QEvent::Paint:
#if 1 // highlight current line in QTextEdit / QPlainTextEdit
			if (widget->objectName() == QLatin1String("qt_scrollarea_viewport")) {
				if (QTextEdit *edit = qobject_cast<QTextEdit *>(widget->parent())) {
#if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
					if (!qstrcmp(edit->metaObject()->className(), "SampleEdit")) {
						QList<QObject *> children = edit->children();
						Q_FOREACH (QObject *child, children) {
							if (child->objectName() == QLatin1String("sample_background")) {
								QWidget *bg = qobject_cast<QWidget *>(child);
								if (bg) {
									QPalette palette = edit->palette();
									palette.setColor(QPalette::Window, palette.color(QPalette::Base));
									bg->setPalette(palette);
								}
							}
						}
					}
#endif
				//	updateTextEditMargins(edit);
					paintCursorLine(edit);
				}
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
				else if (widget->parent()->inherits("QPlainTextEdit")) {
					paintCursorLine(static_cast<QPlainTextEdit *>(widget->parent()));
				}
#endif
			}
#endif
#if 1
                        if (visualizeLayouts) {
                            if (!widget->font().isCopyOf(QApplication::font())) {
                                if (widget->font() == QApplication::font()) {
                                    QPainter painter(widget);
                                    painter.fillRect(widget->rect(), QColor(255, 0, 0, 200));
                                }
                            }
                            if (!widget->palette().isCopyOf(QApplication::palette())) {
                                if (true/*widget->palette() == QApplication::palette()*/) {
                                    QPainter painter(widget);
                                    painter.fillRect(widget->rect(), QColor(0, 200, 255, 200));
                                }
                            }
#if (QT_VERSION >= QT_VERSION_CHECK(4, 2, 0))
                            if (!widget->styleSheet().isEmpty()) {
                                QPainter painter(widget);
                                painter.fillRect(widget->rect(), QColor(255, 0, 255, 100));
                            }
#endif
                            if (QLayout *layout = widget->layout()) {
                                if (!(qobject_cast<QToolBar *>(widget))) {
                                    // explicitely check public layout classes, QMainWindowLayout doesn't work here
                                    if (qobject_cast<QBoxLayout *>(layout)
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
                                    || qobject_cast<QFormLayout *>(layout)
#endif
                                    || qobject_cast<QGridLayout *>(layout)
                                    || qobject_cast<QStackedLayout *>(layout)) {
                                        QPainter painter(widget);
                                        visualizeLayoutSpacing(layout, &painter);
                                    }
                                }
                            }
                        }
#endif
#if 0
			if (QDialog *dialog = qobject_cast<QDialog *>(widget)) {
				QPainter painter(dialog);
				QRect r = dialog->rect();
				QLinearGradient dialogGradient1(r.topLeft(), r.bottomRight());
				dialogGradient1.setColorAt(0.0, QColor(255, 255, 255, 30));
				dialogGradient1.setColorAt(1.0, QColor(0, 0, 0, 10));
			//	painter.fillRect(r, dialogGradient1);

				QRadialGradient dialogGradient2(r.left() + r.width() / 2, r.top(), r.height());
				dialogGradient2.setColorAt(0.0, QColor(255, 255, 225, 160));
				dialogGradient2.setColorAt(1.0, QColor(0, 0, 0, 0));
			//	painter.fillRect(r, dialogGradient2);

				QLinearGradient dialogGradient3(r.topLeft(), r.bottomLeft());
				dialogGradient3.setColorAt(0.0, QColor(255, 255, 255, 30));
				dialogGradient3.setColorAt(1.0, QColor(0, 0, 0, 20));
				painter.fillRect(r, dialogGradient3);

				paintThinFrame(&painter, dialog->rect().adjusted(0, 0, 0, 0), dialog->palette(), 60, -20);
				paintThinFrame(&painter, dialog->rect().adjusted(1, 1, -1, -1), dialog->palette(), -20, 60);
			}
#endif
#if 0
			if (QStatusBar *status = qobject_cast<QStatusBar *>(widget)) {
				QPainter painter(status);
				paintThinFrame(&painter, status->rect(), status->palette(), -20, 60);
			}
			if (QToolBox *toolBox = qobject_cast<QToolBox *>(widget)) {
				QPainter painter(toolBox);
				paintThinFrame(&painter, toolBox->rect(), toolBox->palette(), 60, -20);
				paintThinFrame(&painter, toolBox->rect().adjusted(1, 1, -1, -1), toolBox->palette(), -60, 140);
			}
			if (QLCDNumber *lcd = qobject_cast<QLCDNumber *>(watched)) {
				// TODO nicer digits, antialiased, slight italics
			}
			if (QDockWidget *dock = qobject_cast<QDockWidget *>(widget)) {
				// ### rendering a frame around dock widgets does not work, because
				// the subwidgets are placed at the edges.
			}
#endif
                        if (!qstrcmp(widget->metaObject()->className(), "InfoSidebarPage")
                        || !qstrcmp(widget->metaObject()->className(), "InformationPanel")) {
                            QPainter painter(widget);
                            paintThinFrame(&painter, widget->rect().adjusted(0, 0, 0, 0), widget->palette(), 60, -20);
                            paintThinFrame(&painter, widget->rect().adjusted(1, 1, -1, -1), widget->palette(), -20, 60);
                        }
#if (QT_VERSION >= QT_VERSION_CHECK(4, 3, 0))
			if (!qstrcmp(widget->metaObject()->className(), "KLineEditButton")) {
				QPainter painter(widget);
				QStyleOption option;
				option.initFrom(widget);
                                QIcon::Mode iconMode = QIcon::Normal;
				if (option.state & QStyle::State_Enabled && option.state & QStyle::State_MouseOver) {
                                    //iconMode = QIcon::Active;
				//	painter.fillRect(widget->rect(), Qt::red);
				} else {
                                    //iconMode = QIcon::Disabled;
					painter.setOpacity(0.2);
				}
				QRect r = QRect(widget->rect().center() - QPoint(6, 5), QSize(12, 12));
				painter.drawPixmap(r, q->standardIcon(QStyle::SP_TitleBarCloseButton, &option, widget).pixmap(12, 12, iconMode));
				event->accept();
				return true;
			}
			if (widget->inherits("KFadeWidgetEffect")) {
			//	widget->hide();
				event->accept();
			//	widget->removeEventFilter(this);
				return true;
			}
			break;
#endif
                case QEvent::MouseMove:
                    if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(watched)) {
                        lineEditMouseMoved(lineEdit, static_cast<QMouseEvent *>(event));
                    }
                    break;
		case QEvent::Show:
			if (QProgressBar *pbar = qobject_cast<QProgressBar *>(watched)) {
				if (!widget->inherits("StatusBarSpaceInfo")) {
					setAnimated(pbar, true);
				}
			}
			/* fall through */
		case QEvent::Move:
		case QEvent::Resize:
			if (QTextEdit *edit = qobject_cast<QTextEdit *>(widget)) {
#if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
				if (!qstrcmp(widget->metaObject()->className(), "SampleEdit")) {
					QList<QObject *> children = widget->children();
					Q_FOREACH (QObject *child, children) {
						if (child->objectName() == QLatin1String("sample_background")) {
							QWidget *bg = qobject_cast<QWidget *>(child);
							if (bg) {
								bg->setGeometry(2, 2, widget->width() - 4, widget->height() - 4);
							}
						}
					}
				} else
#endif
                                {
					textEditSourceChanged(edit);
				}
			}
#if (QT_VERSION >= QT_VERSION_CHECK(4, 3, 0))
			else if (qobject_cast<QMdiArea *>(widget)) {
				QList<WidgetShadow *> shadows = widget->findChildren<WidgetShadow *>();
				Q_FOREACH (WidgetShadow *shadow, shadows) {
					shadow->updateGeometry();
				}
			}
#endif
			if (qobject_cast<QAbstractScrollArea *>(widget)
			 || widget->inherits("Q3ScrollView")
#if 1
			 || widget->inherits("Konsole::TerminalDisplay")
			 || widget->inherits("KTextEditor::View")
			 || widget->inherits("KHTMLView")
#endif
			   ) {
				updateFrameShadow(widget);
			}
			break;
		case QEvent::Destroy:
		case QEvent::Hide:
			setAnimated(reinterpret_cast<QProgressBar *>(watched), false);
			break;
#if 0
		case QEvent::MouseButtonRelease:
			if (allowScrollBarSliderToCoverArrows && qobject_cast<QScrollBar *>(widget)) {
				widget->update();
			}
			break;
#endif
		default:
			break;
	}
	return QObject::eventFilter(watched, event);
}


/*-----------------------------------------------------------------------*/

#define SkulptureDrawFunction(function, selectortype, optiontype, array) \
\
void SkulptureStyle::function(selectortype element, const optiontype *option, QPainter *painter, const QWidget *widget) const \
{ \
	if (uint(element) < array_elements(array)) { \
		const Private::DrawElementEntry *entry = &array[element]; \
		if (entry->func && option && (!entry->type || option->type == entry->type)) { \
			entry->func(painter, option, widget, this); \
			return; \
		} \
	} \
	ParentStyle::function(element, option, painter, widget); \
}

SkulptureDrawFunction(drawPrimitive, PrimitiveElement, QStyleOption, d->draw_primitive_entry)
SkulptureDrawFunction(drawControl, ControlElement, QStyleOption, d->draw_element_entry)
//SkulptureDrawFunction(drawComplexControl, ComplexControl, QStyleOptionComplex, d->draw_complex_entry)


/*-----------------------------------------------------------------------*/

#include "skulpture.moc"


