import os
import re

log_dir = "Build/logs"
search_terms = [r"blockio", r"block io", r"install", r"failed", r"fatal", r"hook", r"not found", r"aborting", r"gbl"]

# Compiling search regex
regex = re.compile("|".join(search_terms), re.IGNORECASE)

with open("Build/search_results.txt", "w", encoding="utf-8") as out:
    for root, dirs, files in os.walk(log_dir):
        for file in sorted(files):
            if file.endswith(".idx"):
                continue
            path = os.path.join(root, file)
            out.write(f"\n--- Scanning {path} ---\n")
            try:
                with open(path, "rb") as f:
                    content = f.read()
                
                decoded = content.decode("utf-8", errors="ignore")
                decoded = decoded.replace("\x00", "")
                
                if decoded:
                    lines = decoded.splitlines()
                    matches = 0
                    for line_no, line in enumerate(lines, 1):
                        if regex.search(line):
                            out.write(f"Line {line_no:4d}: {line.strip()}\n")
                            matches += 1
                    out.write(f"Found {matches} matching lines.\n")
            except Exception as e:
                out.write(f"Error reading {path}: {e}\n")
