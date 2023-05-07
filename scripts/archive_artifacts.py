import argparse
import os
import zipfile

def make_rel_archive(a_args):
	archive = zipfile.ZipFile("{}.zip".format(a_args.name), "w", zipfile.ZIP_DEFLATED)
	archive.write(a_args.dll, "F4SE/Plugins/{}".format(os.path.basename(a_args.dll)))

def make_dbg_archive(a_args):
	archive = zipfile.ZipFile("{}_pdb.zip".format(a_args.name), "w", zipfile.ZIP_DEFLATED)
	archive.write(a_args.pdb, os.path.basename(a_args.pdb))

def make_ps_cso_archive(a_args):
	archive = zipfile.ZipFile("{}_ps_cso.zip".format(a_args.name), "w", zipfile.ZIP_DEFLATED)
	a_args = "ScopeEffect_PS"
	archive.write(a_args.cso, os.path.basename(a_args.cso))

def make_vs_cso_archive(a_args):
	archive = zipfile.ZipFile("{}_vs_cso.zip".format(a_args.name), "w", zipfile.ZIP_DEFLATED)
	a_args = "ScopeEffect_VS"
	archive.write(a_args.cso, os.path.basename(a_args.cso))

def parse_arguments():
	parser = argparse.ArgumentParser(description="archive build artifacts for distribution")
	parser.add_argument("--dll", type=str, help="the full dll path", required=True)
	parser.add_argument("--name", type=str, help="the project name", required=True)
	parser.add_argument("--pdb", type=str, help="the full pdb path", required=True)
	parser.add_argument("--vscso", type=str, help="the Complied Shader files", required=True)
	parser.add_argument("--pscso", type=str, help="the Complied Shader files", required=True)
	return parser.parse_args()

def main():
	out = "artifacts"
	try:
		os.mkdir(out)
	except FileExistsError:
		pass
	os.chdir(out)

	args = parse_arguments()
	make_rel_archive(args)
	make_dbg_archive(args)
	make_vs_cso_archive(args)
	make_ps_cso_archive(args)

if __name__ == "__main__":
	main()
