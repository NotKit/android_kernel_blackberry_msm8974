import sys
import re

def resolve_conflict(file_path):
    try:
        with open(file_path, 'rb') as f:
            content = f.read().decode('utf-8', errors='ignore')
        
        # Robust regex for conflict blocks
        # Group 1: Ours, Group 2: Theirs
        pattern = re.compile(r'<<<<<<< HEAD\n(.*?)\n?=======\n(.*?)\n?>>>>>>> .*?\n?', re.DOTALL)
        
        # For resolve_kconfig (Ours)
        resolved_content = pattern.sub(r'\1\n', content)
        
        # Clean up any potential double newlines if \1 was multi-line
        # but actually \1 already contains its newlines.
        # Let's just use a more precise replacement.
        
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(resolved_content)
            
        print(f"Resolved conflicts in {file_path} favoring HEAD.")
        
    except Exception as e:
        print(f"Error resolving {file_path}: {e}")

if __name__ == "__main__":
    for file_path in sys.argv[1:]:
        resolve_conflict(file_path)
