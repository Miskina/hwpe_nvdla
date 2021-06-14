#!/usr/bin/env python3

from ipstools.ipstools.IPDatabase import IPDatabase

import os
from argparse import ArgumentParser


parser = ArgumentParser(prog='HWPE NVDLA IP update',
			description='Update the IPs of HWPE NVDLA')

parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Output more information.')

args = parser.parse_args()

ipdb = IPDatabase(
	skip_scripts=True,
	build_deps_tree=True,
	resolve_deps_conflicts=True,
	use_semver_resolution=True,
	rtl_dir='.',
	ips_dir='dependencies',
	vsim_dir='sim',
	default_server='https://github.com',
	verbose=args.verbose
)

os.makedirs('dependencies', exist_ok=True)

ipdb.update_ips()

ipdb.save_database()