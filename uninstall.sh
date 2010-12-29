#! /bin/sh

if [ -f install_manifest.txt ] ; then
	echo "The following files will be removed:"
	cat install_manifest.txt
	echo "Press RETURN to abort, or enter root's password to remove them."
	su -c "xargs rm -v < install_manifest.txt" && rm -f install_manifest.txt
else
	echo "Cannot find install_manifest.txt, maybe not yet installed?"
fi
