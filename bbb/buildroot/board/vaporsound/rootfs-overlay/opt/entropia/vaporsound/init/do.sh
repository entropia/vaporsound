#!/bin/sh

for i in /opt/entropia/vaporsound/init/init.d/*
do
	$i

	if [ "$?" -ne 0 ]
	then
		NAME=$(basename "$i")

		echo "init step $NAME failed, dropping to shell"

		/bin/sh
	fi
done
