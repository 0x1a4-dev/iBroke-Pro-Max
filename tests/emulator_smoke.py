#!/usr/bin/env python3
"""Headless end-to-end ROM test using the official DeSmuME libretro core.

Usage: python3 tests/emulator_smoke.py CORE.so iPhone_Dou.nds build/iPhone_Dou.elf OUTDIR
Requires Python, NumPy, Pillow and a host `nm` executable. No BIOS files needed.
The emulator is a test dependency and is not included in the release.
"""
import ctypes as C
import json
from pathlib import Path
import subprocess
import sys
import time
import numpy as np
from PIL import Image

core_path, rom_path, elf_path, out_path=map(Path,sys.argv[1:5])
out_path.mkdir(parents=True,exist_ok=True)
symbols=subprocess.check_output(['nm','--defined-only',str(elf_path)],text=True)
state_addr=int(next(line.split()[0] for line in symbols.splitlines() if line.split()[-1]=='state'),16)

class Variable(C.Structure): _fields_=[('key',C.c_char_p),('value',C.c_char_p)]
class Game(C.Structure): _fields_=[('path',C.c_char_p),('data',C.c_void_p),('size',C.c_size_t),('meta',C.c_char_p)]
Env=C.CFUNCTYPE(C.c_bool,C.c_uint,C.c_void_p)
Video=C.CFUNCTYPE(None,C.c_void_p,C.c_uint,C.c_uint,C.c_size_t)
Audio=C.CFUNCTYPE(None,C.c_int16,C.c_int16)
AudioBatch=C.CFUNCTYPE(C.c_size_t,C.c_void_p,C.c_size_t)
Poll=C.CFUNCTYPE(None)
Input=C.CFUNCTYPE(C.c_int16,C.c_uint,C.c_uint,C.c_uint,C.c_uint)
Log=C.CFUNCTYPE(None,C.c_int,C.c_char_p)

options={
    b'desmume_cpu_mode':b'interpreter',b'desmume_opengl_mode':b'disabled',
    b'desmume_internal_resolution':b'256x192',b'desmume_num_cores':b'1',
    b'desmume_screens_layout':b'top/bottom',b'desmume_pointer_type':b'touch',
    b'desmume_pointer_mouse':b'enabled',b'desmume_pointer_colour':b'white',
    b'desmume_load_to_memory':b'enabled',b'desmume_use_external_bios':b'disabled',
    b'desmume_boot_into_bios':b'disabled',b'desmume_screens_gap':b'0',
}
defaults={};system_dir=str(out_path.resolve()).encode();username=b'DouTester'
pixel_format=2;last_image=None;buttons=set();pointer=None;audio_peak=0;video_frames=0

@Log
def log(level,fmt): pass

@Env
def env(cmd,data):
    global pixel_format
    if cmd==10: pixel_format=C.cast(data,C.POINTER(C.c_uint))[0];return True
    if cmd in (9,30,31): C.cast(data,C.POINTER(C.c_char_p))[0]=system_dir;return True
    if cmd==38: C.cast(data,C.POINTER(C.c_char_p))[0]=username;return True
    if cmd==27: C.cast(data,C.POINTER(C.c_void_p))[0]=C.cast(log,C.c_void_p).value;return True
    if cmd in (3,17,49): C.cast(data,C.POINTER(C.c_bool))[0]=cmd==3;return True
    if cmd in (39,52): C.cast(data,C.POINTER(C.c_uint))[0]=0;return True
    if cmd==15:
        var=C.cast(data,C.POINTER(Variable)).contents
        value=options.get(var.key,defaults.get(var.key))
        if value is None:return False
        var.value=value;return True
    if cmd==16:
        arr=C.cast(data,C.POINTER(Variable));n=0
        while arr[n].key:
            val=arr[n].value or b''
            defaults[arr[n].key]=val.split(b'; ',1)[-1].split(b'|',1)[0];n+=1
        return True
    return cmd in (8,11,18,32,35,37,44,55,69)

@Video
def video(data,w,h,pitch):
    global last_image,video_frames
    if not data or data==C.c_void_p(-1).value:return
    video_frames+=1
    raw=C.string_at(data,pitch*h)
    if pixel_format==1:
        a=np.frombuffer(raw,np.uint8).reshape(h,pitch)[:,:w*4].reshape(h,w,4)
        rgb=a[:,:,[2,1,0]].copy()
    else:
        a=np.frombuffer(raw,'<u2').reshape(h,pitch//2)[:,:w].astype(np.uint32)
        if pixel_format==2:
            rgb=np.stack([((a>>11)&31)*255//31,((a>>5)&63)*255//63,(a&31)*255//31],axis=-1).astype('uint8')
        else:rgb=np.stack([((a>>10)&31)*255//31,((a>>5)&31)*255//31,(a&31)*255//31],axis=-1).astype('uint8')
    last_image=Image.fromarray(rgb)

@Audio
def audio(l,r): pass

@AudioBatch
def audio_batch(data,n):
    global audio_peak
    a=np.ctypeslib.as_array(C.cast(data,C.POINTER(C.c_int16)),shape=(n*2,))
    if n:audio_peak=max(audio_peak,int(np.abs(a.astype('int32')).max()))
    return n

@Poll
def poll(): pass

@Input
def input_state(port,device,index,id):
    if port or index:return 0
    if device==1:return int(id in buttons)
    if device==6 and pointer is not None:
        x,y=pointer
        if id==0:return round((x+.5)*65536/256)-32768
        if id==1:return round((192+y+.5)*65536/384)-32768
        if id==2:return 1
    return 0

core=C.CDLL(str(core_path.resolve()))
for name,cb,typ in [('environment',env,Env),('video_refresh',video,Video),('audio_sample',audio,Audio),('audio_sample_batch',audio_batch,AudioBatch),('input_poll',poll,Poll),('input_state',input_state,Input)]:
    fn=getattr(core,'retro_set_'+name);fn.argtypes=[typ];fn(cb)
core.retro_init()
data=rom_path.read_bytes();buffer=C.create_string_buffer(data)
game=Game(str(rom_path.resolve()).encode(),C.cast(buffer,C.c_void_p),len(data),None)
core.retro_load_game.argtypes=[C.POINTER(Game)];core.retro_load_game.restype=C.c_bool
assert core.retro_load_game(C.byref(game)),'Could not load ROM'
core.retro_get_memory_data.argtypes=[C.c_uint];core.retro_get_memory_data.restype=C.c_void_p
ram=core.retro_get_memory_data(2);assert ram
state_ptr=ram+state_addr-0x02000000
def state_int(offset=0):return C.c_int.from_address(state_ptr+offset).value
def run(n):
    for _ in range(n):core.retro_run()
def snap(name):
    assert last_image is not None
    last_image.transpose(Image.Transpose.ROTATE_90).save(out_path/(name+'.png'))
    if name=='00_boot': last_image.save(out_path/'00_native_rotation.png')
def key(id,n=4):
    buttons.add(id);run(n);buttons.clear();run(8)
def touch(x,y,n=5):
    global pointer
    pointer=(255-y,x);run(n);pointer=None;run(9)
def app(expected):assert state_int()==expected,f'Expected app {expected}, got {state_int()}'


GRID=[10,9,2,11,16,12,3,1,8,6,7,4]
DOCK=[13,14,5,15]
def open_app(a):
    if state_int()!=0:key(0)
    if a in GRID:
        n=GRID.index(a);touch(24+n%3*47,109+n//3*38)
    else:
        n=DOCK.index(a);touch(168,100+n*38)
    app(a)

def logical_pointer(x,y):
    global pointer
    pointer=(255-y,x)

started=time.monotonic();run(150);snap('00_boot');app(-1)
assert last_image.size==(256,384)
arr=np.array(last_image);assert arr[:192].std()>20 and arr[192:].std()>20
key(8);app(0);snap('01_home')
# Physical arrows remap to the viewed directions when the DS is rotated left.
for physical,expected in [(5,9),(6,11),(7,6),(4,2)]:
    key(physical);key(8);app(expected);key(0)
for a in range(1,17):
    open_app(a);snap(f'app_{a:02d}');touch(22,29);app(0)
open_app(1);touch(96,117);run(60);snap('02_spari')
open_app(2);key(8);run(20);snap('03_camera')
open_app(3);key(8);run(16);snap('04_store')
open_app(4);key(8);run(180);touch(50,230,30);snap('05_game')
run(1850);app(4);snap('06_game_result')
open_app(5);touch(90,209);snap('07_messages')
open_app(6);touch(100,203);snap('08_weather')
open_app(7)
for k in (8,15,9,14):touch(29+k%4*44,99+k//4*42)
snap('09_calculator')
open_app(11);key(9);touch(13,170);touch(31,170);snap('10_notes')
open_app(13);touch(37,84);touch(96,84);touch(96,236);snap('11_phone')
open_app(15);touch(96,160);run(90);snap('12_music');touch(96,160)
open_app(16);touch(140,198);run(125);touch(140,198);snap('13_stopwatch')
# Swipe the home indicator, with real physical touch coordinates.
logical_pointer(96,250);run(5)
for y in range(250,170,-10):logical_pointer(96,y);run(3)
pointer=None;run(10);app(0)
open_app(8);touch(96,108);snap('14_settings');key(3);app(-1);snap('15_lock_pastel')
# iPhone-style swipe-up unlock in the rotated coordinate system.
logical_pointer(96,236);run(5)
for y in range(236,144,-10):logical_pointer(96,y);run(3)
pointer=None;run(10);app(0);snap('16_home_pastel')
assert audio_peak>0
assert abs(state_int(8)-video_frames)<10, 'VBlank game timing drift'
report={'result':'PASS','emulator':'DeSmuME libretro / interpreter','rom':rom_path.name,
        'orientation':'90 degrees counterclockwise; main screen left; touchscreen right',
        'logical_resolution':'384x256 (two 192x256 panels)',
        'input':'all four rotated d-pad directions; touch; home/unlock swipes; A/B/START',
        'apps':16,'audio_peak':audio_peak,'video_frames':video_frames,
        'app_tick_count':state_int(8),'seconds':round(time.monotonic()-started,2),
        'state_symbol':hex(state_addr),'hardware_tested':False}
(out_path/'report.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report),flush=True)
core.retro_unload_game();core.retro_deinit()
