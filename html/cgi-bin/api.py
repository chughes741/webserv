#!/usr/bin/env python3

import cgi

print("Content-type: text/html\n")

print("<html><body>")
print("<h1>Hello from Python CGI!</h1>")
form = cgi.FieldStorage()
if "name" in form:
    print("<p>Hello, " + form["name"].value + "!</p>")
else:
    print("<p>Hello, anonymous!</p>")
print("</body></html>")