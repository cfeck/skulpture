#! /bin/sh

############################################################################
#
# Skulpture User Installation Script
#
# Copyright (c) 2008 Christoph Feck <christoph@maxiom.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
############################################################################
#
# Version 1.0.3 (10-Dec-2008)
#

# Search in these directories for KDE4
# (in addition to standard KDEDIRS)
KDE4_PATH=/opt/kde4:/usr/lib/kde4:/usr/local/kde4

# Default build type
# Options are: Release, Debug, RelWithDebugInfo, MinSizeRel
BUILD_TYPE=Release

# Name of log file
LOG=install_log.txt


############################################################################

# Welcome user
LANG=C
echo -e "Skulpture User Installation Script"
echo > $LOG "-- Skulpture install log started:" `date +%F-%H:%M:%S`
echo >> $LOG "-- Directory is:" $PWD
echo >> $LOG "-- System is:" `uname -a`
echo >> $LOG "-- PATH is:" $PATH
echo >> $LOG "-- KDEDIRS is:" $KDEDIRS
echo >> $LOG "-- KDE4_PATH is:" $KDE4_PATH
ERROR=0

# Make sure cmake is installed
CMAKE=`which 2> /dev/null cmake || echo "-"`
echo >> $LOG "-- Found cmake:" $CMAKE
if test ! -x $CMAKE ; then
    echo
    echo "ERROR: Required command \"cmake\" not found"
    echo "    You need CMake 2.4 or newer. (http://www.cmake.org/)"
    ERROR=1
else
    $CMAKE --version >> $LOG
fi

# Make sure make is installed
MAKE=`which 2> /dev/null make || echo "-"`
if test ! -x $MAKE ; then
    MAKE=`which 2> /dev/null gmake || echo "-"`
fi
echo >> $LOG "-- Found make:" $MAKE
if test ! -x $MAKE ; then
    echo
    echo "ERROR: Required command \"make\" not found"
    echo "    You need make 3.80 or newer. (http://www.gnu.org/software/make/)"
    ERROR=1
else
    $MAKE --version >> $LOG
fi

# Make sure a C++ compiler is installed
CC=`which 2> /dev/null c++ || echo "-"`
if test ! -x $CC ; then
    CC=`which 2> /dev/null g++ || echo "-"`
    if test ! -x $CC ; then
        CC=`which 2> /dev/null CC || echo "-"`
    fi
fi
echo >> $LOG "-- Found c++:" $CC
if test ! -x $CC ; then
    echo
    echo "ERROR: Required command \"c++\" not found"
    echo "    You need a C++ compiler, e.g. GCC 4.1 or newer. (http://gcc.gnu.org/)"
    ERROR=1
else
    $CC --version >> $LOG
fi

# Come back after requirements are installed
if test $ERROR = 1 ; then
    echo "    Install the required commands and run this script again."
fi

# Make sure we are in the right directory
if test ! -e ./CMakeLists.txt -o ! -x ./install.sh -o ! -d ./src -o ! -e ./src/skulpture.cpp; then
    echo
    echo "ERROR: This install script does not support out-of-source builds"
    echo "    You need to \"cd\" to the directory containing this script."
    echo >> $LOG "ERROR: not in source directory"
    ERROR=1
fi

# Add Skulpture version information to log
echo -n >> $LOG "-- Skulpture version is: "
cat >> $LOG ./VERSION

# Look up kde4-config in KDE 4 binary path
PATH=$PATH:`echo $KDEDIRS:$KDE4_PATH | sed 's|:|/bin:|g' | sed 's|/bin/bin|/bin|g'`/bin
KDE4_CONFIG=`which kde4-config 2> /dev/null || echo "-"`

# Set default install prefix (we get this from kde4-config)
if test -x $KDE4_CONFIG ; then
    $KDE4_CONFIG --version >> $LOG
    KDE_PREFIX=`$KDE4_CONFIG --prefix`
    echo >> $LOG "-- Found KDE4:" $KDE_PREFIX
    # Make sure we have KWin decoration headers
    if test ! -e $KDE_PREFIX/include/kwinglobals.h ; then
        echo
        echo "WARNING: Optional header \"kwinglobals.h\" not found"
        echo "    You need the kdebase-workspace header files to build"
        echo "    the Skulpture window decoration."
        echo
        echo "    If you are using binary packages, install package"
        echo "    \"kdebase-workspace-dev\" and run this script again."
        echo >> $LOG "WARNING: kwinglobals.h not found"
    fi
else
    echo >> $LOG "WARNING: KDE4 not found"
    echo >> $LOG "-- tried with PATH: " $PATH
    echo
    echo "WARNING: Optional command \"kde4-config\" not found"
    echo "    You need KDE 4.0 or newer (http://www.kde.org/)"
    echo "    to build the Skulpture KDE components."
    echo
    echo "    If you have KDE4 installed, add the location"
    echo "    of the KDE4 /bin directory to your PATH and run"
    echo "    this script again."
    echo >> $LOG "WARNING: kde4-config not found"
fi

# If there are no errors, let's build
if test $ERROR = 1 ; then
    echo -n "Installation FAILED"
else
    echo -e "\nConfiguring (running \"$CMAKE .\") ..." &&
    echo "-- Looking for Qt / KDE ..." &&
    rm -rf ./CMakeCache.txt ./lib &&
    $CMAKE -DCMAKE_INSTALL_PREFIX=$KDE_PREFIX -DCMAKE_BUILD_TYPE=$BUILD_TYPE . 2>> $LOG >> $LOG &&
    grep -i "Found\ \(KDE\ \|Qt\)" $LOG &&
    echo -e "\nBuilding (running \"$MAKE\") ..." &&
    echo "-- This may take a while ..." &&
    $MAKE 2>> $LOG >> $LOG &&
    echo -e "\nInstalling (running \"sudo $MAKE install\") ..." &&
    sudo $MAKE install 2>> $LOG >> $LOG &&
    grep "\-\- Installing" $LOG &&
    echo -n -e "\nInstallation done" ||
    (tail -n 10 $LOG &&
    echo -n -e "Installation FAILED.\nPlease report any build failures")
fi

# Build summary
echo >> $LOG "-- Skulpture install log finished:" `date +%F-%H:%M:%S`
echo ", log written to: \"$LOG\""

