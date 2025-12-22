import json
import os
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse


class TaskStore:
    def __init__(self, path: str):
        self.path = path
        self.tasks = []
        self.next_id = 1
        self._load()

    def _load(self):
        if not os.path.exists(self.path):
            return

        try:
            with open(self.path, "r", encoding="utf-8") as f:
                data = json.load(f)
        except Exception:
            return

        if not isinstance(data, list):
            return

        max_id = 0

        # Skip only bad entries
        for item in data:
            if not isinstance(item, dict):
                continue
            if not self._is_valid_task(item):
                continue

            task = {
                "title": item["title"],
                "priority": item["priority"],
                "isDone": bool(item["isDone"]),
                "id": int(item["id"]),
            }
            self.tasks.append(task)
            if task["id"] > max_id:
                max_id = task["id"]
        self.next_id = max_id + 1

    def _save(self):
        with open(self.path, "w", encoding="utf-8") as f:
            json.dump(self.tasks, f)

    def _is_valid_task(self, item: dict) -> bool:
        required = {"title", "priority", "isDone", "id"}
        if not required.issubset(item.keys()):
            return False
        if not isinstance(item["title"], str) or not item["title"]:
            return False
        if not isinstance(item["priority"], str) or not item["priority"]:
            return False
        try:
            int(item["id"])
        except (TypeError, ValueError):
            return False
        return True

    def list_tasks(self):
        return list(self.tasks)

    def create_task(self, title: str, priority: str):
        task = {
            "title": title,
            "priority": priority,
            "isDone": False,
            "id": self.next_id,
        }
        self.next_id += 1
        self.tasks.append(task)
        self._save()
        return task

    def complete_task(self, task_id: int) -> bool:
        for task in self.tasks:
            if task["id"] == task_id:
                task["isDone"] = True
                self._save()
                return True
        return False


class TodoHandler(BaseHTTPRequestHandler):
    def _read_json_body(self):
        length = int(self.headers.get("Content-Length", 0))
        raw = self.rfile.read(length) if length > 0 else b""
        if not raw:
            return None
        try:
            return json.loads(raw)
        except Exception:
            return None

    def _send_json(self, data, status=200):
        payload = json.dumps(data).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(payload)))
        self.end_headers()
        self.wfile.write(payload)

    def _send_empty(self, status=200):
        self.send_response(status)
        self.send_header("Content-Length", "0")
        self.end_headers()

    def _error(self, status, message):
        self._send_json({"error": message}, status=status)

    def do_GET(self):
        parsed = urlparse(self.path)
        if parsed.path == "/tasks":
            self._send_json(self.server.store.list_tasks())
            return

        self._error(404, "Not found")

    def do_POST(self):
        parsed = urlparse(self.path)
        parts = [p for p in parsed.path.split("/") if p]

        if parsed.path == "/tasks":
            self._handle_create_task()
            return

        if len(parts) == 3 and parts[0] == "tasks" and parts[2] == "complete":
            self._handle_complete_task(parts[1])
            return

        self._error(404, "Not found")

    def _handle_create_task(self):
        data = self._read_json_body()
        if not data or "title" not in data or "priority" not in data:
            self._error(400, "Fields 'title' and 'priority' are required")
            return

        title = data["title"]
        priority = data["priority"]
        if not isinstance(title, str) or not title.strip():
            self._error(400, "Field 'title' must be a non-empty string")
            return
        if not isinstance(priority, str) or not priority.strip():
            self._error(400, "Field 'priority' must be a non-empty string")
            return

        task = self.server.store.create_task(title.strip(), priority.strip())
        self._send_json(task)

    def _handle_complete_task(self, raw_id):
        try:
            task_id = int(raw_id)
        except ValueError:
            self._error(400, "Task id must be integer")
            return

        if self.server.store.complete_task(task_id):
            self._send_empty(200)
        else:
            self._send_empty(404)

    def log_message(self, fmt, *args):
        return


def run(host="127.0.0.1", port=8080):
    tasks_path = os.path.abspath("tasks.txt")
    store = TaskStore(tasks_path)

    httpd = HTTPServer((host, port), TodoHandler)
    httpd.store = store
    print(f"Serving on http://{host}:{port}")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("Shutting down...")
        httpd.server_close()


if __name__ == "__main__":
    run()
