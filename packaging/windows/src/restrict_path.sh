#!/bin/bash

if [ "$RESTRICTED_PATH" != "" ]; then
	PATH="$RESTRICTED_PATH"
	echo "PATH set to $PATH"
fi

