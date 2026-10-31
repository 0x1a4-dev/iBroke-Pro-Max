#!/usr/bin/env python3
"""Package the verified ROM; publish from the repository's GitHub Actions job."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import sys
import urllib.error
import urllib.parse
import urllib.request
import zipfile

ROOT = Path(__file__).resolve().parents[1]


def prepare():
    manifest = json.loads((ROOT / 'release.json').read_text())
    rom = ROOT / manifest['rom']
    assert hashlib.sha256(rom.read_bytes()).hexdigest() == manifest['rom_sha256']
    dist = ROOT / 'dist'
    dist.mkdir(exist_ok=True)
    archive = dist / manifest['package']
    prefix = 'iBroke-Pro-Max-' + manifest['tag']
    with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for p in sorted(ROOT.rglob('*')):
            rel = p.relative_to(ROOT)
            if not p.is_file() or any(x in rel.parts for x in ['.git', 'build', 'dist', '__pycache__']):
                continue
            if rel.as_posix() == 'iPhone_Dou.nds':
                continue
            if p.name.startswith('cc') and p.suffix == '.s':
                continue
            info = zipfile.ZipInfo(prefix + '/' + rel.as_posix(), (2026, 10, 31, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o644 << 16
            z.writestr(info, p.read_bytes())
        info = zipfile.ZipInfo(prefix + '/iPhone_Dou.nds', (2026, 10, 31, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        info.external_attr = 0o644 << 16
        z.writestr(info, rom.read_bytes())
    with zipfile.ZipFile(archive) as z:
        assert z.testzip() is None
        assert len(z.namelist()) == len(set(z.namelist()))
        assert hashlib.sha256(z.read(prefix + '/iPhone_Dou.nds')).hexdigest() == manifest['rom_sha256']
    sums = dist / 'SHA256SUMS.txt'
    sums.write_text(''.join(hashlib.sha256(p.read_bytes()).hexdigest() + '  ' + p.name + '\n' for p in [rom, archive]))
    return manifest, [rom, archive, sums]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--prepare-only', action='store_true')
    args = parser.parse_args()
    manifest, assets = prepare()
    if args.prepare_only:
        print(json.dumps({'status': 'prepared', 'tag': manifest['tag'], 'assets': [str(p) for p in assets]}))
        return
    repository = os.environ.get('GITHUB_REPOSITORY', '')
    token = os.environ.get('GH_TOKEN', '')
    if repository != '0x1a4-dev/iBroke-Pro-Max' or not token:
        raise SystemExit('Publishing requires the authorized repository GitHub Actions environment.')
    base = 'https://api.github.com/repos/' + repository

    def api(url, method='GET', data=None, content_type='application/json'):
        req = urllib.request.Request(url, data=data, method=method, headers={
            'Authorization': 'Bearer ' + token,
            'Accept': 'application/vnd.github+json',
            'Content-Type': content_type,
            'User-Agent': 'iBroke-Pro-Max-release',
        })
        with urllib.request.urlopen(req, timeout=120) as response:
            return json.load(response)

    try:
        release = api(base + '/releases/tags/' + manifest['tag'])
    except urllib.error.HTTPError as error:
        if error.code != 404:
            raise
        body = {'tag_name': manifest['tag'], 'name': manifest['title'],
                'target_commitish': os.environ['GITHUB_SHA'],
                'body': (ROOT / manifest['notes']).read_text(), 'draft': False, 'prerelease': False}
        release = api(base + '/releases', 'POST', json.dumps(body).encode())
    existing = {a['name']: a for a in release.get('assets', [])}
    upload = release['upload_url'].split('{', 1)[0]
    if urllib.parse.urlparse(upload).netloc != 'uploads.github.com':
        raise SystemExit('Unexpected release-upload host.')
    for path in assets:
        if path.name in existing:
            if existing[path.name].get('digest') != 'sha256:' + hashlib.sha256(path.read_bytes()).hexdigest():
                raise SystemExit('Existing asset differs; refusing to overwrite: ' + path.name)
            continue
        api(upload + '?name=' + urllib.parse.quote(path.name), 'POST', path.read_bytes(), 'application/octet-stream')
    print('Published ' + release['html_url'])


if __name__ == '__main__':
    main()
