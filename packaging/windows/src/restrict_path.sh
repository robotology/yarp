#!/bin/bash

if [ ! "k$RESTRICTED_PATH" = "k" ]; then
	PATH="$RESTRICTED_PATH"
	echo "PATH set to $PATH"
fi

