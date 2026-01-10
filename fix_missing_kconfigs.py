import os
import re

def fix_kconfig(file_path):
    if not os.path.exists(file_path):
        return
    
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
    
    new_lines = []
    base_dir = os.path.dirname(file_path)
    
    # We need to find the repository root to resolve absolute-looking paths like "drivers/..."
    # Since we are running from root, let's assume current dir is root.
    root_dir = os.getcwd()

    for line in lines:
        match = re.search(r'source\s+"([^"]+)"', line)
        if match:
            sourced_file = match.group(1)
            # Try relative to current dir (root)
            full_path = os.path.join(root_dir, sourced_file)
            if not os.path.exists(full_path):
                print(f"Commenting out missing source in {file_path}: {sourced_file}")
                new_lines.append("# " + line)
                continue
        new_lines.append(line)
        
    with open(file_path, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)

if __name__ == "__main__":
    # Scan all Kconfig files
    for root, dirs, files in os.walk('.'):
        for name in files:
            if name == "Kconfig":
                fix_kconfig(os.path.join(root, name))
