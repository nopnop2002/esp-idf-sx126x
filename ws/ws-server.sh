#!/bin/bash
#Install websocat
#sudo wget -qO /usr/local/bin/websocat https://github.com/vi/websocat/releases/latest/download/websocat.x86_64-unknown-linux-musl
#sudo chmod a+x /usr/local/bin/websocat
#websocat --version
PORT=8080
websocat -E -t ws-listen:0.0.0.0:${PORT} literalreply:'OK'
