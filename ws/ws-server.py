#!/usr/bin/env python
# python3 -m pip install websockets
# https://github.com/python-websockets/websockets

import asyncio
from websockets.server import serve
port=8080

async def echo(websocket):
    async for message in websocket:
        print("{}".format(message))
        await websocket.send('ok')

async def main():
    #async with serve(echo, "localhost", 8080):
    async with serve(echo, "0.0.0.0", port):
        await asyncio.Future()  # run forever

asyncio.run(main())
