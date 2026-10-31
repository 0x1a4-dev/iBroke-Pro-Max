#!/usr/bin/env python3
"""Dou OS 2.1 English: native iPhone-like vector icons, Inter glyphs and photo assets."""
from pathlib import Path
import math
import numpy as np
from PIL import Image, ImageDraw, ImageFont, ImageOps
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'source'
FONT=ROOT/'artwork/fonts/Inter.ttf'
SIZES=[7,9,11,15,22,32,44]

def font(size,weight=400):
    f=ImageFont.truetype(str(FONT),size)
    f.set_variation_by_axes([max(14,min(32,size)),weight])
    return f

def rgb555(im):
    a=np.asarray(im.convert('RGB'),dtype=np.uint16)
    return (0x8000|(a[:,:,0]>>3)|((a[:,:,1]>>3)<<5)|((a[:,:,2]>>3)<<10)).flatten().tolist()

def emit(f,typ,name,values):
    f.write(f'const {typ} {name}[{len(values)}] = {{\n')
    for i in range(0,len(values),24):f.write(','.join(map(str,values[i:i+24]))+',\n')
    f.write('};\n')

def icon(n):
    s=4; sz=32*s
    palette={1:('#162a72','#532571'),2:('#cbd1d6','#9aa0a7'),3:('#38b4ff','#0074f5'),4:('#68ed76','#22bd52'),5:('#67ed69','#08bc36'),6:('#179df1','#0869cb'),7:('#292b30','#0b0b0d'),8:('#b6bac3','#7e838c'),9:('#ffffff','#fafafa'),10:('#ffffff','#ffffff'),11:('#ffffff','#f5f4ed'),12:('#e4efdc','#a7d693'),13:('#62ef6d','#08bf3c'),14:('#ffffff','#ffffff'),15:('#ff657d','#fa244a'),16:('#151518','#050506')}
    def col(h):return tuple(bytes.fromhex(h[1:]))
    a,b=map(col,palette[n]);im=Image.new('RGBA',(sz,sz));d=ImageDraw.Draw(im)
    for y in range(sz):
        t=y/(sz-1);d.line((0,y,sz,y),fill=tuple(round(x*(1-t)+z*t) for x,z in zip(a,b))+(255,))
    def line(pts,c='white',w=1.5):d.line([(round(x*s),round(y*s)) for x,y in pts],fill=c,width=round(w*s),joint='curve')
    def rr(box,r=2,fill=None,outline=None,w=1):d.rounded_rectangle(tuple(round(v*s) for v in box),round(r*s),fill=fill,outline=outline,width=round(w*s))
    def ell(box,fill=None,outline=None,w=1):d.ellipse(tuple(round(v*s) for v in box),fill=fill,outline=outline,width=round(w*s))
    if n==1:
        for r,c in [(12,'#83b6ff'),(10,'#dba0fc'),(8,'#f9adc1'),(6,'#9be8f6')]:ell((16-r,16-r,16+r,16+r),outline=c,w=1.5)
        line([(7,18),(11,12),(15,21),(19,10),(25,16)],w=1.2)
    elif n==2:
        rr((4,10,28,25),3,fill='#44494f');rr((9,7,16,12),2,fill='#44494f')
        ell((10,11,24,25),fill='#d6d9de');ell((12,13,22,23),fill='#202d46');ell((14,14,19,19),fill='#5b7eaf');ell((25,12,27,14),fill='#f7e5ab')
    elif n==3:
        line([(10,24),(19,8)],w=2.5);line([(13,8),(23,25)],w=2.5);line([(6,21),(26,21)],w=2.5)
    elif n==4:
        rr((4,11,26,23),3,outline='white',w=1.5);rr((27,14,29,20),1,fill='white')
        d.polygon([(18*s,8*s),(11*s,18*s),(16*s,18*s),(14*s,26*s),(22*s,15*s),(17*s,15*s)],fill='white')
    elif n==5:
        ell((4,6,28,24),fill='white');d.polygon([(7*s,21*s),(7*s,27*s),(15*s,22*s)],fill='white')
    elif n==6:
        ell((17,5,27,15),fill='#ffdc42');ell((4,13,18,25),fill='white');ell((12,10,25,23),fill='white');rr((6,18,28,25),3,fill='white')
    elif n==7:
        for x,y,c in [(9,9,'#a5a5a9'),(23,9,'#ff9d12'),(9,23,'#a5a5a9'),(23,23,'#ff9d12')]:ell((x-5,y-5,x+5,y+5),fill=c)
        line([(6,9),(12,9)],'#19191c',1);line([(9,6),(9,12)],'#19191c',1);line([(20,9),(26,9)],w=1)
        line([(6,23),(12,23)],'#19191c',1);line([(20,22),(26,22)],w=1);line([(20,24),(26,24)],w=1)
    elif n==8:
        for a in range(12):
            t=a*math.pi/6;line([(16+8*math.cos(t),16+8*math.sin(t)),(16+12*math.cos(t),16+12*math.sin(t))],'#f1f1f3',2.3)
        ell((6,6,26,26),fill='#f1f1f3');ell((9,9,23,23),fill='#656b74');ell((12,12,20,20),fill='#d0d3da');ell((14,14,18,18),fill='#6a6e76')
    elif n==9:
        colors=['#ff5e71','#ff9a45','#f9d842','#92ca5c','#55c4b9','#60aee8','#8b80d7','#c879be']
        for i,c in enumerate(colors):
            petal=Image.new('RGBA',(sz,sz));pd=ImageDraw.Draw(petal);pd.ellipse((12*s,3*s,20*s,18*s),fill=c+'d0')
            im=Image.alpha_composite(im,petal.rotate(-i*45,resample=Image.Resampling.BICUBIC));d=ImageDraw.Draw(im)
    elif n==10:
        d.text((16*s,3*s),'FRI',font=font(5*s,600),fill='#f1393c',anchor='mt')
        d.text((16*s,9*s),'30',font=font(19*s,400),fill='#111115',anchor='mt')
    elif n==11:
        rr((0,0,32,9),0,fill='#ffd83a');line([(0,10),(32,10)],'#e6c136',.5)
        for y in [16,21,26]:line([(5,y),(27,y)],'#d8d8dc',.6)
    elif n==12:
        line([(-2,10),(34,23)],'white',6);line([(12,-2),(22,34)],'white',6)
        line([(-2,10),(34,23)],'#f1cd54',2);line([(12,-2),(22,34)],'#f1cd54',2)
        line([(4,31),(16,18),(29,4)],'#4ea8ff',5)
        ell((10,10,24,24),fill='white');d.polygon([(17*s,12*s),(12*s,22*s),(17*s,19*s),(22*s,22*s)],fill='#0086fa')
    elif n==13:
        # Familiar handset silhouette, drawn from native primitives.
        d.arc((7*s,5*s,28*s,27*s),85,190,fill='white',width=5*s)
        rr((6,5,12,14),2,fill='white');rr((19,21,28,27),2,fill='white')
    elif n==14:
        ell((3,3,29,29),fill='#15a3eb');ell((5,5,27,27),outline='#a8e3ff',w=.6)
        for k in range(12):
            t=k*math.pi/6;line([(16+10*math.cos(t),16+10*math.sin(t)),(16+11*math.cos(t),16+11*math.sin(t))],'white',.6)
        d.polygon([(24*s,7*s),(17*s,18*s),(14*s,15*s)],fill='#fa444d');d.polygon([(8*s,25*s),(14*s,15*s),(17*s,18*s)],fill='white')
    elif n==15:
        line([(13,24),(13,10),(25,7),(25,21)],w=2.5);ell((7,21,14,27),fill='white');ell((19,18,26,24),fill='white')
    elif n==16:
        ell((4,4,28,28),fill='white')
        for k in range(12):
            t=k*math.pi/6;line([(16+9*math.cos(t),16+9*math.sin(t)),(16+10.5*math.cos(t),16+10.5*math.sin(t))],'#18181c',.7)
        line([(16,8),(16,16),(23,19)],'#17171b',1.3);line([(16,16),(12,23)],'#ff493c',.8)
    mask=Image.new('L',(sz,sz));ImageDraw.Draw(mask).rounded_rectangle((0,0,sz-1,sz-1),7*s,fill=255)
    im.putalpha(mask)
    return im.resize((32,32),Image.Resampling.LANCZOS)

def main():
    chars=[chr(x) for x in range(32,127)]+list('äöüÄÖÜß°€×÷→·‹›…—•↑↓♪↻−')
    pixels=[];glyphs=[]
    for size in SIZES:
        f=font(size,700 if size==22 else 600 if size in [7,15] else 300 if size>=32 else 400)
        for ch in chars:
            bx,by,x1,y1=f.getbbox(ch,anchor='ls');w,h=max(0,x1-bx),max(0,y1-by)
            im=Image.new('L',(max(1,w),max(1,h)));ImageDraw.Draw(im).text((-bx,-by),ch,font=f,anchor='ls',fill=255)
            glyphs.append((ord(ch),len(pixels),w,h,bx,by+size,round(f.getlength(ch))))
            if w*h:pixels.extend((np.array(im).flatten()>>4).tolist())
    sheet=Image.open(ROOT/'artwork/photo_assets.png').convert('RGB')
    wall=sheet.crop((0,140,768,652)).resize((384,256),Image.Resampling.LANCZOS)
    portrait=ImageOps.fit(sheet.crop((768,0,1536,1024)),(164,204),Image.Resampling.LANCZOS)
    yy,xx=np.mgrid[:256,:384];t=(xx+yy*.75)/576
    waves=(np.sin(xx/95+yy/74)+1)/2
    alt=np.stack([80+100*t+30*waves,112+36*t+42*waves,215+31*waves],axis=-1).clip(0,255).astype('uint8')
    alt=Image.fromarray(alt)
    wall.save(ROOT/'artwork/wallpaper0.png');alt.save(ROOT/'artwork/wallpaper1.png');portrait.save(ROOT/'artwork/portrait.png')
    with (OUT/'assets.h').open('w') as f:
        f.write('#pragma once\n#include <stdint.h>\ntypedef struct { uint32_t code,offset; uint8_t w,h; int8_t x,y; uint8_t advance; } Glyph;\n')
        f.write(f'#define GLYPHS_PER_FONT {len(chars)}\n#define ICON_SIZE 32\nextern const Glyph glyphs[];\nextern const uint8_t font_pixels[],app_alpha[];\nextern const uint16_t wallpaper0[],wallpaper1[],app_icons[],portrait_pixels[];\n')
    with (OUT/'assets.c').open('w') as f:
        f.write('/* Generated native assets. Photo assets: Imagegen. Font: Inter/OFL. */\n#include "assets.h"\nconst Glyph glyphs[]={\n')
        for g in glyphs:f.write('{'+','.join(map(str,g))+'},\n')
        f.write('};\n');emit(f,'uint8_t','font_pixels',pixels)
        emit(f,'uint16_t','wallpaper0',rgb555(wall));emit(f,'uint16_t','wallpaper1',rgb555(alt));emit(f,'uint16_t','portrait_pixels',rgb555(portrait))
        icons=[];alphas=[]
        for n in range(1,17):
            im=icon(n);im.save(ROOT/f'artwork/app{n:02d}.png');icons+=rgb555(im);alphas+=(np.asarray(im)[:,:,3].flatten()>>4).tolist()
        emit(f,'uint16_t','app_icons',icons);emit(f,'uint8_t','app_alpha',alphas)
    launcher=Image.new('RGB',(32,32),'#007aff');d=ImageDraw.Draw(launcher)
    d.rounded_rectangle((4,6,15,26),3,fill='white');d.rounded_rectangle((17,6,28,26),3,fill='white')
    d.rounded_rectangle((6,8,13,23),1,fill='#b3d4ec');d.rounded_rectangle((19,8,26,23),1,fill='#edd3b1')
    launcher.quantize(colors=15).save(ROOT/'artwork/launcher.png')
    print('Generated',len(pixels),'font bytes, 16 icons and panorama photo assets.')
if __name__=='__main__':main()
