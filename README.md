# pfm — Parse FLAC Metadata

**pfm** (also called `flacdump`) is a simple command-line tool to read and display metadata from FLAC audio files, including stream info, Vorbis comments, seek tables, and embedded pictures.

## Usage

```sh
flacdump.exe /path/to/flacfile.flac
```

### Example (PowerShell)

```powershell
PS C:\Users\balin\cpp\pfm> .\flacdump.exe 'C:\Users\balin\Music\1983.05.13 (95059) UCB Greek Theater - Berkeley, CA\101 Cold Rain and Snow.flac'
```

### Output

```
Block offset: 4    Block Type: 0, Length: 34       STREAMINFO
Minimum Block Size: 4608
Maximum Block Size: 4608
Minimum Frame Size: 3397
Maximum Frame Size: 13994
Sample Rate: 44100
Channels: 2
Bits per Sample: 16
Total Samples: 21806568
Block offset: 42   Block Type: 3, Length: 882      SEEKTABLE
Block offset: 928  Block Type: 4, Length: 444      VORBIS_COMMENT
Vendor Length: 32
Vendor String: reference libFLAC 1.1.2 20050205
Number of comments: 8
album=1983-05-13 Berkeley, CA None (95059)
artist=Grateful Dead
albumartist=Grateful Dead
comment=flac16; Recording Info: SBD -> Master Cassette -> PCM -> Apogee Ensemble -> AIFF; Transfer Info: AIFF -> Adobe Audition v1.5 -> Samplitude Professional v10.1 -> FLAC; All Transfers and Mastering By Charlie Miller
genre=gd1983
title=Cold Rain And Snow
discnumber=1/3
tracknumber=102
Block offset: 1376     Block Type: 6, Length: 108350   PICTURE
Block offset: 109730   Block Type: 1, Length: 55504    PADDING
```

## Features

- Parses and displays:
  - STREAMINFO block
  - SEEKTABLE block
  - VORBIS_COMMENT block (including vendor and all comments)
  - PICTURE block (basic info)
  - PADDING and other blocks with type/length
- Works with standard FLAC files
- Lightweight and easy to integrate or extend

## Build

This tool is written in C++ and depends only on the standard library. To compile:

```sh
g++ flacdump.cpp streaminfo.cpp -o flacdump
```

Or using MSVC:

```sh
cl flacdump.cpp streaminfo.cpp /Fe:flacdump.exe
```

## License

MIT License (or insert your license here).
