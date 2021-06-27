#!/bin/bash

#
# The original exclude list seems to be maintained on the AppImage repositiry.
# See: https://github.com/AppImage/pkg2appimage/blob/master/excludelist
#
# Getting this page returns html content
#
# A raw text file is provided by probonopd (author of linuxdeployqt).
# See: https://raw.githubusercontent.com/probonopd/AppImages/master/excludelist
#

excludeListSourceURL=https://raw.githubusercontent.com/probonopd/AppImages/master/excludelist

headerFileName=LibraryExcludeListLinux.h
destinationFile=$(readlink -f $(dirname "$0"))/../libs/DeployUtils_Core/src/Mdt/DeployUtils/Impl/$headerFileName

excludeList=($(wget --quiet $excludeListSourceURL -O - | sort | uniq | cut -d '#' -f 1 | grep -v "^#.*" | grep "[^-\s]"))
if [ "$excludeList" == "" ];
then
  echo "generate $headerFileName failed: got a empty exclude list from $excludeListSourceURL"
  exit 1;
fi


cat > "$destinationFile" <<EOF
/*
 * List of libraries to exclude for different reasons.
 *
 * Automatically generated from
 * https://raw.githubusercontent.com/probonopd/AppImages/master/excludelist
 *
 * This file shall be committed by the developers occassionally,
 * otherwise systems without access to the internet won't be able to build
 * fully working versions of MdtDeployUtils.
 *
 * See https://github.com/probonopd/linuxdeployqt/issues/274 for more
 * information.
 *
 * Credits: https://github.com/probonopd/linuxdeployqt
 */
#include <QStringList>
#include <QLatin1String>

static
const QStringList generatedExcludelistLinux = {
EOF

for item in ${excludeList[@]:0:${#excludeList[@]}-1};
do
  echo -e '  QLatin1String("'"$item"'"),' >> "$destinationFile"
done
echo -e '  QLatin1String("'"${excludeList[$((${#excludeList[@]}-1))]}"'")' >> "$destinationFile"

echo "};" >> "$destinationFile"
