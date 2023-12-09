#!/usr/bin/env python3

import cgi

print("Content-type: text/html\n")

print("<html><body>")
print("<h1>Hello from Python CGI!</h1>")
form = cgi.FieldStorage()
print("<p>Hello, some person!</p>")
print("</body></html>")