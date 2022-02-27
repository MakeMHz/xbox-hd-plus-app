#!/usr/bin/env python3
import os
import re
import requests, json
from collections import OrderedDict

def script_rel_path(path):
	return os.path.join(os.path.dirname(__file__), path)

contr_repos = [
	'MakeMHz/xbox-hd-plus',
	'MakeMHz/xbox-hd-plus-app',
	'XboxDev/nxdk'
]

contributors = []

# Create the 'Genersted' folder if it doesn't exist
path = script_rel_path('../src/Generated')

if not os.path.exists(path):
	os.makedirs(path)

for repo in contr_repos:
	reqs = requests.get('https://api.github.com/repos/%s/contributors?anon=1' % repo)
	data = json.loads(reqs.text)

	for contr in data:
		if contr['login']:
			name = re.sub(r'[^a-zA-Z0-9]', '', contr['login'])
			contributors.append(name)

# Remove duplicates
contributors = list(OrderedDict.fromkeys(contributors))

# Write out cpp file
with open(script_rel_path('../src/Generated/Contributors.cpp'), 'w') as file:
	file.write('#include <stdint.h>\n')
	file.write('#include "Contributors.h"\n\n')
	file.write('const uint32_t ContributorsCount = %d;\n' % len(contributors))

	file.write('const char *Contributors[] = {\n')
	for name in contributors:
		file.write('    "%s",\n' % name)
	file.write('};\n')

# Write out cpp header file
with open(script_rel_path('../src/Generated/Contributors.h'), 'w') as file:
	file.write('#pragma once\n')
	file.write('#include <stdint.h>\n\n')
	file.write('extern const char *Contributors[];\n')
	file.write('extern const uint32_t ContributorsCount;\n')
