#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# https://qiita.com/tkj/items/210a66213667bc038110

from http.server import HTTPServer
from http.server import BaseHTTPRequestHandler
from urllib.parse import urlparse
from urllib.parse import parse_qs

class class1(BaseHTTPRequestHandler):
	def do_POST(self):
		#parsed = urlparse(self.path)
		#print("parsed={}".format(parsed))
		#params = parse_qs(parsed.query)
		#print("params={}".format(params))
		content_len  = int(self.headers.get("content-length"))
		#print("content_len={}".format(content_len))
		req_body = self.rfile.read(content_len).decode("utf-8")
		#print("req_body={}".format(req_body))
		print("{}".format(req_body))

		body = "OK"
		self.send_response(200)
		self.send_header('Content-type', 'text/html; charset=utf-8')
		self.send_header('Content-length', len(body.encode()))
		self.end_headers()
		self.wfile.write(body.encode())

#host = '127.0.0.1'
host = '0.0.0.0'
port = 8080

print("Listening on port {}". format(port))
server = HTTPServer((host, port), class1)

server.serve_forever()
