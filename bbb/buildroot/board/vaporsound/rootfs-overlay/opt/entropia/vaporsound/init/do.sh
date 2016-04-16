#!/bin/sh

for i in /opt/entropia/vaporsound/init/init.d/*
do
	NAME=$(basename "$i")
	echo "running $NAME"

	$i

	if [ "$?" -ne 0 ]
	then
		echo "init step $NAME failed, dropping to shell"

		/bin/sh
	fi
done
