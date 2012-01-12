/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "xdgautostart.h"
#include "xdgdirs.h"
#include <QtCore/QDebug>
#include <QtCore/QSet>
#include <QtCore/QDir>

/************************************************
The Autostart Directories are $XDG_CONFIG_DIRS/autostart. If the same filename is
located under multiple Autostart Directories only the file under the most
important directory should be used.

When multiple .desktop files with the same name exists in multiple directories
then only the Hidden key in the most important .desktop file must be considered:
If it is set to true all .desktop files with the same name in the other
directories MUST be ignored as well.
 ************************************************/
XdgDesktopFileList XdgAutoStart::desktopFileList(bool excludeHidden)
{
    QStringList dirs;
    dirs << QString("%1/autostart").arg(XdgDirs::configHome());

    foreach(QString dir, XdgDirs::configDirs())
        dirs << QString("%1/autostart").arg(dir);

    dirs.removeDuplicates();

    QSet<QString> processed;
    XdgDesktopFileList ret;
    foreach (QString dirName, dirs)
    {
        QDir dir(dirName);
        if (!dir.exists())
            continue;

        QFileInfoList files = dir.entryInfoList(QStringList("*.desktop"), QDir::Files | QDir::Readable);
        foreach (QFileInfo fi, files)
        {
            if (processed.contains(fi.fileName()))
                continue;

            processed << fi.fileName();

            XdgDesktopFile desktop;
            if (!desktop.load(fi.absoluteFilePath()))
                continue;

            if (!desktop.isApplicable(excludeHidden))
                continue;

            ret << desktop;
        }
    }
    return ret;
}


QString XdgAutoStart::localPath(const XdgDesktopFile& file)
{
    QFileInfo fi(file.fileName());
    return QString("%1/autostart/%2").arg(XdgDirs::configHome(), fi.fileName());
}
