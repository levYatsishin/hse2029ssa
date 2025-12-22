from http.server import BaseHTTPRequestHandler
from http.server import HTTPServer
import os
import urllib.parse
import json
from requests import get, put

YANDEX_TOKEN = input("Enter Yandex.Disk OAuth token: ").strip()
API_BASE = "https://cloud-api.yandex.net/v1/disk"


def fetch_uploaded_files(token, folder="Backup"):
    headers = {"Authorization": "OAuth %s" % token}
    limit = 100
    offset = 0
    uploaded = []

    while True:
        resp = get(
            f"{API_BASE}/resources",
            headers=headers,
            params={"path": folder, "limit": limit, "offset": offset},
        )
        if resp.status_code != 200:
            break

        data = resp.json()
        embedded = data.get("_embedded") or {}
        items = embedded.get("items") or []
        if items and len(items) > 0:
            for item in items:
                if item.get("type") == "file":
                    uploaded.append(item.get("name"))

        if len(items) < limit:
            break
        offset += limit

    return uploaded


def run(handler_class=BaseHTTPRequestHandler):
    server_address = ("", 8000)
    httpd = HTTPServer(server_address, handler_class)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        httpd.server_close()


class HttpGetHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        uploaded = fetch_uploaded_files(YANDEX_TOKEN)
        uploaded_set = set(uploaded)

        def fname2html(fname, uploaded_set):
            style = ""
            if fname in uploaded_set:
                style = ' style="background-color: rgba(0, 200, 0, 0.25)"'
            return f"""
                <li{style} onclick="fetch('/upload', {{'method': 'POST', 'body': '{fname}'}})">
                    {fname}
                </li>
            """

        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        files = os.listdir("pdfs")
        # print("Debug, uploaded set:", uploaded_set)
        self.wfile.write(
            """
            <html>
                <head>
                </head>
                <body>
                    <ul>
                      {files}
                    </ul>
                </body>
            </html>
        """.format(
                files="\n".join([fname2html(fn, uploaded_set) for fn in files])
            ).encode()
        )

    def do_POST(self):
        content_len = int(self.headers.get("Content-Length"))
        fname = self.rfile.read(content_len).decode("utf-8")
        local_path = "pdfs/%s" % fname
        ya_path = f"Backup/{urllib.parse.quote(fname)}"
        headers = {"Authorization": "OAuth %s" % YANDEX_TOKEN}
        resp = get(f"{API_BASE}/resources/upload?path={ya_path}", headers=headers)
        print(resp.text)
        upload_url = json.loads(resp.text)["href"]
        print(upload_url)
        resp = put(upload_url, files={"file": (fname, open(local_path, "rb"))})
        print(resp.status_code)
        self.send_response(200)
        self.end_headers()


run(handler_class=HttpGetHandler)
