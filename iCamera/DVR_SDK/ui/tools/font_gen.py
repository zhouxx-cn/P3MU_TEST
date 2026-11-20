#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
tools/font_gen.py — 拇指相机 UI 中文字库生成工具（LVGL 8.3 原生 fmt_txt 格式）

功能：
  从 SourceHanSansSC TTF 中按"实际用到的字符集"裁剪渲染，生成 4 个
  LVGL C 字体文件（ui_font_sc_12/14/16/20.c，4bpp 无压缩）。

字符集来源（自动收集，无需手工维护）：
  1. ui_common/ui_lang.c 所有语言表的全部字符（自动扫描）；
  2. ASCII 0x20~0x7E；
  3. extra.txt / 内置补充字符（箭头、常用标点等）。

  LVGL 内置 symbol（LV_SYMBOL_*，FontAwesome 私有区 0xF000~0xF8FF 与
  U+2022）不在本字库内，由 .fallback 回退到 lv_font_montserrat_* 渲染，
  因此 lv_conf.h 中 montserrat 12/14/16/20 必须保持开启。

用法：
  python3 tools/font_gen.py <SourceHanSansSC-Regular.ttf> [输出目录]
  （在 DVR_SDK/ui 下运行；默认输出 ui_font/）

依赖：python3 + Pillow（pip3 install Pillow）
"""
import os
import re
import sys

from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
UI_DIR = os.path.dirname(HERE)

SIZES = [12, 14, 16, 20]
FALLBACK = {12: "lv_font_montserrat_12", 14: "lv_font_montserrat_14",
            16: "lv_font_montserrat_16", 20: "lv_font_montserrat_20"}

# 补充字符：箭头（预览页提示）、全角标点、制表常用符号
EXTRA_CHARS = (
    "▲▼●○◆■□★☆"
    "，。、！？：；“”‘’（）《》—…·【】"
    "％＋－×÷＝°"
)


def collect_chars():
    """扫描 ui_lang.c 所有字符串字面量，收集全部 UTF-8 字符。"""
    src_path = os.path.join(UI_DIR, "ui_common", "ui_lang.c")
    with open(src_path, "r", encoding="utf-8") as f:
        text = f.read()
    chars = set()
    for m in re.finditer(r'"((?:[^"\\]|\\.)*)"', text):
        # C 转义仅 \n \t \\ \" 出现于表内，\n 不占字形
        s = m.group(1).replace("\\n", "").replace("\\t", "")
        s = s.replace('\\"', '"').replace("\\\\", "\\")
        chars.update(s)
    chars.update(EXTRA_CHARS)
    chars.update(chr(c) for c in range(0x20, 0x7F))
    # 注意：ASCII 必须保持 0x20..0x7E 完整连续（含 '"' 和 '\'），
    # 因为 FORMAT0_TINY cmap 按 gid = cp-0x20+1 线性映射，缺字会导致后续全部错位
    chars.discard("\n")
    chars.discard("\t")
    return sorted(chars)


def rasterize(font, ch, size):
    """渲染单个字符 → (bitmap bytes 4bpp, box_w, box_h, ofs_x, ofs_y, adv_w)
    ofs_y 以"行顶"为原点（LVGL 约定），行顶 = 基线 - ascent。"""
    adv_w = font.getlength(ch)
    ascent, descent = font.getmetrics()   # ascent: baseline 以上高度
    canvas_w, canvas_h = size * 3, size * 3
    img = Image.new("L", (canvas_w, canvas_h), 0)
    draw = ImageDraw.Draw(img)
    pen_x, baseline_y = size, ascent      # 基线放在 y=ascent，行顶 y=0
    draw.text((pen_x, baseline_y), ch, font=font, fill=255, anchor="ls")
    bbox = img.getbbox()
    if not bbox:
        return b"", 0, 0, 0, 0, int(round(adv_w))
    glyph = img.crop(bbox)
    box_w, box_h = glyph.size
    # 4bpp 打包：行像素连续、无行对齐，高半字节在前（与 lv_font_conv 一致）
    bits = []
    for px in glyph.getdata():
        v = int(round(px * 15 / 255.0))
        bits.append(v & 0xF)
    data = bytearray()
    for i in range(0, len(bits), 2):
        hi = bits[i]
        lo = bits[i + 1] if i + 1 < len(bits) else 0
        data.append((hi << 4) | lo)
    ofs_x = bbox[0] - pen_x
    # LVGL 约定：ofs_y = 字形底部到基线的距离（基线下方为负）。
    # 绘制公式 gpos.y = y + (line_height - base_line) - box_h - ofs_y
    ofs_y = ascent - bbox[3]              # bbox[3] = 字形底边 y（行顶原点），基线 y=ascent
    return bytes(data), box_w, box_h, ofs_x, ofs_y, int(round(adv_w))


def fmt_bytes(data, per_line=12):
    out = []
    for i in range(0, len(data), per_line):
        out.append("    " + ", ".join("0x%02x" % b for b in data[i:i + per_line]) + ",")
    return "\n".join(out)


def gen_font(font_path, size, chars, out_path):
    font = ImageFont.truetype(font_path, size)
    ascent, descent = font.getmetrics()
    line_height = ascent + descent

    # 字形区：glyph_id 从 1 开始（0 = 未找到）；0x20 空格也放实体空字形
    bitmaps = bytearray()
    dsc_rows = []
    cmap2 = []                            # 非 ASCII 的 (cp, gid)
    gid = 1
    for ch in chars:
        cp = ord(ch)
        data, bw, bh, ox, oy, adv = rasterize(font, ch, size)
        dsc_rows.append((cp, gid, len(bitmaps), adv, bw, bh, ox, oy, ch))
        bitmaps.extend(data)
        if not (0x20 <= cp <= 0x7E):
            cmap2.append((cp, gid))
        gid += 1

    # 防错位断言：FORMAT0_TINY 要求 ASCII 区间 gid = cp-0x20+1 严格成立
    for cp, g, *_ in dsc_rows:
        if 0x20 <= cp <= 0x7E:
            assert g == cp - 0x20 + 1, "ASCII gid mapping broken at U+%04X" % cp

    # glyph_dsc：下标必须 == glyph_id。gid 0 是 LVGL 约定的占位项（未找到字形时返回 0），
    # 真实字形从 gid 1 开始（0x20 空格为 gid 1），因此数组头部必须有一个 dummy 条目，
    # 否则所有字形整体错位 +1（表现为"每个字符显示成下一个字符"）。
    lines = ["    /* gid 0 占位：无位图、无尺寸 */"
             " { .bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0 },"]
    for cp, g, bidx, adv, bw, bh, ox, oy, ch in dsc_rows:
        cmt = ch if ch != "\\" and ch != '"' else ""
        lines.append("    /* U+%04X %s */ { .bitmap_index = %u, .adv_w = %u, "
                     ".box_w = %u, .box_h = %u, .ofs_x = %d, .ofs_y = %d },"
                     % (cp, cmt.encode("unicode_escape").decode() if not cmt else cmt,
                        bidx, adv * 16, bw, bh, ox, oy))
    dsc_txt = "\n".join(lines)

    # cmap2 排序；range = [min_cp, max_cp]，rcp 需 < 65536
    cmap2.sort()
    range_start = cmap2[0][0]
    range_end = cmap2[-1][0]
    assert range_end - range_start < 65536, "cmap range too large for uint16"
    unicode_list = ", ".join(str(cp - range_start) for cp, _ in cmap2)
    max_idx = len(bitmaps)

    var = "ui_font_sc_%d" % size
    with open(out_path, "w", encoding="utf-8") as f:
        f.write("""/**
 * @file    %(var)s.c
 * @brief   思源黑体（SourceHanSansSC）裁剪字库 %(size)dpx —— LVGL fmt_txt 4bpp。
 * @note    本文件由 tools/font_gen.py 自动生成，请勿手改；
 *          修改文案/语言后重新运行脚本再编译。
 *          LVGL symbol（LV_SYMBOL_*）经 .fallback 回退到 %(fb)s 渲染。
 */
#include "lvgl.h"
#include "ui_font.h"

#ifndef %(VAR)s
    #define %(VAR)s 1
#endif

#if %(VAR)s

/*-----------------
 *    BITMAPS
 *----------------*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
%(bitmap)s
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/
static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
%(dsc)s
    /* 最后一个哨兵：位图总长标记，防越界读取 */
    { .bitmap_index = %(maxidx)u, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0 }
};

/*--------------------
 *  CHARACTER MAPPING
 *-------------------*/
static const uint16_t unicode_list_0[] = {
    %(ulist)s
};

static const lv_font_fmt_txt_cmap_t cmaps[] = {
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0,
        .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = %(rstart)u, .range_length = %(rlen)u, .glyph_id_start = %(gid2start)u,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL,
        .list_length = %(n2)u, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/
static lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 2,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
    .cache = &cache
};

/*-----------------
 *  PUBLIC FONT
 *----------------*/
const lv_font_t %(var)s = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,
    .line_height = %(lh)d,
    .base_line = %(bl)d,
    .subpx = LV_FONT_SUBPX_NONE,
    .underline_position = -1,
    .underline_thickness = 1,
    .dsc = &font_dsc,
    .fallback = &%(fb)s,
};

#endif /*%(VAR)s*/
""" % {
            "var": var, "VAR": var.upper(), "size": size, "fb": FALLBACK[size],
            "bitmap": fmt_bytes(bitmaps),
            "dsc": dsc_txt,
            "maxidx": max_idx,
            "ulist": unicode_list,
            "rstart": range_start,
            "rlen": range_end - range_start,
            "gid2start": 1 + (0x7F - 0x20),   # ASCII 0x20..0x7E 占 gid 1..95
            "n2": len(cmap2),
            "lh": line_height,
            "bl": descent,
        })
    print("  [FONT] %-24s chars=%3d  bitmap=%6.1fKB  line_height=%d"
          % (os.path.basename(out_path), len(chars), len(bitmaps) / 1024.0, line_height))


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    font_path = sys.argv[1]
    out_dir = sys.argv[2] if len(sys.argv) > 2 else os.path.join(UI_DIR, "ui_font")
    os.makedirs(out_dir, exist_ok=True)

    chars = collect_chars()
    print("[font_gen] charset=%d chars (from ui_lang.c + extras + ASCII)" % len(chars))
    if not os.path.exists(font_path):
        print("[font_gen] ERROR: font not found: %s" % font_path)
        sys.exit(2)
    for size in SIZES:
        gen_font(font_path, size, chars, os.path.join(out_dir, "ui_font_sc_%d.c" % size))
    print("[font_gen] done -> %s" % out_dir)


if __name__ == "__main__":
    main()
