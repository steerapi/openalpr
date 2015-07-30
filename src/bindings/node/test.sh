#!/bin/sh
node-gyp build
killall node
node server.js &
sleep 2
curl --header "Content-Type:application/octet-stream" --trace-ascii debugdump.txt --data-binary @ea7the.jpg http://localhost:3000/upload

