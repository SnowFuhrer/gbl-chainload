import os

for f in sorted(os.listdir('extracted_logfs')):
    p = os.path.join('extracted_logfs', f)
    if not os.path.isfile(p) or f.endswith('.idx'):
        continue
    with open(p, 'rb') as file:
        content = file.read().decode('utf-8', errors='ignore').replace('\x00', '')
    if 'Starting GBL app' in content:
        idx = content.find('Starting GBL app')
        print('='*40)
        print(f'FILE: {f}')
        print('='*40)
        start = max(0, idx - 200)
        end = min(len(content), idx + 2000)
        print(content[start:end])
