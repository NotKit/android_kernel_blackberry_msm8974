import sys
import os

def resolve_file(file_path, preference):
    if not os.path.exists(file_path):
        print(f"File not found: {file_path}")
        return

    try:
        with open(file_path, 'rb') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
        return

    new_lines = []
    state = "normal" # "normal", "ours", "theirs"
    
    for line in lines:
        line_str = line.decode('utf-8', errors='ignore')
        
        if line_str.startswith("<<<<<<<"):
            state = "ours"
            continue
        elif line_str.startswith("======="):
            state = "theirs"
            continue
        elif line_str.startswith(">>>>>>>"):
            state = "normal"
            continue
            
        if state == "normal":
            new_lines.append(line)
        elif state == "ours" and preference == "ours":
            new_lines.append(line)
        elif state == "theirs" and preference == "theirs":
            new_lines.append(line)
            
    try:
        with open(file_path, 'wb') as f:
            f.writelines(new_lines)
        print(f"Resolved {file_path} favoring {preference}")
    except Exception as e:
        print(f"Error writing {file_path}: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 resolve_simple.py <ours|theirs> <file_path> [file_paths...]")
        sys.exit(1)
        
    pref = sys.argv[1]
    for fp in sys.argv[2:]:
        resolve_file(fp, pref)
