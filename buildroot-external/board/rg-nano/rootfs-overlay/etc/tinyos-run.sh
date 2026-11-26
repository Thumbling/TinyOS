#!/bin/sh
# Start the UI and keep its log on the data partition so a black screen can
# still be explained afterwards.
exec /usr/bin/tinyos >> /data/tinyos.log 2>&1
